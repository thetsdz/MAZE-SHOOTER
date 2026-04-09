#ifndef BOSS_H
#define BOSS_H

#include "projectile.h"
#include "raylib.h"
#include "types.h"
#include "raymath.h"

/** \brief Initialise le boss
    \param Le boss passé en paramètre
    \param Le labyrinthe pour trouver une position de spawn aléatoire qui n'est
   pas un mur
*/
void InitBoss(Entity *boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** \brief Mise à jour du boss
    \param Labyrinthe
    \param Position du joueur
    \param Tableau de projectiles
*/
void UpdateBoss(Entity *boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
               Vector3 targetPos, Projectile *projs);
#endif