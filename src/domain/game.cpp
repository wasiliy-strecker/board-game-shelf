#include "domain/game.h"
#include <QCoreApplication>

#include <QCollator>
#include <QLocale>
#include <algorithm>

namespace shelf {

GameDraft validateDraft(GameDraft draft) {
  draft.name = draft.name.trimmed();
  draft.publisher = draft.publisher.trimmed();
  draft.location = draft.location.trimmed();
  if (draft.name.isEmpty()) {
    throw ValidationError(QCoreApplication::translate("App", "Bitte gib einen Spielnamen ein."));
  }
  return draft;
}

QString searchKey(const QString& text) {
  return text.normalized(QString::NormalizationForm_C)
      .toCaseFolded()
      .normalized(QString::NormalizationForm_C);
}

GameList searchGames(const GameList& games, const QString& query, GameSortField field,
                     Qt::SortOrder order) {
  const auto needle = searchKey(query.trimmed());
  GameList matches;
  for (const auto& game : games) {
    const auto& f = game.fields;
    if (searchKey(f.name).contains(needle) || searchKey(f.publisher).contains(needle) ||
        searchKey(f.location).contains(needle) || searchKey(f.note).contains(needle)) {
      matches.append(game);
    }
  }
  QCollator collator{QLocale()};
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  collator.setNumericMode(true);
  const auto value = [field](const BoardGame& game) -> const QString& {
    switch (field) {
    case GameSortField::Publisher:
      return game.fields.publisher;
    case GameSortField::Location:
      return game.fields.location;
    case GameSortField::Name:
      return game.fields.name;
    }
    return game.fields.name;
  };
  std::sort(matches.begin(), matches.end(),
            [&collator, &value, order](const BoardGame& left, const BoardGame& right) {
              const int comparison =
                  collator.compare(value(left).normalized(QString::NormalizationForm_C),
                                   value(right).normalized(QString::NormalizationForm_C));
              if (comparison == 0) {
                return left.id < right.id;
              }
              return order == Qt::AscendingOrder ? comparison < 0 : comparison > 0;
            });
  return matches;
}

} // namespace shelf
