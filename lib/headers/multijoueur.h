#ifndef MULTIJOUEUR_H
#define MULTIJOUEUR_H

#include "reseau.h"
#include "types.h"
/** \version 1.0
 *  \author Corentin Jammes
 *  \date 02/02/2026
 *  \brief Toutes les fonctions du mode multijoueur (Lobby + Gameplay)
 */
/**
 * \brief Initialise les positions de départ opposées
 * \param <Entity* joueur> le joueur local(serveur)
 * \param <Entity* ennemi> l'ennemi distant(client)
 * \param <int estServeur> 1 si le joueur est le serveur, 0 sinon
 */
void InitMultijoueur(Entity* joueur, Entity* ennemi, int estServeur);

/**
 * \brief La boucle principale du mode multi
 * \param <Entity* joueur> le joueur local(serveur)
 * \param <Entity* ennemi> l'ennemi distant(client)
 * \param <Block blocks[NUM_BLOCKS][NUM_BLOCKS]> le tableau des blocs
 * \param <Projectile projs[MAX_PROJ]> le tableau des projectiles
 * \param <Camera3D* camera> la caméra
 * \param <ReseauState* reseau> l'état du réseau
 */
void UpdateMultijoueur(Entity* joueur, Entity* ennemi,
                       Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Projectile projs[MAX_PROJ], Camera3D* camera,
                       ReseauState* reseau);

/**
 * \brief Fonction pour gérer la partie multijoueur (lobby + jeu)
 * \param <Entity* player> le joueur local
 * \param <Entity* remotePlayer> le joueur distant
 * \param <Block blocks[NUM_BLOCKS][NUM_BLOCKS]> le tableau des blocs
 * \param <Projectile projs[MAX_PROJ]> le tableau des projectiles
 * \param <Camera3D* camera> la caméra
 * \param <ReseauState* netState> l'état du réseau
 * \param <bool* jeuInitialise> un pointeur vers une variable indiquant si le jeu est initialisé
 * \param <int* score> un pointeur vers le score du joueur
 * \param <GameScreen* currentScreen> un pointeur vers l'écran de jeu actuel
 */
void partie_multijoueur(Entity* player, Entity* remotePlayer,
                        Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                        Projectile projs[MAX_PROJ], Camera3D* camera,
                        ReseauState* netState, bool* jeuInitialise, int* score,
                        GameScreen* currentScreen);

/**
 * \brief Dessine le lobby d'attente multijoueur
 * \param <ReseauState* netState> etat du reseau
 */
void DessinerLobbyMultijoueur(ReseauState* netState);

/**
 * \brief Dessine le mode multijoueur 
 * \param <Entity* player> le joueur local
 */
void DessinerMultijoueur(Entity* player, Entity* remotePlayer,
                         Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                         Projectile projs[MAX_PROJ], Camera3D* camera,
                         Texture2D viseur, Texture2D armeTex, int score,
                         ReseauState* netState, Model skyModel,
                         Texture2D wallTex, Texture2D floorTex, Model botModel);
#endif
