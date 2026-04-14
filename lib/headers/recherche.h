#ifndef RECHERCHE_H
#define RECHERCHE_H

/**
 * \file recherche.h
 * \brief Fonctions de recherche de chemin et de ligne de vue.
 * Ce module contient les fonctions pour déterminer si un ennemi peut voir le
 * joueur et pour trouver le chemin optimal dans le labyrinthe.
 *   - IsPlayerVisible() : Vérifie la ligne de vue entre deux points en tenant
 * des murs.
 *   - GetNextStepBFS() : Implémente l'algorithme de parcours en largeur pour
 * trouver la prochaine étape vers une cible dans le labyrinthe.
 * \author Corentin Jammes
 * \date 14/04/2026
 * \version 1.0
 */

#include "types.h"

/**
 * \brief Vérifie la ligne de vue entre deux points en tenant compte des murs.
 * \param start Position de départ (ex: bot)
 * \param end Position d'arrivée (ex: joueur)
 * \param blocks Le labyrinthe représenté par une matrice de blocs
 * \return true si la ligne de vue est dégagée, false si un mur bloque la
 * vue
 */
bool IsPlayerVisible(Vector3 start, Vector3 end, Block blocks[NUM_BLOCKS][
                        NUM_BLOCKS]);

/**
 * \brief Trouve la case suivante pour avancer vers la cible dans le labyrinthe.
 * \param start Position de départ
 * \param target Position de la cible
 * \param blocks Le labyrinthe représenté par une matrice de blocs
 * \return La coordonnée de la prochaine case à atteindre
 */
Coord GetNextStepBFS(Coord start, Coord target,
                          Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

#endif  // RECHERCHE_H