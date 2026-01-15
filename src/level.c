#include "level.h"

// Initialisation de la carte
void InitLevel(Block *blocks) {
    // --- Blocs d'obstacles classiques ---
    // Syntaxe : {{x, y, z}, largeur, hauteur, profondeur, couleur}
    blocks[0] = (Block){{2, 0.5f, 2}, 2, 1, 2, DARKGRAY};
    blocks[1] = (Block){{-3, 1.0f, 4}, 3, 1, 3, DARKGRAY};
    blocks[2] = (Block){{5, 1.5f, -2}, 2, 1, 2, DARKGRAY};
    blocks[3] = (Block){{-2, 2.0f, -3}, 4, 1, 2, DARKGRAY};
    blocks[4] = (Block){{0, 3.0f, 5}, 2, 1, 2, DARKGRAY};
    
    // --- Murs de bordure (Arène) ---
    // Ce sont des murs très longs (50 unités) pour fermer la zone
    blocks[5] = (Block){{0, 1.5f, -25}, 50, 3, 1, GRAY};
    blocks[6] = (Block){{0, 1.5f, 25}, 50, 3, 1, GRAY};
    blocks[7] = (Block){{-25, 1.5f, 0}, 1, 3, 50, GRAY};
    blocks[8] = (Block){{25, 1.5f, 0}, 1, 3, 50, GRAY};
    
    // --- Plafond invisible ---
    // Fade(COLOR, 0.0f) rend la couleur totalement transparente
    // Utile pour empêcher le joueur de sortir si la gravité s'inverse ou bug
    blocks[9] = (Block){{0, 10.0f, 0}, 50, 1, 50, Fade(LIGHTGRAY, 0.0f)};
}

void DrawLevel(Block *blocks) {
    // Dessine un sol infini vert pour se repérer
    DrawPlane((Vector3){0,0,0}, (Vector2){50,50}, GREEN);
    // Dessine une grille par-dessus pour mieux voir la perspective
    DrawGrid(100, 1.0f);
    
    // Boucle d'affichage simple des blocs
    for(int i=0; i<NUM_BLOCKS; i++){
        DrawCube(blocks[i].pos, blocks[i].width, blocks[i].height, blocks[i].depth, blocks[i].color);
        // Optionnel : DrawCubeWires pour voir les arêtes
    }
}