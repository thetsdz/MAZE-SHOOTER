#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/headers/asset.h"
#include "../lib/headers/bot.h"
#include "../lib/headers/dessin.h"
#include "../lib/headers/level.h"
#include "../lib/headers/log.h"
#include "../lib/headers/menu.h"
#include "../lib/headers/multijoueur.h"
#include "../lib/headers/option.h"
#include "../lib/headers/pile.h"
#include "../lib/headers/player.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/reseau.h"
#include "../lib/headers/sauvegarde.h"
#include "../lib/headers/types.h"
#include "../lib/headers/updategame.h"
#include "raylib.h"
#include "raymath.h"

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

  srand(time(NULL));

  // --- Variables pour la gestion des états ---
  GameScreen currentScreen = MENU;
  bool jeuInitialise = false;
  bool running = true;
  bool chargement = false;

  // --- Variables du jeu (initialisées plus tard) ---
  Entity player;
  Entity bot;
  Entity remotePlayer;  // L'HUMAIN ADVERSE (pour le mode Multi)
  Block blocks[NUM_BLOCKS][NUM_BLOCKS];
  Projectile projs[MAX_PROJ];
  int score = 0;
  ReseauState netState = {-1, 0, 0};  // socket=-1, isServer=0, connected=0

  // --- Caméra ---
  Camera3D camera = {0};
  camera.up = (Vector3){0, 1, 0};
  camera.fovy = 60;
  camera.projection = CAMERA_PERSPECTIVE;

  // --- Textures ---
  Texture2D viseur = ChargerTexture("../assets/images/crosshair.png");
  Texture2D armeTex = ChargerTexture("../assets/images/weapon_placeholder.png");

  // --- Boucle Principale ---
  while (!WindowShouldClose() && running) {
    if (IsKeyPressed(KEY_ESCAPE)) break;

    // --- Initialisation des objets du jeu (une seule fois) ---
    if ((currentScreen == NOUVELLE_PARTIE && !jeuInitialise) ||
        (currentScreen == CHARGER_PARTIE && !jeuInitialise)) {
      InitPlayer(&player);
      InitBot(&bot, blocks);
      init_lab(blocks);
      creer_lab(blocks);
      InitProjectiles(projs);
      score = 0;  // Pour l'instant on remet le score à 0 chaque fois qu'on
                  // clique sur
      jeuInitialise = true;
    }

    // --- Logique selon l'état ---
    switch (currentScreen) {
      case MENU: {
        GererMenu(&currentScreen);
        break;
      }
      case NOUVELLE_PARTIE: {
        UpdateGame(&player, &bot, blocks, projs, &score, &camera);
        // Retour au menu
        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
        }
        break;
      }
      case MULTIJOUEUR: {
        partie_multijoueur(&player, &remotePlayer, blocks, projs, &camera, &netState,&jeuInitialise,&score,&currentScreen);
          break;
      }
      case CHARGER_PARTIE: {
        if (!chargement) {
          chargerSauvegarde(&player, &bot, &score);
          chargement = true;
          DisableCursor();
        }

        UpdateGame(&player, &bot, blocks, projs, &score, &camera);
        // Retour au menu
        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
          chargement = false;
        }
        break;
      }
      case OPTIONS: {
        GererOption(&currentScreen);
        break;
      }
      case EXIT: {
        running = false;
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
      case NOUVELLE_PARTIE: {
        UpdateDessinGame(&bot, blocks, camera, projs, score, player, viseur,
                         armeTex);
        break;
      }
      case MULTIJOUEUR: {
        DessinerMultijoueur(&player, &remotePlayer, blocks, projs, &camera,
                           viseur, armeTex, score, &netState);
        break;
      }
      case CHARGER_PARTIE: {
        UpdateDessinGame(&bot, blocks, camera, projs, score, player, viseur,
                         armeTex);
        break;
      }
      case OPTIONS: {
        // Le dessin est geré dans GererOption
        break;
      }
      case EXIT: {
        running = false;
        break;
      }
    }

    EndDrawing();
  }

  if (netState.socket != -1) {
    FermerReseau(netState.socket);
  }

  TraceLog(LOG_INFO, "Fin de partie | Score=%d | AmmoMax=%d", score,
           player.maxAmmo);
  CloseLog();
  UnloadTexture(viseur);
  UnloadTexture(armeTex);
  CloseWindow();
  return 0;
}
