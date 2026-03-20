/**
 * \file dessin.c
 */



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
                      Entity player, Texture2D viseur, Texture2D tabArmes[4],
                      Model skyModel, Texture2D wallTex, Texture2D floorTex,
                      Model botModel, Model tabModels[4]) {
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
  /*Vector3 drawPos = bot->pos;
  drawPos.y -= 0.5f; //pour pied au sol
  DrawModelEx(botModel, drawPos, (Vector3){0, 1, 0}, (bot->yaw * RAD2DEG)- 90.0f,
              (Vector3){0.3f, 0.3f, 0.3f}, WHITE); */
  
  Vector3 drawPos = bot->pos;
  drawPos.y -= 0.5f; // On ajuste pour que les pieds touchent le sol

  // --- 2. Création de la Transformation ---
  // On part d'une matrice vide (Identity)
  Matrix transform = MatrixIdentity();

  // A. On applique le SALTO (Rotation sur l'axe X local)
  transform = MatrixMultiply(transform, MatrixRotateX(bot->pitch * DEG2RAD));

  // B. On applique le REGARD (Rotation sur l'axe Y mondial)
  // On ajoute le +90.0f pour compenser l'épaule du modèle
  float angleFinal = (bot->yaw * RAD2DEG) - 90.0f;
  transform = MatrixMultiply(transform, MatrixRotateY(angleFinal * DEG2RAD));

  // --- 3. Application et Dessin ---
  botModel.transform = transform; 
  DrawModel(botModel, drawPos, 0.3f, WHITE);


  // --- Projectiles ---
  DrawProjectiles(projs,tabModels);

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
  TypeArme tab[4]={PISTOLET, FUSIL, SNIPER,GRENADE};  
  int i=0;
  while (player.armeEquipee.type!=tab[i]){
    i+=1;
  }
  DessinerArme(tabArmes[i], GetScreenWidth(), GetScreenHeight());

  DrawText(TextFormat("Point de vie restant: %d", player.health), 10, 190, 20,
           RED);
}