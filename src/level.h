#ifndef LEVEL_H
#define LEVEL_H

#define N 100
#define M 100

#include "types.h"
#include "pile.h"

// Remplit le tableau de blocs avec les murs et le décor
void InitLevel(Block *blocks);

// Affiche la grille au sol et tous les blocs actifs
void DrawLevel(Block *blocks);




void init_lab(int lan[N][M]);
void afficher_lab(int lab[N][M]);
void nettoyer(int lab[N][M]);
int chercher_chemin(int lab[N][M],int xd, int yd, int xa, int ya);
void marquer_chemin(int lab[N][M],int x, int y,int* lgNin);
void init_lab(int lab[N][M]);
int valides(int i, int j);
int est_vide(int i, int j, int lab[N][M]);
int est_mur(int i, int j, int lab[N][M]);
int blocage(int i, int j, int lab[N][M]);
void creer_lab(int lab[N][M]);


#endif