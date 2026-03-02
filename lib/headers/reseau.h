#ifndef RESEAU_H
#define RESEAU_H

#include "types.h"

// Prépare le serveur (Hôte) sur un port donné
int InitServeur(int port);

// Tente de se connecter à une adresse IP (Client)
int InitClient(const char* adresseIP, int port);

// Vérifie si quelqu'un essaie de se connecter (Uniquement pour le serveur)
int AttendreClient(int socketServeur);

// Envoie mes infos à l'autre
void EnvoyerPaquet(int socket, PaquetReseau* paquet);

// Reçoit les infos de l'autre (Renvoie 1 si reçu, 0 si rien)
int RecevoirPaquet(int socket, PaquetReseau* paquet);

// Coupe la communication
void FermerReseau(int socket);
#endif