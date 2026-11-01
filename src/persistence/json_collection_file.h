#pragma once

#include "application/collection_file.h"
#include <functional>
#include <system_error>

namespace shelf {

class JsonCollectionFile final : public CollectionFile {
public:
  using LinkAttempt = std::function<std::error_code(const QString&, const QString&)>;
  explicit JsonCollectionFile(QString dataDirectory = {}, LinkAttempt link = {});
  ExportTarget inspect(const QString& path) override;
  GameDraftList read(const QString& path) override;
  void write(const GameDraftList& games, const ExportTarget& target,
             bool replacementApproved) override;

private:
  QString dataDirectory_;
  LinkAttempt link_;
};

} // namespace shelf
