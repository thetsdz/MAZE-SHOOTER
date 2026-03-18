# 🎮 MAZE-SHOOTER

> Jeu de tir en labyrinthe en 3D développé en C avec la bibliothèque Raylib.

---

## 📋 Table des matières

- [Présentation](#présentation)
- [Fonctionnalités](#fonctionnalités)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Compilation & Lancement](#compilation--lancement)
- [Structure du projet](#structure-du-projet)
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
- **Bots** avec intelligence artificielle (pathfinding)
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

## Structure du projet

```
MAZE-SHOOTER/
├── bin/                    # Binaire compilé
│   └── game
├── doc/                    # Documentation générée par Doxygen
│   └── html/
│       └── index.html
├── lib/
│   ├── headers/            # Fichiers d'en-tête (.h)
│   │   ├── asset.h
│   │   ├── audio.h
│   │   ├── bot.h
│   │   ├── cryptage.h
│   │   ├── dessin.h
│   │   ├── level.h
│   │   ├── log.h
│   │   ├── menu.h
│   │   ├── multijoueur.h
│   │   ├── option.h
│   │   ├── pile.h
│   │   ├── player.h
│   │   ├── projectile.h
│   │   ├── reseau.h
│   │   ├── sauvegarde.h
│   │   ├── types.h
│   │   └── updategame.h
│   └── linux/
│       └── raylib-5.5_linux_amd64/   # Raylib pré-compilé Linux
├── src/                    # Code source
│   ├── asset.c             # Gestion des ressources (textures, modèles)
│   ├── audio.c             # Gestion de l'audio (musiques & sons)
│   ├── bot.c               # Intelligence artificielle des ennemis
│   ├── cryptage.c          # Chiffrement (sauvegardes)
│   ├── dessin.c            # Rendu graphique 3D
│   ├── level.c             # Génération et gestion du labyrinthe
│   ├── log.c               # Système de logs
│   ├── main.c              # Point d'entrée, boucle principale
│   ├── menu.c              # Menu principal
│   ├── multijoueur.c       # Logique multijoueur
│   ├── option.c            # Écran des options
│   ├── pile.c              # Structure de données pile (pathfinding)
│   ├── player.c            # Gestion du joueur
│   ├── projectile.c        # Gestion des projectiles
│   ├── reseau.c            # Communication réseau
│   ├── sauvegarde.c        # Sauvegarde et chargement de partie
│   ├── updategame.c        # Mise à jour de l'état du jeu
│   ├── Makefile            # Makefile principal
│   └── Makefile.linux      # Makefile spécifique Linux
├── Doxyfile                # Configuration Doxygen
└── README.md
```

---

## Contrôles

| Touche | Action |
|---|---|
| `↑` / `↓` | Naviguer dans les menus |
| `Entrée` | Valider / Sélectionner |
| `Z` `Q` `S` `D` | Se déplacer |
| `Souris` | Viser |
| `Clic gauche` | Tirer |
| `R` | Recharger |
| `Échap` | Pause / Retour au menu |

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

### Intégrer master dans sa branche

```bash
git checkout ma-branche
git merge master
git push origin ma-branche
```

---

## Auteurs

- **Hugues Astier** — menu, audio, interface
- **Corentin** — réseau, multijoueur, audio
- **Thomas** — gameplay, bots, level design

---

> Projet universitaire — L2 Informatique
