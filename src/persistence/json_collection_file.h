#pragma once

#include "application/collection_file.h"

namespace shelf {

class JsonCollectionFile final : public CollectionFile {
public:
  explicit JsonCollectionFile(QString dataDirectory = {});
  ExportTarget inspect(const QString& path) override;
  GameDraftList read(const QString& path) override;
  void write(const GameDraftList& games, const ExportTarget& target,
             bool replacementApproved) override;

private:
  QString dataDirectory_;
};

} // namespace shelf
