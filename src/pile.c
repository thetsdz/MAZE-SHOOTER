// Mise en oeuvre d'une pile d'entiers par pointeurs

#include<stdio.h>
#include<stdlib.h>	// nécessaire pour la définition de malloc et free 
#include "pile.h"

// definition d'un element de la pile
typedef struct element{
	int entier; 
	struct element* suivant;
	} t_element;

// declaration de la pile
t_element* pile;

void initpile(void){
// initialise la pile
	pile = NULL;
}

int pilevide(void){
// vrai si la pile est vide, faux sinon
	return (pile == NULL);
}

void empiler(int c){
// empile l'entier c
	t_element* nouv;

	nouv = malloc(sizeof(t_element));
	nouv->entier = c;
	nouv->suivant = pile;
	pile = nouv;
}

void depiler(int * c){
// depile l'entier c
	t_element* sommet;

	if (pile!=NULL){
		*c = pile->entier;
		sommet = pile;
		pile = sommet->suivant;
		free(sommet);
	}
}

