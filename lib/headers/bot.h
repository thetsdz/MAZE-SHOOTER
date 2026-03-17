#ifndef BOT_H
#define BOT_H

#include "types.h"
#include "projectile.h"
#include "raymath.h"

/** \brief Initialise le robot
    \param Le robot passé en paramètre
    \param Le labyrinthe pour trouver une position de spawn aléatoire qui n'est
   pas un mur
*/
void InitBot(Entity* bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** \brief Mise à jour du robot
    \param Labyrinthe
    \param Position du joueur
    \param Tableau de projectiles
*/
void UpdateBot(Entity* bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
               Vector3 targetPos, Projectile* projs);
#endif