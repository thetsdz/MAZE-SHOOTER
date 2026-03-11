#ifndef TYPES_H
#define TYPES_H

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


typedef enum GameScreen { MENU, NOUVELLE_PARTIE, MULTIJOUEUR, CHARGER_PARTIE,OPTIONS, EXIT } GameScreen;  // Ajoute EXIT



/**
 * @enum EntityType
 * @brief Définit le type d'une entité (joueur ou bot).
 */
typedef enum {
    ENTITY_PLAYER,  /**< Entité contrôlée par le joueur. */
    ENTITY_BOT,      /**< Entité contrôlée par l'IA. */
    ENTITY_REMOTE_PLAYER /** < L'autre joueur humain (reseau). */
} EntityType;

/**
 * @enum OwnerType
 * @brief Définit le propriétaire d'un projectile.
 */
typedef enum {
    OWNER_PLAYER,  /**< Projectile tiré par le joueur. */
    OWNER_BOT,      /**< Projectile tiré par un bot. */
    OWNER_REMOTE_PLAYER /**< Projectile tiré par l'autre joueur humain (reseau). */
} OwnerType;

/**
 * @struct Coord
 * @brief Coordonnées pour la pile DFS (parcours du labyrinthe).
 */
typedef struct {
    int i;  /**< Indice de ligne. */
    int j;  /**< Indice de colonne. */
} Coord;

/**
 * @struct Block
 * @brief Représente un bloc du labyrinthe (mur ou couloir).
 */
typedef struct {
    Vector3 pos;    /**< Position centrale du bloc. */
    float width;    /**< Largeur du bloc (axe X). */
    float height;   /**< Hauteur du bloc (axe Y). */
    float depth;    /**< Profondeur du bloc (axe Z). */
    Color color;    /**< Couleur du bloc pour le rendu. */
    bool isWall;    /**< true si c'est un mur, false si c'est un couloir. */
} Block;

/**
 * @struct Projectile
 * @brief Représente un projectile (balle) dans le jeu.
 */
typedef struct {
    Vector3 pos;    /**< Position actuelle. */
    Vector3 vel;    /**< Vecteur vélocité (direction * vitesse). */
    float radius;   /**< Rayon de la sphère (hitbox). */
    bool active;    /**< true si le projectile est actif. */
    float life;     /**< Temps de vie restant (en secondes). */
    Color color;     /**<couleur du projectile */
    OwnerType owner; /**< Propriétaire du projectile. */
} Projectile;





/**
* @struct TypeArme
* @brief Represente PISTOLET,FUSIL ou SNIPER
*/
typedef enum { 
    PISTOLET, 
    FUSIL, 
    SNIPER,
    GRENADE
} TypeArme;

/**
* @struct ModeleArme
* @brief Represente la fiche technique de chaque arme
*/

typedef struct {
    TypeArme type;
    int munitionsMax;      //Taille du chargeur (ex: 30)
    float cadenceTir;      // Temps entre deux balles (ex: 0.1s)
    float vitesseBalle;    // Vitesse du projectile
    float degats;          // Puissance de l'arme
    const char *nom;       // Pour afficher "AK-47" ou "Glock" à l'écran
    float tailleProjectile; //taille du projectile
    Color couleurProjectile; // couleur du projectile
} ModeleArme;



/**
 * @struct Entity
 * @brief Représente une entité du jeu (joueur ou bot).
 */
typedef struct {
    Vector3 pos;      /**< Position de l'entité. */
    float yaw;        /**< Rotation horizontale. */
    float pitch;      /**< Rotation verticale. */
    float velocityY;  /**< Vitesse verticale. */
    bool onGround;    /**< true si l'entité est au sol. */
    float size;       /**< Taille de l'entité. */
    int ammo;         /**< Munitions actuelles. */
<<<<<<< HEAD
    ModeleArme armeEquipee; /**< La fiche technique de l'arme tenue */
    float chronoTir;   /**< Le compteur qui descend vers 0 pour autoriser le tir suivant */
=======
    int maxAmmo;      /**< Capacité maximale du chargeur. */
    int health;       /**< Points de vie actuels. */
    int maxHealth;    /**< Points de vie maximum. */
    int life;         /**< Nombre de vies restantes. */
>>>>>>> master
    EntityType type;  /**< Type de l'entité. */
} Entity;

/**
 * @struct ReseauState
 * @brief Contient l'état de la connexion réseau
 */
typedef struct {
    int socket;      /**< Le socket de communication (-1 si déconnecté) */
    int isServer;    /**< 1 si on est l'Hôte (Serveur), 0 si on est Client */
    int connected;   /**< 1 si la partie a commencé, 0 sinon */
} ReseauState;

/**
 * @struct PaquetReseau
 * @brief Données échangées entre les joueurs à chaque frame
 */
typedef struct {
    Vector3 pos;     /**< Position du joueur */
    float yaw;       /**< Angle de vue horizontal */
    float pitch;     /**< Angle de vue vertical (à ajouter pour gérer le tir en hauteur) */
    int tir;         /**< 1 si le joueur tire, 0 sinon */
    int estMort;     /**< 1 si le joueur est mort */
} PaquetReseau;

#endif