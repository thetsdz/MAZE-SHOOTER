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





// Booleens
#define VRAI 1
#define FAUX 0

// Dimensions du labyrinthe: N lignes et M colonnes (attention: toujours impairs)
#define N 100
#define M 100

// Constantes representant le contenu des cases
#define COULOIR 0
#define MUR -1
#define CHEMIN -2



//******************************
// Partie creation du labyrinthe
//******************************

void init_lab(int lab[N][M]){
//Initialise le labyrinthe avec des murs 
	int i,j;

	for(i=0;i<N;i++)
		for(j=0;j<M;j++)
			lab[i][j] = MUR;
}

int valides(int i, int j){
// renvoie VRAI si i et j designent une case de la matrice
	return(i>=0 && i<N &&j>=0 && j<M);
}

int est_vide(int i, int j, int lab[N][M]){
// renvoie VRAI si i et j designent une case couloir
	return(valides(i,j) && lab[i][j]==COULOIR);
}

int est_mur(int i, int j, int lab[N][M]){
// renvoie VRAI si i et j designent une case mur
	return(valides(i,j) && lab[i][j]==MUR);
}

int blocage(int i, int j, int lab[N][M]){
// renvoie VRAI si aucune case voisine n'est un mur
	return (!est_mur(i+2,j,lab) && !est_mur(i-2,j,lab)
		&& !est_mur(i,j+2,lab) && !est_mur(i,j-2,lab));
}

void creer_lab(int lab[N][M]){
// Cree le labyrinthe de maniere aleatoire avec une pile

	int termine=FAUX;	// booleen indiquant si labyrinthe est termine
	int trouve=FAUX;	// booleen indiquant si une case voisine MUR a ete trouvee
	int i,j,alea;		// alea permet de prendre une case voisine au hasard

	init_lab(lab);
	initpile();

	i=0, 
	j=0;
	lab[0][0]=COULOIR;

   while(!termine){

	if(blocage(i,j,lab)){
	// cas de blocage: repartir de la derniere case visitee, stockee dans la pile
		if(!pilevide()){
			depiler(&j);
			depiler(&i);
		}
		else
			termine=VRAI;
	}
	else{
	// cas general: chercher une case voisine contenant un mur, abattre ce mur et aller sur cette case
		trouve=FAUX;
		while(!trouve){
			alea=rand()%4; 
			switch(alea){
				case 0: 
					if(est_mur(i+2,j,lab)){ 
						empiler(i);
						empiler(j);
						lab[i+1][j]=COULOIR;
						lab[i+2][j]=COULOIR;
						i=i+2;
						trouve=VRAI;
						break;
					}	
				case 1: 	
					if(est_mur(i-2,j,lab)){
						empiler(i);
						empiler(j);
						lab[i-1][j]=COULOIR;
						lab[i-2][j]=COULOIR;
						i=i-2;
						trouve=VRAI;
						break;
					}	
				case 2: 	
					if(est_mur(i,j+2,lab)){
						empiler(i);
						empiler(j);
						lab[i][j+1]=COULOIR;
						lab[i][j+2]=COULOIR;
						j=j+2;
						trouve=VRAI;
						break;
					}	
				case 3: 	
					if(est_mur(i,j-2,lab)){
						empiler(i);
						empiler(j);
						lab[i][j-1]=COULOIR;
						lab[i][j-2]=COULOIR;
						j=j-2;
						trouve=VRAI;
					}	
			}
		}
	}
   }

}