/**
* \file menu.c
* \brief Contient les fonctions de gestion du menu principal
* \author Hugues Astier
* \version 1.0
* \date 11/02.2026
*/

#include "raylib.h"
#include "raymath.h"
#include "../lib/headers/types.h"
#include "../lib/headers/menu.h"

static Music music;
static int musicLoaded = 0;

void UnloadMenuMusic() {
    if (musicLoaded) {
        UnloadMusicStream(music);
        musicLoaded = 0;
    }
}



// Variables statiques pour gérer la sélection
static int selectedButton = 0;      // 0: LANCER PARTIE, 1: OPTIONS, 2: QUITTER
static const char* texteBoutons[5] = {"NOUVELLE PARTIE", "MULTIJOUEUR","CHARGER DERNIERE PARTIE SAUVEGARDÉE","OPTIONS", "QUITTER"};


/**
*   \brief Gere en fonction de si les touches haut,bas,droite,gauche ou entrée sont utilisées
*   \param <Game screen **currentScreen> etat de l'ecran à modifier en question
*   
*/
void GestionClavier(GameScreen ** currentScreen){
    if (IsKeyPressed(KEY_DOWN )) {
        selectedButton = (selectedButton + 1) % 5;  // Passe au bouton suivant
    } 
    else if (IsKeyPressed(KEY_UP)) {
        selectedButton = (selectedButton - 1 + 5) % 5;  // Passe au bouton précédent
    } 
    else {
        if (IsKeyPressed(KEY_ENTER)) { 
            switch (selectedButton) {
                case 0: **currentScreen = NOUVELLE_PARTIE; DisableCursor(); break;  // LANCER PARTIE
                case 1: **currentScreen = MULTIJOUEUR; break;
                case 2: **currentScreen = CHARGER_PARTIE; break;
                case 3: **currentScreen = OPTIONS; break;
                case 4: **currentScreen = EXIT; break;
                default : **currentScreen = MENU; break;
            }
        }
    }
}

/**
*   \brief  Gère en fonction de si les boutons lancer partie, OPTION, QUITTER sont cliqués
*   \param <Game screen **currentScreen> etat de l'ecran actuel à modifier en question du clique
*   \param <Rectangle rect>, rect represente un des boutons (plus precisement sa zone)
    \param <int indice bouton>
*/
void GestionSouris(GameScreen **currentScreen, Rectangle rect, int indice_bouton) {
    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        selectedButton = indice_bouton; // La souris met à jour le bouton sélectionné au survol
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            switch (indice_bouton) {
                case 0: **currentScreen = NOUVELLE_PARTIE; DisableCursor(); break;
                case 1: **currentScreen = MULTIJOUEUR; break;
                case 2: **currentScreen = CHARGER_PARTIE; break;
                case 3: **currentScreen = OPTIONS; break;
                case 4: **currentScreen = EXIT; break;
                default : **currentScreen = MENU; break;
            }
        }
    }
}



/**
*   \brief  Gère le menu, ie, gestion de la logique de l'etat de l'ecran grace à GestionSouris(..) et GestionClavier(..) puis dessins des boutons 
*   \param <Game screen *currentScreen> Etat de l'ecran actuel 
*/
void GererMenu(GameScreen *currentScreen) {
    if (!musicLoaded) {
        music = LoadMusicStream("../assets/song/music.mp3");
        PlayMusicStream(music);
        musicLoaded = 1;
    }
    UpdateMusicStream(music);   





    ShowCursor();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, (Color){10, 10, 20, 255}); 
    // 1. Gestion des entrées
    GestionClavier(&currentScreen);

    
    // 2. Paramètres des boutons
    float btnW = 500;
    float btnH = 50;
    float posX = (sw - btnW) / 2.0f;
    float departY = sh/2 - 30;

    for (int i = 0; i < 5; i++) {
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