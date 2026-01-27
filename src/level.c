#include "level.h"
#include "pile.h"   
#include <stdlib.h>
#include <time.h>

//initialisation du labyrinthe composé de bloc géant
void init_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS]){
    float offset = NUM_BLOCKS - 1; // on gardera le centre plus tard
    for (int i=0; i<NUM_BLOCKS; i++) {
        for (int j=0; j<NUM_BLOCKS; j++) {
            blocks[i][j].pos = (Vector3){ i*3 - offset, 1.0f, j*3 - offset }; // Centre du cube = i*2 et j*2 pour doubler la taille
            blocks[i][j].width  = 3.0f; //largeur du bloc
            blocks[i][j].height = 6.0f; //hauteur du bloc
            blocks[i][j].depth  = 3.0f; //profondeur du bloc
            blocks[i][j].color = BLACK; //couleur du bloc
            blocks[i][j].isWall = 1; //est-ce que le bloc est un mur
        }
    }
}

//verifie si la case est valide
static int valides(int i, int j){
    return i > 0 && i < NUM_BLOCKS-1 && j > 0 && j < NUM_BLOCKS-1;
}

// --------------------------------------------------
// Génération du labyrinthe (DFS)
// --------------------------------------------------
void creer_lab(Block blocks[NUM_BLOCKS][NUM_BLOCKS])
{
    int i = 1, j = 1;

    initpile();
    blocks[i][j].isWall = 0;
    blocks[i][j].color = BLANK;
    empiler(i,j);

    while(!pilevide()) {

        int voisins[4][2] = {
            { i+2, j },
            { i-2, j },
            { i, j+2 },
            { i, j-2 }
        };

        int candidats[4][2];
        int n = 0;

        for(int k=0; k<4; k++) {
            int ni = voisins[k][0];
            int nj = voisins[k][1];

            if(valides(ni,nj) && blocks[ni][nj].isWall) {
                candidats[n][0] = ni;
                candidats[n][1] = nj;
                n++;
            }
        }

        if(n > 0) {
            int r = rand() % n;
            int ni = candidats[r][0];
            int nj = candidats[r][1];

            // Mur intermédiaire
            int mi = (i + ni)/2;
            int mj = (j + nj)/2;

            blocks[mi][mj].isWall = 0;
			blocks[mi][mj].pos.y = -20.0f;
            blocks[mi][mj].color = BLANK;
            blocks[ni][nj].isWall = 0;
            blocks[ni][nj].color = BLANK;
			blocks[ni][nj].pos.y = -20.0f;

            empiler(i,j);
            i = ni; j = nj;
        }
        else {
            Coord c = depiler();
            i = c.i; j = c.j;
        }
    }
}

// --------------------------------------------------
// Affichage
// --------------------------------------------------
void DrawLevel(Block blocks[NUM_BLOCKS][NUM_BLOCKS])
{
    DrawPlane((Vector3){0,0,0}, (Vector2){NUM_BLOCKS*2, NUM_BLOCKS*2}, DARKGREEN);
    DrawGrid(NUM_BLOCKS*2, 1.0f);

    for(int i=0; i<NUM_BLOCKS; i++){
        for(int j=0; j<NUM_BLOCKS; j++){

            Vector3 pos = blocks[i][j].pos;

            if(blocks[i][j].isWall){
                DrawCube(pos,
                         blocks[i][j].width,
                         blocks[i][j].height,
                         blocks[i][j].depth,
                         blocks[i][j].color);
            } else {
                // Dessiner le sol sous les couloirs
                DrawCube((Vector3){pos.x, 0, pos.z}, 2, 0.2f, 2, GRAY);
            }
        }
    }
}
