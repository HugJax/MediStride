"""Tests des fonctions d'analyse."""

import sys
from pathlib import Path

# Ajoute software/analysis au chemin pour permettre `import medistride`
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "software" / "analysis"))

import pandas as pd  # noqa: E402
import pytest  # noqa: E402

from medistride.metrics import (  # noqa: E402
    FOOT_CONTACT_THRESHOLD_N,
    cadence_steps_per_minute,
    detect_heel_strikes,
    symmetry_percent,
    zone_distribution,
)
from medistride.session import FSR_COLUMNS_N, Session  # noqa: E402


def make_session(left_force: list[float], right_force: list[float], dt_ms: int = 50) -> Session:
    """Construit une session synthétique avec forces totales spécifiées."""
    rows = []
    for i, f in enumerate(left_force):
        rows.append({
            "patient_id": "T-001",
            "session_start": "2026-05-12T10:00:00",
            "side": "left",
            "timestamp_ms_device": i * dt_ms,
            "received_at_iso": pd.Timestamp("2026-05-12T10:00:00", tz="UTC") + pd.Timedelta(milliseconds=i * dt_ms),
            **{c: 0 for c in [f"fsr{j}_raw" for j in range(1, 7)]},
            **{c: f / 6.0 for c in FSR_COLUMNS_N},
            "total_force_n": f,
        })
    for i, f in enumerate(right_force):
        rows.append({
            "patient_id": "T-001",
            "session_start": "2026-05-12T10:00:00",
            "side": "right",
            "timestamp_ms_device": i * dt_ms,
            "received_at_iso": pd.Timestamp("2026-05-12T10:00:00", tz="UTC") + pd.Timedelta(milliseconds=i * dt_ms),
            **{c: 0 for c in [f"fsr{j}_raw" for j in range(1, 7)]},
            **{c: f / 6.0 for c in FSR_COLUMNS_N},
            "total_force_n": f,
        })
    return Session(patient_id="T-001", start_iso="2026-05-12T10:00:00", samples=pd.DataFrame(rows))


def test_symmetry_identical_feet_is_100():
    session = make_session([100.0] * 20, [100.0] * 20)
    assert symmetry_percent(session) == pytest.approx(100.0)


def test_symmetry_zero_when_one_side_silent():
    session = make_session([100.0] * 20, [0.0] * 20)
    assert symmetry_percent(session) == pytest.approx(0.0)


def test_symmetry_returns_none_on_empty_session():
    session = make_session([], [])
    assert symmetry_percent(session) is None


def test_detect_heel_strikes_counts_transitions_above_threshold():
    forces = [0, 0, 80, 80, 0, 0, 80, 80, 0, 0]
    session = make_session(forces, [0] * 10, dt_ms=500)
    strikes = detect_heel_strikes(session.left())
    assert len(strikes) == 2  # deux montées au-dessus de FOOT_CONTACT_THRESHOLD_N


def test_detect_heel_strikes_respects_refractory():
    # Oscillation rapide : seul le premier pic doit compter.
    forces = [0, 80, 0, 80, 0, 80]
    session = make_session(forces, [0] * 6, dt_ms=50)  # 50 ms < refractory 300 ms
    strikes = detect_heel_strikes(session.left())
    assert len(strikes) == 1


def test_cadence_estimation_normal_walk():
    # 1 pas chaque 600 ms pendant 12 s ≈ 100 pas/min
    # Représenté par alternance contact / pas de contact.
    forces = []
    for _ in range(20):
        forces.extend([0, 80, 0])
    session = make_session(forces, forces, dt_ms=200)
    cad = cadence_steps_per_minute(session)
    assert cad is not None and 80 < cad < 220


def test_zone_distribution_sums_around_100():
    session = make_session([60] * 20, [60] * 20)
    zones = zone_distribution(session, "left")
    total = sum(zones.values())
    assert 99.0 <= total <= 101.0


def test_session_from_csv_missing_columns_raises(tmp_path):
    bad_csv = tmp_path / "bad.csv"
    bad_csv.write_text("foo,bar\n1,2\n")
    with pytest.raises(ValueError, match="Colonnes manquantes"):
        Session.from_csv(bad_csv)
