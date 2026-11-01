#include "ui/storage_controller.h"
#include "application/collection_transfer.h"
#include "persistence/json_collection_file.h"
#include "persistence/language_preferences.h"
#include <QCoreApplication>

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
      throw StorageError(
          QCoreApplication::translate("App", "Die Sammlung ist noch nicht geladen."));
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
      throw StorageError(
          QCoreApplication::translate("App", "Die Sammlung ist noch nicht geladen."));
    }
    GameService service(*repository_);
    service.remove(id);
    emit removed(id);
  } catch (const std::exception& error) {
    emit failed(Operation::Remove, QString::fromUtf8(error.what()));
  }
}

void StorageWorker::saveLanguage(const QString& directory, const QString& preference) {
  try {
    saveLanguagePreference(directory, preference);
    emit languageSaved(preference);
  } catch (const std::exception& error) {
    emit failed(Operation::Language, QString::fromUtf8(error.what()));
  }
}

// File path and app directory are named explicitly at the queued UI/worker boundary.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void StorageWorker::inspectExport(const QString& path, const QString& directory) {
  try {
    JsonCollectionFile files(directory);
    emit exportInspected(files.inspect(path));
  } catch (const std::exception& error) {
    emit failed(Operation::InspectExport, QString::fromUtf8(error.what()));
  }
}

void StorageWorker::exportCollection(const ExportTarget& target, bool replacementApproved,
                                     const QString& directory) {
  try {
    if (!repository_) {
      throw StorageError(
          QCoreApplication::translate("App", "Die Sammlung ist noch nicht geladen."));
    }
    JsonCollectionFile files(directory);
    CollectionTransfer transfer(*repository_, files);
    emit exported(transfer.exportGames(target, replacementApproved));
  } catch (const std::exception& error) {
    emit failed(Operation::Export, QString::fromUtf8(error.what()));
  }
}

void StorageWorker::readImport(const QString& path) {
  pendingImport_.reset();
  try {
    JsonCollectionFile files;
    pendingImport_ = files.read(path);
    emit importReady(pendingImport_->size());
  } catch (const std::exception& error) {
    emit failed(Operation::ReadImport, QString::fromUtf8(error.what()));
  }
}

void StorageWorker::importCollection() {
  try {
    if (!repository_ || !pendingImport_) {
      throw TransferError(
          QCoreApplication::translate("App", "Wähle zuerst eine gültige Importdatei."));
    }
    // Import the exact validated snapshot the user confirmed, never a later file revision.
    const auto result = repository_->importGames(*pendingImport_);
    pendingImport_.reset();
    emit imported(result);
  } catch (const std::exception& error) {
    pendingImport_.reset();
    emit failed(Operation::Import, QString::fromUtf8(error.what()));
  }
}

void StorageWorker::discardImport() { pendingImport_.reset(); }

StorageController::StorageController(RepositoryFactory factory, QObject* parent) : QObject(parent) {
  qRegisterMetaType<GameDraft>();
  qRegisterMetaType<BoardGame>();
  qRegisterMetaType<GameList>();
  qRegisterMetaType<Operation>();
  qRegisterMetaType<ExportTarget>();
  qRegisterMetaType<ImportResult>();
  auto* worker = new StorageWorker(std::move(factory));
  worker->moveToThread(&thread_);
  connect(&thread_, &QThread::finished, worker, &QObject::deleteLater);
  connect(this, &StorageController::loadRequested, worker, &StorageWorker::load);
  connect(this, &StorageController::saveRequested, worker, &StorageWorker::save);
  connect(this, &StorageController::removeRequested, worker, &StorageWorker::remove);
  connect(this, &StorageController::languageRequested, worker, &StorageWorker::saveLanguage);
  connect(this, &StorageController::inspectExportRequested, worker, &StorageWorker::inspectExport);
  connect(this, &StorageController::exportRequested, worker, &StorageWorker::exportCollection);
  connect(this, &StorageController::readImportRequested, worker, &StorageWorker::readImport);
  connect(this, &StorageController::importRequested, worker, &StorageWorker::importCollection);
  connect(this, &StorageController::discardImportRequested, worker, &StorageWorker::discardImport);
  connect(worker, &StorageWorker::exportInspected, this, [this](const ExportTarget& target) {
    finish();
    emit exportInspected(target);
  });
  connect(worker, &StorageWorker::exported, this, [this](qsizetype count) {
    finish();
    emit exported(count);
  });
  connect(worker, &StorageWorker::importReady, this, [this](qsizetype count) {
    finish();
    emit importReady(count);
  });
  connect(worker, &StorageWorker::imported, this, [this](const ImportResult& result) {
    finish();
    emit imported(result);
  });
  connect(worker, &StorageWorker::languageSaved, this, [this](const QString& preference) {
    finish();
    emit languageSaved(preference);
  });
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

bool StorageController::saveLanguage(const QString& directory, const QString& preference) {
  if (!begin()) {
    return false;
  }
  emit languageRequested(directory, preference);
  return true;
}

bool StorageController::inspectExport(const QString& path, const QString& directory) {
  if (!begin()) {
    return false;
  }
  emit inspectExportRequested(path, directory);
  return true;
}

bool StorageController::exportCollection(const ExportTarget& target, bool replacementApproved,
                                         const QString& directory) {
  if (!begin()) {
    return false;
  }
  emit exportRequested(target, replacementApproved, directory);
  return true;
}

bool StorageController::readImport(const QString& path) {
  if (!begin()) {
    return false;
  }
  emit readImportRequested(path);
  return true;
}

bool StorageController::importCollection() {
  if (!begin()) {
    return false;
  }
  emit importRequested();
  return true;
}

void StorageController::discardImport() { emit discardImportRequested(); }

} // namespace shelf
