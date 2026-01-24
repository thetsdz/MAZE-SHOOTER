#include "bot.h"
#include <math.h>

#define N 101  // Impair
#define M 101  // Impair

void InitBot(Bot *bot) {
    bot->pos = (Vector3){0, 10.0f, 0};
    bot->yaw = 0.0f;
    bot->pitch = 0.0f;
    bot->velocityY = 0.0f;
    bot->onGround = true;
    bot->size = 1.0f;
    bot->ammo = 10;     // Commence avec 10 balles
    bot->maxAmmo = 10;  // Capacité de base de 10
}

void UpdateBot(Bot *bot, Block blocks[N][M], Camera3D *camera) {
    // Constantes de gameplay
    float speed = 0.1f;
    float gravity = 0.02f;
    float jumpStrength = 0.4f;
    float normalHeight = 0.5f;
    float crouchHeight = 0.2f;

    // --- Gestion de la Caméra ---
    // Simuler les mouvements de souris du bot pour une rotation réaliste
    float time = GetFrameTime();
    static float scanTimer = 0.0f;
    scanTimer += time;
    
    // Modification des angles (sensibilité 0.003f)
    // Le bot tourne sa tête de façon réaliste (balayage lent)
    bot->yaw += sinf(scanTimer * 0.5f) * 0.02f;    // Tourner gauche/droite lentement
    bot->pitch = cosf(scanTimer * 0.3f) * 0.3f;    // Regarder haut/bas légèrement
    
    // Clamp (limitation) du pitch pour ne pas lui tordre le cou
    // 1.5 radians correspond environ à 85 degrés (presque à la verticale)
    if(bot->pitch > 1.5f) bot->pitch = 1.5f;
    if(bot->pitch < -1.5f) bot->pitch = -1.5f;

    // Calcul du vecteur "Devant" (Forward) uniquement sur le plan horizontal (XZ)
    // Utile pour se déplacer sans s'envoler quand il regarde en l'air
    Vector3 forward = {sinf(bot->yaw), 0, cosf(bot->yaw)};
    
    // --- Accroupissement ---
    float botHalf = bot->size/2;
    float baseY = normalHeight; // Hauteur minimale du sol
    // Le bot ne s'accroupit pas pour maintenant

    // --- Saut ---
    // Le bot saute occasionnellement
    if(fmodf(scanTimer, 5.0f) < 0.1f && bot->onGround) {
        bot->velocityY = jumpStrength;
        bot->onGround = false;
    }

    // --- Gravité ---
    bot->velocityY -= gravity; // On réduit la vitesse Y à chaque frame

    // --- Calcul du Mouvement ---
    Vector3 nextPos = bot->pos; // Position hypothétique future
    Vector3 move = {0, 0, 0};
    
    // Le bot avance régulièrement
    if(fmodf(scanTimer, 4.0f) < 2.0f) {
        move.x += forward.x;
        move.z += forward.z; 
    }
    
    // Normalisation : Si on avance et on va a droite en même temps, la longueur du vecteur est 1.41 (racine de 2).
    // On doit le ramener à 1.0 pour ne pas courir plus vite en diagonale.
    float moveLen = sqrtf(move.x*move.x + move.z*move.z);
    if(moveLen > 0.00001f){
        move.x = (move.x / moveLen) * speed;
        move.z = (move.z / moveLen) * speed;
        nextPos.x += move.x;
        nextPos.z += move.z;
    }
    
    // On applique le mouvement vertical
    nextPos.y += bot->velocityY;

    // --- Collisions avec les Blocs (AABB) ---
    // AABB = Axis Aligned Bounding Box (Boîte englobante alignée sur les axes)
    bot->onGround = false; // On suppose qu'on est en l'air avant de vérifier
    
    for(int i=0; i<NUM_BLOCKS; i++){
        for(int j=0; j<NUM_BLOCKS; j++){  
            Block b = blocks[i][j];
            float halfX = b.width/2;
            float halfY = b.height/2;
            float halfZ = b.depth/2;

            // Vérification : Est-ce que le cube "bot" chevauche le cube "bloc" sur TOUS les axes ?
            bool collideX = nextPos.x + botHalf > b.pos.x - halfX && nextPos.x - botHalf < b.pos.x + halfX;
            bool collideY = nextPos.y + botHalf > b.pos.y - halfY && nextPos.y - botHalf < b.pos.y + halfY;
            bool collideZ = nextPos.z + botHalf > b.pos.z - halfZ && nextPos.z - botHalf < b.pos.z + halfZ;

            if(collideX && collideY && collideZ){
                // Collision détectée ! On doit résoudre le conflit.
                float top = b.pos.y + halfY;
                float bottom = b.pos.y - halfY;

                // CAS 1 : On tombe sur le bloc (Vélocité négative et on était au-dessus)
                if(bot->velocityY < 0 && bot->pos.y - botHalf >= top){ 
                    nextPos.y = top + botHalf; // On se pose pile dessus
                    bot->velocityY = 0;        // On arrête de tomber
                    bot->onGround = true;      // On peut sauter à nouveau
                }
                // CAS 2 : On cogne le bloc par le bas (Saut)
                else if(bot->velocityY > 0 && bot->pos.y + botHalf <= bottom){ 
                    nextPos.y = bottom - botHalf;
                    bot->velocityY = 0;        // On s'arrête net (on se cogne la tête)
                } 
                // CAS 3 : Collision latérale (Mur)
                else {
                    // Calcule du taux de penetration dans le bloc sur chaque axe
                    // fabsf() donne la valeur absolue
                    float overlapX = (halfX + botHalf) - fabsf(nextPos.x - b.pos.x);
                    float overlapZ = (halfZ + botHalf) - fabsf(nextPos.z - b.pos.z);

                    // Choix de l'axe de moindre pénétration
                    // "sortie la plus rapide" pour ne plus être en collision
                    if (overlapX < overlapZ) {
                        // --- CHOC SUR L'AXE X (Mur à gauche ou à droite) ---
                        // Si on est à gauche du mur, on se met à sa gauche, sinon à sa droite
                        if (nextPos.x < b.pos.x) 
                            nextPos.x = b.pos.x - halfX - botHalf;
                        else 
                            nextPos.x = b.pos.x + halfX + botHalf;
                            
                        // IMPORTANT : On ne touche PAS à nextPos.z !
                    } 
                    else {
                        // --- CHOC SUR L'AXE Z (Mur devant ou derrière) ---
                        if (nextPos.z < b.pos.z)
                            nextPos.z = b.pos.z - halfZ - botHalf;
                        else
                            nextPos.z = b.pos.z + halfZ + botHalf;

                        // IMPORTANT : On ne touche PAS à nextPos.x !(idem)
                    }
                }
            }
        }
    }

    // --- Gravité et sol réel (inclut les couloirs) ---
    float botBottom = nextPos.y - botHalf;
    float closestGround = 0.0f; // Sol Raylib par défaut

    for(int i = 0; i < NUM_BLOCKS; i++){
        for(int j = 0; j < NUM_BLOCKS; j++){
            Block b = blocks[i][j];
            // On tombe sur tous les blocs "vides" (couloirs)
            if(b.color.a != 0) continue; 

            float top = b.pos.y + b.height / 2.0f; 
            if(botBottom >= top && nextPos.y - botHalf <= top){
                if(top > closestGround) closestGround = top;
            }
        }
    }

    // Appliquer la gravité et la position sur le sol le plus haut sous le bot
    if(botBottom <= closestGround){
        nextPos.y = closestGround + botHalf;
        bot->velocityY = 0;
        bot->onGround = true;
    }

    // Validation finale de la position
    bot->pos = nextPos;

    // --- Mise à jour de la Caméra Raylib ---
    // Calcul du vecteur direction 3D complet (sphérique -> cartésien)
    Vector3 camForward = {
        sinf(bot->yaw) * cosf(bot->pitch), // X
        sinf(bot->pitch),                  // Y
        cosf(bot->yaw) * cosf(bot->pitch)  // Z
    };
    
    // La caméra est placée sur le bot (+0.5f pour être au niveau des yeux)
    camera->position = (Vector3){bot->pos.x, bot->pos.y + 0.5f, bot->pos.z};
    // Le point visé est : Position + Direction
    camera->target = (Vector3){
        camera->position.x + camForward.x,
        camera->position.y + camForward.y,
        camera->position.z + camForward.z
    };
}
