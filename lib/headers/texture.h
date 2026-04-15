/**
 * \file texture.h
 * \brief Déclarations pour la gestion des assets graphiques (modèles,
 * textures).
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "raylib.h"
#include "types.h"

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