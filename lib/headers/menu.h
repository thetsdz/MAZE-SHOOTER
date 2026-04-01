/** 
* \version 1.0
* \author Hugues Astier
* \date 11/02.2026
* \brief Contient les fonctions de gestion du menu principal
*/

#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "types.h"

/**
*   \brief Gere en fonction de si les touches haut,bas,droite,gauche ou entrée sont utilisées
*   \param <Game screen **currentScreen> etat de l'ecran à modifier en question
*/
void GestionClavier(GameScreen ** currentScreen);

/**
*   \brief  Gère en fonction de si les boutons lancer partie, OPTION, QUITTER sont cliqués
*   \param <Game screen **currentScreen> etat de l'ecran actuel à modifier en question du clique
*   \param <Rectangle rect>, rect represente un des boutons (plus precisement sa zone)
    \param <int indice bouton>
*/
void GestionSouris(GameScreen **currentScreen, Rectangle rect, int indice_bouton);


/**
*   \brief  Gère le menu, ie, gestion de la logique de l'etat de l'ecran grace à GestionSouris(..) et GestionClavier(..) puis dessins des boutons 
*   \param <Game screen *currentScreen> Etat de l'ecran actuel 
*/
void GererMenu(GameScreen *currentScreen);

#endif
