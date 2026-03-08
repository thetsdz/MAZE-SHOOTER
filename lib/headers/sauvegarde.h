#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/** @brief Sauvegarde les données du joueur dans un fichier texte et l'encrypte
 * directement*/
void sauvegarder(Entity* player, Entity* bot, int* score);

/** @brief Charge les données cryptés du joueur depuis un fichier texte*/
void chargerSauvegarde(Entity* player, Entity* bot, int* score);

#endif