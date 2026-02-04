#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"
#include "../lib/headers/types.h"
 /*pour compiler :
  gcc menu.c -o menu -I../lib/linux/raylib-5.5_linux_amd64/include -I../lib/headers ../lib/linux/raylib-5.5_linux_amd64/lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11 
*/


#include "raylib.h"
#include "types.h"

#include "raylib.h"
#include "types.h"

// Fonction de bouton (inchangée mais cruciale pour la propreté)
bool DessinerBouton(Rectangle rect, const char* texte) {
    Vector2 mousePos = GetMousePosition();
    bool survol = CheckCollisionPointRec(mousePos, rect);
    
    DrawRectangleLinesEx(rect, 2, survol ? WHITE : GRAY);
    int textWidth = MeasureText(texte, 20);
    DrawText(texte, rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - 20)/2, 20, survol ? WHITE : LIGHTGRAY);

    return (survol && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

int main(void) {
    // Initialisation avec le flag RESIZABLE pour aider au redimensionnement
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Menu Dynamique");
    GameScreen currentScreen = MENU;
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- 1. LOGIQUE ---
        if (IsKeyPressed(KEY_F)) ToggleFullscreen();

        if (currentScreen == TEST && IsKeyPressed(KEY_BACKSPACE)) currentScreen = MENU;

        // --- 2. DESSIN ---
        BeginDrawing();
            ClearBackground(BLACK);

            // INSTRUCTION VITALE : On récupère les dimensions ICI, à chaque tour de boucle
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();

            switch(currentScreen) {
                case MENU: {
                    // On définit les tailles
                    float btnW = 300;
                    float btnH = 50;
                    
                    // ON RECALCULE LES POSITIONS ICI : elles s'adaptent au 'sw' et 'sh' actuels
                    float posX = (sw - btnW) / 2.0f;
                    float departY = sh / 3.0f;

                    DrawText("MENU PRINCIPAL", sw/2 - MeasureText("MENU PRINCIPAL", 30)/2, departY - 80, 30, RAYWHITE);

                    if (DessinerBouton((Rectangle){posX, departY, btnW, btnH}, "LANCER PARTIE")) {
                        currentScreen = TEST;
                    }
                    if (DessinerBouton((Rectangle){posX, departY + 70, btnW, btnH}, "OPTIONS")) {
                        /* Action */
                    }
                    if (DessinerBouton((Rectangle){posX, departY + 140, btnW, btnH}, "QUITTER")) {
                        CloseWindow(); return 0;
                    }
                    break;
                }

                case TEST:
                    DrawText("ZONE DE TEST", sw/2 - MeasureText("ZONE DE TEST", 20)/2, sh/2, 20, MAROON);
                    break;
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}