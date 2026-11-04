# Paketierung und spätere Store-Auslieferung

Ziel ist zuerst der Microsoft Store, später der Mac App Store. Die lokalen
Pakete sind Testlieferungen. Ein Paketformat allein belegt weder Lizenzkonformität
noch bestandene Store-Zertifizierung.

## Gemeinsame Regeln

Qt 6.8.3 wird dynamisch eingebunden. Qt Core, Gui, Widgets und Sql stammen aus
derselben SDK-Installation wie die Plattform- und SQLite-Plugins. Ein Paket
enthält auch `qtbase_de.qm`. Qt Test wird nicht ausgeliefert. Die App benötigt
keine separate Qt-Installation, keinen Compiler und kein Konto.

Vor Paketierung müssen Warnungen, Formatter und Tests bestehen. Release-Builds
verwenden dieselben Quellen. Paketnamen enthalten Version, Betriebssystem und
Architektur. Vorhandene Lieferverzeichnisse und Archive werden nicht ersetzt.
Noch nicht geteilte Zwischenlieferungen können zur weiteren Prüfung separat
archiviert werden. Bereits ausgelieferte Versionen bleiben unverändert erhalten.

Die LGPL-Wahl wurde ausdrücklich bestätigt. Lizenztexte und Quellenherkunft
liegen unter `licenses`, Hinweise zusätzlich im Paket und unter „Info“ → „Lizenzen“.
Die Qt-Basisquelle ist lokal mit geprüftem SHA-256 verfügbar. Vor Veröffentlichung
müssen passende Qt-Quellen und Bauhinweise zuverlässig angeboten werden.
Bibliotheksaustausch und Untersuchung geänderter Bibliotheken dürfen nicht durch
App-Bedingungen ausgeschlossen werden. Ein späterer Wechsel zur kommerziellen
Qt-Lizenz erfordert eine Klärung mit Qt und ist nicht automatisch zugesichert.

## Linux als Prüfplattform

```bash
source scripts/env.sh
cmake --preset release
cmake --build --preset release --parallel 4
ctest --preset release
python3 scripts/package_linux.py --qt "$BRETTSPIELREGAL_QT_ROOT"
```

Falls `libxcb-cursor.so.0` nicht systemweit installiert ist, zusätzlich
`--cursor-library /pfad/libxcb-cursor.so.0` und
`--cursor-license /pfad/zum/copyright` angeben. Das Paket übernimmt beide in
seine eigene Laufzeit. Es enthält Qt, ICU 73 und den QSQLITE-Treiber.
Übliche Ubuntu-Desktopbibliotheken wie glibc, X11, Fontconfig und OpenGL bleiben
Systemvoraussetzungen. Das ist ein Ubuntu-24.04-x64-Testpaket, kein universelles
Linux-Paket und kein geplanter Linux-Store-Release.

```bash
./build/releases/brettspielregal-0.1.0-linux-x64/BoardGameShelf.sh --data-dir /tmp/brettspielregal-pakettest
```

Die Paketierung prüft Bibliotheksauflösung und erstellt SHA-256-Prüfsummen.
Anschließend muss das entpackte Paket außerhalb des Quellverzeichnisses mit
bereinigten Qt-Umgebungsvariablen sichtbar getestet werden. Die geladenen
Qt-, ICU- und SQLite-Bibliotheken müssen aus dem Paket stammen.

## Windows x64

Vorgesehen sind MSVC 2022, Windows-SDK, CMake, Ninja und Qt 6.8.3 MSVC-x64.
Die Qt-Basis unterstützt Windows 10 ab 1809 und Windows 11. Der native
Kompatibilitätsnachweis dieser App steht noch aus.

```powershell
$env:BRETTSPIELREGAL_QT_ROOT = 'C:\Qt\6.8.3\msvc2022_64'
.\scripts\package_windows.ps1 -Format zip
```

Das Skript baut und testet, nutzt `windeployqt`, übernimmt QSQLITE explizit und
legt die VC143-CRT-DLLs aus `VCToolsRedistDir` app-lokal bei. Dadurch ist kein
nachzuladender Laufzeitinstaller vorgesehen. Die Nutzungsbedingungen dieser
Microsoft-Redistributables gehören zur nativen Distributionsprüfung.

Für die spätere MSIX-Erstellung werden echte Identitätswerte aus Partner Center
übergeben. Sie sind nicht mit der internen App-Kennung gleichzusetzen:

```powershell
.\scripts\package_windows.ps1 -Format msix -IdentityName '<Partner-Center-Paketname>' -Publisher '<Partner-Center-Publisher>' -PublisherDisplayName '<Anzeigename>'
```

Die Vorlage verwendet `Windows.FullTrustApplication` und `runFullTrust` für die
Win32-Desktop-App. `makeappx.exe` stammt aus dem Windows-SDK. Eine MSI-Upgrade-ID
ist für diesen gewählten Weg nicht erforderlich. Die interne Kennung bleibt
`de.appfabrik.brettspielregal`. Das Skript signiert und veröffentlicht nichts.

Vor Store-Einreichung stehen Produktregistrierung, finale Paketidentität,
Lizenzprüfung für die konkrete MSIX-Auslieferung, Metadaten, Datenschutzauskunft,
native Abnahme und Microsoft-Zertifizierung aus. Der Store signiert eingereichte
MSIX-Pakete im Veröffentlichungsprozess. Lokales Sideloading benötigt einen
eigenen Testsignierungsweg. Keine Zertifikate oder privaten Schlüssel committen.

## macOS Intel und Apple Silicon

Vorgesehen sind Xcode 15 oder neuer und das Qt-6.8.3-macOS-SDK. Deployment-Target
ist macOS 12. Für Intel und Apple Silicon wird nativ getrennt gebaut:

```bash
export BRETTSPIELREGAL_QT_ROOT=/pfad/Qt/6.8.3/macos
bash scripts/package_macos.sh
```

Das Skript erzeugt `Board Game Shelf.app`, bündelt Qt über `macdeployqt`, prüft
QSQLITE und Cocoa-Plugin und erstellt eine DMG. Das Bundle hat die Kennung
`de.appfabrik.brettspielregal` und ein eigenes Icon. Die Ad-hoc-Signatur dient
lokalen Tests und ist keine vertrauenswürdige Distributionssignatur.

`packaging/macos/app-store.entitlements` aktiviert App Sandbox sowie
`com.apple.security.files.user-selected.read-write` für die vom Nutzer
ausgewählten Importdateien und Exportordner. Der Mac-Export fragt zuerst nach
einem Ordner und dann nach dem Dateinamen. Diese Ordnerfreigabe umfasst die
temporäre Exportdatei. Der Import verwendet weiterhin die native Dateiauswahl.
Die App benötigt keinen Netzwerkzugriff und keine pauschalen Dateifreigaben.
Store-Identität, Team, Provisioning und Signierung aller Bundle-Bestandteile
müssen später auf einem Mac festgelegt und geprüft werden. Keine pauschalen
Entitlements zum Abschalten von Bibliotheksprüfungen hinzufügen.

In der Sandbox liegt der von QStandardPaths gelieferte App-Pfad innerhalb des
App-Containers, typischerweise unter
`~/Library/Containers/de.appfabrik.brettspielregal/Data/Library/Application Support/`.
Den vollständigen tatsächlich verwendeten Pfad mit „Über die App“ kontrollieren.
Normale Nutzerdaten nicht automatisch kopieren oder migrieren. `--data-dir`
ist bei Sandbox-Tests auf zugängliche Containerpfade beschränkt.

Für den Mac App Store bleiben insbesondere LGPL-Vertriebskonformität,
App-Sandbox, eingebettete Bibliotheken, Apple-Entwickleridentität,
Einreichungspaket und Review offen. Notarisierte Direktdistribution über eine
DMG ist ein anderer Auslieferungsweg und ersetzt diese Prüfung nicht.

## Native Abnahme und CI

Auf jedem Zielsystem mit einem frischen Benutzerprofil und ohne Qt-SDK prüfen:
Start, Anlegen, Bearbeiten, Unicode-Suche, Löschen, Abbruchschutz, Fehlerversuch,
Neustart, Skalierung und Tastenkürzel. Installation und Updates müssen die
Sammlung erhalten. Auch Datenpfade und Bibliotheksherkunft kontrollieren.

Der GitHub-Workflow prüft Linux, Windows und macOS. Ein manueller Workflowlauf
kann Testartefakte hochladen, erstellt aber keinen Release und reicht nichts
bei Stores ein. Er ist vorbereitet und lokal noch nicht auf GitHub ausgeführt.
Das macOS-Matrixziel deckt zunächst den angebotenen nativen Runner ab. Die zweite
Mac-Architektur bleibt eine gesonderte native Abnahme.

## Offizielle Quellen

Stand der Prüfung: 27. Oktober 2026. Vor Veröffentlichung erneut prüfen.

- [Qt 6.8 unterstützte Plattformen](https://doc.qt.io/qt-6.8/supported-platforms.html)
- [Windows-Deployment mit Qt](https://doc.qt.io/qt-6.8/windows-deployment.html)
- [macOS-Deployment mit Qt](https://doc.qt.io/qt-6.8/macos-deployment.html)
- [Qt-LGPL-Pflichten](https://www.qt.io/development/open-source-lgpl-obligations)
- [Qt-Lizenzwechsel](https://www.qt.io/faq/qt-commercial-licensing)
- [Microsoft: MSIX-Komponenten](https://learn.microsoft.com/en-us/windows/msix/desktop/desktop-to-uwp-manual-conversion)
- [Microsoft: Store und Signierung](https://learn.microsoft.com/en-us/windows/apps/publish/get-started)
- [Apple: Vorbereitung der Auslieferung](https://developer.apple.com/documentation/xcode/preparing-your-app-for-distribution)

## Internationalisierte Vorschau

App- und Qt-Basiskataloge sind seit dem 29. Oktober 2026 eingebettet.
Die Linux-Paketierung benötigt keine externe deutsche QM-Datei mehr.
`--stage-only --output-dir <neuer-pfad>` erstellt einen frischen Prüfordner ohne Archiv. Bestehende Lieferungen bleiben erhalten.
Die [Internationalisierungsanleitung](internationalization.md) beschreibt
die vollständige Prüfung und die Ausgabe von sechs Store-Textpaketen und
24 Linux-Vorschauen als Ordner. Das bisherige Windows-Manifest muss vor
der nativen Store-Abnahme noch für die endgültige Veröffentlichung angepasst werden.

## Paketprüfungen gegen Transporter 90255 und 90277, 4. November 2026

`scripts/macos_bundle.py` prüft die Kennungen in allen enthaltenen `Info.plist`-
Dateien einschließlich eingebetteter Bundles. Ungültige Kennungen, insbesondere
Unterstriche, stoppen die Paketierung. Registrierte App-IDs und fremde
Framework-Kennungen werden nicht automatisch umgeschrieben.

`prepare` ergänzt ausschließlich in der erzeugten `.app`-Kopie die Leserechte
für alle Benutzer sowie die Ausführungsrechte für Verzeichnisse und bereits
als ausführbar markierte Dateien. Daten erhalten keine Ausführungsrechte und
es werden keine zusätzlichen Schreibrechte vergeben. Ein bereits eingebettetes
Provisionierungsprofil wird damit lesbar; das private Original außerhalb der
App bleibt unverändert. Interne Framework-Symlinks bleiben erhalten, externe
oder defekte Links werden abgewiesen. Plist-, Bibliotheks- und Signaturbytes
bleiben unverändert.

Die gezielten Skript-Prüfungen benötigen weder Maven/CMake noch ein SDK,
Signierungszertifikate, Netzwerk oder einen App-Build:

```bash
python3 -B scripts/test_macos_bundle.py
```

Ein vorhandenes Bundle kann ohne Änderungen geprüft werden:

```bash
python3 -B scripts/macos_bundle.py inspect --app "/pfad/zur/App.app"
```

Diese Korrektur ist ausschließlich Quellcode-Vorbereitung. Native Paketprüfung,
Distributionssignierung und Transporter-Validierung folgen beim späteren Release.

`package_macos.sh` führt `prepare` nach dem Kopieren aller Ressourcen vor
`codesign` aus. Nach dem Signieren prüft `inspect` erneut, bevor das DMG
entsteht. Diese Qt-App enthält keine Temurin-Runtime; der konkrete Java-
Runtime-ID-Generator ist hier nicht vorhanden. Der Paketweg bleibt eine
lokale DMG-Lieferung, keine vollständige Mac-App-Store-Auslieferung.
