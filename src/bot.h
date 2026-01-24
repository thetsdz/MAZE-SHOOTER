#ifndef BOT_H
#define BOT_H

#include "types.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"

// Réinitialise les variables du bot (position spawn, vitesse nulle)
void InitBot(Bot *bot);

// gère IA, physique et collisions
// Modifie l'état du bot et met à jour sa 'camera'
void UpdateBot(Bot *bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Camera3D *camera);

#endif