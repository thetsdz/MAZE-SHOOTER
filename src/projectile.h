#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "types.h"
#include "../lib/linux/raylib-5.5_linux_amd64/include/raymath.h"
#include <stdio.h> // Nécessaire pour le type FILE*

// Initialise le tableau de projectiles (tous inactifs au début)
void InitProjectiles(Projectile *projs);

// Trouve un slot libre et tire un projectile dans la direction où regarde le joueur
void ShootProjectile(Projectile *projs, Player p);

// Met à jour la position, la durée de vie et les collisions de tous les tirs actifs
void UpdateProjectiles(Projectile *projs, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Vector3 *targetPos, float targetRadius, int *score);

// Affiche les projectiles actifs
void DrawProjectiles(Projectile *projs);

#endif