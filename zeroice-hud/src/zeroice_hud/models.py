from __future__ import annotations

from datetime import datetime
from typing import Optional

from pydantic import BaseModel, Field


class MetarData(BaseModel):
    icao: str
    raw_text: str
    observation_time: Optional[datetime] = None
    wind: Optional[str] = None
    visibility: Optional[str] = None
    temp_dewpoint_c: Optional[str] = None
    altimeter: Optional[str] = None
    flight_category: Optional[str] = None
    stale: bool = False
    rate_limited: bool = False


class IcingData(BaseModel):
    status: str = "UNKNOWN"
    fzlvl_ft_msl: Optional[int] = None
    details: Optional[str] = None
    stale: bool = False
    rate_limited: bool = False


class HudStatus(BaseModel):
    metar: Optional[MetarData] = None
    icing: Optional[IcingData] = None
    updated_at: Optional[datetime] = None
    disclaimer: str = Field(
        default="NOT FOR OPERATIONAL FLIGHT. SITUATIONAL AWARENESS ONLY."
    )


class Settings(BaseModel):
    stream_url: str = ""
    icao_station: str = "KDEN"
    lat: Optional[float] = None
    lon: Optional[float] = None
    metar_interval_s: int = 60
    icing_interval_s: int = 300
    freezing_level_units: str = "ft"
    wifs_enabled: bool = False
    wifs_api_key: Optional[str] = None
    wifs_flight_level: str = "FL060"
