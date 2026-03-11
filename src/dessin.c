#include "../lib/headers/dessin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/headers/asset.h"
#include "../lib/headers/level.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/types.h"
#include "raylib.h"
#include "raymath.h"

void UpdateDessinGame(Entity* bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                      Camera3D camera, Projectile projs[MAX_PROJ], int score,
                      Entity player, Texture2D viseur, Texture2D tabArmes[4]) {

  int i = player.armeEquipee.type;
  // --- Dessin 3D ---
  BeginMode3D(camera);
  DrawLevel(blocks);
  DrawCube(bot->pos, bot->size, bot->size, bot->size, RED);
  Vector3 lookDir = {sinf(bot->yaw), 0, cosf(bot->yaw)};
  Vector3 eyePos = Vector3Add(bot->pos, Vector3Scale(lookDir, 0.5f));
  eyePos.y += 0.3f;
  DrawCube(eyePos, 0.2f, 0.2f, 0.2f, BLACK);
  DrawProjectiles(projs);
  EndMode3D();

  // --- UI 2D ---
  DrawText(TextFormat("Score: %d | FPS: %d", score, GetFPS()), 10, 10, 20,
           DARKGRAY);
  DrawText(TextFormat("Arme : %s", player.armeEquipee.nom), 10, 35, 20, DARKGRAY);
  Color ammoColor = (player.ammo == 0) ? RED : DARKGREEN;
  DrawText(TextFormat("Munitions: %d / %d", player.ammo, player.armeEquipee.munitionsMax), 10,
           60, 20, ammoColor);
  if (player.ammo < player.armeEquipee.munitionsMax)
    DrawText("Appuyez sur [R] pour Recharger", 10, 85, 10, GRAY);
  if (player.armeEquipee.munitionsMax < MAX_PROJ) {
    if (score >= SCORE_TRADE) {
      DrawText("Appuyez sur [E] pour +2 Munitions Max (-100 pts)", 10, 100, 20,
               GOLD);
    } else {
      DrawText(
          TextFormat("Prochaine amélioration: 100 pts (Actuel: %d)", score), 10,
          100, 10, LIGHTGRAY);
    }
  } else {
    DrawText("Capacité MAX atteinte (50)", 10, 100, 20, MAROON);
  }
  DessinerViseur(viseur, GetScreenWidth(), GetScreenHeight());
  
  DessinerArme(tabArmes[i], GetScreenWidth(), GetScreenHeight());

  DrawText(TextFormat("Point de vie restant: %d", player.health), 10, 190, 20,
           RED);
}