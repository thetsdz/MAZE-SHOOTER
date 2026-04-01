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

/** \version 3.0
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

#include "types.h"

/**
 * @brief permet de dessiner les projectiles à chaque frame
 * @param projs Pointeur vers le tableau de projetiles
 * @param tabModels tableau de texture des differrents projectiles (trié)
 */
void DrawProjectiles(Projectile *projs, Model tabProjModels[]);

/**
 * @brief Gere l'affichage de la map en haut à droite
 * @param Player Entité Joueur
 * @param Bot Entite Bot
 * @param blocks Labyrinthe
 */
void minimap(Entity player, Entity bot[18],Heal heal[10],
             Block blocks[NUM_BLOCKS][NUM_BLOCKS]);

/**
 * @brief permet de faire le dessin du jeu (3D + UI) à chaque frame
 * @param bot Pointeur vers l'entité du bot (pour dessiner sa position)
 * @param blocks Grille de blocs du niveau (pour dessiner les murs et couloirs)
 * @param camera Caméra 3D (pour dessiner la scène en perspective)
 * @param projs Tableau de projectiles actifs (pour les dessiner)
 * @param score Score actuel du joueur (pour l'afficher à l'écran)
 * @param player Entité du joueur (pour afficher les munitions)
 * @param viseur Texture du viseur à dessiner au centre de l'écran
 * @param armeTex Texture de l'arme à dessiner en bas de l'écran
 */
void UpdateDessinGame(Entity bot[18],Heal heal[10], Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                      Camera3D camera, Projectile projs[MAX_PROJ], int score,
                      Entity player, Texture2D viseur, Model tabArmes[],
                      Model skyModel, Model wallModel, Model floorModel,
                      Model botModel, Model tabProjModels[]);
#endif