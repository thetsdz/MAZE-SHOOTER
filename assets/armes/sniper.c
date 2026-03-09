


#include <raylib.h>

int main() {
    InitWindow(100, 100, "Generateur Sniper");
    int w = 800;
    int h = 250;
    Image sniperImg = GenImageColor(w, h, BLANK); 

    Color metalNoir   = (Color){ 30, 30, 30, 255 };  
    Color metalGris   = (Color){ 60, 60, 65, 255 };  
    Color boisSombre  = (Color){ 80, 50, 40, 255 };  
    Color bleuViseur  = (Color){ 50, 150, 255, 100 };

    // --- Dessin Inversé (Canon à gauche) ---

    // 1. La Crosse (Stock) - Déplacée à DROITE
    ImageDrawRectangle(&sniperImg, 580, 80, 200, 80, boisSombre);
    ImageDrawRectangle(&sniperImg, 760, 80, 20, 80, metalNoir); 

    // 2. Le Corps (Receiver)
    ImageDrawRectangle(&sniperImg, 330, 80, 250, 60, metalGris);

    // 3. Le Canon (Barrel) - Vers la GAUCHE
    ImageDrawRectangle(&sniperImg, 30, 95, 300, 20, metalNoir);
    // Frein de bouche au bout à gauche
    ImageDrawRectangle(&sniperImg, 10, 90, 20, 30, metalNoir);

    // 4. La LUNETTE (Scope) - Inversée
    ImageDrawRectangle(&sniperImg, 340, 30, 180, 40, metalNoir); // Tube
    ImageDrawRectangle(&sniperImg, 510, 25, 30, 50, metalNoir);  // Oculaire (arrière)
    ImageDrawRectangle(&sniperImg, 310, 20, 40, 60, metalNoir);  // Objectif (avant)
    ImageDrawRectangle(&sniperImg, 315, 25, 5, 50, bleuViseur);  // Reflet lentille

    // 5. La détente
    ImageDrawRectangle(&sniperImg, 510, 140, 60, 10, metalNoir);
    
    // 6. Le verrou (Bolt)
    ImageDrawRectangle(&sniperImg, 490, 70, 10, 20, metalNoir);

    ExportImage(sniperImg, "../images/sniper_placeholder.png");
    UnloadImage(sniperImg);
    CloseWindow();
    return 0;
}