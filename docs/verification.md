# Prüfbericht

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
