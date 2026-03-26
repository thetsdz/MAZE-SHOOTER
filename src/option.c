/**
 * \file option.c
 */
#include <stddef.h>
#include "raylib.h"
#include "raymath.h"
#include "../lib/headers/types.h"
#include "../lib/headers/menu.h"
#include "../lib/headers/option.h"

static const char *texteBoutonsOption[7] = {
    "Z/Fleche haut : avancer",
    "S/Fleche Bas : Reculer",
    "Q/Fleche Gauche : Gauche",
    "D/Fleche Droite : Droite",
    "R : recharger",
    "Retour Arriere : Revient au menu precedent",
    "Echap : Quitter le jeux"};

// Thèmes disponibles
static const ThemeInfo themes[NB_THEMES] = {
    {"Herbe & Buissons", "../assets/images/bush.jpg",  "../assets/images/herbe.png",    ""},
    {"Beton & Briques",  "../assets/images/brick.png", "../assets/images/concrete.png", ""},
    {"Neige & Glace",    "../assets/images/ice.jpg",   "../assets/images/snow.png",     ""},
    {"Lave & Roche",     "../assets/images/rock.png",  "../assets/images/lava.png",     ""},
};

static ThemeId selectedTheme = THEME_BETON;

const ThemeInfo *GetSelectedTheme(void)
{
    return &themes[selectedTheme];
}

void GererOption(GameScreen *currentScreen)
{
    if (IsKeyPressed(KEY_BACKSPACE))
        *currentScreen = MENU;
    ShowCursor();
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, (Color){10, 10, 20, 255});

    // --- Bouton retour ---
    Rectangle rectRetour = {50, 50, 280, 70};
    bool survol = CheckCollisionPointRec(GetMousePosition(), rectRetour);

    Color couleurFondR    = survol ? (Color){50, 50, 80, 255} : (Color){30, 30, 50, 255};
    Color couleurTexteR   = survol ? WHITE    : LIGHTGRAY;
    Color couleurBordureR = survol ? WHITE    : DARKGRAY;

    if (survol && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        *currentScreen = MENU;

    // --- Paramètres communs des boutons ---
    float btnW    = 500;
    float btnH    = 60;
    float posX    = (sw - btnW) / 2.0f;
    float departY = sh / 2.0f - 250;

    // --- Touches (7 lignes) ---
    for (int i = 0; i < 7; i++)
    {
        Rectangle rect = {posX, departY + i * 65, btnW, btnH};
        DrawRectangleRec(rect, (Color){30, 30, 50, 255});
        DrawRectangleLinesEx(rect, 2, DARKGRAY);
        int tW = MeasureText(texteBoutonsOption[i], 20);
        DrawText(texteBoutonsOption[i],
                 rect.x + (rect.width - tW) / 2,
                 rect.y + (rect.height - 20) / 2,
                 20, LIGHTGRAY);
    }

    // --- Sélection du thème (même style que les boutons du dessus) ---
    float themeY = departY + 7 * 65 + 20;

    // Bouton flèche gauche
    float arrowBtnW = 60;
    float nomBtnW   = btnW - arrowBtnW * 2 - 4; // les 2 flèches + 2 gaps de 2px
    float arrowY    = themeY;

    Rectangle rectG = {posX,                          arrowY, arrowBtnW, btnH};
    Rectangle rectN = {posX + arrowBtnW + 2,          arrowY, nomBtnW,   btnH};
    Rectangle rectD = {posX + arrowBtnW + 2 + nomBtnW + 2, arrowY, arrowBtnW, btnH};

    bool hG = CheckCollisionPointRec(GetMousePosition(), rectG);
    bool hD = CheckCollisionPointRec(GetMousePosition(), rectD);

    // Clics souris
    if (hG && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        selectedTheme = (selectedTheme - 1 + NB_THEMES) % NB_THEMES;
    if (hD && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        selectedTheme = (selectedTheme + 1) % NB_THEMES;

    // Clavier <- ->
    if (IsKeyPressed(KEY_LEFT))
        selectedTheme = (selectedTheme - 1 + NB_THEMES) % NB_THEMES;
    if (IsKeyPressed(KEY_RIGHT))
        selectedTheme = (selectedTheme + 1) % NB_THEMES;

    // Dessin bouton < (même couleur hover que les autres)
    DrawRectangleRec(rectG, hG ? (Color){50, 50, 80, 255} : (Color){30, 30, 50, 255});
    DrawRectangleLinesEx(rectG, 2, hG ? WHITE : DARKGRAY);
    int twG = MeasureText("<", 20);
    DrawText("<", rectG.x + (rectG.width - twG) / 2,
             rectG.y + (rectG.height - 20) / 2, 20, hG ? WHITE : LIGHTGRAY);

    // Dessin bouton nom du thème (mis en valeur si sélectionné = toujours "actif")
    DrawRectangleRec(rectN, (Color){50, 50, 80, 255});
    DrawRectangleLinesEx(rectN, 2, WHITE);
    const char *nomTheme = themes[selectedTheme].nom;
    int twN = MeasureText(nomTheme, 20);
    DrawText(nomTheme, rectN.x + (rectN.width - twN) / 2,
             rectN.y + (rectN.height - 20) / 2, 20, WHITE);

    // Dessin bouton >
    DrawRectangleRec(rectD, hD ? (Color){50, 50, 80, 255} : (Color){30, 30, 50, 255});
    DrawRectangleLinesEx(rectD, 2, hD ? WHITE : DARKGRAY);
    int twD = MeasureText(">", 20);
    DrawText(">", rectD.x + (rectD.width - twD) / 2,
             rectD.y + (rectD.height - 20) / 2, 20, hD ? WHITE : LIGHTGRAY);

    // Points indicateurs sous le sélecteur
    float dotsY      = arrowY + btnH + 10;
    float dotsStartX = posX + btnW / 2.0f - (NB_THEMES * 18) / 2.0f;
    for (ThemeId i = 0; i < NB_THEMES; i++)
    {
        Color dot = (i == selectedTheme) ? WHITE : DARKGRAY;
        DrawCircle((int)(dotsStartX + i * 18), (int)dotsY, 4, dot);
    }

    // --- Dessin bouton retour ---
    DrawRectangleRec(rectRetour, couleurFondR);
    DrawRectangleLinesEx(rectRetour, 2, couleurBordureR);
    int tW = MeasureText("Retour au menu", 30);
    DrawText("Retour au menu",
             rectRetour.x + (rectRetour.width - tW) / 2,
             rectRetour.y + (rectRetour.height - 30) / 2,
             30, couleurTexteR);
}