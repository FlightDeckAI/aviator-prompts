from __future__ import annotations

import sys
from datetime import datetime
from pathlib import Path

from PySide6 import QtCore, QtWidgets

from zeroice_hud.aviationweather_gairmet import GairmetClient
from zeroice_hud.aviationweather_metar import MetarClient
from zeroice_hud.aviationweather_wifs import WifsClient
from zeroice_hud.hud_overlay import HudOverlay
from zeroice_hud.models import HudStatus, Settings
from zeroice_hud.util import StatusServer, utc_now
from zeroice_hud.video_player import VideoPlayer


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("ZeroIce HUD")
        self.resize(1280, 720)

        self.settings = Settings()
        self.metar_client = MetarClient(min_interval_s=self.settings.metar_interval_s)
        self.gairmet_client = GairmetClient(min_interval_s=self.settings.icing_interval_s)
        self.wifs_client: WifsClient | None = None

        self.status = HudStatus()
        self.status_server = StatusServer("127.0.0.1", 8787, self._status_payload)
        self.status_server.start()

        self.video_widget = VideoPlayer(self)
        self.hud_overlay = HudOverlay(self)

        container = QtWidgets.QStackedLayout()
        video_container = QtWidgets.QWidget()
        video_container.setLayout(container)
        container.addWidget(self.video_widget)
        container.addWidget(self.hud_overlay)

        central = QtWidgets.QWidget()
        central_layout = QtWidgets.QVBoxLayout(central)
        central_layout.setContentsMargins(0, 0, 0, 0)
        central_layout.addWidget(video_container)
        self.setCentralWidget(central)

        self._build_settings_panel()
        self._setup_timers()
        self._start_video()

    def _build_settings_panel(self) -> None:
        dock = QtWidgets.QDockWidget("Settings", self)
        dock.setAllowedAreas(QtCore.Qt.DockWidgetArea.LeftDockWidgetArea)
        panel = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(panel)

        self.stream_input = QtWidgets.QLineEdit(self.settings.stream_url)
        self.icao_input = QtWidgets.QLineEdit(self.settings.icao_station)
        self.lat_input = QtWidgets.QLineEdit("")
        self.lon_input = QtWidgets.QLineEdit("")
        self.metar_interval_input = QtWidgets.QSpinBox()
        self.metar_interval_input.setMinimum(60)
        self.metar_interval_input.setValue(self.settings.metar_interval_s)
        self.icing_interval_input = QtWidgets.QSpinBox()
        self.icing_interval_input.setMinimum(300)
        self.icing_interval_input.setValue(self.settings.icing_interval_s)
        self.units_combo = QtWidgets.QComboBox()
        self.units_combo.addItems(["ft", "m"])
        self.units_combo.setCurrentText(self.settings.freezing_level_units)

        self.wifs_toggle = QtWidgets.QCheckBox("Enable WIFS (requires key)")
        self.wifs_key_input = QtWidgets.QLineEdit("")
        self.wifs_key_input.setEchoMode(QtWidgets.QLineEdit.EchoMode.Password)
        self.wifs_level_input = QtWidgets.QLineEdit(self.settings.wifs_flight_level)

        apply_button = QtWidgets.QPushButton("Apply")
        apply_button.clicked.connect(self._apply_settings)

        layout.addRow("Stream URL", self.stream_input)
        layout.addRow("ICAO", self.icao_input)
        layout.addRow("Lat override", self.lat_input)
        layout.addRow("Lon override", self.lon_input)
        layout.addRow("METAR interval (s)", self.metar_interval_input)
        layout.addRow("Icing interval (s)", self.icing_interval_input)
        layout.addRow("Freezing level units", self.units_combo)
        layout.addRow(self.wifs_toggle)
        layout.addRow("WIFS API key", self.wifs_key_input)
        layout.addRow("WIFS flight level", self.wifs_level_input)
        layout.addRow(apply_button)

        dock.setWidget(panel)
        self.addDockWidget(QtCore.Qt.DockWidgetArea.LeftDockWidgetArea, dock)

    def _setup_timers(self) -> None:
        self.metar_timer = QtCore.QTimer(self)
        self.metar_timer.timeout.connect(self._update_metar)
        self.metar_timer.start(self.settings.metar_interval_s * 1000)

        self.icing_timer = QtCore.QTimer(self)
        self.icing_timer.timeout.connect(self._update_icing)
        self.icing_timer.start(self.settings.icing_interval_s * 1000)

        self.hud_timer = QtCore.QTimer(self)
        self.hud_timer.timeout.connect(self._refresh_hud)
        self.hud_timer.start(1000)

        self._update_metar()
        self._update_icing()
        self._refresh_hud()

    def _apply_settings(self) -> None:
        self.settings.stream_url = self.stream_input.text().strip()
        self.settings.icao_station = self.icao_input.text().strip().upper() or "KDEN"
        self.settings.metar_interval_s = self.metar_interval_input.value()
        self.settings.icing_interval_s = self.icing_interval_input.value()
        self.settings.freezing_level_units = self.units_combo.currentText()
        self.settings.wifs_enabled = self.wifs_toggle.isChecked()
        self.settings.wifs_api_key = self.wifs_key_input.text().strip() or None
        self.settings.wifs_flight_level = self.wifs_level_input.text().strip() or "FL060"

        lat_text = self.lat_input.text().strip()
        lon_text = self.lon_input.text().strip()
        self.settings.lat = float(lat_text) if lat_text else None
        self.settings.lon = float(lon_text) if lon_text else None

        self.metar_client = MetarClient(min_interval_s=self.settings.metar_interval_s)
        self.gairmet_client = GairmetClient(min_interval_s=self.settings.icing_interval_s)
        if self.settings.wifs_enabled and self.settings.wifs_api_key:
            self.wifs_client = WifsClient(
                api_key=self.settings.wifs_api_key,
                min_interval_s=self.settings.icing_interval_s,
            )
        else:
            self.wifs_client = None

        self.metar_timer.setInterval(self.settings.metar_interval_s * 1000)
        self.icing_timer.setInterval(self.settings.icing_interval_s * 1000)
        self._start_video()

    def _start_video(self) -> None:
        demo_path = Path(__file__).resolve().parent / "assets" / "sample.mp4"
        self.video_widget.play(self.settings.stream_url, demo_path)

    def _update_metar(self) -> None:
        icao = self.settings.icao_station
        metar = self.metar_client.fetch_metar(icao)
        self.status.metar = metar

    def _update_icing(self) -> None:
        lat = self.settings.lat
        lon = self.settings.lon
        if lat is None or lon is None:
            coords = self.metar_client.get_station_coords(self.settings.icao_station)
            if coords:
                lat, lon = coords
        icing = self.gairmet_client.fetch_icing(lat, lon)
        icing = self._apply_units(icing)

        if self.wifs_client and lat is not None and lon is not None:
            wifs = self.wifs_client.fetch_icing(lat, lon, self.settings.wifs_flight_level)
            if wifs and wifs.severity:
                detail_parts = [icing.details or ""]
                detail_parts.append(
                    f"WIFS {self.settings.wifs_flight_level}: {wifs.severity}"
                )
                if wifs.probability:
                    detail_parts.append(f"Prob {wifs.probability}")
                icing.details = " | ".join(part for part in detail_parts if part)
        self.status.icing = icing

    def _apply_units(self, icing):
        if icing.fzlvl_ft_msl is None:
            return icing
        if self.settings.freezing_level_units == "m":
            meters = int(icing.fzlvl_ft_msl * 0.3048)
            icing.fzlvl_ft_msl = meters
        return icing

    def _refresh_hud(self) -> None:
        self.status.updated_at = utc_now()
        self.hud_overlay.update_hud(self.status)

    def _status_payload(self) -> dict:
        payload = self.status.model_dump()
        payload["updated_at"] = (
            self.status.updated_at.isoformat() if self.status.updated_at else None
        )
        return payload


def main() -> None:
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
