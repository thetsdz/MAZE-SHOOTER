#ifndef ENDGAME_H
#define ENDGAME_H

#include "raylib.h"
#include "types.h"

/** \version 1.0
 * \author Corentin Jammes
 * \date 23/03/2026
 * \brief Gére la creation/dessin de la fin de partie, avec les touches pour
 * retourner au menu ou quitter le jeu
 */

/**
 *   \brief Gére la creation/dessin des chaques boutons et zone d'informations
 * lors de la defaite du joueur
 *   \param <GameScreen* currentScreen> Etat de l'ecran actuel
 *   \param <int score> Score final du joueur
 */
void GererGameOver(GameScreen *currentScreen, int score);

/**
 *   \brief Gére la creation/dessin des chaques boutons et zone d'informations
 * lors de la victoire du joueur
 *   \param <GameScreen* currentScreen> Etat de l'ecran actuel
 *   \param <int score> Score final du joueur
 */
void GererVictoire(GameScreen *currentScreen, int score);
#endif
