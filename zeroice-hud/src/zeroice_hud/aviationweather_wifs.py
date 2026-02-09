from __future__ import annotations

from dataclasses import dataclass
from typing import Optional

import requests

from zeroice_hud.util import BackoffState, RateLimiter

WIFS_URL = "https://aviationweather.gov/api/data/wifs"
USER_AGENT = "zeroice-hud/0.1 (+https://example.local)"


@dataclass
class WifsIcing:
    severity: Optional[str] = None
    probability: Optional[str] = None
    description: Optional[str] = None


class WifsClient:
    def __init__(self, api_key: str, min_interval_s: int = 300) -> None:
        self._api_key = api_key
        self._limiter = RateLimiter(min_interval_s)
        self._backoff = BackoffState(base_delay=min_interval_s, max_delay=1200)

    def fetch_icing(
        self, lat: float, lon: float, flight_level: str
    ) -> Optional[WifsIcing]:
        if not self._limiter.ready():
            return None
        params = {
            "lat": lat,
            "lon": lon,
            "flight_level": flight_level,
        }
        headers = {
            "User-Agent": USER_AGENT,
            "X-API-Key": self._api_key,
        }
        try:
            response = requests.get(WIFS_URL, params=params, headers=headers, timeout=10)
            if response.status_code == 429:
                self._backoff.next_delay()
                self._limiter.mark()
                return None
            response.raise_for_status()
        except requests.RequestException:
            self._limiter.mark()
            return None
        self._backoff.reset()
        self._limiter.mark()
        payload = response.json()
        return WifsIcing(
            severity=payload.get("severity"),
            probability=payload.get("probability"),
            description=payload.get("description"),
        )
