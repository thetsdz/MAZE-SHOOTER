#ifndef RECHERCHE_H
#define RECHERCHE_H

#include "types.h"

// --- FONCTION LIGNE DE VUE ---
// Retourne vrai si aucun mur ne sépare le bot du joueur
bool IsPlayerVisible(Vector3 start, Vector3 end, Block blocks[NUM_BLOCKS][
                        NUM_BLOCKS]);

// --- FONCTION PATHFINDING (BFS) ---
// Trouve la case suivante pour avancer vers la cible dans le labyrinthe
Coord GetNextStepBFS(Coord start, Coord target,
                          Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

#endif  // RECHERCHE_H