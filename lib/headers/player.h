#ifndef PLAYER_H
#define PLAYER_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 11/01/2026
 * \brief Système de contrôle du joueur : déplacement, saut, gestion de la
 * caméra et collisions avec les blocs (arret tootale des mouvements).
 */
/** \version 1.1
 * \author Corentin Jammes
 * \date 15/01/2026
 * \brief Ajout d'une limite de munitions et du systeme de rechargement.
 */
/** \version 2.0
 * \author Corentin Jammes
 * \date 24/01/2026
 * \brief Amélioration du système de contrôle du joueur : ajout de collisions
 * plus réalistes (glissement sur les murs au lieu d'un arrêt total).
 */
/** \version 2.1
 * \author Thomas Dequirez
 * \date 27/01/2026
 * \brief ajout du deplacement a l'aide des fleches du clavier en + de zqsd.
 */
/** \version 2.2
 * \author Thomas Dequirez
 * \date 30/01/2026
 * \brief changement des definitions de constantes locales en definitions
 * globales sur l'ensembles des fichiers.
 */
/** \version 3.0
 * \author Corentin Jammes
 * \date 05/02/2026
 * \brief Changement du type Player en type Entity pour uniformiser les données
 * et faciliter les interactions avec les autres entités (ennemis, projectiles).
 */
/** \version 4.0
 * \author Corentin Jammes
 * \date 02/03/2026
 * \brief adaptattion de player pour le multijoueur et ajout de la collision
 * avec les ennemis (glissement + saut sur la tête).
 */
/** \version 5.0
 * \author Hugues Astier
 * \date 17/03/2026
 * \brief ajout de l'initialisation et la gestion des nouvelles armes.
 */

#include "types.h"
#include "raymath.h"

/** @brief Réinitialise les variables du joueur (position spawn, vitesse
 * nulle)
 *  @param player Le joueur à initialiser
 */
void InitPlayer(Entity *player);

/** @brief C'est le cerveau du joueur : gère clavier, souris, physique et
 * collisions Modifie l'état du 'player' et met à jour la 'camera'
 * @param player Le joueur à mettre à jour
 * @param blocks Le labyrinthe représenté par une matrice de blocs (pour les
 * collisions)
 * @param camera La caméra 3D pour le rendu (position et target seront mis à
 * jour pour suivre le joueur)
 * @param ennemi Pointeur vers l'ennemi (pour gérer les collisions avec
 *  les bots et les interactions de combat)
 */
void UpdatePlayer(Entity *player, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                  Camera3D *camera, Entity **ennemi);

#endif