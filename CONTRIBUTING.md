# Guide de contribution

Merci de votre intérêt pour MediStride ! Ce guide vous aidera à contribuer efficacement au projet.

## Comment contribuer

### 1. Signaler un bug ou proposer une amélioration

- Ouvrez une [issue](https://github.com/HugJax/MediStride/issues) sur GitHub
- Décrivez clairement le problème ou la proposition
- Ajoutez des captures d'écran ou des logs si pertinent

### 2. Soumettre du code

1. **Forkez** le dépôt
2. **Créez une branche** depuis `main` :
   ```bash
   git checkout -b feature/ma-fonctionnalite
   ```
3. **Développez** votre modification
4. **Testez** vos changements
5. **Committez** avec un message clair :
   ```bash
   git commit -m "Add: description courte de la modification"
   ```
6. **Poussez** votre branche :
   ```bash
   git push origin feature/ma-fonctionnalite
   ```
7. **Ouvrez une Pull Request** vers `main`

### 3. Contribuer au hardware

- Les schémas électroniques utilisent [Fritzing](https://fritzing.org/) ou [KiCad](https://www.kicad.org/)
- Les modèles 3D utilisent [FreeCAD](https://www.freecad.org/) ou [Fusion 360](https://www.autodesk.com/products/fusion-360)
- Fournissez les fichiers sources et les exports (PDF, STL)

## Conventions

### Commits

Utilisez des préfixes pour vos messages de commit :

| Préfixe | Usage |
|---------|-------|
| `Add:` | Nouvelle fonctionnalité |
| `Fix:` | Correction de bug |
| `Update:` | Amélioration d'une fonctionnalité existante |
| `Docs:` | Documentation |
| `Refactor:` | Refactorisation sans changement fonctionnel |
| `Test:` | Ajout ou modification de tests |
| `Hardware:` | Modification hardware (schémas, PCB, 3D) |

### Code

- **Firmware (C/C++)** : style Arduino, noms de variables en `camelCase`, constantes en `UPPER_SNAKE_CASE`
- **Python** : suivre [PEP 8](https://peps.python.org/pep-0008/), noms en `snake_case`
- Commentez le code en anglais pour maximiser l'accessibilité

### Branches

- `main` : branche stable
- `feature/*` : nouvelles fonctionnalités
- `fix/*` : corrections de bugs
- `hardware/*` : modifications matérielles

## Code de conduite

Ce projet adhère au [Contributor Covenant](https://www.contributor-covenant.org/version/2/1/code_of_conduct/). En participant, vous vous engagez à respecter ce code de conduite.

En résumé :
- Soyez respectueux et bienveillant
- Acceptez les critiques constructives
- Concentrez-vous sur ce qui est le mieux pour la communauté
- Faites preuve d'empathie envers les autres membres

## Questions ?

N'hésitez pas à ouvrir une [discussion](https://github.com/HugJax/MediStride/discussions) si vous avez des questions.