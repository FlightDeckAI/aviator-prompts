from __future__ import annotations

from pathlib import Path

from mpv import MPV
from PySide6 import QtCore, QtWidgets

from zeroice_hud.util import ensure_sample_video


class VideoPlayer(QtWidgets.QFrame):
    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)
        self.setAttribute(QtCore.Qt.WidgetAttribute.WA_NativeWindow)
        self.setAttribute(QtCore.Qt.WidgetAttribute.WA_DontCreateNativeAncestors)
        self.player = MPV(
            wid=str(int(self.winId())),
            log_handler=None,
            input_default_bindings=True,
            input_vo_keyboard=True,
            osc=False,
            config=False,
            loglevel="warn",
            hwdec="auto",
        )

    def play(self, url: str, demo_path: Path) -> None:
        if not url:
            sample_path = ensure_sample_video(demo_path)
            self.player.play(str(sample_path))
        else:
            self.player.play(url)

    def stop(self) -> None:
        self.player.stop()

    def closeEvent(self, event: QtCore.QEvent) -> None:  # noqa: N802 - Qt naming
        self.player.terminate()
        super().closeEvent(event)
