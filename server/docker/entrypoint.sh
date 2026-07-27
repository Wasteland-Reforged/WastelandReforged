#!/usr/bin/env bash
set -euo pipefail

APPID=1874900   # "Arma Reforger Server" - verify on SteamDB before relying on this

echo "==> Updating/installing Arma Reforger dedicated server via SteamCMD"
/steamcmd/steamcmd.sh \
    +force_install_dir /server \
    +login anonymous \
    +app_update "$APPID" validate \
    +quit

echo "==> Launching server"
exec /server/ArmaReforgerServer \
    -config /config/serverConfig.json \
    -profile /profile \
    -maxFPS 60 \
    -logStats 60
