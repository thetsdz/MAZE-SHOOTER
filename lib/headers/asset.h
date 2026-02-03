#ifndef ASSET_H
#define ASSET_H

#include "../linux/raylib-5.5_linux_amd64/include/raylib.h"

/** @brief Charge une image depuis le disque et la transforme en Texture optimisée GPU*/
Texture2D ChargerTexture(const char *fileName);

/** @brief Dessine une texture au centre de l'écran (parfait pour un viseur)*/
void DessinerViseur(Texture2D texture, int screenWidth, int screenHeight);

/** @brief Dessine une texture au centre de l'écran (parfait pour une arme)*/
void DessinerArme(Texture2D texture, int screenWidth, int screenHeight);

#endif