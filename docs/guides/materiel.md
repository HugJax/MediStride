# Guide matériel MediStride

Ce guide liste les composants nécessaires pour construire une paire de semelles intelligentes MediStride. Tous les composants sont disponibles en ligne à coût réduit.

## Architecture globale

```
[Pied gauche]                    [Pied droit]
6x FlexiForce A301              6x FlexiForce A301
    |                                |
XIAO ESP32-S3                   XIAO ESP32-S3
    |                                |
  LiPo 500mAh                   LiPo 500mAh
    |                                |
    +---------- BLE ----------+
                 |
          [Smartphone]
          Application mobile
          - Visualisation temps réel
          - Analyse de symétrie G/D
          - Historique des sessions
```

**Flux de données** : les 6 FSR par pied sont échantillonnés via l'ADC de l'ESP32 à ~20 Hz, transmis en BLE vers l'application mobile qui calcule la distribution de pression et la symétrie gauche/droite.

## Estimation du coût total

| Composant | Option économique | Option qualité |
|-----------|-------------------|----------------|
| 6x Capteurs de pression | ~30 USD (IMS-C20B) | ~120 USD (FlexiForce A301) |
| Microcontrôleur | ~7 USD (XIAO ESP32-S3) | ~35 USD (Nano 33 BLE Sense) |
| Batterie LiPo 500mAh | ~4 USD | ~7 USD |
| ADC externe (optionnel) | ~2 USD | ~5 USD |
| Câblage / connecteurs | ~5 USD | ~10 USD |
| Semelle / support | ~5 USD (EVA) | ~15 USD (flex PCB) |
| Résistances + divers | ~3 USD | ~5 USD |
| **Total par pied** | **~56 USD** | **~197 USD** |
| **Total pour la paire** | **~112 USD** | **~394 USD** |

---

## 1. Capteurs de pression

> **Note importante sur le dimensionnement** : Un adulte de 80 kg génère des forces de réaction au sol de 800-960 N en marchant (1,0 à 1,2x le poids corporel). Les pics de pression sous le talon atteignent 300-500 N par point. Les capteurs doivent donc supporter **au minimum 450 N** par zone de mesure. Les capteurs FSR classiques (FSR 402, plage 0,2-20 N) sont **inadaptés** pour cette application.

### Option recommandée : FlexiForce A301 445 N (Tekscan)

- **Description** : Capteur de force piézorésistif à film mince
- **Prix** : ~15-20 USD/unité (DigiKey, Mouser) / ~118 USD le pack de 8 (Tekscan)
- **Où acheter** : DigiKey, Mouser, Tekscan
- **Spécifications** : plage 0-445 N (100 lbs), épaisseur 0,2 mm, zone de détection 9,53 mm de diamètre
- **Pourquoi** : La plage de 445 N couvre parfaitement les forces de marche d'un adulte. Épaisseur de seulement 0,2 mm, idéale pour l'intégration en semelle. Capteur éprouvé en recherche clinique pour l'analyse de la marche.

### Alternative haute précision : SingleTact S15-450N

- **Description** : Capteur de force capacitif
- **Prix** : ~37,50 USD/unité (capteur seul)
- **Où acheter** : SingleTact.com, Amazon, RobotShop
- **Spécifications** : plage 0-450 N, épaisseur 0,3 mm, diamètre 15 mm, résolution < 0,2%, répétabilité < 1%
- **Pourquoi** : Technologie capacitive offrant une meilleure linéarité et précision que les capteurs piézorésistifs. Nécessite une électronique externe dédiée (carte I2C SingleTact). Plus cher mais plus fiable.

### Alternative économique : IMS-C20B (capteur chinois générique)

- **Description** : Capteur piézorésistif à membrane flexible
- **Prix** : ~5-10 USD/unité (AliExpress, Amazon)
- **Où acheter** : AliExpress, Amazon, Alibaba
- **Spécifications** : disponible en 10 kg, 20 kg, 50 kg et 100 kg (~980 N), épaisseur ~0,3-0,5 mm, zone active 20 mm
- **Pourquoi** : La version 50 kg (490 N) ou 100 kg (980 N) couvre les besoins. Très économique. Précision et répétabilité non garanties — acceptable pour un prototype, pas pour de la recherche clinique.

### Pourquoi PAS les FSR 402 / FSR 406 ?

Les capteurs FSR classiques d'Interlink Electronics (FSR 402 : plage 0,2-20 N, FSR 406 similaire) **ne conviennent pas** pour la mesure de forces plantaires d'un adulte :
- Saturation complète au-delà de 20-100 N
- Précision très dégradée hors plage nominale (erreur > 50%)
- Risque d'endommagement sous des forces de 200-500 N

### Placement recommandé (6 capteurs par pied)

| # | Position | Rôle |
|---|----------|------|
| 1 | Talon (centre) | Phase d'attaque du pas |
| 2 | Talon (latéral) | Pronation / supination |
| 3 | Milieu du pied (arche) | Détection pied plat / creux |
| 4 | 1er métatarse | Propulsion médiale |
| 5 | 3e-4e métatarse | Propulsion latérale |
| 6 | Gros orteil | Phase finale de propulsion |

---

## 2. Microcontrôleur

### Option recommandée : Seeed Studio XIAO ESP32-S3

- **Prix** : ~7-10 USD
- **Où acheter** : Seeed Studio, Amazon, AliExpress, DigiKey
- **Spécifications** : Xtensa dual-core 240 MHz, WiFi + BLE 5.0, ~20 canaux ADC, dimensions 21 x 17,5 mm, deep sleep ~14 µA, 8 Mo flash, USB-C
- **Pourquoi** : Le plus petit module ESP32 disponible (21 x 17,5 mm). Suffisamment d'entrées ADC pour 6 FSR sans ADC externe. BLE intégré. **Meilleur rapport taille/fonctionnalité/prix.**

### Alternative : Arduino Nano 33 BLE Sense Rev2

- **Prix** : ~32-38 USD
- **Où acheter** : Arduino Store, Adafruit, Amazon
- **Spécifications** : nRF52840 (ARM Cortex-M4, 64 MHz), BLE 5.0, 8 entrées analogiques, IMU 9 axes intégrée, 45 x 18 mm
- **Pourquoi** : L'IMU intégrée est un atout pour l'analyse de marche. Très économe en énergie. Prix plus élevé.

### Alternative ultra-économique : XIAO ESP32-C3

- **Prix** : ~5-6 USD
- **Où acheter** : Seeed Studio, AliExpress
- **Spécifications** : RISC-V 160 MHz, WiFi + BLE 5.0, seulement 2 canaux ADC, 21 x 17,5 mm
- **Pourquoi** : Le moins cher, mais nécessite un ADC externe (ADS1115) vu le nombre limité de canaux ADC.

---

## 3. Alimentation

### Option recommandée : Batterie LiPo 3,7V 500mAh

- **Prix** : ~4-6 USD
- **Où acheter** : Adafruit (#1578), AliExpress
- **Spécifications** : type 503035, dimensions ~35 x 25 x 5 mm, connecteur JST-PH 2.0
- **Autonomie estimée** : 8-12h en BLE basse consommation
- **Note** : Le XIAO ESP32-S3 dispose de pads de batterie avec circuit de charge intégré.

### Alternative ultra-fine : LiPo 250mAh

- **Prix** : ~3-5 USD (AliExpress)
- **Spécifications** : type 402030, ~30 x 20 x 4 mm
- **Autonomie** : ~4-6h, suffisante pour une session de rééducation

### Module de charge (si non intégré au MCU)

- **Produit** : TP4056 USB-C
- **Prix** : ~0,50-1 USD (AliExpress)

---

## 4. Convertisseur analogique-numérique (ADC) externe (optionnel)

### ADS1115

- **Prix** : ~3-5 USD (Adafruit #1085) / ~1-2 USD (AliExpress)
- **Spécifications** : 16 bits, 4 canaux, I2C (adresses configurables, jusqu'à 4 modules chaînables), 8 à 860 SPS
- **Pourquoi** : Avec le XIAO ESP32-S3, pas strictement nécessaire. Mais offre une résolution 16 bits (vs 12 bits interne) pour plus de précision.

### Alternative : Multiplexeur CD74HC4067

- **Prix** : ~1-2 USD (AliExpress, SparkFun BOB-09056)
- **Spécifications** : multiplexeur 16 canaux analogiques
- **Pourquoi** : Permet de lire jusqu'à 16 capteurs avec un seul canal ADC. Moins cher qu'un ADC externe, mais ne change pas la résolution.

---

## 5. Communication

Avec l'ESP32-S3 ou le Nano 33 BLE, le **BLE est intégré** — aucun module externe nécessaire.

**Protocole recommandé** : BLE GATT personnalisé avec notification toutes les 50-100 ms. Service GATT avec une caractéristique contenant les 6 valeurs de pression multiplexées. Débit suffisant pour 6 capteurs à ~20 Hz.

---

## 6. Câblage et connecteurs

| Composant | Prix approx. | Plateforme | Usage |
|-----------|-------------|------------|-------|
| Ruban de cuivre adhésif (6 mm) | ~5-7 USD / 15 m | Adafruit (#1128), Amazon | Pistes conductrices sur la semelle (prototypage) |
| Fil conducteur acier inox | ~3-5 USD / 7 m | Adafruit (#640), SparkFun | Connexions cousues dans le textile |
| Câbles FFC (flat flexible) | ~1-3 USD / lot de 5-10 | AliExpress, DigiKey | Solution fine et robuste (version finale) |
| Fil émaillé 30 AWG | ~3-5 USD / bobine | Amazon, AliExpress | Connexions fines et flexibles |

**Recommandation** : Ruban de cuivre pour le prototype, câbles FFC pour la version finale.

---

## 7. Matériau de semelle / Support

### Phase prototype : Semelle EVA modifiable

- **Prix** : ~3-8 USD la paire (Amazon, magasins de chaussures)
- **Méthode** : Intégrer les capteurs entre deux couches de mousse EVA, collés et câblés.

### Phase produit : PCB flexible (Flex PCB)

- **Prix** : ~5-15 USD/unité (commande min. de 5)
- **Où commander** : JLCPCB, PCBWay
- **Pourquoi** : Solution professionnelle. PCB en forme de semelle avec emplacements des capteurs et pistes intégrées. Conception avec KiCad.

### Solution textile : Tissu conducteur + Velostat

- **Prix** : ~10-20 USD pour une feuille A4 de tissu conducteur (Adafruit #1168)
- **Méthode** : Sandwich tissu conducteur / Velostat / tissu conducteur = capteur souple intégrable dans une semelle textile.

---

## 8. Composants supplémentaires utiles

| Composant | Prix approx. | Pourquoi |
|-----------|-------------|----------|
| MPU-6050 (IMU 6 axes) | ~2-3 USD (AliExpress) | Orientation du pied, cadence, détection phases du pas |
| BNO055 (IMU 9 axes avec fusion) | ~25-30 USD (Adafruit) | Version premium avec algorithme de fusion intégré |
| Kit de résistances (10 kΩ pour diviseurs de tension FSR) | ~2-3 USD | Chaque FSR nécessite un pull-down de 10 kΩ |

---

## 9. Projets open source de référence

- **OpenSole** (GitHub) — Semelle instrumentée Arduino + FSR + BLE. Architecture de référence.
- **Projet MIT "Insole Pressure Mapping"** — Velostat + Arduino + Processing pour la visualisation.
- **Tutoriels Instructables / Hackaday** — "DIY Smart Insole", "Pressure Sensing Insole", "Open Source Gait Analysis".

### Bibliothèques logicielles utiles

- **NimBLE-Arduino** — Stack BLE léger pour ESP32
- **Adafruit_ADS1x15** — Pilote ADS1115
- **ArduinoBLE** — Pour Nano 33 BLE
- **MPU6050_light** — Pilote IMU léger

---

## 10. Liste d'achat rapide (configuration recommandée)

Pour **une paire de semelles** :

### Option économique (prototype)

| Qté | Composant | Réf. suggérée | Prix unitaire |
|-----|-----------|---------------|---------------|
| 12 | IMS-C20B (50 kg / 490 N) | AliExpress | ~5-10 USD |
| 2 | XIAO ESP32-S3 | Seeed Studio | ~7-10 USD |
| 2 | Batterie LiPo 500mAh | AliExpress | ~4-6 USD |
| 12 | Résistances 10 kΩ | AliExpress | ~0,10 USD |
| 1 | Ruban de cuivre 15m | Amazon | ~5-7 USD |
| 1 | Paire de semelles EVA | Amazon | ~3-8 USD |
| 1 | Fil émaillé 30 AWG | AliExpress | ~3-5 USD |

**Total estimé : ~112 USD la paire**

### Option qualité (recherche / clinique)

| Qté | Composant | Réf. suggérée | Prix unitaire |
|-----|-----------|---------------|---------------|
| 12 | FlexiForce A301 (445 N) | DigiKey / Mouser | ~15-20 USD |
| 2 | XIAO ESP32-S3 | Seeed Studio | ~7-10 USD |
| 2 | Batterie LiPo 500mAh | Adafruit | ~6-7 USD |
| 2 | ADS1115 (16 bits) | Adafruit | ~3-5 USD |
| 12 | Résistances 10 kΩ | DigiKey | ~0,10 USD |
| 1 | Câbles FFC | DigiKey | ~3-5 USD |
| 1 | Paire de semelles EVA | Amazon | ~3-8 USD |

**Total estimé : ~394 USD la paire**

> **Note** : Les prix sont des estimations. Vérifiez les tarifs actuels sur les plateformes mentionnées avant toute commande. Les systèmes commerciaux équivalents (Tekscan F-Scan, Novel Pedar) coûtent entre 5 000 et 15 000 USD, ce qui place MediStride à une fraction du coût.