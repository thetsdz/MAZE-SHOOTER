#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define NUM_BLOCKS 10
#define MAX_PROJ 50
#define SCORE_TRADE 100

// Structure représentant un obstacle (mur, sol, plateforme)
typedef struct {
    Vector3 pos;    // Position centrale du bloc
    float width;    // Largeur (axe X)
    float height;   // Hauteur (axe Y)
    float depth;    // Profondeur (axe Z)
    Color color;    // Couleur pour le rendu
} Block;

// Structure pour une balle/projectile
typedef struct {
    Vector3 pos;    // Position actuelle
    Vector3 vel;    // Vecteur vélocité (Direction * Vitesse)
    float radius;   // Rayon de la sphère (hitbox)
    bool active;    // Si false, le projectile est "mort" et peut être réutilisé
    float life;     // Temps de vie restant en secondes
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

#endif