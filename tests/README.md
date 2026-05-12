# Tests MediStride

## Tests Python (analyse)

```bash
cd MediStride
python3 -m venv .venv
source .venv/bin/activate
pip install -r software/analysis/requirements.txt
pytest tests/
```

Couvre :
- calcul de symétrie G/D ;
- détection des appuis (heel strikes) avec gestion du refractory ;
- calcul de cadence ;
- répartition par zone anatomique ;
- chargement / validation des CSV exportés.

## Tests firmware

Le firmware est testé manuellement pour l'instant. Voir
[docs/guides/depannage.md](../docs/guides/depannage.md) pour les procédures
de vérification (multimètre, moniteur série, LED).

Une suite de tests automatisés via [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/)
est prévue ultérieurement.
