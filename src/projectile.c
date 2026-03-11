#include "../lib/headers/projectile.h"

#include <stdio.h>  // Ajout pour printf/TraceLog si besoin
#include <stdlib.h>

void InitProjectiles(Projectile* projs) {
  for (int i = 0; i < MAX_PROJ; i++) projs[i].active = false;
}

// Fonction générique pour tirer (Bot ou Joueur)
<<<<<<< HEAD
void ShootProjectile(Projectile *projs, Vector3 startPos, Vector3 direction, OwnerType owner, float speed, float radius, Color color) {
    // Normalisation de la direction par sécurité
    Vector3 dir = Vector3Normalize(direction);
=======
void ShootProjectile(Projectile* projs, Vector3 startPos, Vector3 direction,
                     OwnerType owner) {
  // Normalisation de la direction par sécurité
  Vector3 dir = Vector3Normalize(direction);
>>>>>>> master

  // Point d'apparition un peu devant pour ne pas se tirer dessus
  Vector3 spawn = Vector3Add(startPos, Vector3Scale(dir, 0.8f));

<<<<<<< HEAD
    for(int i=0; i<MAX_PROJ; i++){
        if(!projs[i].active){
            projs[i].active = true;
            projs[i].pos = spawn;
            projs[i].vel = Vector3Scale(dir, speed); // Vitesse du projectile
            projs[i].radius = radius;
            projs[i].color=color;
            projs[i].life = 5.0f;
            projs[i].owner = owner; // <-- On définit le propriétaire
            break;
        }
=======
  for (int i = 0; i < MAX_PROJ; i++) {
    if (!projs[i].active) {
      projs[i].active = true;
      projs[i].pos = spawn;
      projs[i].vel = Vector3Scale(dir, 50.0f);  // Vitesse du projectile
      projs[i].radius = 0.2f;
      projs[i].life = 5.0f;
      projs[i].owner = owner;  // <-- On définit le propriétaire
      break;
>>>>>>> master
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
    projs[i].pos = Vector3Add(projs[i].pos, Vector3Scale(projs[i].vel, dt));
    projs[i].life -= dt;
    
    if (projs[i].life <= 0.0f) {
      projs[i].active = false;
      continue;
    }

    // --- NOUVELLE LOGIQUE DE COLLISION AABB ---
    // 1. MES BALLES touchent l'AUTRE (Bot ou RemotePlayer)
    if (projs[i].owner == OWNER_PLAYER) {
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

        float h = player->size / 2.0f;
        if (projs[i].pos.x > player->pos.x - h && projs[i].pos.x < player->pos.x + h &&
            projs[i].pos.y > player->pos.y && projs[i].pos.y < player->pos.y + player->size &&
            projs[i].pos.z > player->pos.z - h && projs[i].pos.z < player->pos.z + h) 
        {
            player->health -= 20;
            projs[i].active = false;

            // --- GESTION DE LA MORT EN SOLO ---
            if (projs[i].owner == OWNER_BOT && player->health <= 0) {
                player->health = player->maxHealth;
                player->ammo = player->maxAmmo;
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
        if (projs[i].pos.x > b.pos.x - halfX &&
            projs[i].pos.x < b.pos.x + halfX &&
            projs[i].pos.y > b.pos.y - halfY &&
            projs[i].pos.y < b.pos.y + halfY &&
            projs[i].pos.z > b.pos.z - halfZ &&
            projs[i].pos.z < b.pos.z + halfZ) {
          projs[i].active = false;
          break;
        }
      }
      if (!projs[i].active) break;
    }
  }
}

<<<<<<< HEAD


void DrawProjectiles(Projectile *projs) {
    for(int i=0; i<MAX_PROJ; i++){
        if(projs[i].active) {
            // On utilise la couleur qu'on a pris la peine d'enregistrer !
            DrawSphere(projs[i].pos, projs[i].radius, projs[i].color);
        }
=======
void DrawProjectiles(Projectile* projs) {
  for (int i = 0; i < MAX_PROJ; i++) {
    if (projs[i].active) {
      Color c = RED;
      if (projs[i].owner == OWNER_PLAYER)
        c = MAGENTA;
      else if (projs[i].owner == OWNER_BOT)
        c = ORANGE;
      else if (projs[i].owner == OWNER_REMOTE_PLAYER)
        c = YELLOW;

      DrawSphere(projs[i].pos, projs[i].radius, c);
>>>>>>> master
    }
  }
}