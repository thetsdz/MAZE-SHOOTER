#ifndef MULTIJOUEUR_H
#define MULTIJOUEUR_H

#include "reseau.h"
#include "types.h"

// Initialise les positions de départ opposées
void InitMultijoueur(Entity* joueur, Entity* ennemi, int estServeur);

// La boucle principale du mode multi
void UpdateMultijoueur(Entity* joueur, Entity* ennemi,
                       Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Projectile projs[MAX_PROJ], Camera3D* camera,
                       ReseauState* reseau);

// Fonction pour gérer la partie multijoueur (lobby + jeu)
void partie_multijoueur(Entity* player, Entity* remotePlayer,
                        Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                        Projectile projs[MAX_PROJ], Camera3D* camera,
                        ReseauState* netState, bool* jeuInitialise, int* score,
                        GameScreen* currentScreen);

void DessinerLobbyMultijoueur(ReseauState* netState);

void DessinerMultijoueur(Entity* player, Entity* remotePlayer,
                         Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                         Projectile projs[MAX_PROJ], Camera3D* camera,
                         Texture2D viseur, Texture2D armeTex, int score,
                         ReseauState* netState, Model skyModel,
                         Texture2D wallTex, Texture2D floorTex, Model botModel);
#endif
