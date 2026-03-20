#ifndef PROJECTILE_H
#define PROJECTILE_H

/** \version 1.0
 *  \author Corentin Jammes
 *  \date 11/01/2026
 *  \brief systeme de tir sans munitions sans rechargement mais un systeme de
 * collision avec une cible et les murs du labyrinthe
 */

/** \version 2.0
 *  \author Corentin Jammes
 *  \date 15/01/2026
 *  \brief systeme de tir avec munitions et rechargement ainsi qu'un systeme de
*   collision avec une cible et les murs du labyrinthe
*/

/** \version 3.0
 *  \author Corentin Jammes
 *  \date 01/02/2026
 *  \brief Systèmes de projectiles fonctionnels avec
collisions(mur,bot,joueurs(meme en multijoueur)) collisions(mur,bot,joueurs(meme
en multijoueur)) systeme de tir avec munitions et rechargement ainsi qu'un
systeme de collision avec une cible et les murs du labyrinthe et ajout du tire
pour le bot
 */

/** \version 4.0
 *  \author Corentin Jammes
 *  \date 02/03/2026
 *  \brief Systèmes de projectiles fonctionnels avec
 * collisions(mur,bot,joueurs(meme en multijoueur))
 */

 /** \version 5.0
 *  \author Hugues Astier
 *  \date 18/03/2026
 *  \brief Changeement de logique de detection des projectiles en colision sur le bot, ajout des grenades, et des differents type de projectile 
 *  en focntion de l'arme.
 *  Ajout de la fonction explosion, gravite et RebondirGrenade pour gerer la grenade, plus ajout des textures sur les projectiles 
 * Systeme de projectiles fonctiennels 
 */



#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "types.h"

/** @brief Initialise le tableau de projectiles (tous inactifs au début)*/
void InitProjectiles(Projectile* projs);

/** @brief Trouve un slot libre et tire un projectile dans la direction où regarde le joueur ou le bot*/
void ShootProjectile(Projectile *projs, Vector3 startPos, Vector3 direction, OwnerType owner,ModeleArme arme);

/** @brief Met à jour la position, la durée de vie et les collisions de tous les
 * tirs actifs*/
void UpdateProjectiles(Projectile* projs, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Entity* bot, Entity* player, int* score);

/** @brief Affiche les projectiles actifs*/
void DrawProjectiles(Projectile* projs, Model tabModels[4]);

#endif