"""Tests du module de calibration BLE (parties non-BLE)."""

import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "software" / "analysis"))

import pytest  # noqa: E402

from medistride.calibrate import (  # noqa: E402
    CMD_SET_CAL,
    _build_set_cal_payload,
    fit_all_sensors,
    linear_fit,
    read_calibration_csv,
)


def test_linear_fit_two_points():
    slope, intercept = linear_fit([(0, 0), (100, 50)])
    assert slope == pytest.approx(0.5)
    assert intercept == pytest.approx(0.0)


def test_linear_fit_with_intercept():
    # y = 2x + 3 avec un peu de bruit symétrique → on s'attend à retrouver à peu près 2 et 3
    slope, intercept = linear_fit([(0, 3), (10, 23), (20, 43), (30, 63)])
    assert slope == pytest.approx(2.0, rel=1e-9)
    assert intercept == pytest.approx(3.0, rel=1e-9)


def test_linear_fit_rejects_too_few_points():
    with pytest.raises(ValueError, match="Au moins 2 points"):
        linear_fit([(0, 0)])


def test_linear_fit_rejects_identical_x():
    with pytest.raises(ValueError, match="x sont identiques"):
        linear_fit([(5, 0), (5, 10), (5, 20)])


def test_read_calibration_csv(tmp_path):
    csv_file = tmp_path / "cal.csv"
    csv_file.write_text(
        "sensor,adc,force_n\n"
        "1,0,0\n"
        "1,1000,50\n"
        "2,0,0\n"
        "2,2000,100\n"
        "2,4000,200\n"
    )
    samples = read_calibration_csv(csv_file)
    assert set(samples.keys()) == {0, 1}  # 0-based en interne
    assert len(samples[0]) == 2
    assert len(samples[1]) == 3
    assert samples[1][2] == (4000.0, 200.0)


def test_read_calibration_csv_missing_columns(tmp_path):
    csv_file = tmp_path / "bad.csv"
    csv_file.write_text("sensor,adc\n1,100\n")
    with pytest.raises(ValueError, match="Colonnes manquantes"):
        read_calibration_csv(csv_file)


def test_read_calibration_csv_invalid_sensor_index(tmp_path):
    csv_file = tmp_path / "bad.csv"
    csv_file.write_text("sensor,adc,force_n\n7,100,50\n")
    with pytest.raises(ValueError, match="Capteur invalide"):
        read_calibration_csv(csv_file)


def test_fit_all_sensors_returns_per_sensor_coefficients():
    samples = {
        0: [(0, 0), (1000, 100)],
        2: [(0, 5), (500, 55)],
    }
    cal = fit_all_sensors(samples)
    assert set(cal.keys()) == {0, 2}
    slope, intercept = cal[0]
    assert slope == pytest.approx(0.1)
    assert intercept == pytest.approx(0.0)
    slope, intercept = cal[2]
    assert slope == pytest.approx(0.1)
    assert intercept == pytest.approx(5.0)


def test_build_set_cal_payload_layout():
    """Le payload doit être : [CMD, sensor, slope_le_f32, intercept_le_f32] = 10 octets."""
    payload = _build_set_cal_payload(3, 0.5, 2.5)
    assert len(payload) == 10
    assert payload[0] == CMD_SET_CAL
    assert payload[1] == 3
    slope_back = struct.unpack("<f", payload[2:6])[0]
    intercept_back = struct.unpack("<f", payload[6:10])[0]
    assert slope_back == pytest.approx(0.5)
    assert intercept_back == pytest.approx(2.5)
