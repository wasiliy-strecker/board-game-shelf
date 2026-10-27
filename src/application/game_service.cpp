#include "application/game_service.h"

namespace shelf {

GameList GameService::load() {
  repository_.initialize();
  return repository_.findAll();
}

BoardGame GameService::save(qint64 id, const GameDraft& draft) {
  const auto validated = validateDraft(draft);
  return id == 0 ? repository_.create(validated) : repository_.update(id, validated);
}

void GameService::remove(qint64 id) { repository_.remove(id); }

} // namespace shelf
