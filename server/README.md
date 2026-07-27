# Battlefield Wasteland - dedicated server config tooling

Generates a ready-to-deploy `serverConfig.json` for the Hetzner-hosted dedicated
server, from three version-controlled inputs:

| File | What it holds | Edit this when... |
|---|---|---|
| `mods.json` | The addon list (modId, name, enabled) | adding/removing/disabling a mod |
| `serverConfig.template.json` | Everything else: server name, ports, gameProperties, scenarioId | changing gameplay/server settings |
| `secrets.json` (not in git - copy from `secrets.example.json`) | Public IP, admin password, RCON password | onboarding a new machine, rotating passwords |

## Usage

```bash
cp secrets.example.json secrets.json   # first time only, then fill in real values
python generate-config.py
```

Output goes to `dist/serverConfig.json` (gitignored). Copy its contents into
`~/reforger-server/config/serverConfig.json` on the server yourself (nano + paste) -
this repo has no SSH access to the server by design, deploys are always manual.

After updating the config on the server:
- Only `serverConfig.json` changed → `docker compose restart reforger`
- `docker/` files changed too → `docker compose up -d --build`

## Gotchas learned the hard way

- **`58D0FB3206B6F859` ("Arma Reforger") is the base game, not a mod** - never add it
  to `mods.json`. `generate-config.py` will refuse to generate if it's present.
- **`scenarioId` format**: `"{GUID}Missions/Name.conf"`. The GUID is the resource's
  own GUID (extractable from `resourceDatabase.rdb` - see the RDB GUID technique in
  project memory), not the addon's top-level GUID and not the World's GUID.
  `_meta.scenarioIdVerified` in the template tracks whether this has been confirmed
  against the "Scenarios" tab on the BFW Reforged Worthy Islands Workshop page -
  flip it to `true` once you've checked.
- **RCON stays bound to `127.0.0.1`** in `docker-compose.yml` even though the Hetzner
  Robot firewall permits external tcp/19999 - don't change the bind address to `0.0.0.0`.
- **Reserved ports on this box, never reuse**: 80, 443, 5000, 5001 (chatbot/nginx),
  8502 (a second "chatbot-demo" service). Game ports 2001/udp + 17777/udp are already
  allowed in the Hetzner Robot firewall panel (robot.hetzner.com -> server -> Firewall) -
  if you ever change `bindPort`/`a2s.port`, add a new rule there too, an OS firewall
  rule alone is not enough (Hetzner filters at the network level before traffic reaches
  the box; ufw is intentionally left inactive).
- **SteamCMD APPID `1874900`** for "Arma Reforger Server" - Bohemia has changed this
  before; verify on SteamDB if the container fails to update after a game patch.
- The server logs in as **root** on Ubuntu 24.04 (Noble) - no separate sudo user exists.
