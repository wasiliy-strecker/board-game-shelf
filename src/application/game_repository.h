#pragma once

#include "domain/game.h"

namespace shelf {

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
};

} // namespace shelf
