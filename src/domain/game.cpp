#include "domain/game.h"

#include <QCollator>
#include <QLocale>
#include <algorithm>

namespace shelf {

GameDraft validateDraft(GameDraft draft) {
  draft.name = draft.name.trimmed();
  draft.publisher = draft.publisher.trimmed();
  draft.location = draft.location.trimmed();
  if (draft.name.isEmpty()) {
    throw ValidationError(QStringLiteral("Bitte gib einen Spielnamen ein."));
  }
  return draft;
}

QString searchKey(const QString& text) {
  return text.normalized(QString::NormalizationForm_C)
      .toCaseFolded()
      .normalized(QString::NormalizationForm_C);
}

GameList searchGames(const GameList& games, const QString& query) {
  const auto needle = searchKey(query.trimmed());
  GameList matches;
  for (const auto& game : games) {
    const auto& f = game.fields;
    if (searchKey(f.name).contains(needle) || searchKey(f.publisher).contains(needle) ||
        searchKey(f.location).contains(needle) || searchKey(f.note).contains(needle)) {
      matches.append(game);
    }
  }
  QCollator collator{QLocale(QLocale::German, QLocale::Germany)};
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  collator.setNumericMode(true);
  std::sort(matches.begin(), matches.end(),
            [&collator](const BoardGame& left, const BoardGame& right) {
              const int comparison =
                  collator.compare(left.fields.name.normalized(QString::NormalizationForm_C),
                                   right.fields.name.normalized(QString::NormalizationForm_C));
              return comparison == 0 ? left.id < right.id : comparison < 0;
            });
  return matches;
}

} // namespace shelf
