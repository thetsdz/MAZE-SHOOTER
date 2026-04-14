/**
 * \file boss.c
 */

#include "../lib/headers/boss.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../lib/headers/arme.h"
#include "../lib/headers/recherche.h"

void InitBoss(Entity* boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  boss->yaw = 0.0f;
  boss->pitch = 0.0f;
  boss->velocityY = 0.0f;
  boss->onGround = true;
  boss->size = 2.5f;
  boss->health = 1000;
  boss->maxHealth = 1000;
  boss->life = 200;
  boss->armeEquipee = ObtenirModeleArme(SNIPER);
  boss->type = ENTITY_BOSS;
  boss->chronoTir = (float)(rand() % 100) / 100.0f;
  
  float offset = NUM_BLOCKS - 1;
  int i, j;

  do {
    i = rand() % NUM_BLOCKS;
    j = rand() % NUM_BLOCKS;
  } while (blocks[i][j].isWall);

  boss->pos.x = i * 3.0f - offset;
  boss->pos.z = j * 3.0f - offset;
  boss->pos.y = 5.0f;
}

void UpdateBoss(Entity* boss, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Vector3 targetPos, Projectile* projs) {
  float speed = 0.04f;
  float gravity = 0.02f;
  float bossHalf = boss->size / 2.0f;
  float dt = GetFrameTime();
  float offset = NUM_BLOCKS - 1;

  // 1. --- ANALYSE DE LA SITUATION ---
  bool playerVisible = IsPlayerVisible(boss->pos, targetPos, blocks);
  Vector3 moveTarget = targetPos;

  // 2. --- LOGIQUE DE NAVIGATION (PATHFINDING) ---
  if (!playerVisible) {
    Coord bossGrid = {
        (int)roundf((boss->pos.x + offset) / 3.0f),
        (int)roundf((boss->pos.z + offset) / 3.0f)
    };
    Coord playerGrid = {
        (int)roundf((targetPos.x + offset) / 3.0f),
        (int)roundf((targetPos.z + offset) / 3.0f)
    };

    // Sécurisation stricte des indices
    bossGrid.i = (bossGrid.i < 0) ? 0 : ((bossGrid.i >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : bossGrid.i);
    bossGrid.j = (bossGrid.j < 0) ? 0 : ((bossGrid.j >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : bossGrid.j);
    playerGrid.i = (playerGrid.i < 0) ? 0 : ((playerGrid.i >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : playerGrid.i);
    playerGrid.j = (playerGrid.j < 0) ? 0 : ((playerGrid.j >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : playerGrid.j);

    Coord nextStep = GetNextStepBFS(bossGrid, playerGrid, blocks);
    moveTarget.x = nextStep.i * 3.0f - offset;
    moveTarget.z = nextStep.j * 3.0f - offset;
    moveTarget.y = boss->pos.y; 
  }

  // 3. --- ORIENTATION ET VISÉE ---
  float dxMove = moveTarget.x - boss->pos.x;
  float dzMove = moveTarget.z - boss->pos.z;
  boss->yaw = atan2f(dxMove, dzMove);

  float dxPlayer = targetPos.x - boss->pos.x;
  float dzPlayer = targetPos.z - boss->pos.z;
  float distToPlayer = sqrtf(dxPlayer * dxPlayer + dzPlayer * dzPlayer);
  
  // Visée vers le joueur en hauteur (Pitch)
  float dy = (targetPos.y + 0.5f) - (boss->pos.y + 0.5f);
  boss->pitch = atan2f(dy, distToPlayer);

  // 4. --- GESTION DU TIR ---
  boss->chronoTir += dt;
  if (playerVisible && boss->chronoTir > 2.0f && distToPlayer < 30.0f) {
    Vector3 aimDir = Vector3Normalize(Vector3Subtract(targetPos, boss->pos));
    Vector3 shootOrigin = {boss->pos.x, boss->pos.y + 0.5f, boss->pos.z};
    
    ShootProjectile(projs, shootOrigin, aimDir, OWNER_BOSS, boss->armeEquipee, boss->yaw, boss->pitch);
    boss->chronoTir = (float)(rand() % 100) / 200.0f;
  }

  // 5. --- PHYSIQUE ET MOUVEMENT ---
  boss->velocityY -= gravity; 
  Vector3 nextPos = boss->pos;
  float distToMoveTarget = sqrtf(dxMove * dxMove + dzMove * dzMove);

  if (!(playerVisible && distToPlayer <= 5.0f) && distToMoveTarget > 0.5f) {
    nextPos.x += sinf(boss->yaw) * speed;
    nextPos.z += cosf(boss->yaw) * speed;
  }
  nextPos.y += boss->velocityY;

  // 6. --- COLLISIONS AVEC LES MURS (OPTIMISÉES) ---
  boss->onGround = false;

  // Zone de collision locale pour éviter de boucler sur tout le niveau
  int gridI = (int)roundf((nextPos.x + offset) / 3.0f);
  int gridJ = (int)roundf((nextPos.z + offset) / 3.0f);
  int minI = (gridI - 2 < 0) ? 0 : gridI - 2;
  int maxI = (gridI + 2 >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : gridI + 2;
  int minJ = (gridJ - 2 < 0) ? 0 : gridJ - 2;
  int maxJ = (gridJ + 2 >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : gridJ + 2;

  for (int i = minI; i <= maxI; i++) {
    for (int j = minJ; j <= maxJ; j++) {
      Block b = blocks[i][j];
      if (!b.isWall) continue;

      float halfX = b.width / 2.0f;
      float halfY = b.height / 2.0f;
      float halfZ = b.depth / 2.0f;

      bool collideX = nextPos.x + bossHalf > b.pos.x - halfX && nextPos.x - bossHalf < b.pos.x + halfX;
      bool collideY = nextPos.y + bossHalf > b.pos.y - halfY && nextPos.y - bossHalf < b.pos.y + halfY;
      bool collideZ = nextPos.z + bossHalf > b.pos.z - halfZ && nextPos.z - bossHalf < b.pos.z + halfZ;

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
            nextPos.x = (nextPos.x < b.pos.x) ? (b.pos.x - halfX - bossHalf) : (b.pos.x + halfX + bossHalf);
          } else {
            nextPos.z = (nextPos.z < b.pos.z) ? (b.pos.z - halfZ - bossHalf) : (b.pos.z + halfZ + bossHalf);
          }
        }
      }
    }
  }

  // 7. --- COLLISIONS AVEC LE SOL ---
  float groundLevel = 0.1f;
  if (nextPos.y - bossHalf < groundLevel && boss->velocityY <= 0.0f) {
    nextPos.y = groundLevel + bossHalf;
    boss->velocityY = 0.0f;
    boss->onGround = true;
  }

  boss->pos = nextPos;
}