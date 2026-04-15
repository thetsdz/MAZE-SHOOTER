/**
 * \file main.c
 * \brief Point d'entrée principal du jeu MAZE-SHOOTER
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Modules internes du jeu ---
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
#include "../lib/headers/texture.h"
#include "../lib/headers/types.h"
#include "../lib/headers/updategame.h"

// --- Librairies Raylib ---
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

int main(void) {

    // =========================================================
    // INITIALISATION DU SYSTÈME DE LOG
    // Permet d'enregistrer les événements du jeu dans un fichier
    // =========================================================
    if (!InitLog("log.txt"))
        return 1;
    SetTraceLogCallback(LogToFile);
    SetTraceLogLevel(LOG_INFO);

    // =========================================================
    // INITIALISATION DE LA FENÊTRE ET DE L'AUDIO
    // Configuration Raylib + passage en plein écran
    // =========================================================
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "MAZE-SHOOTER");
    InitAudioDevice();
    InitGameAudio();
    ToggleFullscreen();
    SetTargetFPS(60);

    // Initialisation du générateur aléatoire
    srand(time(NULL));

    // =========================================================
    // CHARGEMENT DES RESSOURCES GRAPHIQUES
    // (modèles 3D, textures, UI, etc.)
    // =========================================================
    GameAssets assets;
    LoadGameAssets(&assets);

    // =========================================================
    // GESTION DES ÉTATS DU JEU (machine à états)
    // Permet de naviguer entre menus, jeu, options, etc.
    // =========================================================
    GameScreen currentScreen = MENU;
    GameScreen previousScreen = MENU;

    bool jeuInitialise = false; // évite de réinitialiser le jeu plusieurs fois
    bool running = true;        // contrôle de la boucle principale
    bool chargement = false;    // état de chargement d'une sauvegarde
    bool IsBossAlive = false;   // état du boss
    bool joueurATriche = false; // détection de triche

    // =========================================================
    // VARIABLES PRINCIPALES DU JEU
    // (joueur, ennemis, objets, réseau...)
    // =========================================================
    Entity player = {0};
    Entity bot[18];
    Entity remotePlayer;
    Entity boss;

    Heal heal[10];
    Block blocks[NUM_BLOCKS][NUM_BLOCKS];
    Projectile projs[MAX_PROJ];

    ReseauState netState = {-1, 0, 0};

    float timerTriche = 0.0f;

    // =========================================================
    // CONFIGURATION DE LA CAMÉRA 3D
    // =========================================================
    Camera3D camera = {0};
    camera.up = (Vector3){0, 1, 0};
    camera.fovy = 60;
    camera.projection = CAMERA_PERSPECTIVE;

    // =========================================================
    // BOUCLE PRINCIPALE DU JEU
    // Gère l'entrée utilisateur, la logique et le rendu
    // =========================================================
    while (!WindowShouldClose() && running) {

        UpdateGameAudio();

        // Permet de quitter rapidement avec Échap
        if (IsKeyPressed(KEY_ESCAPE))
            break;

        // =====================================================
        // GESTION DES TRANSITIONS D'ÉCRAN
        // Exemple : recharger les textures si thème modifié
        // =====================================================
        if (currentScreen != previousScreen) {
            if (previousScreen == OPTIONS &&
                (currentScreen == MENU || currentScreen == NOUVELLE_PARTIE ||
                 currentScreen == CHARGER_PARTIE)) {
                RechargerTheme(&assets);
            }
            previousScreen = currentScreen;
        }

        // =====================================================
        // INITIALISATION DU JEU (UNE SEULE FOIS)
        // Création du joueur, du labyrinthe, des ennemis, etc.
        // =====================================================
        if ((currentScreen == NOUVELLE_PARTIE && !jeuInitialise) ||
            (currentScreen == CHARGER_PARTIE && !jeuInitialise)) {

            InitPlayer(&player);

            init_lab(blocks);
            creer_lab(blocks);

            for (int i = 0; i < 18; i++)
                InitBot(&bot[i], blocks);

            for (int i = 0; i < 10; i++)
                InitHeal(&heal[i], blocks);

            InitProjectiles(projs);

            jeuInitialise = true;
        }

        // =====================================================
        // DÉBUT DU RENDU GRAPHIQUE
        // =====================================================
        BeginDrawing();
        ClearBackground(BLANK);

        // =====================================================
        // MACHINE À ÉTATS PRINCIPALE
        // Chaque écran a sa propre logique et son rendu
        // =====================================================
        switch (currentScreen) {

        // ---------------- MENU PRINCIPAL ----------------
        case MENU:
            GererMenu(&currentScreen);
            break;

        // ---------------- PARTIE SOLO ----------------
        case NOUVELLE_PARTIE:
            StopAllMusic();

            // Mise à jour de la logique du jeu
            UpdateGame(&player, bot, heal, blocks, projs, &camera,
                       &currentScreen, &boss, &IsBossAlive);

            // Retour au menu
            if (IsKeyPressed(KEY_BACKSPACE)) {
                currentScreen = MENU;
                jeuInitialise = false;
                IsBossAlive = false;
            }

            // Rendu du jeu
            UpdateDessinGame(bot, heal, blocks, camera, projs, player,
                             assets.viseur, assets.tabArmes, assets.healModel,
                             assets.skyModel, assets.wallModel,
                             assets.floorModel, assets.botModel,
                             assets.tabProjModels, &boss, IsBossAlive,
                             assets.bossModel, assets.iconesArmes);
            break;

        // ---------------- MULTIJOUEUR ----------------
        case MULTIJOUEUR:
            StopAllMusic();

            // Gestion réseau + logique multijoueur
            partie_multijoueur(&player, &remotePlayer, heal, blocks, projs,
                               &camera, &netState, &jeuInitialise,
                               &currentScreen);

            // Rendu multijoueur
            DessinerMultijoueur(
    &player, &remotePlayer, heal, blocks, projs, &camera,
    assets.viseur, assets.tabArmes, assets.healModel,
    assets.skyModel, assets.wallModel, assets.floorModel,
    assets.botModel, assets.tabProjModels, assets.iconesArmes);
            break;

        // ---------------- CHARGEMENT DE PARTIE ----------------
        case CHARGER_PARTIE:

            // Chargement effectué une seule fois
            if (!chargement) {
                int succes =
                    chargerSauvegarde(&player, bot, &boss, &IsBossAlive, heal);

                if (succes == 2) {
                    // Sauvegarde invalide → nouvelle partie
                    StopAllMusic();
                    UpdateGame(&player, bot, heal, blocks, projs, &camera,
                               &currentScreen, &boss, &IsBossAlive);
                } else if (succes == 1) {
                    // Triche détectée
                    joueurATriche = true;
                    timerTriche = 0.0f;
                }

                chargement = true;
                DisableCursor();
            }

            // Gestion du cas triche
            if (joueurATriche) {
                timerTriche += GetFrameTime();

                if (timerTriche >= 5.0f) {
                    // Réinitialisation après sanction
                    currentScreen = NOUVELLE_PARTIE;
                    jeuInitialise = false;
                    joueurATriche = false;
                    chargement = false;
                    IsBossAlive = false;
                    timerTriche = 0.0f;
                }

                DrawTricheur(GetScreenWidth());
            } else {
                // Jeu normal
                UpdateGame(&player, bot, heal, blocks, projs, &camera,
                           &currentScreen, &boss, &IsBossAlive);

                UpdateDessinGame(
                    bot, heal, blocks, camera, projs, player, assets.viseur,
                    assets.tabArmes, assets.healModel, assets.skyModel,
                    assets.wallModel, assets.floorModel, assets.botModel,
                    assets.tabProjModels, &boss, IsBossAlive, assets.bossModel,
                    assets.iconesArmes);
            }

            // Retour menu
            if (IsKeyPressed(KEY_BACKSPACE)) {
                currentScreen = MENU;
                jeuInitialise = false;
                chargement = false;
                IsBossAlive = false;
                joueurATriche = false;
            }
            break;

        // ---------------- OPTIONS ----------------
        case OPTIONS:
            GererOption(&currentScreen);
            break;

        // ---------------- QUITTER ----------------
        case EXIT:
            running = false;
            break;

        // ---------------- GAME OVER ----------------
        case GAME_OVER:
            GererGameOver(&currentScreen, player.score);
            if (currentScreen == MENU) {
                jeuInitialise = false;
                IsBossAlive = false;
                chargement = false;
            }
            break;

        // ---------------- VICTOIRE ----------------
        case VICTOIRE:
            GererVictoire(&currentScreen, player.score);
            if (currentScreen == MENU) {
                jeuInitialise = false;
                IsBossAlive = false;
                chargement = false;
            }
            break;
        }

        EndDrawing();
    }

    // =========================================================
    // NETTOYAGE FINAL
    // Libération mémoire, fermeture réseau/audio/fenêtre
    // =========================================================
    if (netState.socket != -1)
        FermerReseau(netState.socket);

    TraceLog(LOG_INFO, "Fin de partie | Score=%d | maxAmmo=%d", player.score,
             player.ammo);

    CloseLog();
    UnloadGameAssets(&assets);
    UnloadGameAudio();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}