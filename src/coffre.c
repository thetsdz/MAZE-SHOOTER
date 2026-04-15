
/**
 * \file heal.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "../lib/headers/coffre.h"
#include "../lib/headers/types.h"
#include "../lib/headers/player.h"
 
// Structure pour représenter une position dans la grille du labyrinthe
typedef struct {
    int i;
    int j;
} GridPos;

void InitCoffre(Coffre *coffre,
             Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
    // --- RECHERCHE D'UN SPAWN ALÉATOIRE ---
    float offset = NUM_BLOCKS - 1; // Le même offset que dans level.c
    int i, j;
    int random = rand() % 100; 
    int randomWeapon = rand() % 100;

    if(random <50){
        coffre->healAmount = 20;
    }
    else if(random <85){
        coffre ->healAmount = 40;
    }
    else if(random <99){
        coffre ->healAmount = 60;
    }
    else{
        coffre -> healAmount = 100;
    }

    if (randomWeapon < 70) {
        // 70% de chance : Pas d'arme en plus
        coffre->arme = PISTOLET; // PISTOLET vaut 0
    }
    else if (randomWeapon < 88) {
        // 18% de chance (88 - 70) : Sniper
        coffre->arme= SNIPER; //SNIPER vaut 1
    }
    else if (randomWeapon < 97) {
        // 9% de chance (97 - 88) : Fusil d'Assaut
        coffre->arme = FUSIL; // FUSIL vaut 2
    }
    else {
        // 3% de chance (100 - 97) : Grenade
        coffre->arme = GRENADE; // GRENADE vaut 3 
    }


    // On boucle jusqu'à trouver une case qui N'EST PAS un mur
    do {
        // rand() % NUM_BLOCKS donne un nombre entre 0 et 50
        i = rand() % NUM_BLOCKS;
        j = rand() % NUM_BLOCKS;
    } while (blocks[i][j].isWall);

    coffre->pos.x = i * 3.0f - offset;
    coffre->pos.z = j * 3.0f - offset;
    coffre->pos.y = 0.0f;
}

int UpdateCoffre(Coffre *coffre, Entity *player, Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
    // Vérifier la distance entre le joueur et l'objet de soin
    float dx = coffre->pos.x - player->pos.x;
    float dz = coffre->pos.z - player->pos.z;
    float distance = sqrtf(dx * dx + dz * dz);
    int armeUnlock=-1;

    if (distance < 1.0f) { // Seuil de ramassage (1 mètre)
        player->health += coffre->healAmount; // Soigner le joueur
        armeUnlock=coffre->arme;
        if (player->health > player->maxHealth)
            player->health = player->maxHealth; // Ne pas dépasser la santé max
        // Réinitialiser la position de l'objet de soin
        InitCoffre(coffre, blocks);
        
    }
    return armeUnlock;

}