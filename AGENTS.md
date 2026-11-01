# Board Game Shelf

Eigenständige C++20-Desktop-App. Ergänzend gelten die Workspace-Regeln.
Die erste C++-Stufe bleibt eine kleine lokale Brettspielsammlung mit vier Textfeldern.
Weitere Apps, Fotos, Statuswerte, Checklisten und PDF-Export gehören nicht hierher.

## Architektur

`src/domain` enthält Werttypen und Validierung, `src/application` die Anwendungslogik
und den Repository-Vertrag, `src/persistence` Qt SQL, `src/ui` Qt Widgets.
Modell, Anwendungslogik und Speichervertrag enthalten keine Widgets.
SQL bleibt in der Persistenz. Alle Datenbankoperationen laufen im einzelnen Worker.
Eine SQL-Verbindung darf nie zwischen Threads geteilt werden.

Eigene App-Kennung und Datenpfade beibehalten. Tests verwenden ausschließlich
temporäre Datenbanken oder explizite isolierte `--data-dir`-Verzeichnisse.
Bei Entwurfsabbruch und Hauptfensterschließen gilt derselbe Verwerfschutz.
Erfolg darf erst nach Commit signalisiert werden. Fehler erhalten den Entwurf.

## Befehle

```bash
source scripts/env.sh
find src tests -type f \( -name '*.cpp' -o -name '*.h' \) -print0 | xargs -0 clang-format -i
bash scripts/check.sh
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 bash scripts/check.sh asan
bash scripts/run.sh --data-dir /absoluter/isolierter/testpfad
```

Qt, CMake und Compiler werden über die Umgebung konfiguriert. `.tools/env.sh` ist
optional und ignoriert. Keine Maschinenpfade in versionierte Skripte aufnehmen.
Kein Flutter, Maven oder gemeinsames Portfolio-Paket einführen.

## Abschluss und Auslieferung

Vor dem Commit Formatter, Warnungen, statische Prüfung und vollständige Tests
ausführen. Bei UI-Änderungen echte Fenster zusätzlich bedienen und ansehen.
Ein Paket muss mit SQLite, vollständigem CRUD-Ablauf und Neustart geprüft werden.
`--version` allein genügt nicht. Vorhandene Lieferartefakte nicht überschreiben.

Die Store-Reihenfolge ist Microsoft Store, danach Mac App Store. Kostenfreie
dynamische Qt-LGPL-Bibliotheken sind gewählt. Konkrete Store-Konformität, native
Abnahmen und Signierung bleiben vor Veröffentlichung zu klären.
Keine pauschale Store-Freigabe aus EXE, MSIX, DMG oder Lizenzwahl ableiten.

Änderungen im Workspace-Changelog festhalten. Nach erfolgreichen Prüfungen lokal
committen. Kein Push und keine Veröffentlichung ohne ausdrücklichen Auftrag.

## Internationalisierung

Sechs Sprachen beibehalten: de-DE, en-US, es-ES, fr-FR, it-IT und pt-BR.
Neue sichtbare Texte mit `QCoreApplication::translate("App", ...)` auszeichnen
und alle TS-Kataloge ergänzen. `scripts/check_translations.py` gehört zu CTest.
Qt-Basisübersetzungen bleiben eingebettet. Sprache erst beim nächsten Start
wechseln. Nutzertexte und Schema bleiben sprachunabhängig.

Store-Vorschauen mit `scripts/store_assets/capture.py` auf einem eigenen
Xvfb-Bildschirm aufnehmen. Nur synthetische Daten verwenden. Die Ausgabe von
`assemble.py` bleibt ein Ordner ohne ZIP. Linux-Vorschauen sind keine finale
Windows-Abnahme. Details stehen in `docs/internationalization.md`.

## Öffentlicher Name und Info-Menü, 1. November 2026

Board Game Shelf ist der öffentliche Name in allen sechs Sprachen. Bestehende
technische Kennungen, Datenpfade und Formate bleiben erhalten. „Info“ enthält
Über die App, Datenschutz und Lizenzen. Datenschutztexte sind Teil der
App-Kataloge. Alle mitgelieferten Lizenzdateien bleiben unverändert und werden
für das Lizenzfenster als Ressourcen eingebettet. Die roten Schließen-Buttons
in diesen drei Fenstern beibehalten.
