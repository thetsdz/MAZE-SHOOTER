/** \file bot.c
    \brief Gère les robots
*/


#include <math.h>
#include <stdlib.h>
#include "../lib/headers/bot.h"

void InitBot(Entity *bot) {
    bot->pos = (Vector3){0, 10.0f, 0}; //initialisation de la position du robot
    bot->yaw = 0.0f; //angle du bot
    bot->pitch = 0.0f; //angle du bot
    bot->velocityY = 0.0f; //vitesse du bot
    bot->onGround = true; //le bot est initialisée au sol
    bot->size = 1.0f; //taille du bot
    bot->type = ENTITY_BOT; //type de l'entité
}

void UpdateBot(Entity *bot, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Vector3 targetPos, Projectile *projs) {
    
    /** \brief Paramètres du Bot
        \code  
        float speed = 0.08f;
        float gravity = 0.02f;
        float botHalf = bot->size/2;
        float dt = GetFrameTime();
        \endcode
    */
    float speed = 0.08f;
    float gravity = 0.02f;
    float botHalf = bot->size/2;
    float dt = GetFrameTime();



    /** \brief Calcul de l'angle pour regarder le joueur sur le plan horizontal (XZ)
        \code
        float dx = targetPos.x - bot->pos.x;
        float dz = targetPos.z - bot->pos.z;
        bot->yaw = atan2f(dx, dz); 
        \endcode 
    */ 
    float dx = targetPos.x - bot->pos.x;
    float dz = targetPos.z - bot->pos.z;
    bot->yaw = atan2f(dx, dz); 
    

    /** \brief Viser en hauteur
        \code
        float dist = sqrtf(dx*dx + dz*dz);
        float dy = (targetPos.y + 0.5f) - (bot->pos.y + 0.5f);
        bot->pitch = atan2f(dy, dist);
        \endcode 
    */   
    float dist = sqrtf(dx*dx + dz*dz);
    float dy = (targetPos.y + 0.5f) - (bot->pos.y + 0.5f); 
    bot->pitch = atan2f(dy, dist);


    // --- Tir ---
    static float shootTimer = 0.0f;
    shootTimer += dt;

    // Le bot tire toutes les 1.5 à 2.5 secondes (aléatoire un peu)
    if(shootTimer > 2.0f && dist < 30.0f) { // Ne tire que si < 30 mètres
        
        // Calcul du vecteur de visée parfait
        Vector3 aimDir = Vector3Subtract(targetPos, bot->pos);
        aimDir = Vector3Normalize(aimDir);

        // --- Ajout de l'IMPRÉCISION ---
        // On modifie légèrement le vecteur direction avec des valeurs aléatoires
        // Plus le diviseur est petit, plus le bot est imprécis
        float spread = 0.15f; 
        aimDir.x += ((float)(rand()%100)/50.0f - 1.0f) * spread;
        aimDir.y += ((float)(rand()%100)/50.0f - 1.0f) * spread;
        aimDir.z += ((float)(rand()%100)/50.0f - 1.0f) * spread;

        // Position de départ (au niveau des yeux du bot)
        Vector3 shootOrigin = { bot->pos.x, bot->pos.y + 0.5f, bot->pos.z };

        // Tir avec propriétaire BOT
        ShootProjectile(projs, shootOrigin, aimDir, OWNER_BOT, 40.0f, 0.2f, ORANGE);
        
        // Reset timer (avec une petite variation aléatoire)
        shootTimer = (float)(rand()%100) / 200.0f; // Reset à 0.0 - 0.5s
    }


    // --- Physique & Mouvement (Gravité) ---
    // Le bot saute s'il est bloqué ou aléatoirement
    static float moveTimer = 0.0f;
    moveTimer += dt;
    
    if(fmodf(moveTimer, 4.0f) < 0.1f && bot->onGround) {
        bot->velocityY = 0.35f; // Petit saut
        bot->onGround = false;
    }

    bot->velocityY -= gravity; // Application gravité

    // Calcul position future
    Vector3 nextPos = bot->pos;
    
    // Déplacement basique : Le bot avance doucement vers le joueur (zombie style)
    // Mais s'arrête s'il est trop près (pour tirer)
    if(dist > 5.0f) {
        nextPos.x += sinf(bot->yaw) * speed;
        nextPos.z += cosf(bot->yaw) * speed;
    }

    nextPos.y += bot->velocityY;


    // --- Collisions (AABB) ---
    bot->onGround = false; 
    
    for(int i=0; i<NUM_BLOCKS; i++){
        for(int j=0; j<NUM_BLOCKS; j++){  
            Block b = blocks[i][j];
            float halfX = b.width/2; float halfY = b.height/2; float halfZ = b.depth/2;

            bool collideX = nextPos.x + botHalf > b.pos.x - halfX && nextPos.x - botHalf < b.pos.x + halfX;
            bool collideY = nextPos.y + botHalf > b.pos.y - halfY && nextPos.y - botHalf < b.pos.y + halfY;
            bool collideZ = nextPos.z + botHalf > b.pos.z - halfZ && nextPos.z - botHalf < b.pos.z + halfZ;

            if(collideX && collideY && collideZ){
                float top = b.pos.y + halfY;
                float bottom = b.pos.y - halfY;

                if(bot->velocityY < 0 && bot->pos.y - botHalf >= top){ 
                    nextPos.y = top + botHalf; 
                    bot->velocityY = 0;        
                    bot->onGround = true;      
                }
                else if(bot->velocityY > 0 && bot->pos.y + botHalf <= bottom){ 
                    nextPos.y = bottom - botHalf;
                    bot->velocityY = 0;        
                } 
                else {
                    float overlapX = (halfX + botHalf) - fabsf(nextPos.x - b.pos.x);
                    float overlapZ = (halfZ + botHalf) - fabsf(nextPos.z - b.pos.z);

                    if (overlapX < overlapZ) {
                        if (nextPos.x < b.pos.x) nextPos.x = b.pos.x - halfX - botHalf;
                        else nextPos.x = b.pos.x + halfX + botHalf;
                    } 
                    else {
                        if (nextPos.z < b.pos.z) nextPos.z = b.pos.z - halfZ - botHalf;
                        else nextPos.z = b.pos.z + halfZ + botHalf;
                    }
                }
            }
        }
    }

    // --- Gravité et Sol Raylib (Sécurité) ---
    float botBottom = nextPos.y - botHalf;
    float closestGround = 0.0f; // Sol par défaut

    for(int i = 0; i < NUM_BLOCKS; i++){
        for(int j = 0; j < NUM_BLOCKS; j++){
            Block b = blocks[i][j];
            if(b.color.a != 0) continue; // Ignore les murs pleins, cherche les sols
            float top = b.pos.y + b.height / 2.0f; 
            if(botBottom >= top && nextPos.y - botHalf <= top){
                if(top > closestGround) closestGround = top;
            }
        }
    }

    if(botBottom <= closestGround){
        nextPos.y = closestGround + botHalf;
        bot->velocityY = 0;
        bot->onGround = true;
    }

    // Validation finale
    bot->pos = nextPos;
}