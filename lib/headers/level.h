#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"

/** @brief Initialisation du labyrinthe (tout murs)*/
void init_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** @brief Génération du labyrinthe*/
void creer_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/** @brief Affichage du labyrinthe*/
void DrawLevel(Block blocks[NUM_BLOCKS][NUM_BLOCKS], Texture2D wallTex, Texture2D floorTex);

#endif