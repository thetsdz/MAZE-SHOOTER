/**
 * \file sauvegarde.c
 */

#include "../lib/headers/sauvegarde.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/headers/cryptage.h"
#include "../lib/headers/dessin.h"
#include "../lib/headers/types.h"

// --- CONSTANTES DE SAUVEGARDE ---
#define MAX_ARMES 4
#define MAX_BOTS 18
#define MAX_HEALS 10

// CLÉ DE CRYPTAGE (À ne pas partager ou modifier !)
static const char* GAME_KEY =
    "a998cd54ac641b3bf1a48369f52fb4ec9e3efffd835525e0578130c1068de2bd";

// Structure qui contient toutes les données d'état du jeu
typedef struct {
  int score;
  float x, y, z;
  float yaw, pitch;
  int onGround;
  int tabammo[MAX_ARMES];
  int armeUnlock[MAX_ARMES];
  int health;
  int maxHealth;
  int life;
  
  // ATTENTION : Si Entity ou Heal contiennent des Texture2D ou Model, 
  // il faudra un jour créer des "SaveEntity" qui ne gardent que la pos et la vie.
  Entity bot[MAX_BOTS];
  Entity boss;
  int IsBossAlive;
  Heal heal[MAX_HEALS];
} SaveData;

// Bloc écrit sur le disque : Données + Sécurité
typedef struct {
  SaveData gameData;  // Les données de jeu
  uint32_t checksum;  // L'empreinte de sécurité
} SaveFile;


void sauvegarder(Entity* player, Entity bot[MAX_BOTS], Entity* boss, bool IsBossAlive, Heal heal[MAX_HEALS]) {
  SaveFile save;

  // 1. Remplissage des variables simples du joueur
  save.gameData.score = player->score;
  save.gameData.x = player->pos.x;
  save.gameData.y = player->pos.y;
  save.gameData.z = player->pos.z;
  save.gameData.yaw = player->yaw;
  save.gameData.pitch = player->pitch;
  save.gameData.onGround = player->onGround;
  save.gameData.health = player->health;
  save.gameData.maxHealth = player->maxHealth;
  save.gameData.life = player->life;

  // 2. Copies ultra-rapides des tableaux avec memcpy (remplace les boucles et affectations manuelles)
  memcpy(save.gameData.tabammo, player->tabammo, sizeof(int) * MAX_ARMES);
  memcpy(save.gameData.armeUnlock, player->armeUnlock, sizeof(int) * MAX_ARMES);
  memcpy(save.gameData.bot, bot, sizeof(Entity) * MAX_BOTS);
  memcpy(save.gameData.heal, heal, sizeof(Heal) * MAX_HEALS);

  // 3. Sauvegarde du boss
  save.gameData.boss = *boss;
  save.gameData.IsBossAlive = IsBossAlive;

  // 4. Calcul du Checksum sur les données en clair
  save.checksum = calculate_checksum((unsigned char*)&save.gameData, sizeof(SaveData));

  // 5. Cryptage RC4 de TOUT le fichier (Données + Checksum)
  rc4_crypt((unsigned char*)&save, sizeof(SaveFile), GAME_KEY, strlen(GAME_KEY));

  // 6. Écriture Binaire
  FILE* fw = fopen("save.dat", "wb");
  if (fw) {
    fwrite(&save, sizeof(SaveFile), 1, fw);
    fclose(fw);
    printf("[Sauvegarde] Partie sauvegardée et sécurisée dans save.dat\n");
  } else {
    perror("Erreur ouverture fichier sauvegarde");
  }
}

int chargerSauvegarde(Entity* player, Entity bot[MAX_BOTS], Entity* boss, bool* IsBossAlive, Heal heal[MAX_HEALS]) {
  FILE* fr = fopen("save.dat", "rb");
  if (!fr) {
    printf("[Chargement] Aucune sauvegarde trouvée.\n");
    return 2;  // Échec
  }

  SaveFile save;
  size_t lu = fread(&save, sizeof(SaveFile), 1, fr);
  fclose(fr);

  if (lu != 1) {
    printf("[Chargement] Erreur : Fichier de sauvegarde corrompu.\n");
    return 2; // Échec
  }

  // 1. Décryptage
  rc4_crypt((unsigned char*)&save, sizeof(SaveFile), GAME_KEY, strlen(GAME_KEY));
  
  // 2. Vérification de l'intégrité
  uint32_t verif = calculate_checksum((unsigned char*)&save.gameData, sizeof(SaveData));

  if (verif != save.checksum) {
    printf("ALERTE TRICHE : Le fichier de sauvegarde a été modifié manuellement !\n");
    return 1; // Triche détectée
  }

  // 3. Application des données simples
  player->score = save.gameData.score;
  player->pos.x = save.gameData.x;
  player->pos.y = save.gameData.y;
  player->pos.z = save.gameData.z;
  player->yaw = save.gameData.yaw;
  player->pitch = save.gameData.pitch;
  player->onGround = save.gameData.onGround;
  player->health = save.gameData.health;
  player->maxHealth = save.gameData.maxHealth;
  player->life = save.gameData.life;

  // 4. Restauration ultra-rapide des tableaux avec memcpy
  memcpy(player->tabammo, save.gameData.tabammo, sizeof(int) * MAX_ARMES);
  memcpy(player->armeUnlock, save.gameData.armeUnlock, sizeof(int) * MAX_ARMES);
  memcpy(bot, save.gameData.bot, sizeof(Entity) * MAX_BOTS);
  memcpy(heal, save.gameData.heal, sizeof(Heal) * MAX_HEALS);

  // 5. Restauration du boss
  *boss = save.gameData.boss;
  *IsBossAlive = save.gameData.IsBossAlive;

  printf("[Chargement] Partie chargée avec succès !\n");
  return 0; // Succès
}