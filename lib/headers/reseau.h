#ifndef RESEAU_H
#define RESEAU_H

#include "types.h"

/** \version 1.0
 *  \author Corentin Jammes
 *  \date 02/02/2026
 *  \brief Toutes les fonctions de reseau nécessaires pour le mode multijoueur
 */
/** \version 1.1
 * \author Corentin Jammes
 * \date 12.02.2026
 * \brief corrections pour la compatibilité Windows (cast des options de socket,
 * typage de len pour accept, etc.)
 */
/** \version 2.0
 * \author Corentin Jammes
 * \date 18.03.2026
 * \brief modification du fonctionnement pour pouvoir realiser une recherche
 * broadcast facilement
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
int InitClient(const char *adresseIP, int port);

/**
 * \brief Vérifie si quelqu'un essaie de se connecter (Uniquement pour le
 * serveur)
 * \param <int socketServeur> le socket du serveur
 * \return 1 si un client tente de se connecter, 0 sinon
 */
int AttendreClient(int socketServeur);

/**
 * \brief Envoie mes infos à l'autre
 * \param <int socket> le socket de la communication
 * \param <PaquetReseau* paquet> le paquet à envoyer
 */
void EnvoyerPaquet(int socket, PaquetReseau *paquet);

/**
 * \brief Reçoit les infos de l'autre
 * \param <int socket> le socket de la communication
 * \param <PaquetReseau* paquet> le paquet à recevoir
 * \return 1 si un paquet est reçu, 0 sinon
 */
int RecevoirPaquet(int socket, PaquetReseau *paquet);

/**
 * \brief Coupe la communication
 * \param <int socket> le socket de la communication
 */
void FermerReseau(int socket);

/**
 * \brief Prépare un socket pour envoyer des Broadcasts UDP (Pour l'Hôte)
 */
int InitUDPBroadcastSender(void);

/**
 * \brief Envoie un message de présence sur le réseau local
 */
void EnvoyerBroadcast(int sock, int port);

/**
 * \brief Prépare un socket pour écouter les Broadcasts UDP (Pour le Client)
 */
int InitUDPBroadcastListener(int port);

/**
 * \brief Vérifie si un message de serveur a été reçu et extrait son IP
 * \param ipSortie Tableau de char (ex: char ip[20]) qui contiendra l'IP trouvée
 * \return 1 si un serveur est trouvé, 0 sinon
 */
int RecevoirBroadcast(int sock, char *ipSortie);
#endif