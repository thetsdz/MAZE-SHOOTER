#ifndef BOT_H
#define BOT_H

#include "types.h"
#include "projectile.h"
#include "../linux/raylib-5.5_linux_amd64/include/raymath.h"

void InitBot(Bot *bot); // Réinitialise les variables du bot (position spawn, vitesse nulle)
void UpdateBot(Bot *bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Vector3 targetPos, Projectile *projs);// Modifie l'état du bot et met à jour sa 'camera'  + gère IA, physique et collisions
 
#endif