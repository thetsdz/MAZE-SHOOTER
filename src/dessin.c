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




#define MINIMAP_W 400
#define MINIMAP_H 300
#define MINIMAP_PADDING 10



void DrawProjectiles(Projectile *projs, Model tabModels[4]) {
    for (int i = 0; i < MAX_PROJ; i++) {
        if (!projs[i].active) continue;

        // Reset de la matrice du modèle spécifique avant de travailler dessus
        tabModels[projs[i].type].transform = MatrixIdentity();

        switch (projs[i].type) {
            case PROJ_PISTOLET: {
                float s = 0.25f;
                // On combine le fait de le coucher (90°) AVEC ton inclinaison (Pitch) sur l'axe X
                // (Mets un "+" ou un "-" devant le pitch selon si la balle monte ou descend)
                Matrix rot = MatrixRotateX((90.0f - projs[i].pitch) * DEG2RAD); 
                rot = MatrixMultiply(rot, MatrixRotateY(projs[i].yaw * DEG2RAD));
                
                tabModels[PROJ_PISTOLET].transform = rot;
                DrawModel(tabModels[PROJ_PISTOLET], projs[i].pos, s, WHITE);
                break;
            }
            case PROJ_FUSIL: {
              float s = 0.25f; // Taille du fusil
              // On fait EXACTEMENT comme le pistolet : on combine le 90° et le pitch sur l'axe X !
              Matrix rot = MatrixRotateX((90.0f - projs[i].pitch) * DEG2RAD); 
              rot = MatrixMultiply(rot, MatrixRotateY(projs[i].yaw * DEG2RAD));
              
              tabModels[PROJ_FUSIL].transform = rot;
              DrawModel(tabModels[PROJ_FUSIL], projs[i].pos, s, WHITE);
              break;
            }

           case PROJ_SNIPER: {
              float s = 0.25f; 
              // On utilise la MÊME logique de rotation que le pistolet
              // Si le sniper est "perpendiculaire", on garde le (90.0f - pitch)
              Matrix rot = MatrixRotateX((90.0f - projs[i].pitch) * DEG2RAD);
              rot = MatrixMultiply(rot, MatrixRotateY(projs[i].yaw * DEG2RAD));
              
              // LA SEULE DIFFÉRENCE POSSIBLE : 
              // Si la balle de sniper pointe à gauche/droite au lieu de devant, 
              // on ajoute un petit quart de tour final ici :
              // rot = MatrixMultiply(MatrixRotateZ(90.0f * DEG2RAD), rot);

              tabModels[PROJ_SNIPER].transform = rot;
              DrawModel(tabModels[PROJ_SNIPER], projs[i].pos, s, WHITE);
              break;
            }
            case PROJ_GRENADE: {
                float s = 0.2f;
                // Pas de rotation complexe nécessaire pour la grenade
                DrawModel(tabModels[PROJ_GRENADE], projs[i].pos, s, WHITE);
                break;


            }
        }
      //pour  tester taille balle mettre le switch en commentaire et prendre DrawSphere
      //DrawSphere(projs[i].pos, projs[i].radius, projs[i].color);
    }
}
  




void minimap(Entity player, Entity bot[18],
             Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  int minimapX = GetScreenWidth() - MINIMAP_W - MINIMAP_PADDING;
  int minimapY = MINIMAP_PADDING;

  float blockSize = blocks[0][0].width;
  float mapTotalSize = NUM_BLOCKS * blockSize;

  float scaleX = (float)MINIMAP_W / mapTotalSize;
  float scaleY = (float)MINIMAP_H / mapTotalSize;

  // Origine de la carte
  float originX = blocks[0][0].pos.x - blocks[0][0].width / 2.0f;
  float originZ = blocks[0][0].pos.z - blocks[0][0].depth / 2.0f;

  // Fond + bordure
  DrawRectangle(minimapX, minimapY, MINIMAP_W, MINIMAP_H,
                (Color){0, 0, 0, 180});
  DrawRectangleLines(minimapX, minimapY, MINIMAP_W, MINIMAP_H, WHITE);

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];

      if (!b.isWall) continue;

      int dotW = (int)(b.width * scaleX);
      int dotH = (int)(b.depth * scaleY);
      if (dotW < 1) dotW = 1;
      if (dotH < 1) dotH = 1;

      int dotX = minimapX + (int)((b.pos.x - originX) * scaleX) - dotW / 2;
      int dotY = minimapY + (int)((b.pos.z - originZ) * scaleY) - dotH / 2;

      DrawRectangle(dotX, dotY, dotW, dotH, b.color);
    }
  }

  // Joueur
  int playerDotX = minimapX + (int)((player.pos.x - originX) * scaleX);
  int playerDotY = minimapY + (int)((player.pos.z - originZ) * scaleY);
  DrawRectangle(playerDotX - 3, playerDotY - 3, 6, 6, GREEN);
  int botDoxX = minimapX + (int)((bot[0].pos.x - originX) * scaleX);
  int botDotY = minimapY + (int)((bot[0].pos.z - originZ) * scaleY);
  DrawRectangle(botDoxX - 3, botDotY - 3, 6, 6, RED);
  for (int i = 1; i < 18; i++) {
    botDoxX = minimapX + (int)((bot[i].pos.x - originX) * scaleX);
    botDotY = minimapY + (int)((bot[i].pos.z - originZ) * scaleY);
    DrawRectangle(botDoxX - 3, botDotY - 3, 6, 6, RED);
  }
}

void UpdateDessinGame(Entity bot[18], Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                      Camera3D camera, Projectile projs[MAX_PROJ], int score,
                      Entity player, Texture2D viseur, Model tabArmes[4],
                      Model skyModel, Model wallModel, Model floorModel,
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
  DrawLevel(blocks, wallModel, floorModel);
  // --- Bot ---
<<<<<<< HEAD
  Vector3 drawPos = bot->pos;
  drawPos.y -= 0.5f; // On ajuste pour que les pieds touchent le sol
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
  
=======
  /*Vector3 drawPos = bot->pos;
  drawPos.y -= 0.5f; //pour pied au sol
  DrawModelEx(botModel, drawPos, (Vector3){0, 1, 0}, (bot->yaw *
  RAD2DEG)- 90.0f, (Vector3){0.3f, 0.3f, 0.3f}, WHITE); */

  for (int i = 0; i < 18; i++) {
    Vector3 drawPos = bot[i].pos;
    drawPos.y -= 0.5f;  // On ajuste pour que les pieds touchent le sol

    // --- 2. Création de la Transformation ---
    // On part d'une matrice vide (Identity)
    Matrix transform = MatrixIdentity();

    // A. On applique le SALTO (Rotation sur l'axe X local)
    transform =
        MatrixMultiply(transform, MatrixRotateX(bot[i].pitch * DEG2RAD));

    // B. On applique le REGARD (Rotation sur l'axe Y mondial)
    // On ajoute le +90.0f pour compenser l'épaule du modèle
    float angleFinal = (bot[i].yaw * RAD2DEG) - 90.0f;
    transform = MatrixMultiply(transform, MatrixRotateY(angleFinal * DEG2RAD));
>>>>>>> master

    // --- 3. Application et Dessin ---
    botModel.transform = transform;
    DrawModel(botModel, drawPos, 0.3f, WHITE);
  }
  // --- Projectiles ---
  DrawProjectiles(projs,tabModels);

  EndMode3D();

  //--- armes --- apres de dernier End3Mod3D()
  TypeArme tab[4]={PISTOLET, FUSIL, SNIPER,GRENADE};  

  int i=0;
  while (player.armeEquipee.type!=tab[i]){
    i+=1;
  }
  DessinerArme(tabArmes[i],i);

  // --- UI 2D ---  
  DrawText(TextFormat("Score: %d | FPS: %d", score, GetFPS()), 10, 10, 20,
           DARKGRAY);
  DrawText(TextFormat("Arme : %s", player.armeEquipee.nom), 10, 35, 20,
           DARKGRAY);
  Color ammoColor = (player.ammo == 0) ? RED : DARKGREEN;
  DrawText(TextFormat("Munitions: %d / %d", player.ammo,
                      player.armeEquipee.munitionsMax),
           10, 60, 20, ammoColor);
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
<<<<<<< HEAD
=======
  TypeArme tab[4] = {PISTOLET, FUSIL, SNIPER, GRENADE};
  int i = 0;
  while (player.armeEquipee.type != tab[i]) {
    i += 1;
  }
  DessinerArme(tabArmes[i], GetScreenWidth(), GetScreenHeight());
>>>>>>> master

  DrawText(TextFormat("Point de vie restant: %d", player.health), 10, 190, 20,
           RED);

  minimap(player, bot, blocks);
}