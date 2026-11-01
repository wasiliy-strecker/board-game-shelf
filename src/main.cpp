#include "i18n/languages.h"
#include "persistence/data_directory.h"
#include "persistence/language_preferences.h"
#include "persistence/sqlite_game_repository.h"
#include "ui/main_window.h"
#include "ui/native_edit_menus.h"
#include <QCoreApplication>

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QMessageBox>
#include <QTimer>

int main(int argc, char* argv[]) {
  QApplication application(argc, argv);
  QCoreApplication::setApplicationName(QStringLiteral("de.appfabrik.brettspielregal"));
  QCoreApplication::setOrganizationDomain(QStringLiteral("appfabrik.de"));
  QCoreApplication::setApplicationVersion(QStringLiteral(APP_VERSION));
  QApplication::setApplicationDisplayName(QStringLiteral("Board Game Shelf"));
#ifdef Q_OS_LINUX
  QGuiApplication::setDesktopFileName(QCoreApplication::applicationName());
#endif
  QApplication::setWindowIcon(QIcon(QStringLiteral(":/assets/branding/icon-256.png")));
  shelf::Translations translations;
  translations.install(QStringLiteral("auto"));
  QCommandLineParser parser;
  parser.setApplicationDescription(
      QCoreApplication::translate("App", "Eine lokale Sammlung für deine Brettspiele."));
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption dataOption(
      QStringLiteral("data-dir"),
      QCoreApplication::translate("App", "Isoliertes absolutes Datenverzeichnis."),
      QCoreApplication::translate("App", "pfad"));
  parser.addOption(dataOption);
  parser.process(application);
  try {
    const auto directory =
        shelf::resolveDataDirectory(parser.value(dataOption), parser.isSet(dataOption));
    const auto preference = shelf::readLanguagePreference(directory);
    translations.install(preference.value);
#ifdef Q_OS_MACOS
    shelf::NativeEditMenus nativeEditMenus;
#endif
    shelf::StorageController controller(
        [directory] { return std::make_unique<shelf::SqliteGameRepository>(directory); });
    shelf::MainWindow window(controller, directory, preference.value);
    window.show();
    if (preference.readFailed) {
      QTimer::singleShot(0, &window, [&window] {
        QMessageBox::warning(
            &window, QCoreApplication::translate("App", "Sprache"),
            QCoreApplication::translate(
                "App", "Die gespeicherte Sprache konnte nicht gelesen werden. Englisch wird "
                       "verwendet. Wähle die Sprache erneut in den Einstellungen."));
      });
    }
    return QApplication::exec();
  } catch (const std::exception& error) {
    QMessageBox::critical(nullptr,
                          QCoreApplication::translate("App", "Board Game Shelf kann nicht starten"),
                          QString::fromUtf8(error.what()));
    return 1;
  }
}
