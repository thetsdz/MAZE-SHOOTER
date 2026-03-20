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

//signature github 

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
  bool jeuInitialise = false;
  bool running = true;
  bool chargement = false;

  // --- Variables du jeu (initialisées plus tard) ---
  Entity player;
  Entity bot;
  Entity remotePlayer;
  Block blocks[NUM_BLOCKS][NUM_BLOCKS];
  Projectile projs[MAX_PROJ];
  int score = 0;
  ReseauState netState = {-1, 0, 0};

  // --- Caméra ---
  Camera3D camera = {0};
  camera.up = (Vector3){0, 1, 0};
  camera.fovy = 60;
  camera.projection = CAMERA_PERSPECTIVE;

  // --- Textures ---
  Texture2D tabArmes[4];
  tabArmes[0] = LoadTexture("../assets/images/pistolet_placeholder.png");
  tabArmes[1] = LoadTexture("../assets/images/fusil_assault.png");
  tabArmes[2] = LoadTexture("../assets/images/sniper_placeholder.png");
  tabArmes[3] = LoadTexture("../assets/images/grenade_placeholder.png");
  Texture2D viseur = LoadTexture("../assets/images/crosshair.png");

  Texture2D wallTex = LoadTexture("../assets/images/brick.png");
  Texture2D floorTex = LoadTexture("../assets/images/concrete.png");

  // 1. Charger les modèle bot et projectiles (provient de poly.pizza)
  Model tabModels[4];
  Model botModel = LoadModel("../assets/images/robot/Robot.glb");
  
  tabModels[0]= LoadModel("../assets/models/projectiles/Bullet_pistolet.glb");
  tabModels[1] = LoadModel("../assets/models/projectiles/Bullet_fusil_assault.glb");
  tabModels[2] = LoadModel("../assets/models/projectiles/Bullet_sniper3.glb");
  tabModels[3] = LoadModel("../assets/models/projectiles/Grenade.glb");

  // --- AUTOPSIE DU MODÈLE ---
printf("--- DEBUG SNIPER ---\n");
printf("Nombre de meshes : %d\n", tabModels[2].meshCount);
printf("Nombre de materiaux : %d\n", tabModels[2].materialCount);

if (tabModels[2].meshCount > 0) {
    BoundingBox bb = GetModelBoundingBox(tabModels[2]);
    printf("Taille du modele (Min): X:%.2f, Y:%.2f, Z:%.2f\n", bb.min.x, bb.min.y, bb.min.z);
    printf("Taille du modele (Max): X:%.2f, Y:%.2f, Z:%.2f\n", bb.max.x, bb.max.y, bb.max.z);
}
printf("--------------------\n");
 


  
  // --- Skybox (cross vertical 3x4) ---
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

    // --- Initialisation des objets du jeu (une seule fois) ---
    if ((currentScreen == NOUVELLE_PARTIE && !jeuInitialise) ||
        (currentScreen == CHARGER_PARTIE && !jeuInitialise)) {
      InitPlayer(&player);
      InitBot(&bot, blocks,player.pos);
      init_lab(blocks);
      creer_lab(blocks);
      InitProjectiles(projs);
      score = 0;
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
        UpdateGame(&player, &bot, blocks, projs, &score, &camera);
        if (IsKeyPressed(KEY_BACKSPACE)) {
          currentScreen = MENU;
          jeuInitialise = false;
        }
        break;
      }
      case MULTIJOUEUR: {
        StopAllMusic();
        partie_multijoueur(&player, &remotePlayer, blocks, projs, &camera,
                           &netState, &jeuInitialise, &score, &currentScreen);
        break;
      }
      case CHARGER_PARTIE: {
        if (!chargement) {
          chargerSauvegarde(&player, &bot, &score);
          chargement = true;
          DisableCursor();
        }
        UpdateGame(&player, &bot, blocks, projs, &score, &camera);
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
    ClearBackground(BLANK);

    switch (currentScreen) {
      case MENU: {
        // Le dessin est géré dans GererMenu
        break;
      }
      case NOUVELLE_PARTIE: {
        UpdateDessinGame(&bot, blocks, camera, projs, score, player, viseur,
                         tabArmes, skyModel, wallTex, floorTex, botModel, tabModels);
        break;
      }
      case MULTIJOUEUR: {
        DessinerMultijoueur(&player, &remotePlayer, blocks, projs, &camera,
                            viseur, tabArmes, score, &netState, skyModel,
                            wallTex, floorTex, botModel, tabModels);
        break;
      }
      case CHARGER_PARTIE: {
        UpdateDessinGame(&bot, blocks, camera, projs, score, player, viseur,
                         tabArmes, skyModel, wallTex, floorTex, botModel, tabModels) ;
        break;
      }
      case OPTIONS: {
        // Le dessin est géré dans GererOption
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

  TraceLog(LOG_INFO, "Fin de partie | Score=%d | maxAmmo=%d", score,
           player.ammo);
  CloseLog();
  UnloadTexture(viseur);
  for (int i=0;i<3;i++){
    UnloadTexture(tabArmes[i]);
  }
  UnloadShader(skyModel.materials[0].shader);
  UnloadTexture(skyModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);
  UnloadModel(skyModel);
  UnloadModel(botModel);
  
  UnloadGameAudio();
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
