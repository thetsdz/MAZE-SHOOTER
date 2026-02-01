#ifndef TYPES_H
#define TYPES_H

#include "../linux/raylib-5.5_linux_amd64/include/raylib.h"

#define NUM_BLOCKS 101
#define MAX_PROJ 50
#define SCORE_TRADE 100

#define TRUE 1
#define FALSE 0

typedef enum GameScreen { MENU, TEST } GameScreen;

// Structure représentant un obstacle (mur, sol, plateforme)
// Bloc du labyrinthe
typedef struct {
    Vector3 pos;    // Position centrale du bloc
    float width;    // Largeur (axe X)
    float height;   // Hauteur (axe Y)
    float depth;    // Profondeur (axe Z)
    Color color;    // Couleur pour le rendu
    int isWall;     // 1 = mur, 0 = couloir
} Block;

// Coordonnée pour la pile DFS
typedef struct {
    int i;
    int j;
} Coord;

// Type du propriétaire du projectile
typedef enum {
    OWNER_PLAYER, // Projectile tiré par le joueur
    OWNER_BOT     // Projectile tiré par un bot
} OwnerType;

// Structure pour une balle/projectile
typedef struct {
    Vector3 pos;    // Position actuelle
    Vector3 vel;    // Vecteur vélocité (Direction * Vitesse)
    float radius;   // Rayon de la sphère (hitbox)
    bool active;    // Si false, le projectile est "mort" et peut être réutilisé
    float life;     // Temps de vie restant en secondes
    OwnerType owner; // Type du propriétaire (Joueur ou Bot)
} Projectile;

// Structure regroupant tout l'état du joueur pour éviter de passer 10 variables
typedef struct {
    Vector3 pos;      // Position des pieds/centre du joueur
    float yaw;        // Rotation horizontale (regarder gauche/droite)
    float pitch;      // Rotation verticale (regarder haut/bas)
    float velocityY;  // Vitesse verticale (pour la gravité/saut)
    bool onGround;    // Est-ce que le joueur touche le sol ?
    float size;       // Taille du cube joueur
    int ammo;         // Munitions actuelles dans le chargeur
    int maxAmmo;      // Capacité actuelle du chargeur (commence à 10, max 50)
} Player;

// Structure regroupant tout l'état du bot IA pour éviter de passer 10 variables
typedef struct {
    Vector3 pos;      // Position des pieds/centre du bot
    float yaw;        // Rotation horizontale (regarder gauche/droite)
    float pitch;      // Rotation verticale (regarder haut/bas)
    float velocityY;  // Vitesse verticale (pour la gravité/saut)
    bool onGround;    // Est-ce que le bot touche le sol ?
    float size;       // Taille du cube bot
    int ammo;         // Munitions actuelles dans le chargeur
    int maxAmmo;      // Capacité actuelle du chargeur
} Bot;

#endif