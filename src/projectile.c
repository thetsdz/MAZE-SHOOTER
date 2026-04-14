/**
 * \file projectile.c
 */

#include "../lib/headers/projectile.h"

#include <stdio.h>  // Ajout pour printf/TraceLog si besoin
#include <stdlib.h>

#include "../lib/headers/audio.h"
#include "../lib/headers/bot.h"

void InitProjectiles(Projectile* projs) {
  for (int i = 0; i < MAX_PROJ; i++) projs[i].active = false;
}

void RebondirGrenade(Projectile* p, Block* b) {
  float elasticite =
      0.6f;  // Un peu moins d'élasticité pour qu'elle s'arrête plus vite
  // --- CAS DES MURS ---
  float halfX = b->width / 2.0f;
  float halfY = b->height / 2.0f;
  float halfZ = b->depth / 2.0f;

  float overlapX = halfX - fabsf(p->pos.x - b->pos.x);
  float overlapY = halfY - fabsf(p->pos.y - b->pos.y);
  float overlapZ = halfZ - fabsf(p->pos.z - b->pos.z);

  if (overlapX < overlapY && overlapX < overlapZ) {
    p->vel.x = p->vel.x * -elasticite;
    if (p->pos.x > b->pos.x)
      p->pos.x = p->pos.x + overlapX;
    else
      p->pos.x = p->pos.x - overlapX;
  } else if (overlapY < overlapX && overlapY < overlapZ) {
    p->vel.y = p->vel.y * -elasticite;
    if (p->pos.y > b->pos.y)
      p->pos.y = p->pos.y + overlapY;
    else
      p->pos.y = p->pos.y - overlapY;
  } else {
    p->vel.z = p->vel.z * -elasticite;
    if (p->pos.z > b->pos.z)
      p->pos.z = p->pos.z + overlapZ;
    else
      p->pos.z = p->pos.z - overlapZ;
  }
}

// gere le vecteur vitesste pos.y de projectiles
void gravite(Projectile* p, float dt) {
  float forceG = 30.0f;  // L'accélération (m/s²)
  p->vel.y -= forceG * dt;
}

void explosion(Projectile* p) {
  PlayExplosion();
  p->pos.y = 0;
  p->radius = 3.0f;
  p->color = WHITE;
  p->life = 1.5f;
  p->degats = 500;
}

// Fonction générique pour tirer (Bot ou Joueur)
void ShootProjectile(Projectile* projs, Vector3 startPos, Vector3 direction,
                     OwnerType owner, ModeleArme arme, float camYaw,
                     float camPitch) {
  // Normalisation de la direction par sécurité
  Vector3 dir = Vector3Normalize(direction);
  // Point d'apparition un peu devant pour ne pas se tirer dessus
  Vector3 spawn = Vector3Add(startPos, Vector3Scale(dir, 0.8f));

  for (int i = 0; i < MAX_PROJ; i++) {
    if (!projs[i].active) {
      projs[i].active = true;
      projs[i].pos = spawn;
      projs[i].vel =
          Vector3Scale(dir, arme.vitesseProj);  // Vitesse du projectile
      projs[i].radius = arme.tailleProjectile;
      projs[i].color = arme.couleurProjectile;
      projs[i].life = 5.0f;
      projs[i].degats = arme.degats;
      projs[i].owner = owner;  // <-- On définit le propriétaire
      projs[i].yaw = camYaw * RAD2DEG;
      projs[i].pitch = camPitch * RAD2DEG;
      projs[i].touche =false;
      switch (arme.type) {
        case PISTOLET:
          projs[i].type = PROJ_PISTOLET;
          PlayPistolet();
          break;

        case FUSIL:
          projs[i].type = PROJ_FUSIL;
          PlayMitraillette();
          break;

        case SNIPER:
          projs[i].type = PROJ_SNIPER;
          PlayPompe();
          break;

        case GRENADE:
          projs[i].type = PROJ_GRENADE;
          break;

        default:
          break;
      }
      break;
    }
  }
}


static bool colision(Vector3 projsPos, Vector3 pos, float r, float h){
  return (fabsf(projsPos.x - pos.x) < (r + h) &&
            fabsf(projsPos.y - (pos.y + h)) < (r + h) &&
            fabsf(projsPos.z - pos.z) < (r + h));
}

// ... (Début du fichier identique)

void UpdateProjectiles(Projectile* projs, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                       Entity** autre, Entity* player,
                       GameScreen* currentScreen, bool* IsBossAlive,
                       Entity* boss) {
  float dt = GetFrameTime();
  for (int i = 0; i < MAX_PROJ; i++) {
    if (!projs[i].active) continue;
    // On gere les calculs de pos "à la main" pour la grenade
    // Gestion du rebond avec le sol pour la grenade
    if (projs[i].type == PROJ_GRENADE) {
      gravite(&projs[i], dt);  // on mets a jours vel.y
      Vector3 distance = {projs[i].vel.x * dt, projs[i].vel.y * dt,
                          projs[i].vel.z * dt};
      float elasticite = 0.6;
      if (projs[i].pos.y != 0)
        projs[i].pos = Vector3Add(
            projs[i].pos,
            distance);  // on met a jours pos attention si pos.y=0, elle y reste
      else {
        projs[i].pos.x = projs[i].pos.x + distance.x;
        projs[i].pos.z = projs[i].pos.z + distance.z;
      }
      if (projs[i].pos.y < projs[i].radius &&
          projs[i].pos.y != 0) {           // si futur pos.y < tailleballe
        projs[i].pos.y = projs[i].radius;  // avoir une belle balle en surface
        if (projs[i].vel.y < -0.5f) {
          projs[i].vel.y = projs[i].vel.y * -elasticite;  // el famoso rebondo
          // 3. FRICTION : On freine  sur les côtés
          projs[i].vel.x = projs[i].vel.x * 0.5f;
          projs[i].vel.z = projs[i].vel.z * 0.5f;
        }
        if (-0.2f < projs[i].vel.y &&
            projs[i].vel.y < 0.2) {  // velocité y trop faible alors immobile
          projs[i].vel.y = 0;
        }
      }
      projs[i].life -= dt;
      if (projs[i].life <= 0.0f && projs[i].pos.y != 0)
        explosion(&projs[i]);  // pas deja explosé alors boom
    } else {
      projs[i].pos =
          Vector3Add(projs[i].pos,Vector3Scale(projs[i].vel,dt));  // sinon on calcul la nouvelle position
                                         // pour tout les autres projectiles
      projs[i].life -= dt;
    }
    if (projs[i].life <= 0.0f && projs[i].type != PROJ_GRENADE) {
      projs[i].active = false;
      continue;
    }

    // --- NOUVELLE LOGIQUE DE COLLISION AABB ---

    // 1. MES BALLES touchent l'AUTRE (Bot ou RemotePlayer) OU me touchent MOI
    if (projs[i].owner == OWNER_PLAYER) {
      if ((*autre)->type == ENTITY_REMOTE_PLAYER) {
        float h = ((*autre)->size / 2.0f);
        float r = projs[i].radius;
        // Condition pour que le projectile touche un Bot RP
        if (colision(projs[i].pos, (*autre)->pos, r, h)){
          if (projs[i].type != PROJ_GRENADE) projs[i].active = false;
          continue;
        }
      } else if ((*autre)[0].type == ENTITY_BOT) {
        for (int j = 0; j < 18; j++) {
          float h = ((*autre)[j].size / 2.0f);
          float r = projs[i].radius;

          if (colision(projs[i].pos,(*autre)[j].pos,r,h)) {
            (*autre)[j].health -= projs[i].degats;
            if (projs[i].type != PROJ_GRENADE) projs[i].active = false;

            if ((*autre)[j].health <= 0) {
              player->score += 1;
              InitBot(&(*autre)[j], blocks);
            }
            break;
          }
        }
      }
      if (boss != NULL && boss->type == ENTITY_BOSS && *IsBossAlive) {
        float h = (boss->size / 2.0f);
        float r = projs[i].radius;

        if (colision(projs[i].pos,boss->pos,r,h)) {
          if (projs[i].type == PROJ_GRENADE) {
            if (projs[i].touche == false && projs[i].degats!=0) { 
                boss->health -= projs[i].degats;
                projs[i].touche = true; // Verrouille pour cette grenade précise
            }
            // Si touche est true on fait rien
          } 
          else {
            // Pour les balles/autres : dégâts + destruction immédiate
            boss->health -= projs[i].degats;
            projs[i].active = false; 
          }
          if (boss->health <= 0) {
            player->score += 25;
            boss->health = boss->maxHealth;
            *IsBossAlive = false;  // Le boss meurt

            // 2. LA MAGIE : On téléporte le cadavre du boss en dessous du sol
            // Comme ça, l'explosion ne peut plus le toucher !
            boss->pos.y = -1000.0f;
          }
        }
      }
      if (projs[i].type == PROJ_GRENADE) {
        // A. Vérification de la collision avec le Joueur
        if (player->health > 0) {
          float h = player->size / 2.0f;
          float r = projs[i].radius;

          if (colision(projs[i].pos,player->pos,r,h)) {
            player->health -= projs[i].degats;
            // if (projs[i].type != PROJ_GRENADE) projs[i].active = false;

            // --- GESTION DE LA MORT EN SOLO ---
            if (projs[i].owner == OWNER_PLAYER && player->health <= 0) {
              player->life -= 1;
              if (player->life <= 0) {
                TraceLog(LOG_INFO, "Game Over !");
                *currentScreen = GAME_OVER;
              } else {
                player->health = player->maxHealth;
                player->ammo = player->armeEquipee.munitionsMax;
                player->pos =
                    (Vector3){1.5f, 10.0f, 1.5f};  // Position de respawn solo
                player->velocityY = 0;             // IMPORTANT : stop la chute
                TraceLog(LOG_INFO, "Mort en solo ! Respawn...");
                for (int p=0;p<MAX_PROJ;p++){
                  projs[i].active=false;
                }
              }
            }
          }
        }
      }
    }

    // 2. LES BALLES ENNEMIES (Bot ou Remote ou mes propres grenades !) me
    // touchent MOI
    else if (projs[i].owner == OWNER_BOSS ||
             projs[i].owner == OWNER_REMOTE_PLAYER ||
             projs[i].owner == OWNER_BOT ||
             (projs[i].owner == OWNER_PLAYER &&
              projs[i].type == PROJ_GRENADE)) {
      bool aToucheJoueur = false;

      // A. Vérification de la collision avec le Joueur
      if (player->health > 0) {
        float h = player->size / 2.0f;
        float r = projs[i].radius;

        if (fabsf(projs[i].pos.x - player->pos.x) < (r + h) &&
            fabsf(projs[i].pos.y - (player->pos.y + h)) < (r + h) &&
            fabsf(projs[i].pos.z - player->pos.z) < (r + h)) {
          player->health -= projs[i].degats;
          if (projs[i].type != PROJ_GRENADE) projs[i].active = false;
          aToucheJoueur = true;

          // --- GESTION DE LA MORT EN SOLO ---
          if (projs[i].owner == OWNER_BOT && player->health <= 0) {
            player->life -= 1;
            if (player->life <= 0) {
              TraceLog(LOG_INFO, "Game Over !");
              *currentScreen = GAME_OVER;
            } else {
              player->health = player->maxHealth;
              player->ammo = player->armeEquipee.munitionsMax;
              player->pos =
                  (Vector3){1.5f, 10.0f, 1.5f};  // Position de respawn solo
              player->velocityY = 0;             // IMPORTANT : stop la chute
              TraceLog(LOG_INFO, "Mort en solo ! Respawn...");
              for (int p = 0; p < MAX_PROJ; p++) {
                projs[p].active = false;
              }
            }
          } else if (projs[i].owner == OWNER_BOSS && player->health <= 0) {
            player->life -= 1;
            if (player->life <= 0) {
              TraceLog(LOG_INFO, "Game Over !");
              *currentScreen = GAME_OVER;
              *IsBossAlive = false;
            } else {
              player->health = player->maxHealth;
              player->ammo = player->armeEquipee.munitionsMax;
              player->pos =
                  (Vector3){1.5f, 10.0f, 1.5f};  // Position de respawn solo
              player->velocityY = 0;             // IMPORTANT : stop la chute
              TraceLog(LOG_INFO, "Mort en solo ! Respawn...");
              for (int p = 0; p < MAX_PROJ; p++) {
                projs[p].active = false;
              }
            }
          }
        }
      }

      // B. Vérification de la collision avec d'autres Bots (seulement si le
      // projectile appartient à un bot et n'a pas déjà touché le joueur)
      if (!aToucheJoueur && projs[i].owner == OWNER_BOSS) {
        for (int j = 0; j < 18; j++) {
          if ((*autre)[j].type != ENTITY_BOT) continue;

          float h = ((*autre)[j].size / 2.0f);
          float r = projs[i].radius;

          if (colision(projs[i].pos,(*autre)[j].pos,r,h)) {
            (*autre)[j].health -= projs[i].degats;
            if (projs[i].type != PROJ_GRENADE) projs[i].active = false;

            if ((*autre)[j].health <= 0) {
              (*autre)[j].health = (*autre)[j].maxHealth;
              (*autre)[j].pos = (Vector3){(float)(rand() % NUM_BLOCKS), 10.0f,
                                          (float)(rand() % NUM_BLOCKS)};
              (*autre)[j].velocityY = 0;
            }
            break;  // Le projectile a touché ce bot, on arrête de tester les
                    // autres
          }
        }
      }

      if (aToucheJoueur)
        continue;  // Passe au projectile suivant si le joueur a été touché
    }
    // pour la grenade on desactive uniquement si sa vie est finie pas quand le
    // bot meurt sinon pas de zone d'explosion
    if (projs[i].life <= 0) {
      projs[i].active = false;
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
          if (projs[i].type == PROJ_GRENADE) {
            RebondirGrenade(&(projs[i]), &b);
          } else {
            projs[i].active = false;
          }
          break;
        }
      }
      if (!projs[i].active) break;
    }
  }
}
