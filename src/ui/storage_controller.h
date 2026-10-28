#pragma once

#include "application/game_service.h"

#include <QObject>
#include <QThread>
#include <functional>
#include <memory>

namespace shelf {

enum class Operation : quint8 { Load, Save, Remove, Language };
using RepositoryFactory = std::function<std::unique_ptr<GameRepository>()>;

class StorageWorker final : public QObject {
  Q_OBJECT
public:
  explicit StorageWorker(RepositoryFactory factory);

public slots:
  void load();
  void save(qint64 id, const shelf::GameDraft& draft);
  void remove(qint64 id);
  void saveLanguage(const QString& directory, const QString& preference);

signals:
  void loaded(const shelf::GameList& games);
  void saved(const shelf::BoardGame& game);
  void removed(qint64 id);
  void languageSaved(const QString& preference);
  void failed(shelf::Operation operation, const QString& message);

private:
  RepositoryFactory factory_;
  std::unique_ptr<GameRepository> repository_;
};

class StorageController final : public QObject {
  Q_OBJECT
public:
  explicit StorageController(RepositoryFactory factory, QObject* parent = nullptr);
  ~StorageController() override;
  bool isBusy() const { return busy_; }
  bool load();
  bool save(qint64 id, const GameDraft& draft);
  bool remove(qint64 id);
  bool saveLanguage(const QString& directory, const QString& preference);

signals:
  void loaded(const shelf::GameList& games);
  void saved(const shelf::BoardGame& game);
  void removed(qint64 id);
  void languageSaved(const QString& preference);
  void failed(shelf::Operation operation, const QString& message);
  void busyChanged(bool busy);
  void loadRequested();
  void saveRequested(qint64 id, const shelf::GameDraft& draft);
  void removeRequested(qint64 id);
  void languageRequested(const QString& directory, const QString& preference);

private:
  bool begin();
  void finish();
  QThread thread_;
  bool busy_ = false;
};

} // namespace shelf
Q_DECLARE_METATYPE(shelf::Operation)
