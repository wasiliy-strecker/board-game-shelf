#pragma once

#include "domain/game.h"
#include <QByteArray>

namespace shelf {

inline constexpr qsizetype maxTransferGames = 10000;
inline constexpr qint64 maxTransferBytes = qint64{10} * 1024 * 1024;

class TransferError final : public std::runtime_error {
public:
  explicit TransferError(const QString& message) : std::runtime_error(message.toStdString()) {}
};

struct ExportTarget {
  QString path;
  QByteArray directoryIdentity;
  bool exists = false;
  QByteArray identity;
  QByteArray revision;
  QByteArray digest;
  bool operator==(const ExportTarget&) const = default;
};

class CollectionFile {
public:
  virtual ~CollectionFile() = default;
  virtual ExportTarget inspect(const QString& path) = 0;
  virtual GameDraftList read(const QString& path) = 0;
  virtual void write(const GameDraftList& games, const ExportTarget& target,
                     bool replacementApproved) = 0;
};

} // namespace shelf

Q_DECLARE_METATYPE(shelf::ExportTarget)
