# Tutoriel pas à pas : construire MediStride de zéro

Ce tutoriel vous accompagne **depuis l'achat des composants jusqu'à la première
session de mesure** avec un patient. Aucun prérequis technique avancé n'est
nécessaire : si vous savez tenir un fer à souder et suivre une recette, vous
pouvez réaliser MediStride.

**Durée totale estimée** : 1 demi-journée pour la commande + 2 à 3 heures
d'assemblage + 30 minutes de tests.

> ⚠️ **Avant de commencer** : lisez les [mentions légales](../legal.md).
> MediStride n'est pas un dispositif médical certifié et ne remplace pas
> l'avis d'un professionnel de santé qualifié.

---

## Étape 1 — Commander les composants

Reportez-vous à la liste détaillée dans [matériel.md](materiel.md).
Pour **une paire de semelles**, en option économique :

### Composants principaux

| Quantité | Composant | Où acheter | Prix estimé |
|---:|---|---|---:|
| 2 | XIAO ESP32-S3 | [Seeed Studio](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html) | 7-10 USD pièce |
| 12 | Capteur de force IMS-C20B 50 kg *ou* FlexiForce A301 445 N | AliExpress / DigiKey | 5-20 USD pièce |
| 2 | Batterie LiPo 3,7 V 500 mAh JST-PH 2.0 | Adafruit / AliExpress | 4-6 USD pièce |
| 12 | Résistance 10 kΩ 1/4 W (kit assorti) | AliExpress / Amazon | 5 USD le kit |
| 2 | Condensateur céramique 100 nF | AliExpress | < 1 USD le lot |
| 2 | Condensateur électrolytique 10 µF / 6,3 V | AliExpress | < 1 USD le lot |
| 2 | Interrupteur SPST miniature | AliExpress | 1 USD pièce |
| 1 | Rouleau ruban de cuivre adhésif (6 mm × 15 m) | Amazon | 5-7 USD |
| 1 | Bobine fil émaillé 30 AWG | AliExpress | 3-5 USD |
| 1 | Paire de semelles EVA (3-5 mm) | Amazon | 3-8 USD |

**Budget total : environ 110 USD** la paire en option économique.

### Outils

Si vous n'avez pas :
- Fer à souder à température réglable (~25 USD)
- Étain sans plomb (~10 USD)
- Multimètre numérique basique (~15 USD)
- Pince coupante + pince à dénuder (~10 USD)

### Conseils d'achat

- **Vérifiez les délais de livraison** : AliExpress peut prendre 3-4 semaines.
  Pour démarrer plus vite, privilégiez Amazon / DigiKey pour les composants
  critiques (microcontrôleur, batterie).
- **Achetez en double** : un FSR cassé en cours d'assemblage et il faut
  recommencer la commande. Prévoyez 2-3 capteurs supplémentaires.
- **Faites attention aux contrefaçons** : sur les capteurs FlexiForce, achetez
  chez DigiKey ou Mouser pour avoir l'authentique.

---

## Étape 2 — Installer les outils logiciels

Pendant que les composants sont en livraison, préparez votre poste.

### 2.1 Sur votre ordinateur

1. **Navigateur** : installez [Google Chrome](https://www.google.com/chrome/) ou
   [Microsoft Edge](https://www.microsoft.com/edge). Firefox et Safari ne fonctionneront pas.
2. **Câble USB-C de qualité** : un câble qui transmet les données, pas seulement
   l'alimentation. Testez-le en branchant un téléphone : si l'ordinateur le
   reconnaît, c'est bon.

### 2.2 Si vous souhaitez modifier le firmware (optionnel)

Si vous voulez compiler le firmware vous-même (sinon, on utilisera le
flasheur web — pas besoin d'installer quoi que ce soit) :

```bash
# Linux / macOS
pip install platformio
```

Sur Windows, installer VS Code et l'extension PlatformIO IDE.

### 2.3 Récupérer le projet

```bash
git clone https://github.com/HugJax/MediStride.git
cd MediStride
```

---

## Étape 3 — Vérifier la réception des composants

Au déballage :

- [ ] Les XIAO ESP32-S3 s'allument bien quand on les branche en USB-C.
      Une LED orange (charge) ou verte (alimentation) doit s'allumer.
- [ ] Les FSR sont **intacts**, sans pli marqué ni perforation.
- [ ] Les batteries LiPo ne sont pas **gonflées** ni endommagées.
      Un léger renflement = à mettre au rebut, ne pas utiliser.
- [ ] Les résistances 10 kΩ ont bien les anneaux **marron-noir-orange-or**
      (vérifier au multimètre : doit lire environ 10 000 Ω).

---

## Étape 4 — Premier test « à blanc » avec un seul capteur

Avant de monter la semelle complète, validez la chaîne avec **un seul FSR**.

### 4.1 Câblage minimal

Sur une mini-breadboard (ou en câblage volant) :

```
+3V3 du XIAO ──┬── une patte du FSR
               │
               └── autre patte du FSR ──┬── broche A0 du XIAO
                                         │
                                       [10 kΩ]
                                         │
                                        GND
```

### 4.2 Flasher le firmware

1. **Lancer le flasheur web** :
   - démarrer un petit serveur depuis le dossier du projet :
     ```bash
     cd software/app/web
     python3 -m http.server 8000
     ```
   - ouvrir Chrome sur [http://localhost:8000/flash.html](http://localhost:8000/flash.html).
2. **Mettre le XIAO en mode programmation** : maintenir `BOOT`, appuyer brièvement
   sur `RESET`, relâcher `BOOT`.
3. Cliquer **« Installer (gauche) »** et sélectionner le port série du module.
4. Attendre la fin du flashage (~30 s).

> 💡 Si le flasheur web ne marche pas chez vous, suivez le [guide PlatformIO
> manuel](flashage.md#méthode-b--platformio-pour-développeurs).

### 4.3 Premier test BLE

1. Ouvrir [http://localhost:8000/](http://localhost:8000/) dans Chrome.
2. Cliquer **« Connecter le pied gauche »** et sélectionner `MediStride-L`.
3. Appuyer sur le FSR avec votre pouce :
   - **Le capteur numéro 1 du pied gauche doit s'allumer en temps réel.**
   - La force totale doit varier entre 0 N (au repos) et 20-50 N (appui pouce).

🎉 **Si vous voyez la valeur changer, votre chaîne est fonctionnelle !** Vous
pouvez maintenant assembler la paire complète.

### 4.4 Si rien ne réagit

Voir [dépannage.md](depannage.md). Le plus souvent :
- mauvais sens du FSR (le côté actif est métallisé, il doit recevoir la pression) ;
- résistance pull-down mal branchée ;
- court-circuit entre +3V3 et GND.

---

## Étape 5 — Assembler la première semelle (pied gauche)

Suivez le [guide d'assemblage détaillé](assemblage.md). En résumé :

1. **Marquer** au stylo les 6 emplacements de capteurs sur la semelle EVA.
2. **Coller** un FSR à chaque emplacement (côté actif vers le haut).
3. **Câbler** chacun vers une mini-perfboard à l'arrière du talon, en passant
   par un diviseur de tension (résistance 10 kΩ vers GND, sortie vers la broche
   ADC correspondante).
4. **Souder** les rails +3V3 et GND.
5. **Ajouter** les condensateurs de découplage (100 nF + 10 µF) au plus près du XIAO.
6. **Brancher** la batterie LiPo via l'interrupteur, **batterie débranchée**
   pendant tout le câblage par sécurité.
7. **Souder le pont GPIO9 → GND** pour identifier ce module comme pied **gauche**.

> 🔧 **Astuce** : numérotez chaque fil à mesure que vous soudez (marqueur
> permanent ou ruban d'identification). Six fils qui se ressemblent, c'est vite
> mélangé.

### Vérifications avant mise sous tension

Multimètre en mode résistance :
- entre `+3V3` et `GND` du XIAO : **> 1 kΩ** (sinon court-circuit) ;
- chaque broche ADC → GND : environ **10 kΩ** (pull-down présente) ;
- chaque broche ADC → +3V3 : varie selon la pression sur le FSR.

---

## Étape 6 — Flasher la première semelle

Mode programmation (BOOT + RESET) → Flasheur web → bouton **« Installer (gauche) »**.

Vérifier que le module annonce bien `MediStride-L` en BLE.

> 💡 Le pont de soudure GPIO9 ↔ GND sert à choisir le côté **sans recompiler le
> firmware**. Si vous le retirez et reflashez sans pont, le module se nommera
> `MediStride-R`. Le pont peut aussi être contourné par configuration BLE
> (voir [calibration.md](calibration.md)).

---

## Étape 7 — Tester la première semelle complète

1. Allumer le module (interrupteur ON).
2. Sur Chrome, ouvrir l'app, connecter `MediStride-L`.
3. Appuyer successivement sur chaque capteur 1 à 6 :
   les six zones doivent s'allumer dans la cartographie.

Marcher en chaussant la semelle :
- la force totale doit varier de 0 (pied levé) à environ 500-1000 N (appui plein) ;
- les zones talon → arche → avant-pied doivent s'allumer dans l'ordre.

---

## Étape 8 — Construire le pied droit

Répétez les étapes 5 à 7 pour le second module, **mais sans le pont GPIO9 ↔ GND**
(c'est ce qui fait basculer le firmware en mode « pied droit »).

Vérifier que ce module annonce bien `MediStride-R` en BLE.

---

## Étape 9 — Calibrer (optionnel mais recommandé)

Pour obtenir des Newtons fiables (pas seulement un ordre de grandeur), suivez
le [guide de calibration](calibration.md), section **« Niveau B »**.

Vous aurez besoin :
- d'une balance précise ;
- de poids de référence (bouteilles d'eau, pack, votre poids) ;
- de 15 minutes par capteur.

À défaut, la **calibration de niveau A** (par défaut) suffit pour observer
la symétrie et la cadence.

---

## Étape 10 — Première session avec un utilisateur

1. **Charger** les deux semelles à 100 %.
2. **Désinfecter** la face supérieure des semelles avant utilisation
   (lingette adaptée).
3. **Glisser** les semelles dans une paire de chaussures fermées et
   confortables. Préférer des chaussettes propres à l'usage pieds nus.
4. **Allumer** les deux modules (interrupteur ON sur chacun).
5. **Ouvrir** l'app sur Chrome ou Edge, connecter les deux pieds.
6. **Saisir un identifiant** pseudonymisé (ex. `P-001`, jamais le nom complet).
7. **Cliquer « Démarrer la session »**.
8. **Faire marcher l'utilisateur** sur 5-10 mètres en aller-retour pendant
   2-5 minutes.
9. **Cliquer « Arrêter »** puis **« Exporter CSV »** pour archiver les données.

Voir le [guide d'utilisation](utilisation.md) pour le détail clinique.

---

## Étape 11 — Analyser les résultats

Pour aller plus loin que la visualisation temps réel :

```bash
cd software/analysis
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

python -m medistride ~/Téléchargements/medistride-P-001-2026-05-12T10-00-00.csv
```

Vous obtiendrez :
- résumé numérique (durée, pas, symétrie, cadence, répartition par zone) ;
- graphique de force totale au cours du temps ;
- répartition moyenne par capteur en barres.

---

## Étape 12 — Maintenir, améliorer, partager

### Maintenance régulière

- **Vérifier** l'état des soudures et des fils tous les mois.
- **Charger** les batteries au moins une fois par mois, même si non utilisées
  (les LiPo se détériorent quand laissées déchargées).
- **Nettoyer** les semelles entre chaque utilisateur.

### Améliorations possibles

- Imprimer un **boîtier 3D** pour protéger l'électronique
  (voir [`hardware/3d-models/`](../../hardware/3d-models/README.md)).
- Concevoir une **PCB flex** pour une intégration plus durable
  (voir [`hardware/pcb/`](../../hardware/pcb/README.md)).
- Ajouter un **IMU MPU-6050** pour détecter les phases du cycle de marche
  (voir le schéma [`hardware/schematics/`](../../hardware/schematics/README.md)).

### Partager

Le projet est open source sous licence Apache 2.0. Vos contributions sont
les bienvenues — voir [CONTRIBUTING.md](../../CONTRIBUTING.md).

Documentez votre expérience, vos améliorations, vos données anonymisées :
ouvrir une [discussion GitHub](https://github.com/HugJax/MediStride/discussions)
fait avancer le projet pour tout le monde.

---

## Récapitulatif des étapes

```
1. Commander les composants                    1-3 semaines (livraison)
2. Installer Chrome / Edge                     5 min
3. Vérifier la réception                       10 min
4. Tester avec 1 seul capteur                  30 min
5. Assembler la 1re semelle                    1h-1h30
6. Flasher                                     5 min
7. Tester complet                              15 min
8. Construire la 2e semelle                    1h-1h30
9. Calibrer (optionnel)                        30 min
10. Première session                           15 min
11. Analyser                                   10 min
12. Maintenir et améliorer                     ongoing
```

Total : **~5 h d'assemblage** une fois les composants reçus.

🎉 **Vous avez votre paire MediStride !**

Pour toute question, ouvrir une [discussion GitHub](https://github.com/HugJax/MediStride/discussions)
ou une [issue](https://github.com/HugJax/MediStride/issues) en cas de bug.
