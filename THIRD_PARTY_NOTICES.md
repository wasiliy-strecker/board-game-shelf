# Komponenten und Lizenzhinweise

Board Game Shelf verwendet Qt 6.8.3 unter der GNU Lesser General Public License,
Version 3 (LGPL-3.0). Qt Core, Gui, Widgets und Sql werden dynamisch eingebunden.
Der QSQLITE-Treiber wird als dynamisches Plugin ausgeliefert. Qt Test wird nur
für Entwicklungsprüfungen verwendet. Die unveränderten Qt-Basisübersetzungen
für Deutsch, Spanisch, Französisch, Italienisch und brasilianisches Portugiesisch
aus Qt Translations 6.8.3 sind als Ressourcen eingebettet. Die zugehörigen
Lizenztexte liegen unter `licenses/qttranslations-6.8.3-*`.
Quellcode dieser Übersetzungen:
https://download.qt.io/archive/qt/6.8/6.8.3/submodules/qttranslations-everywhere-src-6.8.3.tar.xz

Qt ist Copyright The Qt Company Ltd. und weiterer Beitragender.
Lizenzinformationen: https://www.qt.io/development/open-source-lgpl-obligations
Quellcode der eingesetzten Qt-Basis:
https://download.qt.io/archive/qt/6.8/6.8.3/submodules/qtbase-everywhere-src-6.8.3.tar.xz

Die Dateien im Ordner licenses enthalten die Lizenztexte und Hinweise zu
mitgelieferten Fremdkomponenten. Das Mac-Store-Paket enthält die vollständigen
unveränderten Qt-Basis- und Übersetzungsquellen unter
`Contents/Resources/QtSources`, jeweils mit Herkunft und SHA-256-Prüfsumme.
`Contents/Resources/qt-library-replacement.md` beschreibt Bau und lokalen
Bibliotheksaustausch; der Helfer `replace_macos_qt.py` liegt daneben.
Die mitgelieferten SPDX-Dateien dokumentieren die Komponenten des konkreten SDK.
Diese Unterlagen sind keine pauschale Zusicherung einer Store-Zertifizierung.

Die Rechte zum Ändern, Ersetzen und erneuten Einbinden der LGPL-Bibliotheken
sowie zur Untersuchung solcher Änderungen werden nicht eingeschränkt.
Eine spätere zusätzliche Nutzungsvereinbarung darf diese Rechte nicht aufheben.

SQLite ist gemeinfrei. Weitere Komponenten innerhalb von Qt, beispielsweise
Unicode-Daten, PCRE2, HarfBuzz, FreeType, libpng und zlib, haben eigene Hinweise.
Die Paketierung übernimmt die mit dem konkreten SDK ausgelieferten Metadaten
und dokumentiert zusätzlich gebündelte Laufzeitbibliotheken.

Eigener App-Code: Copyright 2026 Wasiliy Strecker. Eine gesonderte öffentliche
Quellcodelizenz ist bisher nicht festgelegt. Diese Aussage ändert keine Rechte
an Qt oder anderen Fremdkomponenten.
