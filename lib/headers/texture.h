/**
 * \file texture.h
 * \brief Déclarations pour la gestion des assets graphiques (modèles,
 * textures).
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "raylib.h"

/**
 * \brief Contient tous les assets graphiques du jeu (modèles 3D et textures).
 */
typedef struct {
    Model wallModel;
    Model floorModel;
    Texture2D wallTex;
    Texture2D floorTex;
    Model skyModel;
    Model botModel;
    Model bossModel;
    Model healModel;
    Model tabArmes[4];
    Texture2D iconesArmes[4];
    Model tabProjModels[5];
    Texture2D viseur;
} GameAssets;

/**
 * \brief Charge l'ensemble des assets graphiques du jeu.
 * \param assets Pointeur vers la structure à remplir.
 */
void LoadGameAssets(GameAssets *assets);

/**
 * \brief Décharge l'ensemble des assets graphiques du jeu.
 * \param assets Pointeur vers la structure à décharger.
 */
void UnloadGameAssets(GameAssets *assets);

/**
 * \brief Recharge les modèles/textures du niveau selon le thème sélectionné.
 *        Décharge les anciens avant de charger les nouveaux.
 * \param assets Pointeur vers la structure contenant les assets du niveau.
 */
void RechargerTheme(GameAssets *assets);

#endif /* TEXTURE_H */