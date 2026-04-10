/**
 * \file main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/headers/asset.h"
#include "../lib/headers/audio.h"
#include "../lib/headers/bot.h"
#include "../lib/headers/dessin.h"
#include "../lib/headers/endGame.h"
#include "../lib/headers/heal.h"
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
#include "rlgl.h"

// Recharge wallModel et floorModel selon le thème sélectionné dans les options.
// Décharge les anciens modèles/textures avant de charger les nouveaux.
static void RechargerTheme(Model* wallModel, Model* floorModel,
                           Texture2D* wallTex, Texture2D* floorTex) {
  const ThemeInfo* theme = GetSelectedTheme();

  // Décharger les anciens
  UnloadTexture(*wallTex);
  UnloadTexture(*floorTex);
  UnloadModel(*wallModel);
  UnloadModel(*floorModel);

  // Charger les nouveaux selon le thème
  *wallTex = LoadTexture(theme->wallTexPath);
  *floorTex = LoadTexture(theme->floorTexPath);

  Mesh wallMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
  *wallModel = LoadModelFromMesh(wallMesh);
  wallModel->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = *wallTex;

  Mesh floorMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
  *floorModel = LoadModelFromMesh(floorMesh);
  floorModel->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = *floorTex;
}

int main(void) {
  // --- Initialisation du log ---
  if (!InitLog("log.txt")) return 1;
  SetTraceLogCallback(LogToFile);
  SetTraceLogLevel(LOG_INFO);

  // --- Initialisation Fenêtre & Raylib ---
  int screenWidth = GetMonitorWidth(0);
  int screenHeight = GetMonitorHeight(0);
  InitWindow(screenWidth, screenHeight, "JEU");
  InitAudioDevice();
  InitGameAudio();

  ToggleFullscreen();
  SetTargetFPS(60);

  srand(time(NULL));

  // --- Variables pour la gestion des états ---
  GameScreen currentScreen = MENU;
  GameScreen previousScreen = MENU;  // ← pour détecter les transitions
  bool jeuInitialise = false;
  bool running = true;
  bool chargement = false;
  bool IsBossAlive = false;
  bool joueurATriche = false;

  // --- Variables du jeu (initialisées plus tard) ---
  Entity player;
  Entity bot[18];
  Entity remotePlayer;
  Entity boss;
  Heal heal[10];
  Block blocks[NUM_BLOCKS][NUM_BLOCKS];
  Projectile projs[MAX_PROJ];
  ReseauState netState = {-1, 0, 0};
  float timerTriche = 0.0f;  // chronomètre pour l'écran de triche

  // --- Caméra ---
  Camera3D camera = {0};
  camera.up = (Vector3){0, 1, 0};
  camera.fovy = 60;
  camera.projection = CAMERA_PERSPECTIVE;

  // --- Modèles armes, bot, projectiles ---
  Model tabArmes[4];
  tabArmes[0] = LoadModel("../assets/models/armes/Pistolet.glb");
  tabArmes[1] = LoadModel("../assets/models/armes/Fusil_assault.glb");
  tabArmes[2] = LoadModel("../assets/models/armes/Sniper.glb");
  tabArmes[3] = LoadModel("../assets/models/armes/Grenade.glb");

  Texture2D iconesArmes[4];
  iconesArmes[0] =
      LoadTexture("../assets/images/icone_arme/icone_pistolet.png");
  iconesArmes[1] = LoadTexture("../assets/images/icone_arme/icone_sniper.png");
  iconesArmes[2] = LoadTexture("../assets/images/icone_arme/icone_fusil.png");
  iconesArmes[3] = LoadTexture("../assets/images/icone_arme/icone_grenade.png");

  Texture2D viseur = LoadTexture("../assets/images/crosshair.png");

  Model botModel = LoadModel("../assets/models/robots/Robot.glb");
  Model bossModel = LoadModel("../assets/models/boss/boss.glb");

  // --- Modèle heal ---
  Model healModel = LoadModel("../assets/models/coffre/Chest2.glb");

  Model tabProjModels[5];
  tabProjModels[0] =
      LoadModel("../assets/models/projectiles/Bullet_pistolet.glb");
  tabProjModels[1] =
      LoadModel("../assets/models/projectiles/Bullet_fusil_assault.glb");
  tabProjModels[2] =
      LoadModel("../assets/models/projectiles/Bullet_sniper3.glb");
  tabProjModels[3] = LoadModel("../assets/models/projectiles/Grenade.glb");
  tabProjModels[4] = LoadModel("../assets/models/projectiles/Explosion.glb");

  // --- Textures & modèles du niveau (thème par défaut) ---
  Texture2D wallTex = LoadTexture("../assets/images/brick.png");
  Texture2D floorTex = LoadTexture("../assets/images/concrete.png");

  Mesh wallMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
  Model wallModel = LoadModelFromMesh(wallMesh);
  wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wallTex;

  Mesh floorMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
  Model floorModel = LoadModelFromMesh(floorMesh);
  floorModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTex;

  // --- Skybox ---
  Mesh skyMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
  Model skyModel = LoadModelFromMesh(skyMesh);
  skyModel.materials[0].shader =
      LoadShader("../assets/shaders/skybox.vs", "../assets/shaders/skybox.fs");

  Image skyImg = LoadImage("../assets/images/sky.png");
  ImageFormat(&skyImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  TextureCubemap cubemap =
      LoadTextureCubemap(skyImg, CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE);
  UnloadImage(skyImg);

  skyModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubemap;
  int envMapLoc =
      GetShaderLocation(skyModel.materials[0].shader, "environmentMap");
  SetShaderValue(skyModel.materials[0].shader, envMapLoc,
                 (int[]){MATERIAL_MAP_CUBEMAP}, SHADER_UNIFORM_INT);

  int doGammaLoc = GetShaderLocation(skyModel.materials[0].shader, "doGamma");
  int vflippedLoc = GetShaderLocation(skyModel.materials[0].shader, "vflipped");
  int val0 = 0;
  SetShaderValue(skyModel.materials[0].shader, doGammaLoc, &val0,
                 SHADER_UNIFORM_INT);
  SetShaderValue(skyModel.materials[0].shader, vflippedLoc, &val0,
                 SHADER_UNIFORM_INT);

  // --- Boucle Principale ---
  while (!WindowShouldClose() && running) {
    UpdateGameAudio();
    if (IsKeyPressed(KEY_ESCAPE)) break;

    // --- Détection de transition d'écran ---
    if (currentScreen != previousScreen) {
      // On quitte OPTIONS → recharger les textures si le thème a changé
      if (previousScreen == OPTIONS &&
          (currentScreen == MENU || currentScreen == NOUVELLE_PARTIE ||
           currentScreen == CHARGER_PARTIE)) {
        RechargerTheme(&wallModel, &floorModel, &wallTex, &floorTex);
      }

      previousScreen = currentScreen;
    }

    // --- Initialisation des objets du jeu (une seule fois) ---
    if ((currentScreen == NOUVELLE_PARTIE && !jeuInitialise) ||
        (currentScreen == CHARGER_PARTIE && !jeuInitialise)) {
      InitPlayer(&player);
      init_lab(blocks);  // <--- RE-INITIALISE les positions Y et l'état isWall
                         // de TOUS les blocs
      creer_lab(blocks);
      for (int i = 0; i < 18; i++) InitBot(&bot[i], blocks);

      for (int i = 0; i < 10; i++) InitHeal(&heal[i], blocks);

      InitProjectiles(projs);
      jeuInitialise = true;
    }

    // --- Logique selon l'état ---
    switch (currentScreen) {
      case MENU: {
        GererMenu(&currentScreen);
        break;
      }
      case NOUVELLE_PARTIE: {
        StopAllMusic();
        UpdateGame(&player, bot, heal, blocks, projs, &camera, &currentScreen,
                   &boss, &IsBossAlive);
        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
          IsBossAlive = false;
        }
        break;
      }
      case MULTIJOUEUR: {
        StopAllMusic();
        partie_multijoueur(&player, &remotePlayer, heal, blocks, projs, &camera,
                           &netState, &jeuInitialise, &currentScreen);
        break;
      }
      case CHARGER_PARTIE: {
        if (!chargement) {
          int succes =
              chargerSauvegarde(&player, bot, &boss, &IsBossAlive, heal);

          if (succes == 2) {
            // Fichier introuvable ou corrompu ! On retourne au menu direct.
            StopAllMusic();
            UpdateGame(&player, bot, heal, blocks, projs, &camera,
                       &currentScreen, &boss, &IsBossAlive);
            if(IsKeyPressed(KEY_BACKSPACE)) {
              currentScreen = MENU;
              jeuInitialise = false;
              IsBossAlive = false;
              chargement = false;
            }

          } else if (succes == 1) {
            // Triche détectée (modification propre du fichier)
            joueurATriche = true;
            timerTriche = 0.0f;
          }

          chargement = true;
          DisableCursor();
        }

        // Si le joueur a triché, on fait tourner le chronomètre
        if (joueurATriche) {
          timerTriche += GetFrameTime();  // Ajoute le temps écoulé depuis la
                                          // dernière frame

          // Si 5 secondes se sont écoulées
          if (timerTriche >= 5.0f) {
            currentScreen = NOUVELLE_PARTIE;  // On bascule sur l'écran de jeu
            jeuInitialise = false;  // Force la réinitialisation des entités
                                    // pour une partie neuve
            joueurATriche = false;  // Enlève l'alerte
            chargement = false;     // Réinitialise l'état de chargement
            IsBossAlive = false;
            timerTriche = 0.0f;
          }
        }
        // On update le jeu que si la sauvegarde est valide
        else {
          UpdateGame(&player, bot, heal, blocks, projs, &camera, &currentScreen,
                     &boss, &IsBossAlive);
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
          chargement = false;
          IsBossAlive = false;
          joueurATriche = false;  // Réinitialiser le drapeau
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

      case GAME_OVER:
        GererGameOver(&currentScreen, player.score);
        if (currentScreen == MENU) {
          jeuInitialise = false;
          IsBossAlive = false;
          chargement = false;  // Important si vous utilisiez une sauvegarde
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
        }
        break;

      case VICTOIRE:
        GererVictoire(&currentScreen, player.score);
        if (currentScreen == MENU) {
          jeuInitialise = false;
          IsBossAlive = false;
          chargement = false;
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
        }
        break;
    }

    // --- Dessin selon l'état ---
    BeginDrawing();
    ClearBackground(BLANK);

    switch (currentScreen) {
      case MENU: {
        break;
      }
      case NOUVELLE_PARTIE: {
        UpdateDessinGame(bot, heal, blocks, camera, projs, player, viseur,

                         tabArmes, healModel, skyModel, wallModel, floorModel,
                         botModel, tabProjModels, &boss, IsBossAlive, bossModel,
                         iconesArmes);
        break;
      }
      case MULTIJOUEUR: {
        DessinerMultijoueur(&player, &remotePlayer, heal, blocks, projs,
                            &camera, viseur, tabArmes, &netState, healModel,
                            skyModel, wallModel, floorModel, botModel,
                            tabProjModels, iconesArmes);
        break;
      }
      case CHARGER_PARTIE: {
        if (joueurATriche) {
          // Dessiner uniquement l'écran d'alerte si triche
          DrawTricheur(GetScreenWidth());
        } else {
          // Sinon on dessine le jeu normal
          UpdateDessinGame(bot, heal, blocks, camera, projs, player, viseur,
                           tabArmes, healModel, skyModel, wallModel, floorModel,
                           botModel, tabProjModels, &boss, IsBossAlive,
                           bossModel, iconesArmes);
        }
        break;
      }
      case OPTIONS: {
        break;
      }
      case EXIT: {
        running = false;
        break;
      }
      case GAME_OVER:
      case VICTOIRE:
        break;
    }

    EndDrawing();
  }

  if (netState.socket != -1) {
    FermerReseau(netState.socket);
  }

  TraceLog(LOG_INFO, "Fin de partie | Score=%d | maxAmmo=%d", player.score,
           player.ammo);
  CloseLog();

  UnloadTexture(viseur);
  UnloadTexture(wallTex);
  UnloadTexture(floorTex);
  for (int i = 0; i < 4; i++) UnloadModel(tabArmes[i]);
  for (int i = 0; i < 5; i++) UnloadModel(tabProjModels[i]);
  for (int i = 0; i < 4; i++) UnloadTexture(iconesArmes[i]);
  UnloadShader(skyModel.materials[0].shader);
  UnloadTexture(skyModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);
  UnloadModel(skyModel);
  UnloadModel(botModel);
  UnloadModel(bossModel);
  UnloadModel(healModel);
  UnloadModel(wallModel);
  UnloadModel(floorModel);

  UnloadGameAudio();
  CloseAudioDevice();
  CloseWindow();
  return 0;
}