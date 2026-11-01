#pragma once

#include "application/collection_file.h"
#include "application/game_service.h"

#include <QObject>
#include <QThread>
#include <functional>
#include <memory>
#include <optional>

namespace shelf {

enum class Operation : quint8 {
  Load,
  Save,
  Remove,
  Language,
  InspectExport,
  Export,
  ReadImport,
  Import
};
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
  void inspectExport(const QString& path, const QString& directory);
  void exportCollection(const shelf::ExportTarget& target, bool replacementApproved,
                        const QString& directory);
  void readImport(const QString& path);
  void importCollection();
  void discardImport();

signals:
  void loaded(const shelf::GameList& games);
  void saved(const shelf::BoardGame& game);
  void removed(qint64 id);
  void languageSaved(const QString& preference);
  void exportInspected(const shelf::ExportTarget& target);
  void exported(qsizetype count);
  void importReady(qsizetype count);
  void imported(const shelf::ImportResult& result);
  void failed(shelf::Operation operation, const QString& message);

private:
  RepositoryFactory factory_;
  std::unique_ptr<GameRepository> repository_;
  std::optional<GameDraftList> pendingImport_;
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
  bool inspectExport(const QString& path, const QString& directory);
  bool exportCollection(const ExportTarget& target, bool replacementApproved,
                        const QString& directory);
  bool readImport(const QString& path);
  bool importCollection();
  void discardImport();

signals:
  void loaded(const shelf::GameList& games);
  void saved(const shelf::BoardGame& game);
  void removed(qint64 id);
  void languageSaved(const QString& preference);
  void exportInspected(const shelf::ExportTarget& target);
  void exported(qsizetype count);
  void importReady(qsizetype count);
  void imported(const shelf::ImportResult& result);
  void failed(shelf::Operation operation, const QString& message);
  void busyChanged(bool busy);
  void loadRequested();
  void saveRequested(qint64 id, const shelf::GameDraft& draft);
  void removeRequested(qint64 id);
  void languageRequested(const QString& directory, const QString& preference);
  void inspectExportRequested(const QString& path, const QString& directory);
  void exportRequested(const shelf::ExportTarget& target, bool replacementApproved,
                       const QString& directory);
  void readImportRequested(const QString& path);
  void importRequested();
  void discardImportRequested();

private:
  bool begin();
  void finish();
  QThread thread_;
  bool busy_ = false;
};

} // namespace shelf
Q_DECLARE_METATYPE(shelf::Operation)
