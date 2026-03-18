#ifndef UPDATEGAME_H
#define UPDATEGAME_H

#include "types.h"

/**
 * @brief permet de faire la logique du jeu à chaque frame (mouvements, tirs, collisions, etc.)
 * @param player Pointeur vers l'entité du joueur (pour mettre à jour sa position, munitions, etc.)
 * @param bot Pointeur vers l'entité du bot (pour mettre à jour sa position, IA, etc.)
 * @param blocks Grille de blocs du niveau (pour gérer les collisions avec les murs)
 * @param projs Tableau de projectiles actifs (pour mettre à jour leur position, gérer les collisions, etc.)
 * @param score Pointeur vers le score du joueur (pour le mettre à jour lors des tirs réussis, etc.)
 * @param camera Pointeur vers la caméra 3D (pour l'affichage joueur et les calculs de direction de tir)
 */


void ChangementArme(Entity *joueur);

void UpdateGame(Entity* player, Entity* bot,
                Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                Projectile projs[MAX_PROJ], int* score, Camera3D* camera);

#endif