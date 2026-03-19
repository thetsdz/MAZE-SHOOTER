#ifndef LEVEL_H
#define LEVEL_H

/** \version 1.0
 *  \author Thomas Dequirez
 *  \date 22/01/2026
 * \brief Contient les fonctions de génération et d'affichage du labyrinthe
 */

#include "types.h"

/** @brief Initialisation du labyrinthe (tout murs)*/
void init_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** @brief Génération du labyrinthe*/
void creer_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** @brief Affichage du labyrinthe*/
void DrawLevel(Block blocks[NUM_BLOCKS][NUM_BLOCKS], Model wallModel, Model floorModel);
#endif