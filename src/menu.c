#include "../lib/linux/raylib-5.5_linux_amd64/include/raylib.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"
#include "../lib/headers/types.h"
#include "../lib/headers/menu.h"
// Variables statiques pour gérer la sélection

static int selectedButton = 0;      // 0: LANCER PARTIE, 1: OPTIONS, 2: QUITTER
static const char* texteBoutons[3] = {"LANCER PARTIE", "OPTIONS", "QUITTER"};



void DessinerBouton(Rectangle rect, const char* texte, int indice) {
   
    int textWidth = MeasureText(texte, 20);         // taille longeur en pixels du texte de police 20
    // Couleurs pour le mode sombre
    Color couleurFond ;
    Color couleurTexte ;
    Color couleurBordure;
                                                                     
    couleurFond=(Color){30, 30, 50, 255};       //bleu très foncé
    couleurTexte=LIGHTGRAY;                     // gris clair 
    couleurBordure=DARKGRAY;

    // Dessin du bouton (rempli, pas juste les contours)  + Dessin du texte centré
    DrawRectangleRec(rect, couleurFond);
    DrawRectangleLinesEx(rect,2,couleurBordure);
    DrawText(texte, rect.x + (rect.width - textWidth)/2, rect.y + (rect.height - 20)/2, 20, couleurTexte);


    return ;
}
void ChangementCouleurBouton(rect,)

void GestionEvenement(GameScreen ** currentScreen,Rectangle rect, int num_bouton){
    Vector2 mousePos = GetMousePosition();
    bool survol = CheckCollisionPointRec(mousePos, rect);

    if (IsKeyPressed(KEY_DOWN )) {
        selectedButton = (selectedButton + 1) % 3;  // Passe au bouton suivant
    } 
    else if (IsKeyPressed(KEY_UP)) {
        selectedButton = (selectedButton - 1 + 3) % 3;  // Passe au bouton précédent
    } 
    else {
        //Gestion souris et action en fonction du bouton cliquer
        if (survol && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        
            switch (num_bouton){ 
                case 0: **currentScreen = GAME; DisableCursor(); break;  // LANCER PARTIE
                case 1: **currentScreen = OPTIONS; break;  // OPTIONS
                case 2: **currentScreen = EXIT; break;  // QUITTER
            }
        }

        //Gestion clavier et Action en fonction du bouton sélectionné
        if (IsKeyPressed(KEY_ENTER)) { 
            switch (selectedButton) {
                case 0: **currentScreen = GAME; DisableCursor(); break;  // LANCER PARTIE
                case 1: **currentScreen = OPTIONS; break;  // OPTIONS
                case 2: **currentScreen = EXIT; break;  // QUITTER
            }
        }
    else {
        if (survol  || indice==selectedButton){
        couleurFond=(Color){50, 50, 80, 255};       // Bleu foncé clair     Cette affecattion dit : Color monBleu; monBleu.r = 30; monBleu.g = 30; monBleu.b = 50; monBleu.a = 255;
        couleurTexte=WHITE;                         // Blanc                avec monBleu structure temmporaire faite pour affectation avec CouleurTexte
        couleurBordure=WHITE;
    } 
    }
}



void GererMenu(GameScreen* currentScreen) {
    int sw = GetScreenWidth();  //largeur ecran/fenetre de jeu sur l'horizontale
    int sh = GetScreenHeight(); //longueur ecran/fenetre de jeu sur la verticale
    void ShowCursor(void);
    bool action_souris=FALSE;
    

    // --- Gestion des entrées clavier ---
    GestionClavier(&currentScreen);


    //Partie Dessin :

    // Fond sombre
    DrawRectangle(0, 0, sw, sh, (Color){20, 20, 40, 255});
    // --- Dessin des boutons ---
    float btnW = 300;   // largeur bouton horizontal
    float btnH = 50;    // longueur bouton vertical
    float posX = (sw - btnW) / 2.0f;
    float departY = sh/2 - 30;  // Position centrée verticalement

    
    for (int i=0;i<3;i++){
        Rectangle rect = {posX, departY + i * 70, btnW, btnH};       // Position du bouton numero i
        CreerBouton(rect,texteBoutons[i],i);
        GestionEvenement(&currentScreen,rect,i);
        }

        DrawFPS(10, 10);
         // Titre, sous-titre et instructions pour le clavier
        DrawText("MAZE SHOOTER", sw/2 - MeasureText("MAZE SHOOTER", 40)/2, sh/4 - 50, 40, WHITE);
        DrawText("MENU PRINCIPAL", sw/2 - MeasureText("MENU PRINCIPAL", 25)/2, sh/4, 25, LIGHTGRAY);
        DrawText("Utilise les flèches et Entrée pour naviguer", sw/2 - MeasureText("Utilise les flèches et Entrée pour naviguer", 15)/2, sh - 50, 15, LIGHTGRAY);

    }
}

