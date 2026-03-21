/**
 * \file multijoueur.c
 */

#include "../lib/headers/multijoueur.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lib/headers/asset.h"
#include "../lib/headers/dessin.h"
#include "../lib/headers/level.h"
#include "../lib/headers/log.h"
#include "../lib/headers/player.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/arme.h"
#include "../lib/headers/updategame.h"
#include "../lib/headers/audio.h"


// --- VARIABLES STATIQUES POUR LE BROADCAST ---
static bool rechercheEnCours = false;
static int udpSocket = -1;
static float dernierBroadcast = 0.0f;
#define PORT_BROADCAST 30001

void InitMultijoueur(Entity* joueur, Entity* ennemi, int estServeur) {
  // --- Logique de Spawn Opposé ---
  InitPlayer(joueur);
  InitPlayer(ennemi);
  float coinHautGauche = 1.5f;
  float coinBasDroite =
      (float)NUM_BLOCKS - 1.5f;  // pour ne pas être DANS le mur du fond

  if (estServeur) {
    // Le joueur hébergeant commence en Haut-Gauche
    joueur->pos = (Vector3){coinHautGauche, 1.0f, coinHautGauche};
    joueur->yaw = 0.0f;  // Regarde vers le sud

    // L'ennemi (Client) commence théoriquement en Bas-Droite
    ennemi->pos = (Vector3){coinBasDroite, 1.0f, coinBasDroite};
  } else {
    // Le Client commence en Bas-Droite
    joueur->pos = (Vector3){coinBasDroite, 1.0f, coinBasDroite};
    joueur->yaw = PI;  // Regarde vers le nord (demi-tour)

    // L'ennemi (Serveur) est en Haut-Gauche
    ennemi->pos = (Vector3){coinHautGauche, 1.0f, coinHautGauche};
  }

  joueur->type = ENTITY_PLAYER;
  ennemi->type = ENTITY_REMOTE_PLAYER;
}

void UpdateMultijoueur(Entity* joueur, Entity* ennemi,
                       Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Projectile projs[MAX_PROJ], Camera3D* camera,
                       ReseauState* reseau) {
  // 1. Mise à jour de MON joueur (Clavier/Souris + Collisions locales)
  UpdatePlayer(joueur, blocks, camera, &ennemi);

  // --- GESTION DU TIR LOCAL ---
  bool jeTire = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

  if (jeTire) {
    if (joueur->ammo > 0) {
      joueur->ammo--;
      // On calcule la direction exacte depuis la caméra (prend en compte la
      // hauteur/pitch)
      Vector3 dir = Vector3Subtract(camera->target, camera->position);
      dir = Vector3Normalize(dir);
      ShootProjectile(projs, camera->position, dir, OWNER_PLAYER, joueur->armeEquipee,joueur->yaw, joueur->pitch);
    } else {
      jeTire = false;
    }
  }
  ChangementArme(joueur);
  if (IsKeyPressed(KEY_R)) {
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    PlayReload();
  }


// Debug : Se suicider pour tester le respawn
if (IsKeyPressed(KEY_N)) {
  joueur->health -= 20;
}

if (IsKeyPressed(KEY_M)) {
  joueur->pos = ennemi->pos;  // Téléportation pour tester les collisions
}

  // 2. Je prépare le paquet
  PaquetReseau paquetEnvoi;
  paquetEnvoi.pos = joueur->pos;
  paquetEnvoi.yaw = joueur->yaw;
  paquetEnvoi.pitch = joueur->pitch;
  paquetEnvoi.tir = jeTire ? 1 : 0;
  paquetEnvoi.estMort = 0;

  // Gestion mort locale
  if (joueur->health <= 0) {
    paquetEnvoi.estMort = 1;  // On prévient l'autre qu'on est mort

    // Respawn Local
    joueur->health = joueur->maxHealth;
    joueur->ammo = joueur->armeEquipee.munitionsMax;
    if (reseau->isServer) {
      joueur->pos = (Vector3){1.5f, 10.0f, 1.5f};
    } else {
      joueur->pos =
          (Vector3){(float)NUM_BLOCKS - 1.5f, 10.0f, (float)NUM_BLOCKS - 1.5f};
    }
    // Mise à jour immédiate de la caméra
    camera->position =
        (Vector3){joueur->pos.x, joueur->pos.y + 0.5f, joueur->pos.z};
    camera->target = Vector3Add(
        camera->position, (Vector3){sinf(joueur->yaw), 0, cosf(joueur->yaw)});
  }

  // 3. J'envoie le paquet
  EnvoyerPaquet(reseau->socket, &paquetEnvoi);

  // 4. RÉCEPTION (Correction: boucle WHILE pour lire TOUS les paquets en
  // attente)
  PaquetReseau paquetRecu;
  // Utiliser WHILE permet de ne pas rater le tir si plusieurs paquets arrivent
  // en même temps
  while (RecevoirPaquet(reseau->socket, &paquetRecu)) {
    ennemi->pos = Vector3Lerp(ennemi->pos, paquetRecu.pos, 0.2f);
    ennemi->yaw = paquetRecu.yaw;
    ennemi->pitch =
        paquetRecu
            .pitch;  // Optionnel : utile si tu veux animer le buste de l'ennemi

    if (paquetRecu.tir) {
      Vector3 originTir = ennemi->pos;
      originTir.y += 0.5f;

      // DIRECTION CORRIGÉE : Utilise le yaw ET le pitch reçu
      Vector3 directionTir = {sinf(paquetRecu.yaw) * cosf(paquetRecu.pitch),
                              sinf(paquetRecu.pitch),
                              cosf(paquetRecu.yaw) * cosf(paquetRecu.pitch)};

      ShootProjectile(projs, originTir, directionTir, OWNER_REMOTE_PLAYER,ennemi->armeEquipee,ennemi->yaw, ennemi->pitch);
      TraceLog(LOG_INFO, "Tir ennemi reçu et créé !");
    }
  }

  if (paquetRecu.estMort) {
    TraceLog(LOG_INFO, "L'ennemi est mort !");
  }

  // 5. Physique des balles et Collisions
  // 'ennemi' est passé comme 'autre' pour que MES balles le touchent
  // 'joueur' est passé comme 'player' pour que SES balles me touchent
  int scoreTemp = 0;
  UpdateProjectiles(projs, blocks, &ennemi, joueur, &scoreTemp);
}

void DessinerLobbyMultijoueur(ReseauState* netState) {
  DrawText("MODE MULTIJOUEUR", 100, 100, 30, WHITE);

  if (netState->socket == -1 && !rechercheEnCours) {
      // Affichage du menu normal
      DrawText("[H] HEBERGER une partie (Serveur)", 100, 200, 20, WHITE);
      DrawText("[C] REJOINDRE une partie (Recherche locale)", 100, 240, 20, WHITE);
      DrawText("[BACKSPACE] Retour Menu Principal", 100, 500, 20, WHITE);
  } else {
    if (netState->isServer) {
      DrawText("Hébergement en cours...", 100, 160, 20, BLUE);
      DrawText("En attente d'un adversaire sur le réseau...", 100, 350, 20, WHITE);
    } else if (rechercheEnCours) {
      // Animation des petits points pour faire patienter
      int points = (int)(GetTime() * 3.0f) % 4;
      const char* texteRecherche = TextFormat("Recherche d'un serveur local%s", 
                                  points == 1 ? "." : points == 2 ? ".." : points == 3 ? "..." : "");
      DrawText(texteRecherche, 100, 350, 20, DARKGREEN);
    }
    DrawText("[BACKSPACE] Annuler", 100, 500, 20, WHITE);
  }
}

void partie_multijoueur(Entity* player, Entity* remotePlayer,
                        Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                        Projectile projs[MAX_PROJ], Camera3D* camera,
                        ReseauState* netState, bool* jeuInitialise, int* score,
                        GameScreen* currentScreen) {
  // 1. LOBBY (Si on n'est pas encore connecté)
  if (!netState->connected) {
      
    // Si on n'est ni serveur, ni en train de chercher
    if (!rechercheEnCours && netState->socket == -1) {
      // Choix H : Héberger
      if (IsKeyPressed(KEY_H)) {
        netState->socket = InitServeur(30000);
        netState->isServer = 1;
        // On prépare le lance-voix UDP
        udpSocket = InitUDPBroadcastSender();
        dernierBroadcast = 0.0f;
      }

      // Choix C : Chercher une partie
      if (IsKeyPressed(KEY_C)) {
        rechercheEnCours = true;
        // On prépare les oreilles UDP
        udpSocket = InitUDPBroadcastListener(PORT_BROADCAST);
      }

      if (IsKeyPressed(KEY_BACKSPACE)) {
        *currentScreen = MENU;
      }
    }
    else {
      // --- LOGIQUE SERVEUR (Hébergeur) ---
      if (netState->isServer && netState->socket != -1) {
        // Crier notre présence toutes les secondes
        if (udpSocket != -1 && GetTime() - dernierBroadcast > 1.0f) {
            EnvoyerBroadcast(udpSocket, PORT_BROADCAST);
            dernierBroadcast = GetTime();
        }

        // Vérifier si un client s'est connecté au TCP
        int clientSock = AttendreClient(netState->socket);
        if (clientSock != -1) {
          FermerReseau(netState->socket);
          if (udpSocket != -1) { FermerReseau(udpSocket); udpSocket = -1; } // On arrête de crier
          netState->socket = clientSock;
          netState->connected = 1;

          InitMultijoueur(player, remotePlayer, 1);
          srand(42); init_lab(blocks); creer_lab(blocks); srand(time(NULL));
          InitProjectiles(projs); *score = 0; *jeuInitialise = true; DisableCursor();
        }
      }
      
      // --- LOGIQUE CLIENT (Chercheur) ---
      if (rechercheEnCours && udpSocket != -1) {
        char ipTrouvee[20];
        // Si on entend un serveur
        if (RecevoirBroadcast(udpSocket, ipTrouvee)) {
            TraceLog(LOG_INFO, "Serveur trouvé à l'IP : %s", ipTrouvee);
            FermerReseau(udpSocket); udpSocket = -1; // On arrête d'écouter
            rechercheEnCours = false;

            // On s'y connecte
            netState->socket = InitClient(ipTrouvee, 30000);
            netState->isServer = 0;

            if (netState->socket != -1) {
              netState->connected = 1;
              InitMultijoueur(player, remotePlayer, 0);
              srand(42); init_lab(blocks); creer_lab(blocks); srand(time(NULL));
              InitProjectiles(projs); *score = 0; *jeuInitialise = true; DisableCursor();
            }
        }
      }

      // Annuler l'attente ou la recherche
      if (IsKeyPressed(KEY_BACKSPACE)) {
        if (netState->socket != -1) FermerReseau(netState->socket);
        if (udpSocket != -1) FermerReseau(udpSocket);
        netState->socket = -1;
        udpSocket = -1;
        netState->isServer = 0;
        rechercheEnCours = false;
        *currentScreen = MENU;
      }
    }
  }
  // 2. JEU EN COURS (Le code existant ne change pas en dessous)
  else {
      UpdateMultijoueur(player, remotePlayer, blocks, projs, camera, netState);
      if (IsKeyPressed(KEY_BACKSPACE)) {
          FermerReseau(netState->socket);
          netState->connected = 0;
          netState->socket = -1;
          *currentScreen = MENU;
          *jeuInitialise = false;
      }
  }
}

void DessinerMultijoueur(Entity* player, Entity* remotePlayer,
                         Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                         Projectile projs[MAX_PROJ], Camera3D* camera,
                         Texture2D viseur, Model tabArmes[4], int score,
                         ReseauState* netState, Model skyModel,
                         Model wallModel, Model floorModel,
                         Model botModel, Model tabModels[4]) {
  if (!netState->connected) {
    // --- DESSIN DU LOBBY (Appel de la nouvelle fonction) ---
    DessinerLobbyMultijoueur(netState);
  } else {

    // --- DESSIN JEU MULTI ---
    // Code existant pour le jeu...
    UpdateDessinGame(remotePlayer, blocks, *camera, projs, score, *player,
                     viseur, tabArmes, skyModel,  wallModel,  floorModel, botModel, tabModels);

    DrawText(TextFormat("POS: X: %.2f | Y: %.2f | Z: %.2f", player->pos.x,
                        player->pos.y, player->pos.z),
             10, 130, 20, GREEN);

    const char* role = netState->isServer ? "SERVEUR" : "CLIENT";
    DrawText(TextFormat("[%s] Ping: -- ms", role), 10, 160, 20, YELLOW);
  }
}


