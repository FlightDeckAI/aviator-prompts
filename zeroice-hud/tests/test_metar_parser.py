from zeroice_hud.aviationweather_metar import MetarClient


def test_parse_metar_payload():
    payload = [
        {
            "rawOb": "KDEN 011653Z 09008KT 10SM FEW080 27/13 A3012",
            "obsTime": "2024-06-01T16:53:00Z",
            "wdir": 90,
            "wspd": 8,
            "visib": "10SM",
            "temp": 27,
            "dewp": 13,
            "altim": "A3012",
            "fltCat": "VFR",
        }
    ]
    client = MetarClient()
    metar = client._parse_payload("KDEN", payload)
    assert metar.icao == "KDEN"
    assert "KDEN" in metar.raw_text
    assert metar.wind == "090/08KT"
    assert metar.visibility == "10SM"
    assert metar.temp_dewpoint_c == "27/13"
    assert metar.altimeter == "A3012"
    assert metar.flight_category == "VFR"
