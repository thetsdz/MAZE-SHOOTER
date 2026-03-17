#include "../lib/headers/projectile.h"

#include <stdio.h>  // Ajout pour printf/TraceLog si besoin
#include <stdlib.h>

void InitProjectiles(Projectile* projs) {
  for (int i = 0; i < MAX_PROJ; i++) projs[i].active = false;
}


void RebondirGrenade(Projectile *p, Block *b) {
    float elasticite = 0.4f; // Un peu moins d'élasticité pour qu'elle s'arrête plus vite
    float radius = p->radius;

      if (b == NULL) {
      if (p->pos.y < p->radius) {
        
          // 1. On la plaque au sol (plus de +0.01f qui fait sauter la balle)
          p->pos.y = p->radius; 

          // 2. REBOND : Seulement si la chute est significative
          // Si elle tombe à plus de 0.5 unités/s, on rebondit
          if (p->vel.y < -0.5f) {
              p->vel.y = p->vel.y * -elasticite;
          } else {
              // Sinon, elle est trop lente : on stoppe la verticale net
              p->vel.y = 0;
          }

          // 3. FRICTION : On freine fort sur les côtés
          p->vel.x = p->vel.x * 0.5f;
          p->vel.z = p->vel.z * 0.5f;

          // 4. VERROU FINAL : Si elle ne bouge presque plus, on fige tout
          if (p->vel.y < 0.2f && p->vel.y > -0.2f) {
              p->vel.y = 0;
              // Si le mouvement horizontal est aussi très faible, on arrête la glissade
              if (p->vel.x < 0.1f && p->vel.x > -0.1f) p->vel.x = 0;
              if (p->vel.z < 0.1f && p->vel.z > -0.1f) p->vel.z = 0;
          }
        }
    }
    else {
        // --- CAS DES MURS ---
        float halfX = b->width / 2.0f;
        float halfY = b->height / 2.0f;
        float halfZ = b->depth / 2.0f;

        float overlapX = halfX - fabsf(p->pos.x - b->pos.x);
        float overlapY = halfY - fabsf(p->pos.y - b->pos.y);
        float overlapZ = halfZ - fabsf(p->pos.z - b->pos.z);

        if (overlapX < overlapY && overlapX < overlapZ) {
            p->vel.x = p->vel.x * -elasticite;
            if (p->pos.x > b->pos.x) p->pos.x = p->pos.x + overlapX;
            else p->pos.x = p->pos.x - overlapX;
        } 
        else if (overlapY < overlapX && overlapY < overlapZ) {
            p->vel.y = p->vel.y * -elasticite;
            if (p->pos.y > b->pos.y) p->pos.y = p->pos.y + overlapY;
            else p->pos.y = p->pos.y - overlapY;
        } 
        else {
            p->vel.z = p->vel.z * -elasticite;
            if (p->pos.z > b->pos.z) p->pos.z = p->pos.z + overlapZ;
            else p->pos.z = p->pos.z - overlapZ;
        }
    }
}






// Fonction générique pour tirer (Bot ou Joueur)
void ShootProjectile(Projectile *projs, Vector3 startPos, Vector3 direction, OwnerType owner, ModeleArme arme, float speed, float radius, Color color) {
    // Normalisation de la direction par sécurité
    Vector3 dir = Vector3Normalize(direction);

  // Point d'apparition un peu devant pour ne pas se tirer dessus
  Vector3 spawn = Vector3Add(startPos, Vector3Scale(dir, 0.8f));

    for(int i=0; i<MAX_PROJ; i++){
        if(!projs[i].active){
            projs[i].active = true;
            projs[i].pos = spawn;
            projs[i].vel = Vector3Scale(dir, speed); // Vitesse du projectile
            projs[i].radius = radius;
            projs[i].color=color;
            projs[i].life = 5.0f;
            projs[i].owner = owner; // <-- On définit le propriétaire
            if (arme.type==GRENADE) projs[i].type=PROJ_GRENADE;
            else  projs[i].type=PROJ_NORMALE;
            break;
        }

    }
  }

// ... (Début du fichier identique)

void UpdateProjectiles(Projectile* projs, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Entity* autre, Entity* player, int* score) {
  float dt = GetFrameTime();

  for (int i = 0; i < MAX_PROJ; i++) {
    if (!projs[i].active) continue;

    // Déplacement
      if (projs[i].type==PROJ_GRENADE){
        projs[i].vel.y -= 35.0f * dt;   //modifier ici pour plus de gravité de la grenade
      }
    projs[i].pos = Vector3Add(projs[i].pos, Vector3Scale(projs[i].vel, dt));
    projs[i].life -= dt;
    
    if (projs[i].life <= 0.0f) {
      projs[i].active = false;
      continue;
    }

    // --- NOUVELLE LOGIQUE DE COLLISION AABB ---
    // 1. MES BALLES touchent l'AUTRE (Bot ou RemotePlayer)
    if (projs[i].owner == OWNER_PLAYER && projs[i].type!=PROJ_GRENADE) {
        float h = autre->size / 2.0f;
        if (projs[i].pos.x > autre->pos.x - h && projs[i].pos.x < autre->pos.x + h &&
            projs[i].pos.y > autre->pos.y && projs[i].pos.y < autre->pos.y + autre->size &&
            projs[i].pos.z > autre->pos.z - h && projs[i].pos.z < autre->pos.z + h) {
            
            autre->health -= 20;
            projs[i].active = false;

            if (autre->health <= 0) {
                *score += 1;
                // Si c'est le BOT, on le fait respawn ailleurs
                if (autre->type == ENTITY_BOT) {
                    autre->health = autre->maxHealth;
                    autre->pos = (Vector3){(float)(rand()%NUM_BLOCKS), 10.0f, (float)(rand()%NUM_BLOCKS)};
                    autre->velocityY = 0;
                }
            }
            continue;
        }
    }
    

    // 2. LES BALLES ENNEMIES (Bot ou Remote) me touchent MOI
    else if (projs[i].owner == OWNER_BOT || projs[i].owner == OWNER_REMOTE_PLAYER) {
        if (player->health <= 0) continue; // Sécurité : on ne touche pas un mort

        float h = player->size / 50.0f;
        if (projs[i].pos.x > player->pos.x - h && projs[i].pos.x < player->pos.x + h &&
            projs[i].pos.y > player->pos.y && projs[i].pos.y < player->pos.y + player->size &&
            projs[i].pos.z > player->pos.z - h && projs[i].pos.z < player->pos.z + h) 
        {
            player->health -= 20;
            projs[i].active = false;

            // --- GESTION DE LA MORT EN SOLO ---
            if (projs[i].owner == OWNER_BOT && player->health <= 0) {
                player->health = player->maxHealth;
                player->ammo = player->armeEquipee.munitionsMax;
                player->pos = (Vector3){1.5f, 10.0f, 1.5f}; // Position de respawn solo
                player->velocityY = 0; // IMPORTANT : stop la chute
                TraceLog(LOG_INFO, "Mort en solo ! Respawn...");
            }
            continue;
        }
    }

    // 4. Murs (Identique)
    for (int x = 0; x < NUM_BLOCKS; x++) {
      for (int y = 0; y < NUM_BLOCKS; y++) {
        Block b = blocks[x][y];
        if (b.color.a == 0) continue;  // Ignore l'air

        float halfX = b.width / 2;
        float halfY = b.height / 2;
        float halfZ = b.depth / 2;
        if (
            projs[i].pos.x > b.pos.x - halfX &&
            projs[i].pos.x < b.pos.x + halfX &&
            projs[i].pos.y > b.pos.y - halfY &&
            projs[i].pos.y < b.pos.y + halfY &&
            projs[i].pos.z > b.pos.z - halfZ &&
            projs[i].pos.z < b.pos.z + halfZ ) 
            {
              if (projs[i].type==PROJ_GRENADE){
                  RebondirGrenade(&(projs[i]),&b);
              }
              else {
                projs[i].active = false;
              }
              break;
            }
    
      }
      if (!projs[i].active) break;
    }
    // 5. Sol ici
    if (projs[i].active && projs[i].type == PROJ_GRENADE) {
        // On vérifie le sol seulement ici !
        if (projs[i].pos.y <= 0.0f) {
            RebondirGrenade(&(projs[i]), NULL);
        }
    }


    
  }
}



void DrawProjectiles(Projectile *projs) {
    for(int i=0; i<MAX_PROJ; i++){
        if(projs[i].active) {
            // On utilise la couleur qu'on a pris la peine d'enregistrer !
            DrawSphere(projs[i].pos, projs[i].radius, projs[i].color);
        }
    }
  }
