#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"

// Initialisation du labyrinthe (tout murs)
void init_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

// Génération du labyrinthe
void creer_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

// Affichage du labyrinthe
void DrawLevel(Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

#endif
