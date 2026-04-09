/**
 * \file sauvegarde.c
 */

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
  float x, y, z;
  float yaw, pitch;
  int onGround;
  int ammo;
  int health;
  int maxHealth;
  int life;
  Entity bot[18];
  Entity boss;
  int IsBossAlive;
  Block blocks[NUM_BLOCKS][NUM_BLOCKS];
  Heal heal[10];
} SaveData;

// C'est ce bloc qui sera écrit sur le disque : Données + Sécurité
typedef struct {
  SaveData gameData;  // Tes données de jeu
  uint32_t checksum;  // L'empreinte de sécurité
} SaveFile;

void sauvegarder(Entity* player, Entity bot[18], Entity* boss, bool IsBossAlive,
                 Block blocks[NUM_BLOCKS][NUM_BLOCKS], Heal heal[10]) {
  SaveFile save;

  // Remplissage de la structure de données
  save.gameData.score = player->score;
  save.gameData.ammo = player->ammo;

  save.gameData.x = player->pos.x;
  save.gameData.y = player->pos.y;
  save.gameData.z = player->pos.z;

  save.gameData.yaw = player->yaw;
  save.gameData.pitch = player->pitch;

  save.gameData.onGround = player->onGround;

  save.gameData.health = player->health;
  save.gameData.maxHealth = player->maxHealth;
  save.gameData.life = player->life;

  for (int i = 0; i < 18; i++) {
    save.gameData.bot[i] = bot[i];
  }

  save.gameData.boss = *boss;
  save.gameData.IsBossAlive = IsBossAlive;

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      save.gameData.blocks[i][j] = blocks[i][j];
    }
  }

  for (int i = 0; i < 10; i++) {
    save.gameData.heal[i] = heal[i];
  }

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

void chargerSauvegarde(Entity* player, Entity bot[18], Entity* boss,
                       bool* IsBossAlive, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Heal heal[10]) {
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
        "[Chargement] Erreur : Fichier de sauvegarde corrompu ou "
        "incomplet.\n");
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
        "ALERTE TRICHE : Le fichier de sauvegarde a été modifié "
        "manuellement "
        "!\n");
    // Ici tu peux décider de bloquer le chargement, ou mettre le score à 0
    return;
  }

  // Application des données (Si tout est bon)
  player->score = save.gameData.score;
  player->ammo = save.gameData.ammo;

  player->pos.x = save.gameData.x;
  player->pos.y = save.gameData.y;
  player->pos.z = save.gameData.z;

  player->yaw = save.gameData.yaw;
  player->pitch = save.gameData.pitch;

  player->onGround = save.gameData.onGround;

  player->health = save.gameData.health;
  player->maxHealth = save.gameData.maxHealth;
  player->life = save.gameData.life;

  for (int i = 0; i < 18; i++) {
    bot[i] = save.gameData.bot[i];
  }
  *boss = save.gameData.boss;
  *IsBossAlive = save.gameData.IsBossAlive;

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      blocks[i][j] = save.gameData.blocks[i][j];
    }
  }

    for (int i = 0; i < 10; i++) {
        heal[i] = save.gameData.heal[i];
    }

  printf("[Chargement] Partie chargée avec succès !\n");
}
