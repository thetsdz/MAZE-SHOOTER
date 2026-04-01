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

/* ------------------------------------------------------------------ */
/*  Palette                                                             */
/* ------------------------------------------------------------------ */
#define COL_PANEL_BG (Color){0, 0, 0, 160}
#define COL_PANEL_BORD (Color){255, 255, 255, 40}
#define COL_LABEL (Color){170, 170, 170, 255}
#define COL_BAR_BG (Color){51, 51, 51, 255}
#define COL_HP_HI (Color){97, 196, 89, 255}
#define COL_HP_MID (Color){226, 168, 74, 255}
#define COL_HP_LO (Color){226, 75, 74, 255}
#define COL_AMMO (Color){250, 199, 117, 255}
#define COL_AMMO_EMPTY (Color){226, 75, 74, 255}
#define COL_SCORE (Color){151, 196, 89, 255}
#define COL_FPS (Color){93, 202, 165, 255}
#define COL_HINT (Color){160, 160, 160, 255}
#define COL_HINT_GOLD (Color){250, 199, 117, 255}
#define COL_HINT_RED (Color){200, 80, 80, 255}
#define COL_WHITE_DIM (Color){255, 255, 255, 80}

void DrawProjectiles(Projectile* projs, Model tabProjModels[]) {
  for (int i = 0; i < MAX_PROJ; i++) {
    if (!projs[i].active) continue;

    // Reset de la matrice du modèle spécifique avant de travailler dessus
    tabProjModels[projs[i].type].transform = MatrixIdentity();

    switch (projs[i].type) {
      case PROJ_PISTOLET: {
        float s = 0.25f;
        // On combine le fait de le coucher (90°) AVEC ton inclinaison (Pitch)
        // sur l'axe X (Mets un "+" ou un "-" devant le pitch selon si la balle
        // monte ou descend)
        Matrix rot = MatrixRotateX((90.0f - projs[i].pitch) * DEG2RAD);
        rot = MatrixMultiply(rot, MatrixRotateY(projs[i].yaw * DEG2RAD));

        tabProjModels[PROJ_PISTOLET].transform = rot;
        DrawModel(tabProjModels[PROJ_PISTOLET], projs[i].pos, s, WHITE);
        break;
      }
      case PROJ_FUSIL: {
        float s = 0.25f;  // Taille du fusil
        // On fait EXACTEMENT comme le pistolet : on combine le 90° et le pitch
        // sur l'axe X !
        Matrix rot = MatrixRotateX((90.0f - projs[i].pitch) * DEG2RAD);
        rot = MatrixMultiply(rot, MatrixRotateY(projs[i].yaw * DEG2RAD));

        tabProjModels[PROJ_FUSIL].transform = rot;
        DrawModel(tabProjModels[PROJ_FUSIL], projs[i].pos, s, WHITE);
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

        tabProjModels[PROJ_SNIPER].transform = rot;
        DrawModel(tabProjModels[PROJ_SNIPER], projs[i].pos, s, WHITE);
        break;
      }
      case PROJ_GRENADE: {
        // on affiche deux models en fonction de si la grenade a explosé ou non
        if (projs[i].radius == 3.0f) {
          float s = 17.0f;
          DrawModel(tabProjModels[4], projs[i].pos, s, WHITE);
          break;
        } else {
          float s = 0.2f;
          // Pas de rotation complexe nécessaire pour la grenade
          DrawModel(tabProjModels[PROJ_GRENADE], projs[i].pos, s, WHITE);
          break;
        }
      }
    }
    // pour  tester taille balle mettre le switch en commentaire et prendre
    // DrawSphere DrawSphere(projs[i].pos, projs[i].radius, projs[i].color);
  }
}

/* ------------------------------------------------------------------ */
/*  Utilitaires                                                         */
/* ------------------------------------------------------------------ */

static void DrawPanel(int x, int y, int w, int h) {
  DrawRectangle(x, y, w, h, COL_PANEL_BG);
  DrawRectangleLines(x, y, w, h, COL_PANEL_BORD);
}

static void DrawBar(int x, int y, int w, int h, float ratio, Color fg) {
  if (ratio < 0.0f) ratio = 0.0f;
  if (ratio > 1.0f) ratio = 1.0f;
  DrawRectangle(x, y, w, h, COL_BAR_BG);
  DrawRectangle(x, y, (int)(w * ratio), h, fg);
  DrawRectangleLines(x, y, w, h, (Color){255, 255, 255, 20});
}

/* ------------------------------------------------------------------ */
/*  Minimap                                                             */
/* ------------------------------------------------------------------ */

void minimap(Entity player, Entity bot[18],
             Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
  int minimapX = GetScreenWidth() - MINIMAP_W - MINIMAP_PADDING;
  int minimapY = MINIMAP_PADDING;

  float blockSize = blocks[0][0].width;
  float mapTotalSize = NUM_BLOCKS * blockSize;
  float scaleX = (float)MINIMAP_W / mapTotalSize;
  float scaleY = (float)MINIMAP_H / mapTotalSize;

  float originX = blocks[0][0].pos.x - blocks[0][0].width / 2.0f;
  float originZ = blocks[0][0].pos.z - blocks[0][0].depth / 2.0f;

  /* En-tête */
  int labelW = MeasureText("CARTE", 10);
  DrawText("CARTE", minimapX + MINIMAP_W / 2 - labelW / 2, minimapY - 14, 10,
           COL_LABEL);

  /* Fond + bordure */
  DrawRectangle(minimapX, minimapY, MINIMAP_W, MINIMAP_H,
                (Color){0, 0, 0, 180});
  DrawRectangleLines(minimapX, minimapY, MINIMAP_W, MINIMAP_H, COL_WHITE_DIM);

  /* Murs */
  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];
      if (!b.isWall) continue;

      int dotW = (int)(b.width * scaleX);
      if (dotW < 1) dotW = 1;
      int dotH = (int)(b.depth * scaleY);
      if (dotH < 1) dotH = 1;
      int dotX = minimapX + (int)((b.pos.x - originX) * scaleX) - dotW / 2;
      int dotY = minimapY + (int)((b.pos.z - originZ) * scaleY) - dotH / 2;

      DrawRectangle(dotX, dotY, dotW, dotH, b.color);
    }
  }

  /* Bot (rouge) */
  for (int b = 0; b < 18; b++) {
    int botDotX = minimapX + (int)((bot[b].pos.x - originX) * scaleX);
    int botDotY = minimapY + (int)((bot[b].pos.z - originZ) * scaleY);
    DrawRectangle(botDotX - 3, botDotY - 3, 6, 6, RED);
  }

  /* Joueur (vert, par-dessus) */
  int playerDotX = minimapX + (int)((player.pos.x - originX) * scaleX);
  int playerDotY = minimapY + (int)((player.pos.z - originZ) * scaleY);
  DrawRectangle(playerDotX - 3, playerDotY - 3, 6, 6, GREEN);
}

/* ------------------------------------------------------------------ */
/*  HUD                                                                 */
/* ------------------------------------------------------------------ */

static void DrawHUD(Entity player) {
  const int PX = 10;
  const int PW = 220;
  const int PAD = 8;
  int py = 10;

  /* ---- Panneau VIE -------------------------------------------- */
  DrawPanel(PX, py, PW, 58);

  DrawText("VIE", PX + PAD, py + 6, 10, COL_LABEL);

  float hpRatio = (float)player.health / (float)player.maxHealth;
  Color hpColor = (hpRatio > 0.5f)    ? COL_HP_HI
                  : (hpRatio > 0.25f) ? COL_HP_MID
                                      : COL_HP_LO;
  DrawBar(PX + PAD, py + 22, PW - PAD * 2, 10, hpRatio, hpColor);
  DrawText(TextFormat("%d / %d", player.health, player.maxHealth), PX + PAD,
           py + 38, 10, hpColor);

  py += 66;

  /* ---- Panneau ARME + MUNITIONS -------------------------------- */
  DrawPanel(PX, py, PW, 68);

  /* Nom arme */
  DrawText(player.armeEquipee.nom, PX + PAD, py + 6, 14, COL_AMMO);

  /* Badge munitions (cadre à droite) */
  Color ammoCol = (player.ammo == 0) ? COL_AMMO_EMPTY : COL_AMMO;
  const char* ammoBadge =
      TextFormat("%d / %d", player.ammo, player.armeEquipee.munitionsMax);
  int badgeW = MeasureText(ammoBadge, 12);
  int bx = PX + PW - PAD - badgeW - 10;
  DrawRectangle(bx, py + 7, badgeW + 10, 18, (Color){42, 42, 42, 255});
  DrawRectangleLines(bx, py + 7, badgeW + 10, 18, ammoCol);
  DrawText(ammoBadge, bx + 5, py + 10, 12, ammoCol);

  /* Barre munitions */
  float ammoRatio = (float)player.ammo / (float)player.armeEquipee.munitionsMax;
  DrawBar(PX + PAD, py + 32, PW - PAD * 2, 8, ammoRatio, ammoCol);

  /* Hints inline */
  if (player.ammo < player.armeEquipee.munitionsMax)
    DrawText("[R] Recharger", PX + PAD, py + 48, 10, COL_HINT);

  py += 76;

  /* ---- Panneau SCORE / FPS / VIES ------------------------------ */
  DrawPanel(PX, py, PW, 48);

  DrawText("SCORE", PX + PAD, py + 6, 10, COL_LABEL);
  DrawText(TextFormat("%d", player.score), PX + PAD, py + 20, 16, COL_SCORE);

  DrawText("FPS", PX + 95, py + 6, 10, COL_LABEL);
  DrawText(TextFormat("%d", GetFPS()), PX + 95, py + 20, 16, COL_FPS);

  DrawText("VIES", PX + 160, py + 6, 10, COL_LABEL);
  for (int v = 0; v < 3; v++) {
    Color vc = (v < player.life) ? COL_HP_LO : COL_BAR_BG;
    DrawCircle(PX + 163 + v * 16, py + 33, 5, vc);
  }

  py += 56;

  /* ---- Hint capacité ------------------------------------------ */
  if (player.armeEquipee.munitionsMax >= MAX_PROJ)
    DrawText("Capacite MAX atteinte", PX, py, 10, COL_HINT_RED);
  /* ---- Panneau POSITION ---------------------------------------- */
  DrawPanel(PX, py, PW, 24);
  DrawText(TextFormat("POS: X: %.1f | Y: %.1f | Z: %.1f", player.pos.x,
                      player.pos.y, player.pos.z),
           PX + PAD, py + 7, 10, COL_HINT_GOLD);
}

/* ------------------------------------------------------------------ */
/*  Point d'entrée principal                                            */
/* ------------------------------------------------------------------ */

void UpdateDessinGame(Entity bot[18], Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                      Camera3D camera, Projectile projs[MAX_PROJ],
                      Entity player, Texture2D viseur, Model tabArmes[],
                      Model skyModel, Model wallModel, Model floorModel,
                      Model botModel, Model tabProjModels[]) {
  /* --- Rendu 3D --- */
  BeginMode3D(camera);

  rlDisableBackfaceCulling();
  rlDisableDepthMask();
  DrawModel(skyModel, camera.position, 1.0f, WHITE);
  rlEnableBackfaceCulling();
  rlEnableDepthMask();

  DrawLevel(blocks, wallModel, floorModel);

  for (int b = 0; b < 18; b++) {
    Vector3 drawPos = {bot[b].pos.x,bot[b].pos.y,bot[b].pos.z};
    Matrix transform = MatrixIdentity();
    transform =
        MatrixMultiply(transform, MatrixRotateX(bot[b].pitch * DEG2RAD));
    float angleFinal = (bot[b].yaw * RAD2DEG) - 90.0f;
    transform = MatrixMultiply(transform, MatrixRotateY(angleFinal * DEG2RAD));
    botModel.transform = transform;
    DrawModel(botModel, drawPos, 0.3f, WHITE);
  }

  DrawProjectiles(projs, tabProjModels);
  EndMode3D();

  /* --- UI 2D --- */
  DrawHUD(player);

  DessinerViseur(viseur, GetScreenWidth(), GetScreenHeight());

  TypeArme tab[4] = {PISTOLET, FUSIL, SNIPER, GRENADE};
  int i = 0;
  while (player.armeEquipee.type != tab[i]) i++;
  DessinerArme(tabArmes[i], i);
  minimap(player, bot, blocks);
}