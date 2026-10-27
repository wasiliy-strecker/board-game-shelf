#pragma once

#include "application/game_repository.h"

#include <QLockFile>
#include <QSqlDatabase>
#include <memory>

namespace shelf {

class SqliteGameRepository final : public GameRepository {
public:
  explicit SqliteGameRepository(QString directory);
  ~SqliteGameRepository() override;
  SqliteGameRepository(const SqliteGameRepository&) = delete;
  SqliteGameRepository& operator=(const SqliteGameRepository&) = delete;
  void initialize() override;
  GameList findAll() override;
  BoardGame create(const GameDraft& draft) override;
  BoardGame update(qint64 id, const GameDraft& draft) override;
  void remove(qint64 id) override;

private:
  void close();
  QString directory_;
  QString connectionName_;
  QSqlDatabase database_;
  std::unique_ptr<QLockFile> lock_;
  bool initialized_ = false;
};

} // namespace shelf
