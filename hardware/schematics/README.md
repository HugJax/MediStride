# Schéma électronique MediStride

## Vue d'ensemble du circuit (1 pied)

```
                          ┌──────────────────────────────┐
                          │      XIAO ESP32-S3 (U1)      │
  ┌─────────┐             │                              │
  │  LiPo   │───[SW1]───→│ BAT+                    3V3  │──→ +3V3 rail
  │ 3.7V    │             │ BAT-                    GND  │──→ GND rail
  │ 500mAh  │─────────── →│                              │
  └─────────┘             │                              │     ┌───────┐
                          │ A0 (GPIO1) ←── ADC0 ─────────│─────│ FSR1  │
        Charge via        │ A1 (GPIO2) ←── ADC1 ─────────│─────│ FSR2  │
        USB-C intégré     │ A2 (GPIO3) ←── ADC2 ─────────│─────│ FSR3  │
                          │ A3 (GPIO4) ←── ADC3 ─────────│─────│ FSR4  │
                          │ A4 (GPIO5) ←── ADC4 ─────────│─────│ FSR5  │
                          │ A5 (GPIO6) ←── ADC5 ─────────│─────│ FSR6  │
                          │                              │
                          │ D6 (GPIO43) ── SDA ──────────│──┐
                          │ D7 (GPIO44) ── SCL ──────────│──┤
                          │                              │  │
                          │        [BLE 5.0 intégré]     │  │
                          └──────────────────────────────┘  │
                                        │                   │
                                   ┌────┴────┐    ┌────────┴────────┐
                                   │ C1  C2  │    │  MPU-6050 (U2)  │
                                   │100nF 10µF│    │  [OPTIONNEL]    │
                                   └────┬────┘    │                 │
                                       GND       │ VCC ←── +3V3    │
                                                  │ GND ←── GND     │
                                                  │ SDA ←── GPIO43  │
                                                  │ SCL ←── GPIO44  │
                                                  │ AD0 ←── GND     │
                                                  │                 │
                                                  │ Accéléromètre   │
                                                  │ 3 axes (±2/4/8g)│
                                                  │ Gyroscope       │
                                                  │ 3 axes (±250-   │
                                                  │   2000 °/s)     │
                                                  └─────────────────┘
```

## Circuit de conditionnement des capteurs

Chaque capteur FlexiForce A301 est câblé en **diviseur de tension** avec une résistance pull-down de 10 kΩ :

```
    +3V3
     │
     │
  ┌──┴──┐
  │     │
  │ FSR │  FlexiForce A301 (0-445N)
  │     │  Résistance variable :
  │     │  ∞ (pas de force) → ~1kΩ (force max)
  └──┬──┘
     │
     ├──────────→ ADC (GPIO)  ← Mesure Vout ici
     │
  ┌──┴──┐
  │     │
  │ 10k │  Résistance pull-down
  │     │
  └──┬──┘
     │
    GND
```

### Formule

```
Vout = 3.3V × R_pulldown / (R_FSR + R_pulldown)
```

| État | R_FSR | Vout | Valeur ADC (12 bits) |
|------|-------|------|----------------------|
| Pas de force | → ∞ | → 0 V | → 0 |
| Force légère | ~100 kΩ | ~0,3 V | ~370 |
| Force moyenne | ~10 kΩ | ~1,65 V | ~2048 |
| Force maximale (445 N) | ~1 kΩ | ~3,0 V | ~3720 |

## IMU MPU-6050 (optionnel)

Le module MPU-6050 est une **centrale inertielle (IMU)** 6 axes qui mesure les accélérations linéaires (accéléromètre) et les vitesses de rotation (gyroscope). Son ajout est **optionnel** : les capteurs de pression seuls suffisent pour l'analyse de base de la répartition des appuis.

L'IMU apporte des données complémentaires utiles pour une analyse avancée :
- Détection des **phases du cycle de marche** (attaque, appui, propulsion, oscillation)
- Mesure de la **cadence** (pas/minute)
- Analyse de l'**orientation dynamique du pied** (pronation/supination en mouvement)

### Câblage I2C

```
    +3V3                          +3V3
     │                             │
     │    ┌────────────────┐       │
     │    │   MPU-6050     │       │
     │    │                │       │
     ├────│ VCC            │       │
     │    │                │       │
     │    │ SDA ───────────│───────┤──── GPIO43 (D6)
     │    │                │       │       │
     │    │ SCL ───────────│───────┤──── GPIO44 (D7)
     │    │                │       │
     │    │ AD0 ──┐        │    ┌──┴──┐ ┌──┴──┐
     │    │       │        │    │4.7k │ │4.7k │  Pull-ups I2C
     │    │ INT ──│── NC   │    │ R7  │ │ R8  │  (optionnels si
     │    │       │        │    └──┬──┘ └──┬──┘   non présents
     │    │ GND ──┤        │       │       │      sur le module)
     │    │       │        │      SDA     SCL
     │    └───────│────────┘
     │            │
    GND          GND
          (AD0 = GND → adresse I2C = 0x68)
```

> **Note** : La plupart des modules MPU-6050 du commerce incluent déjà les résistances de pull-up I2C et le condensateur de découplage. Dans ce cas, R7 et R8 ne sont pas nécessaires.

### Alternative : BNO055 (option premium)

Pour une analyse encore plus précise, le BNO055 (Bosch) offre 9 axes (accéléromètre + gyroscope + magnétomètre) avec un **algorithme de fusion intégré** qui fournit directement l'orientation absolue. Même câblage I2C, mais prix ~25-30 USD vs ~2-3 USD pour le MPU-6050.

## Assignation des broches

| Broche ESP32-S3 | GPIO | Fonction | Capteur | Position anatomique |
|-----------------|------|----------|---------|---------------------|
| A0 | GPIO1 | ADC | FSR1 | Talon (centre) |
| A1 | GPIO2 | ADC | FSR2 | Talon (latéral) |
| A2 | GPIO3 | ADC | FSR3 | Milieu du pied (arche) |
| A3 | GPIO4 | ADC | FSR4 | 1er métatarse |
| A4 | GPIO5 | ADC | FSR5 | 3e-4e métatarse |
| A5 | GPIO6 | ADC | FSR6 | Gros orteil |
| A7 / D9 | GPIO8 | ADC | Tension batterie | Diviseur 2× 220 kΩ vers BAT+ |
| D6  | GPIO43 | I2C SDA | IMU (optionnel) | MPU-6050 SDA             |
| D7  | GPIO44 | I2C SCL | IMU (optionnel) | MPU-6050 SCL             |
| D10 | GPIO9 | Choix L/R | — | Pont de soudure → GND = pied gauche, ouvert = droit |
| BAT+ | — | Alimentation | — | LiPo + via interrupteur |
| BAT- | — | Alimentation | — | LiPo - |

### Pont de soudure « Gauche / Droit »

Pour éviter de devoir compiler deux variantes du firmware, le module détecte
son côté au démarrage via **GPIO9** (D8) :

```
       GPIO9 (D8)
          │
       ──[•]── pont de soudure ── GND   →   "MediStride-L" (gauche)
          │
       ──[ ]── ouvert                     →   "MediStride-R" (droit)
```

Le pont est un simple cavalier soudé sur la PCB (ou un fil court entre GPIO9 et GND).
Le choix peut aussi être forcé à distance par l'application via la caractéristique
BLE de configuration (`CONFIG_CHAR_UUID`).

### Diviseur de tension batterie

```
    BAT+ ───[220 kΩ]──┬───→ GPIO8 (A7)
                      │
                    [220 kΩ]
                      │
                     GND
```

Cela permet la mesure du niveau de batterie sans dépasser la plage 0-3,3 V de l'ADC
(BAT+ peut atteindre 4,2 V à pleine charge). Le firmware applique automatiquement
le facteur 2 pour reconstituer la tension batterie réelle.

## Liste des composants (BOM) — 1 pied

| Réf. | Composant | Valeur | Qté | Description |
|------|-----------|--------|-----|-------------|
| U1 | XIAO ESP32-S3 | — | 1 | Microcontrôleur WiFi + BLE |
| FSR1-6 | FlexiForce A301 | 445 N | 6 | Capteurs de force |
| R1-6 | Résistance | 10 kΩ | 6 | Pull-down diviseur de tension |
| C1 | Condensateur céramique | 100 nF | 1 | Découplage alimentation |
| C2 | Condensateur électrolytique | 10 µF | 1 | Filtrage alimentation |
| BT1 | Batterie LiPo | 3,7 V 500 mAh | 1 | Alimentation |
| SW1 | Interrupteur SPST | — | 1 | Marche/arrêt |
| **Optionnel** | | | | |
| U2 | MPU-6050 (module) | — | 1 | IMU 6 axes (accéléromètre + gyroscope) |
| R7 | Résistance | 4,7 kΩ | 1 | Pull-up I2C SDA (si absent du module) |
| R8 | Résistance | 4,7 kΩ | 1 | Pull-up I2C SCL (si absent du module) |

**Total composants par pied : 17** (sans IMU) / **20** (avec IMU)
**Total composants pour la paire : 34** (sans IMU) / **40** (avec IMU)

## Configuration ADC

```
Résolution      : 12 bits (0-4095)
Atténuation     : 11 dB (plage d'entrée 0-3,3 V)
Échantillonnage : 20 Hz par capteur (120 lectures/seconde au total)
```

## Notes de conception

1. **Découplage** : C1 (100 nF) et C2 (10 µF) sont placés au plus près des broches 3V3 et GND du XIAO pour filtrer le bruit d'alimentation.

2. **Interrupteur** : SW1 est sur la ligne BAT+ pour couper l'alimentation sans débrancher la batterie. Le XIAO reste alimenté par USB-C pendant la charge même si SW1 est ouvert.

3. **IMU (optionnel)** : Le MPU-6050 est connecté en I2C sur GPIO43 (SDA) et GPIO44 (SCL). Il apporte l'analyse de mouvement (orientation, cadence, phases du pas) en complément des capteurs de pression. Son adresse I2C est 0x68 (AD0 → GND). Les modules du commerce incluent généralement les pull-ups I2C et le découplage — vérifier avant d'ajouter R7/R8.

4. **Valeur du pull-down** : La résistance de 10 kΩ est un bon compromis pour la plage 0-445 N du FlexiForce A301. Pour une meilleure sensibilité dans les basses forces, une résistance plus élevée (47 kΩ) peut être utilisée, au détriment de la plage dynamique.

## Fichiers

| Fichier | Format | Description |
|---------|--------|-------------|
| `medistride.kicad_pro` | KiCad 8.0 | Projet KiCad |
| `medistride.kicad_sch` | KiCad 8.0 | Schéma électronique |