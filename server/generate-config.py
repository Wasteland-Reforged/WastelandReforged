#!/usr/bin/env python3
"""Merge serverConfig.template.json + mods.json + secrets.json into a deployable serverConfig.json.

Usage: python generate-config.py
Output: server/dist/serverConfig.json (gitignored) - copy its contents into
        ~/reforger-server/config/serverConfig.json on the Hetzner server yourself.
"""
import json
import sys
from pathlib import Path

SERVER_DIR = Path(__file__).resolve().parent
BASE_GAME_MOD_ID = "58D0FB3206B6F859"  # "Arma Reforger" itself - never a real mod entry


def load_json(path: Path):
    if not path.exists():
        sys.exit(f"Missing required file: {path}")
    with path.open("r", encoding="utf-8") as f:
        return json.load(f)


def main():
    template = load_json(SERVER_DIR / "serverConfig.template.json")
    mods = load_json(SERVER_DIR / "mods.json")

    secrets_path = SERVER_DIR / "secrets.json"
    if not secrets_path.exists():
        sys.exit(
            f"Missing {secrets_path}.\n"
            f"Copy secrets.example.json to secrets.json and fill in real values first."
        )
    secrets = load_json(secrets_path)

    meta = template.pop("_meta", {})

    enabled_mods = [m for m in mods if m.get("enabled", True)]
    for m in enabled_mods:
        if m["modId"] == BASE_GAME_MOD_ID:
            sys.exit(
                f"mods.json contains {BASE_GAME_MOD_ID} (\"Arma Reforger\" - the base game itself). "
                f"Remove it, it is not a Workshop mod."
            )

    template["publicAddress"] = secrets["publicAddress"]
    template["rcon"]["password"] = secrets["rconPassword"]
    template["game"]["passwordAdmin"] = secrets["adminPassword"]
    template["game"]["mods"] = [
        {"modId": m["modId"], "name": m["name"]} for m in enabled_mods
    ]

    dist_dir = SERVER_DIR / "dist"
    dist_dir.mkdir(exist_ok=True)
    out_path = dist_dir / "serverConfig.json"
    with out_path.open("w", encoding="utf-8") as f:
        json.dump(template, f, indent=2)
        f.write("\n")

    print(f"Wrote {out_path}")
    print(f"Enabled mods: {len(enabled_mods)} (of {len(mods)} total in mods.json)")
    if not meta.get("scenarioIdVerified", False):
        print(
            "\nWARNING: scenarioId has NOT been marked verified in serverConfig.template.json "
            "(_meta.scenarioIdVerified is false). Double-check it against the 'Scenarios' tab "
            "on the Workshop page before deploying, then flip that flag once confirmed."
        )
    print(
        "\nNext: copy the contents of dist/serverConfig.json into "
        "~/reforger-server/config/serverConfig.json on the server (nano + paste), "
        "then `docker compose restart reforger` (or `up -d --build` if docker/ files changed)."
    )


if __name__ == "__main__":
    main()
