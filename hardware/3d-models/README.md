# Modèles 3D MediStride

Ce dossier est destiné à recevoir les **modèles 3D imprimables** pour MediStride.

## État actuel

🚧 **Pas encore disponibles.** Les premières paires sont assemblées avec un
boîtier improvisé (boîte ABS du commerce, mousse EVA, etc.).

## À concevoir

| Pièce | Description | Format |
|---|---|---|
| **Boîtier électronique** | Boîte fermée contenant XIAO ESP32-S3 + LiPo + interrupteur, fixée au talon. Ouverture latérale pour le câble USB-C de recharge. | STL + STEP |
| **Trappe d'accès** | Fenêtre amovible pour accéder à l'interrupteur sans démonter. | STL |
| **Guide capteurs** | Gabarit pour positionner précisément les 6 FSR sur la semelle. | STL + DXF |
| **Support semelle** | Cale en TPU souple intégrant les capteurs (alternative à l'EVA). | STL |

## Contraintes de conception

- **Boîtier électronique** :
  - dimensions intérieures min. 40 × 30 × 12 mm pour accueillir XIAO + LiPo 500 mAh ;
  - ouverture latérale pour USB-C (largeur 11 mm) ;
  - clip ou vis M2 pour ouverture facile ;
  - matériau : PLA suffisant pour le prototype, ABS ou PETG pour la durabilité.
- **Pièces en contact pied** :
  - bords arrondis, pas d'arête saillante ;
  - matériau biocompatible (TPU souple plutôt que PLA dur) ;
  - épaisseur minimale (idéalement < 5 mm) pour ne pas gêner la marche.

## Contribuer un modèle

1. Fournir le **fichier source** (FreeCAD, Fusion 360, OpenSCAD) en plus du STL.
2. Documenter les **paramètres d'impression** recommandés (température, vitesse,
   support, infill).
3. Préciser le **matériau testé** et la durabilité observée.
4. Joindre des **photos** de la pièce imprimée et installée.

## Outils suggérés

- [FreeCAD](https://www.freecad.org/) (libre, recommandé)
- [Fusion 360](https://www.autodesk.com/products/fusion-360) (gratuit pour usage personnel)
- [OpenSCAD](https://openscad.org/) (libre, pour pièces paramétriques)
