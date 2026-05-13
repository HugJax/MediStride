# MediStride — Analyse Python

Scripts d'analyse offline pour les fichiers CSV exportés depuis l'application web.

## Installation

```bash
cd software/analysis
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## Utilisation

### Analyser une session

```bash
python -m medistride session.csv
```

Affiche :
- résumé de la session (durée, nombre de pas) ;
- symétrie moyenne G/D ;
- cadence moyenne et écart-type ;
- répartition des forces par zone (talon, médio-pied, avant-pied) ;
- graphiques (sauvegardés en PNG à côté du CSV).

### Calibrer un module via BLE

```bash
# Vérifier les coefficients ajustés (offline) :
python -m medistride.calibrate --csv calibration_data.csv --dry-run

# Envoyer la calibration au module (BLE) :
python -m medistride.calibrate --device MediStride-L --csv calibration_data.csv

# Réinitialiser la calibration aux valeurs par défaut :
python -m medistride.calibrate --device MediStride-L --reset
```

Le CSV doit contenir trois colonnes : `sensor` (1-6), `adc`, `force_n`.
Plusieurs lignes par capteur sont acceptées : le script fait une régression
linéaire des moindres carrés par capteur, puis envoie chaque couple
(slope, intercept) via la caractéristique CONFIG (`…0005`) du firmware.

Voir [`docs/guides/calibration.md`](../../docs/guides/calibration.md) pour la
procédure complète de calibration.

## Format CSV attendu

Les fichiers exportés par l'application web ont les colonnes suivantes :

```
patient_id, session_start, side, timestamp_ms_device, received_at_iso,
fsr1_raw..fsr6_raw, fsr1_n..fsr6_n, total_force_n
```

## Tests

```bash
pytest tests/
```
