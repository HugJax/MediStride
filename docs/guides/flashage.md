# Guide de flashage du firmware

Ce guide explique comment installer le firmware MediStride sur un module
XIAO ESP32-S3. **Deux méthodes** sont disponibles :

- **Méthode A — Flasheur web** (recommandée, aucun logiciel à installer)
- **Méthode B — PlatformIO** (pour les développeurs)

---

## Méthode A — Flasheur web (recommandée)

### Pré-requis

- Un **ordinateur** sous Windows, macOS ou Linux
- Le navigateur **Google Chrome** ou **Microsoft Edge** (Firefox et Safari ne fonctionnent pas)
- Un **câble USB-C** qui transmet les données (certains câbles bas de gamme ne font que la charge)
- Le module **XIAO ESP32-S3** à programmer

### Étapes

1. **Ouvrez** la page de flashage dans Chrome ou Edge :
   - en local : `software/app/web/flash.html` (servir via `python3 -m http.server`)
   - en ligne : *(URL GitHub Pages à publier — voir le README)*

2. **Branchez** le XIAO ESP32-S3 à votre ordinateur avec le câble USB-C.

3. **Mettez le module en mode programmation** :
   - maintenez enfoncé le bouton `BOOT` du XIAO ;
   - appuyez brièvement sur `RESET` (sans relâcher `BOOT`) ;
   - relâchez `BOOT`.
   - Sur certains modèles, il suffit d'appuyer deux fois rapidement sur `RESET`.

4. **Choisissez** le firmware selon le pied que vous programmez :
   - **Pied gauche** → bouton « Installer (gauche) »
   - **Pied droit** → bouton « Installer (droit) »

5. **Autorisez** le navigateur à accéder au port série, sélectionnez le port du module
   dans la liste (selon votre OS) :
   - Linux : `/dev/ttyACM0` ou `/dev/ttyUSB0`
   - macOS : `/dev/cu.usbmodem…`
   - Windows : `COM3`, `COM4` ou similaire

6. **Patientez** ~30 secondes. La barre de progression s'affiche. À la fin, le module
   redémarre automatiquement.

7. **Vérifiez** que tout fonctionne :
   - le module émet en BLE sous le nom `MediStride-L` ou `MediStride-R` ;
   - vous pouvez le détecter avec l'[application web](../../software/app/web/).

### Si ça ne marche pas

- **« No compatible device found »** → le navigateur ne voit pas le module.
  Vérifiez le câble (testez avec un autre), réessayez la mise en mode BOOT,
  et installez les pilotes série si vous êtes sur Windows.
- **Le port apparaît mais le flashage échoue** → mettez le module en mode programmation
  juste **avant** de cliquer sur « Installer ». Si vous attendez trop longtemps,
  l'ESP32 démarre normalement et n'accepte plus le flashage.
- Voir aussi [dépannage.md](depannage.md).

---

## Méthode B — PlatformIO (pour développeurs)

Cette méthode est utile pour modifier le firmware, le compiler localement, et déboguer.

### Pré-requis

- Python 3.10 ou supérieur
- PlatformIO Core :
  ```bash
  pip install platformio
  ```
  (ou utiliser l'extension PlatformIO IDE pour VS Code)

### Compilation et flashage

```bash
cd firmware

# Pied gauche : la valeur par défaut de DEVICE_NAME est "MediStride-L"
pio run -e xiao_esp32s3 --target upload

# Pour le pied droit, modifier src/config.h :
#   #define DEVICE_NAME "MediStride-R"
# Puis :
pio run -e xiao_esp32s3 --target upload
```

### Suivi série pendant le développement

```bash
pio device monitor -b 115200
```

Vous verrez les logs `[MAIN]`, `[BLE]`, `[SENSORS]`, etc.

### Activer l'IMU (optionnel)

Dans `firmware/platformio.ini`, décommentez :
```ini
build_flags =
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DIMU_ENABLED
```

Puis recompilez et reflashez.

---

## Que faire après le flashage ?

- Tester la connexion BLE avec l'app : [`software/app/web/`](../../software/app/web/)
- Calibrer les capteurs (procédure dans [calibration.md](calibration.md))
- Procéder à l'intégration finale dans la chaussure : [assemblage.md](assemblage.md)
