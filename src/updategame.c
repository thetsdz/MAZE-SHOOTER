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
#include "../lib/headers/boss.h"
#include "../lib/headers/heal.h"
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
  if (IsKeyPressed(KEY_F3) && (joueur->score>=4 || joueur->armeUnlock[1]==0)) {
    joueur->armeEquipee = ObtenirModeleArme(FUSIL);
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    joueur->chronoTir = 0;
  }
  // F3 : Sniper
  if (IsKeyPressed(KEY_F2) && (joueur->score>=2 || joueur->armeUnlock[0]==0) ) {
    joueur->armeEquipee = ObtenirModeleArme(SNIPER);
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    joueur->chronoTir = 0;
  }
  if (IsKeyPressed(KEY_F4) && (joueur->score>=6 || joueur->armeUnlock[2]==0) ) { 
    joueur->armeEquipee = ObtenirModeleArme(GRENADE);
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    joueur->chronoTir = 0;
  }
}

void UpdateGame(Entity *player, Entity bot[18],Heal heal[10],
                Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Projectile projs[MAX_PROJ], Camera3D* camera,
                GameScreen* currentScreen, Entity* boss, bool* IsBossAlive) {
  // --- Logique du jeu ---
  Entity* bot_ptr = &bot[0];
  int armeUnlock=-1;
  UpdatePlayer(player, blocks, camera, &bot_ptr);
    
    if(*IsBossAlive ==  true)
        UpdateBoss(boss, blocks, player->pos, projs);
    else if(((player->score)%50==0) && (player->score)!=0){
        *IsBossAlive=true;
        InitBoss(boss, blocks);
    }
    for (int i = 0; i < 18; i++) {
        UpdateBot(&bot[i], blocks, player->pos, projs);
    }
    for(int i = 0; i < 10; i++) {
        armeUnlock=UpdateHeal(&heal[i],player,blocks);
        if (armeUnlock!=0) {
          switch (armeUnlock){
            case 1 : player->armeUnlock[1]=0; break; // par convention le tableau represente sniper,fusil,grenade mais fusil vaut 1 (bref mal organisé je sais)
            case 2 : player->armeUnlock[0]=0; break;
            case 3 : player->armeUnlock[2]=0; break;
            default : break;
          }
        }
    }

  if (IsKeyPressed(KEY_Y)) sauvegarder(player, bot);

    if (IsKeyPressed(KEY_R)) {
        player->ammo = player->armeEquipee.munitionsMax;
        PlayReload();
    }

    if (player->chronoTir > 0) {
        player->chronoTir -= GetFrameTime();
    }
    // --- Changement d'arme ---
    ChangementArme(player);

    if (IsKeyPressed(KEY_R))
        player->ammo = player->armeEquipee.munitionsMax;
    ;

    if(IsKeyPressed(KEY_M))
        player->score+=50;
    if(IsKeyPressed(KEY_Q))
        player->pos = boss-> pos;
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

        ShootProjectile(projs, startPos, camDir, OWNER_PLAYER,
                        player->armeEquipee, player->yaw, player->pitch);

    player->ammo--;
    player->chronoTir =
        player->armeEquipee.cadenceTir;  // On réinitialise le délai
  }
  UpdateProjectiles(projs, blocks, &bot, player, currentScreen, IsBossAlive, boss);
}