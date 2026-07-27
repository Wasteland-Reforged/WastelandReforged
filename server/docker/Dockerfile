FROM debian:bookworm-slim

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates curl locales lib32gcc-s1 && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -m -u 10001 -s /bin/bash reforger
RUN mkdir -p /steamcmd && \
    curl -sSL https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz \
        | tar -xz -C /steamcmd && \
    chown -R reforger:reforger /steamcmd

USER reforger
WORKDIR /home/reforger

COPY --chown=reforger:reforger entrypoint.sh /home/reforger/entrypoint.sh
RUN chmod +x /home/reforger/entrypoint.sh

ENTRYPOINT ["/home/reforger/entrypoint.sh"]
