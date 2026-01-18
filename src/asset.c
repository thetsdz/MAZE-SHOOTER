#include "asset.h"
#include <stdio.h> // Pour printf en cas d'erreur

Texture2D ChargerTexture(const char *fileName) {
    // 1. On charge l'image
    Texture2D texture = LoadTexture(fileName);

    // 2. Vérification de sécurité (Bonne pratique)
    if (texture.id == 0) {
        printf("ERREUR: Impossible de charger l'image : %s\n", fileName);
        // On pourrait renvoyer une texture violette "missing texture" ici
    }

    return texture;
}

void DessinerViseur(Texture2D texture, int screenWidth, int screenHeight) {
    // Calcul pour centrer l'image exactement
    // Position X = (LargeurEcran / 2) - (LargeurImage / 2)
    int posX = (screenWidth / 2) - (texture.width / 2);
    int posY = (screenHeight / 2) - (texture.height / 2);

    // Couleur WHITE signifie "pas de filtre couleur", on affiche l'image telle quelle
    DrawTexture(texture, posX, posY, WHITE);
}


void DessinerArme(Texture2D texture, int screenWidth, int screenHeight) {
    // Sécurité
    if (texture.id <= 0) return;

    // --- 1. Calcul de l'ÉCHELLE (Scaling) ---
    // On veut que l'arme occupe, par exemple, 25% (un quart) de la largeur de l'écran.
    float targetWidth = screenWidth * 0.25f;
    
    // Le facteur d'échelle est : LargeurVoulue / LargeurRéelle de l'image
    float scale = targetWidth / (float)texture.width;

    // --- 2. Calcul de la POSITION (Bas-Droite) ---
    // Marge pour ne pas coller au bord de l'écran
    int margin = 20;

    // Position X = LargeurEcran - LargeurImageMiseAL'echelle - Marge
    float posX = screenWidth - (texture.width * scale) - margin;
    
    // Position Y = HauteurEcran - HauteurImageMiseAL'echelle - Marge
    float posY = screenHeight - (texture.height * scale) - margin;

    // --- 3. Dessin avancé (DrawTextureEx) ---
    // On utilise un Vector2 pour la position
    Vector2 position = { posX, posY };
    float rotation = 15.0f;
    // Rotation 0.0f (pas de rotation), Scale calculé, couleur WHITE (normal)
    DrawTextureEx(texture, position, rotation, scale, WHITE);
}








