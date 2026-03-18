# 🎮 MAZE-SHOOTER

> Jeu de tir en labyrinthe en 3D développé en C avec la bibliothèque Raylib.

---

## 📋 Table des matières

- [Présentation](#présentation)
- [Fonctionnalités](#fonctionnalités)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Compilation & Lancement](#compilation--lancement)
- [Contrôles](#contrôles)
- [Multijoueur](#multijoueur)
- [Sauvegarde](#sauvegarde)
- [Documentation](#documentation)
- [Branches & Workflow Git](#branches--workflow-git)
- [Auteurs](#auteurs)

---

## Présentation

MAZE-SHOOTER est un jeu de tir à la première personne dans lequel le joueur évolue dans un labyrinthe généré, combat des bots, et peut jouer en réseau local en mode multijoueur. Le projet est développé en C (norme C99) avec [Raylib 5.5](https://www.raylib.com/).

---

## Fonctionnalités

- **Menu principal** interactif (navigation clavier et souris)
- **Génération de labyrinthes** procédurale
- **Bots** resolution du labyrinthe(plus court chemin vers le joueur)
- **Système de tir** avec plusieurs armes (pistolet, mitraillette, fusil à pompe)
- **Audio dynamique** : musiques et effets sonores distincts selon l'état du jeu
- **Sauvegarde / Chargement** de partie avec checksum d'intégrité
- **Multijoueur en réseau local**
- **Options** (paramètres audio, visuels, etc.)
- **Système de logs** pour le débogage

---

## Prérequis

| Dépendance | Version | Usage |
|---|---|---|
| GCC | ≥ 13 | Compilation C99 |
| Raylib | 5.5 | Moteur graphique & audio |
| Make | toute | Automatisation de la compilation |
| Doxygen | toute | Génération de documentation (optionnel) |

> Les bibliothèques Raylib pré-compilées sont déjà incluses dans `lib/`.

---

## Installation

Cloner le dépôt :

```bash
git clone git@github.com:thetsdz/MAZE-SHOOTER.git
cd MAZE-SHOOTER
```

Aucune installation de dépendance supplémentaire n'est nécessaire, Raylib est embarqué dans le projet.

---

## Compilation & Lancement

### Linux

```bash
cd src
make run
```
### Windows

```bash
cd src
make run
```
### Mac

```bash
cd src
make run
```

### Nettoyage des fichiers objets

```bash
cd src
make clean
```

### Compiler sans lancer

```bash
cd src
make
```

> Le binaire compilé est placé dans `bin/game`.

---

## Contrôles

| Touche | Action |
|---|---|
| `↑` / `↓` | Naviguer dans les menus |
| `Entrée` | Valider / Sélectionner |
| `Z` `Q` `S` `D` | Se déplacer |
| `Souris` | Changer l'orientation de la caméra |
| `Clic gauche` | Tirer |
| `R` | Recharger |
| `Échap` | Quitter le jeu |
| `back-space` | revenir au menui précédent |

---

## Multijoueur

Le mode multijoueur fonctionne en **réseau local (LAN)**. Un joueur héberge la partie et les autres s'y connectent via l'adresse IP locale.

La communication réseau est gérée dans `src/reseau.c` et `src/multijoueur.c`.

---

## Sauvegarde

Les sauvegardes sont stockées localement et protégées par un **checksum d'intégrité** pour détecter toute corruption. Les données sauvegardées incluent : position du joueur, santé, munitions, score, état du labyrinthe et état des bots.

---

## Documentation

La documentation du code source est générée avec **Doxygen** :

```bash
cd MAZE-SHOOTER
doxygen Doxyfile
```

Ouvrir ensuite :

```bash
xdg-open doc/html/index.html
```

---

## Branches & Workflow Git

| Branche | Responsable | Rôle |
|---|---|---|
| `master` | — | Branche principale stable |
| `Hugues` | Hugues Astier | Développement Hugues |
| `corentin` | Corentin | Développement Corentin |
| `Thomas` | Thomas | Développement Thomas |


## Auteurs

- **Hugues Astier** — menu, changement d'armes, interface
- **Corentin** — réseau, multijoueur, gameplay, bots
- **Thomas** — audio, level, design

---

> Projet universitaire — L2 Informatique
