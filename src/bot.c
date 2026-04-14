/**
 * \file bot.c
 */

#include "../lib/headers/bot.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../lib/headers/recherche.h"
#include "../lib/headers/arme.h"

void InitBot(Entity* bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  bot->yaw = 0.0f;
  bot->pitch = 0.0f;
  bot->velocityY = 0.0f;
  bot->onGround = true;
  bot->size = 1.0f;
  bot->health = 100;
  bot->maxHealth = 100;
  bot->life = 10000;
  bot->armeEquipee = ObtenirModeleArme(PISTOLET);
  bot->type = ENTITY_BOT;
  bot->chronoTir = (float)(rand() % 100) / 100.0f;
  
  float offset = NUM_BLOCKS - 1;
  int i, j;

  // Apparition sur une case vide
  do {
    i = rand() % NUM_BLOCKS;
    j = rand() % NUM_BLOCKS;
  } while (blocks[i][j].isWall);

  bot->pos.x = i * 3.0f - offset;
  bot->pos.z = j * 3.0f - offset;
  bot->pos.y = 5.0f; // Chute depuis le ciel au spawn
}

void UpdateBot(Entity* bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Vector3 targetPos, Projectile* projs) {
  float speed = 0.08f;
  float gravity = 0.02f;
  float botHalf = bot->size / 2.0f;
  float dt = GetFrameTime();
  float offset = NUM_BLOCKS - 1;

  // 1. --- ANALYSE DE LA SITUATION ---
  bool playerVisible = IsPlayerVisible(bot->pos, targetPos, blocks);
  Vector3 moveTarget = targetPos; // Par défaut, on va en ligne droite vers le joueur

  // 2. --- LOGIQUE DE NAVIGATION (PATHFINDING) ---
  if (!playerVisible) { 
    Coord botGrid = {
        (int)roundf((bot->pos.x + offset) / 3.0f),
        (int)roundf((bot->pos.z + offset) / 3.0f)
    };
    Coord playerGrid = {
        (int)roundf((targetPos.x + offset) / 3.0f),
        (int)roundf((targetPos.z + offset) / 3.0f)
    };

    // Sécurisation stricte des indices du tableau (évite les SegFaults)
    botGrid.i = (botGrid.i < 0) ? 0 : ((botGrid.i >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : botGrid.i);
    botGrid.j = (botGrid.j < 0) ? 0 : ((botGrid.j >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : botGrid.j);
    playerGrid.i = (playerGrid.i < 0) ? 0 : ((playerGrid.i >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : playerGrid.i);
    playerGrid.j = (playerGrid.j < 0) ? 0 : ((playerGrid.j >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : playerGrid.j);
    
    Coord nextStep = GetNextStepBFS(botGrid, playerGrid, blocks);
    moveTarget.x = nextStep.i * 3.0f - offset;
    moveTarget.z = nextStep.j * 3.0f - offset;
    moveTarget.y = bot->pos.y;
  }

  // 3. --- ORIENTATION ET VISÉE ---
  float dxMove = moveTarget.x - bot->pos.x;
  float dzMove = moveTarget.z - bot->pos.z;
  bot->yaw = atan2f(dxMove, dzMove);

  float dxPlayer = targetPos.x - bot->pos.x;
  float dzPlayer = targetPos.z - bot->pos.z;
  float distToPlayer = sqrtf(dxPlayer * dxPlayer + dzPlayer * dzPlayer);

  // 4. --- GESTION DU TIR ---
  bot->chronoTir += dt;
  if (playerVisible && bot->chronoTir > 2.0f && distToPlayer < 30.0f) {
    Vector3 aimDir = Vector3Normalize(Vector3Subtract(targetPos, bot->pos));

    // Ajout d'imprécision pour le bot
    float spread = 0.15f;
    aimDir.x += ((float)(rand() % 100) / 50.0f - 1.0f) * spread;
    aimDir.y += ((float)(rand() % 100) / 50.0f - 1.0f) * spread;
    aimDir.z += ((float)(rand() % 100) / 50.0f - 1.0f) * spread;

    Vector3 shootOrigin = {bot->pos.x, bot->pos.y + 0.5f, bot->pos.z};
    ShootProjectile(projs, shootOrigin, aimDir, OWNER_BOT, bot->armeEquipee, bot->yaw, bot->pitch);
    
    bot->chronoTir = (float)(rand() % 100) / 200.0f; // Reset aléatoire
  }

  // 5. --- PHYSIQUE ET MOUVEMENT ---
  static float moveTimer = 10.0f;
  moveTimer += (dt / 18.0f);

  // Sauts aléatoires
  if (fmodf(moveTimer, 5.0f) < 0.1f && bot->onGround) {
    bot->velocityY = 0.35f;
    bot->onGround = false;
  }

  // Rotation (salto) dans les airs
  if (!bot->onGround) {
    if (bot->pitch < 360.0f) bot->pitch += 720.0f * dt;
  } else {
    bot->pitch = 0.0f;
  }

  bot->velocityY -= gravity;
  Vector3 nextPos = bot->pos;
  float distToMoveTarget = sqrtf(dxMove * dxMove + dzMove * dzMove);

  // Déplacement seulement si loin de la cible pour éviter les tremblements
  if (!(playerVisible && distToPlayer <= 5.0f) && distToMoveTarget > 0.5f) {
    nextPos.x += sinf(bot->yaw) * speed;
    nextPos.z += cosf(bot->yaw) * speed;
  }
  nextPos.y += bot->velocityY;

  // 6. --- COLLISIONS AVEC LES MURS (OPTIMISÉES) ---
  bot->onGround = false;

  // Calcul d'une zone de collision locale (rayon de 2 cases autour du bot)
  int gridI = (int)roundf((nextPos.x + offset) / 3.0f);
  int gridJ = (int)roundf((nextPos.z + offset) / 3.0f);
  int minI = (gridI - 2 < 0) ? 0 : gridI - 2;
  int maxI = (gridI + 2 >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : gridI + 2;
  int minJ = (gridJ - 2 < 0) ? 0 : gridJ - 2;
  int maxJ = (gridJ + 2 >= NUM_BLOCKS) ? NUM_BLOCKS - 1 : gridJ + 2;

  // On boucle uniquement sur les blocs proches !
  for (int i = minI; i <= maxI; i++) {
    for (int j = minJ; j <= maxJ; j++) {
      Block b = blocks[i][j];
      if (!b.isWall) continue;

      float halfX = b.width / 2.0f;
      float halfY = b.height / 2.0f;
      float halfZ = b.depth / 2.0f;

      bool collideX = nextPos.x + botHalf > b.pos.x - halfX && nextPos.x - botHalf < b.pos.x + halfX;
      bool collideY = nextPos.y + botHalf > b.pos.y - halfY && nextPos.y - botHalf < b.pos.y + halfY;
      bool collideZ = nextPos.z + botHalf > b.pos.z - halfZ && nextPos.z - botHalf < b.pos.z + halfZ;

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
            nextPos.x = (nextPos.x < b.pos.x) ? (b.pos.x - halfX - botHalf) : (b.pos.x + halfX + botHalf);
          } else {
            nextPos.z = (nextPos.z < b.pos.z) ? (b.pos.z - halfZ - botHalf) : (b.pos.z + halfZ + botHalf);
          }
        }
      }
    }
  }

  // 7. --- COLLISIONS AVEC LE SOL (OPTIMISÉES) ---
  float groundLevel = 0.1f; // Hauteur constante du sol de votre labyrinthe
  if (nextPos.y - botHalf < groundLevel && bot->velocityY <= 0.0f) {
    nextPos.y = groundLevel + botHalf;
    bot->velocityY = 0.0f;
    bot->onGround = true;
  }

  bot->pos = nextPos;
}