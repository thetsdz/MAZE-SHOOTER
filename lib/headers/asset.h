#ifndef ASSET_H
#define ASSET_H

#include "raylib.h"


/** \brief Charge la texture passé en paramètre
    \param Nom de la texture
    \return La texture chargé
*/
Texture2D ChargerTexture(const char *fileName);



/** \brief Dessine le viseur
    \param Texture du viseur
    \param Largeur de la fenètre
    \param Hauteur de la fenètre
*/
void DessinerViseur(Texture2D texture, int screenWidth, int screenHeight);



/** \brief Dessine l'arme
    \param Texture de l'arme
    \param Largeur de la fenètre
    \param Hauteur de la fenètre
*/
void DessinerArme(Texture2D texture, int screenWidth, int screenHeight);

#endif