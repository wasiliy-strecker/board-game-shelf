# Internationalisierung und Store-Vorschau

BrettspielRegal unterstützt de-DE, en-US, es-ES, fr-FR, it-IT und pt-BR.
Der Produktname und die App-Kennung bleiben erhalten. Die GitHub-Bezeichnung
lautet weiterhin `board-game-shelf`. Die Sammlung hat weiterhin vier Textfelder.

## Sprachwahl und Daten

„Sprache“ oder Strg+Komma öffnet die Sprachwahl. Automatisch verwendet die Sprache
des Systems und ordnet regionale Varianten einer der sechs unterstützten
Sprachfassungen zu. Für andere Systemsprachen wird Englisch verwendet.
Eine ausdrückliche Auswahl gilt ab dem nächsten App-Start.

`language.txt` liegt neben `brettspielregal.sqlite3` im bestehenden
Datenverzeichnis. Auch `--data-dir` isoliert beide Dateien. Änderungen werden
im vorhandenen Worker über `QSaveFile` atomar gespeichert. Direkte Schreibzugriffe
als Ersatz für die atomare Ersetzung sind nicht aktiviert. Bei einem Fehler
bleibt die Auswahl im Dialog erhalten und kann erneut gespeichert werden.
Eine nicht lesbare oder unbekannte Einstellung erzeugt beim Start einen Hinweis
und verwendet Englisch. Die App überschreibt sie nicht ohne eine neue Auswahl.

Die Sprachwahl verändert keine Spielnamen, Notizen, IDs oder SQLite-Strukturen.
Suchnormalisierung bleibt Unicode-basiert. Sortierung und Zahlen verwenden die
aktive `QLocale`. Die Zählanzeige verwendet eine Beschriftung mit zwei Zahlen,
sodass kein falscher Singular aus einer zusammengesetzten Mengenangabe entsteht.

## Kataloge und Build

81 eigene Meldungen stehen in `translations/app_<locale>.ts`. Dazu gehören
Formulare, Bestätigungen, Speicherfehler, Hinweise und Kommandozeilenbeschreibungen.
Der deutsche Bestand bleibt die Quellsprache. Alle sechs Zielkataloge sind
vollständig und werden mit Qt LinguistTools in QM-Dateien übersetzt.

Die App-Kataloge und die fünf benötigten Qt-Basiskataloge sind als Ressourcen
in die Anwendung eingebettet. Englische Qt-Standardtexte sind bereits in Qt
enthalten. Damit funktionieren Text-Kontextmenüs auch ohne externes Qt-SDK.
Übersetzer werden vor Fenstern und Worker installiert und während einer Sitzung
nicht ausgetauscht. Lizenztexte und technische Diagnoseanhänge bleiben Originaltexte.

Neue Texte müssen `QCoreApplication::translate("App", ...)` verwenden.
`lupdate src -ts translations/app_*.ts -no-obsolete` kann die Kataloge aktualisieren.
Alle Übersetzungen anschließend ergänzen und prüfen:

```bash
source scripts/env.sh
python3 scripts/check_translations.py
bash scripts/check.sh release
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 bash scripts/check.sh asan
ruff check scripts
ruff format --check scripts
```

Der Katalogtest ist Teil von CTest. Er kontrolliert vollständige Schlüssel,
Platzhalter, Zeilenumbrüche, Tastatur-Mnemonics, Store-Texte und Beispieldaten.
Grundlage sind die offiziellen Anleitungen zu
[Qt-Übersetzungen](https://doc.qt.io/qt-6.8/internationalization.html) und
[qt_add_lrelease](https://doc.qt.io/qt-6.8/qtlinguist-cmake-qt-add-lrelease.html).

## Echte Screenshots und Ordnerausgabe

`store-listings/<locale>` enthält direkt kopierbare Felder ohne Aufzählungszeichen.
`assets/store_demo/<locale>.json` enthält ausschließlich erfundene Spiele und
Verlage. Diese Dateien werden nur für Tests verwendet und nicht beim normalen
Start in eine Sammlung eingefügt.

Der Linux-Aufnahmehelfer benötigt Python 3, Xvfb, Openbox, xdotool, xsel, xprop
und ImageMagick. Er erzeugt einen eigenen Bildschirm mit 1920 × 1080 Pixeln.
Der vorhandene Desktop wird nicht benutzt. Maus und Tastatur bedienen die echte
Anwendung. Die PNG-Dateien werden direkt vom Bildschirm aufgenommen.

Zunächst einen Release-Build und einen neuen Laufzeitordner erstellen.
Vorhandene Ausgaben werden nicht ersetzt. `--stage-only` verhindert ein Archiv.
Falls libxcb-cursor nicht systemweit vorhanden ist, seine Bibliothek und den
zugehörigen Lizenztext über `--cursor-library` und `--cursor-license` angeben.

```bash
source scripts/env.sh
bash scripts/check.sh release
python3 scripts/package_linux.py --qt "$BRETTSPIELREGAL_QT_ROOT" \
  --stage-only --output-dir build/staging/international-new
python3 scripts/store_assets/capture.py \
  --binary build/staging/international-new/brettspielregal-0.1.0-linux-x64/BrettspielRegal.sh \
  --output build/international-captures-new --clean-environment
```

Bei nicht systemweit installierten Testwerkzeugen können `--xvfb` und
`--window-manager` auf die jeweiligen Programme oder Openbox-Wrapper zeigen.
Ein einzelner Probelauf ist mit `--language de-DE` möglich. Für die Lieferung
sind alle sechs Sprachen erforderlich.

Pro Sprache entstehen vier Ansichten: Sammlung, Bearbeitung, Suche und Sprachwahl.
Weitere Fehler- und Neustartaufnahmen liegen getrennt unter `verification`.
Der Test prüft echte SQLite-Sperren, Wiederholung, Unicode-CRUD, sichere
Bestätigungen, kleine Fenster, Einstellungsfehler und den Prozessneustart in
der nächsten Sprache. Die Daten werden unabhängig aus SQLite gelesen.

Nach erfolgreichen Prüfungen, Sichtkontrolle und lokalem Commit:

```bash
python3 scripts/store_assets/assemble.py --capture build/international-captures-new \
  --check-log build/international-release-check.log \
  --check-log build/international-asan-check.log
```

Die Ausgabe liegt unter `build/releases/international/` und trägt die Commit-ID
im Namen. Sie enthält `Store/<locale>/text`, `Store/<locale>/screenshots`,
`Branding`, `Verification`, ein Manifest und SHA-256-Prüfsummen.
Der Helfer erstellt ausschließlich einen Ordner, keine ZIP-Datei.
Ein schmutziger Arbeitsbaum, veraltete Quellnachweise oder veränderte Aufnahmen
verhindern die Ausgabe.

## Grenze zur Windows-Veröffentlichung

Die 24 Bilder sind echte Linux-Vorschauen. Die endgültigen Store-Bilder werden
mit dem geprüften Windows-Paket neu aufgenommen. Windows-Paketierung, finale
Paket-Sprachdeklarationen, Signierung, Identität im Partner Center und native
Store-Abnahme sind separate Schritte. Der bestehende MSIX-Entwurf ist noch
kein international abgenommenes Store-Paket.

Das Branding enthält ein 300 × 300 Pixel großes Store-Symbol sowie die bereits
vorhandenen SVG-, PNG-, ICO- und Windows-Kachelvarianten. Optionales Hero-Artwork
ist nicht enthalten. Vor Einreichung die dann gültigen
[Microsoft-Vorgaben für Bilder](https://learn.microsoft.com/en-us/windows/apps/publish/publish-your-app/msix/screenshots-and-images)
und die konkreten Felder im Partner Center prüfen.
