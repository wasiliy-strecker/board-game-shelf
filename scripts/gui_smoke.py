"""Exercise real X11 windows using only synthetic data in a temporary directory."""

import argparse
import json
import os
import sqlite3
import subprocess
import tempfile
import time
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--clean-environment", action="store_true")
    args = parser.parse_args()
    binary = args.binary.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    environment = dict(os.environ)
    if args.clean_environment:
        for key in list(environment):
            if key.startswith(("QT_", "QML_", "BRETTSPIELREGAL_")) or key in (
                "LD_LIBRARY_PATH",
                "CMAKE_PREFIX_PATH",
            ):
                environment.pop(key)
        environment["PATH"] = "/usr/bin:/bin"
    environment["QT_QPA_PLATFORM"] = "xcb"
    environment["QT_SCALE_FACTOR"] = "1"

    def command(*arguments):
        return subprocess.check_output(
            list(arguments), env=environment, text=True, stderr=subprocess.DEVNULL
        ).strip()

    def key(*keys):
        command("xdotool", "key", "--clearmodifiers", *keys)
        time.sleep(0.12)

    def type_text(text):
        command("xdotool", "type", "--clearmodifiers", "--delay", "2", text)
        time.sleep(0.1)

    def wait_for(predicate, timeout=6):
        end = time.monotonic() + timeout
        while time.monotonic() < end:
            try:
                value = predicate()
                if value:
                    return value
            except (subprocess.CalledProcessError, sqlite3.OperationalError):
                pass
            time.sleep(0.05)
        raise AssertionError("GUI-Zustand nicht rechtzeitig erreicht")

    with tempfile.TemporaryDirectory(prefix="brettspielregal-gui-") as temporary:
        directory = Path(temporary)
        (directory / "language.txt").write_text("de-DE\n", encoding="utf-8")
        database = directory / "brettspielregal.sqlite3"
        log = (output / "app.log").open("w")
        process = None

        def rows():
            if not database.exists():
                return None
            with sqlite3.connect(
                f"file:{database}?mode=ro", uri=True, timeout=0.1
            ) as connection:
                return connection.execute(
                    "SELECT id,name,publisher,location,note FROM games ORDER BY id"
                ).fetchall()

        def window(title):
            identifier = wait_for(
                lambda: command(
                    "xdotool",
                    "search",
                    "--onlyvisible",
                    "--pid",
                    str(process.pid),
                    "--name",
                    title,
                ).splitlines()[-1]
            )
            command("xdotool", "windowfocus", identifier)
            time.sleep(0.15)
            return identifier

        def capture(identifier, name):
            time.sleep(0.15)
            command("import", "-window", identifier, str(output / f"{name}.png"))

        def click(identifier, x, y):
            command(
                "xdotool",
                "mousemove",
                "--window",
                identifier,
                str(x),
                str(y),
                "click",
                "1",
            )
            time.sleep(0.15)

        def destructive(identifier, discard=False):
            geometry = dict(
                line.split("=", 1)
                for line in command(
                    "xdotool", "getwindowgeometry", "--shell", identifier
                ).splitlines()
            )
            click(
                identifier,
                int(geometry["WIDTH"]) - (198 if discard else 140),
                int(geometry["HEIGHT"]) - 24,
            )

        def start():
            nonlocal process
            process = subprocess.Popen(
                [str(binary), "--data-dir", str(directory)],
                cwd=directory,
                env=environment,
                stdout=log,
                stderr=log,
            )
            main_window = window("^Board Game Shelf$")
            command("xdotool", "windowfocus", main_window)
            wait_for(lambda: rows() is not None)
            time.sleep(0.25)
            return main_window

        def stop():
            command("xdotool", "windowfocus", main_window)
            key("ctrl+q")
            process.wait(timeout=5)
            assert process.returncode == 0

        try:
            main_window = start()
            assert rows() == []
            capture(main_window, "01-empty")
            key("ctrl+n")
            editor = window("^Spiel hinzufügen$")
            key("ctrl+s")
            assert rows() == []
            capture(editor, "02-validation")
            key("alt+s")
            type_text("Azul")
            key("Tab")
            type_text("Musterverlag")
            key("Tab")
            type_text("Wohnzimmer, Regal 2")
            key("Tab")
            type_text("Erweiterung liegt in derselben Schachtel.")
            key("Return")
            type_text("Deutsche Anleitung im Deckel.")
            capture(editor, "03-draft")
            key("ctrl+s")
            wait_for(lambda: len(rows()) == 1)
            original = rows()[0]
            assert original[1:] == (
                "Azul",
                "Musterverlag",
                "Wohnzimmer, Regal 2",
                "Erweiterung liegt in derselben Schachtel.\nDeutsche Anleitung im Deckel.",
            )
            capture(main_window, "04-saved")
            key("ctrl+f")
            type_text("ANLEITUNG")
            capture(main_window, "05-search-note")
            key("ctrl+a")
            type_text("unauffindbar")
            capture(main_window, "06-no-results")
            key("ctrl+a", "BackSpace")
            key("ctrl+f", "Tab", "Return")
            editor = window("^Spiel bearbeiten$")
            key("alt+n", "ctrl+a")
            type_text("Ungespeicherter Entwurf")
            key("Escape")
            confirmation = window("Änderungen verwerfen")
            capture(confirmation, "07-safe-default")
            key("Return")
            window("^Spiel bearbeiten$")
            key("Escape")
            confirmation = window("Änderungen verwerfen")
            destructive(confirmation, discard=True)
            assert rows()[0] == original
            key("ctrl+f", "Tab", "Return")
            editor = window("^Spiel bearbeiten$")
            key("alt+a", "ctrl+a")
            type_text("Arbeitszimmer, Regal 3")
            with sqlite3.connect(database) as blocker:
                blocker.execute("BEGIN EXCLUSIVE")
                key("ctrl+s")
                time.sleep(2.2)
                window("^Spiel bearbeiten$")
                capture(editor, "08-locked")
                command("xdotool", "windowsize", editor, "400", "360")
                capture(editor, "09-small-error-dialog")
                blocker.rollback()
            assert rows()[0] == original
            key("ctrl+s")
            wait_for(lambda: rows()[0][3] == "Arbeitszimmer, Regal 3")
            assert rows()[0][0] == original[0]
            assert rows()[0][4] == original[4]
            window("^Board Game Shelf$")
            key("ctrl+n")
            editor = window("^Spiel hinzufügen$")
            key("alt+s")
            type_text("Azul")
            capture(editor, "10a-duplicate-draft")
            key("ctrl+s")
            wait_for(lambda: len(rows()) == 2)
            assert rows()[0][0] != rows()[1][0]
            capture(main_window, "10-duplicates")
            maps = Path(f"/proc/{process.pid}/maps").read_text()
            libraries = sorted(
                {
                    line.split()[-1]
                    for line in maps.splitlines()
                    if any(part in line for part in ("libQt6", "libqsqlite", "libicu"))
                }
            )
            (output / "loaded-libraries.json").write_text(
                json.dumps(libraries, indent=2) + "\n"
            )
            if args.clean_environment:
                assert all(str(binary.parent) in path for path in libraries), libraries
                assert any("libqsqlite" in path for path in libraries)
            before_restart = rows()
            stop()
            main_window = start()
            assert rows() == before_restart
            capture(main_window, "11-reopened")
            key("ctrl+f", "Tab", "Delete")
            window("Spiel löschen")
            key("Escape")
            assert rows() == before_restart
            key("ctrl+f", "Tab", "Delete")
            confirmation = window("Spiel löschen")
            capture(confirmation, "12-delete-confirmation")
            destructive(confirmation)
            wait_for(lambda: len(rows()) == 1)
            capture(main_window, "13-one-remaining")
            key("ctrl+f", "Tab", "Delete")
            confirmation = window("Spiel löschen")
            destructive(confirmation)
            wait_for(lambda: len(rows()) == 0)
            capture(main_window, "14-empty-again")
            stop()
            (output / "result.json").write_text(
                json.dumps(
                    {
                        "result": "passed",
                        "binary": str(binary),
                        "clean_environment": args.clean_environment,
                        "synthetic_data_only": True,
                        "steps": 14,
                    },
                    indent=2,
                )
                + "\n"
            )
            print(f"GUI-Prüfung bestanden: {output}")
        finally:
            if process is not None and process.poll() is None:
                process.terminate()
                process.wait(timeout=5)
            log.close()


if __name__ == "__main__":
    main()
