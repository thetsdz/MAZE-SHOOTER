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



/** \brief Met à jour l'état du jeu à chaque frame.
 * Cette fonction gère la logique du jeu : mouvements, tirs, collisions, etc.
 * Elle est appelée à chaque frame dans la boucle principale du jeu.
 * \param player Le joueur principal
 * \param bot Tableau des bots ennemis
 * \param heal Tableau des objets de soin et d'armes
 * \param blocks Le labyrinthe représenté par une matrice de blocs
 * \param projs Tableau des projectiles actifs dans le jeu
 * \param camera La caméra 3D pour le rendu
 * \param currentScreen L'écran de jeu actuel (pour gérer les transitions)
 * \param boss Le boss actuel (si présent)
 * \param IsBossAlive Indique si le boss est vivant ou non
 */
void UpdateGame(Entity* player, Entity bot[18], Coffre coffre[10],
                Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Projectile projs[MAX_PROJ], Camera3D* camera,
                GameScreen* currentScreen, Entity* boss, bool* IsBossAlive);

#endif