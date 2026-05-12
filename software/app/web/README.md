# MediStride — Application web

Application web PWA pour visualiser en temps réel les données des semelles MediStride via Web Bluetooth.

## Lancer en local

L'application est statique. Pour un essai rapide :

```bash
cd software/app/web
python3 -m http.server 8000
```

Ouvrir [http://localhost:8000](http://localhost:8000) dans **Chrome** ou **Edge** (Web Bluetooth n'est pas supporté par Firefox ni Safari).

> Pour la production, servez ces fichiers via HTTPS (GitHub Pages, Netlify, etc.) — Web Bluetooth exige une origine sécurisée.

## Compatibilité

| Navigateur | Bureau | Android | iOS |
|---|---|---|---|
| Chrome / Edge | ✅ | ✅ | ❌ |
| Firefox | ❌ | ❌ | ❌ |
| Safari | ❌ | — | ❌ |

Sur iOS, utiliser l'application [Bluefy](https://apps.apple.com/app/bluefy-web-ble-browser/id1492822055) (navigateur tiers avec Web Bluetooth).

## Architecture

```
web/
├── index.html               # Vue principale
├── flash.html               # Flasheur firmware (ESP Web Tools)
├── manifest.webmanifest     # PWA manifest
├── service-worker.js        # Cache offline
├── css/style.css
├── js/
│   ├── app.js               # Logique principale
│   ├── ble.js               # Connexion BLE + décodage paquets
│   ├── viz.js               # Rendu canvas (heatmap)
│   ├── analysis.js          # Symétrie + cadence
│   └── export.js            # Export CSV
└── icons/icon.svg
```

## Protocole BLE attendu

Le firmware expose un service GATT (`4d454449-5354-5249-4445-000000000001`) avec :

| Caractéristique | UUID (fin) | Format | Description |
|---|---|---|---|
| Pression | `…0002` | 6× uint16 LE + uint32 LE | ADC bruts + timestamp ms |
| IMU | `…0003` | 6× int16 LE + uint32 LE | accel + gyro + timestamp |
| Batterie | `…0004` | uint8 | niveau en % |

Le nom BLE doit commencer par `MediStride-L` (gauche) ou `MediStride-R` (droit).
