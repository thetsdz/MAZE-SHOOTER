#ifndef DESSIN_H
#define DESSIN_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 11/02/2026
 * \brief Permet de faire le dessin du jeu (3D + UI) à chaque frame
 */

/** \version 2.0
 * \author Thomas Dequirez
 * \date 11/03/2026
 * \brief ajout de nouvelles textures (mur,sol ciel et bot).
 */

/** \version 3000000000289a1030000001a
.0
 * \author Hugues Astier
 * \date 17/03/2026
 * \brief Ajout de nouvelles textures (bot,projectiles,armes), avec texture du
 * bot qui pivote pour suivre le joueur. Deplacement de la fonction
 * DrawProjectiles dans dessin.c, avec des modifications pour que les
 * projectiles suivent le viseur.
 */

/** \version 4.0
 * \author Thomas Dequirez
 * \date 20/03/2026
 * \brief ajout de la minimap
 */

/** \version 5.0
 * \author Hugues Astier
 * \date 17/03/2026
 * \brief Changement de la prise en compte du score (passer en champs de entité)
 * en vue de changer le changement d'arme
 */

 /** \version 5.5
 * \author Hugues Astier
 * \date 09/04/2026
 * \brief ajout de l'inventaire d'arme
 */

#include "types.h"

/**
 * @brief permet de dessiner les projectiles à chaque frame
 * @param projs Pointeur vers le tableau de projetiles
 * @param tabModels tableau de texture des differrents projectiles (trié)
 */
void DrawProjectiles(Projectile* projs, Model tabProjModels[]);

/**
 * @brief Gere l'affichage de la map en haut à droite
 * @param Player Entité Joueur
 * @param Bot Entite Bot
 * @param heal Tableau des objets de soin et d'armes
 * @param blocks Labyrinthe
 * @param boss Pointeur vers l'entité du boss (pour dessiner sa position)
 * @param IsBossAlive Indique si le boss est vivant ou non (pour ne pas le dessiner s'il est mort)
 */
void minimap(Entity player, Entity bot[18], Heal heal[10],
             Block blocks[NUM_BLOCKS][NUM_BLOCKS], Entity* boss,
             bool IsBossAlive);

/**
 * @brief Dessine une alerte pour indiquer que le joueur a triché
 * @param screenWidth Largeur de l'écran (pour centrer l'alerte)
 * @param timerTriche Chronomètre pour l'écran de triche
 */
void DrawTricheur(int screenWidth);

/**
 * @brief permet de faire le dessin du jeu (3D + UI) à chaque frame
 * @param bot Pointeur vers l'entité du bot (pour dessiner sa position)
 * @param heal Pointeur vers le tableau des objets de soin et d'armes (pour les dessiner)
 * @param blocks Grille de blocs du niveau (pour dessiner les murs et couloirs)
 * @param camera Caméra 3D (pour dessiner la scène en perspective)
 * @param projs Tableau de projectiles actifs (pour les dessiner)
 * @param player Entité du joueur (pour afficher les munitions)
 * @param viseur Texture du viseur à dessiner au centre de l'écran
 * @param tabArmes Tableau des modèles 3D des armes (pour les dessiner dans l'inventaire)
 * @param healModel Modèle 3D de l'objet de soin (pour les dessiner les objets de soin)
 * @param skyModel Modèle 3D du ciel (pour dessiner le ciel)
 * @param wallModel Modèle 3D des murs (pour dessiner les murs)
 * @param floorModel Modèle 3D du sol (pour dessiner le sol)
 * @param botModel Modèle 3D des bots (pour dessiner les bots)
 * @param tabProjModels Tableau des modèles 3D des projectiles (pour les dessiner)
 * @param boss Pointeur vers l'entité du boss (pour dessiner sa position)
 * @param IsBossAlive Indique si le boss est vivant ou non (pour ne pas le dessiner s'il est mort)
 * @param bossModel Modèle 3D du boss (pour dessiner le boss)
 * @param iconesArmes Tableau des textures des icones des armes (pour dessiner les icones dans l'inventaire)
 */
void UpdateDessinGame(Entity bot[18], Heal heal[10],
                      Block blocks[NUM_BLOCKS][NUM_BLOCKS], Camera3D camera,
                      Projectile projs[MAX_PROJ], Entity player,
                      Texture2D viseur, Model tabArmes[], Model healModel,
                      Model skyModel, Model wallModel, Model floorModel,
                      Model botModel, Model tabProjModels[], Entity* boss,
                      bool IsBossAlive, Model bossModel,Texture2D iconesArmes[]);
#endif