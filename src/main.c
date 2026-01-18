#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Inclusion de nos propres modules
#include "types.h"
#include "level.h"
#include "player.h"
#include "projectile.h"
#include "asset.h"

int main(void){
    // --- Initialisation Fenêtre & Raylib ---
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "JEU REFACTORISE");
    ToggleFullscreen();
    SetTargetFPS(60);   // Essaye de maintenir 60 images/seconde
    DisableCursor();    // Bloque la souris dans la fenêtre pour la visée
    srand(time(NULL));  // Initialise le générateur aléatoire

    // --- Initialisation des Objets ---
    Player player;
    InitPlayer(&player);

    Block blocks[NUM_BLOCKS];
    InitLevel(blocks);

    Projectile projs[MAX_PROJ];
    InitProjectiles(projs);

    // Setup de la cible rouge
    Vector3 ciblePos = {(float)(rand()%20-10), 0.5f, (float)(rand()%20-10)};
    float cibleRadius = 0.5f;
    int score = 0;

    // Setup Caméra Raylib standard
    Camera3D camera = {0};
    camera.up = (Vector3){0,1,0}; // L'axe Y pointe vers le haut
    camera.fovy = 60;              // Champ de vision (Field of View)
    camera.projection = CAMERA_PERSPECTIVE;

    // Initialisation des textures
    Texture2D viseur = ChargerTexture("../assets/crosshair.png");
    Texture2D armeTex = ChargerTexture("../assets/weapon_placeholder.png");

    // Fichier de log
    FILE *f = fopen("log.txt", "w");
    // --- 3. Boucle Principale ---
    while(!WindowShouldClose()){
        
        if(IsKeyPressed(KEY_ESCAPE)) break;

        // --- ETAPE UPDATE ---
        
        UpdatePlayer(&player, blocks, &camera);

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
        if(IsKeyPressed(KEY_U)){
            FILE * fr = fopen("save.txt", "r");
            if(fr){
                fscanf(fr, "%d\n", &score);
                fscanf(fr, "%d\n", &player.maxAmmo);
                fclose(fr);
            }
        }
        // --- GESTION MUNITIONS & AMELIORATIONS ---

        // Recharger (Touche R)
        if(IsKeyPressed(KEY_R)){
            player.ammo = player.maxAmmo; // Remplit le chargeur au max actuel
        }

        // Acheter Amélioration (Touche E)
        // Condition : Avoir assez de score ET ne pas dépasser 50 de capacité max
        if(IsKeyPressed(KEY_E) && score >= SCORE_TRADE && player.maxAmmo < MAX_PROJ){
            score -= SCORE_TRADE;       // Coût
            player.maxAmmo += 2; // Bonus
            TraceLog(LOG_INFO, "Achat amélioration : nouvelle capacité max = %d", player.maxAmmo);
        }

        // Tirer (Clic Gauche)
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            if(player.ammo > 0) {
                ShootProjectile(projs, player); 
                player.ammo--; // On décrémente une balle
            } else {
                TraceLog(LOG_INFO, "Clic! Plus de munitions.");
            }
        }

        UpdateProjectiles(projs, blocks, &ciblePos, cibleRadius, &score);

        if(Vector3Distance(player.pos, ciblePos) <= player.size/2 + cibleRadius){
            score++;
            ciblePos.x = (float)(rand()%20-10);
            ciblePos.z = (float)(rand()%20-10);
        }

        // --- ETAPE DRAW ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
            DrawLevel(blocks);
            DrawCube(player.pos, player.size, player.size, player.size, BLUE);
            DrawSphere(ciblePos, cibleRadius, RED);
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

    // --- Enregistrement des informations de fin de partie ---
    if(f){
        fprintf(f, "%d\n", score);
        fprintf(f, "%d\n", player.maxAmmo);
    }
    // --- Nettoyage ---
    if(f) fclose(f);
    UnloadTexture(viseur);
    UnloadTexture(armeTex);
    CloseWindow(); // Ferme la fenêtre OpenGL
    return 0;
}

