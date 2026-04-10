
/**
 * \file heal.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "../lib/headers/heal.h"
#include "../lib/headers/types.h"
#include "../lib/headers/player.h"
 
// Structure pour représenter une position dans la grille du labyrinthe
typedef struct {
    int i;
    int j;
} GridPos;

void InitHeal(Heal *heal,
             Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
    // --- RECHERCHE D'UN SPAWN ALÉATOIRE ---
    float offset = NUM_BLOCKS - 1; // Le même offset que dans level.c
    int i, j;
    int random = rand() % 100; 
    int randomWeapon = rand() % 100;

    if(random <50){
        heal->healAmount = 20;
    }
    else if(random <85){
        heal ->healAmount = 40;
    }
    else if(random <99){
        heal ->healAmount = 60;
    }
    else{
        heal -> healAmount = 100;
    }

    if (randomWeapon < 70) {
        // 70% de chance : Pas d'arme en plus
        heal->arme = PISTOLET;
    }
    else if (randomWeapon < 88) {
        // 18% de chance (88 - 70) : Sniper
        heal->arme= SNIPER; //SNIPER vaut 2
    }
    else if (randomWeapon < 97) {
        // 9% de chance (97 - 88) : Fusil d'Assaut
        heal->arme = FUSIL; // FUSIL vaut 1
    }
    else {
        // 3% de chance (100 - 97) : Grenade
        heal->arme = GRENADE; // GRENADE vaut 3 (je sais mal fais faut réorganiser mais plus tard)
    }


    // On boucle jusqu'à trouver une case qui N'EST PAS un mur
    do {
        // rand() % NUM_BLOCKS donne un nombre entre 0 et 50
        i = rand() % NUM_BLOCKS;
        j = rand() % NUM_BLOCKS;
    } while (blocks[i][j].isWall);

    heal->pos.x = i * 3.0f - offset;
    heal->pos.z = j * 3.0f - offset;
    heal->pos.y = 0.0f;
}

int UpdateHeal(Heal *heal, Entity *player, Block blocks[NUM_BLOCKS][NUM_BLOCKS]) {
    // Vérifier la distance entre le joueur et l'objet de soin
    float dx = heal->pos.x - player->pos.x;
    float dz = heal->pos.z - player->pos.z;
    float distance = sqrtf(dx * dx + dz * dz);
    int armeUnlock=-1;

    if (distance < 1.0f) { // Seuil de ramassage (1 mètre)
        player->health += heal->healAmount; // Soigner le joueur
        armeUnlock=heal->arme;
        if (player->health > player->maxHealth)
            player->health = player->maxHealth; // Ne pas dépasser la santé max
        // Réinitialiser la position de l'objet de soin
        InitHeal(heal, blocks);
        
    }
    return armeUnlock;

}