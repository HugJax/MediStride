"""Calcul des métriques de marche (symétrie, cadence, répartition par zone)."""

from __future__ import annotations

import numpy as np
import pandas as pd

from .session import FSR_COLUMNS_N, Session


FOOT_CONTACT_THRESHOLD_N = 50.0
STRIKE_REFRACTORY_MS = 300


def symmetry_percent(session: Session) -> float | None:
    """Symétrie G/D = 100 × (1 − |L−R| / (L+R)) sur les forces moyennes."""
    left_df = session.left()
    right_df = session.right()
    if "total_force_n" not in left_df.columns or "total_force_n" not in right_df.columns:
        return None
    left_mean = left_df["total_force_n"].mean()
    right_mean = right_df["total_force_n"].mean()
    if pd.isna(left_mean) or pd.isna(right_mean):
        return None
    total = left_mean + right_mean
    if total < 1.0:
        return None
    return (1.0 - abs(left_mean - right_mean) / total) * 100.0


def detect_heel_strikes(side_df: pd.DataFrame) -> list[int]:
    """Retourne les indices d'échantillons correspondant au début d'un appui."""
    if side_df.empty:
        return []
    force = side_df["total_force_n"].to_numpy()
    # Le firmware envoie un timestamp synchronisé entre G et D (voir main.cpp).
    ts_ms = side_df["timestamp_ms_device"].to_numpy()

    strikes: list[int] = []
    last_ms = -10_000
    prev = 0.0
    for i, f in enumerate(force):
        if (prev < FOOT_CONTACT_THRESHOLD_N
                and f >= FOOT_CONTACT_THRESHOLD_N
                and int(ts_ms[i]) - last_ms > STRIKE_REFRACTORY_MS):
            strikes.append(i)
            last_ms = int(ts_ms[i])
        prev = f
    return strikes


def cadence_steps_per_minute(session: Session) -> float | None:
    """Cadence moyenne sur la durée totale de la session."""
    n_left = len(detect_heel_strikes(session.left()))
    n_right = len(detect_heel_strikes(session.right()))
    total_steps = n_left + n_right
    duration = session.duration_sec()
    if total_steps < 2 or duration < 5.0:
        return None
    return total_steps * 60.0 / duration


# Zones d'analyse anatomique (indices 1-based des FSR de config.h)
ZONES = {
    "talon": ["fsr1_n", "fsr2_n"],
    "medio_pied": ["fsr3_n"],
    "avant_pied": ["fsr4_n", "fsr5_n", "fsr6_n"],
}


def zone_distribution(session: Session, side: str) -> dict[str, float]:
    """Pourcentage moyen de la force totale dans chaque zone anatomique."""
    df = session.left() if side == "left" else session.right()
    if df.empty:
        return {z: 0.0 for z in ZONES}

    totals = df[FSR_COLUMNS_N].sum(axis=1).replace(0, np.nan)
    out = {}
    for zone, cols in ZONES.items():
        zone_force = df[cols].sum(axis=1)
        pct = (zone_force / totals).mean() * 100.0
        out[zone] = 0.0 if np.isnan(pct) else float(pct)
    return out
