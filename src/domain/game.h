#pragma once

#include <QMetaType>
#include <QString>
#include <QVector>
#include <stdexcept>

namespace shelf {

struct GameDraft {
  QString name;
  QString publisher;
  QString location;
  QString note;
  bool operator==(const GameDraft&) const = default;
};

struct BoardGame {
  qint64 id = 0;
  GameDraft fields;
  bool operator==(const BoardGame&) const = default;
};

using GameList = QVector<BoardGame>;
using GameDraftList = QVector<GameDraft>;

enum class GameSortField : quint8 { Name, Publisher, Location };

class ValidationError final : public std::runtime_error {
public:
  explicit ValidationError(const QString& message) : std::runtime_error(message.toStdString()) {}
};

GameDraft validateDraft(GameDraft draft);
QString searchKey(const QString& text);
GameList searchGames(const GameList& games, const QString& query,
                     GameSortField field = GameSortField::Name,
                     Qt::SortOrder order = Qt::AscendingOrder);

} // namespace shelf

Q_DECLARE_METATYPE(shelf::GameDraft)
Q_DECLARE_METATYPE(shelf::BoardGame)
Q_DECLARE_METATYPE(shelf::GameList)
Q_DECLARE_METATYPE(shelf::GameDraftList)
