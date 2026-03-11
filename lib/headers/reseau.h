#ifndef RESEAU_H
#define RESEAU_H

#include "types.h"

/** \version 1.0
 *  \author Corentin Jammes
 *  \date 02/02/2026
 *  \brief Toutes les fonctions de reseau nécessaires pour le mode multijoueur
 */

/**
 * \brief Prépare le serveur (Hôte) sur un port donné
 * \param <int port> le port sur lequel le serveur écoute
 * \return le socket du serveur
 */
int InitServeur(int port);

/**
 * \brief Tente de se connecter à une adresse IP (Client)
 * \param <const char* adresseIP> l'adresse IP du serveur a rejoindre
 * \param <int port> le port vers lequel envoyer les infos 
 * \return le socket du client
 */
int InitClient(const char* adresseIP, int port);

/**
 * \brief Vérifie si quelqu'un essaie de se connecter (Uniquement pour le serveur)
 * \param <int socketServeur> le socket du serveur
 * \return 1 si un client tente de se connecter, 0 sinon
 */
int AttendreClient(int socketServeur);

/**
 * \brief Envoie mes infos à l'autre
 * \param <int socket> le socket de la communication
 * \param <PaquetReseau* paquet> le paquet à envoyer
 */
void EnvoyerPaquet(int socket, PaquetReseau* paquet);

/**
 * \brief Reçoit les infos de l'autre
 * \param <int socket> le socket de la communication
 * \param <PaquetReseau* paquet> le paquet à recevoir
 * \return 1 si un paquet est reçu, 0 sinon
 */
int RecevoirPaquet(int socket, PaquetReseau* paquet);

/**
 * \brief Coupe la communication
 * \param <int socket> le socket de la communication
 */
void FermerReseau(int socket);
#endif