# zeroice-hud

ZeroIce HUD turns a Steam Deck (SteamOS Desktop Mode) into a low-latency FPV video viewer with an aviation weather + icing awareness overlay. This is for **situational awareness only** and **not for safety-of-life or operational flight decisions**.

> **Disclaimer**: This tool is **not** a certified aviation product. Do **not** use it for operational decisions. Always consult official aviation sources and follow local regulations.

## Features
- MPV/libmpv video playback (RTMP, RTSP, HLS)
- Heads-up display overlay with METAR and icing awareness
- “Zero ice” awareness: highlights possible or active icing areas
- Local status API (`http://localhost:8787/status`)
- Demo mode (plays a bundled local sample video if no stream URL is provided)

## Requirements
- Steam Deck OLED (SteamOS Desktop Mode)
- Python 3.11+
- System dependencies:
  - `mpv`
  - `libmpv` (provided by mpv package)
  - Qt (via PySide6)

On SteamOS (Arch-based), install dependencies:

```bash
sudo pacman -S --needed mpv
```

## Install

```bash
cd zeroice-hud
python -m venv .venv
source .venv/bin/activate
pip install -U pip
pip install -e .
```

## Run

```bash
zeroice-hud
```

### Example Usage
- Stream URL: `rtmp://your-drone-stream/live`
- ICAO station: `KDEN`

## Configuration
Use the settings panel to configure:
- Stream URL (RTMP/RTSP/HLS)
- ICAO station (default `KDEN`)
- Optional lat/lon override (used for icing proximity)
- Update intervals (METAR >= 60s, G-AIRMET >= 300s)
- Units (feet/meters for freezing level)

## API Sources
- METAR: `https://aviationweather.gov/api/data/metar?ids={ICAO}&format=json`
- G-AIRMET cache: `https://aviationweather.gov/data/cache/gairmets.cache.xml.gz`
- Optional WIFS icing severity (requires key)

## Local Status API
The app exposes:

```
GET http://localhost:8787/status
```

Response example:

```json
{
  "metar": {
    "icao": "KDEN",
    "raw_text": "KDEN 011653Z 09008KT 10SM FEW080 27/13 A3012",
    "observation_time": "2024-06-01T16:53:00Z",
    "wind": "090/08KT",
    "visibility": "10SM",
    "temp_dewpoint_c": "27/13",
    "altimeter": "A3012",
    "flight_category": "VFR"
  },
  "icing": {
    "status": "POSSIBLE",
    "fzlvl_ft_msl": 11000,
    "details": "G-AIRMET Zulu icing area within 200nm"
  },
  "updated_at": "2024-06-01T16:54:02Z"
}
```

## Notes on Rate Limits & Offline Use
- METAR data is polled no more than once per 60 seconds per station.
- G-AIRMET cache is refreshed no more than once per 5 minutes.
- If network is unavailable, the HUD shows last known values marked **STALE**.

## Safety Disclaimer
This software is intended for **situational awareness only**. It is **not** suitable for safety-of-life or operational flight decisions. Always use official aviation sources and comply with regulations.
