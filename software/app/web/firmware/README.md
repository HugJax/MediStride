# Binaires firmware MediStride

Ce dossier contient les **manifests ESP Web Tools** et les binaires compilés
pour le flashage direct depuis le navigateur (via `flash.html`).

## Contenu

```
firmware/
├── manifest-left.json    # Manifest pour le pied gauche
├── manifest-right.json   # Manifest pour le pied droit
└── bin/
    ├── medistride-left.bin   ← À générer
    └── medistride-right.bin  ← À générer
```

## Générer les binaires

Les binaires `.bin` ne sont **pas commités** dans le dépôt (taille).
Ils doivent être générés à chaque release. Procédure :

```bash
cd firmware

# 1. Compiler pour le pied gauche
sed -i 's/"MediStride-L"/"MediStride-L"/' src/config.h   # Vérifier valeur
pio run -e xiao_esp32s3
cp .pio/build/xiao_esp32s3/firmware.bin \
   ../software/app/web/firmware/bin/medistride-left.bin

# 2. Compiler pour le pied droit
sed -i 's/"MediStride-L"/"MediStride-R"/' src/config.h
pio run -e xiao_esp32s3
cp .pio/build/xiao_esp32s3/firmware.bin \
   ../software/app/web/firmware/bin/medistride-right.bin

# 3. Restaurer config par défaut
git checkout src/config.h
```

> **À automatiser** : un workflow GitHub Actions qui compile les deux variantes
> à chaque tag `v*` et publie les `.bin` dans une release.

## Publication

Les binaires sont publiés sur la page
[Releases GitHub](https://github.com/HugJax/MediStride/releases).

L'app web (sur GitHub Pages ou hébergement HTTPS) référence ces fichiers via les
manifests ESP Web Tools.

## Vérification

Pour vérifier qu'un binaire est correct avant publication :

```bash
esptool.py --chip esp32s3 image_info firmware.bin
```

## Alternative : flashage manuel

Si vous préférez ne pas utiliser ESP Web Tools, voir
[`docs/guides/flashage.md`](../../../../docs/guides/flashage.md) pour la procédure PlatformIO.
