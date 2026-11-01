#include "application/collection_transfer.h"

namespace shelf {

qsizetype CollectionTransfer::exportGames(const ExportTarget& target, bool replacementApproved) {
  // Export the saved snapshot, independently of the UI's search, selection and sorting.
  const auto saved = repository_.findAll();
  GameDraftList drafts;
  drafts.reserve(saved.size());
  for (const auto& game : saved) {
    drafts.append(game.fields);
  }
  files_.write(drafts, target, replacementApproved);
  return drafts.size();
}

} // namespace shelf
