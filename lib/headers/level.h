#ifndef LEVEL_H
#define LEVEL_H

/** \version 1.0
 *  \author Thomas Dequirez
 *  \date 22/01/2026
 * \brief Contient les fonctions de génération et d'affichage du labyrinthe
 */

#include "types.h"

/** @brief Initialisation du labyrinthe (tout murs)
 * @param blocks Le labyrinthe représenté par une matrice de blocs à initialiser
*/
void init_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** @brief Génération du labyrinthe
 * @param blocks Le labyrinthe représenté par une matrice de blocs à générer
*/
void creer_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** @brief Affichage du labyrinthe
 * @param blocks Le labyrinthe représenté par une matrice de blocs à afficher
 * @param wallModel Le modèle 3D utilisé pour les murs
 * @param floorModel Le modèle 3D utilisé pour le sol
*/
void DrawLevel(Block blocks[NUM_BLOCKS][NUM_BLOCKS], Model wallModel,
               Model floorModel);

/** @brief Génération du labyrinthe
 * @param blocks Le labyrinthe représenté par une matrice de blocs à générer
*/
void creer_lab_multi(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

#endif