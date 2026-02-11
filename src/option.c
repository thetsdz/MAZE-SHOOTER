/**
*   \file option.c
*   \brief Gere la page option
*   \author Hugues Astier
*   \version 1.0
*   \date 11/02/2026
*/




#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"
#include "../lib/headers/types.h"
#include "../lib/headers/menu.h"

static const char* texteBoutonsOption[7]= {"Z/Fleche haut : avancer","S/Fleche Bas : Reculer", "Q/Fleche Gauche : Gauche","D/Fleche Droite : Droite","R : recharger ","Retour Arriere : Revient au menu precedent","Echap : Quitter le jeux"};

void GererOption(GameScreen *currentScreen) {
    if (IsKeyPressed(KEY_BACKSPACE)) *currentScreen = MENU;
    ShowCursor();
    
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Fond d'écran
    DrawRectangle(0, 0, sw, sh, (Color){10, 10, 20, 255}); 

    Rectangle rectRetour = { 50, 50, 280, 70 };
    
    // --- CORRECTION ICI ---
    // 1. On déclare les variables couleur
    Color couleurFondR;
    Color couleurTexteR;
    Color couleurBordureR;

    // 2. On vérifie DIRECTEMENT si la souris est dessus
    bool survol = CheckCollisionPointRec(GetMousePosition(), rectRetour);

    if (survol) {
        // Cas : Souris dessus -> Blanc / Allumé
        couleurFondR = (Color){50, 50, 80, 255}; 
        couleurTexteR = WHITE;
        couleurBordureR = WHITE;
        
        // Clic pour sortir (On gère le clic ici aussi, c'est plus propre)
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            *currentScreen = MENU;
        }
    }
    else {
        // Cas : Souris ailleurs -> Bleu Foncé / Éteint
        couleurFondR = (Color){30, 30, 50, 255};
        couleurTexteR = LIGHTGRAY;
        couleurBordureR = DARKGRAY;
    }

    
    // 2. Paramètres des options
    float btnW = 500;
    float btnH = 60;
    float posX = (sw - btnW) / 2.0f;
    float departY = sh/2 - 150;

    for (int i = 0; i < 7; i++) {
        Rectangle rect = {posX, departY + i * 70, btnW, btnH};
    
        // 4. Préparation des couleurs avec des if/else
        Color couleurFond= (Color){30, 30, 50, 255}; // Bleu très foncé
        Color couleurTexte= LIGHTGRAY;
        Color couleurBordure= DARKGRAY;
        

        // 5. DESSIN 7 options
        DrawRectangleRec(rect, couleurFond);
        DrawRectangleLinesEx(rect, 2, couleurBordure);
        
        int tW = MeasureText(texteBoutonsOption[i], 20);
        DrawText(texteBoutonsOption[i], rect.x + (rect.width - tW)/2, rect.y + (rect.height - 20)/2, 20, couleurTexte);
    }

    // 5. DESSIN Bouton retour
    DrawRectangleRec(rectRetour, couleurFondR);
    DrawRectangleLinesEx(rectRetour, 2, couleurBordureR);
    int tW = MeasureText("Retour au menu", 30);
    DrawText("Retour au menu", rectRetour.x + (rectRetour.width - tW)/2, rectRetour.y + (rectRetour.height - 30)/2, 30, couleurTexteR);
}