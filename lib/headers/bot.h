#ifndef BOT_H
#define BOT_H

#include "types.h"
#include "projectile.h"
#include "../linux/raylib-5.5_linux_amd64/include/raymath.h"




/** \brief Initialise le robot
    \param Le robot passé en paramètre
*/
void InitBot(Entity *bot);



/** \brief Mise à jour du robot
    \param Labyrinthe
    \param Position du joueur
    \param Tableau de projectiles
*/
void UpdateBot(Entity *bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Vector3 targetPos, Projectile *projs);
#endif