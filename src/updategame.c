#include "../lib/headers/updategame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/headers/bot.h"
#include "../lib/headers/player.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/sauvegarde.h"
#include "../lib/headers/types.h"
#include "../lib/headers/arme.h"







void UpdateGame(Entity* player, Entity* bot,
                Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Projectile projs[MAX_PROJ], int* score, Camera3D* camera) {
  // --- Logique du jeu ---
  UpdatePlayer(player, blocks, camera, bot);
  UpdateBot(bot, blocks, player->pos, projs);
 // if (IsKeyPressed(KEY_Y)) sauvegarder(player, score);


 if (player->chronoTir > 0) {
        player->chronoTir -= GetFrameTime();
    }
 // --- Changement d'arme ---
// F1 : Pistolet
if (IsKeyPressed(KEY_F1)) {
    player->armeEquipee = ObtenirModeleArme(PISTOLET);
    player->ammo = player->armeEquipee.munitionsMax; // On recharge auto au changement ?
    player->chronoTir = 0; 
}
// F2 : Fusil
if (IsKeyPressed(KEY_F2)) {
    player->armeEquipee = ObtenirModeleArme(FUSIL);
    player->ammo = player->armeEquipee.munitionsMax;
    player->chronoTir = 0;
}
// F3 : Sniper
if (IsKeyPressed(KEY_F3)) {
    player->armeEquipee = ObtenirModeleArme(SNIPER);
    player->ammo = player->armeEquipee.munitionsMax;
    player->chronoTir = 0;
}
    if (IsKeyPressed(KEY_F4)) {
    player->armeEquipee = ObtenirModeleArme(GRENADE);
    player->ammo = player->armeEquipee.munitionsMax;
    player->chronoTir = 0;
}

  if (IsKeyPressed(KEY_R)) player->ammo = player->armeEquipee.munitionsMax;;

  if (IsKeyPressed(KEY_E) && *score >= SCORE_TRADE &&
      player->armeEquipee.munitionsMax < MAX_PROJ) {
    *score -= SCORE_TRADE;
    player->armeEquipee.munitionsMax += 2;
    /*TraceLog(LOG_INFO, "Achat amélioration : nouvelle capacité max = %d",
             player->maxAmmo);*/
  }


  bool veutTirer = false;
  if (player->armeEquipee.type == FUSIL) {
      veutTirer = IsMouseButtonDown(MOUSE_BUTTON_LEFT); // Continu
  } else {
      veutTirer = IsMouseButtonPressed(MOUSE_BUTTON_LEFT); // Coup par coup
  }
  if (veutTirer && player->ammo > 0 && player->chronoTir <= 0) {
    Vector3 camDir = {sinf(player->yaw) * cosf(player->pitch),
                      sinf(player->pitch),
                      cosf(player->yaw) * cosf(player->pitch)};
    Vector3 startPos = {player->pos.x, player->pos.y + 0.5f, player->pos.z};

    
    ShootProjectile(projs, startPos, camDir, OWNER_PLAYER, player->armeEquipee, player->armeEquipee.vitesseBalle, 
                player->armeEquipee.tailleProjectile, 
                player->armeEquipee.couleurProjectile);

    
    player->ammo--;
    player->chronoTir = player->armeEquipee.cadenceTir; // On réinitialise le délai
  }

  UpdateProjectiles(projs, blocks, bot, player, score);
}