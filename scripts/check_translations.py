"""Check translation coverage, placeholders and copyable listing fields."""

import ast
import json
import re
from pathlib import Path
from xml.etree import ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
LANGUAGES = ("de-DE", "en-US", "es-ES", "fr-FR", "it-IT", "pt-BR")


def main():
    sources = set()
    pattern = r'QCoreApplication::translate\(\s*"App",\s*((?:"(?:[^"\\]|\\.)*"\s*)+)\)'
    for path in (ROOT / "src").rglob("*.cpp"):
        for match in re.finditer(pattern, path.read_text(encoding="utf-8")):
            sources.add(
                "".join(
                    ast.literal_eval(part)
                    for part in re.findall(r'"(?:[^"\\]|\\.)*"', match[1])
                )
            )
    assert sources
    for language in LANGUAGES:
        tree = ET.parse(ROOT / "translations" / f"app_{language}.ts")
        assert tree.getroot().get("language") == language.replace("-", "_")
        messages = tree.findall("./context/message")
        assert len(messages) == len(sources), language
        assert {m.findtext("source") for m in messages} == sources, language
        for message in messages:
            source = message.findtext("source")
            translation = message.findtext("translation")
            assert translation and message.find("translation").get("type") is None, (
                language,
                source,
            )
            assert sorted(re.findall(r"%[1-9]", source)) == sorted(
                re.findall(r"%[1-9]", translation)
            ), (language, source)
            assert source.count("\n") == translation.count("\n"), (language, source)
            assert source.count("&") == translation.count("&"), (language, source)
        listing = ROOT / "store-listings" / language
        for field, limit in (
            ("name", 50),
            ("short-description", 1000),
            ("description", 10000),
            ("features", 10000),
            ("captions", 1000),
        ):
            value = (listing / f"{field}.txt").read_text(encoding="utf-8")
            assert value.strip() and len(value) <= limit, (language, field)
            assert ";" not in value
            assert not re.search(r"(?m)^\s*(?:[-*#]|\d+[.)])\s", value), (
                language,
                field,
            )
        assert (
            len((listing / "captions.txt").read_text(encoding="utf-8").splitlines())
            == 4
        )
        fixture = json.loads(
            (ROOT / "assets/store_demo" / f"{language}.json").read_text(
                encoding="utf-8"
            )
        )
        assert len(fixture["games"]) == 5
        assert all(len(game) == 4 and all(game) for game in fixture["games"])
        assert (
            sum(
                fixture["search"].casefold() in " ".join(game).casefold()
                for game in fixture["games"]
            )
            == 1
        )
    print(f"{len(sources)} messages complete in {len(LANGUAGES)} languages")


if __name__ == "__main__":
    main()
