#include "player.h"
#include <math.h>

void InitPlayer(Player *player) {
    player->pos = (Vector3){0, 0.5f, 0};
    player->yaw = 0.0f;
    player->pitch = 0.0f;
    player->velocityY = 0.0f;
    player->onGround = true;
    player->size = 1.0f;
    player->ammo = 10;     // Commence avec 10 balles
    player->maxAmmo = 10;  // Capacité de base de 10
}

void UpdatePlayer(Player *player, Block *blocks, Camera3D *camera) {
    // Constantes de gameplay
    float speed = 0.1f;
    float gravity = 0.02f;
    float jumpStrength = 0.4f;
    float normalHeight = 0.5f;
    float crouchHeight = 0.2f;

    // --- Gestion de la Caméra (Souris) ---
    // On récupère le déplacement de la souris depuis la dernière frame
    Vector2 mouseDelta = GetMouseDelta();
    
    // Modification des angles (sensibilité 0.003f)
    player->yaw -= mouseDelta.x * 0.003f;   // Tourner gauche/droite
    player->pitch -= mouseDelta.y * 0.003f; // Regarder haut/bas
    
    // Clamp (limitation) du pitch pour ne pas se tordre le cou
    // 1.5 radians correspond environ à 85 degrés (presque à la verticale)
    if(player->pitch > 1.5f) player->pitch = 1.5f;
    if(player->pitch < -1.5f) player->pitch = -1.5f;

    // Calcul du vecteur "Devant" (Forward) uniquement sur le plan horizontal (XZ)
    // Utile pour se déplacer sans s'envoler quand on regarde en l'air
    Vector3 forward = {sinf(player->yaw), 0, cosf(player->yaw)};
    
    // --- Accroupissement ---
    float playerHalf = player->size/2;
    float baseY = normalHeight; // Hauteur minimale du sol
    if(IsKeyDown(KEY_LEFT_SHIFT) && player->onGround) baseY = crouchHeight;

    // --- Saut ---
    if(IsKeyPressed(KEY_SPACE) && player->onGround){
        player->velocityY = jumpStrength;
        player->onGround = false; // On quitte le sol
    }

    // --- Gravité ---
    player->velocityY -= gravity; // On réduit la vitesse Y à chaque frame

    // --- Calcul du Mouvement (Clavier) ---
    Vector3 nextPos = player->pos; // Position hypothétique future
    Vector3 move = {0, 0, 0};
    
    // On ajoute les vecteurs directionnels selon les touches
    if(IsKeyDown(KEY_W)){ move.x += forward.x; move.z += forward.z; }
    if(IsKeyDown(KEY_S)){ move.x -= forward.x; move.z -= forward.z; }
    
    // Pour aller à gauche/droite, on inverse X et Z du vecteur forward
    // (Mathématiquement : vecteur orthogonal)
    if(IsKeyDown(KEY_A)){ move.x += forward.z; move.z -= forward.x; }
    if(IsKeyDown(KEY_D)){ move.x -= forward.z; move.z += forward.x; }

    // Normalisation : Si on appuie sur W et D en même temps, la longueur du vecteur est 1.41 (racine de 2).
    // On doit le ramener à 1.0 pour ne pas courir plus vite en diagonale.
    float moveLen = sqrtf(move.x*move.x + move.z*move.z);
    if(moveLen > 0.00001f){
        move.x = (move.x / moveLen) * speed;
        move.z = (move.z / moveLen) * speed;
        nextPos.x += move.x;
        nextPos.z += move.z;
    }
    
    // On applique le mouvement vertical
    nextPos.y += player->velocityY;

    // --- Collisions avec les Blocs (AABB) ---
    // AABB = Axis Aligned Bounding Box (Boîte englobante alignée sur les axes)
    player->onGround = false; // On suppose qu'on est en l'air avant de vérifier
    
    for(int i=0; i<NUM_BLOCKS; i++){
        Block b = blocks[i];
        float halfX = b.width/2;
        float halfY = b.height/2;
        float halfZ = b.depth/2;

        // Vérification : Est-ce que le cube "joueur" chevauche le cube "bloc" sur TOUS les axes ?
        bool collideX = nextPos.x + playerHalf > b.pos.x - halfX && nextPos.x - playerHalf < b.pos.x + halfX;
        bool collideY = nextPos.y + playerHalf > b.pos.y - halfY && nextPos.y - playerHalf < b.pos.y + halfY;
        bool collideZ = nextPos.z + playerHalf > b.pos.z - halfZ && nextPos.z - playerHalf < b.pos.z + halfZ;

        if(collideX && collideY && collideZ){
            // Collision détectée ! On doit résoudre le conflit.
            float top = b.pos.y + halfY;
            float bottom = b.pos.y - halfY;

            // CAS 1 : On tombe sur le bloc (Vélocité négative et on était au-dessus)
            if(player->velocityY < 0 && player->pos.y - playerHalf >= top){ 
                nextPos.y = top + playerHalf; // On se pose pile dessus
                player->velocityY = 0;        // On arrête de tomber
                player->onGround = true;      // On peut sauter à nouveau
            }
            // CAS 2 : On cogne le bloc par le bas (Saut)
            else if(player->velocityY > 0 && player->pos.y + playerHalf <= bottom){ 
                nextPos.y = bottom - playerHalf;
                player->velocityY = 0;        // On s'arrête net (on se cogne la tête)
            } 
            // CAS 3 : Collision latérale (Mur)
            else {
                // Annulation simple du mouvement horizontal
                nextPos.x = player->pos.x;
                nextPos.z = player->pos.z;
            }
        }
    }

    // --- Collision avec le Sol global ---
    if(nextPos.y <= baseY){
        nextPos.y = baseY;
        player->velocityY = 0;
        player->onGround = true;
    }

    // Validation finale de la position
    player->pos = nextPos;

    // --- Mise à jour de la Caméra Raylib ---
    // Calcul du vecteur direction 3D complet (sphérique -> cartésien)
    Vector3 camForward = {
        sinf(player->yaw) * cosf(player->pitch), // X
        sinf(player->pitch),                     // Y
        cosf(player->yaw) * cosf(player->pitch)  // Z
    };
    
    // La caméra est placée sur le joueur (+0.5f pour être au niveau des yeux)
    camera->position = (Vector3){player->pos.x, player->pos.y + 0.5f, player->pos.z};
    // Le point visé est : Position + Direction
    camera->target   = Vector3Add(camera->position, camForward);
}