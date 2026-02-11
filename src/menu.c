#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"
#include "../lib/headers/types.h"
#include "../lib/headers/menu.h"

// Variables statiques pour gérer la sélection
static int selectedButton = 0;      // 0: LANCER PARTIE, 1: OPTIONS, 2: QUITTER
static const char* texteBoutons[3] = {"LANCER PARTIE", "OPTIONS", "QUITTER"};



void Gestionclavier(GameScreen ** currentScreen){
    if (IsKeyPressed(KEY_DOWN )) {
        selectedButton = (selectedButton + 1) % 3;  // Passe au bouton suivant
    } 
    else if (IsKeyPressed(KEY_UP)) {
        selectedButton = (selectedButton - 1 + 3) % 3;  // Passe au bouton précédent
    } 
    else {
        if (IsKeyPressed(KEY_ENTER)) { 
            switch (selectedButton) {
                case 0: **currentScreen = GAME; DisableCursor(); break;  // LANCER PARTIE
                case 1: **currentScreen = OPTIONS; break;  // OPTIONS
                case 2: **currentScreen = EXIT; break;  // QUITTER
            }
        }
    }
}

void GestionSouris(GameScreen **currentScreen, Rectangle rect, int indice_bouton) {
    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        selectedButton = indice_bouton; // La souris met à jour le bouton sélectionné au survol
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            switch (indice_bouton) {
                case 0: **currentScreen = GAME; DisableCursor(); break;
                case 1: **currentScreen = OPTIONS; break;
                case 2: **currentScreen = EXIT; break;
                default : **currentScreen = MENU;
            }
        }
    }
}


void GererMenu(GameScreen *currentScreen) {
    
    ShowCursor();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, (Color){10, 10, 20, 255}); 
    // 1. Gestion des entrées
    Gestionclavier(&currentScreen);

    
    // 2. Paramètres des boutons
    float btnW = 300;
    float btnH = 50;
    float posX = (sw - btnW) / 2.0f;
    float departY = sh/2 - 30;

    for (int i = 0; i < 3; i++) {
        Rectangle rect = {posX, departY + i * 70, btnW, btnH};

        // 3. Gestion souris pour ce bouton précis
        GestionSouris(&currentScreen, rect, i);
    

        // 4. Préparation des couleurs avec des if/else
        Color couleurFond;
        Color couleurTexte;
        Color couleurBordure;

        if (i == selectedButton) {
            // Apparence du bouton sélectionné (Clavier ou Souris)
            couleurFond = (Color){50, 50, 80, 255}; // Bleu plus clair
            couleurTexte = WHITE;
            couleurBordure = WHITE;
        } 
        else {
            // Apparence du bouton normal
            couleurFond = (Color){30, 30, 50, 255}; // Bleu très foncé
            couleurTexte = LIGHTGRAY;
            couleurBordure = DARKGRAY;
        }

        // 5. DESSIN
        DrawRectangleRec(rect, couleurFond);
        DrawRectangleLinesEx(rect, 2, couleurBordure);
        
        int tW = MeasureText(texteBoutons[i], 20);
        DrawText(texteBoutons[i], rect.x + (rect.width - tW)/2, rect.y + (rect.height - 20)/2, 20, couleurTexte);
    }
    // Titres et textes fixes
    DrawText("MAZE SHOOTER", sw/2 - MeasureText("MAZE SHOOTER", 40)/2, sh/4 - 50, 40, WHITE);
    DrawText("MENU PRINCIPAL", sw/2 - MeasureText("MENU PRINCIPAL", 25)/2, sh/4, 25, LIGHTGRAY);
    DrawText("Utilise les flèches et Entrée pour naviguer", sw/2 - MeasureText("Utilise les flèches et Entrée pour naviguer", 15)/2, sh - 50, 15, LIGHTGRAY);
  
}






    