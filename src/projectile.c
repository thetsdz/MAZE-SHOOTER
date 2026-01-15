#include "projectile.h"
#include <stdlib.h> // pour rand()



void InitProjectiles(Projectile *projs) {
    // On marque tous les slots comme inactifs
    // Cela évite d'utiliser malloc/free à chaque tir (plus performant)
    for(int i=0; i<MAX_PROJ; i++) projs[i].active = false;
}

void ShootProjectile(Projectile *projs, Player p) {
    
    // 1. Calcul de la direction du tir
    // C'est la même formule que pour la caméra dans player.c
    Vector3 camDir = { sinf(p.yaw)*cosf(p.pitch), sinf(p.pitch), cosf(p.yaw)*cosf(p.pitch) };
    camDir = Vector3Normalize(camDir); // On s'assure que la longueur du vecteur est 1

    // 2. Point d'apparition
    // On fait apparaître la balle un peu devant le joueur (Vector3Scale)
    // Sinon la balle spawnerait DANS le joueur et pourrait causer des bugs
    Vector3 spawn = Vector3Add((Vector3){p.pos.x, p.pos.y + 0.5f, p.pos.z}, Vector3Scale(camDir, p.size));

    // 3. Recherche d'un slot libre dans le tableau (Object Pooling)
    for(int i=0; i<MAX_PROJ; i++){
        if(!projs[i].active){
            projs[i].active = true;
            projs[i].pos = spawn;
            projs[i].vel = Vector3Scale(camDir, 20.0f); // Direction * Vitesse (20.0f)
            projs[i].radius = 0.2f;
            projs[i].life = 5.0f; // Disparaît après 5 secondes
            TraceLog(LOG_INFO, "Spawn projectile at (%f,%f,%f)", projs[i].pos.x, projs[i].pos.y, projs[i].pos.z);
            break; // On a tiré une balle, on arrête la boucle
        }
    }
}

void UpdateProjectiles(Projectile *projs, Block *blocks, Vector3 *targetPos, float targetRadius, int *score) {
    float dt = GetFrameTime(); // Temps écoulé depuis la dernière frame (ex: 0.016s)
    
    for(int i=0; i<MAX_PROJ; i++){
        if(!projs[i].active) continue; // On ignore les balles mortes
        
        // Mouvement : Position = Position + (Vitesse * Temps)
        projs[i].pos = Vector3Add(projs[i].pos, Vector3Scale(projs[i].vel, dt));
        
        // Gestion de la vie
        projs[i].life -= dt;
        if(projs[i].life <= 0.0f){ projs[i].active = false; continue; }

        // --- Collision avec la Cible (Sphère contre Sphère) ---
        // Si Distance < Rayon1 + Rayon2, alors ça touche
        if(Vector3Distance(projs[i].pos, *targetPos) <= projs[i].radius + targetRadius){
            TraceLog(LOG_INFO, "Projectile hit target!");
            (*score)++;
            // Déplace la cible aléatoirement
            targetPos->x = (float)(rand()%20-10);
            targetPos->z = (float)(rand()%20-10);
            projs[i].active = false; // Détruit la balle
            continue;
        }

        // --- Collision avec les Blocs (Point dans AABB) ---
        // Simplifié : on considère la balle comme un point pour collision avec murs
        for(int j=0; j<NUM_BLOCKS; j++){
            Block b = blocks[j];
            float halfX = b.width/2;
            float halfY = b.height/2;
            float halfZ = b.depth/2;
            
            // Vérifie si le point (balle) est à l'intérieur du rectangle (mur)
            if(projs[i].pos.x > b.pos.x - halfX && projs[i].pos.x < b.pos.x + halfX &&
               projs[i].pos.y > b.pos.y - halfY && projs[i].pos.y < b.pos.y + halfY &&
               projs[i].pos.z > b.pos.z - halfZ && projs[i].pos.z < b.pos.z + halfZ){
                
                projs[i].active = false; // La balle touche un mur et disparaît
                break;
            }
        }
    }
}

void DrawProjectiles(Projectile *projs) {
    for(int i=0; i<MAX_PROJ; i++){
        if(projs[i].active) DrawSphere(projs[i].pos, projs[i].radius, MAGENTA);
    }
}