# Prüfbericht

## Mac-Dateizugriff und portable Exporte, 1. November 2026

Die Mac-Sandbox-Vorlage enthält jetzt `com.apple.security.files.user-selected.read-write`.
Beim Export auf macOS wird zuerst ein Ordner nativ gewählt, anschließend ein
Dateiname eingegeben. Die Ordnerfreigabe schließt die temporäre Exportdatei ein.
Die Dateinameneingabe verhindert Pfadwechsel, Kontrollzeichen und überlange
Namen. Alle neuen Texte sind in sechs Sprachen vorhanden. Abbrechen in beiden
Dialogen erstellt keine Datei. Der Import verwendet weiterhin die native
Dateiauswahl. Benutzerkonten, Paketkennungen und Datenpfade bleiben unverändert.

Die unterstützte Systemsprache beim Erststart ist vom Nutzer ausdrücklich
bestätigt, mit Englisch als Fallback. Vorhandene Sprachwahlen bleiben erhalten.

Der Export wird vor der Veröffentlichung vollständig in einer temporären Datei
erzeugt. Unterstützt das Dateisystem Hardlinks, bleibt die bisherige atomare
Veröffentlichung neuer Dateien erhalten. Andernfalls wird das neue Ziel exklusiv
angelegt (`CREATE_NEW` beziehungsweise `NewOnly`) und mit dem fertigen Inhalt
beschrieben. Eine zwischenzeitlich entstandene Datei oder ein symbolischer
Verweis wird nicht überschrieben. Erfolge werden erst nach geschlossenem
Ausgabestrom gemeldet. Kontrollierte Schreibfehler entfernen die eigene
Teildatei, sofern ihre Identität noch nachweisbar ist. Eine inzwischen ersetzte
Datei wird nicht zur Fehlerbereinigung gelöscht. Bestehende Exporte werden
weiterhin nur nach Bestätigung atomar ersetzt, ohne unsicheren Überschreibfallback.

Grenze des Ausweichwegs: Das Kopieren einer neuen Datei ist selbst kein atomarer
Vorgang. Bei hartem Prozessabbruch, abgezogenem Datenträger oder fehlender
Dateiidentität kann eine unvollständige neue Exportdatei zurückbleiben. Die
interne Sammlung und zuvor vorhandene Exportdateien werden dadurch nicht
ersetzt. Die neue Exportdatei dann erneut unter einem neuen Namen erstellen.
Die Tests simulieren fehlende Hardlink-Unterstützung und Schreibfehler. Ein
physischer exFAT-Datenträger und native Mac-Sandbox-Ausführung waren hier nicht
verfügbar. Daraus folgt keine native macOS- oder Store-Freigabe.

Prüfung mit `bash scripts/check.sh`: Formatter, Compilerwarnungen als Fehler,
clang-tidy, alle neun CTest-Suiten und 118 Texte in sechs Sprachen bestanden.
Zusätzlich alle neun Suiten mit gezielt verweigerten nativen Hardlink-Aufrufen
für Exportdateien ausgeführt. Eigene Regressionen prüfen Kollisionen, einen
spät entstandenen Symlink, eine tatsächliche Schreibgrenze über `RLIMIT_FSIZE`,
Bereinigung und erneuten Export. Ordner-/Dateiname-Dialoge in allen sechs
Sprachen und der bestehende Transferablauf bestehen auch mit echten Qt-Fenstern
auf einem isolierten Xvfb-/Openbox-Bildschirm. Ansichten visuell geprüft.

Nachweise: `build/portable-exports-check.log`, `build/portable-exports-no-links.log`,
`build/portable-exports-ui/`. Nur temporäre synthetische Sammlungen verwendet.
Keine Store-Artefakte, Veröffentlichung oder Änderung persönlicher Sammlungen.

Native Mac-Abnahme: Signierte Sandbox-App mit frischem Profil starten und die
effektiven Entitlements prüfen. JSON aus einem extern gewählten Ordner importieren.
In einen neuen extern gewählten Ordner exportieren, beide Abbruchwege sowie
Ersetzen und Neustart prüfen. Einen exFAT-Datenträger zusätzlich ausprobieren.

## Info-Menü, Datenschutz und englischer Name, 1. November 2026

Die erste C++-App heißt in allen Sprachen Board Game Shelf. Hauptfenster,
Start- und Speicherfehler, lokale Store-Felder und sichtbare Plattformnamen
verwenden die englische Marke. Technische Kennungen, Datenpfade, ausführbarer
Targetname und Sammlungsformat bleiben unverändert.

„Info“ enthält drei Einträge. „Über die App“ zeigt Version, Kurzbeschreibung,
Datenverzeichnis und Support. „Datenschutz“ enthält eine vollständige Erklärung
für die vier Spielfelder, Spracheinstellung und JSON-Transfer. „Lizenzen“ zeigt
die bisherigen Hinweise und Originaltexte. Alle Inhalte sind offline eingebettet.
Die Original-Lizenzdateien wurden nicht verändert. Alle Fenster unterstützen
Kopieren, Scrollen, Escape und den roten Schließen-Button.

Die Datenschutztexte übernehmen die Struktur der lokalen Bookshelf-Erklärung
mit angepassten Spielfeldern und einem aktuellen Stand. Kontaktangaben stammen
aus den bestehenden App-Erklärungen. Als zusätzliche Orientierung wurde die
[Information der Europäischen Kommission](https://commission.europa.eu/law/law-topic/data-protection/information-individuals_en)
zu transparenter Information herangezogen. Die Implementierung enthält keine
Netzwerkschnittstelle für Sammlung, Spracheinstellung oder Info-Fenster.

```bash
bash scripts/check.sh
```

Alle acht CTest-Suiten bestanden. 116 eigene Texte sind in sechs Sprachen
vollständig vorhanden. Formatter, Compiler-Warnungen als Fehler und clang-tidy
sind ohne Befund. Neue GUI-Fälle prüfen das echte Info-Menü mit Pfeiltasten und
Enter, separate Dialoginhalte, Schreibschutz, Auswahl/Scrollen, Escape und rote
Schließen-Buttons in allen sechs Sprachen. Sammlung und Sprachdatei bleiben
bei den Tests bytegleich. Diese Fälle bestanden zusätzlich mit xcb und Openbox
auf einem eigenen Xvfb-Bildschirm. Ansichten wurden visuell kontrolliert.

Paketierungsskripte wurden syntaktisch geprüft. Mac- und Windows-Vorlagen sind
angepasst, native Pakete und neue Store-Lieferungen wurden nicht gebaut.
Die verifizierte Entwicklungsversion wurde mit der bisherigen Nutzersammlung
neu geöffnet. Ein vorhandenes Editorfenster wäre vor dem Neustart erhalten worden.
Keine persönlichen Daten für Tests verwendet, kein Push und keine Veröffentlichung.

Nachweise: `build/information-final-check.log`, `build/information-native-menu/`.

## JSON-Sammlungstransfer und roter Schließen-Button, 1. November 2026

Die Hauptansicht enthält Exportieren und Importieren rechts neben Info und Sprache.
Der Info-Dialog hat einen roten Schließen-Button mit weißer Schrift und sichtbarem
Tastaturfokus. Die vorhandenen Spalten und die aktive Sortierung bleiben erhalten.

Das app-eigene JSON-Format ist `board-game-shelf.collection`, Version 1. Ein Export
enthält sämtliche gespeicherten Spiele unabhängig von der Suche. Ein Import wird
zuerst vollständig validiert und erst nach Bestätigung atomar ergänzt. Der bereits
geprüfte Inhalt bleibt während der Bestätigung als Snapshot im Worker. Identische
Exemplare werden nach Anzahl abgeglichen. Bestehende IDs bleiben erhalten.

Vorhandene Exportdateien benötigen eine ausdrückliche Ersatzbestätigung. Die
Veröffentlichung erfolgt aus einer vollständigen temporären Datei. Dateiidentität,
Revision und Inhalt werden vor Ersatz erneut geprüft. Neue Ziele werden ohne
Überschreiben veröffentlicht. App-Daten, Hardlinks auf App-Dateien und symbolische
Dateiverweise sind als Exportziel ausgeschlossen. Unbekannte Formate, doppelte
JSON-Schlüssel, ungültiges Unicode und überschrittene Grenzen werden abgewiesen.

`bash scripts/check.sh` besteht mit Formatter, strikten Compilerwarnungen,
clang-tidy und allen sieben CTest-Suiten. 158 fachliche Qt-Testfälle und zwölf
Initialisierungs-/Abschlussaufrufe sowie der Katalogtest sind erfolgreich.
107 Meldungen sind in allen sechs App-Sprachen vollständig vorhanden.
Neue Tests prüfen unter anderem den Rundlauf mit Unicode und mehrzeiligen Notizen,
Duplikatanzahlen, unveränderte IDs, geänderte Exportziele, 10.000 Einträge, 10 MiB,
einen späten SQL-Fehler mit vollständigem Rollback und erneuten Importversuch,
Abbrüche, Ersatzbestätigung sowie den Such- und Sortierzustand nach Import.

Zusätzliche echte Qt-Fenster prüfen den erweiterten Footer in allen sechs Sprachen
bei 1000 × 740 und 560 × 520 Pixeln. Eine isolierte Ausführung der Produktionsapp
mit nativen GTK3-Dateidialogen besteht Export bei aktiver Suche, Importabbruch,
ergänzenden und wiederholten Import sowie Neustart mit unveränderten Daten.
Der rote Button wurde visuell geprüft und tatsächlich geklickt. Sämtliche
Testdaten liegen in temporären, synthetischen Sammlungen.

Protokolle und Bilder liegen lokal unter `build/collection-transfer-check.log`,
`build/collection-transfer-gui/` und `build/collection-transfer-native/`.
Native Windows-/macOS-Dateidialoge, Mac-Sandbox und Store-Pakete wurden hier
nicht geprüft. Keine neue Paketierung und keine Veröffentlichung.

## Buttonanordnung und Spaltensortierung, 1. November 2026

Die Hauptaktionen folgen der Anordnung von Bookshelf by Appfabrik. Hinzufügen
steht oben rechts, Suche leeren neben dem Suchfeld und Bearbeiten sowie Löschen
rechts unter der Tabelle. App-Info und Sprache befinden sich unten rechts.
Die drei festen Spalten haben dauerhaft sichtbare Pfeilpaare mit hervorgehobener
Sortierrichtung. Der native Fensterstil bleibt erhalten.

`bash scripts/check.sh` prüft Formatter, strikte Compilerwarnungen, alle fünf
CTest-Suiten und clang-tidy. 129 fachliche Qt-Testfälle plus acht Initialisierungs-
und Abschlussaufrufe sowie der Katalogtest bestehen. Die neuen Regressionen
prüfen alle drei Spalten in beiden Richtungen und allen sechs Sprachen.
Unicode-Normalisierung, Groß-/Kleinschreibung, natürliche Zahlenfolge, leere
optionale Felder und stabile IDs bei gleichen Sortierwerten sind enthalten.
Echte Headerklicks prüfen Suche, Auswahl und Notiz, anschließendes Bearbeiten,
Anlegen, Löschen und erneutes Laden bei aktiver Sortierung.

Auf einem isolierten X11-Bildschirm wurden die neuen Layout- und Sortiertests
zusätzlich mit echten Qt-Fenstern ausgeführt. Alle sechs Sprachen bestehen bei
1000 × 740 und 560 × 520 Pixeln. Die englische Normalansicht und die französische
kleine Ansicht wurden visuell geprüft. Der bestehende GUI-Smoke-Test besteht
mit synthetischen Daten einschließlich SQLite-Sperre, Entwurfschutz, vollständigem
CRUD-Ablauf und Neustart. Die Tastaturnavigation ersetzt darin die bisherigen
festen Koordinaten der verschobenen Buttons.

Prüfprotokolle und Aufnahmen liegen lokal in `build/action-sorting-check.log`
und `build/action-sorting-gui/`. Keine neue Paket- oder Store-Auslieferung.

## Internationalisierter Stand, 29. Oktober 2026

86 fachliche Qt-Testfälle und ein zusätzlicher Katalogtest bestehen in fünf
CTest-Suiten. Darin enthalten sind die bisherigen 46 Fälle und 40 neue
Internationalisierungsfälle. Initialisierung und Abschluss der vier Qt-Suiten
kommen als acht erfolgreiche Aufrufe hinzu.

Der Release-Check umfasst clang-format, strikte Compilerwarnungen, die vollständige
Testsuite und clang-tidy. ASan, LeakSanitizer und UBSan bestehen zusätzlich.
Ruff prüft und formatiert alle Python-Helfer. Die vorgebauten Qt-Bibliotheken
sind weiterhin nicht selbst mit Sanitizern instrumentiert.

Die neuen Fälle prüfen sechs Systemsprachen samt regionalen Varianten,
englischen Fallback, ausdrückliche Sprachwahl, echte Qt-Kontextmenüs,
übersetzte Validierungs- und Datenbankfehler, Unicode-Daten, atomare
Einstellungen, Fehler mit Wiederholung, Sortierung, Zahlenformate und
kleine Dialoge mit Schriftgröße 18. Alle 81 Meldungen sind vollständig übersetzt.

Die aktuelle Aufnahmeprüfung startet die gebündelte Produktionsapp in zwölf
Prozessen auf einem isolierten Xvfb-Bildschirm mit Openbox. Jede Sprache wird
mit Übersicht, Editor, Suche und Sprachwahl aufgenommen. Zusätzlich werden
Pflichtfelder, eine echte exklusive SQLite-Sperre, erneutes Speichern,
Unicode-CRUD, sichere Bestätigungen, kleine Fenster und fehlgeschlagenes
Speichern der Sprache durchgespielt. Nach einer erfolgreichen Sprachänderung
wird die App beendet und mit derselben Datenbank neu gestartet. Sprache und
unveränderte Unicode-Daten werden geprüft.

Die App läuft dabei mit bereinigten Qt-, QML- und SDK-Umgebungsvariablen und
`PATH=/usr/bin:/bin`. `/proc/<pid>/maps` weist die eigenen Qt-Bibliotheken und
das eigene QSQLITE-Plugin nach. App- und Qt-Übersetzungen sind eingebettet.
Die Testdaten liegen ausschließlich in temporären Verzeichnissen.

Zusätzlich besteht der bisherige deutsche GUI-Smoke-Test. Er wählt jetzt
im isolierten Testverzeichnis ausdrücklich de-DE.

Die 24 unveränderten PNG-Aufnahmen haben jeweils 1920 × 1080 Pixel und wurden
visuell kontrolliert. `report.json` dokumentiert die Prozesse, Quellen,
Laufzeitdateien und Bildprüfsummen. Die Ordnerlieferung enthält den geprüften
Commit und weitere Prüfsummen. Sie enthält kein ZIP-Archiv.

Windows- und macOS-Ausführung, Signierung, finale Paketressourcen und
Store-Einreichung sind in diesem Schritt nicht geprüft. Die Bilder sind
Linux-Vorschauen. Befehle und Voraussetzungen stehen in
[internationalization.md](internationalization.md).

## Ursprünglicher Stand vom 27. Oktober 2026

Stand: 27. Oktober 2026, BrettspielRegal 0.1.0.

## Umgebung und automatische Prüfungen

Ubuntu 24.04.4 LTS x64, GCC 13.3.0, Qt 6.8.3, CMake 4.4.4, Ninja 1.11.1,
clang-format 23.1.3 und clang-tidy 22.1.8. Qt selbst ist ein vorgebautes,
dynamisches SDK. Die Anwendung und ihre Tests verwenden C++20.

```bash
bash scripts/check.sh
cmake --build --preset release --parallel 4
ctest --preset release
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 bash scripts/check.sh asan
```

46 fachliche Testausführungen bestehen in drei CTest-Suiten. Qt Test meldet
zusätzlich sechs erfolgreiche Initialisierungs- und Abschlussaufrufe.
Es gibt 16 Domain-Fälle, 16 Repository-Fälle und 14 GUI-Fälle.
Keine Fehler und keine übersprungenen Fälle auf dieser Linux-Umgebung.

Die Tests prüfen Unicode-Leerraum, optionale Felder, unveränderte mehrzeilige
Notizen, CRUD, gleichnamige Spiele mit unterschiedlichen IDs und stabile
Sortierung. Die Suche berücksichtigt jedes Feld, Groß- und Kleinschreibung,
Umlaute sowie zusammengesetzte und zerlegte Schreibweisen.

Echte temporäre SQLite-Dateien prüfen Wiederöffnen, SQL-ähnliche Eingaben,
Schreibschutz, falsche Pfade, Datenbanksperren beim Start und Schreiben sowie
erneutes Speichern. Ein offener Leser verhindert gezielt den Commit einer
Änderung. Anschließend wird der vollständige unveränderte Originalstand geprüft.
Beschädigte, fremde, bereits vorhandene leere und unbekannt neuere Datenbanken
bleiben unverändert. Schemafehler und eine zweite App-Instanz werden abgewiesen.

Echte Qt-Dialoge prüfen ungültige Eingaben mit erhaltenem Entwurf, Abbrechen,
Escape und Fensterschließen, sichere Vorauswahl, Verwerfen bestehender Änderungen,
Löschabbruch, Löschfehler, fehlgeschlagenes Speichern und Wiederholung.
Eine verzögerte Operation prüft GUI-Reaktionsfähigkeit, Doppelaktionsschutz,
Thread-Zuordnung, sicheren Abbau und späte Ergebnisse bei zerstörtem Dialog.

Compilerwarnungen werden für eigenen C++-Code als Fehler behandelt. Formatter
und clang-tidy laufen ohne Befunde. AddressSanitizer, LeakSanitizer und
UndefinedBehaviorSanitizer bestehen ohne Befunde oder Unterdrückungsdateien.
Die vorgebauten Qt- und SQLite-Bibliotheken sind selbst nicht mit diesen
Sanitizern instrumentiert. Deren interne Ausführung ist deshalb nicht in
gleichem Umfang abgedeckt wie der eigene Code.

Zusätzlich bestehen Ruff-Prüfung und Formatprüfung der Python-Helfer,
`bash -n` der Shell-Helfer sowie JSON- und XML-Syntaxprüfungen. PowerShell,
Windows-Ressourcencompiler und native Apple-Werkzeuge wurden hier nicht ausgeführt.

## Sichtbare Oberfläche

Die Entwicklungsapp wurde auf dem vorhandenen Linux-Desktop gestartet und
visuell kontrolliert. Der vollständige Ablauf wurde anschließend auf einem
separaten rendernden X11-Bildschirm mit echten Qt-Fenstern, Tastatur- und
Mausereignissen durchgeführt. Die Screenshots wurden angesehen und die
Datenbank zusätzlich unabhängig gelesen. Das sind zusätzliche Prüfungen zu
den fensterlosen automatisierten Dialogtests.

```bash
DISPLAY=:91 python3 scripts/gui_smoke.py --binary build/dev/brettspielregal --output build/manual/dev-complete
```

Der Testhelfer verwendet ausschließlich ein temporäres Datenverzeichnis.
Normale Sammlungen werden weder geöffnet noch beschrieben. Der X11-Bildschirm
muss vorher bereitstehen. Benötigte Testwerkzeuge sind Python 3, xdotool und
ImageMagick. Diese Werkzeuge gehören nicht zur ausgelieferten App.

| Ablauf | Ergebnis |
| --- | --- |
| Leere Sammlung und Pflichtfeldfehler | Verständliche Hinweise, keine Datensätze angelegt |
| Vollständiges Spiel mit mehrzeiliger Notiz | Alle Felder korrekt gespeichert |
| Suche in der Notiz und Suche ohne Treffer | Passende Liste und eigener Leerzustand |
| Escape bei Änderungen und Enter in der Rückfrage | „Weiterbearbeiten“ behält den Entwurf |
| Verwerfen einer geänderten Notiz | Gespeicherte Notiz bleibt unverändert |
| Bearbeiten bei echter exklusiver SQLite-Sperre | Dialog und Eingaben bleiben erhalten |
| Sperre lösen und erneut speichern | Vollständige Änderung mit unveränderter ID gespeichert |
| Kleiner Fehlerdialog | Scrollbarer Inhalt und erreichbare Aktionen |
| Gleichnamiges Spiel anlegen | Zwei unabhängige IDs |
| App beenden und erneut starten | Beide Einträge samt Notiz vorhanden |
| Löschabbruch und bestätigtes Löschen | Abbruch erhält Daten, Bestätigung löscht nur die gewählte ID |
| Letzten Eintrag löschen | Leere Sammlung wieder korrekt dargestellt |

Ein zusätzlicher echter X11-Dialogtest verwendet Schriftgröße 18 und ein Fenster
von 400 × 340 Pixeln. Beide Dialogaktionen bleiben sichtbar. Der Formularbereich
scrollt. Die Screenshot-Abnahme bestätigt das Verhalten.

Die lokalen Belege liegen unter `build/manual`. Ausgewählte synthetische
Screenshots liegen unter `docs/screenshots` im Repository.

## Prüfung des eigenständigen Pakets

Ein Linux-x64-Testpaket mit eigener Qt-Laufzeit wurde erzeugt, in ein neues
Verzeichnis außerhalb des Projekts kopiert und dort mit bereinigter Umgebung
gestartet. `PATH` enthielt nur `/usr/bin:/bin`. Externe Qt-, QML-, SDK- und
Bibliothekspfade wurden entfernt. Der Paketlauncher verwendet seine eigenen
Bibliotheken und Plugins.

```bash
DISPLAY=:91 python3 scripts/gui_smoke.py --binary /absoluter/pfad/zum/testpaket/BrettspielRegal.sh --output build/manual/package-complete --clean-environment
```

Der vollständige oben beschriebene Ablauf besteht auch mit diesem Paket.
Es wurden echte Fenster gerendert und bedient, beide gleichen Namen nach einem
Prozessneustart geladen und schließlich unabhängig gelöscht. Eine echte
Datenbanksperre erhält den Entwurf auch im Paket. Nach Freigabe gelingt das
erneute Speichern mit unveränderter ID und unveränderter Notiz.

Die Prozessabbildung unter `/proc/<pid>/maps` bestätigt, dass alle geladenen
Qt- und ICU-Bibliotheken sowie `plugins/sqldrivers/libqsqlite.so` aus dem
Paketverzeichnis stammen. Der Nachweis steht unter
`build/manual/package-complete/loaded-libraries.json`. Die üblichen
Ubuntu-Systembibliotheken bleiben Voraussetzungen. Dieser Linux-Nachweis
ersetzt keine native Windows- oder macOS-Abnahme.

## Noch offene Zielsysteme

Windows-x64-Paket und macOS-Bundles für Intel und Apple Silicon sind vorbereitet.
Sie wurden auf diesem Linux-Rechner nicht nativ gebaut oder ausgeführt.
Windows-ACLs, native Installation, Updates, macOS-Sandbox, Systemdialoge und
plattformspezifische Tastenkürzel benötigen dort eine eigene Abnahme.

Die GitHub-Workflows wurden nicht ausgeführt. Signierung, Store-Lizenzprüfung,
Store-Metadaten, Einreichung und Veröffentlichung stehen aus. Es wurde weder
gepusht noch veröffentlicht.
