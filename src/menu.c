#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"

typedef enum GameScreen { MENU, TEST } GameScreen;

int main(void) {
    InitWindow(800, 450, "Menu de Test");
    GameScreen currentScreen = MENU; // État initial
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- LOGIQUE ---
        switch(currentScreen) {
            case MENU:
                if (IsKeyPressed(KEY_ENTER)) currentScreen = TEST;
                break;
            case TEST:
                if (IsKeyPressed(KEY_BACKSPACE)) currentScreen = MENU;
                break;
        }

        // --- DESSIN ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch(currentScreen) {
                case MENU:
                    DrawText("MENU PRINCIPAL", 190, 200, 20, DARKGRAY);
                    DrawText("Appuyez sur ENTRER pour tester", 190, 230, 10, GRAY);
                    break;
                case TEST:
                    DrawText("ZONE DE TEST", 190, 200, 20, MAROON);
                    DrawText("Appuyez sur RETOUR pour le menu", 190, 230, 10, GRAY);
                    break;
            }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}