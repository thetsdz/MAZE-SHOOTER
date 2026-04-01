#ifndef UPDATEGAME_H
#define UPDATEGAME_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 11/02/2026
 * \brief Fonction centrale qui gère la logique du jeu à chaque frame :
 * mouvements, tirs, collisions, etc uniquement dans le mode solo.
 */
/** \version 1.1
 * \author Corentin Jammes
 * \date 02/03/2026
 * \brief Adaptation de l'appelle des fonctions qui ont été mise à jour pour le
 * multijoueur.
 */

 /** \version 1.2
 * \author Hugues Astier
 * \date 01/04/2026
 * \brief Changement du fonctionnement du score
 */
#include "types.h"

/**
 * @brief permet de faire la logique du jeu à chaque frame (mouvements, tirs,
 * collisions, etc.)
 * @param player Pointeur vers l'entité du joueur (pour mettre à jour sa
 * position, munitions, etc.)
 * @param bot Pointeur vers l'entité du bot (pour mettre à jour sa position, IA,
 * etc.)
 * @param blocks Grille de blocs du niveau (pour gérer les collisions avec les
 * murs)
 * @param projs Tableau de projectiles actifs (pour mettre à jour leur position,
 * gérer les collisions, etc.)
 
 * @param camera Pointeur vers la caméra 3D (pour l'affichage joueur et les
 * calculs de direction de tir)
 */

void ChangementArme(Entity *joueur);

void UpdateGame(Entity *player, Entity bot[18],Heal heal[10],
                Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Projectile projs[MAX_PROJ], Camera3D* camera, GameScreen* currentScreen);

#endif