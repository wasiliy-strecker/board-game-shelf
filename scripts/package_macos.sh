#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
source scripts/env.sh
: "${BRETTSPIELREGAL_QT_ROOT:?Qt-6.8.3-SDK angeben}"
brettspiel_arch="$(uname -m)"
brettspiel_version="$(sed -n 's/project(BrettspielRegal VERSION \([0-9.]*\).*/\1/p' CMakeLists.txt)"
brettspiel_name="brettspielregal-$brettspiel_version-macos-$brettspiel_arch"
brettspiel_stage="$PWD/build/releases/$brettspiel_name"
brettspiel_dmg="$PWD/build/releases/$brettspiel_name.dmg"
if [[ -e "$brettspiel_stage" || -e "$brettspiel_dmg" ]]; then
  echo "Lieferung existiert bereits: $brettspiel_name" >&2
  exit 1
fi
cmake --preset release -DBUILD_TESTING=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 "-DCMAKE_OSX_ARCHITECTURES=$brettspiel_arch"
cmake --build --preset release --parallel 4
ctest --preset release
mkdir -p "$brettspiel_stage"
cp README.md THIRD_PARTY_NOTICES.md "$brettspiel_stage/"
cp -R docs "$brettspiel_stage/docs"
cp -R build/release/brettspielregal.app "$brettspiel_stage/BrettspielRegal.app"
brettspiel_app="$brettspiel_stage/BrettspielRegal.app"
"$BRETTSPIELREGAL_QT_ROOT/bin/macdeployqt" "$brettspiel_app" -always-overwrite -appstore-compliant
test -f "$brettspiel_app/Contents/PlugIns/sqldrivers/libqsqlite.dylib"
test -f "$brettspiel_app/Contents/PlugIns/platforms/libqcocoa.dylib"
mkdir -p "$brettspiel_app/Contents/Resources/translations"
cp "$BRETTSPIELREGAL_QT_ROOT/translations/qtbase_de.qm" "$brettspiel_app/Contents/Resources/translations/"
cat > "$brettspiel_app/Contents/Resources/qt.conf" <<'QTCONF'
[Paths]
Plugins=PlugIns
Translations=Resources/translations
QTCONF
cp -R licenses "$brettspiel_app/Contents/Resources/licenses"
cp -R "$BRETTSPIELREGAL_QT_ROOT/sbom" "$brettspiel_app/Contents/Resources/licenses/qt-sdk-sbom"
cp THIRD_PARTY_NOTICES.md "$brettspiel_app/Contents/Resources/"
brettspiel_icons="$PWD/build/BrettspielRegal.iconset"
mkdir -p "$brettspiel_icons"
for brettspiel_size in 16 32 128 256 512; do
  sips -z "$brettspiel_size" "$brettspiel_size" assets/branding/icon-1024.png --out "$brettspiel_icons/icon_${brettspiel_size}x${brettspiel_size}.png" >/dev/null
  brettspiel_double=$((brettspiel_size * 2))
  sips -z "$brettspiel_double" "$brettspiel_double" assets/branding/icon-1024.png --out "$brettspiel_icons/icon_${brettspiel_size}x${brettspiel_size}@2x.png" >/dev/null
done
iconutil -c icns "$brettspiel_icons" -o "$brettspiel_app/Contents/Resources/BrettspielRegal.icns"
/usr/libexec/PlistBuddy -c "Add :CFBundleIconFile string BrettspielRegal.icns" "$brettspiel_app/Contents/Info.plist"
# Ad-hoc signing permits local Apple-Silicon testing. It is not a distribution identity.
codesign --force --deep --sign - "$brettspiel_app"
hdiutil create -volname BrettspielRegal -srcfolder "$brettspiel_stage" -ov -format UDZO "$brettspiel_dmg"
shasum -a 256 "$brettspiel_dmg" > "$brettspiel_dmg.sha256"
echo "$brettspiel_dmg"
echo "Native GUI-Abnahme und spätere Store-Signierung bleiben erforderlich."
