#ifndef ARME_H
#define ARME_H


#include "types.h"
#include "raymath.h"
#include <stdio.h>


/** \version 1.0
 * \author Astier Hugues
 * \date 15/03/2026
 * \brief permet d'obtenir la fiche technique de chaque arme, et de changer d'armes 
 */



 /** \brief Charge toutes les données de l'arme
 *   \param type type de l'arme
 */

ModeleArme ObtenirModeleArme(TypeArme type);


/** \brief Change l'arme du joueur.
 * Le joueur peut changer d'arme en appuyant sur les touches F1 à F4, à
 * condition d'avoir débloqué l'arme correspondante avec son score.
 * \param joueur Le joueur dont on veut changer l'arme
 */
void ChangementArme(Entity* joueur);


#endif
