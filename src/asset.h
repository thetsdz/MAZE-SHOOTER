#ifndef ASSET_H
#define ASSET_H

#include <raylib.h>

// Charge une image depuis le disque et la transforme en Texture optimisée GPU
Texture2D ChargerTexture(const char *fileName);

// Dessine une texture au centre de l'écran (parfait pour un viseur)
void DessinerViseur(Texture2D texture, int screenWidth, int screenHeight);

void DessinerArme(Texture2D texture, int screenWidth, int screenHeight);

#endif

