#ifndef PILE_H
#define PILE_H

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