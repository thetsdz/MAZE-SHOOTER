#ifndef HEAL_H
#define HEAL_H
/** \version 1.0
 * \author Corentin Jammes
 * \date 01/04/2026
 * \brief Implémentation de la logique pour les objets de soin dans le jeu.
 * Ces objets permettent au joueur de récupérer des points de vie lorsqu'il les ramasse.
 */

#include "types.h"

/**
 * @brief Initialise un objet de soin à une position aléatoire dans le labyrinthe.
 * @param heal Pointeur vers l'objet de soin à initialiser.
 * @param blocks Grille de blocs du niveau (pour éviter de placer le soin dans un mur).
 */
void InitHeal(Heal *heal, Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/**
 * @brief Met à jour l'état de l'objet de soin, vérifie si le joueur le ramasse.
 * Si le joueur est suffisamment proche, il récupère des points de vie et le
 * soin est réinitialisé à une nouvelle position.
 * @param heal Pointeur vers l'objet de soin à mettre à jour.
 * @param player Pointeur vers l'entité du joueur (pour vérifier la distance et soigner).
 * @param blocks Grille de blocs du niveau (pour éviter de réinitialiser le soin dans un mur).
 */
void UpdateHeal(Heal *heal, Entity *player, Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

#endif 