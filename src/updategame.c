/**
 * \file updategame.c
 */

#include "../lib/headers/updategame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/headers/arme.h"
#include "../lib/headers/audio.h"
#include "../lib/headers/bot.h"
#include "../lib/headers/player.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/sauvegarde.h"
#include "../lib/headers/types.h"

void ChangementArme(Entity* joueur) {
  // F1 : Pistolet
  if (IsKeyPressed(KEY_F1)) {
    joueur->armeEquipee = ObtenirModeleArme(PISTOLET);
    joueur->ammo =
        joueur->armeEquipee.munitionsMax;  // On recharge auto au changement ?
    joueur->chronoTir = 0;
  }
  // F2 : Fusil
  if (IsKeyPressed(KEY_F2)) {
    joueur->armeEquipee = ObtenirModeleArme(FUSIL);
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    joueur->chronoTir = 0;
  }
  // F3 : Sniper
  if (IsKeyPressed(KEY_F3)) {
    joueur->armeEquipee = ObtenirModeleArme(SNIPER);
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    joueur->chronoTir = 0;
  }
  if (IsKeyPressed(KEY_F4)) {
    joueur->armeEquipee = ObtenirModeleArme(GRENADE);
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    joueur->chronoTir = 0;
  }
}

void UpdateGame(Entity* player, Entity bot[18],
                Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Projectile projs[MAX_PROJ], int* score, Camera3D* camera) {
  // --- Logique du jeu ---
  Entity* bot_ptr = &bot[0];
  UpdatePlayer(player, blocks, camera, &bot_ptr);

  for (int i = 0; i < 18; i++) {
    UpdateBot(&bot[i], blocks, player->pos, projs);
  }

  if (IsKeyPressed(KEY_Y)) sauvegarder(player, bot, score);

  if (IsKeyPressed(KEY_R)) {
    player->ammo = player->armeEquipee.munitionsMax;
    PlayReload();
  }

  if (player->chronoTir > 0) {
    player->chronoTir -= GetFrameTime();
  }
  // --- Changement d'arme ---
  ChangementArme(player);

  if (IsKeyPressed(KEY_R)) player->ammo = player->armeEquipee.munitionsMax;
  ;

  if (IsKeyPressed(KEY_E) && *score >= SCORE_TRADE &&
      player->armeEquipee.munitionsMax < MAX_PROJ) {
    *score -= SCORE_TRADE;
    player->armeEquipee.munitionsMax += 2;
    TraceLog(LOG_INFO, "Achat amélioration : nouvelle capacité max = %d",
             player->ammo);
  }

  bool veutTirer = false;
  if (player->armeEquipee.type == FUSIL) {
    veutTirer = IsMouseButtonDown(MOUSE_BUTTON_LEFT);  // Continu
  } else {
    veutTirer = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);  // Coup par coup
  }
  if (veutTirer && player->ammo > 0 && player->chronoTir <= 0) {
    Vector3 camDir = {sinf(player->yaw) * cosf(player->pitch),
                      sinf(player->pitch),
                      cosf(player->yaw) * cosf(player->pitch)};
    Vector3 startPos = {player->pos.x, player->pos.y + 0.5f, player->pos.z};

    ShootProjectile(projs, startPos, camDir, OWNER_PLAYER, player->armeEquipee);

    player->ammo--;
    player->chronoTir =
        player->armeEquipee.cadenceTir;  // On réinitialise le délai
  }
  UpdateProjectiles(projs, blocks, &bot, player, score);
}