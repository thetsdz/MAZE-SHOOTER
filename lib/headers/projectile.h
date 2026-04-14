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
 *  \brief Changement de logique de detection des projectiles en colision sur le
 * bot, ajout des grenades, et des differents type de projectile en focntion de
 * l'arme. Ajout de la fonction explosion, gravite et RebondirGrenade pour gerer
 * la grenade, plus ajout des textures sur les projectiles Systeme de
 * projectiles fonctiennels
 */

 /** \version 5.1
 *  \author Hugues Astier
 *  \date 18/03/2026
 *  \brief Adaptation du cgangement de score
 */


#include <stdio.h>

#include "raylib.h"
#include "raymath.h"
#include "types.h"

/** @brief Initialise le tableau de projectiles (tous inactifs au début)
 * @param projs Le tableau de projectiles à initialiser
 */
void InitProjectiles(Projectile *projs);

/** @brief Trouve un slot libre et tire un projectile dans la direction où
 * regarde le joueur ou le bot
 * @param projs Le tableau de projectiles actifs dans le jeu
 * @param startPos La position de départ du projectile (généralement la position
 * du joueur ou du bot)
 * @param direction La direction dans laquelle tirer le projectile (généralement
 * la direction de la caméra)
 * @param owner Le propriétaire du projectile (joueur, bot, etc.)
 * @param arme La fiche technique de l'arme utilisée pour tirer (vitesse, dégâts
 *  taille du projectile, etc.)
 * @param camYaw L'angle de rotation horizontale de la caméra (pour orienter
 *  le projectile)
 * @param camPitch L'angle de rotation verticale de la caméra (pour orienter le
 * projectile)
 */
void ShootProjectile(Projectile *projs, Vector3 startPos, Vector3 direction,
                     OwnerType owner, ModeleArme arme, float camYaw,
                     float camPitch);

/** @brief Met à jour la position, la durée de vie et les collisions de tous les
 * tirs actifs
 * @param projs Le tableau de projectiles à mettre à jour
 * @param blocks Le labyrinthe représenté par une matrice de blocs (pour les
 * collisions)
 * @param autre Pointeur vers l'autre joueur (pour gérer les collisions en
 * multijoueur)
 * @param player Pointeur vers le joueur principal (pour gérer les collisions en
 * solo)
 * @param currentScreen Pointeur vers l'écran de jeu actuel (pour gérer les
 * transitions en cas de mort du joueur)
 * @param IsBossAlive Pointeur vers un booléen indiquant si le boss est vivant
 * (pour gérer les collisions avec le boss)
 * @param boss Pointeur vers le boss (pour gérer les collisions avec le boss)
 */
void UpdateProjectiles(Projectile* projs, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Entity** autre, Entity* player, GameScreen* currentScreen, bool* IsBossAlive, Entity* boss);

#endif