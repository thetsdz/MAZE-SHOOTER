/**
 * \file endGame.c
 */

#include "../lib/headers/endGame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/headers/menu.h"
#include "../lib/headers/types.h"
#include "raylib.h"
#include "raymath.h"

static const char* texteBoutonsDefaite[3] = {
    "GAME OVER", "Vous avez perdu.", "Score de fin de partie :%d points."};
static const char* texteBoutonsVictoire[3] = {
    "VICTOIRE", "Vous avez gagné.", "Score de fin de partie :%d points."};

void GererGameOver(GameScreen* currentScreen, int score) {
  char scoretext[50];
  snprintf(scoretext, sizeof(scoretext), "Score de fin de partie : %d points.",
           score);
  if (IsKeyPressed(KEY_ENTER)) *currentScreen = MENU;
  ShowCursor();

  int sw = GetScreenWidth();
  int sh = GetScreenHeight();

  // Fond d'écran
  DrawRectangle(0, 0, sw, sh, (Color){10, 10, 20, 255});

  Rectangle rectRetour = {50, 50, 280, 70};

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
  } else {
    // Cas : Souris ailleurs -> Bleu Foncé / Éteint
    couleurFondR = (Color){30, 30, 50, 255};
    couleurTexteR = LIGHTGRAY;
    couleurBordureR = DARKGRAY;
  }

  // 2. Paramètres des options
  float btnW = 500;
  float btnH = 60;
  float posX = (sw - btnW) / 2.0f;
  float departY = sh / 2 - 150;

  for (int i = 0; i < 3; i++) {
    Rectangle rect = {posX, departY + i * 70, btnW, btnH};

    // Préparation des couleurs avec des if/else
    Color couleurFond = (Color){30, 30, 50, 255};  // Bleu très foncé
    Color couleurTexte = LIGHTGRAY;
    Color couleurBordure = DARKGRAY;

    // DESSIN des rectangles
    DrawRectangleRec(rect, couleurFond);
    DrawRectangleLinesEx(rect, 2, couleurBordure);

    // On choisit quel texte on va dessiner
    const char* texteADessiner;
    if (i == 2) {
      texteADessiner = scoretext;  // Pour la 3ème case, on utilise ton texte
                                   // formaté avec le score
    } else {
      texteADessiner = texteBoutonsDefaite[i];  // Pour les autres, on utilise
                                                // le tableau "GAME OVER", etc.
    }

    // On utilise texteADessiner pour mesurer et dessiner
    int tW = MeasureText(texteADessiner, 20);
    DrawText(texteADessiner, rect.x + (rect.width - tW) / 2,
             rect.y + (rect.height - 20) / 2, 20, couleurTexte);
  }

  // 5. DESSIN Bouton retour
  DrawRectangleRec(rectRetour, couleurFondR);
  DrawRectangleLinesEx(rectRetour, 2, couleurBordureR);
  int tW = MeasureText("Retour au menu", 30);
  DrawText("Retour au menu", rectRetour.x + (rectRetour.width - tW) / 2,
           rectRetour.y + (rectRetour.height - 30) / 2, 30, couleurTexteR);
}

void GererVictoire(GameScreen* currentScreen, int score) {
  char scoretext[50];
  snprintf(scoretext, sizeof(scoretext), "Score de fin de partie : %d points.",
           score);
  if (IsKeyPressed(KEY_ENTER)) *currentScreen = MENU;
  ShowCursor();

  int sw = GetScreenWidth();
  int sh = GetScreenHeight();

  // Fond d'écran
  DrawRectangle(0, 0, sw, sh, (Color){10, 10, 20, 255});

  Rectangle rectRetour = {50, 50, 280, 70};

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
  } else {
    // Cas : Souris ailleurs -> Bleu Foncé / Éteint
    couleurFondR = (Color){30, 30, 50, 255};
    couleurTexteR = LIGHTGRAY;
    couleurBordureR = DARKGRAY;
  }

  // 2. Paramètres des options
  float btnW = 500;
  float btnH = 60;
  float posX = (sw - btnW) / 2.0f;
  float departY = sh / 2 - 150;

  for (int i = 0; i < 3; i++) {
    Rectangle rect = {posX, departY + i * 70, btnW, btnH};

    // Préparation des couleurs avec des if/else
    Color couleurFond = (Color){30, 30, 50, 255};  // Bleu très foncé
    Color couleurTexte = LIGHTGRAY;
    Color couleurBordure = DARKGRAY;

    // DESSIN des rectangles
    DrawRectangleRec(rect, couleurFond);
    DrawRectangleLinesEx(rect, 2, couleurBordure);

    // On choisit quel texte on va dessiner
    const char* texteADessiner;
    if (i == 2) {
      texteADessiner = scoretext;  // Pour la 3ème case, on utilise ton texte
                                   // formaté avec le score
    } else {
      texteADessiner = texteBoutonsVictoire[i];  // Pour les autres, on utilise
                                                 // le tableau "GAME OVER", etc.
    }

    // On utilise texteADessiner pour mesurer et dessiner
    int tW = MeasureText(texteADessiner, 20);
    DrawText(texteADessiner, rect.x + (rect.width - tW) / 2,
             rect.y + (rect.height - 20) / 2, 20, couleurTexte);
  }

  // 5. DESSIN Bouton retour
  DrawRectangleRec(rectRetour, couleurFondR);
  DrawRectangleLinesEx(rectRetour, 2, couleurBordureR);
  int tW = MeasureText("Retour au menu", 30);
  DrawText("Retour au menu", rectRetour.x + (rectRetour.width - tW) / 2,
           rectRetour.y + (rectRetour.height - 30) / 2, 30, couleurTexteR);
}