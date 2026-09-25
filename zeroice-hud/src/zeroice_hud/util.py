from __future__ import annotations

import base64
import json
import math
import threading
import time
from dataclasses import dataclass
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path
from typing import Callable, Iterable, Optional

SAMPLE_VIDEO_B64 = (
    "AAAAIGZ0eXBpc29tAAACAGlzb20yYXZjMW1wNDEAAAPYbW9vdgAAAGxtdmhkAAAA"
    "ANrG2gDaxu0AAABAAAB4AAAAGG1kYXQhEAUCBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
    "gYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGBgYGB"
)


def ensure_sample_video(path: Path) -> Path:
    path.parent.mkdir(parents=True, exist_ok=True)
    if not path.exists():
        data = base64.b64decode(SAMPLE_VIDEO_B64)
        path.write_bytes(data)
    return path


def utc_now() -> datetime:
    return datetime.now(timezone.utc)


@dataclass
class BackoffState:
    base_delay: float = 5.0
    max_delay: float = 300.0
    factor: float = 2.0
    current_delay: float = 0.0

    def next_delay(self) -> float:
        if self.current_delay == 0:
            self.current_delay = self.base_delay
        else:
            self.current_delay = min(self.current_delay * self.factor, self.max_delay)
        return self.current_delay

    def reset(self) -> None:
        self.current_delay = 0.0


def point_in_polygon(lat: float, lon: float, polygon: Iterable[tuple[float, float]]) -> bool:
    x = lon
    y = lat
    inside = False
    points = list(polygon)
    if len(points) < 3:
        return False
    for i in range(len(points)):
        j = (i - 1) % len(points)
        xi, yi = points[i][1], points[i][0]
        xj, yj = points[j][1], points[j][0]
        intersect = ((yi > y) != (yj > y)) and (
            x < (xj - xi) * (y - yi) / ((yj - yi) or 1e-9) + xi
        )
        if intersect:
            inside = not inside
    return inside


def haversine_nm(lat1: float, lon1: float, lat2: float, lon2: float) -> float:
    r_km = 6371.0
    phi1 = math.radians(lat1)
    phi2 = math.radians(lat2)
    dphi = math.radians(lat2 - lat1)
    dlambda = math.radians(lon2 - lon1)
    a = math.sin(dphi / 2) ** 2 + math.cos(phi1) * math.cos(phi2) * math.sin(dlambda / 2) ** 2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    km = r_km * c
    return km * 0.539957


class StatusServer:
    def __init__(self, host: str, port: int, status_provider: Callable[[], dict]):
        self._host = host
        self._port = port
        self._status_provider = status_provider
        self._thread: Optional[threading.Thread] = None
        self._httpd: Optional[HTTPServer] = None

    def start(self) -> None:
        if self._thread:
            return

        server = self

        class Handler(BaseHTTPRequestHandler):
            def do_GET(self) -> None:  # noqa: N802 - stdlib signature
                if self.path != "/status":
                    self.send_response(404)
                    self.end_headers()
                    return
                payload = server._status_provider()
                body = json.dumps(payload, default=str).encode("utf-8")
                self.send_response(200)
                self.send_header("Content-Type", "application/json")
                self.send_header("Content-Length", str(len(body)))
                self.end_headers()
                self.wfile.write(body)

            def log_message(self, format: str, *args: object) -> None:  # noqa: A003
                return

        self._httpd = HTTPServer((self._host, self._port), Handler)
        self._thread = threading.Thread(target=self._httpd.serve_forever, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        if not self._httpd:
            return
        self._httpd.shutdown()
        self._httpd.server_close()
        self._httpd = None
        self._thread = None


class RateLimiter:
    def __init__(self, min_interval_s: int):
        self._min_interval_s = min_interval_s
        self._last_call: float = 0.0

    def ready(self) -> bool:
        return (time.time() - self._last_call) >= self._min_interval_s

    def mark(self) -> None:
        self._last_call = time.time()
