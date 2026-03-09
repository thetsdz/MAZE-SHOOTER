/** \file asset.c
    \brief Gère les textures
    \author Hugues Astier
*/

#include <stdio.h> 
#include "../lib/headers/asset.h"


Texture2D ChargerTexture(const char *fileName) {
    Texture2D texture = LoadTexture(fileName);
    if (texture.id == 0) {
        printf("ERREUR: Impossible de charger l'image : %s\n", fileName);
    }
    return texture;
}


void DessinerViseur(Texture2D texture, int screenWidth, int screenHeight) {
    int posX = (screenWidth / 2) - (texture.width / 2);
    int posY = (screenHeight / 2) - (texture.height / 2);
    DrawTexture(texture, posX, posY, WHITE);
}





void DessinerArme(Texture2D texture, int screenWidth, int screenHeight) {
    if (texture.id <= 0) return;

    // --- 1. Calcul de l'ÉCHELLE ---
    float ratio = 0.25f;
    
    // Petite astuce : si l'image est presque carrée (comme la grenade), 
    // on réduit un peu le ratio pour qu'elle ne prenne pas tout l'écran en hauteur.
    if (texture.width < 400) ratio = 0.15f; 

    float targetWidth = screenWidth * ratio;
    float scale = targetWidth / (float)texture.width;

    // --- 2. Calcul de la POSITION ---
    int margin = 20;
    int offsetY = 120; 

    float posX = screenWidth - (texture.width * scale) - margin;
    float posY = screenHeight - (texture.height * scale) - margin - offsetY;

    // --- 3. DESSIN ---
    Vector2 position = { posX, posY };
    float rotation = 20.0f; 

    // Pour la grenade, on peut annuler la rotation pour qu'elle reste droite
    if (texture.width < 400) rotation = 0.0f;

    DrawTextureEx(texture, position, rotation, scale, WHITE);
}