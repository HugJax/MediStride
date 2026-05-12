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
python -m medistride.calibrate --device MediStride-L --csv calibration_data.csv
```

Le CSV doit contenir trois colonnes : `sensor` (1-6), `force_n`, `adc`.
Le script ajuste une régression linéaire par capteur et envoie les coefficients
au module via la caractéristique BLE de configuration.

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
