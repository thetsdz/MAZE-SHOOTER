#include "../lib/headers/updategame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/headers/bot.h"
#include "../lib/headers/player.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/sauvegarde.h"
#include "../lib/headers/types.h"
#include "../lib/headers/audio.h"

void UpdateGame(Entity* player, Entity* bot,
                Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Projectile projs[MAX_PROJ], int* score, Camera3D* camera) {
  // --- Logique du jeu ---
  UpdatePlayer(player, blocks, camera, bot);
  UpdateBot(bot, blocks, player->pos, projs);
  if (IsKeyPressed(KEY_Y)) sauvegarder(player, bot, score);

  if (IsKeyPressed(KEY_R)){
    player->ammo = player->maxAmmo;
    PlayReload();
  } 

  if (IsKeyPressed(KEY_E) && *score >= SCORE_TRADE &&
      player->maxAmmo < MAX_PROJ) {
    *score -= SCORE_TRADE;
    player->maxAmmo += 2;
    TraceLog(LOG_INFO, "Achat amélioration : nouvelle capacité max = %d",
             player->maxAmmo);
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->ammo > 0) {
    PlayMitraillette();
    Vector3 camDir = {sinf(player->yaw) * cosf(player->pitch),
                      sinf(player->pitch),
                      cosf(player->yaw) * cosf(player->pitch)};
    Vector3 startPos = {player->pos.x, player->pos.y + 0.5f, player->pos.z};
    ShootProjectile(projs, startPos, camDir, OWNER_PLAYER);
    player->ammo--;
  }

  UpdateProjectiles(projs, blocks, bot, player, score);
}