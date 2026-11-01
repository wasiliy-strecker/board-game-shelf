#include "application/game_repository.h"
#include "domain/game.h"
#include "persistence/data_directory.h"

#include <QScopeGuard>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtTest>

using namespace shelf;

class DomainTest final : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() {
    QLocale::setDefault(QLocale(QStringLiteral("de-DE")));
    QStandardPaths::setTestModeEnabled(true);
    QCoreApplication::setApplicationName(QStringLiteral("brettspielregal-tests"));
  }
  void requiredName_data() {
    QTest::addColumn<QString>("name");
    QTest::newRow("empty") << QString();
    QTest::newRow("ascii") << QStringLiteral(" \t\r\n ");
    QTest::newRow("nbsp") << QString(QChar(0x00a0));
    QTest::newRow("figure-space") << QString(QChar(0x2007));
    QTest::newRow("narrow-nbsp") << QString(QChar(0x202f));
    QTest::newRow("ideographic") << QString(QChar(0x3000));
    QTest::newRow("mixed") << QStringLiteral("\u2002 \u2003\u0085\u2028\u2029");
  }
  void requiredName() {
    QFETCH(QString, name);
    QVERIFY_EXCEPTION_THROWN(validateDraft({name, {}, {}, {}}), ValidationError);
  }
  void trimsSingleLinesPreservesNote() {
    const QString note = QStringLiteral("  Erste Zeile\n\n\tGeschenk von Ömer.\n ");
    const auto draft =
        validateDraft({QStringLiteral("\u00a0 Azul \u3000"), QStringLiteral("  Verlag\u202f"),
                       QStringLiteral("\t Regal 2 "), note});
    QCOMPARE(draft.name, QStringLiteral("Azul"));
    QCOMPARE(draft.publisher, QStringLiteral("Verlag"));
    QCOMPARE(draft.location, QStringLiteral("Regal 2"));
    QCOMPARE(draft.note, note);
    const auto minimal = validateDraft({QStringLiteral("Spiel"), {}, {}, {}});
    QVERIFY(minimal.publisher.isEmpty());
    QVERIFY(minimal.location.isEmpty());
    QVERIFY(minimal.note.isEmpty());
  }
  void searchEveryField_data() {
    QTest::addColumn<QString>("query");
    QTest::newRow("name") << QStringLiteral("ÄRGER");
    QTest::newRow("publisher") << QStringLiteral("mÜllEr");
    QTest::newRow("location") << QStringLiteral("KÜCHE");
    QTest::newRow("note") << QStringLiteral("ÖMER");
    QTest::newRow("decomposed-query") << QStringLiteral("O\u0308mer");
  }
  void searchEveryField() {
    QFETCH(QString, query);
    const GameList games{{42,
                          {QStringLiteral("Ärger"), QStringLiteral("Müller"),
                           QStringLiteral("Küche"), QStringLiteral("Geschenk von Ömer.")}}};
    QCOMPARE(searchGames(games, query).size(), 1);
    QCOMPARE(searchGames(games, query).first().id, 42);
  }
  void normalizationInStoredText() {
    const GameList games{
        {7, {QStringLiteral("A\u0308rger"), {}, {}, QStringLiteral("O\u0308mer")}}};
    QCOMPARE(searchGames(games, QStringLiteral("ÄRGER")).size(), 1);
    QCOMPARE(searchGames(games, QStringLiteral("ömer")).size(), 1);
  }
  void emptyMissingAndStableOrder() {
    const GameList games{{9, {QStringLiteral("Spiel 10"), {}, {}, {}}},
                         {8, {QStringLiteral("Azul"), {}, {}, {}}},
                         {4, {QStringLiteral("azul"), {}, {}, {}}},
                         {5, {QStringLiteral("Spiel 2"), {}, {}, {}}}};
    const auto result = searchGames(games, QStringLiteral("\u00a0 "));
    QCOMPARE(result.size(), 4);
    QCOMPARE(result.at(0).id, 4);
    QCOMPARE(result.at(1).id, 8);
    QCOMPARE(result.at(2).id, 5);
    QCOMPARE(result.at(3).id, 9);
    QVERIFY(searchGames(games, QStringLiteral("nicht vorhanden")).isEmpty());
    QVERIFY(searchGames({}, {}).isEmpty());
  }
  void isolatedDataDirectory() {
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    QCOMPARE(resolveDataDirectory(directory.path(), true), directory.path());
    QVERIFY_EXCEPTION_THROWN(resolveDataDirectory({}, true), StorageError);
    QVERIFY_EXCEPTION_THROWN(resolveDataDirectory(QStringLiteral("relative"), true), StorageError);
    QVERIFY(!defaultDataDirectory().isEmpty());
    QVERIFY(defaultDataDirectory().contains(QStringLiteral("brettspielregal-tests")));
  }
  void sortEveryField_data() {
    QTest::addColumn<QString>("locale");
    QTest::addColumn<int>("field");
    QTest::addColumn<int>("order");
    for (const auto* locale : {"de-DE", "en-US", "es-ES", "fr-FR", "it-IT", "pt-BR"}) {
      for (int field = 0; field < 3; ++field) {
        for (int order = 0; order < 2; ++order) {
          const auto name = QStringLiteral("%1-%2-%3").arg(locale).arg(field).arg(order);
          QTest::newRow(qPrintable(name)) << QString::fromLatin1(locale) << field << order;
        }
      }
    }
  }
  void sortEveryField() {
    QFETCH(QString, locale);
    QFETCH(int, field);
    QFETCH(int, order);
    const QLocale original;
    const auto restore = qScopeGuard([original] { QLocale::setDefault(original); });
    QLocale::setDefault(QLocale(locale));
    const QStringList values{QStringLiteral("zebra"), QStringLiteral("Étoile"),
                             QStringLiteral("e\u0301toile"), QStringLiteral("Spiel 10"),
                             QStringLiteral("Spiel 2")};
    GameList games;
    for (qsizetype index = 0; index < values.size(); ++index) {
      GameDraft draft{QString::number(index), QStringLiteral("same"), QStringLiteral("same"),
                      index < 2 ? QStringLiteral("match") : QString()};
      if (field == 0) {
        draft.name = values.at(index);
      } else if (field == 1) {
        draft.publisher = values.at(index);
      } else {
        draft.location = values.at(index);
      }
      games.append({index + 1, draft});
    }
    // Empty optional values and equal names must keep independent, stable identities.
    if (field != 0) {
      games.append({6, {QStringLiteral("Empty fields"), {}, {}, {}}});
    }
    const auto sortField = static_cast<GameSortField>(field);
    const auto direction = static_cast<Qt::SortOrder>(order);
    const auto sorted = searchGames(games, {}, sortField, direction);
    QList<qint64> expected =
        order == 0 ? QList<qint64>{2, 3, 5, 4, 1} : QList<qint64>{1, 4, 5, 2, 3};
    if (field != 0) {
      if (order == 0) {
        expected.prepend(6);
      } else {
        expected.append(6);
      }
    }
    QList<qint64> actual;
    for (const auto& game : sorted) {
      actual.append(game.id);
    }
    QCOMPARE(actual, expected);
    const auto filtered = searchGames(games, QStringLiteral("match"), sortField, direction);
    QCOMPARE(filtered.size(), 2);
    QCOMPARE(filtered[0].id, order == 0 ? 2 : 1);
    QCOMPARE(filtered[1].id, order == 0 ? 1 : 2);
  }
};

QTEST_GUILESS_MAIN(DomainTest)
#include "domain_test.moc"
