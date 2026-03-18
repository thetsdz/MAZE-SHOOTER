#ifndef PILE_H
#define PILE_H
/** \version 1.0
 * \author Dequirez Thomas
 * \date 21/01/2026
 * \brief Système de pile pour le backtracking dans la génération du labyrinthe.
 */
#include "types.h"

/** @brief Initialisation de la île (tout murs)*/
void initpile(void);

/** @brief Verifie si la pile est vide ou non*/
int pilevide(void);

/** @brief Empile les 2 chiffres passés en paramètres*/
void empiler(int i, int j);

/** @brief Depile en renvoyant une coordonnées*/
Coord depiler(void);

#endif