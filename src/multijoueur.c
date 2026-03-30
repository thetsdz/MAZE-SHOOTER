/**
 * \file multijoueur.c
 */

#include "../lib/headers/multijoueur.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lib/headers/arme.h"
#include "../lib/headers/asset.h"
#include "../lib/headers/audio.h"
#include "../lib/headers/dessin.h"
#include "../lib/headers/level.h"
#include "../lib/headers/log.h"
#include "../lib/headers/player.h"
#include "../lib/headers/projectile.h"
#include "../lib/headers/updategame.h"

/*Avec ce code, on peux se connecter en entrant l'IP publique de l'hebergeur.
Cependant, pour que cela fonctionne à travers internet (et pas juste en local),
l'hebergeur doit impérativement ouvrir le port TCP 30000 sur l'interface
d'administration de sa Box Internet / Routeur (c'est ce qu'on appelle la
redirection de port ou le Port Forwarding NAT).

Si l'hôte n'ouvre pas ce port sur sa box, sa box bloquera ta tentative de
connexion par sécurité, et la fonction InitClient échouera en retournant -1.*/

// --- VARIABLES STATIQUES POUR LES BOUTONS DU LOBBY ---
static int selectedLobbyButton = 0;
static const char *texteBoutonsLobby[4] = {
    "HEBERGER UNE PARTIE", "REJOINDRE (RECHERCHE LOCALE)",
    "REJOINDRE (IP MANUELLE)", "RETOUR MENU"};

// --- VARIABLES STATIQUES POUR LE BROADCAST ---
static bool rechercheEnCours = false;
static int udpSocket = -1;
static float dernierBroadcast = 0.0f;
#define PORT_BROADCAST 30001

// --- VARIABLES STATIQUES POUR LA SAISIE MANUELLE DE L'IP ---
static bool saisieIPEnCours = false;
static char ipSaisie[20] = "";
static int ipSaisieLen = 0;
static bool erreurConnexion =
    false; // Pour afficher un message si l'IP est injoignable

static float ping = 0.0f;
static double lastPingSent = 0.0;

void InitMultijoueur(Entity *joueur, Entity *ennemi, int estServeur) {
    // --- Logique de Spawn Opposé ---
    InitPlayer(joueur);
    InitPlayer(ennemi);
    if (estServeur) {
        // Le joueur hébergeant commence en Haut-Gauche
        joueur->pos = (Vector3){-NUM_BLOCKS + 6.5f, 1.0f, -NUM_BLOCKS + 4.5f};
        joueur->yaw = 0.0f; // Regarde vers le sud

        // L'ennemi (Client) commence théoriquement en Bas-Droite
        ennemi->pos = (Vector3){(float)2 * NUM_BLOCKS - 4.5f, 1.0f,
                                (float)2 * NUM_BLOCKS - 4.5f};
    } else {
        // Le Client commence en Bas-Droite
        joueur->pos = (Vector3){(float)2 * NUM_BLOCKS - 4.5f, 1.0f,
                                (float)2 * NUM_BLOCKS - 4.5f};
        joueur->yaw = PI; // Regarde vers le nord (demi-tour)

        // L'ennemi (Serveur) est en Haut-Gauche
        ennemi->pos = (Vector3){-NUM_BLOCKS + 6.5f, 1.0f, -NUM_BLOCKS + 4.5f};
    }

    joueur->type = ENTITY_PLAYER;
    ennemi->type = ENTITY_REMOTE_PLAYER;
}

void UpdateMultijoueur(Entity *joueur, Entity *ennemi,
                       Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Projectile projs[MAX_PROJ], Camera3D *camera,
                       ReseauState *reseau, GameScreen *currentScreen) {
    // 1. Mise à jour de MON joueur (Clavier/Souris + Collisions locales)
    UpdatePlayer(joueur, blocks, camera, &ennemi);
    if (joueur->chronoTir > 0) {
        joueur->chronoTir -= GetFrameTime();
    }
    // --- GESTION DU TIR LOCAL ---
    bool jeTire;

    if (joueur->armeEquipee.type == FUSIL) {
        jeTire = IsMouseButtonDown(MOUSE_BUTTON_LEFT); // Continu
    } else {
        jeTire = IsMouseButtonPressed(MOUSE_BUTTON_LEFT); // Coup par coup
    }
    if (jeTire) {
        if (joueur->ammo > 0 && joueur->chronoTir <= 0) {
            joueur->ammo--;
            joueur->chronoTir =
                joueur->armeEquipee.cadenceTir; // On réinitialise le délai

            // On calcule la direction exacte depuis la caméra (prend en compte
            // la hauteur/pitch)
            Vector3 dir = Vector3Subtract(camera->target, camera->position);
            dir = Vector3Normalize(dir);
            ShootProjectile(projs, camera->position, dir, OWNER_PLAYER,
                            joueur->armeEquipee, joueur->yaw, joueur->pitch);
        } else {
            jeTire = false;
        }
    }
    ChangementArme(joueur);
    if (IsKeyPressed(KEY_R)) {
        joueur->ammo = joueur->armeEquipee.munitionsMax;
        PlayReload();
    }

    /* Debug : Se suicider pour tester le respawn
    if (IsKeyPressed(KEY_N)) {
      joueur->health -= 20;
    }
  */
    if (IsKeyPressed(KEY_M)) {
        joueur->pos = ennemi->pos; // Téléportation pour tester les collisions
    }

    // 2. Je prépare le paquet
    PaquetReseau paquetEnvoi;
    paquetEnvoi.pos = joueur->pos;
    paquetEnvoi.yaw = joueur->yaw;
    paquetEnvoi.pitch = joueur->pitch;
    paquetEnvoi.tir = jeTire ? 1 : 0;
    paquetEnvoi.estMort = 0;
    paquetEnvoi.arme = joueur->armeEquipee.type;
    paquetEnvoi.life = joueur->life; // <-- On indique nos vies

    // Gestion mort locale
    if (joueur->health <= 0) {
        paquetEnvoi.estMort = 1;
        joueur->life--;
        paquetEnvoi.life = joueur->life; // <-- On met à jour avec la vie perdue

        if (joueur->life <= 0) {
            // --- NOUVEAU : On envoie le paquet fatal avant de couper ! ---
            EnvoyerPaquet(reseau->socket, &paquetEnvoi);

            TraceLog(LOG_INFO, "Game Over !");
            if (reseau->socket != -1)
                FermerReseau(reseau->socket);
            reseau->connected = 0;
            reseau->socket = -1;
            *currentScreen = GAME_OVER;
            return;
        }

        // Respawn Local
        joueur->health = joueur->maxHealth;
        joueur->ammo = joueur->armeEquipee.munitionsMax;
        if (reseau->isServer) {
            joueur->pos =
                (Vector3){-NUM_BLOCKS + 6.5f, 10.0f, -NUM_BLOCKS + 4.5f};
        } else {
            joueur->pos = (Vector3){(float)2 * NUM_BLOCKS - 4.5f, 10.0f,
                                    (float)2 * NUM_BLOCKS - 4.5f};
        }
        // Mise à jour immédiate de la caméra
        camera->position =
            (Vector3){joueur->pos.x, joueur->pos.y + 0.5f, joueur->pos.z};
        camera->target =
            Vector3Add(camera->position,
                       (Vector3){sinf(joueur->yaw), 0, cosf(joueur->yaw)});
    }

    // 3. J'envoie le paquet
    EnvoyerPaquet(reseau->socket, &paquetEnvoi);

    double now = GetTime();

    if (now - lastPingSent > 0.1) // toutes les 100 ms
    {
        PaquetReseau pingPacket = {0};
        pingPacket.timestamp = now;
        pingPacket.isPing = 1;

        EnvoyerPaquet(reseau->socket, &pingPacket);

        lastPingSent = now;
    }

    // 4. RÉCEPTION
    PaquetReseau paquetRecu;
    paquetRecu.estMort = 0; // Par sécurité
    int statutRecu = 0;

    // On stocke le résultat de la réception dans statutRecu
    while ((statutRecu = RecevoirPaquet(reseau->socket, &paquetRecu)) == 1) {
        // --- PING ALLER ---
        if (paquetRecu.isPing == 1) {
            paquetRecu.isPing = 2;
            EnvoyerPaquet(reseau->socket, &paquetRecu);
            continue;
        }

        // --- PING RETOUR ---
        if (paquetRecu.isPing == 2) {
            double now = GetTime();
            ping = (float)((now - paquetRecu.timestamp) * 1000.0);
            continue;
        }
        ennemi->pos = Vector3Lerp(ennemi->pos, paquetRecu.pos, 0.2f);
        ennemi->yaw = paquetRecu.yaw;
        ennemi->pitch = paquetRecu.pitch;

        // AJOUT : On équipe l'ennemi distant avec l'arme qu'il a sélectionnée
        ennemi->armeEquipee = ObtenirModeleArme(paquetRecu.arme);

        if (paquetRecu.tir) {
            Vector3 originTir = ennemi->pos;
            originTir.y += 0.5f;

            Vector3 directionTir = {
                sinf(paquetRecu.yaw) * cosf(paquetRecu.pitch),
                sinf(paquetRecu.pitch),
                cosf(paquetRecu.yaw) * cosf(paquetRecu.pitch)};
            ShootProjectile(projs, originTir, directionTir, OWNER_REMOTE_PLAYER,
                            ennemi->armeEquipee, ennemi->yaw, ennemi->pitch);
        }
    }

    // On gagne SI l'autre a quitté (statut -1) OU s'il est mort sans vies
    // restantes
    if (statutRecu == -1 || (paquetRecu.estMort == 1 && paquetRecu.life <= 0)) {
        TraceLog(LOG_INFO, "Victoire ! L'adversaire est éliminé ou a quitté.");

        // Déconnexion propre
        if (reseau->socket != -1)
            FermerReseau(reseau->socket);
        reseau->connected = 0;
        reseau->socket = -1;

        *currentScreen = VICTOIRE;
        return; // On arrête
    }
    // SI l'autre est mort MAIS qu'il lui reste des vies
    else if (paquetRecu.estMort == 1 && paquetRecu.life > 0) {
        TraceLog(LOG_INFO, "Kill ! Il reste %d vies à l'adversaire.",
                 paquetRecu.life);
    }

    // 5. Physique des balles et Collisions
    int scoreTemp = 0;
    UpdateProjectiles(projs, blocks, &ennemi, joueur, &scoreTemp,
                      currentScreen);
}

void DessinerLobbyMultijoueur(ReseauState *netState) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("MODE MULTIJOUEUR",
             sw / 2 - MeasureText("MODE MULTIJOUEUR", 30) / 2, sh / 4 - 50, 30,
             WHITE);

    if (netState->socket == -1 && !rechercheEnCours && !saisieIPEnCours) {
        // --- DESSIN DES BOUTONS ---
        float btnW = 500;
        float btnH = 50;
        float posX = (sw - btnW) / 2.0f;
        float departY =
            sh / 2 - 30; // Même hauteur de départ que le menu principal

        for (int i = 0; i < 4; i++) {
            Rectangle rect = {posX, departY + i * 70, btnW, btnH};

            Color couleurFond;
            Color couleurTexte;
            Color couleurBordure;

            // Apparence selon la sélection
            if (i == selectedLobbyButton) {
                couleurFond = (Color){50, 50, 80, 255}; // Bleu plus clair
                couleurTexte = WHITE;
                couleurBordure = WHITE;
            } else {
                couleurFond = (Color){30, 30, 50, 255}; // Bleu très foncé
                couleurTexte = LIGHTGRAY;
                couleurBordure = DARKGRAY;
            }

            DrawRectangleRec(rect, couleurFond);
            DrawRectangleLinesEx(rect, 2, couleurBordure);

            int tW = MeasureText(texteBoutonsLobby[i], 20);
            DrawText(texteBoutonsLobby[i], rect.x + (rect.width - tW) / 2,
                     rect.y + (rect.height - 20) / 2, 20, couleurTexte);
        }

        if (erreurConnexion) {
            const char *msgErr =
                "Erreur : Impossible de se connecter a cette adresse IP.";
            DrawText(msgErr, sw / 2 - MeasureText(msgErr, 20) / 2,
                     departY + 4 * 70 + 20, 20, RED);
        }

        DrawText(
            "Utilise les flèches et Entrée ou la Souris",
            sw / 2 -
                MeasureText("Utilise les flèches et Entrée ou la Souris", 15) /
                    2,
            sh - 50, 15, LIGHTGRAY);
    } else if (saisieIPEnCours) {
        // --- AFFICHAGE SAISIE IP (Centré) ---
        const char *titre =
            "Entrez l'adresse IP publique ou locale du serveur :";
        DrawText(titre, sw / 2 - MeasureText(titre, 20) / 2, sh / 2 - 60, 20,
                 WHITE);

        DrawRectangle(sw / 2 - 150, sh / 2 - 20, 300, 40, DARKGRAY);
        DrawText(ipSaisie, sw / 2 - 140, sh / 2 - 10, 20, RAYWHITE);

        // Curseur clignotant
        if (((int)(GetTime() * 2)) % 2 == 0) {
            DrawText("_", sw / 2 - 140 + MeasureText(ipSaisie, 20), sh / 2 - 10,
                     20, RAYWHITE);
        }

        const char *infoText =
            "[ENTER] Valider    [BACKSPACE] Effacer / Annuler";
        DrawText(infoText, sw / 2 - MeasureText(infoText, 20) / 2, sh / 2 + 50,
                 20, GRAY);
        const char *warnText = "Attention: Le jeu va figer quelques secondes "
                               "pendant la tentative.";
        DrawText(warnText, sw / 2 - MeasureText(warnText, 15) / 2, sh / 2 + 90,
                 15, RED);
    } else {
        // --- AFFICHAGE CHARGEMENT / RECHERCHE (Centré) ---
        if (netState->isServer) {
            DrawText("Hébergement en cours...",
                     sw / 2 - MeasureText("Hébergement en cours...", 20) / 2,
                     sh / 2 - 30, 20, BLUE);
            DrawText("En attente d'un adversaire sur le réseau...",
                     sw / 2 - MeasureText(
                                  "En attente d'un adversaire sur le réseau...",
                                  20) /
                                  2,
                     sh / 2 + 20, 20, WHITE);
        } else if (rechercheEnCours) {
            int points = (int)(GetTime() * 3.0f) % 4;
            const char *texteRecherche = TextFormat(
                "Recherche d'un serveur local%s", points == 1   ? "."
                                                  : points == 2 ? ".."
                                                  : points == 3 ? "..."
                                                                : "");
            DrawText(texteRecherche,
                     sw / 2 - MeasureText(texteRecherche, 20) / 2, sh / 2, 20,
                     DARKGREEN);
        }
        DrawText("[BACKSPACE] Annuler",
                 sw / 2 - MeasureText("[BACKSPACE] Annuler", 20) / 2, sh - 100,
                 20, WHITE);
    }
}

void partie_multijoueur(Entity *player, Entity *remotePlayer,
                        Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                        Projectile projs[MAX_PROJ], Camera3D *camera,
                        ReseauState *netState, bool *jeuInitialise, int *score,
                        GameScreen *currentScreen) {
    // 1. LOBBY (Si on n'est pas encore connecté)
    if (!netState->connected) {
        // Si on n'est ni serveur, ni en train de chercher, ni en train de taper
        // une IP
        if (!rechercheEnCours && netState->socket == -1 && !saisieIPEnCours) {
            ShowCursor(); // S'assurer que la souris est visible dans ce menu

            // Navigation au clavier
            if (IsKeyPressed(KEY_DOWN))
                selectedLobbyButton = (selectedLobbyButton + 1) % 4;
            if (IsKeyPressed(KEY_UP))
                selectedLobbyButton = (selectedLobbyButton - 1 + 4) % 4;

            float btnW = 500;
            float btnH = 50;
            float posX = (GetScreenWidth() - btnW) / 2.0f;
            float departY = GetScreenHeight() / 2 - 30;

            bool actionTriggered = false;
            int actionIndex = -1;

            // Validation au clavier
            if (IsKeyPressed(KEY_ENTER)) {
                actionTriggered = true;
                actionIndex = selectedLobbyButton;
            }

            // Navigation et validation à la souris
            for (int i = 0; i < 4; i++) {
                Rectangle rect = {posX, departY + i * 70, btnW, btnH};
                if (CheckCollisionPointRec(GetMousePosition(), rect)) {
                    selectedLobbyButton =
                        i; // La souris prend le dessus sur la sélection
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        actionTriggered = true;
                        actionIndex = i;
                    }
                }
            }

            // Exécution de l'action choisie
            if (actionTriggered) {
                switch (actionIndex) {
                case 0: // Héberger
                    netState->socket = InitServeur(30000);
                    netState->isServer = 1;
                    udpSocket = InitUDPBroadcastSender();
                    dernierBroadcast = 0.0f;
                    erreurConnexion = false;
                    break;
                case 1: // Recherche Locale
                    rechercheEnCours = true;
                    udpSocket = InitUDPBroadcastListener(PORT_BROADCAST);
                    erreurConnexion = false;
                    break;
                case 2: // IP Manuelle
                    saisieIPEnCours = true;
                    erreurConnexion = false;
                    ipSaisie[0] = '\0';
                    ipSaisieLen = 0;
                    break;
                case 3: // Retour Menu
                    *currentScreen = MENU;
                    break;
                }
            }
        } else if (saisieIPEnCours) {
            // --- LOGIQUE SAISIE MANUELLE ---
            int key = GetCharPressed();
            while (key > 0) {
                // Autoriser uniquement les chiffres et les points pour une
                // adresse IP (IPv4)
                if (((key >= '0' && key <= '9') || key == '.') &&
                    ipSaisieLen < 15) {
                    ipSaisie[ipSaisieLen] = (char)key;
                    ipSaisie[ipSaisieLen + 1] = '\0';
                    ipSaisieLen++;
                }
                key = GetCharPressed(); // Récupère le prochain caractère s'il y
                                        // en a plusieurs
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (ipSaisieLen > 0) {
                    ipSaisieLen--;
                    ipSaisie[ipSaisieLen] = '\0';
                } else {
                    saisieIPEnCours = false; // Annuler et revenir au choix si
                                             // le champ est vide
                }
            }

            if (IsKeyPressed(KEY_ENTER) && ipSaisieLen > 0) {
                // Tentative de connexion (Peut bloquer l'écran 1 à 2 secondes
                // si l'IP est mauvaise)
                netState->socket = InitClient(ipSaisie, 30000);

                if (netState->socket != -1) {
                    netState->isServer = 0;
                    netState->connected = 1;
                    saisieIPEnCours = false;

                    InitMultijoueur(player, remotePlayer, 0);
                    srand(42);
                    init_lab(blocks);
                    creer_lab_multi(blocks);
                    srand(time(NULL));
                    InitProjectiles(projs);
                    *score = 0;
                    *jeuInitialise = true;
                    DisableCursor();
                } else {
                    // Echec de la connexion
                    erreurConnexion = true;
                    saisieIPEnCours = false;
                }
            }
        } else {
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
                    if (udpSocket != -1) {
                        FermerReseau(udpSocket);
                        udpSocket = -1;
                    } // On arrête de crier
                    netState->socket = clientSock;
                    netState->connected = 1;

                    InitMultijoueur(player, remotePlayer, 1);
                    srand(42);
                    init_lab(blocks);
                    creer_lab_multi(blocks);
                    srand(time(NULL));
                    InitProjectiles(projs);
                    *score = 0;
                    *jeuInitialise = true;
                    DisableCursor();
                }
            }

            // --- LOGIQUE CLIENT (Chercheur) ---
            if (rechercheEnCours && udpSocket != -1) {
                char ipTrouvee[20];
                // Si on entend un serveur
                if (RecevoirBroadcast(udpSocket, ipTrouvee)) {
                    TraceLog(LOG_INFO, "Serveur trouvé à l'IP : %s", ipTrouvee);
                    FermerReseau(udpSocket);
                    udpSocket = -1; // On arrête d'écouter
                    rechercheEnCours = false;

                    // On s'y connecte
                    netState->socket = InitClient(ipTrouvee, 30000);
                    netState->isServer = 0;

                    if (netState->socket != -1) {
                        netState->connected = 1;
                        InitMultijoueur(player, remotePlayer, 0);
                        srand(42);
                        init_lab(blocks);
                        creer_lab_multi(blocks);
                        srand(time(NULL));
                        InitProjectiles(projs);
                        *score = 0;
                        *jeuInitialise = true;
                        DisableCursor();
                    }
                }
            }

            // Annuler l'attente ou la recherche
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (netState->socket != -1)
                    FermerReseau(netState->socket);
                if (udpSocket != -1)
                    FermerReseau(udpSocket);
                netState->socket = -1;
                udpSocket = -1;
                netState->isServer = 0;
                rechercheEnCours = false;
                *currentScreen = MENU;
            }
        }
    } else {
        UpdateMultijoueur(player, remotePlayer, blocks, projs, camera, netState,
                          currentScreen);
        if (IsKeyPressed(KEY_BACKSPACE)) {
            FermerReseau(netState->socket);
            netState->connected = 0;
            netState->socket = -1;
            *currentScreen = MENU;
            *jeuInitialise = false;
        }
    }
}

void DessinerMultijoueur(Entity *player, Entity *remotePlayer,
                         Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                         Projectile projs[MAX_PROJ], Camera3D *camera,
                         Texture2D viseur, Model tabArmes[4], int score,
                         ReseauState *netState, Model skyModel, Model wallModel,
                         Model floorModel, Model botModel, Model tabModels[4]) {
    if (!netState->connected) {
        DessinerLobbyMultijoueur(netState);
    } else {
        Entity dummyBots[18] = {0};
        dummyBots[0] = *remotePlayer;

        UpdateDessinGame(dummyBots, blocks, *camera, projs, score, *player,
                         viseur, tabArmes, skyModel, wallModel, floorModel,
                         botModel, tabModels);
    }
    DrawText(TextFormat("Ping: %.0f ms", ping), 10, GetScreenHeight() - 30, 20,
             YELLOW);
}