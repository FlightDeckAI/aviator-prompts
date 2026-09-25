from pathlib import Path

from zeroice_hud.aviationweather_gairmet import GairmetClient


def test_parse_gairmet_fixture():
    client = GairmetClient()
    fixture = Path(__file__).parent / "fixtures" / "gairmet.xml"
    hazards = client._parse_gairmet_xml(fixture.read_bytes())
    assert hazards
    hazard = hazards[0]
    assert hazard.hazard_type == "ICE"
    assert hazard.freezing_level_ft == 11000
    assert len(hazard.points) == 4
