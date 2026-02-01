#include <stdlib.h>
#include "../lib/headers/projectile.h"

void InitProjectiles(Projectile *projs) {
    for(int i=0; i<MAX_PROJ; i++) projs[i].active = false;
}

// Fonction générique pour tirer (Bot ou Joueur)
void ShootProjectile(Projectile *projs, Vector3 startPos, Vector3 direction, OwnerType owner) {
    // Normalisation de la direction par sécurité
    Vector3 dir = Vector3Normalize(direction);

    // Point d'apparition un peu devant pour ne pas se tirer dessus
    Vector3 spawn = Vector3Add(startPos, Vector3Scale(dir, 0.8f));

    for(int i=0; i<MAX_PROJ; i++){
        if(!projs[i].active){
            projs[i].active = true;
            projs[i].pos = spawn;
            projs[i].vel = Vector3Scale(dir, 50.0f); // Vitesse du projectile
            projs[i].radius = 0.2f;
            projs[i].life = 5.0f;
            projs[i].owner = owner; // <-- On définit le propriétaire
            break;
        }
    }
}

void UpdateProjectiles(Projectile *projs, Block blocks[NUM_BLOCKS][NUM_BLOCKS], Bot *bot, Player *player, int *score) {
    float dt = GetFrameTime();
    
    for(int i=0; i<MAX_PROJ; i++){
        if(!projs[i].active) continue;
        
        projs[i].pos = Vector3Add(projs[i].pos, Vector3Scale(projs[i].vel, dt));
        projs[i].life -= dt;
        if(projs[i].life <= 0.0f){ projs[i].active = false; continue; }

        // --- COLLISIONS ---

        // 1. Si c'est un tir du JOUEUR -> Vérifie collision avec le BOT
        if(projs[i].owner == OWNER_PLAYER) {
            float botRadius = bot->size / 2.0f;
            if(Vector3Distance(projs[i].pos, bot->pos) <= projs[i].radius + botRadius){
                TraceLog(LOG_INFO, "Le joueur a touche le bot !");
                (*score)++;
                // Respawn du bot
                bot->pos.x = (float)(rand()%20 - 10);
                bot->pos.z = (float)(rand()%20 - 10);
                projs[i].active = false;
                continue;
            }
        }
        
        // 2. Si c'est un tir du BOT -> Vérifie collision avec le JOUEUR
        else if(projs[i].owner == OWNER_BOT) {
            float playerRadius = player->size / 2.0f;
            // On vise le centre du corps du joueur (pos.y + 0.5f pour le torse)
            Vector3 playerCenter = {player->pos.x, player->pos.y, player->pos.z};
            
            if(Vector3Distance(projs[i].pos, playerCenter) <= projs[i].radius + playerRadius){
                TraceLog(LOG_INFO, "Le bot a touche le joueur ! Score Reset.");
                *score = 0; // Remise à zéro du score
                projs[i].active = false;
                continue;
            }
        }

        // 3. Collision avec les Murs
        for(int x=0; x<NUM_BLOCKS; x++){
            for(int y = 0; y <NUM_BLOCKS; y++){
                Block b = blocks[x][y];
                float halfX = b.width/2; float halfY = b.height/2; float halfZ = b.depth/2;
                if(projs[i].pos.x > b.pos.x - halfX && projs[i].pos.x < b.pos.x + halfX &&
                   projs[i].pos.y > b.pos.y - halfY && projs[i].pos.y < b.pos.y + halfY &&
                   projs[i].pos.z > b.pos.z - halfZ && projs[i].pos.z < b.pos.z + halfZ){
                    projs[i].active = false;
                    break;
                }
            }
        }
    }
}

void DrawProjectiles(Projectile *projs) {
    for(int i=0; i<MAX_PROJ; i++){
        if(projs[i].active) {
            // Couleur différente pour différencier les tirs
            Color c = (projs[i].owner == OWNER_PLAYER) ? MAGENTA : ORANGE;
            DrawSphere(projs[i].pos, projs[i].radius, c);
        }
    }
}