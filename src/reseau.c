// --- SECTION IMPORTANTE POUR WINDOWS ---
// On doit définir ces macros AVANT d'inclure winsock2.h pour éviter
// que Windows ne définisse des symboles comme Rectangle, CloseWindow,
// ShowCursor qui entrent en conflit avec Raylib.
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI   // Désactive les fonctions graphiques GDI (Rectangle, etc.)
#define NOUSER  // Désactive les fonctions fenêtrage (CloseWindow, ShowCursor,
                // etc.)
#endif

#include "../lib/headers/reseau.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction utilitaire pour rendre une socket non-bloquante (Portable)
void SetNonBlocking(int sock) {
#ifdef _WIN32
  // Version Windows : On utilise ioctlsocket
  unsigned long mode = 1;
  ioctlsocket(sock, FIONBIO, &mode);
#else
  // Version Linux/Mac : On utilise fcntl
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif
}

int InitServeur(int port) {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("Echec initialisation Winsock\n");
    return -1;
  }
#endif

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return -1;

  int opt = 1;
  // Correction Windows : cast (const char*)&opt
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("Erreur Bind");
    return -1;
  }

  if (listen(sock, 1) < 0) {
    return -1;
  }

  SetNonBlocking(sock);

  printf("[RESEAU] Serveur ouvert sur le port %d\n", port);
  return sock;
}

int InitClient(const char* adresseIP, int port) {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    printf("Echec initialisation Winsock\n");
    return -1;
  }
#endif
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) return -1;

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (inet_pton(AF_INET, adresseIP, &addr.sin_addr) <= 0) {
    return -1;
  }

  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    // En mode non-bloquant ou erreur, connect peut retourner -1.
    // Ici on simplifie, on assume que l'erreur est gérée plus haut ou ignorée
    // pour l'instant perror("Erreur Connect"); return -1;
  }

  SetNonBlocking(sock);
  printf("[RESEAU] Connecte au serveur %s\n", adresseIP);
  return sock;
}

int AttendreClient(int socketServeur) {
  struct sockaddr_in clientAddr;

#ifdef _WIN32
  int len = sizeof(clientAddr);
#else
  socklen_t len = sizeof(clientAddr);
#endif

  // Sur Linux, &len est maintenant correctement typé (socklen_t*). 
  // Sur Windows, il reste (int*).
  int clientSock = accept(socketServeur, (struct sockaddr*)&clientAddr, &len);

  if (clientSock >= 0) {
    printf("[RESEAU] Joueur connecte !\n");
    SetNonBlocking(clientSock);
  }
  return clientSock;
}

void EnvoyerPaquet(int socket, PaquetReseau* paquet) {
  // Correction Windows : cast (const char*)
  send(socket, (const char*)paquet, sizeof(PaquetReseau), 0);
}

int RecevoirPaquet(int socket, PaquetReseau* paquet) {
  // Correction Windows : cast (char*)
  int recus = recv(socket, (char*)paquet, sizeof(PaquetReseau), 0);
  if (recus == sizeof(PaquetReseau)) {
    return 1;
  }
  return 0;
}

void FermerReseau(int socket) {
  if (socket >= 0) {
#ifdef _WIN32
    closesocket(socket);
    WSACleanup();
#else
    close(socket);
#endif
  }
}

int InitUDPBroadcastSender(void) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) return -1;

  int opt = 1;
  // Autorise l'envoi en broadcast
  setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt));
  SetNonBlocking(sock);
  
  return sock;
}

void EnvoyerBroadcast(int sock, int port) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_BROADCAST; // 255.255.255.255

  const char* msg = "MAZE_HOST"; // Notre mot de passe de reconnaissance
  sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(addr));
}

int InitUDPBroadcastListener(int port) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) return -1;

  int opt = 1;
  // Permet à plusieurs programmes de réutiliser le port si besoin
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY; // Ecoute sur toutes les interfaces

  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    return -1;
  }
  
  SetNonBlocking(sock);
  return sock;
}

int RecevoirBroadcast(int sock, char* ipSortie) {
  char buffer[32];
  struct sockaddr_in senderAddr;
  
#ifdef _WIN32
  int len = sizeof(senderAddr);
#else
  socklen_t len = sizeof(senderAddr);
#endif

  int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&senderAddr, &len);
  if (n > 0) {
    buffer[n] = '\0';
    // Si c'est bien notre jeu qui diffuse
    if (strcmp(buffer, "MAZE_HOST") == 0) {
      // On convertit l'adresse IP binaire en texte lisible
      strcpy(ipSortie, inet_ntoa(senderAddr.sin_addr));
      return 1;
    }
  }
  return 0;
}