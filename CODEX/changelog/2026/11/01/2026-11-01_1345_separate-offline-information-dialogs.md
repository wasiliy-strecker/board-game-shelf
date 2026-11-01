# Separate offline information dialogs across six desktop apps

Auf ausdrücklichen Folgeauftrag drei Info-Einträge in Bookshelf, Yarn Box,
Knitting Projects, Sewing Planner, Costume Planner und der ersten C++-App
umgesetzt. Über die App, Datenschutz und Lizenzen haben eigene Fenster.
Menütexte und Erläuterungen sind in allen sechs Sprachen verfügbar. Bestehende
Java-Datenschutzerklärungen einschließlich Zusatzabschnitten bleiben erhalten.
Originale Lizenztexte sind unverändert offline lesbar.

Die erste C++-App heißt in allen Sprachen Board Game Shelf. Ihre neue
Datenschutzerklärung deckt lokale Spielfelder, Sprachwahl, JSON-Import/-Export,
Löschung und Kontakt ab. Der rote Schließen-Button bleibt erhalten.
Technische Kennungen, Datenpfade, Sammlungsformate und Versionen unverändert.

Prüfung: 815 Java-Tests über fünf Apps, jeweils Compiler-Lint, Enforcer,
Spotless und SpotBugs bestanden. Neue Dialogprüfungen in allen sechs Sprachen.
Fünf Linux-App-Images isoliert gebaut, Klassen/Ressourcen abgeglichen und native
Starts samt normalem Schließen geprüft. Desktop-Einträge aktualisiert, bisherige
Datenoptionen erhalten. Keine laufenden Java-Nutzerfenster geschlossen.
Ein Paketbau scheiterte zunächst an fehlendem Speicherplatz. Nach Entfernen
nur der selbst erstellten temporären Baukopien bestand der erneute Paketbau.

C++: acht CTest-Suiten, 116 Übersetzungen in sechs Sprachen, Compiler, Formatter
und clang-tidy bestanden. Zusätzlich reale xcb-Menübedienung, Dialoge und rote
Buttons in allen sechs Sprachen geprüft. Board Game Shelf mit bisheriger
Sammlung neu geöffnet. Die anderen vier C++-Apps wurden nicht geändert.

Keine persönlichen Exporte im Commit, keine Datenmigration, keine neuen
Store-Auslieferungen, kein Push oder Upload. App-lokale Prüfberichte enthalten
Befehle und Nachweise. Root- und App-Regeln halten Name und Info-Aufteilung fest.
