#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "raymath.h"

// Réinitialise les variables du joueur (position spawn, vitesse nulle)
void InitPlayer(Player *player);

// C'est le cerveau du joueur : gère clavier, souris, physique et collisions
// Modifie l'état du 'player' et met à jour la 'camera'
void UpdatePlayer(Player *player, Block *blocks, Camera3D *camera);

#endif