"""Calibration BLE des capteurs FSR d'un module MediStride.

Usage typique :

    # 1. Acquérir des points (force_n, adc) par capteur dans un CSV.
    # 2. Lancer la régression linéaire et pousser les coefficients :
    python -m medistride.calibrate --device MediStride-L --csv calibration.csv

    # Vérifier les coefficients sans envoyer au module :
    python -m medistride.calibrate --csv calibration.csv --dry-run

    # Remettre la calibration aux valeurs par défaut :
    python -m medistride.calibrate --device MediStride-L --reset

Le CSV doit avoir trois colonnes : `sensor` (1 à 6), `adc`, `force_n`.
Plusieurs lignes par capteur sont acceptées (régression linéaire des moindres carrés).
"""

from __future__ import annotations

import argparse
import asyncio
import csv
import struct
import sys
from collections import defaultdict
from pathlib import Path
from typing import Iterable


# UUIDs alignés avec firmware/src/config.h
SERVICE_UUID     = "4d454449-5354-5249-4445-000000000001"
CONFIG_CHAR_UUID = "4d454449-5354-5249-4445-000000000005"

CMD_SET_CAL    = 0x02
CMD_SAVE_CAL   = 0x03
CMD_RESET_CAL  = 0x04


def linear_fit(points: Iterable[tuple[float, float]]) -> tuple[float, float]:
    """Régression linéaire y = slope*x + intercept par moindres carrés.

    Args:
        points: itérable de (x, y).

    Returns:
        (slope, intercept) en flottants.

    Raises:
        ValueError si moins de deux points.
    """
    pts = list(points)
    n = len(pts)
    if n < 2:
        raise ValueError(f"Au moins 2 points requis, {n} fourni(s).")
    sum_x = sum(x for x, _ in pts)
    sum_y = sum(y for _, y in pts)
    sum_xx = sum(x * x for x, _ in pts)
    sum_xy = sum(x * y for x, y in pts)
    denom = n * sum_xx - sum_x * sum_x
    if denom == 0:
        raise ValueError("Tous les x sont identiques, régression impossible.")
    slope = (n * sum_xy - sum_x * sum_y) / denom
    intercept = (sum_y - slope * sum_x) / n
    return float(slope), float(intercept)


def read_calibration_csv(path: Path) -> dict[int, list[tuple[float, float]]]:
    """Lit un CSV de calibration. Retourne {sensor_idx_0based: [(adc, force_n), ...]}."""
    samples: dict[int, list[tuple[float, float]]] = defaultdict(list)
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        required = {"sensor", "adc", "force_n"}
        fields = set(reader.fieldnames or [])
        if not required.issubset(fields):
            missing = required - fields
            raise ValueError(f"Colonnes manquantes dans {path}: {missing}")
        for row in reader:
            sensor = int(row["sensor"])
            if not 1 <= sensor <= 6:
                raise ValueError(f"Capteur invalide : {sensor} (attendu 1-6).")
            adc = float(row["adc"])
            force = float(row["force_n"])
            samples[sensor - 1].append((adc, force))
    return dict(samples)


def fit_all_sensors(
    samples: dict[int, list[tuple[float, float]]]
) -> dict[int, tuple[float, float]]:
    return {idx: linear_fit(points) for idx, points in sorted(samples.items())}


def _build_set_cal_payload(sensor_idx: int, slope: float, intercept: float) -> bytes:
    return struct.pack("<BBff", CMD_SET_CAL, sensor_idx, slope, intercept)


async def push_calibration(
    device_name: str,
    calibration: dict[int, tuple[float, float]],
    save: bool = True,
    scan_timeout: float = 10.0,
) -> None:
    """Connecte au module BLE et envoie les coefficients de calibration."""
    try:
        from bleak import BleakClient, BleakScanner
    except ImportError:
        raise RuntimeError(
            "Le paquet `bleak` est requis pour communiquer en BLE. "
            "Installer avec : pip install bleak"
        )

    print(f"Recherche de '{device_name}' (timeout {scan_timeout} s)…")
    device = await BleakScanner.find_device_by_name(device_name, timeout=scan_timeout)
    if device is None:
        raise RuntimeError(
            f"Module '{device_name}' introuvable. Vérifier qu'il est allumé "
            f"et à portée Bluetooth."
        )

    print(f"Connexion à {device.address}…")
    async with BleakClient(device) as client:
        for sensor_idx in sorted(calibration):
            slope, intercept = calibration[sensor_idx]
            payload = _build_set_cal_payload(sensor_idx, slope, intercept)
            await client.write_gatt_char(CONFIG_CHAR_UUID, payload, response=True)
            print(f"  Capteur {sensor_idx + 1}: slope={slope:.6g}  intercept={intercept:.6g}")

        if save:
            await client.write_gatt_char(CONFIG_CHAR_UUID, bytes([CMD_SAVE_CAL]), response=True)
            print("Calibration sauvegardée en NVS (persistante).")
        else:
            print("Calibration appliquée en RAM uniquement (non persistante).")


async def reset_calibration(device_name: str, scan_timeout: float = 10.0) -> None:
    try:
        from bleak import BleakClient, BleakScanner
    except ImportError:
        raise RuntimeError("bleak non installé : `pip install bleak`")

    print(f"Recherche de '{device_name}'…")
    device = await BleakScanner.find_device_by_name(device_name, timeout=scan_timeout)
    if device is None:
        raise RuntimeError(f"Module '{device_name}' introuvable.")

    async with BleakClient(device) as client:
        await client.write_gatt_char(CONFIG_CHAR_UUID, bytes([CMD_RESET_CAL]), response=True)
        print("Calibration remise à zéro (valeurs par défaut).")


async def _amain(args: argparse.Namespace) -> int:
    if args.reset:
        await reset_calibration(args.device)
        return 0

    if not args.csv:
        print("Erreur : --csv requis (ou --reset).", file=sys.stderr)
        return 1

    samples = read_calibration_csv(args.csv)
    if not samples:
        print(f"Aucun point trouvé dans {args.csv}.", file=sys.stderr)
        return 1

    calibration = fit_all_sensors(samples)

    print("Coefficients ajustés :")
    for sensor_idx, (slope, intercept) in sorted(calibration.items()):
        n_points = len(samples[sensor_idx])
        print(f"  Capteur {sensor_idx + 1} (n={n_points}) : "
              f"y = {slope:.4g}·adc + {intercept:.4g}")

    if args.dry_run:
        print("\n--dry-run : aucun envoi BLE effectué.")
        return 0

    await push_calibration(args.device, calibration, save=not args.no_save)
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        prog="medistride.calibrate",
        description="Calibration des capteurs FSR d'un module MediStride via BLE.",
    )
    parser.add_argument(
        "--device", default="MediStride-L",
        help="Nom BLE du module à calibrer (défaut : MediStride-L).",
    )
    parser.add_argument(
        "--csv", type=Path,
        help="CSV de données de calibration (colonnes : sensor, adc, force_n).",
    )
    parser.add_argument(
        "--reset", action="store_true",
        help="Réinitialise la calibration aux valeurs par défaut firmware.",
    )
    parser.add_argument(
        "--no-save", action="store_true",
        help="Ne pas persister en NVS — calibration valable jusqu'au reboot.",
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Calculer et afficher les coefficients sans connexion BLE.",
    )
    args = parser.parse_args(argv)

    try:
        return asyncio.run(_amain(args))
    except RuntimeError as e:
        print(f"Erreur : {e}", file=sys.stderr)
        return 2
    except KeyboardInterrupt:
        print("\nInterrompu.", file=sys.stderr)
        return 130


if __name__ == "__main__":
    sys.exit(main())
