#ifndef PILE_H
#define PILE_H

#include "types.h"

void initpile(void);
int pilevide(void);
void empiler(int i, int j);
Coord depiler(void);  // Retourne une structure Coord

#endif
