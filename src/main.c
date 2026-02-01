// Inclusion libairies
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>


// Inclusion Raylib
#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"

// Inclusion de nos propres modules
#include "../lib/headers/types.h"
#include "../lib/headers/level.h"
#include "../lib/headers/player.h"
#include "../lib/headers/bot.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/asset.h"
#include "../lib/headers/pile.h"

static FILE *logFile = NULL;

void LogToFile(int logLevel, const char *text, va_list args){
    if (!logFile) return;

    // Préfixe lisible
    const char *levels[] = {
        "ALL", "TRACE", "DEBUG", "INFO",
        "WARNING", "ERROR", "FATAL", "NONE"
    };

    fprintf(logFile, "[%s] ", levels[logLevel]);
    vfprintf(logFile, text, args);
    fprintf(logFile, "\n");
    fflush(logFile);
}


int main(void){

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
    SetTargetFPS(60);   // Essaye de maintenir 60 images/seconde
    DisableCursor();    // Bloque la souris dans la fenêtre pour la visée
    srand(time(NULL));  // Initialise le générateur aléatoire

    // --- Initialisation des Objets ---
    Player player;
    InitPlayer(&player);

    Bot bot;
    InitBot(&bot);

    Block blocks[NUM_BLOCKS][NUM_BLOCKS];
    init_lab(blocks);
    creer_lab(blocks);


    Projectile projs[MAX_PROJ];
    InitProjectiles(projs);

    int score = 0;

    // Setup Caméra Raylib standard
    Camera3D camera = {0};
    camera.up = (Vector3){0,1,0}; // L'axe Y pointe vers le haut
    camera.fovy = 60;              // Champ de vision (Field of View)
    camera.projection = CAMERA_PERSPECTIVE;

    // Initialisation des textures
    Texture2D viseur = ChargerTexture("../assets/images/crosshair.png");
    Texture2D armeTex = ChargerTexture("../assets/images/weapon_placeholder.png");

    // Fichier de log
    // --- Boucle Principale ---
    while(!WindowShouldClose()){
        
        if(IsKeyPressed(KEY_ESCAPE)) break;

        // --- ETAPE UPDATE ---
        
        UpdatePlayer(&player, blocks, &camera);
        
        // Update du bot (sans affichage pour maintenant)
        UpdateBot(&bot, blocks,player.pos, projs);

        // enregistrer les informations joueur dans le fichier de sauvegarde
        if(IsKeyPressed(KEY_Y)){
            FILE *fw = fopen("save.txt", "w");
            if(fw){
                fprintf(fw, "%d\n", score);
                fprintf(fw, "%d\n", player.maxAmmo);
                fclose(fw);
                
            }
        }

        // charger les informations joueur depuis le fichier de sauvegarde
        if (IsKeyPressed(KEY_U)) {
        FILE *fr = fopen("save.txt", "r");
        if (fr) {
            if (fscanf(fr, "%d", &score) != 1) {
                TraceLog(LOG_WARNING, "Erreur lecture score dans save.txt");
            }
            if (fscanf(fr, "%d", &player.maxAmmo) != 1) {
                TraceLog(LOG_WARNING, "Erreur lecture ammo max dans save.txt");
            }
            fclose(fr);
        } else {
            TraceLog(LOG_ERROR, "Impossible d'ouvrir save.txt en lecture");
        }
        }

        // --- GESTION MUNITIONS & AMELIORATIONS ---

        if(IsKeyPressed(KEY_R)){
            player.ammo = player.maxAmmo;
        }


        // Acheter Amélioration (Touche E)
        // Condition : Avoir assez de score ET ne pas dépasser 50 de capacité max
        if(IsKeyPressed(KEY_E) && score >= SCORE_TRADE && player.maxAmmo < MAX_PROJ){
            score -= SCORE_TRADE;       // Coût
            player.maxAmmo += 2; // Bonus
            TraceLog(LOG_INFO, "Achat amélioration : nouvelle capacité max = %d", player.maxAmmo);
        }

        // --- Tir du joueur ---
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            if(player.ammo > 0) {
                // 1. Calcul direction tir (Où regarde le joueur ?)
                Vector3 camDir = { 
                    sinf(player.yaw)*cosf(player.pitch), 
                    sinf(player.pitch), 
                    cosf(player.yaw)*cosf(player.pitch) 
                };
                
                // 2. Position de départ (Yeux du joueur)
                Vector3 startPos = {player.pos.x, player.pos.y + 0.5f, player.pos.z};
                ShootProjectile(projs, startPos, camDir, OWNER_PLAYER);
                
                player.ammo--; 
            }
        }

        UpdateProjectiles(projs, blocks, &bot, &player, &score);

        // --- ETAPE DRAW ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
            DrawLevel(blocks);
            DrawCube(bot.pos, bot.size, bot.size, bot.size, RED);  
            // Petits yeux noirs pour voir où il regarde
            Vector3 lookDir = { sinf(bot.yaw), 0, cosf(bot.yaw) };
            Vector3 eyePos = Vector3Add(bot.pos, Vector3Scale(lookDir, 0.5f));
            eyePos.y += 0.3f;
            DrawCube(eyePos, 0.2f, 0.2f, 0.2f, BLACK);

            DrawProjectiles(projs);
        EndMode3D();
	// --- UI 2D (Après la 3D) ---
      
        // --- INTERFACE UTILISATEUR (UI) MISE A JOUR ---
 	       
        // Affichage Score et FPS
        DrawText(TextFormat("Score: %d | FPS: %d", score, GetFPS()), 10, 10, 20, DARKGRAY);
        
        // Affichage Munitions (Rouge si vide, Vert sinon)
        Color ammoColor = (player.ammo == 0) ? RED : DARKGREEN;
        DrawText(TextFormat("Munitions: %d / %d", player.ammo, player.maxAmmo), 10, 40, 20, ammoColor);
        
        // Affichage instruction Recharge
        if(player.ammo < player.maxAmmo) {
            DrawText("Appuyez sur [R] pour Recharger", 10, 65, 10, GRAY);
        }

        // Affichage Magasin (Upgrade)
        if(player.maxAmmo < MAX_PROJ) {
            if(score >= SCORE_TRADE) {
                // Le joueur peut acheter : Texte en Or/Orange
                DrawText("Appuyez sur [E] pour +2 Munitions Max (-100 pts)", 10, 90, 20, GOLD);
            } else {
                // Pas assez de points : Texte gris
                DrawText(TextFormat("Prochaine amélioration: 100 pts (Actuel: %d)", score), 10, 90, 10, LIGHTGRAY);
            }
        } else {
            DrawText("Capacité MAX atteinte (50)", 10, 90, 20, MAROON);
        }
	//Dessin du viseur et de l'arme
    DessinerViseur(viseur, GetScreenWidth(), GetScreenHeight());  
	DessinerArme(armeTex, GetScreenWidth(), GetScreenHeight());

        EndDrawing();
    }


    TraceLog(LOG_INFO, "Fin de partie | Score=%d | AmmoMax=%d",score, player.maxAmmo);
    if (logFile) fclose(logFile);


    UnloadTexture(viseur);
    UnloadTexture(armeTex);
    CloseWindow(); // Ferme la fenêtre OpenGL
    return 0;
}

