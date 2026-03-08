#include "../lib/headers/sauvegarde.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/headers/cryptage.h"
#include "../lib/headers/types.h"

// CLÉ DE CRYPTAGE (À ne pas partager ou modifié !)
// générer avec : openssl rand -hex 32
static const char* GAME_KEY =
    "a998cd54ac641b3bf1a48369f52fb4ec9e3efffd835525e0578130c1068de2bd";

// On crée une structure qui contient toutes les données qu'on veut sauvegarder
typedef struct {
  int score;
  int maxAmmo;
  float x, y, z;
  float yaw, pitch;
  int onGround;
  int ammo;
  int health;
  int maxHealth;
  int life;
  Entity bot;
} SaveData;

// C'est ce bloc qui sera écrit sur le disque : Données + Sécurité
typedef struct {
  SaveData gameData;  // Tes données de jeu
  uint32_t checksum;  // L'empreinte de sécurité
} SaveFile;

void sauvegarder(Entity* player, Entity* bot, int* score) {
  SaveFile save;

  // Remplissage de la structure de données
  save.gameData.score = *score;
  save.gameData.maxAmmo = player->maxAmmo;

  save.gameData.x = player->pos.x;
  save.gameData.y = player->pos.y;
  save.gameData.z = player->pos.z;

  save.gameData.yaw = player->yaw;
  save.gameData.pitch = player->pitch;

  save.gameData.onGround = player->onGround;
  save.gameData.ammo = player->ammo;

  save.gameData.health = player->health;
  save.gameData.maxHealth = player->maxHealth;
  save.gameData.life = player->life;

  save.gameData.bot = *bot;

  // Calcul du Checksum (Sur les données EN CLAIR)
  save.checksum =
      calculate_checksum((unsigned char*)&save.gameData, sizeof(SaveData));

  // Cryptage de TOUT le fichier (Données + Checksum)
  // On crypte l'ensemble de la structure 'SaveFile'
  rc4_crypt((unsigned char*)&save, sizeof(SaveFile), GAME_KEY,
            strlen(GAME_KEY));

  // Écriture Binaire
  // Note le "wb" (Write Binary) au lieu de "w"
  FILE* fw = fopen("save.dat", "wb");
  if (fw) {
    fwrite(&save, sizeof(SaveFile), 1, fw);
    fclose(fw);
    printf("[Sauvegarde] Partie sauvegardée et sécurisée dans save.dat\n");
  } else {
    perror("Erreur ouverture fichier sauvegarde");
  }
}

void chargerSauvegarde(Entity* player, Entity* bot, int* score) {
  // Lecture Binaire
  // Note le "rb" (Read Binary) au lieu de "r"
  FILE* fr = fopen("save.dat", "rb");
  if (!fr) {
    printf("[Chargement] Aucune sauvegarde trouvée.\n");
    return;
  }

  SaveFile save;
  size_t lu = fread(&save, sizeof(SaveFile), 1, fr);
  fclose(fr);

  if (lu != 1) {
    printf(
        "[Chargement] Erreur : Fichier de sauvegarde corrompu ou incomplet.\n");
    return;
  }

  // Décryptage
  // RC4 est symétrique donc on rappelle la fonction pour décrypter
  rc4_crypt((unsigned char*)&save, sizeof(SaveFile), GAME_KEY,
            strlen(GAME_KEY));

  // Vérification Anti-Triche
  // On recalcule le checksum sur les données qu'on vient de décrypter
  uint32_t verif =
      calculate_checksum((unsigned char*)&save.gameData, sizeof(SaveData));

  if (verif != save.checksum) {
    printf(
        "ALERTE TRICHE : Le fichier de sauvegarde a été modifié manuellement "
        "!\n");
    // Ici tu peux décider de bloquer le chargement, ou mettre le score à 0
    return;
  }

  // Application des données (Si tout est bon)
  *score = save.gameData.score;
  player->maxAmmo = save.gameData.maxAmmo;

  player->pos.x = save.gameData.x;
  player->pos.y = save.gameData.y;
  player->pos.z = save.gameData.z;

  player->yaw = save.gameData.yaw;
  player->pitch = save.gameData.pitch;

  player->onGround = save.gameData.onGround;
  player->ammo = save.gameData.ammo;

  player->health = save.gameData.health;
  player->maxHealth = save.gameData.maxHealth;
  player->life = save.gameData.life;

  *bot = save.gameData.bot;
  printf("[Chargement] Partie chargée avec succès !\n");
}
