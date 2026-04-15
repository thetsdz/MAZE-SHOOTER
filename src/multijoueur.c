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
#include "../lib/headers/audio.h"
#include "../lib/headers/dessin.h"
#include "../lib/headers/coffre.h"
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

#define PORT_BROADCAST 30001    // Port pour le broadcast
#define MAX_COFFRES_MULTI 10      // Nombre maximum de soins sur la carte en multi
#define VIES_MULTIJOUEUR 25     // Nombre de vies de chaque joueur

// ======================================================================
// CONTEXTE ET ÉTATS DU MULTIJOUEUR
// ======================================================================

// Instance unique (Singleton) contenant tout l'état du module multijoueur
static ContexteMultijoueur ctx = {
    .etat = ETAT_LOBBY_MENU,
    .selectedButton = 0,
    .texteBoutons = {"HEBERGER UNE PARTIE", "REJOINDRE (RECHERCHE LOCALE)", "REJOINDRE (IP MANUELLE)", "RETOUR MENU"},
    .erreurConnexion = false,
    .udpSocket = -1,
    .dernierBroadcast = 0.0f,
    .ipSaisie = "",
    .ipSaisieLen = 0,
    .ping = 0.0f,
    .lastPingSent = 0.0
};

// ======================================================================
// SOUS-FONCTIONS UTILITAIRES
// ======================================================================

// Centralise les coordonnées d'apparition pour éviter de les écrire en double
static Vector3 CalculerPositionSpawn(int estServeur, float hauteur) {
    if (estServeur) {
        return (Vector3){-NUM_BLOCKS + 6.5f, hauteur, -NUM_BLOCKS + 4.5f};
    } else {
        return (Vector3){(float)2 * NUM_BLOCKS - 4.5f, hauteur, (float)2 * NUM_BLOCKS - 4.5f};
    }
}

// Factorisation du code de lancement d'une partie (évite les répétitions)
static void LancerPartieReseau(Entity* player, Entity* remotePlayer, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Coffre* coffre, Projectile projs[MAX_PROJ], int estServeur, bool* jeuInitialise) {
    InitMultijoueur(player, remotePlayer, estServeur);
    srand(42); // Seed commune pour générer le même labyrinthe sur les 2 PCs
    init_lab(blocks);
    creer_lab_multi(blocks);
    for (int i = 0; i < MAX_COFFRES_MULTI; i++) InitCoffre(&coffre[i], blocks);
    srand(time(NULL)); // Reset de la seed pour l'aléatoire normal (tirs, etc.)
    InitProjectiles(projs);
    player->score = 0;
    *jeuInitialise = true;
    DisableCursor();
}

static void AnnulerRechercheOuAttente(ReseauState* netState, GameScreen* currentScreen) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (netState->socket != -1) FermerReseau(netState->socket);
        if (ctx.udpSocket != -1) FermerReseau(ctx.udpSocket);
        netState->socket = -1;
        ctx.udpSocket = -1;
        netState->isServer = 0;
        
        ctx.etat = ETAT_LOBBY_MENU; // Retour au menu principal du lobby
        *currentScreen = MENU;
    }
}

// ======================================================================
// INITIALISATION DU JOUEUR EN MULTI
// ======================================================================

void InitMultijoueur(Entity* joueur, Entity* ennemi, int estServeur) {
  InitPlayer(joueur);
  InitPlayer(ennemi);
  
  joueur->pos = CalculerPositionSpawn(estServeur, 1.0f);
  ennemi->pos = CalculerPositionSpawn(!estServeur, 1.0f); // L'ennemi spawn à l'opposé
  
  joueur->yaw = estServeur ? 0.0f : PI; // Regarde vers le sud ou le nord
  
  joueur->life = VIES_MULTIJOUEUR; 
  ennemi->life = VIES_MULTIJOUEUR;
  joueur->type = ENTITY_PLAYER;
  ennemi->type = ENTITY_REMOTE_PLAYER;
}

// ======================================================================
// SOUS-FONCTIONS POUR LA LOGIQUE EN JEU (UpdateMultijoueur)
// ======================================================================

static void GererActionsLocales(Entity* joueur, Camera3D* camera, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Entity** ennemiPtr, Coffre* coffre, Projectile projs[MAX_PROJ], int coffreUnlockLocal[MAX_COFFRES_MULTI], bool* jeTire) {
    UpdatePlayer(joueur, blocks, camera, ennemiPtr);
    
    int CurrentArme = joueur->armeEquipee.type;
    
    for (int i = 0; i < MAX_COFFRES_MULTI; i++) {
        int armeUnlock = UpdateCoffre(&coffre[i], joueur, blocks);
        if (armeUnlock > 0) {
            coffreUnlockLocal[i] = 1;
            if (armeUnlock == 1) joueur->armeUnlock[1] = 0;
            else if (armeUnlock == 2) joueur->armeUnlock[0] = 0;
            else if (armeUnlock == 3) joueur->armeUnlock[2] = 0;
        }
    }

    if (joueur->chronoTir > 0) joueur->chronoTir -= GetFrameTime();

    *jeTire = (joueur->armeEquipee.type == FUSIL) ? IsMouseButtonDown(MOUSE_BUTTON_LEFT) : IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    
    if (*jeTire && joueur->tabammo[CurrentArme] > 0 && joueur->chronoTir <= 0) {
        joueur->tabammo[CurrentArme]--;
        joueur->chronoTir = joueur->armeEquipee.cadenceTir;
        Vector3 dir = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
        ShootProjectile(projs, camera->position, dir, OWNER_PLAYER, joueur->armeEquipee, joueur->yaw, joueur->pitch);
    } else {
        *jeTire = false;
    }

    ChangementArme(joueur);
    if (IsKeyPressed(KEY_R)) {
        joueur->tabammo[CurrentArme] = joueur->armeEquipee.munitionsMax;
        PlayReload();
    }
}

static void EnvoyerEtatLocal(Entity* joueur, ReseauState* reseau, Camera3D* camera, GameScreen* currentScreen, int coffreUnlockLocal[MAX_COFFRES_MULTI], bool jeTire) {
    PaquetReseau paquetEnvoi = {0};
    paquetEnvoi.pos = joueur->pos;
    paquetEnvoi.yaw = joueur->yaw;
    paquetEnvoi.pitch = joueur->pitch;
    paquetEnvoi.tir = jeTire ? 1 : 0;
    paquetEnvoi.arme = joueur->armeEquipee.type;
    paquetEnvoi.life = joueur->life;
    memcpy(paquetEnvoi.coffreRamasses, coffreUnlockLocal, sizeof(int)*MAX_COFFRES_MULTI);

    if (joueur->health <= 0) {
        paquetEnvoi.estMort = 1;
        joueur->life--;
        paquetEnvoi.life = joueur->life;

        if (joueur->life <= 0) {
            EnvoyerPaquet(reseau->socket, &paquetEnvoi);
            TraceLog(LOG_INFO, "Game Over !");
            if (reseau->socket != -1) FermerReseau(reseau->socket);
            reseau->connected = 0;
            reseau->socket = -1;
            ctx.etat = ETAT_LOBBY_MENU; // Réinitialise l'état pour une prochaine partie
            *currentScreen = GAME_OVER;
            return;
        }

        // Respawn
        joueur->health = joueur->maxHealth;
        joueur->ammo = joueur->armeEquipee.munitionsMax;
        
        // On spawn en l'air (10.0f) pour l'effet de chute
        joueur->pos = CalculerPositionSpawn(reseau->isServer, 10.0f);
        
        camera->position = (Vector3){joueur->pos.x, joueur->pos.y + 0.5f, joueur->pos.z};
        camera->target = Vector3Add(camera->position, (Vector3){sinf(joueur->yaw), 0, cosf(joueur->yaw)});
    }

    EnvoyerPaquet(reseau->socket, &paquetEnvoi);
}

static void GererPingReseau(ReseauState* reseau) {
    double now = GetTime();
    if (now - ctx.lastPingSent > 0.1) {
        PaquetReseau pingPacket = {0};
        pingPacket.timestamp = now;
        pingPacket.isPing = 1;
        EnvoyerPaquet(reseau->socket, &pingPacket);
        ctx.lastPingSent = now;
    }
}

static int RecevoirEtTraiterPaquets(Entity* ennemi, Coffre* coffre, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Projectile projs[MAX_PROJ], ReseauState* reseau, int* infoMortRecue, int* viesAdversaire) {
    PaquetReseau paquetRecu;
    int statutRecu = 0;

    while ((statutRecu = RecevoirPaquet(reseau->socket, &paquetRecu)) == 1) {
        if (paquetRecu.isPing == 1) {
            paquetRecu.isPing = 2;
            EnvoyerPaquet(reseau->socket, &paquetRecu);
            continue;
        }
        if (paquetRecu.isPing == 2) {
            ctx.ping = (float)((GetTime() - paquetRecu.timestamp) * 1000.0);
            continue;
        }

        if (paquetRecu.estMort == 1) {
            *infoMortRecue = 1;
            *viesAdversaire = paquetRecu.life;
        }

        ennemi->pos = Vector3Lerp(ennemi->pos, paquetRecu.pos, 0.2f);
        ennemi->yaw = paquetRecu.yaw;
        ennemi->pitch = paquetRecu.pitch;

        for (int i = 0; i < MAX_COFFRES_MULTI; i++) {
            if (paquetRecu.coffreRamasses[i] == 1) InitCoffre(&coffre[i], blocks);
        }

        ennemi->armeEquipee = ObtenirModeleArme(paquetRecu.arme);

        if (paquetRecu.tir) {
            Vector3 originTir = {ennemi->pos.x, ennemi->pos.y + 0.5f, ennemi->pos.z};
            Vector3 directionTir = {sinf(paquetRecu.yaw) * cosf(paquetRecu.pitch), sinf(paquetRecu.pitch), cosf(paquetRecu.yaw) * cosf(paquetRecu.pitch)};
            ShootProjectile(projs, originTir, directionTir, OWNER_REMOTE_PLAYER, ennemi->armeEquipee, ennemi->yaw, ennemi->pitch);
        }
    }
    return statutRecu;
}

// ======================================================================
// UPDATE GAMEPLAY MULTI
// ======================================================================

void UpdateMultijoueur(Entity* joueur, Entity* ennemi, Coffre* coffre, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Projectile projs[MAX_PROJ], Camera3D* camera, ReseauState* reseau, GameScreen* currentScreen) {
    
    int coffreUnlockLocal[MAX_COFFRES_MULTI] = {0};
    bool jeTire = false;
    Entity* ennemiPtr = ennemi;
    
    GererActionsLocales(joueur, camera, blocks, &ennemiPtr, coffre, projs, coffreUnlockLocal, &jeTire);

    if (IsKeyPressed(KEY_M)) joueur->pos = ennemi->pos; // Cheat debug teleporation

    EnvoyerEtatLocal(joueur, reseau, camera, currentScreen, coffreUnlockLocal, jeTire);
    if (*currentScreen == GAME_OVER) return;

    GererPingReseau(reseau);

    int infoMortRecue = 0;
    int viesAdversaire = 3;
    int statutRecu = RecevoirEtTraiterPaquets(ennemi, coffre, blocks, projs, reseau, &infoMortRecue, &viesAdversaire);

    if (infoMortRecue == 1) {
        joueur->score++;
        TraceLog(LOG_INFO, viesAdversaire > 0 ? "Kill ! Vies restantes adversaire: %d" : "Dernier kill !", viesAdversaire);
    }

    if (statutRecu == -1 || (infoMortRecue == 1 && viesAdversaire <= 0)) {
        TraceLog(LOG_INFO, "Victoire ! L'adversaire est éliminé ou a quitté.");
        if (reseau->socket != -1) FermerReseau(reseau->socket);
        reseau->connected = 0;
        reseau->socket = -1;
        ctx.etat = ETAT_LOBBY_MENU; // Réinitialise l'état
        *currentScreen = VICTOIRE;
        return;
    }

    UpdateProjectiles(projs, blocks, &ennemiPtr, joueur, currentScreen, false, NULL);
}

// ======================================================================
// SOUS-FONCTIONS POUR LE LOBBY (NAVIGATION ET ÉTATS)
// ======================================================================

static void GererMenuLobby(ReseauState* netState, GameScreen* currentScreen) {
    ShowCursor();
    if (IsKeyPressed(KEY_DOWN)) ctx.selectedButton = (ctx.selectedButton + 1) % 4;
    if (IsKeyPressed(KEY_UP)) ctx.selectedButton = (ctx.selectedButton - 1 + 4) % 4;

    float btnW = 500, btnH = 50, posX = (GetScreenWidth() - btnW) / 2.0f, departY = GetScreenHeight() / 2 - 30;
    bool actionTriggered = false;
    int actionIndex = -1;

    if (IsKeyPressed(KEY_ENTER)) {
        actionTriggered = true;
        actionIndex = ctx.selectedButton;
    }

    for (int i = 0; i < 4; i++) {
        Rectangle rect = {posX, departY + i * 70, btnW, btnH};
        if (CheckCollisionPointRec(GetMousePosition(), rect)) {
            ctx.selectedButton = i;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                actionTriggered = true;
                actionIndex = i;
            }
        }
    }

    if (actionTriggered) {
        ctx.erreurConnexion = false;
        switch (actionIndex) {
            case 0: // Héberger
                netState->socket = InitServeur(30000);
                netState->isServer = 1;
                ctx.udpSocket = InitUDPBroadcastSender();
                ctx.dernierBroadcast = 0.0f;
                ctx.etat = ETAT_LOBBY_ATTENTE;
                break;
            case 1: // Recherche Locale
                ctx.udpSocket = InitUDPBroadcastListener(PORT_BROADCAST);
                ctx.etat = ETAT_LOBBY_RECHERCHE;
                break;
            case 2: // IP Manuelle
                ctx.ipSaisie[0] = '\0';
                ctx.ipSaisieLen = 0;
                ctx.etat = ETAT_LOBBY_SAISIE_IP;
                break;
            case 3: // Retour Menu
                *currentScreen = MENU;
                break;
        }
    }
}

static void GererSaisieIPClient(ReseauState* netState, Entity* player, Entity* remotePlayer, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Coffre* coffre, Projectile projs[MAX_PROJ], bool* jeuInitialise) {
    int key = GetCharPressed();
    while (key > 0) {
        if (((key >= '0' && key <= '9') || key == '.') && ctx.ipSaisieLen < 15) {
            ctx.ipSaisie[ctx.ipSaisieLen] = (char)key;
            ctx.ipSaisie[ctx.ipSaisieLen + 1] = '\0';
            ctx.ipSaisieLen++;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (ctx.ipSaisieLen > 0) {
            ctx.ipSaisie[--ctx.ipSaisieLen] = '\0';
        } else {
            ctx.etat = ETAT_LOBBY_MENU; // Annuler la saisie
        }
    }

    if (IsKeyPressed(KEY_ENTER) && ctx.ipSaisieLen > 0) {
        netState->socket = InitClient(ctx.ipSaisie, 30000);
        if (netState->socket != -1) {
            netState->isServer = 0;
            netState->connected = 1;
            ctx.etat = ETAT_EN_JEU;
            LancerPartieReseau(player, remotePlayer, blocks, coffre, projs, 0, jeuInitialise);
        } else {
            ctx.erreurConnexion = true;
            ctx.etat = ETAT_LOBBY_MENU; // Retour au menu pour afficher l'erreur
        }
    }
}

// ======================================================================
// FONCTION PRINCIPALE : LOBBY + JEU
// ======================================================================

void partie_multijoueur(Entity* player, Entity* remotePlayer, Coffre* coffre, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Projectile projs[MAX_PROJ], Camera3D* camera, ReseauState* netState, bool* jeuInitialise, GameScreen* currentScreen) {
    
    switch (ctx.etat) {
        
        case ETAT_LOBBY_MENU:
            GererMenuLobby(netState, currentScreen);
            break;
            
        case ETAT_LOBBY_SAISIE_IP:
            GererSaisieIPClient(netState, player, remotePlayer, blocks, coffre, projs, jeuInitialise);
            break;
            
        case ETAT_LOBBY_ATTENTE:
            if (ctx.udpSocket != -1 && GetTime() - ctx.dernierBroadcast > 1.0f) {
                EnvoyerBroadcast(ctx.udpSocket, PORT_BROADCAST);
                ctx.dernierBroadcast = GetTime();
            }
            int clientSock = AttendreClient(netState->socket);
            if (clientSock != -1) {
                FermerReseau(netState->socket);
                if (ctx.udpSocket != -1) { FermerReseau(ctx.udpSocket); ctx.udpSocket = -1; }
                netState->socket = clientSock;
                netState->connected = 1;
                ctx.etat = ETAT_EN_JEU;
                LancerPartieReseau(player, remotePlayer, blocks, coffre, projs, 1, jeuInitialise);
            }
            AnnulerRechercheOuAttente(netState, currentScreen);
            break;
            
        case ETAT_LOBBY_RECHERCHE:
            if (ctx.udpSocket != -1) {
                char ipTrouvee[20];
                if (RecevoirBroadcast(ctx.udpSocket, ipTrouvee)) {
                    TraceLog(LOG_INFO, "Serveur trouvé : %s", ipTrouvee);
                    FermerReseau(ctx.udpSocket); ctx.udpSocket = -1;
                    
                    netState->socket = InitClient(ipTrouvee, 30000);
                    if (netState->socket != -1) {
                        netState->isServer = 0;
                        netState->connected = 1;
                        ctx.etat = ETAT_EN_JEU;
                        LancerPartieReseau(player, remotePlayer, blocks, coffre, projs, 0, jeuInitialise);
                    } else {
                        ctx.erreurConnexion = true;
                        ctx.etat = ETAT_LOBBY_MENU;
                    }
                }
            }
            AnnulerRechercheOuAttente(netState, currentScreen);
            break;
            
        case ETAT_EN_JEU:
            UpdateMultijoueur(player, remotePlayer, coffre, blocks, projs, camera, netState, currentScreen);

            if (*currentScreen == GAME_OVER || *currentScreen == VICTOIRE || IsKeyPressed(KEY_BACKSPACE)) {
                if (IsKeyPressed(KEY_BACKSPACE) && netState->socket != -1) {
                    FermerReseau(netState->socket);
                    *currentScreen = MENU;
                }
                netState->connected = 0;
                netState->socket = -1;
                ctx.etat = ETAT_LOBBY_MENU;
                *jeuInitialise = false;
            }
            break;
    }
}

// ======================================================================
// FONCTIONS DE DESSIN
// ======================================================================

// ---------------- SOUS-FONCTIONS DE DESSIN DU LOBBY -------------------

static void DessinerLobbyMenu(int sw, int sh) {
    float btnW = 500, btnH = 50, posX = (sw - btnW) / 2.0f, departY = sh / 2 - 30;

    for (int i = 0; i < 4; i++) {
        Rectangle rect = {posX, departY + i * 70, btnW, btnH};
        Color cFond = (i == ctx.selectedButton) ? (Color){50, 50, 80, 255} : (Color){30, 30, 50, 255};
        Color cTexte = (i == ctx.selectedButton) ? WHITE : LIGHTGRAY;
        Color cBord = (i == ctx.selectedButton) ? WHITE : DARKGRAY;

        DrawRectangleRec(rect, cFond);
        DrawRectangleLinesEx(rect, 2, cBord);
        int tW = MeasureText(ctx.texteBoutons[i], 20);
        DrawText(ctx.texteBoutons[i], rect.x + (rect.width - tW) / 2, rect.y + (rect.height - 20) / 2, 20, cTexte);
    }

    if (ctx.erreurConnexion) {
        const char* msgErr = "Erreur : Impossible de se connecter a cette adresse IP ou Serveur injoignable.";
        DrawText(msgErr, sw / 2 - MeasureText(msgErr, 20) / 2, departY + 4 * 70 + 20, 20, RED);
    }

    DrawText("Utilise les fleches et Entree ou la Souris", sw / 2 - MeasureText("Utilise les fleches et Entree ou la Souris", 15) / 2, sh - 50, 15, LIGHTGRAY);
}

static void DessinerLobbySaisieIP(int sw, int sh) {
    const char* titre = "Entrez l'adresse IP publique ou locale du serveur :";
    DrawText(titre, sw / 2 - MeasureText(titre, 20) / 2, sh / 2 - 60, 20, WHITE);

    DrawRectangle(sw / 2 - 150, sh / 2 - 20, 300, 40, DARKGRAY);
    DrawText(ctx.ipSaisie, sw / 2 - 140, sh / 2 - 10, 20, RAYWHITE);

    // Curseur clignotant
    if (((int)(GetTime() * 2)) % 2 == 0) {
        DrawText("_", sw / 2 - 140 + MeasureText(ctx.ipSaisie, 20), sh / 2 - 10, 20, RAYWHITE);
    }

    const char* infoText = "[ENTER] Valider    [BACKSPACE] Effacer / Annuler";
    DrawText(infoText, sw / 2 - MeasureText(infoText, 20) / 2, sh / 2 + 50, 20, GRAY);
    const char* warnText = "Attention: Le jeu va figer quelques secondes pendant la tentative.";
    DrawText(warnText, sw / 2 - MeasureText(warnText, 15) / 2, sh / 2 + 90, 15, RED);
}

static void DessinerLobbyAttente(int sw, int sh) {
    DrawText("Hebergement en cours...", sw / 2 - MeasureText("Hebergement en cours...", 20) / 2, sh / 2 - 30, 20, BLUE);
    DrawText("En attente d'un adversaire sur le reseau...", sw / 2 - MeasureText("En attente d'un adversaire sur le reseau...", 20) / 2, sh / 2 + 20, 20, WHITE);
    DrawText("[BACKSPACE] Annuler", sw / 2 - MeasureText("[BACKSPACE] Annuler", 20) / 2, sh - 100, 20, WHITE);
}

static void DessinerLobbyRecherche(int sw, int sh) {
    int points = (int)(GetTime() * 3.0f) % 4;
    const char* txt = TextFormat("Recherche d'un serveur local%s", points==1 ? "." : points==2 ? ".." : points==3 ? "..." : "");
    DrawText(txt, sw / 2 - MeasureText(txt, 20) / 2, sh / 2, 20, DARKGREEN);
    DrawText("[BACKSPACE] Annuler", sw / 2 - MeasureText("[BACKSPACE] Annuler", 20) / 2, sh - 100, 20, WHITE);
}

// ======================================================================
// FONCTION PRINCIPALE DE DESSIN DU LOBBY
// ======================================================================

void DessinerLobbyMultijoueur(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("MODE MULTIJOUEUR", sw / 2 - MeasureText("MODE MULTIJOUEUR", 30) / 2, sh / 4 - 50, 30, WHITE);

    switch(ctx.etat) {
        case ETAT_LOBBY_MENU:      DessinerLobbyMenu(sw, sh);      break;
        case ETAT_LOBBY_SAISIE_IP: DessinerLobbySaisieIP(sw, sh);  break;
        case ETAT_LOBBY_ATTENTE:   DessinerLobbyAttente(sw, sh);   break;
        case ETAT_LOBBY_RECHERCHE: DessinerLobbyRecherche(sw, sh); break;
        default: break;
    }
}

// ======================================================================
// FONCTION DE DESSIN EN JEU (LOBBY + PARTIE)
// ======================================================================

void DessinerMultijoueur(Entity* player, Entity* remotePlayer, Coffre* coffre,
                         Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                         Projectile projs[MAX_PROJ], Camera3D* camera,
                         Texture2D viseur, Model tabArmes[4],
                         Model coffreModel, Model skyModel,
                         Model wallModel, Model floorModel, Model botModel,
                         Model tabModels[4], Texture2D iconesArmes[]) {
                         
  if (ctx.etat != ETAT_EN_JEU) {
    DessinerLobbyMultijoueur();
  } else {
    Entity dummyBots[18] = {0};
    dummyBots[0] = *remotePlayer;

    UpdateDessinGame(dummyBots, coffre, blocks, *camera, projs, *player, viseur,
                     tabArmes, coffreModel, skyModel, wallModel, floorModel,
                     botModel, tabModels, NULL, false, botModel, iconesArmes);
                     
    DrawText(TextFormat("Ping: %.0f ms", ctx.ping), 10, GetScreenHeight() - 30, 20, YELLOW);
  }
}