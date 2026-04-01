#include "../../lib/headers/cryptage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// --- EXEMPLE D'UTILISATION ---

typedef struct {
    int level;
    int gold;
    float health;
    char player_name[32];
} GameSave;

int main() {
    // 1. Création d'une sauvegarde exemple
    GameSave mySave = {5, 9999, 75.5f, "Arthur"};
    const char *KEY = "MaSuperCleSecrete123"; // Une clé privée
    
    printf("--- AVANT SAUVEGARDE ---\n");
    printf("Or: %d, Vie: %.1f\n", mySave.gold, mySave.health);

    // ---------------------------------------------------------
    // ETAPE A : SAUVEGARDER (Ecriture disque)
    // ---------------------------------------------------------
    
    // a. Calcul du checksum AVANT cryptage (pour vérifier l'intégrité plus tard)
    uint32_t checksum = calculate_checksum((unsigned char*)&mySave, sizeof(GameSave));
    
    // b. Cryptage des données (Modification en place)
    rc4_crypt((unsigned char*)&mySave, sizeof(GameSave), KEY, strlen(KEY));

    // c. Ecriture dans le fichier (Mode binaire "wb" indispensable !)
    FILE *fp = fopen("savegame.txt", "wb");
    if (fp) {
        fwrite(&checksum, sizeof(uint32_t), 1, fp); // On écrit le checksum en premier
        fwrite(&mySave, sizeof(GameSave), 1, fp);   // On écrit les données cryptées ensuite
        fclose(fp);
        printf("\n[Succes] Fichier 'savegame.txt' ecrit (encrypte).\n");
    }

    // ---------------------------------------------------------
    // ETAPE B : CHARGER (Lecture disque)
    // ---------------------------------------------------------
    
    GameSave loadedSave;
    uint32_t read_checksum;

    fp = fopen("savegame.txt", "rb"); // Mode binaire "rb" indispensable !
    if (fp) {
        fread(&read_checksum, sizeof(uint32_t), 1, fp);
        fread(&loadedSave, sizeof(GameSave), 1, fp);
        fclose(fp);

        // a. Décryptage (RC4 est symétrique : on réapplique la même fonction)
        rc4_crypt((unsigned char*)&loadedSave, sizeof(GameSave), KEY, strlen(KEY));

        // b. Vérification Anti-Triche
        uint32_t calculated_new_checksum = calculate_checksum((unsigned char*)&loadedSave, sizeof(GameSave));

        if (calculated_new_checksum == read_checksum) {
            printf("\n--- CHARGEMENT REUSSI ---\n");
            printf("Integrite validee. Or: %d, Vie: %.1f\n", loadedSave.gold, loadedSave.health);
        } else {
            printf("\n[ERREUR] Triche detectee ou fichier corrompu !\n");
        }
    }

    return 0;
}