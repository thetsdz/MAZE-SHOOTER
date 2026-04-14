#ifndef BOSS_H
#define BOSS_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 08/04/2026
 * \brief ajout du boss avec la même logique de déplacement que le bot mais avec
 * une IA de tir plus avancée (vise le joueur en hauteur et en horizontale, et
 * tire plus souvent)
 */
/** \version 2.0
 * \author Corentin Jammes
 * \date 14/04/2026
 * \brief simplification du code du boss
 */

#include "projectile.h"
#include "raylib.h"
#include "raymath.h"
#include "types.h"

/** \brief Initialise le boss
    \param Le boss passé en paramètre
    \param Le labyrinthe pour trouver une position de spawn aléatoire qui n'est
   pas un mur
*/
void InitBoss(Entity* boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** \brief Mise à jour du boss
    \param Labyrinthe
    \param Position du joueur
    \param Tableau de projectiles
*/
void UpdateBoss(Entity* boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Vector3 targetPos, Projectile* projs);
#endif