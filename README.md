# MAZE-SHOOTER

> Jeu de tir en labyrinthe en 3D développé en C avec la bibliothèque Raylib.

---

## Table des matières

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

MAZE-SHOOTER est un jeu de tir à la première personne dans lequel le joueur évolue dans un labyrinthe généré aléatoirement, combat des bots, et peut jouer en réseau local en mode multijoueur. Le projet est développé en C avec [Raylib 5.5](https://www.raylib.com/).

---

## Fonctionnalités

- **Menu principal** interactif (navigation clavier et souris)
- **Génération de labyrinthes** procédurale
- **Bots** resolution du labyrinthe (plus court chemin vers le joueur)
- **Système de tir** avec plusieurs armes (pistolet, mitraillette, fusil à pompe)
- **Audio dynamique** : musiques et effets sonores distincts selon l'état du jeu
- **Sauvegarde / Chargement** de partie avec checksum d'intégrité
- **Multijoueur en réseau local**
- **Options** (paramètres audio, visuels, etc.)
- **Système de logs** pour le débogage

---

## Prérequis

| Dépendance | Obligatoire ? | Usage |
|---|---|---|
| GCC | ✅ | Compilation C |
| Make | ✅ | Automatisation de la compilation |
| Doxygen | ❌  | Génération de documentation |

> Les bibliothèques Raylib pré-compilées sont déjà incluses dans `lib/`.

---

## Installation

Cloner le dépôt :

```bash
git clone git@github.com:thetsdz/MAZE-SHOOTER.git
cd MAZE-SHOOTER
```

Compiler le projet :

```bash
cd src
make
```


---
