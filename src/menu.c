#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"
#include "../lib/headers/types.h"
#include "../lib/headers/menu.h"
// Variables statiques pour gérer la sélection
static int selectedButton = 0;  // 0: LANCER PARTIE, 1: OPTIONS, 2: QUITTER
static const char* texteBoutons[3] = {"LANCER PARTIE", "OPTIONS", "QUITTER"};


bool DessinerBouton(Rectangle rect, const char* texte) {
    Vector2 mousePos = GetMousePosition();
    bool survol = CheckCollisionPointRec(mousePos, rect);

    // Couleurs pour le mode sombre
    Color couleurFond = (survol) ? (Color){50, 50, 80, 255} : (Color){30, 30, 50, 255};  // Bleu foncé clair/bleu très foncé
    Color couleurTexte = (survol) ? WHITE : LIGHTGRAY;  // Blanc ou gris clair

    // Dessin du bouton (rempli, pas juste les contours)
    DrawRectangleRec(rect, couleurFond);
    DrawRectangleLinesEx(rect, 2, (survol) ? WHITE : DARKGRAY);  // Contour blanc ou gris foncé

    // Dessin du texte centré
    int textWidth = MeasureText(texte, 20);
    DrawText(texte, rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - 20)/2, 20, couleurTexte);

    // Retourne vrai si la souris est sur le bouton ET qu'on clique
    return (survol && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}





void GererMenu(GameScreen* currentScreen) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Fond sombre
    DrawRectangle(0, 0, sw, sh, (Color){20, 20, 40, 255});

    // Titre et sous-titre
    DrawText("MAZE SHOOTER", sw/2 - MeasureText("MAZE SHOOTER", 40)/2, sh/4 - 50, 40, WHITE);
    DrawText("MENU PRINCIPAL", sw/2 - MeasureText("MENU PRINCIPAL", 25)/2, sh/4, 25, LIGHTGRAY);

    // Instructions pour le clavier
    DrawText("Utilise les flèches et Entrée pour naviguer", sw/2 - MeasureText("Utilise les flèches et Entrée pour naviguer", 15)/2, sh - 50, 15, LIGHTGRAY);

    // --- Gestion des entrées clavier ---
    if (IsKeyPressed(KEY_DOWN)) {
        selectedButton = (selectedButton + 1) % 3;  // Passe au bouton suivant
    } else if (IsKeyPressed(KEY_UP)) {
        selectedButton = (selectedButton - 1 + 3) % 3;  // Passe au bouton précédent
    } else if (IsKeyPressed(KEY_ENTER)) {
        // Action en fonction du bouton sélectionné
        switch (selectedButton) {
            case 0: *currentScreen = TEST; break;  // LANCER PARTIE
            case 1: *currentScreen = OPTIONS; break;  // OPTIONS
            case 2: *currentScreen = EXIT; break;  // QUITTER
        }
    }

    // --- Dessin des boutons ---
    float btnW = 300;
    float btnH = 50;
    float posX = (sw - btnW) / 2.0f;
    float departY = sh/2 - 30;  // Position centrée verticalement

    for (int i = 0; i < 3; i++) {
        // Couleur du bouton : bleu clair si sélectionné, bleu foncé sinon
        Color couleurFond = (i == selectedButton) ? (Color){50, 50, 80, 255} : (Color){30, 30, 50, 255};
        Color couleurTexte = (i == selectedButton) ? WHITE : LIGHTGRAY;

        // Position du bouton i
        Rectangle rect = {posX, departY + i * 70, btnW, btnH};

        // Dessin du bouton
        DrawRectangleRec(rect, couleurFond);
        DrawRectangleLinesEx(rect, 2, (i == selectedButton) ? WHITE : DARKGRAY);

        // Dessin du texte centré
        int textWidth = MeasureText(texteBoutons[i], 20);
        DrawText(texteBoutons[i], rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - 20)/2, 20, couleurTexte);
    }
}

