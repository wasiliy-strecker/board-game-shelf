# BrettspielRegal

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
