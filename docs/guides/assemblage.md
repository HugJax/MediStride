# Guide d'assemblage MediStride

Ce guide décrit, étape par étape, comment construire **une paire de semelles MediStride**.
Il est destiné à des utilisateurs non spécialistes : il n'y a **rien à programmer** ici, juste
à assembler des composants. Comptez environ **2 à 3 heures** pour la première paire.

> ⚠️ Avant de commencer, lisez les [mentions légales](../legal.md). MediStride est un
> outil pédagogique et de recherche, pas un dispositif médical certifié.

## 1. Matériel à préparer

Voir la liste détaillée et les liens d'achat dans [matériel.md](materiel.md). Pour rappel,
pour **une paire** vous avez besoin de :

- 2× XIAO ESP32-S3
- 12× capteurs de force (FlexiForce A301 ou IMS-C20B 50 kg)
- 12× résistances 10 kΩ (1/4 W)
- 2× batteries LiPo 3,7 V 500 mAh avec connecteur JST-PH 2.0
- 2× interrupteurs SPST miniatures
- 2× condensateurs 100 nF + 2× condensateurs 10 µF
- 1 paire de semelles EVA (épaisseur 3-5 mm)
- Ruban de cuivre adhésif **ou** fil émaillé 30 AWG
- Fer à souder, étain sans plomb, pince à dénuder, multimètre, ciseaux

### Outils

- Fer à souder (température réglable, 320-350 °C)
- Pince coupante et pince à dénuder
- Multimètre (indispensable pour les tests)
- Cutter, ciseaux
- Pistolet à colle chaude **ou** ruban adhésif double-face très fin
- Marqueur indélébile

## 2. Vue d'ensemble du circuit

Chaque pied a un circuit identique :

```
                   +3V3 (du XIAO)
                     │
              ┌──────┴──────┐
              │             │
            [FSR]         (… 5 autres
              │            FSR identiques)
              ├──→ ADC du XIAO (A0..A5)
              │
            [10kΩ]
              │
             GND
```

Pour un schéma complet voir [`hardware/schematics/README.md`](../../hardware/schematics/README.md).

## 3. Préparer la semelle

1. **Tracer** au marqueur les 6 zones de mesure sur la semelle, vue de dessus :

```
       Gros orteil  (FSR6)        ◯
   1er méta (FSR4)  ◯         ◯   (FSR5) 3-4e méta
                       Arche
            Milieu du pied (FSR3) ◯
                       Talon
                   ◯       ◯
       Talon centre (FSR1)  (FSR2) Talon latéral
```

2. **Coller un FSR** à chaque emplacement avec un point de colle chaude
   (ou un petit carré d'adhésif double-face). Le côté actif (rond métallisé)
   doit être tourné vers la pression — donc **vers le haut**, côté pied.

3. **Acheminer les fils** de chaque FSR vers l'arrière de la semelle (talon).
   Utilisez le ruban de cuivre ou le fil émaillé en suivant des chemins courts
   et qui ne se croisent pas. Les deux pattes d'un FSR ne doivent jamais se toucher.

## 4. Câbler le module XIAO

> Repérez votre **pied gauche** et votre **pied droit** dès maintenant.
> Marquez chaque XIAO avec un point au marqueur (L ou R) pour ne pas les
> mélanger pendant le flashage.

### 4.1 Diviseurs de tension

Sur une mini-plaque (ou directement à l'arrière du XIAO), monter **6 diviseurs de tension** :

```
Pour chaque FSR (i = 1 à 6) :

    +3V3 ───┐
            │
            └──[FSR]──┬──→ broche A(i-1) du XIAO
                      │
                     [10kΩ]
                      │
                     GND
```

Concrètement :
- soudez une patte de chaque résistance 10 kΩ à la masse commune (rail GND) ;
- l'autre patte va à la broche ADC correspondante du XIAO **et** à une patte du FSR ;
- l'autre patte du FSR va au rail +3V3.

| Capteur | Broche XIAO | GPIO |
|---|---|---|
| FSR1 (talon centre) | A0 | GPIO1 |
| FSR2 (talon latéral) | A1 | GPIO2 |
| FSR3 (arche) | A2 | GPIO3 |
| FSR4 (1er méta) | A3 | GPIO4 |
| FSR5 (3-4e méta) | A4 | GPIO5 |
| FSR6 (gros orteil) | A5 | GPIO6 |

### 4.2 Découplage alimentation

Soudez **un condensateur 100 nF et un condensateur 10 µF** entre le rail +3V3 et le GND,
au plus près du XIAO. Le pôle `−` du 10 µF (électrolytique) va au GND.

### 4.3 Batterie et interrupteur

```
LiPo (+) ──→ [interrupteur SW1] ──→ BAT+ du XIAO
LiPo (−) ──────────────────────────→ BAT− du XIAO
```

Le XIAO ESP32-S3 a deux pads soudables `BAT+` et `BAT−` au dos.
L'interrupteur permet de couper la batterie sans devoir la débrancher.

> ⚠️ **Sécurité LiPo** : ne court-circuitez jamais une batterie LiPo. Si vous
> manipulez les pôles, isolez-les avec du ruban Kapton. Une LiPo gonflée ou
> qui chauffe doit être mise au rebut immédiatement dans un sac ignifuge.

## 5. Vérifications avant mise sous tension

Avec un **multimètre**, **batterie débranchée** :

1. Mesurez la résistance entre `+3V3` et `GND` du XIAO : doit être **> 1 kΩ**
   (sinon court-circuit, à corriger avant la suite).
2. Mesurez la résistance de chaque FSR au repos : doit être **très élevée**
   (généralement > 1 MΩ).
3. Vérifiez que chaque broche ADC est bien reliée à son FSR et à sa résistance pull-down.

## 6. Première mise sous tension

1. **Branchez le XIAO en USB-C** (sans la batterie pour ce premier test).
2. La LED du XIAO doit s'allumer.
3. Procédez au flashage du firmware via le **flasheur web** : voir
   [flashage.md](flashage.md).
4. Une fois le firmware installé, débranchez l'USB et branchez la batterie via l'interrupteur.

## 7. Test fonctionnel

Avec l'[application MediStride](../../software/app/web/) :

1. Allumez la semelle (interrupteur sur ON).
2. Sur Chrome ou Edge, cliquez **Connecter le pied gauche** (ou droit selon le module flashé).
3. Sélectionnez `MediStride-L` ou `MediStride-R` dans la liste.
4. Appuyez successivement sur chaque capteur avec le doigt : la zone correspondante
   doit s'allumer dans l'application.

Si un capteur ne réagit pas : voir [dépannage.md](depannage.md).

## 8. Intégration finale dans la semelle

Une fois le test validé, vous pouvez :

- coller une **fine couche d'EVA par-dessus** pour protéger les fils et capteurs ;
- fixer le boîtier électronique sur le **dos du talon** (côté extérieur de la chaussure)
  avec une bande Velcro pour pouvoir le retirer ;
- prévoir une **trappe d'accès** pour brancher l'USB-C lors des recharges.

## 9. Que faire ensuite ?

- Réaliser la **calibration** des capteurs : voir [calibration.md](calibration.md)
- Lire le **guide d'utilisation** pour une première session : [utilisation.md](utilisation.md)
- En cas de problème : [dépannage.md](depannage.md)
