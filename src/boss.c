/**
 * \file boss.c
 */

#include "../lib/headers/boss.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../lib/headers/arme.h"

// Structure pour représenter une position dans la grille du labyrinthe
typedef struct {
  int i;
  int j;
} GridPos;

// --- FONCTION LIGNE DE VUE ---
// Retourne vrai si aucun mur ne sépare le boss du joueur
static bool IsPlayerVisible(Vector3 start, Vector3 end,
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
static GridPos GetNextStepBFS(GridPos start, GridPos target,
                              Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  if (start.i == target.i && start.j == target.j) return target;

  bool visited[NUM_BLOCKS][NUM_BLOCKS] = {false};
  GridPos parent[NUM_BLOCKS][NUM_BLOCKS];

  GridPos queue[NUM_BLOCKS * NUM_BLOCKS];
  int head = 0, tail = 0;

  queue[tail++] = start;
  visited[start.i][start.j] = true;
  parent[start.i][start.j] = (GridPos){-1, -1};

  int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
  bool found = false;

  while (head < tail) {
    GridPos curr = queue[head++];

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
          queue[tail++] = (GridPos){ni, nj};
        }
      }
    }
  }

  if (!found)
    return start;  // Si le joueur est inatteignable (bug/mur fermé), on
                   // reste sur place

  // Remonter le chemin depuis la cible jusqu'à la première case après le
  // départ
  GridPos curr = target;
  while (parent[curr.i][curr.j].i != start.i ||
         parent[curr.i][curr.j].j != start.j) {
    curr = parent[curr.i][curr.j];
  }

  return curr;
}

void InitBoss(Entity* boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  boss->yaw = 0.0f;        // angle du boss
  boss->pitch = 0.0f;      // angle du boss
  boss->velocityY = 0.0f;  // vitesse du boss
  boss->onGround = true;   // le boss est initialisée au sol
  boss->size = 2.5f;       // taille du boss
  boss->health = 1000;     // Points de vie du boss
  boss->maxHealth = 1000;  // Points de vie maximum du boss
  boss->life = 200;        // Le boss a une vie très longue
  boss->armeEquipee = ObtenirModeleArme(SNIPER);  // le type d'arme du boss
  boss->type = ENTITY_BOSS;                       // type de l'entité
  boss->chronoTir = (float)(rand() % 100) / 100.0f;
  // --- RECHERCHE D'UN SPAWN ALÉATOIRE ---
  float offset = NUM_BLOCKS - 1;  // Le même offset que dans level.c
  int i, j;

  // On boucle jusqu'à trouver une case qui N'EST PAS un mur
  do {
    // rand() % NUM_BLOCKS donne un nombre entre 0 et 50
    i = rand() % NUM_BLOCKS;
    j = rand() % NUM_BLOCKS;
  } while (blocks[i][j].isWall);

  boss->pos.x = i * 3.0f - offset;
  boss->pos.z = j * 3.0f - offset;
  boss->pos.y = 5.0f;  // On le fait spawner un peu en l'air pour qu'il retombe
                       // doucement au sol */
}

void UpdateBoss(Entity* boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Vector3 targetPos, Projectile* projs) {
  /** \brief Paramètres du Boss
      \code
      float speed = 0.08f;
      float gravity = 0.02f;
      float bossHalf = boss->size/2;
      float dt = GetFrameTime();
      \endcode
  */
  float speed = 0.08f;
  float gravity = 0.02f;
  float bossHalf = boss->size / 2;
  float dt = GetFrameTime();
  float offset = NUM_BLOCKS - 1;

  // 1. --- ANALYSE DE LA SITUATION ---
  bool playerVisible = IsPlayerVisible(boss->pos, targetPos, blocks);
  Vector3 moveTarget = targetPos;  // Par défaut, on va vers le joueur

  // 2. --- LOGIQUE DE NAVIGATION (PATHFINDING) ---
  if (!playerVisible) {
    GridPos bossGrid = {(int)roundf((boss->pos.x + offset) / 3.0f),
                        (int)roundf((boss->pos.z + offset) / 3.0f)};
    GridPos playerGrid = {(int)roundf((targetPos.x + offset) / 3.0f),
                          (int)roundf((targetPos.z + offset) / 3.0f)};

    // Sécurité pour ne pas sortir du tableau
    if (bossGrid.i < 0)
      bossGrid.i = 0;
    else if (bossGrid.i >= NUM_BLOCKS)
      bossGrid.i = NUM_BLOCKS - 1;
    if (bossGrid.j < 0)
      bossGrid.j = 0;
    else if (bossGrid.j >= NUM_BLOCKS)
      bossGrid.j = NUM_BLOCKS - 1;
    if (playerGrid.i < 0)
      playerGrid.i = 0;
    else if (playerGrid.i >= NUM_BLOCKS)
      playerGrid.i = NUM_BLOCKS - 1;
    if (playerGrid.j < 0)
      playerGrid.j = 0;
    else if (playerGrid.j >= NUM_BLOCKS)
      playerGrid.j = NUM_BLOCKS - 1;

    GridPos nextStep = GetNextStepBFS(bossGrid, playerGrid, blocks);

    // Conversion de la prochaine case Grid en position Monde
    moveTarget.x = nextStep.i * 3.0f - offset;
    moveTarget.z = nextStep.j * 3.0f - offset;
    moveTarget.y = boss->pos.y;  // Rester à la même hauteur
  }

  /** \brief Calcul de l'angle pour regarder le joueur sur le plan horizontal
     (XZ)
      \code
      float dx = targetPos.x - boss->pos.x;
      float dz = targetPos.z - boss->pos.z;
      boss->yaw = atan2f(dx, dz);
      \endcode
  */
  // 3. --- ORIENTATION ET VISÉE ---
  // Le roboss regarde vers là où il se déplace
  float dxMove = moveTarget.x - boss->pos.x;
  float dzMove = moveTarget.z - boss->pos.z;
  boss->yaw = atan2f(dxMove, dzMove);

  /** \brief Viser en hauteur
      \code
      float dist = sqrtf(dx*dx + dz*dz);
      float dy = (targetPos.y + 0.5f) - (boss->pos.y + 0.5f);
      boss->pitch = atan2f(dy, dist);
      \endcode
  */
  // Visée vers le joueur en hauteur (Pitch)
  float dxPlayer = targetPos.x - boss->pos.x;
  float dzPlayer = targetPos.z - boss->pos.z;
  float distToPlayer = sqrtf(dxPlayer * dxPlayer + dzPlayer * dzPlayer);
  float dy = (targetPos.y + 0.5f) - (boss->pos.y + 0.5f);
  boss->pitch = atan2f(dy, distToPlayer);

  // --- Tir ---
  // CORRECTION : On utilise le chronoTir propre à l'entité
  boss->chronoTir += dt;

  // Le boss tire toutes les 1.5 à 2.5 secondes (aléatoire un peu)
  // Le boss NE TIRE QUE S'IL VOIT LE JOUEUR
  if (playerVisible && boss->chronoTir > 2.0f &&
      distToPlayer < 30.0f) {  // Ne tire que si < 30 mètres

    // Calcul du vecteur de visée parfait
    Vector3 aimDir = Vector3Subtract(targetPos, boss->pos);
    aimDir = Vector3Normalize(aimDir);

    // Position de départ (au niveau des yeux du boss)
    Vector3 shootOrigin = {boss->pos.x, boss->pos.y + 0.5f, boss->pos.z};
    // Tir avec propriétaire BOSS
    ShootProjectile(projs, shootOrigin, aimDir, OWNER_BOSS, boss->armeEquipee,
                    boss->yaw, boss->pitch);
    // Reset timer (avec une petite variation aléatoire)
    boss->chronoTir = (float)(rand() % 100) / 200.0f;  // Reset à 0.0 - 0.5s
  }

  // --- Physique & Mouvement (Gravité) ---

  boss->velocityY -= gravity;  // Application gravité

  // Calcul position future
  Vector3 nextPos = boss->pos;

  float distToMoveTarget = sqrtf(dxMove * dxMove + dzMove * dzMove);

  // Déplacement basique : Le boss avance doucement vers le joueur (zombie
  // style) Mais s'arrête s'il est trop près (pour tirer)
  if (playerVisible && distToPlayer <= 5.0f) {
    // Arrêt stratégique si le boss voit le joueur et est déjà proche
  } else if (distToMoveTarget > 0.5f) {
    // Tolérance (0.5) pour éviter que le boss tremble une fois arrivé au
    // centre d'une case de labyrinthe
    nextPos.x += sinf(boss->yaw) * speed;
    nextPos.z += cosf(boss->yaw) * speed;
  }

  nextPos.y += boss->velocityY;

  // --- Collisions (AABB) ---
  boss->onGround = false;

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];
      if (!b.isWall) continue;  // Optimisation pour vérifier que les murs

      float halfX = b.width / 2;
      float halfY = b.height / 2;
      float halfZ = b.depth / 2;

      bool collideX = nextPos.x + bossHalf > b.pos.x - halfX &&
                      nextPos.x - bossHalf < b.pos.x + halfX;
      bool collideY = nextPos.y + bossHalf > b.pos.y - halfY &&
                      nextPos.y - bossHalf < b.pos.y + halfY;
      bool collideZ = nextPos.z + bossHalf > b.pos.z - halfZ &&
                      nextPos.z - bossHalf < b.pos.z + halfZ;

      if (collideX && collideY && collideZ) {
        float top = b.pos.y + halfY;
        float bottom = b.pos.y - halfY;

        if (boss->velocityY < 0 && boss->pos.y - bossHalf >= top) {
          nextPos.y = top + bossHalf;
          boss->velocityY = 0;
          boss->onGround = true;
        } else if (boss->velocityY > 0 && boss->pos.y + bossHalf <= bottom) {
          nextPos.y = bottom - bossHalf;
          boss->velocityY = 0;
        } else {
          float overlapX = (halfX + bossHalf) - fabsf(nextPos.x - b.pos.x);
          float overlapZ = (halfZ + bossHalf) - fabsf(nextPos.z - b.pos.z);

          if (overlapX < overlapZ) {
            if (nextPos.x < b.pos.x)
              nextPos.x = b.pos.x - halfX - bossHalf;
            else
              nextPos.x = b.pos.x + halfX + bossHalf;
          } else {
            if (nextPos.z < b.pos.z)
              nextPos.z = b.pos.z - halfZ - bossHalf;
            else
              nextPos.z = b.pos.z + halfZ + bossHalf;
          }
        }
      }
    }
  }

  // --- Gravité et Sol Raylib (Sécurité) ---
  float oldBossBottom = boss->pos.y - bossHalf;  // Ancienne position Y
  float newBossBottom = nextPos.y - bossHalf;    // Nouvelle position Y
  float closestGround = 0.0f;                    // Sol par défaut

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];
      if (b.isWall) continue;

      float top = 0.1f;

      // S'il était au-dessus du sol, et que sa nouvelle position est en-dessous
      if (oldBossBottom >= top && newBossBottom <= top) {
        if (top > closestGround) closestGround = top;
      }
    }
  }

  // On utilise newBossBottom ici
  if (newBossBottom < closestGround && boss->velocityY <= 0) {
    nextPos.y = closestGround +
                bossHalf;  // On a déjà corrigé le + bossHalf tout à l'heure !
    boss->velocityY = 0;
    boss->onGround = true;
  }

  // Validation finale
  boss->pos = nextPos;
}
