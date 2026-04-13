#ifndef MULTIJOUEUR_H
#define MULTIJOUEUR_H

#include "reseau.h"
#include "types.h"
/** \version 1.0
 *  \author Corentin Jammes
 *  \date 12/02/2026
 *  \brief Toutes les fonctions du mode multijoueur (Lobby + Gameplay)
 */
/** \version 2.0
 *  \author Corentin Jammes
 *  \date 18/03/2026
 *  \brief ajout d'un broadcast UDP pour la recherche de serveur local
 */
/** \version 3.0
 *  \author Corentin Jammes
 *  \date 20/03/2026
 *  \brief reajout de la possibilité de se connecter via IP manuelle (en plus du
 * broadcast) + refonte de l'interface du lobby pour gérer les 2 méthodes de
 * connexion + ajout de messages d'erreur et d'instructions dans le lobby
 */
 /** \version 3.1
 *  \author Hugues Astier
 *  \date 01/04/2026
 *  \brief Prise en compte du score dans l'entité
 */

/**
 * \brief Initialise les positions de départ opposées
 * \param <Entity* joueur> le joueur local(serveur)
 * \param <Entity* ennemi> l'ennemi distant(client)
 * \param <int estServeur> 1 si le joueur est le serveur, 0 sinon
 */
void InitMultijoueur(Entity *joueur, Entity *ennemi, int estServeur);

/**
 * \brief La boucle principale du mode multi
 * \param <Entity* joueur> le joueur local(serveur)
 * \param <Entity* ennemi> l'ennemi distant(client)
 * \param <Block blocks[NUM_BLOCKS][NUM_BLOCKS]> le tableau des blocs
 * \param <Projectile projs[MAX_PROJ]> le tableau des projectiles
 * \param <Camera3D* camera> la caméra
 * \param <ReseauState* reseau> l'état du réseau
 */
void UpdateMultijoueur(Entity *joueur, Entity *ennemi, Heal* heal,
                       Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Projectile projs[MAX_PROJ], Camera3D *camera,
                       ReseauState *reseau, GameScreen *currentScreen);

/**
 * \brief Fonction pour gérer la partie multijoueur (lobby + jeu)
 * \param <Entity* player> le joueur local
 * \param <Entity* remotePlayer> le joueur distant
 * \param <Block blocks[NUM_BLOCKS][NUM_BLOCKS]> le tableau des blocs
 * \param <Projectile projs[MAX_PROJ]> le tableau des projectiles
 * \param <Camera3D* camera> la caméra
 * \param <ReseauState* netState> l'état du réseau
 * \param <bool* jeuInitialise> un pointeur vers une variable indiquant si le
 * jeu est initialisé

 * \param <GameScreen* currentScreen> un pointeur vers l'écran de jeu actuel
 */
void partie_multijoueur(Entity *player, Entity *remotePlayer,Heal* heal,
                        Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                        Projectile projs[MAX_PROJ], Camera3D* camera,
                        ReseauState* netState, bool* jeuInitialise,
                        GameScreen* currentScreen);

/**
 * \brief Dessine le lobby d'attente multijoueur
 */
void DessinerLobbyMultijoueur();

/**
 * \brief Dessine le mode multijoueur
 * \param <Entity* player> le joueur local
 * \param <Entity* remotePlayer> le joueur distant
 * \param <Block blocks[NUM_BLOCKS][NUM_BLOCKS]> le tableau des blocs
 * \param <Projectile projs[MAX_PROJ]> le tableau des projectiles
 * \param <Camera3D* camera> la caméra
 * \param <Texture2D viseur> la texture du viseur
 * \param <Model tabArmes[4]> les modèles 3D des armes
 * \param <Model healModel> le modèle 3D de l'objet de soin
 * \param <Model skyModel> le modèle 3D du ciel
 * \param <Model wallModel> le modèle 3D des murs
 * \param <Model floorModel> le modèle 3D du sol
 * \param <Model botModel> le modèle 3D des bots (joueurs)
 * \param <Model tabModels[4]> les modèles 3D des armes pour les icones
 * \param <Texture2D iconesArmes[]> les textures des icones des armes
 */

void DessinerMultijoueur(Entity *player,
                            Entity *remotePlayer,
                            Heal* heal,
                            Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                            Projectile projs[MAX_PROJ],
                            Camera3D *camera,
                            Texture2D viseur,
                            Model tabArmes[4],
                            Model healModel,
                            Model skyModel,
                            Model wallModel,
                            Model floorModel,
                            Model botModel,
                            Model tabModels[4],
                            Texture2D iconesArmes[]);
                            


#endif
