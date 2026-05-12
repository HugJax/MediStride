# Mentions légales et confidentialité

## Statut du dispositif

**MediStride n'est pas un dispositif médical au sens de la réglementation européenne
(règlement UE 2017/745 « MDR ») ni de la FDA américaine.** Il n'a pas fait l'objet
d'une procédure de certification CE médicale, ni d'un marquage CE 0123, ni d'un
agrément de la HAS, de l'ANSM ou d'un organisme équivalent.

À ce titre :

- MediStride **ne doit pas être utilisé pour poser un diagnostic médical**,
  pour prendre une décision thérapeutique seul, ou pour remplacer l'évaluation
  d'un professionnel de santé qualifié.
- MediStride est fourni à des fins **pédagogiques, de recherche et d'observation**
  uniquement.
- Les mesures fournies sont des **estimations** soumises à la dispersion des
  capteurs commerciaux et à la qualité de l'assemblage. Voir
  [calibration.md](guides/calibration.md).
- Toute utilisation clinique réelle relève de la **responsabilité du
  professionnel** qui choisit d'employer l'outil.

## Sécurité matérielle

- Les batteries LiPo utilisées sont des composants industriels standards.
  Elles **doivent être manipulées avec précaution** : ne pas percer, ne pas
  court-circuiter, ne pas exposer à des températures supérieures à 60 °C.
- Une batterie LiPo gonflée, chaude ou présentant un défaut visible doit être
  **immédiatement mise hors tension** et apportée à un point de collecte adapté.
- L'utilisateur doit s'assurer que l'électronique ne crée aucun risque d'inconfort,
  de blessure ou d'allergie au contact du pied (vérifier l'isolation des fils,
  éviter les arêtes vives).

## Hygiène

Les semelles étant en contact avec les pieds :

- nettoyer / désinfecter la **face supérieure** entre chaque patient ;
- éviter l'usage pieds nus, préférer une chaussette propre ;
- ne **jamais immerger** la semelle ou l'électronique ;
- pour un usage en établissement, mettre en place un protocole de désinfection
  écrit et vérifier la compatibilité des produits avec les matériaux utilisés.

---

## Protection des données (RGPD)

Toute donnée enregistrée par MediStride et associée à un individu constitue une
**donnée de santé** au sens de l'article 9 du RGPD. À ce titre, son traitement
est soumis à des exigences renforcées.

### Si vous utilisez MediStride dans un cadre privé / domestique

Le RGPD ne s'applique pas aux usages strictement personnels et familiaux.
Vous pouvez utiliser l'outil pour vous-même ou un proche sans formalité,
mais nous recommandons malgré tout de **ne pas partager les données brutes**
sans précaution.

### Si vous utilisez MediStride dans un cadre professionnel (kiné, médecin, recherche)

Vous devez :

1. **Identifier la base légale du traitement** : consentement du patient,
   intérêt légitime (recherche), ou exécution d'une mission d'intérêt public.
2. **Informer le patient par écrit** : finalité, durée de conservation,
   destinataires, droits d'accès / rectification / effacement / opposition.
3. **Pseudonymiser les identifiants** dans les exports CSV (utiliser `P-001`
   plutôt que `Dupont Jean`).
4. **Conserver les données dans un environnement sécurisé** : disque chiffré,
   accès restreint, sauvegardes contrôlées.
5. **Tenir un registre des traitements** si votre activité y est soumise.
6. **Permettre à tout moment au patient** :
   - d'accéder à ses données,
   - de demander leur rectification,
   - de demander leur effacement,
   - de retirer son consentement.

### Localisation des données

L'application web MediStride **ne transmet aucune donnée vers un serveur tiers**.
Toutes les mesures restent sur l'appareil de l'utilisateur (navigateur), et les
exports CSV sont téléchargés directement par l'utilisateur. Aucun fichier n'est
hébergé sur Internet par MediStride.

Si vous décidez d'héberger l'application sur votre propre serveur ou un service
tiers (GitHub Pages, etc.), assurez-vous que la connexion est en **HTTPS** et que
votre hébergeur respecte les exigences RGPD (de préférence un hébergeur situé
dans l'UE).

### Hébergement de données de santé en France

En France, l'hébergement de données de santé à caractère personnel est soumis
à une **certification HDS (Hébergeur de Données de Santé)** délivrée par
l'Agence du Numérique en Santé. Si vous prévoyez de stocker les exports sur
un serveur partagé ou cloud, vérifiez que celui-ci est certifié HDS, ou
conservez les données strictement en local sur un poste sécurisé.

---

## Licence

Le code source, les schémas et la documentation sont distribués sous licence
[Apache 2.0](../LICENSE). Cette licence permissive autorise notamment l'usage
commercial, à condition de conserver la mention de copyright et la licence.

> **Attention** : la licence Apache 2.0 fournit le projet « tel quel », sans
> aucune garantie. Les contributeurs et auteurs déclinent toute responsabilité
> quant aux conséquences d'un usage de MediStride, en particulier dans un
> contexte médical.

## Contact

Pour toute question relative à ces mentions, ouvrir une issue sur
[github.com/HugJax/MediStride](https://github.com/HugJax/MediStride/issues)
ou contacter le mainteneur du projet par courriel.
