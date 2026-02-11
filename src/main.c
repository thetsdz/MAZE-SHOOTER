#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/headers/sauvegarde.h"
#include "../lib/headers/asset.h"
#include "../lib/headers/bot.h"
#include "../lib/headers/level.h"
#include "../lib/headers/pile.h"
#include "../lib/headers/player.h"
#include "../lib/headers/menu.h"
#include "../lib/headers/option.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/types.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"


static FILE* logFile = NULL;

void LogToFile(int logLevel, const char* text, va_list args) {
    if (!logFile) return;
    const char* levels[] = {"ALL", "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL", "NONE"};
    fprintf(logFile, "[%s] ", levels[logLevel]);
    vfprintf(logFile, text, args);
    fprintf(logFile, "\n");
    fflush(logFile);
}

int main(void) {
    // --- Initialisation du log ---
    logFile = fopen("log.txt", "w");
    if (!logFile) return 1;
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
    while (!WindowShouldClose() && etat == 1 ) {
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
                // --- Logique du jeu ---
                UpdatePlayer(&player, blocks, &camera);
                UpdateBot(&bot, blocks, player.pos, projs);

                if (IsKeyPressed(KEY_Y)) sauvegarder(&player, &score);
                if (IsKeyPressed(KEY_U)) chargerSauvegarde(&player, &score);

                if (IsKeyPressed(KEY_R)) player.ammo = player.maxAmmo;

                if (IsKeyPressed(KEY_E) && score >= SCORE_TRADE && player.maxAmmo < MAX_PROJ) {
                    score -= SCORE_TRADE;
                    player.maxAmmo += 2;
                    TraceLog(LOG_INFO, "Achat amélioration : nouvelle capacité max = %d", player.maxAmmo);
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player.ammo > 0) {
                    Vector3 camDir = {sinf(player.yaw) * cosf(player.pitch), sinf(player.pitch), cosf(player.yaw) * cosf(player.pitch)};
                    Vector3 startPos = {player.pos.x, player.pos.y + 0.5f, player.pos.z};
                    ShootProjectile(projs, startPos, camDir, OWNER_PLAYER);
                    player.ammo--;
                }

                UpdateProjectiles(projs, blocks, &bot, &player, &score);

                // Retour au menu
                if (IsKeyPressed( KEY_BACKSPACE )) {
                    currentScreen = MENU;
                    jeuInitialise = false;
                }
                break;
            }
            case OPTIONS: {
                GererOption(&currentScreen);
                break;
            }
            case EXIT:{
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
                // --- Dessin 3D ---
                BeginMode3D(camera);
                DrawLevel(blocks);
                DrawCube(bot.pos, bot.size, bot.size, bot.size, RED);
                Vector3 lookDir = {sinf(bot.yaw), 0, cosf(bot.yaw)};
                Vector3 eyePos = Vector3Add(bot.pos, Vector3Scale(lookDir, 0.5f));
                eyePos.y += 0.3f;
                DrawCube(eyePos, 0.2f, 0.2f, 0.2f, BLACK);
                DrawProjectiles(projs);
                EndMode3D();

                // --- UI 2D ---
                DrawText(TextFormat("Score: %d | FPS: %d", score, GetFPS()), 10, 10, 20, DARKGRAY);
                Color ammoColor = (player.ammo == 0) ? RED : DARKGREEN;
                DrawText(TextFormat("Munitions: %d / %d", player.ammo, player.maxAmmo), 10, 40, 20, ammoColor);
                if (player.ammo < player.maxAmmo) DrawText("Appuyez sur [R] pour Recharger", 10, 65, 10, GRAY);
                if (player.maxAmmo < MAX_PROJ) {
                    if (score >= SCORE_TRADE) {
                        DrawText("Appuyez sur [E] pour +2 Munitions Max (-100 pts)", 10, 90, 20, GOLD);
                    } else {
                        DrawText(TextFormat("Prochaine amélioration: 100 pts (Actuel: %d)", score), 10, 90, 10, LIGHTGRAY);
                    }
                } else {
                    DrawText("Capacité MAX atteinte (50)", 10, 90, 20, MAROON);
                }
                DessinerViseur(viseur, GetScreenWidth(), GetScreenHeight());
                DessinerArme(armeTex, GetScreenWidth(), GetScreenHeight());
                break;
            }
            case OPTIONS: {
                // Le dessin est geré dans GererOption
                break;
            }
            case EXIT:{
                break;
            }
        }

        EndDrawing();
    }

    TraceLog(LOG_INFO, "Fin de partie | Score=%d | AmmoMax=%d", score, player.maxAmmo);
    if (logFile) fclose(logFile);
    UnloadTexture(viseur);
    UnloadTexture(armeTex);
    CloseWindow();
    return 0;
}
