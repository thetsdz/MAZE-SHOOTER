#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../lib/headers/recherche.h"
#include "../lib/headers/types.h"


// --- FONCTION LIGNE DE VUE ---
// Retourne vrai si aucun mur ne sépare le bot du joueur
bool IsPlayerVisible(Vector3 start, Vector3 end,
                            Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  float dx = end.x - start.x;
  float dz = end.z - start.z;
  float distSq = dx * dx + dz * dz;
  float dist = sqrtf(distSq);

  int steps = (int)(dist * 3.0f);  // 3 échantillons par unité de distance
  if (steps == 0) return true;

  float stepX = dx / steps;
  float stepZ = dz / steps;
  float offset = NUM_BLOCKS - 1;  // Correspond à la création dans level.c (50)

  for (int k = 0; k <= steps; k++) {
    float x = start.x + stepX * k;
    float z = start.z + stepZ * k;

    // Convertir la position Monde en position Grille
    int i = (int)roundf((x + offset) / 3.0f);
    int j = (int)roundf((z + offset) / 3.0f);

    if (i >= 0 && i < NUM_BLOCKS && j >= 0 && j < NUM_BLOCKS) {
      if (blocks[i][j].isWall) return false;  // Bloqué par un mur
    }
  }
  return true;
}

// --- FONCTION PATHFINDING (BFS) ---
// Trouve la case suivante pour avancer vers la cible dans le labyrinthe
Coord GetNextStepBFS(Coord start, Coord target,
                              Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  if (start.i == target.i && start.j == target.j) return target;

  bool visited[NUM_BLOCKS][NUM_BLOCKS] = {false};
  Coord parent[NUM_BLOCKS][NUM_BLOCKS];

  Coord queue[NUM_BLOCKS * NUM_BLOCKS];
  int head = 0, tail = 0;

  queue[tail++] = start;
  visited[start.i][start.j] = true;
  parent[start.i][start.j] = (Coord){-1, -1};

  int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  bool found = false;

  while (head < tail) {
    Coord curr = queue[head++];

    if (curr.i == target.i && curr.j == target.j) {
      found = true;
      break;
    }

    for (int d = 0; d < 4; d++) {
      int ni = curr.i + dirs[d][0];
      int nj = curr.j + dirs[d][1];

      // Si la case est dans la grille, non visitée, et n'est pas un mur
      if (ni >= 0 && ni < NUM_BLOCKS && nj >= 0 && nj < NUM_BLOCKS) {
        if (!visited[ni][nj] && !blocks[ni][nj].isWall) {
          visited[ni][nj] = true;
          parent[ni][nj] = curr;
          queue[tail++] = (Coord){ni, nj};
        }
      }
    }
  }

  if (!found)
    return start;  // Si le joueur est inatteignable (bug/mur fermé), on
                   // reste sur place

  // Remonter le chemin depuis la cible jusqu'à la première case après le
  // départ
  Coord curr = target;
  while (parent[curr.i][curr.j].i != start.i ||
         parent[curr.i][curr.j].j != start.j) {
    curr = parent[curr.i][curr.j];
  }

  return curr;
}