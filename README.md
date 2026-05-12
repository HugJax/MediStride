# MediStride

**Chaussures intelligentes open source pour la rééducation motrice**

MediStride est une plateforme matérielle et logicielle open source qui utilise des semelles équipées de capteurs de pression pour analyser la marche des patients en temps réel. Le projet vise à rendre la rééducation de haute précision accessible au plus grand nombre en réduisant les coûts des dispositifs médicaux existants.

> ⚠️ **MediStride n'est pas un dispositif médical certifié.** Outil d'aide à
> l'observation et à la recherche uniquement. Voir [docs/legal.md](docs/legal.md).

## Démarrer

Selon votre profil :

| Vous êtes… | Commencez par |
|---|---|
| 🏥 **Soignant** qui souhaite essayer l'outil | [Tutoriel pas à pas](docs/guides/tutoriel.md) |
| 🔧 **Maker** qui veut construire la paire | [Guide d'assemblage](docs/guides/assemblage.md) |
| 💻 **Développeur** | [Architecture](#architecture-du-projet) ci-dessous |
| 🩺 **Clinicien** intéressé par la recherche | [docs/legal.md](docs/legal.md) puis [Tutoriel](docs/guides/tutoriel.md) |

## Objectifs

- **Analyse en temps réel** de la répartition des appuis plantaires et de la symétrie du pas
- **Plateforme complète** : plans hardware, firmware, application web et algorithmes d'analyse
- **Coût réduit** : composants accessibles (~110-400 USD la paire vs. 5 000-15 000 USD en commercial)
- **Open source** : favoriser l'innovation collaborative dans le domaine de la e-santé
- **Accessible** : utilisable par du personnel non technique grâce à un flasheur web et une app sans installation

## Architecture du projet

```
MediStride/
├── firmware/              # Code embarqué (XIAO ESP32-S3 / Arduino)
│   └── src/
│       ├── main.cpp           # Boucle principale
│       ├── sensors.{h,cpp}    # Lecture FSR
│       ├── ble_server.{h,cpp} # Serveur BLE GATT
│       ├── imu.{h,cpp}        # IMU MPU-6050 (optionnel)
│       ├── calibration.{h,cpp}# Calibration par capteur (NVS)
│       ├── power.{h,cpp}      # Batterie + deep sleep
│       └── device_id.{h,cpp}  # Détection L/R automatique
├── hardware/              # Conception matérielle
│   ├── schematics/        # Schémas KiCad
│   ├── pcb/               # Fichiers PCB
│   └── 3d-models/         # Modèles 3D
├── software/
│   ├── app/web/           # Application web PWA (BLE + visualisation)
│   └── analysis/          # Scripts Python d'analyse post-session
├── docs/
│   ├── guides/            # Tutoriel, assemblage, flashage, utilisation…
│   ├── images/            # Images et diagrammes
│   └── legal.md           # Mentions légales & RGPD
└── tests/                 # Tests Python
```

## Stack technique

| Composant | Technologie |
|-----------|-------------|
| Microcontrôleur | XIAO ESP32-S3 (WiFi + BLE 5.0) |
| Capteurs | FlexiForce A301 445 N (Tekscan) ou IMS-C20B 50 kg |
| IMU optionnelle | MPU-6050 (6 axes, I2C) |
| Firmware | C++ (PlatformIO / Arduino framework) |
| Application | PWA Web Bluetooth (vanilla JS) |
| Analyse offline | Python 3.10+ |

## Démarrage rapide

### Pour utiliser une paire déjà assemblée

1. Charger les deux semelles via USB-C.
2. Allumer les modules (interrupteur talon).
3. Ouvrir [`software/app/web/index.html`](software/app/web/index.html) dans Chrome ou Edge.
4. Connecter les deux pieds, démarrer la session.

### Pour construire une paire à partir de zéro

Suivre le [tutoriel pas à pas](docs/guides/tutoriel.md) qui couvre l'achat des composants,
l'assemblage, le flashage du firmware et la première session.

### Pour les développeurs

```bash
# Firmware
cd firmware
pio run -e xiao_esp32s3 --target upload

# Application web (serveur local pour test)
cd software/app/web
python3 -m http.server 8000
# Puis Chrome sur http://localhost:8000
```

## Documentation

- 📖 [Tutoriel complet pas à pas](docs/guides/tutoriel.md) — de zéro à la première mesure
- 🔧 [Guide d'assemblage](docs/guides/assemblage.md)
- ⚡ [Guide de flashage](docs/guides/flashage.md)
- 🩺 [Guide d'utilisation soignant](docs/guides/utilisation.md)
- 📏 [Calibration des capteurs](docs/guides/calibration.md)
- 🔌 [Guide matériel](docs/guides/materiel.md)
- 🛠️ [Dépannage](docs/guides/depannage.md)
- ⚖️ [Mentions légales & RGPD](docs/legal.md)

## Contribuer

Les contributions sont les bienvenues ! Consultez le [guide de contribution](CONTRIBUTING.md).

Que vous soyez développeur, maker, professionnel de santé ou étudiant :
- amélioration du firmware et des algorithmes ;
- conception hardware (PCB flex, boîtier 3D) ;
- tests cliniques et retours d'utilisation ;
- documentation et traduction.

## Licence

Apache 2.0 — voir [LICENSE](LICENSE).

## Contact

- **Issues** : [GitHub Issues](https://github.com/HugJax/MediStride/issues)
- **Discussions** : [GitHub Discussions](https://github.com/HugJax/MediStride/discussions)
