#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

/**
 *  \version 1.0
 *  \author Corentin Jammes
 *  \date 15/01/2026
 *  \brief Fichier de sauvegarde en clair dans un fichier txt
 */

/**
 *  \version 2.0
 *  \author Corentin Jammes
 *  \date 05/02/2026
 *  \brief Fichier de sauvegarde sécurisé (Checksum + RC4) pour eviter la triche
 */
/**
 *  \version 2.1
 *  \author Corentin Jammes
 *  \date 02/03/2026
 *  \brief Fichier de sauvegarde sécurisé (Checksum + RC4) pour eviter la triche
et ajout du bot dans le fichier de sauvegarde
 */

/**
*  \version 2.2
*  \author Hugues Astier
*  \date 02/03/2026
*  \brief Changement du fonctionnement de, la sauvegarde du
 score (passer en champs de entity)
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

/** @brief Sauvegarde les données du joueur dans un fichier texte et l'encrypte
 * directement
 * \param <Entity* player> récupère les infos du joueur
 * \param <Entity* bot> récupère les infos du bot
 * \param <int* score> récupère le score de la partie
 */
void sauvegarder(Entity* player, Entity bot[18], Entity* boss, bool IsBossAlive,
                 Heal heal[10]);

/** @brief Charge les données cryptés du joueur depuis un fichier texte
 *   \param <Entity* player> charge les infos du joueur
 *   \param <Entity* bot> charge les infos du bot
 *   \param <int* score> charge le score de la partie
 */
int chargerSauvegarde(Entity* player, Entity bot[18], Entity* boss,
<<<<<<< HEAD
                       bool* IsBossAlive, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Heal heal[10]);
=======
                      bool* IsBossAlive, Heal heal[10]);
>>>>>>> master

#endif