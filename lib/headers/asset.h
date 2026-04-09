#ifndef ASSET_H
#define ASSET_H

#include "raylib.h"

/**
    \version 1.0
    \brief Charge la texture passé en paramètre (obsolete)
    \param Nom de la texture
    \return La texture chargé
*/
Texture2D ChargerTexture(const char *fileName);

/**
    \version 1.0
    \brief Dessine le viseur
    \param Texture du viseur
    \param Largeur de la fenètre
    \param Hauteur de la fenètre
*/
void DessinerViseur(Texture2D texture, int screenWidth, int screenHeight);

/**
    \version 2.0
    \brief Dessine l'arme on est passé d'une texture 2d à un model 3d
    \param Texture de l'arme
    \param Largeur de la fenètre
    \param Hauteur de la fenètre
*/
void DessinerArme(Model ModelArme, int indexArme);

Model ChargerHeal(void);

#endif