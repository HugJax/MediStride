# MediStride

**Chaussures intelligentes open source pour la rééducation motrice**

MediStride est une plateforme matérielle et logicielle open source qui utilise des semelles équipées de capteurs de pression pour analyser la marche des patients en temps réel. Le projet vise à rendre la rééducation de haute précision accessible au plus grand nombre en réduisant les coûts des dispositifs médicaux existants.

## Objectifs

- **Analyse en temps réel** de la répartition des appuis plantaires et de la symétrie du pas
- **Plateforme complète** : plans de conception hardware, firmware et algorithmes d'analyse
- **Coût réduit** : composants accessibles et abordables disponibles en ligne
- **Open source** : favoriser l'innovation collaborative dans le domaine de la e-santé

## Architecture du projet

```
MediStride/
├── firmware/          # Code embarqué (microcontrôleur)
│   ├── src/           # Code source principal
│   ├── lib/           # Bibliothèques et dépendances
│   └── config/        # Fichiers de configuration
├── hardware/          # Conception matérielle
│   ├── schematics/    # Schémas électroniques
│   ├── pcb/           # Fichiers PCB
│   └── 3d-models/     # Modèles 3D des pièces mécaniques
├── software/          # Logiciel côté utilisateur
│   ├── app/           # Application de visualisation
│   └── analysis/      # Algorithmes d'analyse de la marche
├── docs/              # Documentation
│   ├── images/        # Images et diagrammes
│   └── guides/        # Guides d'utilisation et d'assemblage
└── tests/             # Tests unitaires et d'intégration
```

## Stack technique

| Composant | Technologie |
|-----------|-------------|
| Microcontrôleur | ESP32 (WiFi + BLE intégré) |
| Capteurs | FlexiForce A301 445 N (Tekscan) ou IMS-C20B |
| Firmware | C/C++ (Arduino / ESP-IDF) |
| Analyse | Python |
| Communication | BLE (Bluetooth Low Energy) |

## Prérequis

- [PlatformIO](https://platformio.org/) ou [Arduino IDE](https://www.arduino.cc/en/software) pour le firmware
- [Python 3.10+](https://www.python.org/) pour les algorithmes d'analyse
- Composants matériels (voir [docs/guides/materiel.md](docs/guides/materiel.md))

## Installation

### Firmware

```bash
# Cloner le dépôt
git clone https://github.com/HugJax/MediStride.git
cd MediStride/firmware

# Avec PlatformIO
pio run --target upload
```

### Logiciel d'analyse

```bash
cd MediStride/software
pip install -r requirements.txt
python -m analysis
```

## Contribuer

Les contributions sont les bienvenues ! Consultez le [guide de contribution](CONTRIBUTING.md) pour commencer.

Que vous soyez développeur, maker, professionnel de santé ou étudiant, votre aide est précieuse :
- Amélioration du firmware et des algorithmes
- Conception hardware et optimisation des capteurs
- Tests cliniques et retours d'utilisation
- Documentation et traduction

## Licence

Ce projet est distribué sous licence [Apache 2.0](LICENSE).

## Contact

- **Issues** : [GitHub Issues](https://github.com/HugJax/MediStride/issues)
- **Discussions** : [GitHub Discussions](https://github.com/HugJax/MediStride/discussions)