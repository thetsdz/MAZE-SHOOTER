#ifndef TYPES_H
#define TYPES_H

#include "../linux/raylib-5.5_linux_amd64/include/raylib.h"

/** @brief Nombre total de blocs dans le labyrinthe. */
#define NUM_BLOCKS 101
/** @brief Nombre maximal de projectiles actifs simultanément. */
#define MAX_PROJ 50
/** @brief Score obtenu par échange ou action spécifique. */
#define SCORE_TRADE 100

/** @brief Valeurs booléennes personnalisées. */
#define TRUE 1
#define FALSE 0

/**
 * @enum GameScreen
 * @brief Définit les écrans disponibles dans le jeu.
 */
typedef enum GameScreen { MENU, TEST } GameScreen;

/**
 * @enum EntityType
 * @brief Définit le type d'une entité (joueur ou bot).
 */
typedef enum {
    ENTITY_PLAYER,  /**< Entité contrôlée par le joueur. */
    ENTITY_BOT      /**< Entité contrôlée par l'IA. */
} EntityType;

/**
 * @enum OwnerType
 * @brief Définit le propriétaire d'un projectile.
 */
typedef enum {
    OWNER_PLAYER,  /**< Projectile tiré par le joueur. */
    OWNER_BOT      /**< Projectile tiré par un bot. */
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
    OwnerType owner;/**< Propriétaire du projectile. */
} Projectile;

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
    int maxAmmo;      /**< Capacité maximale du chargeur. */
    EntityType type;  /**< Type de l'entité. */
} Entity;

#endif