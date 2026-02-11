#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/headers/asset.h"
#include "../lib/headers/bot.h"
#include "../lib/headers/dessin.h"
#include "../lib/headers/level.h"
#include "../lib/headers/log.h"
#include "../lib/headers/menu.h"
#include "../lib/headers/pile.h"
#include "../lib/headers/player.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/sauvegarde.h"
#include "../lib/headers/types.h"
#include "../lib/headers/updategame.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"

int main(void) {
  // --- Initialisation du log ---
  if (!InitLog("log.txt")) return 1;
  SetTraceLogCallback(LogToFile);
  SetTraceLogLevel(LOG_INFO);

  // --- Initialisation Fenêtre & Raylib ---
  int screenWidth = GetMonitorWidth(0);
  int screenHeight = GetMonitorHeight(0);
  InitWindow(screenWidth, screenHeight, "JEU");
  ToggleFullscreen();
  SetTargetFPS(60);
  void ShowCursor(void);

  srand(time(NULL));
  int etat = 1;

  // --- Variables pour la gestion des états ---
  GameScreen currentScreen = MENU;
  bool jeuInitialise = false;

  // --- Variables du jeu (initialisées plus tard) ---
  Entity player;
  Entity bot;
  Block blocks[NUM_BLOCKS][NUM_BLOCKS];
  Projectile projs[MAX_PROJ];
  int score = 0;

  // --- Caméra ---
  Camera3D camera = {0};
  camera.up = (Vector3){0, 1, 0};
  camera.fovy = 60;
  camera.projection = CAMERA_PERSPECTIVE;

  // --- Textures ---
  Texture2D viseur = ChargerTexture("../assets/images/crosshair.png");
  Texture2D armeTex = ChargerTexture("../assets/images/weapon_placeholder.png");

  // --- Boucle Principale ---
  while (!WindowShouldClose() && etat == 1) {
    if (IsKeyPressed(KEY_ESCAPE)) break;

    // --- Initialisation des objets du jeu (une seule fois) ---
    if (currentScreen == GAME && !jeuInitialise) {
      InitPlayer(&player);
      InitBot(&bot);
      init_lab(blocks);
      creer_lab(blocks);
      InitProjectiles(projs);
      jeuInitialise = true;
    }

    // --- Logique selon l'état ---
    switch (currentScreen) {
      case MENU: {
        GererMenu(&currentScreen);
        break;
      }
      case GAME: {
        UpdateGame(&player, &bot, blocks, projs, &score, &camera);
        // Retour au menu
        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
        }
        break;
      }
      case OPTIONS: {
        GererOption(&currentScreen);
        break;
      }
      case EXIT: {
        etat = 0;
        break;
      }
    }

    // --- Dessin selon l'état ---
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentScreen) {
      case MENU: {
        // Le dessin est géré dans GererMenu
        break;
      }
      case GAME: {
        UpdateDessinGame(&bot, blocks, camera, projs, score, player, viseur,
                         armeTex);
        break;
      }
      case OPTIONS: {
        // Le dessin est geré dans GererOption
        break;
      }
      case EXIT: {
        break;
      }
    }

    EndDrawing();
  }

  TraceLog(LOG_INFO, "Fin de partie | Score=%d | AmmoMax=%d", score,
           player.maxAmmo);
  CloseLog();
  UnloadTexture(viseur);
  UnloadTexture(armeTex);
  CloseWindow();
  return 0;
}
