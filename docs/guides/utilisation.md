# Guide d'utilisation MediStride (pour soignants)

Ce guide s'adresse aux professionnels de santé et accompagnants qui utilisent
les semelles MediStride pour observer la marche d'un patient.

> ⚠️ **Avertissement** : MediStride n'est pas un dispositif médical certifié.
> Les données fournies sont une aide à l'observation et ne remplacent pas un
> diagnostic clinique. Voir [legal.md](../legal.md).

## Avant la session

### Matériel à préparer

- [ ] Paire de semelles MediStride chargées (au moins 2 h d'autonomie)
- [ ] Un ordinateur (Chrome/Edge) ou un téléphone Android avec Bluetooth activé
- [ ] L'application MediStride accessible : ouvrir `index.html` en local
      ou la version en ligne
- [ ] Une fiche patient (papier ou numérique) pour noter l'identifiant et le contexte

### Hygiène

Les semelles entrent en contact avec les pieds du patient. Avant chaque utilisation :

- nettoyer la **face supérieure** des semelles avec une lingette désinfectante adaptée ;
- vérifier qu'aucun fil n'est exposé ou abîmé ;
- préférer l'usage avec une **chaussette propre du patient** plutôt que pieds nus
  (réduit le risque infectieux et améliore la durabilité).

> 🧼 Pour un usage en établissement, mettre en place un protocole de désinfection
> entre patients. Les semelles ne sont pas immergeables.

## Démarrer une session

1. **Allumer** chaque semelle via son interrupteur (au talon).
2. Sur l'ordinateur, **ouvrir l'application**.
3. Cliquer **« Connecter le pied gauche »**, sélectionner `MediStride-L` dans la liste,
   puis **« Connecter le pied droit »** et sélectionner `MediStride-R`.
4. Les deux points en haut à droite passent au vert.
5. **Saisir l'identifiant patient** (ne JAMAIS saisir le nom complet ; utiliser un code).
6. Cliquer **« Démarrer la session »**.
7. Demander au patient de **marcher normalement** dans la pièce, sur 5 à 10 mètres,
   plusieurs allers-retours. Une session typique dure **2 à 5 minutes**.

## Pendant la session

L'application affiche en temps réel :

| Indicateur | Signification |
|---|---|
| **Cartographie gauche / droite** | Pression à chaque point du pied (chaud = forte pression) |
| **Force totale** | Somme des forces sous chaque pied, en Newtons |
| **Symétrie G/D** | 100 % = appuis parfaitement équilibrés. < 90 % = asymétrie notable |
| **Cadence** | Nombre de pas par minute (norme adulte : 100-130) |
| **Échantillons** | Nombre de mesures enregistrées (information technique) |
| **Batterie G / D** | Niveau de batterie de chaque module |

> 💡 La **symétrie** se stabilise après une dizaine de pas. Sur une session
> trop courte, la valeur est peu fiable.

## Terminer et exporter

1. Cliquer **« Arrêter »** pour figer la session.
2. Cliquer **« Exporter CSV »**. Un fichier
   `medistride-<patient>-<date>.csv` est téléchargé.
3. Ce fichier contient toutes les mesures brutes et peut être :
   - ouvert dans Excel, LibreOffice, Numbers ;
   - analysé en Python (voir [`software/analysis/`](../../software/analysis/)) ;
   - archivé dans le dossier patient (selon votre protocole RGPD).

## Bonnes pratiques

- **Toujours commencer par 30 secondes de marche d'échauffement** avant d'enregistrer,
  pour que le patient se sente à l'aise.
- **Faire marcher le patient pieds chaussés** plutôt que pieds nus si possible —
  cela améliore la stabilité de la mesure.
- **Comparer les sessions du même patient à des semaines d'intervalle** est plus
  pertinent que de comparer entre patients (les capteurs FSR ont une dispersion
  capteur-à-capteur de ±25 %).
- Noter le **contexte** dans le nom du fichier ou un journal séparé : douleur
  ressentie, chaussage, fatigue du patient, etc.

## Confidentialité et RGPD

Les données enregistrées sont des **données de santé**. Si vous les exploitez
en France :

- Utilisez un **identifiant pseudonymisé** (ex. `P-001`), jamais le nom du patient.
- Conservez le tableau de correspondance identifiant ↔ patient **séparément**,
  dans un endroit sécurisé.
- Informez le patient par écrit de l'usage qui sera fait des données.
- Le **patient peut demander à tout moment** la suppression de ses données.
- Voir [legal.md](../legal.md) pour le détail RGPD.

## En cas de problème

- **Module ne se connecte pas** → vérifier l'interrupteur, recharger la batterie,
  redémarrer le module (interrupteur OFF puis ON), voir [dépannage.md](depannage.md).
- **Données incohérentes** → recalibrer les capteurs ([calibration.md](calibration.md)).
- **Application ne démarre pas** → utiliser Chrome ou Edge, pas Firefox ni Safari.
