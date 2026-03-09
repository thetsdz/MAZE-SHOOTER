#include <raylib.h>

int main() {
    InitWindow(100, 100, "Generateur Grenade");

    // Canvas plus petit et carré pour une grenade
    int w = 200;
    int h = 200;
    Image grenadeImg = GenImageColor(w, h, BLANK);

    Color vertMilitaire = (Color){ 60, 80, 40, 255 };
    Color metalGris     = (Color){ 80, 80, 85, 255 };
    Color securiteRouge = (Color){ 200, 50, 50, 255 };

    // 1. Corps de la grenade (Ovale/Rectangle arrondi)
    ImageDrawCircle(&grenadeImg, 100, 110, 60, vertMilitaire);
    
    // 2. Tête de la grenade (Le détonateur)
    ImageDrawRectangle(&grenadeImg, 80, 40, 40, 30, metalGris);

    // 3. Le Levier (La cuillère)
    ImageDrawRectangle(&grenadeImg, 110, 45, 10, 80, metalGris);

    // 4. L'anneau de sécurité (Goupille)
    ImageDrawCircleLines(&grenadeImg, 70, 50, 15, metalGris);

    ExportImage(grenadeImg, "../images/grenade_placeholder.png");

    UnloadImage(grenadeImg);
    CloseWindow();
    return 0;
}