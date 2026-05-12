# Publier une release MediStride

Ce document décrit la procédure pour :

1. Activer le déploiement automatique de l'application sur GitHub Pages.
2. Publier une release versionnée avec les binaires firmware.

## 1. Activer GitHub Pages (à faire une seule fois)

1. Aller sur la page de ton dépôt GitHub.
2. **Settings → Pages**.
3. Sous **Source**, choisir **GitHub Actions**.
4. Sauver.

À la prochaine push sur `main`, le workflow `pages-deploy.yml` :
- compile les firmwares `medistride-left.bin` et `medistride-right.bin` ;
- les place dans `software/app/web/firmware/bin/` ;
- déploie l'ensemble du dossier `software/app/web/` sur GitHub Pages.

L'URL publique sera de la forme `https://<ton-utilisateur>.github.io/MediStride/`.

> Tant que Pages n'est pas activé, le job `deploy` échouera mais le build
> firmware fonctionnera et tu peux récupérer les binaires dans les artifacts
> du workflow.

## 2. Tester le workflow Pages dès maintenant

Avant de pousser, tu peux déclencher le workflow à la main depuis l'onglet
**Actions** du dépôt → **Deploy MediStride app to GitHub Pages** → **Run workflow**.

## 3. Publier une release versionnée

Une release publie les `.bin` firmware sur la page **Releases** du dépôt
GitHub, ce qui est utile pour le flashage manuel et la traçabilité clinique.

```bash
# Depuis la racine du projet, sur la branche main à jour :
git pull
git tag v1.0.0
git push origin v1.0.0
```

Le workflow `firmware-build.yml` se déclenche automatiquement sur les tags
`v*` et :
- compile les trois variantes (`xiao_esp32s3_left`, `xiao_esp32s3_right`,
  `xiao_esp32s3` universel) ;
- génère un fichier `SHA256SUMS.txt` ;
- crée une **release GitHub** avec les `.bin` et le SHA en attachés.

Aucune intervention manuelle n'est requise sur la release elle-même.

## 4. Vérifier le résultat

- **Pages** : ouvrir `https://<ton-utilisateur>.github.io/MediStride/` et
  vérifier que l'app se charge sur Chrome / Edge.
- **Release** : aller sur `https://github.com/HugJax/MediStride/releases` et
  vérifier que les trois `.bin` sont attachés.
- **Vérification d'intégrité** côté utilisateur :
  ```bash
  sha256sum -c SHA256SUMS.txt
  ```

## 5. Procédure de hotfix

Pour publier un correctif urgent :

```bash
git checkout main
# ... corriger le bug
git commit -m "Fix: description"
git push
git tag v1.0.1
git push origin v1.0.1
```

Pages est redéployé automatiquement sur la push main, et la release v1.0.1
est créée sur la push du tag.

## 6. Numérotation des versions

Suivre [SemVer](https://semver.org/lang/fr/) :

- `v1.0.0` : première version publique stable.
- `v1.0.X` : correctifs de bug, pas de changement d'usage.
- `v1.X.0` : nouvelles fonctionnalités rétro-compatibles.
- `v2.0.0` : changement cassant (protocole BLE, format CSV, etc.).
