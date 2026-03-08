<div align="center">

# ratdeck

**Encrypted mesh messenger for the LilyGo T-Deck Plus**

<table><tr>
<td><img src="assets/card3.JPG" width="100%" alt="RatDeck booting up, showing the Ratspeak splash screen"></td>
<td><img src="assets/card4.JPG" width="100%" alt="RatDeck running, showing the home screen with tabs"></td>
</tr></table>

A standalone Reticulum node with a keyboard, trackball, and screen — no computer required.

</div>

---

RatDeck turns a [LilyGo T-Deck Plus](https://www.lilygo.cc/products/t-deck-plus) into a fully self-contained [Reticulum](https://reticulum.network/) mesh node. It's not an RNode and it's not a gateway — it's a complete encrypted communicator you carry with you.

End-to-end encrypted [LXMF](https://github.com/markqvist/LXMF) messaging over LoRa, WiFi bridging to the wider Reticulum network, node discovery, multiple identities, transport relay mode, and a configurable SX1262 radio — all through an LVGL touchscreen UI with a physical QWERTY keyboard and trackball.

## Get one

1. Buy a **LilyGo T-Deck Plus** (~$55 — [LilyGo](https://www.lilygo.cc/), [AliExpress](https://aliexpress.com), or Amazon)
2. Attach a **915 MHz antenna** (SMA, usually included)
3. Flash the firmware

### Flash it

The easiest way is the **[web flasher](https://ratspeak.org/download.html)** — plug in USB, click flash, done.

To build from source:

```bash
git clone https://github.com/ratspeak/ratdeck
cd ratdeck
pip install platformio
python3 -m platformio run --target upload
```

First build takes a couple minutes while PlatformIO pulls the toolchain. After that it's fast.

## Using it

On first boot, RatDeck generates a Reticulum identity and shows a name input screen. Your LXMF address (32-character hex string) is what you share with contacts.

**Tabs:** Home, Friends, Msgs, Peers, Setup — navigate with the trackball or `,` / `/` keys.

**Sending a message:** Find someone in Peers, click with the trackball to open chat, type your message, hit Enter. Status goes yellow (sending) → green (delivered).

**Radio presets** (Setup → Radio):
- **Long Range** — SF12, 62.5 kHz, 22 dBm. Maximum distance, very slow.
- **Balanced** — SF9, 125 kHz, 17 dBm. Good default.
- **Fast** — SF7, 250 kHz, 14 dBm. Short range, quick transfers.

All radio parameters are individually tunable. Changes apply immediately, no reboot.

### WiFi bridging

RatDeck can bridge your laptop to the LoRa mesh:

1. Set WiFi to **AP mode** in Setup → Network (creates `ratdeck-XXXX`, password: `ratspeak`)
2. Connect your laptop to that network
3. Add to your Reticulum config:

```ini
[[ratdeck]]
  type = TCPClientInterface
  target_host = 192.168.4.1
  target_port = 4242
```

Your desktop Reticulum instance now reaches the LoRa mesh through RatDeck's radio.

Or use **STA mode** to connect to existing WiFi and reach remote nodes like `rns.ratspeak.org:4242`.

### Transport mode

Enable in Setup → Network to turn your RatDeck into a mesh relay — it'll forward packets and maintain routing tables for other nodes. Best for fixed, elevated positions with good radio coverage.

## Docs

The detailed stuff lives in [`docs/`](docs/):

- **[Quick Start](docs/QUICKSTART.md)** — build, flash, first boot, first message
- [Building](docs/BUILDING.md) — build flags, flashing, CI, partition table
- [Architecture](docs/ARCHITECTURE.md) — layer diagram, data flow, design decisions
- [Development](docs/DEVELOPMENT.md) — adding screens, settings, transports
- [Hotkeys](docs/HOTKEYS.md) — keyboard shortcuts and navigation
- [Pin Map](docs/PINMAP.md) — full T-Deck Plus GPIO assignments
- [Troubleshooting](docs/TROUBLESHOOTING.md) — radio, build, boot, storage

## License

GPL-3.0
