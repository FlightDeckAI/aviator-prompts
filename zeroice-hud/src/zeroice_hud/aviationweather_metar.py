from __future__ import annotations

import requests

from zeroice_hud.models import MetarData
from zeroice_hud.util import BackoffState, RateLimiter, utc_now

METAR_URL = "https://aviationweather.gov/api/data/metar"
STATION_URL = "https://aviationweather.gov/api/data/stationinfo"
USER_AGENT = "zeroice-hud/0.1 (+https://example.local)"


class MetarClient:
    def __init__(self, min_interval_s: int = 60) -> None:
        self._limiter = RateLimiter(min_interval_s)
        self._backoff = BackoffState(base_delay=min_interval_s, max_delay=600)
        self._last_metar: MetarData | None = None
        self._last_station_coords: dict[str, tuple[float, float]] = {}

    def fetch_metar(self, icao: str) -> MetarData:
        icao = icao.strip().upper()
        if not self._limiter.ready():
            return self._mark_stale(self._last_metar, icao)

        params = {"ids": icao, "format": "json"}
        headers = {"User-Agent": USER_AGENT}
        try:
            response = requests.get(METAR_URL, params=params, headers=headers, timeout=10)
            if response.status_code == 429:
                self._backoff.next_delay()
                self._limiter.mark()
                return self._mark_rate_limited(self._last_metar, icao)
            response.raise_for_status()
        except requests.RequestException:
            self._limiter.mark()
            return self._mark_stale(self._last_metar, icao)

        self._backoff.reset()
        self._limiter.mark()
        payload = response.json()
        metar = self._parse_payload(icao, payload)
        self._last_metar = metar
        return metar

    def get_station_coords(self, icao: str) -> tuple[float, float] | None:
        icao = icao.strip().upper()
        if icao in self._last_station_coords:
            return self._last_station_coords[icao]

        params = {"ids": icao, "format": "json"}
        headers = {"User-Agent": USER_AGENT}
        try:
            response = requests.get(STATION_URL, params=params, headers=headers, timeout=10)
            response.raise_for_status()
        except requests.RequestException:
            return None
        payload = response.json()
        if not payload:
            return None
        station = payload[0]
        lat = station.get("lat")
        lon = station.get("lon")
        if lat is None or lon is None:
            return None
        coords = (float(lat), float(lon))
        self._last_station_coords[icao] = coords
        return coords

    def _parse_payload(self, icao: str, payload: list[dict]) -> MetarData:
        if not payload:
            return MetarData(icao=icao, raw_text="No METAR", stale=True)
        entry = payload[0]
        return MetarData(
            icao=icao,
            raw_text=entry.get("rawOb") or entry.get("raw_text") or "",
            observation_time=entry.get("obsTime") or entry.get("observation_time"),
            wind=entry.get("wdir") and entry.get("wspd")
            and f"{int(entry.get('wdir')):03d}/{int(entry.get('wspd')):02d}KT"
            or entry.get("wind"),
            visibility=entry.get("visib") or entry.get("visibility"),
            temp_dewpoint_c=entry.get("temp")
            and entry.get("dewp")
            and f"{entry.get('temp')}/{entry.get('dewp')}"
            or entry.get("temp_dewpoint_c"),
            altimeter=entry.get("altim") or entry.get("altimeter"),
            flight_category=entry.get("fltCat") or entry.get("flight_category"),
            stale=False,
            rate_limited=False,
        )

    def _mark_stale(self, metar: MetarData | None, icao: str) -> MetarData:
        if metar is None:
            return MetarData(
                icao=icao,
                raw_text="METAR unavailable",
                observation_time=utc_now(),
                stale=True,
            )
        return metar.copy(update={"stale": True, "rate_limited": False})

    def _mark_rate_limited(self, metar: MetarData | None, icao: str) -> MetarData:
        if metar is None:
            return MetarData(
                icao=icao,
                raw_text="METAR rate limited",
                observation_time=utc_now(),
                rate_limited=True,
            )
        return metar.copy(update={"rate_limited": True, "stale": True})
