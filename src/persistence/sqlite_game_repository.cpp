#include "persistence/sqlite_game_repository.h"

#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>
#include <utility>

namespace shelf {
namespace {

[[noreturn]] void fail(const QSqlError& error) {
  // SQLite extended result codes keep the primary result in the low byte.
  const int code = error.nativeErrorCode().toInt() & 0xff;
  if (code == 5 || code == 6) {
    throw StorageError(QStringLiteral("Die Datenbank ist gerade gesperrt. Beende andere Zugriffe "
                                      "und versuche es erneut. Deine Eingaben bleiben erhalten."));
  }
  if (code == 11 || code == 26) {
    throw StorageError(
        QStringLiteral("Die Datenbank ist beschädigt oder keine gültige SQLite-Datei. "
                       "Sie wird nicht ersetzt. Sichere die Datei vor einer Reparatur."));
  }
  if (code == 3 || code == 8 || code == 14) {
    throw StorageError(QStringLiteral("Die Datenbank kann nicht geöffnet oder beschrieben werden. "
                                      "Prüfe das Datenverzeichnis und deine Zugriffsrechte."));
  }
  if (code == 13) {
    throw StorageError(QStringLiteral("Es ist nicht genügend Speicherplatz verfügbar. "
                                      "Schaffe Platz und versuche es erneut."));
  }
  throw StorageError(
      QStringLiteral("Der Datenbankzugriff ist fehlgeschlagen. "
                     "Prüfe Datenträger und Zugriffsrechte und versuche es erneut.\n%1")
          .arg(error.text()));
}

void execute(QSqlQuery& query, const QString& sql) {
  if (!query.exec(sql)) {
    fail(query.lastError());
  }
}

void execute(QSqlQuery& query) {
  if (!query.exec()) {
    fail(query.lastError());
  }
}

void prepare(QSqlQuery& query, const QString& sql) {
  if (!query.prepare(sql)) {
    fail(query.lastError());
  }
}

class Transaction final {
public:
  explicit Transaction(QSqlDatabase& database) : database_(database) {
    if (!database_.transaction()) {
      fail(database_.lastError());
    }
  }
  ~Transaction() {
    if (!committed_) {
      database_.rollback();
    }
  }
  Transaction(const Transaction&) = delete;
  Transaction& operator=(const Transaction&) = delete;
  void commit() {
    if (!database_.commit()) {
      fail(database_.lastError());
    }
    committed_ = true;
  }

private:
  QSqlDatabase& database_;
  bool committed_ = false;
};

void bindDraft(QSqlQuery& query, const GameDraft& draft) {
  // Empty optional QString values must be TEXT, never SQL NULL.
  query.bindValue(QStringLiteral(":name"), draft.name.isNull() ? QStringLiteral("") : draft.name);
  query.bindValue(QStringLiteral(":publisher"),
                  draft.publisher.isNull() ? QStringLiteral("") : draft.publisher);
  query.bindValue(QStringLiteral(":location"),
                  draft.location.isNull() ? QStringLiteral("") : draft.location);
  query.bindValue(QStringLiteral(":note"), draft.note.isNull() ? QStringLiteral("") : draft.note);
}

void requireAffectedRow(const QSqlQuery& query) {
  if (query.numRowsAffected() != 1) {
    throw StorageError(
        QStringLiteral("Dieses Spiel ist nicht mehr vorhanden. "
                       "Lade die Sammlung erneut. Deine Eingaben bleiben erhalten."));
  }
}

} // namespace

SqliteGameRepository::SqliteGameRepository(QString directory) : directory_(std::move(directory)) {}

SqliteGameRepository::~SqliteGameRepository() { close(); }

void SqliteGameRepository::close() {
  initialized_ = false;
  if (database_.isValid()) {
    database_.close();
    database_ = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName_);
  }
  lock_.reset();
}

void SqliteGameRepository::initialize() {
  if (initialized_) {
    return;
  }
  close();
  if (!QDir::isAbsolutePath(directory_) || !QDir().mkpath(directory_)) {
    throw StorageError(QStringLiteral("Das Datenverzeichnis kann nicht angelegt werden. "
                                      "Prüfe Pfad, Datenträger und Zugriffsrechte."));
  }
  directory_ = QFileInfo(directory_).canonicalFilePath();
  lock_ = std::make_unique<QLockFile>(QDir(directory_).filePath(QStringLiteral("collection.lock")));
  lock_->setStaleLockTime(0);
  if (!lock_->tryLock()) {
    const bool otherInstance = lock_->error() == QLockFile::LockFailedError;
    lock_.reset();
    throw StorageError(
        otherInstance
            ? QStringLiteral("Diese Sammlung ist bereits in einer anderen App-Instanz geöffnet. "
                             "Schließe sie dort und lade hier erneut.")
            : QStringLiteral(
                  "Das Datenverzeichnis ist nicht beschreibbar. Prüfe deine Zugriffsrechte."));
  }
  try {
    const QString path = QDir(directory_).filePath(QStringLiteral("brettspielregal.sqlite3"));
    const bool isNew = !QFileInfo::exists(path);
    connectionName_ = QUuid::createUuid().toString(QUuid::WithoutBraces);
    database_ = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName_);
    database_.setDatabaseName(path);
    database_.setConnectOptions(QStringLiteral("QSQLITE_BUSY_TIMEOUT=2000"));
    if (!database_.open()) {
      fail(database_.lastError());
    }
    QSqlQuery query(database_);
    execute(query, QStringLiteral("PRAGMA user_version"));
    if (!query.next()) {
      throw StorageError(QStringLiteral("Die Schema-Version konnte nicht gelesen werden."));
    }
    const int version = query.value(0).toInt();
    query.finish();
    if (version > 1) {
      throw StorageError(
          QStringLiteral("Diese Datenbank benötigt eine neuere Version von BrettspielRegal. "
                         "Die vorhandene Datei wird nicht verändert."));
    }
    if (isNew) {
      Transaction transaction(database_);
      execute(query,
              QStringLiteral("CREATE TABLE games (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "name TEXT NOT NULL, publisher TEXT NOT NULL DEFAULT '', "
                             "location TEXT NOT NULL DEFAULT '', note TEXT NOT NULL DEFAULT '')"));
      execute(query, QStringLiteral("PRAGMA user_version=1"));
      query.finish();
      transaction.commit();
    } else if (version != 1) {
      throw StorageError(
          QStringLiteral("Die vorhandene Datei hat kein bekanntes BrettspielRegal-Schema. "
                         "Sie wird nicht überschrieben."));
    }
    execute(query, QStringLiteral("PRAGMA quick_check"));
    if (!query.next() || query.value(0).toString() != QStringLiteral("ok")) {
      throw StorageError(QStringLiteral("Die Datenbankprüfung hat einen Schaden gefunden. "
                                        "Sichere die Datei. Sie wird nicht ersetzt."));
    }
    query.finish();
    execute(query, QStringLiteral("PRAGMA table_info(games)"));
    QStringList columns;
    bool validTypes = true;
    while (query.next()) {
      columns.append(query.value(1).toString());
      const bool isId = columns.size() == 1;
      validTypes = validTypes &&
                   query.value(2).toString().toUpper() ==
                       (isId ? QStringLiteral("INTEGER") : QStringLiteral("TEXT")) &&
                   query.value(5).toInt() == (isId ? 1 : 0) && (isId || query.value(3).toBool());
    }
    if (!validTypes || columns != QStringList{QStringLiteral("id"), QStringLiteral("name"),
                                              QStringLiteral("publisher"),
                                              QStringLiteral("location"), QStringLiteral("note")}) {
      throw StorageError(QStringLiteral("Die Datenbankstruktur ist unbekannt oder beschädigt. "
                                        "Die Datei wird nicht ersetzt."));
    }
    initialized_ = true;
  } catch (...) {
    close();
    throw;
  }
}

GameList SqliteGameRepository::findAll() {
  QSqlQuery query(database_);
  execute(query, QStringLiteral("SELECT id,name,publisher,location,note FROM games ORDER BY id"));
  GameList games;
  while (query.next()) {
    games.append({query.value(0).toLongLong(),
                  {query.value(1).toString(), query.value(2).toString(), query.value(3).toString(),
                   query.value(4).toString()}});
  }
  if (query.lastError().isValid()) {
    fail(query.lastError());
  }
  return games;
}

BoardGame SqliteGameRepository::create(const GameDraft& draft) {
  const auto valid = validateDraft(draft);
  Transaction transaction(database_);
  QSqlQuery query(database_);
  prepare(query, QStringLiteral("INSERT INTO games(name,publisher,location,note) "
                                "VALUES(:name,:publisher,:location,:note)"));
  bindDraft(query, valid);
  execute(query);
  const auto id = query.lastInsertId().toLongLong();
  query.finish();
  transaction.commit();
  return {id, valid};
}

BoardGame SqliteGameRepository::update(qint64 id, const GameDraft& draft) {
  const auto valid = validateDraft(draft);
  Transaction transaction(database_);
  QSqlQuery query(database_);
  prepare(query,
          QStringLiteral("UPDATE games SET name=:name,publisher=:publisher,location=:location,"
                         "note=:note WHERE id=:id"));
  bindDraft(query, valid);
  query.bindValue(QStringLiteral(":id"), id);
  execute(query);
  requireAffectedRow(query);
  query.finish();
  transaction.commit();
  return {id, valid};
}

void SqliteGameRepository::remove(qint64 id) {
  Transaction transaction(database_);
  QSqlQuery query(database_);
  prepare(query, QStringLiteral("DELETE FROM games WHERE id=:id"));
  query.bindValue(QStringLiteral(":id"), id);
  execute(query);
  requireAffectedRow(query);
  query.finish();
  transaction.commit();
}

} // namespace shelf
