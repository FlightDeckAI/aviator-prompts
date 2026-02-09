from __future__ import annotations

import gzip
import io
import re
from dataclasses import dataclass
from typing import Iterable, Optional
from xml.etree import ElementTree as ET

import requests

from zeroice_hud.models import IcingData
from zeroice_hud.util import BackoffState, RateLimiter, haversine_nm, point_in_polygon, utc_now

GAIRMET_URL = "https://aviationweather.gov/data/cache/gairmets.cache.xml.gz"
USER_AGENT = "zeroice-hud/0.1 (+https://example.local)"


@dataclass
class GairmetHazard:
    hazard_type: str
    points: list[tuple[float, float]]
    raw_text: str
    freezing_level_ft: Optional[int] = None


class GairmetClient:
    def __init__(self, min_interval_s: int = 300) -> None:
        self._limiter = RateLimiter(min_interval_s)
        self._backoff = BackoffState(base_delay=min_interval_s, max_delay=1200)
        self._last_icing: IcingData | None = None

    def fetch_icing(self, lat: float | None, lon: float | None) -> IcingData:
        if not self._limiter.ready():
            return self._mark_stale(self._last_icing)

        headers = {"User-Agent": USER_AGENT}
        try:
            response = requests.get(GAIRMET_URL, headers=headers, timeout=20)
            if response.status_code == 429:
                self._backoff.next_delay()
                self._limiter.mark()
                return self._mark_rate_limited(self._last_icing)
            response.raise_for_status()
        except requests.RequestException:
            self._limiter.mark()
            return self._mark_stale(self._last_icing)

        self._backoff.reset()
        self._limiter.mark()
        hazards = self._parse_gairmet_xml(response.content)
        icing = self._evaluate_icing(hazards, lat, lon)
        self._last_icing = icing
        return icing

    def _parse_gairmet_xml(self, content: bytes) -> list[GairmetHazard]:
        try:
            decompressed = gzip.decompress(content)
        except OSError:
            decompressed = content
        tree = ET.parse(io.BytesIO(decompressed))
        root = tree.getroot()
        hazards: list[GairmetHazard] = []
        for hazard in root.findall(".//hazard"):
            hazard_type = (hazard.findtext(".//phenomenon") or "").upper()
            if "ICE" not in hazard_type and "ICING" not in hazard_type:
                continue
            points = []
            for point in hazard.findall(".//polygon/point"):
                lat = point.findtext("lat")
                lon = point.findtext("lon")
                if lat and lon:
                    points.append((float(lat), float(lon)))
            raw_text = hazard.findtext(".//raw_text") or ""
            freezing_level_ft = self._extract_freezing_level(raw_text)
            hazards.append(
                GairmetHazard(
                    hazard_type=hazard_type,
                    points=points,
                    raw_text=raw_text,
                    freezing_level_ft=freezing_level_ft,
                )
            )
        return hazards

    def _extract_freezing_level(self, text: str) -> Optional[int]:
        match = re.search(r"FZLVL\s*(\d{3})", text)
        if not match:
            return None
        return int(match.group(1)) * 100

    def _evaluate_icing(
        self, hazards: Iterable[GairmetHazard], lat: float | None, lon: float | None
    ) -> IcingData:
        hazards = list(hazards)
        if not hazards:
            return IcingData(status="NONE", details="No icing hazards found")

        fzlvl = next((h.freezing_level_ft for h in hazards if h.freezing_level_ft), None)
        if lat is None or lon is None:
            return IcingData(
                status="POSSIBLE",
                fzlvl_ft_msl=fzlvl,
                details="Active G-AIRMET icing reported",
            )

        for hazard in hazards:
            if hazard.points and point_in_polygon(lat, lon, hazard.points):
                return IcingData(
                    status="ACTIVE",
                    fzlvl_ft_msl=fzlvl,
                    details="Inside G-AIRMET icing area",
                )

        distances = [
            haversine_nm(lat, lon, point[0], point[1])
            for hazard in hazards
            for point in hazard.points
        ]
        nearest = min(distances) if distances else None
        if nearest is not None and nearest <= 200:
            return IcingData(
                status="POSSIBLE",
                fzlvl_ft_msl=fzlvl,
                details=f"G-AIRMET icing within {nearest:.0f}nm",
            )
        return IcingData(
            status="POSSIBLE",
            fzlvl_ft_msl=fzlvl,
            details="Icing hazards active in CONUS",
        )

    def _mark_stale(self, icing: IcingData | None) -> IcingData:
        if icing is None:
            return IcingData(
                status="UNKNOWN",
                details="Icing unavailable",
                stale=True,
            )
        return icing.copy(update={"stale": True, "rate_limited": False})

    def _mark_rate_limited(self, icing: IcingData | None) -> IcingData:
        if icing is None:
            return IcingData(
                status="UNKNOWN",
                details="Icing rate limited",
                rate_limited=True,
            )
        return icing.copy(update={"rate_limited": True, "stale": True})
