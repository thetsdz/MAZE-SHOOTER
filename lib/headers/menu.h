// menu.h
#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "types.h"

// Fonction pour dessiner un bouton et détecter un clic
bool DessinerBouton(Rectangle rect, const char* texte);

// Fonction pour gérer la logique du menu
void GererMenu(GameScreen* currentScreen);
void GererOption(GameScreen* currentScreen);

#endif
