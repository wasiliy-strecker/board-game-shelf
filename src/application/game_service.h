#pragma once

#include "application/game_repository.h"

namespace shelf {

class GameService final {
public:
  explicit GameService(GameRepository& repository) : repository_(repository) {}
  GameList load();
  BoardGame save(qint64 id, const GameDraft& draft);
  void remove(qint64 id);

private:
  GameRepository& repository_;
};

} // namespace shelf
