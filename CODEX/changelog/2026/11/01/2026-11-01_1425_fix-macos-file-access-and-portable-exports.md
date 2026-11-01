# Fix macOS file access and portable exports

Mac-Sandbox-Rechte für ausgewählte Dateien und Ordner ergänzt. Export auf macOS
über native Ordnerwahl und validierten Dateinamen, mit Texten in sechs Sprachen.
Neue Exporte funktionieren auch ohne Hardlinks. Bestehende Dateien bleiben
vor unbestätigtem Überschreiben geschützt. Bestätigter Ersatz bleibt atomar.
Systemsprache mit Englisch-Fallback auf Nutzerwunsch beibehalten.

Geprüft mit Formatter, strikten Compilerwarnungen, clang-tidy und neun CTest-Suiten.
Zusätzlich verweigerte native Hardlinks, Schreibfehler, Dateikollisionen und
Qt-Dialoge auf Xvfb in sechs Sprachen geprüft. Native Mac-/exFAT-Abnahme und
Grenzen des Kopierfallbacks sind in docs/verification.md dokumentiert.
Checkpoint vor Umsetzung. Abschlusscommit nach Prüfung. Kein Push oder Store-Bau.
