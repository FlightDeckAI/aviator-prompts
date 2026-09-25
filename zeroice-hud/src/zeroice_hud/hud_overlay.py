from __future__ import annotations

from datetime import datetime
from typing import Optional

from PySide6 import QtCore, QtGui, QtWidgets

from zeroice_hud.models import HudStatus


class HudOverlay(QtWidgets.QFrame):
    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)
        self.setAttribute(QtCore.Qt.WidgetAttribute.WA_TransparentForMouseEvents)
        self.setAttribute(QtCore.Qt.WidgetAttribute.WA_NoSystemBackground)
        self.setStyleSheet(
            "QFrame { background-color: rgba(0, 0, 0, 120); color: #e8f7ff; "
            "border: 1px solid rgba(255, 255, 255, 60); border-radius: 6px; }"
        )
        layout = QtWidgets.QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)
        layout.setSpacing(6)

        self.title = QtWidgets.QLabel("ZEROICE HUD")
        title_font = QtGui.QFont("Sans Serif", 12, QtGui.QFont.Weight.Bold)
        self.title.setFont(title_font)

        self.metar_label = QtWidgets.QLabel("METAR: --")
        self.wind_label = QtWidgets.QLabel("WIND: --")
        self.visibility_label = QtWidgets.QLabel("VIS: --")
        self.temp_label = QtWidgets.QLabel("TEMP/DEW: --")
        self.altimeter_label = QtWidgets.QLabel("ALT: --")
        self.category_label = QtWidgets.QLabel("CATEGORY: --")
        self.icing_label = QtWidgets.QLabel("ICING: --")
        self.updated_label = QtWidgets.QLabel("HUD updated: --")
        self.disclaimer_label = QtWidgets.QLabel(
            "NOT FOR OPERATIONAL FLIGHT. SITUATIONAL AWARENESS ONLY."
        )
        self.disclaimer_label.setStyleSheet("color: #ffcc66;")

        for widget in (
            self.title,
            self.metar_label,
            self.wind_label,
            self.visibility_label,
            self.temp_label,
            self.altimeter_label,
            self.category_label,
            self.icing_label,
            self.updated_label,
            self.disclaimer_label,
        ):
            layout.addWidget(widget)

        layout.addStretch(1)

    def update_hud(self, status: HudStatus) -> None:
        metar = status.metar
        icing = status.icing
        if metar:
            stale_tag = " STALE" if metar.stale else ""
            rate_tag = " RATE LIMITED" if metar.rate_limited else ""
            self.metar_label.setText(f"METAR: {metar.raw_text}{stale_tag}{rate_tag}")
            self.wind_label.setText(f"WIND: {metar.wind or '--'}")
            self.visibility_label.setText(f"VIS: {metar.visibility or '--'}")
            self.temp_label.setText(f"TEMP/DEW: {metar.temp_dewpoint_c or '--'}")
            self.altimeter_label.setText(f"ALT: {metar.altimeter or '--'}")
            self.category_label.setText(
                f"CATEGORY: {metar.flight_category or '--'}"
            )
        else:
            self.metar_label.setText("METAR: --")

        if icing:
            stale_tag = " STALE" if icing.stale else ""
            rate_tag = " RATE LIMITED" if icing.rate_limited else ""
            details = f" - {icing.details}" if icing.details else ""
            fzlvl = (
                f" FZLVL {icing.fzlvl_ft_msl}"
                if icing.fzlvl_ft_msl is not None
                else ""
            )
            self.icing_label.setText(
                f"ICING: {icing.status}{fzlvl}{details}{stale_tag}{rate_tag}"
            )
        else:
            self.icing_label.setText("ICING: --")

        updated = status.updated_at or datetime.utcnow()
        self.updated_label.setText(
            f"HUD updated: {updated.strftime('%H:%M:%SZ')}"
        )
