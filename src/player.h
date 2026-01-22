#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"

// Réinitialise les variables du joueur (position spawn, vitesse nulle)
void InitPlayer(Player *player);

// C'est le cerveau du joueur : gère clavier, souris, physique et collisions
// Modifie l'état du 'player' et met à jour la 'camera'
void UpdatePlayer(Player *player, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Camera3D *camera);

#endif