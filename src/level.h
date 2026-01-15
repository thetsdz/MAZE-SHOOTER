#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"

// Remplit le tableau de blocs avec les murs et le décor
void InitLevel(Block *blocks);

// Affiche la grille au sol et tous les blocs actifs
void DrawLevel(Block *blocks);

#endif