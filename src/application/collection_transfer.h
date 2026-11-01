#pragma once

#include "application/collection_file.h"
#include "application/game_repository.h"

namespace shelf {

class CollectionTransfer final {
public:
  CollectionTransfer(GameRepository& repository, CollectionFile& files)
      : repository_(repository), files_(files) {}
  qsizetype exportGames(const ExportTarget& target, bool replacementApproved);

private:
  GameRepository& repository_;
  CollectionFile& files_;
};

} // namespace shelf
