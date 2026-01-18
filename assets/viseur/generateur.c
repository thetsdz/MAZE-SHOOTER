#include <raylib.h>

int main() {
    // On initialise une fenêtre (nécessaire même si on ne l'affiche pas vraiment)
    InitWindow(100, 100, "Generateur");

    // 1. Créer une image vide de 64x64 pixels
    // BLANK = Transparent (0,0,0,0)
    Image image = GenImageColor(64, 64, BLANK);

    // 2. On dessine dessus (Pixel art)
    // Cercle rouge
    ImageDrawCircleLines(&image, 32, 32, 28, RED);
    
    // Lignes du viseur
    ImageDrawLine(&image, 32, 10, 32, 54, RED); // Verticale
    ImageDrawLine(&image, 10, 32, 54, 32, RED); // Horizontale
    
    // Point central jaune
    ImageDrawCircle(&image, 32, 32, 2, YELLOW);

    // 3. Sauvegarder en PNG (Format universel que Raylib adore)
    ExportImage(image, "crosshair.png");

    // Nettoyage
    UnloadImage(image);
    CloseWindow();
    
    return 0;
}
