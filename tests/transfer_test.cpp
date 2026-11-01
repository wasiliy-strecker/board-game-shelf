#include "application/collection_transfer.h"
#include "persistence/json_collection_file.h"
#include "persistence/sqlite_game_repository.h"
#include "ui/storage_controller.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScopeGuard>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <QUuid>
#include <QtTest>
#include <filesystem>

using namespace shelf;

namespace {
const GameDraft sample{QStringLiteral("Étoile 東京 🎲"), QStringLiteral("Müller & Fils"),
                       QStringLiteral("Regal 2"), QStringLiteral("  Zeile 1\n\n\tNotiz \\\"\n")};

void put(const QString& path, const QByteArray& bytes) {
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size()) {
    throw std::runtime_error("Cannot write test fixture");
  }
}

QByteArray get(const QString& path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    return {};
  }
  return file.readAll();
}

QByteArray collection(const QByteArray& games) {
  return "{\"format\":\"board-game-shelf.collection\",\"version\":1,\"games\":" + games + '}';
}

QByteArray item() { return R"({"name":"Azul","publisher":"","location":"","note":""})"; }

void externalSql(const QString& directory, const QString& sql) {
  const auto name = QUuid::createUuid().toString();
  {
    auto database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), name);
    database.setDatabaseName(QDir(directory).filePath(QStringLiteral("brettspielregal.sqlite3")));
    if (!database.open() || !QSqlQuery(database).exec(sql)) {
      throw std::runtime_error("Cannot prepare test database");
    }
    database.close();
  }
  QSqlDatabase::removeDatabase(name);
}
} // namespace

class TransferTest final : public QObject {
  Q_OBJECT
private slots:
  void roundTripSavedCollectionAndEmptyFile() {
    QTemporaryDir data;
    QTemporaryDir exports;
    SqliteGameRepository repository(data.path());
    repository.initialize();
    repository.create(sample);
    repository.create(sample);
    const auto path = exports.filePath(QStringLiteral("Sammlung 東京.JSON"));
    JsonCollectionFile files(data.path());
    CollectionTransfer transfer(repository, files);
    QCOMPARE(transfer.exportGames(files.inspect(path), false), 2);
    QCOMPARE(files.read(path), (GameDraftList{sample, sample}));
    const auto json = QJsonDocument::fromJson(get(path)).object();
    QCOMPARE(json.value(QStringLiteral("format")).toString(),
             QStringLiteral("board-game-shelf.collection"));
    QVERIFY(!json.value(QStringLiteral("games"))
                 .toArray()[0]
                 .toObject()
                 .contains(QStringLiteral("id")));
    QVERIFY_EXCEPTION_THROWN(files.write({}, files.inspect(path), false), TransferError);
    QCOMPARE(files.read(path).size(), 2);
    files.write({}, files.inspect(path), true);
    QVERIFY(files.read(path).isEmpty());
    QCOMPARE(repository.findAll().size(), 2);
    QCOMPARE(QDir(exports.path()).entryList(QDir::Files | QDir::Hidden).size(), 1);
  }

  void invalidFiles_data() {
    QTest::addColumn<QByteArray>("bytes");
    QTest::newRow("empty") << QByteArray();
    QTest::newRow("broken") << QByteArray("{broken");
    QTest::newRow("trailing") << collection("[]") + "false";
    QTest::newRow("root-array") << QByteArray("[]");
    QTest::newRow("foreign-product")
        << QByteArray(R"({"format":"bookshelf.collection","version":1,"books":[]})");
    QTest::newRow("future-version")
        << QByteArray(R"({"format":"board-game-shelf.collection","version":2,"games":[]})");
    QTest::newRow("string-version")
        << QByteArray(R"({"format":"board-game-shelf.collection","version":"1","games":[]})");
    QTest::newRow("duplicate-root") << QByteArray(
        R"({"format":"wrong","format":"board-game-shelf.collection","version":1,"games":[]})");
    QTest::newRow("escaped-duplicate-key") << collection(
        R"([{"name":"One","na\u006de":"Two","publisher":"","location":"","note":""}])");
    QTest::newRow("missing-field")
        << collection(R"([{"name":"Azul","publisher":"","location":""}])");
    QTest::newRow("extra-field") << collection(
        R"([{"name":"Azul","publisher":"","location":"","note":"","other":0}])");
    QTest::newRow("non-object") << collection("[null]");
    QTest::newRow("null-note") << collection(
        R"([{"name":"Azul","publisher":"","location":"","note":null}])");
    QTest::newRow("wrong-array-type") << collection("{}");
    QTest::newRow("blank-name") << collection(
        R"([{"name":"\u00a0","publisher":"","location":"","note":""}])");
    QTest::newRow("unpaired-surrogate")
        << collection(R"([{"name":"Azul","publisher":"","location":"","note":"\ud800"}])");
    QTest::newRow("invalid-utf8") << collection(
        "[" + item().replace("Azul", QByteArray(1, '\xff')) + "]");
  }
  void invalidFiles() {
    QFETCH(QByteArray, bytes);
    QTemporaryDir directory;
    const auto path = directory.filePath(QStringLiteral("invalid.json"));
    put(path, bytes);
    JsonCollectionFile files;
    QVERIFY_EXCEPTION_THROWN(files.read(path), TransferError);
    QCOMPARE(get(path), bytes);
  }

  void byteAndEntryLimitsPreserveExistingExport() {
    QTemporaryDir data;
    QTemporaryDir output;
    JsonCollectionFile files(data.path());
    const auto path = output.filePath(QStringLiteral("backup.json"));
    files.write({sample}, files.inspect(path), false);
    const auto original = get(path);
    GameDraftList many(maxTransferGames, sample);
    const auto boundary = output.filePath(QStringLiteral("boundary.json"));
    files.write(many, files.inspect(boundary), false);
    QCOMPARE(files.read(boundary).size(), maxTransferGames);
    many.append(sample);
    QVERIFY_EXCEPTION_THROWN(files.write(many, files.inspect(path), true), TransferError);
    QCOMPARE(get(path), original);
    auto oversized = sample;
    oversized.note = QString(static_cast<int>(maxTransferBytes), QChar('x'));
    QVERIFY_EXCEPTION_THROWN(files.write({oversized}, files.inspect(path), true), TransferError);
    QCOMPARE(get(path), original);
    // JSON escaping also counts towards the byte limit.
    oversized.note = QString(2000000, QChar(1));
    QVERIFY_EXCEPTION_THROWN(files.write({oversized}, files.inspect(path), true), TransferError);
    QCOMPARE(get(path), original);
    const auto invalid = output.filePath(QStringLiteral("oversized.json"));
    put(invalid, QByteArray(maxTransferBytes + 1, ' '));
    QVERIFY_EXCEPTION_THROWN(files.read(invalid), TransferError);
    QByteArray entries("[");
    for (qsizetype index = 0; index <= maxTransferGames; ++index) {
      if (index != 0) {
        entries += ',';
      }
      entries += item();
    }
    entries += ']';
    put(invalid, collection(entries));
    QVERIFY_EXCEPTION_THROWN(files.read(invalid), TransferError);
  }

  void changedDestinationsAreNeverReplaced() {
    QTemporaryDir data;
    QTemporaryDir output;
    JsonCollectionFile files(data.path());
    const auto path = output.filePath(QStringLiteral("backup.json"));
    const auto absent = files.inspect(path);
    put(path, "created by another application");
    QVERIFY_EXCEPTION_THROWN(files.write({sample}, absent, false), TransferError);
    QCOMPARE(get(path), QByteArray("created by another application"));
    const auto original = files.inspect(path);
    put(path, "changed after selection");
    QVERIFY_EXCEPTION_THROWN(files.write({sample}, original, true), TransferError);
    QCOMPARE(get(path), QByteArray("changed after selection"));
    const auto replaced = files.inspect(path);
    QVERIFY(QFile::rename(path, path + QStringLiteral(".old")));
    put(path, "changed after selection");
    QVERIFY_EXCEPTION_THROWN(files.write({sample}, replaced, true), TransferError);
    QCOMPARE(get(path), QByteArray("changed after selection"));
  }

  void invalidTargetsAndLinksProtectAppData() {
    QTemporaryDir data;
    QTemporaryDir output;
    JsonCollectionFile files(data.path());
    QVERIFY_EXCEPTION_THROWN(files.inspect(QStringLiteral("relative.json")), TransferError);
    QVERIFY_EXCEPTION_THROWN(files.inspect(output.filePath(QStringLiteral("backup.txt"))),
                             TransferError);
    QVERIFY_EXCEPTION_THROWN(files.inspect(data.filePath(QStringLiteral("backup.json"))),
                             TransferError);
    const auto folder = output.filePath(QStringLiteral("folder.json"));
    QVERIFY(QDir().mkdir(folder));
    QVERIFY_EXCEPTION_THROWN(files.inspect(folder), TransferError);
    const auto protectedPath = data.filePath(QStringLiteral("language.txt"));
    put(protectedPath, "en-US\n");
#ifndef Q_OS_WIN
    const auto symbolic = output.filePath(QStringLiteral("symbolic.json"));
    QVERIFY(QFile::link(protectedPath, symbolic));
    QVERIFY_EXCEPTION_THROWN(files.inspect(symbolic), TransferError);
    const auto hard = output.filePath(QStringLiteral("hard.json"));
    std::error_code error;
    std::filesystem::create_hard_link(protectedPath.toStdString(), hard.toStdString(), error);
    QVERIFY2(!error, error.message().c_str());
    QVERIFY_EXCEPTION_THROWN(files.inspect(hard), TransferError);
#endif
    QCOMPARE(get(protectedPath), QByteArray("en-US\n"));
    QVERIFY_EXCEPTION_THROWN(files.read(output.filePath(QStringLiteral("missing.json"))),
                             TransferError);
    QVERIFY_EXCEPTION_THROWN(files.read(folder), TransferError);
  }

  void importPreservesMultiplicityAndExistingIdentities() {
    QTemporaryDir directory;
    GameList expected;
    {
      SqliteGameRepository repository(directory.path());
      repository.initialize();
      const auto existing = repository.create(sample);
      auto different = sample;
      different.note += QStringLiteral("Different copy");
      const auto result = repository.importGames({sample, sample, different});
      QCOMPARE(result.added, 2);
      QCOMPARE(result.skipped, 1);
      QCOMPARE(result.games.size(), 3);
      QCOMPARE(result.games.first(), existing);
      QVERIFY(result.games[1].id != existing.id);
      QCOMPARE(result.games[1].fields, sample);
      const auto repeated = repository.importGames({sample, sample, different});
      QCOMPARE(repeated.added, 0);
      QCOMPARE(repeated.skipped, 3);
      QCOMPARE(repeated.games, result.games);
      expected = result.games;
      const auto empty = repository.importGames({});
      QCOMPARE(empty.games, expected);
      QCOMPARE(empty.added, 0);
    }
    SqliteGameRepository reopened(directory.path());
    reopened.initialize();
    QCOMPARE(reopened.findAll(), expected);
  }

  void lateSqlFailureRollsBackEntireImportAndAllowsRetry() {
    QTemporaryDir directory;
    SqliteGameRepository repository(directory.path());
    repository.initialize();
    const auto original = repository.create(sample);
    externalSql(
        directory.path(),
        QStringLiteral("CREATE TRIGGER fail_import BEFORE INSERT ON games WHEN NEW.name='Reject' "
                       "BEGIN SELECT RAISE(ABORT, 'Synthetic late failure'); END"));
    const GameDraftList incoming{{QStringLiteral("Inserted first"), {}, {}, {}},
                                 {QStringLiteral("Reject"), {}, {}, {}}};
    QVERIFY_EXCEPTION_THROWN(repository.importGames(incoming), StorageError);
    QCOMPARE(repository.findAll(), GameList{original});
    externalSql(directory.path(), QStringLiteral("DROP TRIGGER fail_import"));
    QCOMPARE(repository.importGames(incoming).added, 2);
    QCOMPARE(repository.findAll().size(), 3);
    QVERIFY_EXCEPTION_THROWN(repository.importGames({sample, {{}, {}, {}, {}}}), ValidationError);
    QCOMPARE(repository.findAll().size(), 3);
  }

  void collectionLimitRollsBackAndKeepsExistingGames() {
    QTemporaryDir directory;
    SqliteGameRepository repository(directory.path());
    repository.initialize();
    QCOMPARE(repository.importGames(GameDraftList(maxTransferGames - 1, sample)).added,
             maxTransferGames - 1);
    const auto before = repository.findAll();
    QVERIFY_EXCEPTION_THROWN(repository.importGames({{QStringLiteral("One"), {}, {}, {}},
                                                     {QStringLiteral("Two"), {}, {}, {}}}),
                             TransferError);
    QCOMPARE(repository.findAll(), before);
  }

  void workerImportsOnlyTheConfirmedSnapshotAndDiscardsCancelledInput() {
    QTemporaryDir data;
    QTemporaryDir output;
    JsonCollectionFile files(data.path());
    const auto path = output.filePath(QStringLiteral("import.json"));
    files.write({sample}, files.inspect(path), false);
    StorageController controller(
        [&data] { return std::make_unique<SqliteGameRepository>(data.path()); });
    QSignalSpy loaded(&controller, &StorageController::loaded);
    QSignalSpy ready(&controller, &StorageController::importReady);
    QSignalSpy imported(&controller, &StorageController::imported);
    QSignalSpy failed(&controller, &StorageController::failed);
    QVERIFY(controller.load());
    QVERIFY(QTest::qWaitFor([&] { return loaded.size() == 1; }));
    QVERIFY(controller.readImport(path));
    QVERIFY(!controller.importCollection());
    QVERIFY(QTest::qWaitFor([&] { return ready.size() == 1; }));
    put(path, "file changed after preview");
    QVERIFY(controller.importCollection());
    QVERIFY(QTest::qWaitFor([&] { return imported.size() == 1; }));
    QCOMPARE(qvariant_cast<ImportResult>(imported.first().first()).games.first().fields, sample);
    files.write({sample}, files.inspect(path), true);
    QVERIFY(controller.readImport(path));
    QVERIFY(QTest::qWaitFor([&] { return ready.size() == 2; }));
    controller.discardImport();
    QVERIFY(controller.importCollection());
    QVERIFY(QTest::qWaitFor([&] { return failed.size() == 1; }));
    QCOMPARE(imported.size(), 1);
    QVERIFY(controller.load());
    QVERIFY(QTest::qWaitFor([&] { return loaded.size() == 2; }));
    QCOMPARE(qvariant_cast<GameList>(loaded.last().first()).size(), 1);
  }
};

QTEST_GUILESS_MAIN(TransferTest)
#include "transfer_test.moc"
