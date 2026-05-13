# Guide de calibration des capteurs

Les capteurs FlexiForce A301 (et a fortiori les modèles génériques) ont une **dispersion
capteur-à-capteur de ±25 %**. Sans calibration individuelle, la formule de conversion
ADC → Newtons donne un ordre de grandeur, pas une mesure précise.

Ce guide décrit deux niveaux de calibration :

- **Niveau A — calibration relative** (sans matériel) : suffisante pour observer
  la symétrie G/D et la cadence.
- **Niveau B — calibration absolue** (avec un objet de poids connu) : nécessaire
  pour comparer des forces entre semelles ou avec d'autres systèmes.

---

## Niveau A — Calibration relative

C'est la **méthode par défaut** du firmware actuel. Le passage de l'ADC à un Newton
utilise les paramètres fixes définis dans `firmware/src/config.h` :

```c
#define FSR_MAX_FORCE_N    445.0f
#define PULLDOWN_RESISTANCE 10000.0f
```

Aucune action n'est requise. Les valeurs affichées sont des **estimations** à environ
±30 % de la force réelle, ce qui reste utile pour :

- comparer les zones d'un même pied (talon vs. avant-pied) ;
- comparer la symétrie gauche / droite ;
- détecter une évolution sur le même patient au fil des séances.

---

## Niveau B — Calibration absolue

Pour des mesures comparables d'un patient à l'autre, calibrer chaque capteur
individuellement avec un poids connu.

### Matériel nécessaire

- Une **balance précise** (cuisine, ±5 g)
- Des **poids étalons** : une bouteille d'eau 1,5 L (≈14,7 N), un pack de 6 (≈59 N),
  votre propre poids (à vous peser).
- Un objet rigide pour répartir le poids sur le capteur (palet de bois ø 15 mm).

### Procédure

1. **Brancher** une semelle en USB-C à un ordinateur.
2. **Ouvrir un moniteur série** à 115200 baud :
   ```bash
   cd firmware
   pio device monitor -b 115200
   ```
3. Pour **chaque capteur (1 à 6)** :
   a. Poser le capteur à plat sur la balance.
   b. Lire la valeur ADC affichée dans les logs `[DATA]` au repos (devrait être 0-50).
   c. Poser un poids connu (ex. 1,5 kg = 14,7 N) sur le palet, lui-même sur le capteur.
   d. Noter la valeur ADC affichée et la force réelle.
   e. Répéter avec 2-3 autres poids pour avoir plusieurs points de calibration.
4. **Construire un tableau** par capteur :

   | Capteur | F (N) | ADC moyen |
   |---|---|---|
   | FSR1 | 0 | 12 |
   | FSR1 | 14,7 | 380 |
   | FSR1 | 59 | 1450 |
   | FSR1 | 120 | 2580 |

5. **Calculer la pente** : `pente = F / ADC` ou ajuster une régression linéaire
   (formule `=PENTE()` dans LibreOffice / Excel).
6. **Stocker la calibration** dans le firmware (NVS) — voir le module
   `calibration.cpp` du firmware ≥ 1.1.

### Sauvegarder la calibration dans le firmware

Le firmware (≥ v1.1) expose une caractéristique BLE d'écriture (`CONFIG_CHAR_UUID`,
définie dans [`firmware/src/config.h`](../../firmware/src/config.h)) qui accepte
des commandes pour stocker la calibration en NVS persistante.

Un **script Python** automatise toute la chaîne — régression linéaire des points
de mesure puis envoi BLE au module :

```bash
cd software/analysis
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt   # installe aussi bleak (BLE)

# 1. Construire un CSV de calibration avec vos mesures :
cat > calibration_left.csv <<EOF
sensor,adc,force_n
1,12,0
1,380,14.7
1,1450,59
1,2580,120
2,15,0
2,400,14.7
2,1480,59
# ... un capteur à la fois
EOF

# 2. Vérifier les coefficients ajustés sans connexion BLE :
python -m medistride.calibrate --csv calibration_left.csv --dry-run

# 3. Envoyer la calibration au module (allumé et à portée Bluetooth) :
python -m medistride.calibrate --device MediStride-L --csv calibration_left.csv
```

Options principales :
- `--device` : nom BLE du module (`MediStride-L` ou `MediStride-R`).
- `--csv` : fichier de mesures (colonnes `sensor` 1-6, `adc`, `force_n`).
- `--dry-run` : calcule et affiche les coefficients sans BLE.
- `--no-save` : applique en RAM uniquement, perdu au reboot.
- `--reset` : remet la calibration aux valeurs par défaut firmware.

Le script utilise la bibliothèque [`bleak`](https://bleak.readthedocs.io/) qui
fonctionne sur Linux (BlueZ), macOS et Windows 10/11.

---

## Vérifier la calibration

Une fois la calibration appliquée, relancer le moniteur série et appuyer sur
chaque capteur avec une force connue (par exemple, vous appuyer dessus avec
un pouce à 30 N — ressenti « ferme »). Vérifier que la valeur en Newtons
affichée est cohérente.

Pour un test plus poussé, refaire passer la balance sous le capteur monté
dans la semelle finale : la valeur affichée par l'app doit correspondre
à ±10 % du poids réel.

---

## Calibration en condition réelle (poids du patient)

Une calibration alternative, plus simple, consiste à demander au patient de :

1. **Se tenir immobile** sur les deux pieds, poids réparti.
2. Lire la force totale gauche + droite affichée par l'app.
3. Vérifier qu'elle est cohérente avec le **poids réel du patient en Newtons**
   (poids kg × 9,81).
4. Si l'écart dépasse 15 %, multiplier les valeurs par un coefficient correctif global.

Cette méthode ne corrige pas la dispersion capteur-à-capteur mais ajuste
l'échelle globale d'une semelle.
