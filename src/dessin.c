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
#include "rlgl.h"

void UpdateDessinGame(Entity* bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                      Camera3D camera, Projectile projs[MAX_PROJ], int score,
                      Entity player, Texture2D viseur, Texture2D armeTex,
                      Model skyModel, Texture2D wallTex, Texture2D floorTex,
                      Model botModel) {
  // --- Dessin 3D ---
  BeginMode3D(camera);

  // --- Skybox ---
  rlDisableBackfaceCulling();
  rlDisableDepthMask();
  DrawModel(skyModel, camera.position, 1.0f, WHITE);
  rlEnableBackfaceCulling();
  rlEnableDepthMask();

  // --- Niveau ---
  DrawLevel(blocks, wallTex, floorTex);

  // --- Bot ---
  DrawModelEx(botModel, bot->pos, (Vector3){0, 1, 0}, bot->yaw * RAD2DEG,
              (Vector3){bot->size, bot->size, bot->size}, WHITE);

  // --- Projectiles ---
  DrawProjectiles(projs);

  EndMode3D();

  // --- UI 2D ---
  DrawText(TextFormat("Score: %d | FPS: %d", score, GetFPS()), 10, 10, 20,
           DARKGRAY);

  Color ammoColor = (player.ammo == 0) ? RED : DARKGREEN;
  DrawText(TextFormat("Munitions: %d / %d", player.ammo, player.maxAmmo), 10,
           40, 20, ammoColor);

  if (player.ammo < player.maxAmmo)
    DrawText("Appuyez sur [R] pour Recharger", 10, 65, 10, GRAY);

  if (player.maxAmmo < MAX_PROJ) {
    if (score >= SCORE_TRADE) {
      DrawText("Appuyez sur [E] pour +2 Munitions Max (-100 pts)", 10, 90, 20,
               GOLD);
    } else {
      DrawText(
          TextFormat("Prochaine amélioration: 100 pts (Actuel: %d)", score), 10,
          90, 10, LIGHTGRAY);
    }
  } else {
    DrawText("Capacité MAX atteinte (50)", 10, 90, 20, MAROON);
  }

  DessinerViseur(viseur, GetScreenWidth(), GetScreenHeight());
  DessinerArme(armeTex, GetScreenWidth(), GetScreenHeight());
  DrawText(TextFormat("Point de vie restant: %d", player.health), 10, 190, 20,
           RED);
}