#include "ui/storage_controller.h"

#include <utility>

namespace shelf {

StorageWorker::StorageWorker(RepositoryFactory factory) : factory_(std::move(factory)) {}

void StorageWorker::load() {
  try {
    if (!repository_) {
      repository_ = factory_();
    }
    GameService service(*repository_);
    emit loaded(service.load());
  } catch (const std::exception& error) {
    emit failed(Operation::Load, QString::fromUtf8(error.what()));
  }
}

void StorageWorker::save(qint64 id, const GameDraft& draft) {
  try {
    if (!repository_) {
      throw StorageError(QStringLiteral("Die Sammlung ist noch nicht geladen."));
    }
    GameService service(*repository_);
    emit saved(service.save(id, draft));
  } catch (const std::exception& error) {
    emit failed(Operation::Save, QString::fromUtf8(error.what()));
  }
}

void StorageWorker::remove(qint64 id) {
  try {
    if (!repository_) {
      throw StorageError(QStringLiteral("Die Sammlung ist noch nicht geladen."));
    }
    GameService service(*repository_);
    service.remove(id);
    emit removed(id);
  } catch (const std::exception& error) {
    emit failed(Operation::Remove, QString::fromUtf8(error.what()));
  }
}

StorageController::StorageController(RepositoryFactory factory, QObject* parent) : QObject(parent) {
  qRegisterMetaType<GameDraft>();
  qRegisterMetaType<BoardGame>();
  qRegisterMetaType<GameList>();
  qRegisterMetaType<Operation>();
  auto* worker = new StorageWorker(std::move(factory));
  worker->moveToThread(&thread_);
  connect(&thread_, &QThread::finished, worker, &QObject::deleteLater);
  connect(this, &StorageController::loadRequested, worker, &StorageWorker::load);
  connect(this, &StorageController::saveRequested, worker, &StorageWorker::save);
  connect(this, &StorageController::removeRequested, worker, &StorageWorker::remove);
  connect(worker, &StorageWorker::loaded, this, [this](const GameList& games) {
    finish();
    emit loaded(games);
  });
  connect(worker, &StorageWorker::saved, this, [this](const BoardGame& game) {
    finish();
    emit saved(game);
  });
  connect(worker, &StorageWorker::removed, this, [this](qint64 id) {
    finish();
    emit removed(id);
  });
  connect(worker, &StorageWorker::failed, this,
          [this](Operation operation, const QString& message) {
            finish();
            emit failed(operation, message);
          });
  thread_.start();
}

StorageController::~StorageController() {
  // MainWindow prevents normal closure during outstanding work. This also makes
  // exceptional teardown safe. All connection ownership stays in the worker.
  thread_.quit();
  thread_.wait();
}

bool StorageController::begin() {
  if (busy_) {
    return false;
  }
  busy_ = true;
  emit busyChanged(true);
  return true;
}

void StorageController::finish() {
  busy_ = false;
  emit busyChanged(false);
}

bool StorageController::load() {
  if (!begin()) {
    return false;
  }
  emit loadRequested();
  return true;
}

bool StorageController::save(qint64 id, const GameDraft& draft) {
  if (!begin()) {
    return false;
  }
  emit saveRequested(id, draft);
  return true;
}

bool StorageController::remove(qint64 id) {
  if (!begin()) {
    return false;
  }
  emit removeRequested(id);
  return true;
}

} // namespace shelf
