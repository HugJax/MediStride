# Guide de dépannage

Solutions aux problèmes les plus fréquents, classées par symptôme.

## Connexion BLE

### La semelle n'apparaît pas dans la liste Bluetooth

1. **Vérifier l'alimentation** : l'interrupteur est-il sur ON ? La LED du XIAO
   doit s'allumer brièvement au démarrage. Si rien ne s'allume, recharger la batterie.
2. **Vérifier le navigateur** : utiliser Chrome ou Edge. Web Bluetooth ne
   fonctionne pas sur Firefox, Safari, ni sur iOS (sauf via l'app Bluefy).
3. **Vérifier la distance** : rester à moins de 10 mètres, sans gros obstacle.
4. **Redémarrer la semelle** : interrupteur OFF, attendre 5 s, puis ON.
5. **Vérifier le firmware** : si le module vient d'être assemblé, le firmware
   est-il bien flashé ? Voir [flashage.md](flashage.md).

### La connexion se coupe en cours de session

- **Batterie faible** : observer l'indicateur batterie. < 15 % = à recharger.
- **Interférences** : éloigner les sources WiFi 2,4 GHz et les autres appareils BLE.
- **Antenne masquée** : éviter de mettre le module contre du métal ou dans une
  poche fermée.

### Les deux semelles ne se connectent pas en même temps

C'est normal au premier appairage : Web Bluetooth oblige à sélectionner les
appareils **un par un**. Cliquer successivement « Connecter le pied gauche »
puis « Connecter le pied droit ».

## Flashage

### « No compatible device found »

- **Câble** : essayer un autre câble USB-C (certains câbles ne transmettent
  que l'alimentation, pas les données).
- **Pilotes** : sur Windows, installer les pilotes CP210x ou CH340 selon votre carte.
- **Permissions Linux** : ajouter votre utilisateur au groupe `dialout` :
  ```bash
  sudo usermod -a -G dialout $USER
  # puis redémarrer la session
  ```

### Le flashage commence puis échoue

- **Mode programmation** : remettre le module en mode BOOT juste avant de cliquer
  sur « Installer » (maintenir BOOT, appuyer brièvement RESET, relâcher BOOT).
- **Port occupé** : fermer toute autre application qui pourrait utiliser le port série
  (Arduino IDE, moniteur série, etc.).
- **Mémoire pleine** : très rare avec l'ESP32-S3 (8 Mo flash), mais possible si
  vous avez modifié le firmware. Effacer complètement avant de reflasher :
  ```bash
  esptool.py --chip esp32s3 erase_flash
  ```

## Capteurs

### Un capteur ne réagit pas

1. **Vérifier le câblage** : avec un multimètre, mesurer la résistance entre
   la patte du FSR et la broche ADC du XIAO. Doit être 0 Ω (continuité).
2. **Vérifier la résistance pull-down** : 10 kΩ (anneaux brun-noir-orange).
   Mesurer entre la broche ADC et GND : doit être 10 kΩ.
3. **Tester le FSR seul** : appuyer fort dessus tout en mesurant sa résistance
   au multimètre. Doit varier de très grande (repos) à ~1 kΩ (force max).
4. **Tester par les logs série** : connecter en USB-C avec un moniteur série
   à 115200 baud. Les valeurs ADC s'affichent toutes les secondes.

### Les valeurs sont incohérentes ou bruitées

- **Calibration** : la formule de conversion ADC → Newtons est une approximation.
  Pour une mesure plus précise, suivre [calibration.md](calibration.md).
- **Dispersion** : les capteurs FlexiForce ont une dispersion d'environ ±25 %
  entre exemplaires. C'est normal — pour une analyse fine, calibrer chaque
  capteur individuellement.
- **Bruit électrique** : vérifier la qualité des soudures, ajouter un
  condensateur 100 nF entre chaque broche ADC et GND si nécessaire.
- **Câbles trop longs ou croisés** : éviter de faire passer les fils sensibles
  près de la batterie ou de l'antenne BLE.

### Toutes les valeurs sont à 0

- Le rail **+3V3** est-il bien alimenté ? Mesurer la tension : 3,30 V ±0,1 V.
- Vérifier que la pile LiPo est chargée (au moins 3,7 V à vide).

### Toutes les valeurs sont au maximum (saturation)

- Vérifier que la résistance pull-down est bien à 10 kΩ et bien connectée à GND.
- Vérifier qu'il n'y a pas de court-circuit entre la broche ADC et +3V3.

## Application

### L'app affiche « Web Bluetooth non supporté »

- **Sur ordinateur** : utiliser **Chrome** ou **Edge** ; Firefox et Safari ne
  supportent pas Web Bluetooth.
- **Sur Android** : utiliser Chrome.
- **Sur iPhone / iPad** : installer l'app [Bluefy](https://apps.apple.com/app/bluefy-web-ble-browser/id1492822055),
  qui apporte Web Bluetooth à iOS.

### L'export CSV ne fonctionne pas

- Vérifier que la session a bien été démarrée puis arrêtée (le bouton « Exporter »
  s'active seulement après).
- Sur certains navigateurs, l'export peut être bloqué : autoriser les
  téléchargements automatiques pour cette page.

### Le service worker pose problème (cache obsolète)

Vider le cache du site :
- Chrome / Edge → F12 → Application → Service Workers → Unregister
- Recharger la page (Ctrl+Shift+R)

## Batterie

### L'autonomie est très faible (< 2 h)

- Vérifier que le mode deep sleep est actif (firmware ≥ 1.1).
- Tester avec une batterie neuve — les LiPo perdent leur capacité avec l'âge.
- Couper la semelle entre les sessions (interrupteur sur OFF).

### La batterie chauffe ou est gonflée

**Cesser immédiatement l'utilisation**. Placer la batterie dans un sac ignifuge
(ou dans du sable) et l'apporter à un point de collecte (recyclerie, déchèterie).
Ne pas jeter à la poubelle, ne pas démonter, ne pas percer.

## Demander de l'aide

Si vous restez bloqué :

1. Décrire le problème dans une **issue GitHub** :
   [github.com/HugJax/MediStride/issues](https://github.com/HugJax/MediStride/issues)
2. Joindre :
   - le **modèle exact** des composants utilisés (FSR, microcontrôleur) ;
   - les **logs série** si possible ;
   - des **photos** du câblage côté soudures.
3. Pour les questions plus ouvertes, utiliser les
   [Discussions GitHub](https://github.com/HugJax/MediStride/discussions).
