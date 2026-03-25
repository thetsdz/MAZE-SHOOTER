#ifndef TYPES_H
#define TYPES_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 11/01/2026
 * \brief Définition des types de données utilisés dans le projet
 */
/** \version 1.1
 * \author Corentin Jammes
 * \date 15/01/2026
 * \brief ajout de 2 types de données pour le type player (les munitions)
 */
/** \version 2.0
 * \author Thomas Dequirez
 * \date 22/01/2026
 * \brief ajout du type coor 2D pour la generation du labyrinthe + ajout d'un
 * type isWall pour différencier les murs des couloirs dans le labyrinthe
 */
/** \version 3.0
 * \author Corentin Jammes
 * \date 24/01/2026
 * \brief ajout d'un type bot
 */
/** \version 3.0
 * \author Corentin Jammes
 * \date 24/01/2026
 * \brief ajout d'un type bot
 */
/** \version 5.0
 * \author Thomas Dequirez
 * \date 03/02/2026
 * \brief suppresion des types player et bot au profit d'un type plus générique
 * entity avec un champ pour différencier les types d'entités
 */
/** \version 6.0
 * \author Thomas Dequirez
 * \date 06/02/2026
 * \brief completion du type GameScreen pour ajouter les valeurs EXIT,OPTION et
 * multi
 */
/** \version 6.1
 * \author Hugues Astier
 * \date 06/02/2026
 * \brief remplacement de la valeur test par GAME dans le type GameScreen
 */
/** \version 6.2
 * \author Hugues Astier
 * \date 11/02/2026
 * \brief remplacement de la valeur de multi par MULTIJOUEUR et la valeur GAME
 * par NOUVELLE PARTIE, et ajout de charger partie
 */
/** \version 7.0
 * \author Corentin Jammes
 * \date 02/03/2026
 * \brief ajout de tot les types necessaire pour un multijoueur en reseau (type
 * pour différencier les projectiles tirés par le joueur, les bots et l'autre
 * joueur humain, type pour stocker l'état de la connexion réseau, type pour
 * stocker les données échangées entre les joueurs à chaque frame)
 */

/** \version 8.0
 * \author Hugues Astier
 * \date 18/03/2026
 * \brief ajout des types armes, des types projectiles, de modelearme et de
 * quelque champs dans entity
 */

#include "raylib.h"

/** @brief Nombre total de blocs dans le labyrinthe*/
#define NUM_BLOCKS 51
/** @brief Nombre maximal de projectiles actifs simultanément*/
#define MAX_PROJ 50
/** @brief Score obtenu par échange ou action spécifique*/
#define SCORE_TRADE 100

/** @brief Valeurs booléennes personnalisées*/
#define TRUE 1
#define FALSE 0

/**
 * @enum GameScreen
 * @brief Définit les écrans disponibles dans le jeu.
 */
// types.h

typedef enum GameScreen {
  MENU,
  NOUVELLE_PARTIE,
  MULTIJOUEUR,
  CHARGER_PARTIE,
  OPTIONS,
  GAME_OVER,
  VICTOIRE,
  EXIT
} GameScreen;  // Ajoute EXIT

/**
 * @enum EntityType
 * @brief Définit le type d'une entité (joueur ou bot).
 */
typedef enum {
  ENTITY_PLAYER,       /**< Entité contrôlée par le joueur. */
  ENTITY_BOT,          /**< Entité contrôlée par l'IA. */
  ENTITY_REMOTE_PLAYER /** < L'autre joueur humain (reseau). */
} EntityType;

/**
 * @enum OwnerType
 * @brief Définit le propriétaire d'un projectile.
 */
typedef enum {
  OWNER_PLAYER,       /**< Projectile tiré par le joueur. */
  OWNER_BOT,          /**< Projectile tiré par un bot. */
  OWNER_REMOTE_PLAYER /**< Projectile tiré par l'autre joueur humain (reseau).
                       */
} OwnerType;

/**
 * @struct Coord
 * @brief Coordonnées pour la pile DFS (parcours du labyrinthe).
 */
typedef struct {
  int i; /**< Indice de ligne. */
  int j; /**< Indice de colonne. */
} Coord;

/**
 * @struct Block
 * @brief Représente un bloc du labyrinthe (mur ou couloir).
 */
typedef struct {
  Vector3 pos;  /**< Position centrale du bloc. */
  float width;  /**< Largeur du bloc (axe X). */
  float height; /**< Hauteur du bloc (axe Y). */
  float depth;  /**< Profondeur du bloc (axe Z). */
  Color color;  /**< Couleur du bloc pour le rendu. */
  bool isWall;  /**< true si c'est un mur, false si c'est un couloir. */
} Block;

/**
 * @struct type de balles
 * @brief Représente soit PROJ_GRENADE et PROJ_NORMALE
 */

typedef enum {
  PROJ_PISTOLET,
  PROJ_FUSIL,
  PROJ_SNIPER,
  PROJ_GRENADE
} type_projectile;

/**
 * @struct Projectile
 * @brief Représente un projectile (balle) dans le jeu.
 */
typedef struct {
  Vector3 pos;          /**< Position actuelle. */
  Vector3 vel;          /**< Vecteur vélocité (direction * vitesse). */
  float yaw;            /**< Rotation horizontale. */
  float pitch;          /**< Rotation verticale. */
  float radius;         /**< Rayon de la sphère (hitbox). */
  bool active;          /**< true si le projectile est actif. */
  float life;           /**< Temps de vie restant (en secondes). */
  Color color;          /**<couleur du projectile */
  int degats;           /**<degat du projectile */
  type_projectile type; /**< en fonction balles classiques ou grenade */
  OwnerType owner;      /**< Propriétaire du projectile. */
} Projectile;

/**
 * @struct TypeArme
 * @brief Represente PISTOLET,FUSIL ou SNIPER
 */
typedef enum { PISTOLET, FUSIL, SNIPER, GRENADE } TypeArme;

/**
 * @struct ModeleArme
 * @brief Represente la fiche technique de chaque arme
 */

typedef struct {
  TypeArme type;
  int munitionsMax;         // Taille du chargeur (ex: 30)
  float cadenceTir;         // Temps entre deux balles (ex: 0.1s)
  float vitesseProj;        // Vitesse du projectile
  int degats;               // Puissance de l'arme
  const char* nom;          // Pour afficher "AK-47" ou "Glock" à l'écran
  float tailleProjectile;   // taille du projectile
  Color couleurProjectile;  // couleur du projectile
} ModeleArme;

/**
 * @struct Entity
 * @brief Représente une entité du jeu (joueur ou bot).
 */
typedef struct {
  Vector3 pos;            /**< Position de l'entité. */
  float yaw;              /**< Rotation horizontale. */
  float pitch;            /**< Rotation verticale. */
  float velocityY;        /**< Vitesse verticale. */
  bool onGround;          /**< true si l'entité est au sol. */
  float size;             /**< Taille de l'entité. */
  int ammo;               /**< Munitions actuelles. */
  int health;             /**< Points de vie actuels. */
  int maxHealth;          /**< Points de vie maximum. */
  int life;               /**< Nombre de vies restantes. */
  ModeleArme armeEquipee; /**< La fiche technique de l'arme tenue */
  float chronoTir; /**< Le compteur qui descend vers 0 pour autoriser le tir
                      suivant */
  EntityType type; /**< Type de l'entité. */
} Entity;

/**
 * @struct ReseauState
 * @brief Contient l'état de la connexion réseau
 */
typedef struct {
  int socket;    /**< Le socket de communication (-1 si déconnecté) */
  int isServer;  /**< 1 si on est l'Hôte (Serveur), 0 si on est Client */
  int connected; /**< 1 si la partie a commencé, 0 sinon */
} ReseauState;

/**
 * @struct PaquetReseau
 * @brief Données échangées entre les joueurs à chaque frame
 */
typedef struct {
  Vector3 pos;   /**< Position du joueur */
  float yaw;     /**< Angle de vue horizontal */
  float pitch;   /**< Angle de vue vertical */
  int tir;       /**< 1 si le joueur tire, 0 sinon */
  int estMort;   /**< 1 si le joueur est mort */
  TypeArme arme; /**< L'arme que le joueur tient actuellement */
  int life;       /**< Nombre de vies restantes du joueur */
  int isPing;      // 0=normal, 1=ping aller, 2=ping retour
  double timestamp; // Pour mesurer le RTT
} PaquetReseau;

#endif