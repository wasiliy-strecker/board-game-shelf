#include "persistence/data_directory.h"
#include "persistence/sqlite_game_repository.h"
#include "ui/main_window.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>

int main(int argc, char* argv[]) {
  QApplication application(argc, argv);
  QCoreApplication::setApplicationName(QStringLiteral("de.appfabrik.brettspielregal"));
  QCoreApplication::setOrganizationDomain(QStringLiteral("appfabrik.de"));
  QCoreApplication::setApplicationVersion(QStringLiteral(APP_VERSION));
  QApplication::setApplicationDisplayName(QStringLiteral("BrettspielRegal"));
  QApplication::setWindowIcon(QIcon(QStringLiteral(":/assets/branding/icon-256.png")));
  QTranslator translator;
  if (translator.load(QStringLiteral("qtbase_de"),
                      QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
    application.installTranslator(&translator);
  }
  QCommandLineParser parser;
  parser.setApplicationDescription(QStringLiteral("Eine lokale Sammlung für deine Brettspiele."));
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption dataOption(QStringLiteral("data-dir"),
                                QStringLiteral("Isoliertes absolutes Datenverzeichnis."),
                                QStringLiteral("pfad"));
  parser.addOption(dataOption);
  parser.process(application);
  try {
    const auto directory =
        shelf::resolveDataDirectory(parser.value(dataOption), parser.isSet(dataOption));
    shelf::StorageController controller(
        [directory] { return std::make_unique<shelf::SqliteGameRepository>(directory); });
    shelf::MainWindow window(controller, directory);
    window.show();
    return QApplication::exec();
  } catch (const std::exception& error) {
    QMessageBox::critical(nullptr, QStringLiteral("BrettspielRegal kann nicht starten"),
                          QString::fromUtf8(error.what()));
    return 1;
  }
}
