/**
 * \file bot.c
 */

#include "../lib/headers/bot.h"

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
// Retourne vrai si aucun mur ne sépare le bot du joueur
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
    return start;  // Si le joueur est inatteignable (bug/mur fermé), on reste
                   // sur place

  // Remonter le chemin depuis la cible jusqu'à la première case après le départ
  GridPos curr = target;
  while (parent[curr.i][curr.j].i != start.i ||
         parent[curr.i][curr.j].j != start.j) {
    curr = parent[curr.i][curr.j];
  }

  return curr;
}

void InitBot(Entity* bot,
             Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {  // temporaire pour respawn
  bot->yaw = 0.0f;                                    // angle du bot
  bot->pitch = 0.0f;                                  // angle du bot
  bot->velocityY = 0.0f;                              // vitesse du bot
  bot->onGround = true;  // le bot est initialisée au sol
  bot->size = 1.0f;      // taille du bot
  bot->health = 100;     // Points de vie du bot
  bot->maxHealth = 100;  // Points de vie maximum du bot
  bot->life = 10000;     // Le bot a une vie très longue
  bot->armeEquipee = ObtenirModeleArme(PISTOLET);  //
  bot->type = ENTITY_BOT;                          // type de l'entité
  bot->chronoTir = (float)(rand() % 100) / 100.0f;
  // --- RECHERCHE D'UN SPAWN ALÉATOIRE ---
  float offset = NUM_BLOCKS - 1;  // Le même offset que dans level.c
  int i, j;

  // On boucle jusqu'à trouver une case qui N'EST PAS un mur
  do {
    // rand() % NUM_BLOCKS donne un nombre entre 0 et 50
    i = rand() % NUM_BLOCKS;
    j = rand() % NUM_BLOCKS;
  } while (blocks[i][j].isWall);

  bot->pos.x = i * 3.0f - offset;
  bot->pos.z = j * 3.0f - offset;
  bot->pos.y = 5.0f;  // On le fait spawner un peu en l'air pour qu'il retombe
                      // doucement au sol */
}

void UpdateBot(Entity* bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
               Vector3 targetPos, Projectile* projs) {
  /** \brief Paramètres du Bot
      \code
      float speed = 0.08f;
      float gravity = 0.02f;
      float botHalf = bot->size/2;
      float dt = GetFrameTime();
      \endcode
  */
  float speed = 0.08f;
  float gravity = 0.02f;
  float botHalf = bot->size / 2;
  float dt = GetFrameTime();
  float offset = NUM_BLOCKS - 1;

  // 1. --- ANALYSE DE LA SITUATION ---
  bool playerVisible = IsPlayerVisible(bot->pos, targetPos, blocks);
  Vector3 moveTarget = targetPos;  // Par défaut, on va vers le joueur

  // 2. --- LOGIQUE DE NAVIGATION (PATHFINDING) ---
  if (!playerVisible) {
    GridPos botGrid = {(int)roundf((bot->pos.x + offset) / 3.0f),
                       (int)roundf((bot->pos.z + offset) / 3.0f)};
    GridPos playerGrid = {(int)roundf((targetPos.x + offset) / 3.0f),
                          (int)roundf((targetPos.z + offset) / 3.0f)};

    // Sécurité pour ne pas sortir du tableau
    if (botGrid.i < 0)
      botGrid.i = 0;
    else if (botGrid.i >= NUM_BLOCKS)
      botGrid.i = NUM_BLOCKS - 1;
    if (botGrid.j < 0)
      botGrid.j = 0;
    else if (botGrid.j >= NUM_BLOCKS)
      botGrid.j = NUM_BLOCKS - 1;
    if (playerGrid.i < 0)
      playerGrid.i = 0;
    else if (playerGrid.i >= NUM_BLOCKS)
      playerGrid.i = NUM_BLOCKS - 1;
    if (playerGrid.j < 0)
      playerGrid.j = 0;
    else if (playerGrid.j >= NUM_BLOCKS)
      playerGrid.j = NUM_BLOCKS - 1;

    GridPos nextStep = GetNextStepBFS(botGrid, playerGrid, blocks);

    // Conversion de la prochaine case Grid en position Monde
    moveTarget.x = nextStep.i * 3.0f - offset;
    moveTarget.z = nextStep.j * 3.0f - offset;
    moveTarget.y = bot->pos.y;  // Rester à la même hauteur
  }

  /** \brief Calcul de l'angle pour regarder le joueur sur le plan horizontal
     (XZ)
      \code
      float dx = targetPos.x - bot->pos.x;
      float dz = targetPos.z - bot->pos.z;
      bot->yaw = atan2f(dx, dz);
      \endcode
  */
  // 3. --- ORIENTATION ET VISÉE ---
  // Le robot regarde vers là où il se déplace
  float dxMove = moveTarget.x - bot->pos.x;
  float dzMove = moveTarget.z - bot->pos.z;
  bot->yaw = atan2f(dxMove, dzMove);

  /** \brief Viser en hauteur
      \code
      float dist = sqrtf(dx*dx + dz*dz);
      float dy = (targetPos.y + 0.5f) - (bot->pos.y + 0.5f);
      bot->pitch = atan2f(dy, dist);
      \endcode
  */
  // Visée vers le joueur en hauteur (Pitch)
  float dxPlayer = targetPos.x - bot->pos.x;
  float dzPlayer = targetPos.z - bot->pos.z;
  float distToPlayer = sqrtf(dxPlayer * dxPlayer + dzPlayer * dzPlayer);
  // float dy = (targetPos.y + 0.5f) - (bot->pos.y + 0.5f);
  // bot->pitch = atan2f(dy, distToPlayer); commenté pour avoir salto à
  //  decommenter et commenté la partie salto ci dessous pour arreter

  // --- Tir ---
  // CORRECTION : On utilise le chronoTir propre à l'entité
  bot->chronoTir += dt;

  // Le bot tire toutes les 1.5 à 2.5 secondes (aléatoire un peu)
  // Le bot NE TIRE QUE S'IL VOIT LE JOUEUR
  if (playerVisible && bot->chronoTir > 2.0f &&
      distToPlayer < 30.0f) {  // Ne tire que si < 30 mètres

    // Calcul du vecteur de visée parfait
    Vector3 aimDir = Vector3Subtract(targetPos, bot->pos);
    aimDir = Vector3Normalize(aimDir);

    // --- Ajout de l'IMPRÉCISION ---
    float spread = 0.15f;
    aimDir.x += ((float)(rand() % 100) / 50.0f - 1.0f) * spread;
    aimDir.y += ((float)(rand() % 100) / 50.0f - 1.0f) * spread;
    aimDir.z += ((float)(rand() % 100) / 50.0f - 1.0f) * spread;

    // Position de départ (au niveau des yeux du bot)
    Vector3 shootOrigin = {bot->pos.x, bot->pos.y + 0.5f, bot->pos.z};

    // Tir avec propriétaire BOT
    ShootProjectile(projs, shootOrigin, aimDir, OWNER_BOT, bot->armeEquipee);

    // Reset timer (avec une petite variation aléatoire)
    bot->chronoTir = (float)(rand() % 100) / 200.0f;  // Reset à 0.0 - 0.5s
  }

  // --- Physique & Mouvement (Gravité) ---

  // PARTIE SAUT DE BOT s'il est bloqué ou aléatoirement
  static float moveTimer = 0.0f;
  moveTimer += dt;

  if (fmodf(moveTimer, 4.0f) < 0.1f && bot->onGround) {
    bot->velocityY = 0.35f;  // Petit saut
    bot->onGround = false;
  }

  if (!bot->onGround) {
    // On tourne à 720°/seconde (soit un tour complet en 0.5s)
    if (bot->pitch < 360.0f) {
      bot->pitch += 720.0f * dt;
    }
  } else {
    // Dès qu'il touche le sol, paf, il est droit
    bot->pitch = 0.0f;
  }

  bot->velocityY -= gravity;  // Application gravité

  // Calcul position future
  Vector3 nextPos = bot->pos;

  float distToMoveTarget = sqrtf(dxMove * dxMove + dzMove * dzMove);

  // Déplacement basique : Le bot avance doucement vers le joueur (zombie style)
  // Mais s'arrête s'il est trop près (pour tirer)
  if (playerVisible && distToPlayer <= 5.0f) {
    // Arrêt stratégique si le bot voit le joueur et est déjà proche
  } else if (distToMoveTarget > 0.5f) {
    // Tolérance (0.5) pour éviter que le bot tremble une fois arrivé au centre
    // d'une case de labyrinthe
    nextPos.x += sinf(bot->yaw) * speed;
    nextPos.z += cosf(bot->yaw) * speed;
  }

  nextPos.y += bot->velocityY;

  // --- Collisions (AABB) ---
  bot->onGround = false;

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];
      if (!b.isWall) continue;  // Optimisation pour vérifier que les murs

      float halfX = b.width / 2;
      float halfY = b.height / 2;
      float halfZ = b.depth / 2;

      bool collideX = nextPos.x + botHalf > b.pos.x - halfX &&
                      nextPos.x - botHalf < b.pos.x + halfX;
      bool collideY = nextPos.y + botHalf > b.pos.y - halfY &&
                      nextPos.y - botHalf < b.pos.y + halfY;
      bool collideZ = nextPos.z + botHalf > b.pos.z - halfZ &&
                      nextPos.z - botHalf < b.pos.z + halfZ;

      if (collideX && collideY && collideZ) {
        float top = b.pos.y + halfY;
        float bottom = b.pos.y - halfY;

        if (bot->velocityY < 0 && bot->pos.y - botHalf >= top) {
          nextPos.y = top + botHalf;
          bot->velocityY = 0;
          bot->onGround = true;
        } else if (bot->velocityY > 0 && bot->pos.y + botHalf <= bottom) {
          nextPos.y = bottom - botHalf;
          bot->velocityY = 0;
        } else {
          float overlapX = (halfX + botHalf) - fabsf(nextPos.x - b.pos.x);
          float overlapZ = (halfZ + botHalf) - fabsf(nextPos.z - b.pos.z);

          if (overlapX < overlapZ) {
            if (nextPos.x < b.pos.x)
              nextPos.x = b.pos.x - halfX - botHalf;
            else
              nextPos.x = b.pos.x + halfX + botHalf;
          } else {
            if (nextPos.z < b.pos.z)
              nextPos.z = b.pos.z - halfZ - botHalf;
            else
              nextPos.z = b.pos.z + halfZ + botHalf;
          }
        }
      }
    }
  }

  // --- Gravité et Sol Raylib (Sécurité) ---
  float botBottom = nextPos.y - botHalf;
  float closestGround = 0.0f;  // Sol par défaut

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];
      if (b.isWall)
        continue;  // On vérifie le sol sur les espaces vides, Ignore les murs
                   // pleins, cherche les sols
      float top = 0.1f;  // Le sol par défaut dessiné dans votre level.c fait Y
                         // = ~0, top = ~0.1
      if (botBottom >= top && nextPos.y - botHalf <= top) {
        if (top > closestGround) closestGround = top;
      }
    }
  }

  if (botBottom <= closestGround) {
    nextPos.y = closestGround + botHalf;
    bot->velocityY = 0;
    bot->onGround = true;
  }

  // Validation finale
  bot->pos = nextPos;
}
