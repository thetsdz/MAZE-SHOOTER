/**
 * \file multijoueur.c
 * \brief Contient les fonctions de gestion du mode multijoueur
 * \author Corentin Jammes
 * \version 1.0
 * \date 12.02.2026
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
/* temporairement en pause
// --- VARIABLES STATIQUES POUR LA SAISIE D'IP ---
static bool saisieIP = false;
static char ipTampon[20] = {
    0};  // Tampon pour stocker l'IP (ex: "192.168.1.15")

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
  UpdatePlayer(joueur, blocks, camera, ennemi);

  // --- GESTION DU TIR LOCAL ---
  bool jeTire = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

  if (jeTire) {
    if (joueur->ammo > 0) {
      joueur->ammo--;
      // On calcule la direction exacte depuis la caméra (prend en compte la
      // hauteur/pitch)
      Vector3 dir = Vector3Subtract(camera->target, camera->position);
      dir = Vector3Normalize(dir);
      ShootProjectile(projs, camera->position, dir, OWNER_PLAYER);
    } else {
      jeTire = false;
    }
  }

  if (IsKeyPressed(KEY_R)) {
    joueur->ammo = joueur->maxAmmo;
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
    joueur->ammo = joueur->maxAmmo;
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

      ShootProjectile(projs, originTir, directionTir, OWNER_REMOTE_PLAYER);
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
  UpdateProjectiles(projs, blocks, ennemi, joueur, &scoreTemp);
}

void DessinerLobbyMultijoueur(ReseauState* netState) {
  DrawText("MODE MULTIJOUEUR", 100, 100, 30, DARKGRAY);

  if (netState->socket == -1) {
    if (saisieIP) {
      // Affichage du menu de saisie
      DrawText("Entrez l'IP du serveur:", 100, 200, 20, BLACK);

      // Dessiner la boîte de saisie
      DrawRectangleLines(100, 230, 300, 40, DARKGRAY);
      DrawText(ipTampon, 110, 240, 20, MAROON);

      // Curseur clignotant
      if ((GetTime() * 2.0f) - (int)(GetTime() * 2.0f) < 0.5f) {
        DrawText("_", 110 + MeasureText(ipTampon, 20), 240, 20, MAROON);
      }

      DrawText("[ENTRÉE] Valider   [ECHAP] Annuler", 100, 300, 20, GRAY);
    } else {
      // Affichage du menu normal
      DrawText("[H] HEBERGER une partie (Serveur)", 100, 200, 20, BLACK);
      DrawText("[C] REJOINDRE une partie (Client)", 100, 240, 20, BLACK);
      DrawText("[BACKSPACE] Retour Menu Principal", 100, 500, 20, GRAY);
    }
  } else {
    // En attente de connexion (après validation ou hébergement)
    if (netState->isServer) {
      DrawText(TextFormat("IP Locale: %s - Port: 30000", "0.0.0.0"), 100, 160,
               20, BLUE);
      DrawText("En attente d'un adversaire...", 100, 350, 20, MAROON);
    } else {
      DrawText(TextFormat("Connexion à %s...", ipTampon), 100, 350, 20,
               DARKGREEN);
    }
    DrawText("[BACKSPACE] Annuler", 100, 500, 20, GRAY);
  }
}

void partie_multijoueur(Entity* player, Entity* remotePlayer,
                        Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                        Projectile projs[MAX_PROJ], Camera3D* camera,
                        ReseauState* netState, bool* jeuInitialise, int* score,
                        GameScreen* currentScreen) {
  // 1. LOBBY (Si on n'est pas encore connecté)
  if (!netState->connected) {
    // Si on n'est PAS en train de saisir l'IP
    if (!saisieIP) {
      // Choix H : Héberger
      if (IsKeyPressed(KEY_H) && netState->socket == -1) {
        netState->socket = InitServeur(30000);
        netState->isServer = 1;
      }

      // Choix C : Activer le mode saisie
      if (IsKeyPressed(KEY_C) && netState->socket == -1) {
        saisieIP = true;
        memset(ipTampon, 0, sizeof(ipTampon));  // Vider le tampon
      }

      // Retour Menu depuis Lobby (Uniquement si on ne saisit pas)
      if (IsKeyPressed(KEY_BACKSPACE)) {
        if (netState->socket != -1) FermerReseau(netState->socket);
        netState->socket = -1;
        netState->isServer = 0;
        *currentScreen = MENU;
      }
    }
    // Si on EST en train de saisir l'IP
    else {
      // Capture des caractères (Chiffres et points)
      int key = GetCharPressed();
      while (key > 0) {
        // Accepter uniquement chiffres (48-57) et point (46)
        if ((key >= 48 && key <= 57) || key == 46) {
          int len = strlen(ipTampon);
          if (len < 15) {  // Limite taille IPv4
            ipTampon[len] = (char)key;
            ipTampon[len + 1] = '\0';
          }
        }
        key = GetCharPressed();
      }

      // Effacer (Backspace)
      if (IsKeyPressed(KEY_BACKSPACE)) {
        int len = strlen(ipTampon);
        if (len > 0) {
          ipTampon[len - 1] = '\0';
        }
      }

      // Annuler la saisie (Echap)
      if (IsKeyPressed(KEY_ESCAPE)) {
        saisieIP = false;
      }

      // Valider (Entrée)
      if (IsKeyPressed(KEY_ENTER)) {
        // Si le champ est vide, on met localhost par défaut
        if (strlen(ipTampon) == 0) strcpy(ipTampon, "127.0.0.1");

        netState->socket = InitClient(ipTampon, 30000);
        netState->isServer = 0;
        saisieIP = false;  // On quitte le mode saisie

        // Si connect réussit, on lance !
        if (netState->socket != -1) {
          netState->connected = 1;
          // Init jeu pour le Client
          InitMultijoueur(player, remotePlayer, 0);
          srand(42);
          init_lab(blocks);
          creer_lab(blocks);
          srand(time(NULL));

          InitProjectiles(projs);
          *score = 0;
          *jeuInitialise = true;
          DisableCursor();
        }
      }
    }

    // Si on est serveur, on vérifie si un client arrive (Code existant)
    if (netState->isServer && netState->socket != -1) {
      int clientSock = AttendreClient(netState->socket);
      if (clientSock != -1) {
        FermerReseau(netState->socket);
        netState->socket = clientSock;
        netState->connected = 1;

        InitMultijoueur(player, remotePlayer, 1);
        srand(42);
        init_lab(blocks);
        creer_lab(blocks);
        srand(time(NULL));

        InitProjectiles(projs);
        *score = 0;
        *jeuInitialise = true;
        DisableCursor();
      }
    }
  }
  // 2. JEU EN COURS (Code existant)
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
                         Texture2D viseur, Texture2D armeTex, int score,
                         ReseauState* netState) {
  if (!netState->connected) {
    // --- DESSIN DU LOBBY (Appel de la nouvelle fonction) ---
    DessinerLobbyMultijoueur(netState);
  } else {
    // --- DESSIN JEU MULTI ---
    // Code existant pour le jeu...
    UpdateDessinGame(remotePlayer, blocks, *camera, projs, score, *player,
                     viseur, armeTex);

    DrawText(TextFormat("POS: X: %.2f | Y: %.2f | Z: %.2f", player->pos.x,
                        player->pos.y, player->pos.z),
             10, 130, 20, GREEN);

    const char* role = netState->isServer ? "SERVEUR" : "CLIENT";
    DrawText(TextFormat("[%s] Ping: -- ms", role), 10, 160, 20, YELLOW);
  }
}


*/