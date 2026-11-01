#pragma once

#include "domain/game.h"

namespace shelf {

struct ImportResult {
  qsizetype added = 0;
  qsizetype skipped = 0;
  GameList games;
};

class StorageError final : public std::runtime_error {
public:
  explicit StorageError(const QString& message) : std::runtime_error(message.toStdString()) {}
};

// Implementations belong to one thread. Every successful mutation is committed.
class GameRepository {
public:
  virtual ~GameRepository() = default;
  virtual void initialize() = 0;
  virtual GameList findAll() = 0;
  virtual BoardGame create(const GameDraft& draft) = 0;
  virtual BoardGame update(qint64 id, const GameDraft& draft) = 0;
  virtual void remove(qint64 id) = 0;
  virtual ImportResult importGames(const GameDraftList& drafts) = 0;
};

} // namespace shelf

Q_DECLARE_METATYPE(shelf::ImportResult)
