# Board Game Shelf

## Linux-Anwendungsmenü und Dock

Nach dem Entwicklungsbuild registriert dieser Befehl die App im aktuellen
Benutzerkonto:

```bash
python3 scripts/install_desktop.py --development
```

Danach erscheint **Board Game Shelf** mit dem eigenen Icon im Anwendungsmenü und Dock.
Der Menüeintrag startet den vorhandenen Entwicklungsbuild über
`scripts/launch.sh`. Dieser baut nichts neu. `scripts/run.sh` baut weiterhin
und startet anschließend die App. Bei fehlendem Build zeigt der Starthelfer
eine Fehlermeldung auf stderr.

Der Installer schreibt ausschließlich den eigenen Desktop-Eintrag und Icons
unter `$XDG_DATA_HOME`, standardmäßig `~/.local/share`. Wiederholtes Ausführen
aktualisiert verwaltete Einträge. Fremde Einträge werden nicht überschrieben.
Die normale Sammlung und Spracheinstellung bleiben erhalten.
`--data-dir /absoluter/testpfad` ist nur für einen ausdrücklich gewünschten
abweichenden Startpfad nötig.

Ein eigenständiges Linux-Paket enthält denselben Installer. Im Paketordner
genügt `python3 install_desktop.py`. Alternativ ist
`python3 scripts/install_desktop.py --app-dir /absoluter/paketpfad` möglich.
Dabei verweist der Menüeintrag auf `BoardGameShelf.sh` und die paketierte Laufzeit.

Die bestehenden Motive bleiben unverändert. Zusätzliche PNG-Größen können
mit `python3 scripts/prepare_icons.py` erzeugt werden. Dafür ist Python mit
Pillow erforderlich, für Installation und App-Start nicht.

Eine kleine Desktop-App für deine private Brettspielsammlung. Erfasse den
Spielnamen, Verlag oder Hersteller, Aufbewahrungsort und eine mehrzeilige Notiz.
Suche über alle vier Felder. Die Sammlung bleibt lokal und nach einem Neustart
erhalten. Es gibt kein Konto und die fertige App benötigt kein Internet.

![Board Game Shelf mit synthetischer Sammlung](docs/screenshots/brettspielregal.png)

Version 0.1.0 ist die erste C++-Stufe des Hobby-Portfolios und ein kleines,
nachvollziehbares Bewerbungsprojekt. Die späteren Veröffentlichungsziele sind
**zuerst Microsoft Store und danach Mac App Store**. Noch keine Veröffentlichung.

Die Oberfläche unterstützt Deutsch, Englisch, Spanisch, Französisch, Italienisch
und brasilianisches Portugiesisch. „Sprache“ speichert eine Auswahl für den nächsten
Start. Standardmäßig wird die Systemsprache verwendet, mit Englisch als Fallback.
Die [Internationalisierungsanleitung](docs/internationalization.md) beschreibt
Übersetzungen, Store-Texte und echte Screenshots. Die Vorschau wird als Ordner
ohne ZIP ausgeliefert. Die native Windows-Store-Abnahme steht noch aus.

## Entwickeln und starten

Voraussetzungen sind ein C++20-Compiler, CMake ab 3.25, Ninja und Qt 6.8.3 mit
Core, Gui, Widgets, Sql, Test, LinguistTools, Qt-Basisübersetzungen sowie QSQLITE.
Für den Katalogtest wird Python 3 benötigt. Die lokal geprüfte Kombination ist
Ubuntu 24.04 x64, GCC 13.3.0, CMake 4.4.4 und Ninja 1.11.1.
clang-format 23.1.3 und clang-tidy 22.1.8 dienen der Prüfung.

Linux und macOS:

```bash
export BRETTSPIELREGAL_QT_ROOT=/absoluter/pfad/zu/Qt/6.8.3/gcc_64
source scripts/env.sh
cmake --preset dev
cmake --build --preset dev --parallel 4
./build/dev/brettspielregal
```

Auf macOS zeigt der SDK-Pfad auf `macos`. Der Launcher liegt im erzeugten
`build/dev/brettspielregal.app/Contents/MacOS/brettspielregal`.
Unter Linux funktioniert alternativ `bash scripts/run.sh`.
Maschinenspezifische SDK- und Werkzeugpfade dürfen in `.tools/env.sh` stehen.
Diese Datei wird nicht versioniert und ist für andere Rechner nicht erforderlich.

Windows, in einer x64-Developer-PowerShell für Visual Studio 2022:

```powershell
$env:BRETTSPIELREGAL_QT_ROOT = 'C:\Qt\6.8.3\msvc2022_64'
$env:CMAKE_PREFIX_PATH = $env:BRETTSPIELREGAL_QT_ROOT
$env:PATH = "$env:BRETTSPIELREGAL_QT_ROOT\bin;$env:PATH"
cmake --preset dev
cmake --build --preset dev --parallel 4
.\build\dev\brettspielregal.exe
```

Für die Entwicklung darf dasselbe installierte Qt-SDK verwendet werden wie für
andere Projekte. App-Code, Daten, Git-Historie und ausgelieferte Laufzeit bleiben
vollständig unabhängig. Neue Abhängigkeiten werden nicht beim App-Start geladen.

## Bedienung

„Spiel hinzufügen“ öffnet den gemeinsamen Eingabedialog. Nur der Spielname ist
erforderlich. Doppelklick oder Enter in der Tabelle öffnet die Bearbeitung.
Gleichnamige Spiele bleiben eigenständige Einträge. Löschen verlangt eine
Bestätigung. Die Notiz des ausgewählten Spiels ist vollständig scrollbar.

„Spiel hinzufügen“ steht oben rechts und „Suche leeren“ direkt neben der Suche.
Unter der Tabelle stehen links die Anzahl und rechts „Bearbeiten“ und „Löschen“.
App-Info und Sprachwahl befinden sich unten rechts.

Ein Klick auf Spielname, Verlag / Hersteller oder Aufbewahrungsort sortiert
die Tabelle nach dieser Spalte. Weitere Klicks wechseln zwischen aufsteigender
und absteigender Reihenfolge. Beide Pfeile bleiben sichtbar, die aktive Richtung
ist hervorgehoben. Die Spalten bleiben an ihrem Platz. Umlaute und Zahlen werden
gemäß der App-Sprache einsortiert. Suche, Speichern, Löschen und erneutes Laden
behalten die aktive Sortierung. Die Auswahl folgt der ID des Spiels, auch wenn
sich seine Zeile verschiebt. Nach einem App-Neustart gilt wieder Spielname
aufsteigend.

„Exportieren“ und „Importieren“ stehen unten rechts neben der Sprachwahl.
Der Schließen-Button im Infofenster ist rot.

| Taste | Aktion |
| --- | --- |
| Strg+N / Cmd+N | Spiel hinzufügen |
| Strg+F / Cmd+F | Suche fokussieren und Text auswählen |
| Strg+S / Cmd+S | Dialog speichern |
| Strg+Komma | Sprachwahl öffnen |
| Escape | Dialog abbrechen, bei Änderungen mit Rückfrage |
| Enter im Notizfeld | Neue Zeile |
| Entf in der Tabelle | Löschbestätigung öffnen |
| Cmd+Rücktaste auf macOS | Löschbestätigung öffnen |

„Weiterbearbeiten“ ist bei ungespeicherten Änderungen die sichere Vorauswahl.
Ein Speicherfehler lässt alle Eingaben offen. Während eines Zugriffs sind
Doppelaktionen gesperrt. Nach einem erfolgreichen Speichern außerhalb der
aktuellen Suche erklärt ein Hinweis, warum der Eintrag gerade nicht sichtbar ist.

## Sammlung exportieren und importieren

„Exportieren“ sichert die gesamte gespeicherte Sammlung als JSON-Datei.
Eine aktive Suche oder Zeilenauswahl begrenzt den Export nicht. Spielname,
Verlag / Hersteller, Aufbewahrungsort und die vollständige Notiz sind enthalten.
Der Dateidialog schlägt einen Namen mit Datum und Uhrzeit vor. Auf macOS wird
zuerst der Zielordner im nativen Dialog gewählt, anschließend der Dateiname.
Damit umfasst die Ordnerfreigabe auch die temporäre Exportdatei in der Sandbox.
Eine bestehende Datei wird erst nach Bestätigung ersetzt. Bei einem Fehler
bleibt sie erhalten. Neue Exporte funktionieren auch ohne Hardlink-Unterstützung,
zum Beispiel auf exFAT. Details zum Fehlerschutz stehen im Prüfbericht.

„Importieren“ prüft eine zuvor exportierte Board Game Shelf-Datei und fragt vor
dem Ergänzen nach. Vorhandene Spiele und IDs bleiben erhalten. Identische
Exemplare werden anhand aller vier Textfelder und ihrer Anzahl abgeglichen.
Enthält die Datei zwei identische Exemplare und die Sammlung nur eines, wird
genau eines ergänzt. Ein erneuter Import derselben Datei erzeugt keine weiteren
Exemplare. Gleiche Spielnamen mit unterschiedlichen Angaben bleiben unabhängig.

Der Import wird vollständig oder gar nicht gespeichert. Nach Erfolg wird die
Suche geleert und die bisherige Sortierung beibehalten. Ein Abbruch oder Fehler
ändert die Sammlung und die Suche nicht. Die geprüften Inhalte werden vor der
Bestätigung eingelesen, eine spätere Änderung der Datei verändert den Import nicht.

Das app-eigene UTF-8-Format heißt `board-game-shelf.collection`, Version 1.
Es enthält `games` mit den Feldern `name`, `publisher`, `location` und `note`.
Technische IDs, Spracheinstellungen und App-Datenpfade werden nicht übertragen.
Dateien anderer Portfolio-Apps sind keine Board Game Shelf-Importdateien.
Eine Datei darf höchstens 10.000 Spiele und 10 MiB enthalten. Die drei einzeiligen
Textfelder haben dieselbe Grenze wie der Editor von jeweils 1.000.000 Zeichen.
Ein ergänzender Import darf die Sammlung nicht über 10.000 Spiele vergrößern.
Eine bestehende größere Sammlung wird nicht gekürzt.

## Daten und isolierte Tests

Die Datei `brettspielregal.sqlite3` liegt im von Qt ermittelten persönlichen
App-Datenverzeichnis. „Über die App“ zeigt den tatsächlich verwendeten Pfad.
Typische Pfade einer normalen, nicht sandboxierten Installation:

| System | Verzeichnis |
| --- | --- |
| Windows | `%LOCALAPPDATA%\de.appfabrik.brettspielregal` |
| macOS | `~/Library/Application Support/de.appfabrik.brettspielregal` |
| Linux | `$XDG_DATA_HOME/de.appfabrik.brettspielregal`, sonst `~/.local/share/de.appfabrik.brettspielregal` |

Store-Sandbox und Windows-Paketvirtualisierung können die tatsächlichen Pfade
ändern. Die native Abnahme muss diese ermitteln. Es gibt keinen selbst gebauten
Ersatzpfad ins Programmverzeichnis. Für eine portable Sammlungssicherung dient
„Exportieren“. Für eine zusätzliche manuelle Datenbanksicherung die App schließen
und die SQLite-Datei kopieren. Die zusätzliche `collection.lock` ist keine Nutzerdatenbank.
`language.txt` enthält ausschließlich die gewählte Oberflächensprache.

```bash
./build/dev/brettspielregal --data-dir /tmp/brettspielregal-test
bash scripts/check.sh
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 bash scripts/check.sh asan
```

`--data-dir` muss absolut und nicht leer sein. Fehler fallen nicht auf private
Daten zurück. Automatische Tests verwenden `QTemporaryDir` und synthetische
Texte. Eine zweite Instanz derselben Sammlung wird abgewiesen. Externe
gleichzeitige Bearbeitung ist kein unterstützter Arbeitsablauf.

## Aufbau, Pakete und nächste Stufen

Die [Architektur](docs/architecture.md) erklärt C++-Entscheidungen und den
Speicherablauf für ein Bewerbungsgespräch. Der [Prüfbericht](docs/verification.md)
trennt nachgewiesene Prüfungen von offenen Zielsystemtests. Die
[Paketierungsanleitung](docs/packaging.md) beschreibt eigenständige Testpakete
und spätere Store-Schritte. [Lizenzhinweise](THIRD_PARTY_NOTICES.md) gehören dazu.

| Stufe | Eigenständige App | Neue Hauptfunktion |
| --- | --- | --- |
| 1 | Board Game Shelf | Einträge anlegen, bearbeiten, löschen und suchen |
| 2 | SammlerVitrine | Ein Foto pro Eintrag |
| 3 | ModellbauProjekt | Projektstatus |
| 4 | HolzwerkProjekt | Checkliste |
| 5, Reserve | RestaurierungsHeft | PDF-Export |

Jede spätere App übernimmt die vorherigen Grundfunktionen. Hier gibt es keine
Fotos, Statuswerte, Checklisten, PDF-Exporte, Ausleihe, Bewertungen oder
Online-Anbindungen. FactoryForge bleibt ein separates größeres Projekt.

## Info, Datenschutz und Lizenzen

„Info“ enthält „Über die App“, „Datenschutz“ und „Lizenzen“. Die App-Informationen
zeigen Board Game Shelf, die Version, den Datenpfad und den Supportkontakt.
Die Datenschutzerklärung beschreibt die lokale Brettspielsammlung, die
Spracheinstellung, JSON-Import und -Export, Löschung sowie Kontaktmöglichkeiten.
Sie ist in allen sechs Oberflächensprachen eingebettet und ohne Internet lesbar.
Lizenzhinweise und mitgelieferte Originaltexte stehen im eigenen Lizenzfenster.
Alle drei Fenster sind kopierbar, schreibgeschützt und über den roten
Schließen-Button oder Escape schließbar.
Der öffentliche Name lautet in jeder Sprache Board Game Shelf. Bestehende
App-Kennungen, Datenverzeichnisse und Dateiformate bleiben erhalten.
