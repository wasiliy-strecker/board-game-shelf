#include "i18n/privacy_notice.h"
#include <QCoreApplication>

namespace shelf {
QString privacyNotice() {
  return QCoreApplication::translate("App", "Datenschutzerklärung für Board Game Shelf") +
         QStringLiteral("\n\n") + QCoreApplication::translate("App", "Stand: 1. November 2026") +
         QStringLiteral("\n\n") +
         QCoreApplication::translate(
             "App",
             "Verantwortlicher und Kontakt\nWasiliy Strecker · AppFabrik AI\nLudwigstrasse 23, "
             "86152 Augsburg, Deutschland\nDatenschutz: contact@appfabrik-ai.de\nSupport: "
             "support@appfabrik-ai.de\n\nZweck und gespeicherte Daten\nBoard Game Shelf verwaltet "
             "deine private Brettspielsammlung. Die App speichert Spielnamen, Verlage oder "
             "Hersteller, Aufbewahrungsorte, Notizen und interne Eintragskennungen sowie deine "
             "Spracheinstellung standardmäßig lokal im Benutzerprofil deines Computers. Die "
             "Angaben können personenbezogene Informationen enthalten, wenn du sie selbst "
             "einträgst. Die Verarbeitung dient ausschließlich den von dir angeforderten "
             "Funktionen zum Verwalten, Suchen, Exportieren und Importieren deiner "
             "Sammlung.\n\nLokale Verarbeitung\nDie App benötigt kein Konto und keinen Server. Sie "
             "übermittelt deine Sammlung und Spracheinstellung nicht an den Entwickler oder andere "
             "Empfänger. Sie enthält keine Werbung, Telemetrie, Nutzungsanalyse oder eigene "
             "Cloud-Synchronisierung. Die Datenschutzerklärung ist Bestandteil der App und wird "
             "ohne Internetverbindung angezeigt. Board Game Shelf verschlüsselt weder seine "
             "gespeicherte Sammlung noch exportierte JSON-Dateien selbst. Der Schutz hängt auch "
             "von deinem Benutzerkonto, den Zugriffsrechten und einer gegebenenfalls aktivierten "
             "Datenträgerverschlüsselung ab.\n\nImport, Export und Sicherungskopien\nBeim Import "
             "liest die App die von dir ausgewählte JSON-Datei lokal. Ein Export enthält alle "
             "gespeicherten Brettspiele mit ihren vier Textfeldern. Er enthält weder interne "
             "Eintragskennungen noch die Spracheinstellung. Du bestimmst den Speicherort und eine "
             "mögliche Weitergabe. Bereits exportierte Dateien sind eigenständige Kopien und "
             "werden durch spätere Änderungen in der App nicht aktualisiert oder gelöscht. Wenn du "
             "einen synchronisierten Ordner, ein Netzlaufwerk oder einen Sicherungsdienst nutzt, "
             "kann dieser Dienst die Dateien außerhalb der App verarbeiten oder übertragen. Dafür "
             "gelten die Einstellungen und Datenschutzbedingungen des jeweiligen Anbieters. Board "
             "Game Shelf steuert diese Dienste nicht.\n\nSpeicherdauer und Löschung\nDie App "
             "bewahrt die Sammlung auf, bis du Einträge löschst oder die gespeicherten App-Daten "
             "entfernst. Du kannst Einträge in der App ansehen, bearbeiten und löschen. Zum "
             "vollständigen Entfernen der lokalen Sammlung und Spracheinstellung schließt du die "
             "App und entfernst ihren Datenordner. Die Speicherorte sind in der mitgelieferten "
             "Anleitung beschrieben. Exportdateien und andere Sicherungskopien musst du an ihren "
             "jeweiligen Speicherorten gesondert löschen. Das Löschen eines Eintrags ist keine "
             "garantierte sichere Löschung aller früheren Dateiinhalte. Verlasse dich nicht "
             "darauf, dass eine Deinstallation alle App-Daten und externen Kopien "
             "entfernt.\n\nKontakt und Support\nWenn du uns freiwillig eine E-Mail sendest, "
             "verarbeiten wir deine E-Mail-Adresse, den Nachrichteninhalt und beigefügte Dateien "
             "zur Bearbeitung der Anfrage. Deine lokal gespeicherte Sammlung wird dabei nicht "
             "automatisch mitgesendet. Rechtsgrundlage ist bei vertragsbezogenen Anfragen Artikel "
             "6 Absatz 1 Buchstabe b DSGVO, ansonsten unser berechtigtes Interesse an der "
             "Beantwortung von Anfragen nach Artikel 6 Absatz 1 Buchstabe f DSGVO. Wir bewahren "
             "die Korrespondenz nur so lange auf, wie dies für die Bearbeitung und gesetzliche "
             "Aufbewahrungspflichten erforderlich ist.\n\nDeine Rechte und weitere "
             "Anbieter\nSoweit anwendbar, hast du Rechte auf Auskunft, Berichtigung, Löschung, "
             "Einschränkung der Verarbeitung, Datenübertragbarkeit und Widerspruch sowie das Recht "
             "auf Beschwerde bei einer Datenschutzaufsichtsbehörde. Nutze dafür den "
             "Datenschutzkontakt. Auf Sammlungen, die ausschließlich auf deinem Computer liegen, "
             "hat der Entwickler keinen Zugriff. Beim Bezug oder Aktualisieren über Apple App "
             "Store, Microsoft Store oder GitHub können diese Anbieter selbst Daten verarbeiten. "
             "Dafür gelten deren eigene Datenschutzbedingungen. Diese Erklärung beschreibt die "
             "Datenverarbeitung durch Board Game Shelf und wird bei entsprechenden Änderungen "
             "aktualisiert.");
}
} // namespace shelf
