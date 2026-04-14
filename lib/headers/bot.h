#ifndef BOT_H
#define BOT_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 24/01/2026
 * \brief ajout d'un bot avec une IA basique qui se déplace aléatoirement
 */
/** \version 2.0
 * \author Corentin Jammes
 * \date 01/02/2026
 * \brief ajout de la logique de tir pour le bot (vise le joueur et tire aléatoirement)
 */
/** \version 3.0
 * \author Corentin Jammes
 * \date 08/03/2026
 * \brief ajout de la logique de déplacement pour le bot via un pathfinding basique (BFS) pour éviter les murs
 */
/** \version 4.0
 * \author Corentin Jammes
 * \date 14/04/2026
 * \brief simplification du code du bot
 */

#include "projectile.h"
#include "raylib.h"
#include "raymath.h"
#include "types.h"

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