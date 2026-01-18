#include <raylib.h>

int main() {
    // Nécessaire pour utiliser les fonctions d'image
    InitWindow(100, 100, "Generateur");

    // 1. Canvas : On fait une image plus grande (600x400) et TRANSPARENTE
    int w = 600;
    int h = 400;
    Image armeImg = GenImageColor(w, h, BLANK);

    // --- Palette de couleurs "Métal" ---
    // On définit nos propres gris pour faire plus "arme"
    Color metalSombre = (Color){ 40, 40, 50, 255 };  // Presque noir
    Color metalMoyen  = (Color){ 80, 80, 90, 255 };  // Gris acier
    Color metalClair  = (Color){ 120, 120, 130, 255 }; // Gris canon
    Color poigneeGrip = (Color){ 30, 30, 30, 255 };  // Grip caoutchouc

    // --- Dessin par couches (du fond vers l'avant) ---
    // Imagine que tu poses des briques LEGO

    // Couche 1 : La poignée (Grip) - Rectangle vertical en bas
    // x=150, y=200, largeur=80, hauteur=180
    ImageDrawRectangle(&armeImg, 150, 200, 80, 180, poigneeGrip);

    // Couche 2 : Le corps principal (Receiver) - Gros bloc central
    // x=100, y=120, largeur=320, hauteur=100
    ImageDrawRectangle(&armeImg, 100, 120, 320, 100, metalMoyen);

    // Couche 3 : Le canon (Barrel) - Plus fin, devant
    // x=420, y=140, largeur=160, hauteur=60
    ImageDrawRectangle(&armeImg, 420, 140, 160, 60, metalClair);

    // --- Détails ---

    // Un bout de crosse à l'arrière pour équilibrer
    ImageDrawRectangle(&armeImg, 50, 130, 50, 80, metalSombre);

    // Le bout du canon (Cache-flamme)
    ImageDrawRectangle(&armeImg, 580, 135, 20, 70, metalSombre);
    
    // Un chargeur qui dépasse un peu en bas
    ImageDrawRectangle(&armeImg, 250, 220, 60, 100, metalSombre);

    // Un petit rail/viseur au dessus du corps
    ImageDrawRectangle(&armeImg, 180, 100, 150, 20, metalSombre);
    // Point rouge sur le viseur
    ImageDrawRectangle(&armeImg, 300, 90, 20, 10, RED);


    // 6. Export direct dans le dossier assets
    // Cela va ÉCRASER l'ancienne image moche
    ExportImage(armeImg, "../../assets/weapon_placeholder.png");

    // Nettoyage
    UnloadImage(armeImg);
    CloseWindow();
    
    return 0;
}
