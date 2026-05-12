"""Chargement et structure d'une session MediStride."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

import pandas as pd


FSR_COLUMNS_N = [f"fsr{i}_n" for i in range(1, 7)]
FSR_COLUMNS_RAW = [f"fsr{i}_raw" for i in range(1, 7)]


@dataclass
class Session:
    """Une session de marche, contenant les échantillons des deux pieds."""

    patient_id: str
    start_iso: str
    samples: pd.DataFrame  # une ligne par échantillon, colonne `side` ∈ {left, right}

    @classmethod
    def from_csv(cls, path: Path) -> "Session":
        df = pd.read_csv(path)
        required = {"side", "timestamp_ms_device", "total_force_n", *FSR_COLUMNS_N}
        missing = required - set(df.columns)
        if missing:
            raise ValueError(f"Colonnes manquantes dans le CSV : {missing}")

        patient_id = str(df["patient_id"].iloc[0]) if "patient_id" in df else "?"
        start_iso = str(df["session_start"].iloc[0]) if "session_start" in df else ""
        return cls(patient_id=patient_id, start_iso=start_iso, samples=df)

    def left(self) -> pd.DataFrame:
        if "side" not in self.samples.columns:
            return self.samples.iloc[0:0]
        return self.samples[self.samples["side"] == "left"]

    def right(self) -> pd.DataFrame:
        if "side" not in self.samples.columns:
            return self.samples.iloc[0:0]
        return self.samples[self.samples["side"] == "right"]

    def duration_sec(self) -> float:
        if self.samples.empty or "timestamp_ms_device" not in self.samples.columns:
            return 0.0
        ts = self.samples["timestamp_ms_device"]
        return (ts.max() - ts.min()) / 1000.0
