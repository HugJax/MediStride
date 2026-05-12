# PCB MediStride

Ce dossier est destiné à recevoir les fichiers de PCB (KiCad) pour la version
**finale rigide** ou **flex PCB** de MediStride.

## État actuel

🚧 **Pas encore conçu.** Pour le moment, MediStride se construit en câblage
volant à partir du module XIAO ESP32-S3. Voir [`../schematics/README.md`](../schematics/README.md)
pour le schéma électrique.

## Roadmap

| Version | Format | État |
|---|---|---|
| v1 — câblage volant | breadboard / mini-perfboard | ✅ documenté |
| v2 — PCB rigide | KiCad, 30 × 20 mm | 🔜 à concevoir |
| v3 — PCB flex semelle | KiCad + flex, forme de semelle | 🔜 à concevoir |

## Contribuer une PCB

Si vous souhaitez contribuer une PCB :

1. Utiliser **KiCad 8 ou 9** (cohérence avec le schéma déjà présent).
2. Respecter l'**assignation des broches** documentée dans
   [`../schematics/README.md`](../schematics/README.md).
3. Inclure les fichiers `.kicad_pro`, `.kicad_sch`, `.kicad_pcb` ainsi que les
   gerbers prêts pour fabrication (JLCPCB / PCBWay).
4. Documenter la BOM et les empreintes utilisées.

## Contraintes

- **Tension** : tout est en 3,3 V (sortie du XIAO).
- **Connecteurs** : un connecteur 8-pin (6 ADC + 3V3 + GND) pour les FSR.
- **Encombrement** : la PCB doit pouvoir tenir dans un boîtier de **35 × 25 × 8 mm**
  pour s'insérer dans une chaussure sans gêner le porteur.
- **Charge** : prévoir le passage d'un câble USB-C pour la recharge.
