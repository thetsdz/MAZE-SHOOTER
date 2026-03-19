#include <raylib.h>

int main() {
    InitWindow(100, 100, "Generateur Pistolet");
    int w = 300;
    int h = 200;
    Image pistoletImg = GenImageColor(w, h, BLANK);

    Color polymereNoir = (Color){ 30, 30, 35, 255 };
    Color acierCulasse = (Color){ 70, 70, 80, 255 };
    Color metalClair  = (Color){ 110, 110, 120, 255 };
    Color gripTexture = (Color){ 50, 50, 55, 255 };

    // --- Dessin Inversé (Canon à gauche) ---

    // 1. La Poignée (Grip) - Déplacée à DROITE
    ImageDrawRectangle(&pistoletImg, 190, 80, 60, 110, polymereNoir);
    ImageDrawRectangle(&pistoletImg, 200, 100, 40, 70, gripTexture);

    // 2. La Carcasse inférieure (Frame)
    ImageDrawRectangle(&pistoletImg, 70, 60, 200, 40, polymereNoir);

    // 3. Le Pontet (Garde-détente)
    ImageDrawRectangle(&pistoletImg, 160, 90, 50, 10, polymereNoir); 
    ImageDrawRectangle(&pistoletImg, 160, 90, 10, 30, polymereNoir); 

    // 4. La Culasse (Slide) - S'étend vers la GAUCHE
    ImageDrawRectangle(&pistoletImg, 30, 30, 240, 40, acierCulasse);

    // 5. Le Canon (Muzzle) à GAUCHE
    ImageDrawRectangle(&pistoletImg, 20, 40, 20, 20, metalClair);

    // 6. Détails
    // Striations à l'arrière (maintenant à DROITE)
    for(int i=0; i<5; i++) {
        ImageDrawRectangle(&pistoletImg, 230 - (i*8), 35, 3, 30, polymereNoir);
    }

    // Viseurs inversés
    ImageDrawRectangle(&pistoletImg, 60, 20, 10, 10, acierCulasse);  // Avant
    ImageDrawRectangle(&pistoletImg, 230, 15, 15, 15, acierCulasse); // Arrière

    ExportImage(pistoletImg, "../images/pistolet_placeholder.png");
    UnloadImage(pistoletImg);
    CloseWindow();
    return 0;
}