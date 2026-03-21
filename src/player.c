/**
 * \file player.c
 */

#include "../lib/headers/player.h"

#include <math.h>

#include "../lib/headers/arme.h"
#include "../lib/headers/audio.h"

void InitPlayer(Entity* player) {
  player->pos = (Vector3){0, 10.0f, 0};
  player->yaw = 0.0f;            //
  player->pitch = 0.0f;          //
  player->velocityY = 0.0f;      // vitesse du joeur
  player->onGround = true;       // est-ce que le joueur est au sol
  player->size = 1.0f;           // taille
  player->health = 100;          // Points de vie de base
  player->maxHealth = 100;       // Points de vie maximum
  player->life = 3;              // Nombre de vies de base
  player->type = ENTITY_PLAYER;  // type de l'entité
  player->armeEquipee = ObtenirModeleArme(
      PISTOLET);  //  On charge la "fiche technique" du pistolet
  player->ammo =
      player->armeEquipee
          .munitionsMax;  // On remplit les munitions au maximum défini par le
                          // modèle (ici 12 ou 10 selon ton choix)
  player->chronoTir =
      0.0f;  // On initialise le chrono à 0 pour pouvoir tirer immédiatement
}

void UpdatePlayer(Entity* player, Block blocks[NUM_BLOCKS][NUM_BLOCKS],
                  Camera3D* camera, Entity** ennemi) {
  // Constantes de gameplay
  float speed = 0.1f;         // vitesse par défaut
  float gravity = 0.02f;      // gravité par defaut
  float jumpStrength = 0.4f;  // force de saut

  // --- Gestion de la Caméra (Souris) ---
  // On récupère le déplacement de la souris depuis la dernière frame
  Vector2 mouseDelta = GetMouseDelta();

  // Modification des angles (sensibilité 0.003f)
  player->yaw -= mouseDelta.x * 0.003f;    // Tourner gauche/droite
  player->pitch -= mouseDelta.y * 0.003f;  // Regarder haut/bas

  // Clamp (limitation) du pitch pour ne pas se tordre le cou
  // 1.5 radians correspond environ à 85 degrés (presque à la verticale)
  if (player->pitch > 1.5f) player->pitch = 1.5f;
  if (player->pitch < -1.5f) player->pitch = -1.5f;

  // Calcul du vecteur "Devant" (Forward) uniquement sur le plan horizontal (XZ)
  // Utile pour se déplacer sans s'envoler quand on regarde en l'air
  Vector3 forward = {sinf(player->yaw), 0, cosf(player->yaw)};

  // --- Saut ---
  if (IsKeyPressed(KEY_SPACE) && player->onGround) {
    player->velocityY = jumpStrength;
    player->onGround = false;  // On quitte le sol
  }

  // --- Gravité ---
  player->velocityY -= gravity;  // On réduit la vitesse Y à chaque frame

  // --- Calcul du Mouvement (Clavier) ---
  Vector3 nextPos = player->pos;  // Position hypothétique future
  Vector3 move = {0, 0, 0};

  // On ajoute les vecteurs directionnels selon les touches
  if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {  // QWERTY PT*

    move.x += forward.x;
    move.z += forward.z;
    if (player->onGround == true)
      PlayWalk();
    else {
      PauseWalk();
    }
  } else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
    move.x -= forward.x;
    move.z -= forward.z;
    if (player->onGround == true)
      PlayWalk();
    else {
      PauseWalk();
    }
  }
  // Pour aller à gauche/droite, on inverse X et Z du vecteur forward
  // (Mathématiquement : vecteur orthogonal)
  else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
    move.x += forward.z;
    move.z -= forward.x;
    if (player->onGround == true)
      PlayWalk();
    else {
      PauseWalk();
    }
  } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
    move.x -= forward.z;
    move.z += forward.x;
    if (player->onGround == true)
      PlayWalk();
    else {
      PauseWalk();
    }
  } else {
    PauseWalk();
  }
  PlayGameMusic();

  // Normalisation : Si on appuie sur W et D en même temps, la longueur du
  // vecteur est 1.41 (racine de 2). On doit le ramener à 1.0 pour ne pas courir
  // plus vite en diagonale.
  float moveLen = sqrtf(move.x * move.x + move.z * move.z);
  if (moveLen > 0.00001f) {
    move.x = (move.x / moveLen) * speed;
    move.z = (move.z / moveLen) * speed;
    nextPos.x += move.x;
    nextPos.z += move.z;
  }

  // On applique le mouvement vertical
  nextPos.y += player->velocityY;

  // --- Collisions avec les Blocs (AABB) ---
  // AABB = Axis Aligned Bounding Box (Boîte englobante alignée sur les axes)
  player->onGround = false;  // On suppose qu'on est en l'air avant de vérifier
  float playerHalf = player->size / 2.0f;

  // --- COLLISION AVEC L'ENNEMI (GLISSEMENT + SAUT SUR LA TÊTE) ---
  if ((*ennemi)->type == ENTITY_REMOTE_PLAYER) {
    float enemyHalf = (*ennemi)->size / 2.0f;
    float enemyTop = (*ennemi)->pos.y + enemyHalf;
    float enemyBottom = (*ennemi)->pos.y - enemyHalf;

    // Détection AABB (Axis Aligned Bounding Box)
    bool collideX = nextPos.x + playerHalf > (*ennemi)->pos.x - enemyHalf &&
                    nextPos.x - playerHalf < (*ennemi)->pos.x + enemyHalf;
    bool collideY = nextPos.y + playerHalf > (*ennemi)->pos.y - enemyHalf &&
                    nextPos.y - playerHalf < (*ennemi)->pos.y + enemyHalf;
    bool collideZ = nextPos.z + playerHalf > (*ennemi)->pos.z - enemyHalf &&
                    nextPos.z - playerHalf < (*ennemi)->pos.z + enemyHalf;

    if (collideX && collideY && collideZ) {
      // CAS 1 : On tombe sur la tête de l'autre joueur (on atterrit)
      if (player->velocityY < 0 && player->pos.y - playerHalf >= enemyTop) {
        nextPos.y = enemyTop + playerHalf;  // On se pose sur lui
        player->velocityY = 0;
        player->onGround = true;  // On peut sauter depuis sa tête !
      }
      // CAS 2 : On cogne l'autre joueur par le bas (en sautant)
      else if (player->velocityY > 0 &&
               player->pos.y + playerHalf <= enemyBottom) {
        nextPos.y = enemyBottom - playerHalf;
        player->velocityY = 0;
      }
      // CAS 3 : Collision latérale (Glissement fluide)
      else {
        float overlapX =
            (enemyHalf + playerHalf) - fabsf(nextPos.x - (*ennemi)->pos.x);
        float overlapZ =
            (enemyHalf + playerHalf) - fabsf(nextPos.z - (*ennemi)->pos.z);

        if (overlapX < overlapZ) {
          if (nextPos.x < (*ennemi)->pos.x)
            nextPos.x = (*ennemi)->pos.x - enemyHalf - playerHalf;
          else
            nextPos.x = (*ennemi)->pos.x + enemyHalf + playerHalf;
        } else {
          if (nextPos.z < (*ennemi)->pos.z)
            nextPos.z = (*ennemi)->pos.z - enemyHalf - playerHalf;
          else
            nextPos.z = (*ennemi)->pos.z + enemyHalf + playerHalf;
        }
      }
    }
  } else {
    for (int i = 0; i < 18; i++) {
      float enemyHalf = (*ennemi)[i].size / 2.0f;
      float enemyTop = (*ennemi)[i].pos.y + enemyHalf;
      float enemyBottom = (*ennemi)[i].pos.y - enemyHalf;

      // Détection AABB (Axis Aligned Bounding Box)
      bool collideX = nextPos.x + playerHalf > (*ennemi)[i].pos.x - enemyHalf &&
                      nextPos.x - playerHalf < (*ennemi)[i].pos.x + enemyHalf;
      bool collideY = nextPos.y + playerHalf > (*ennemi)[i].pos.y - enemyHalf &&
                      nextPos.y - playerHalf < (*ennemi)[i].pos.y + enemyHalf;
      bool collideZ = nextPos.z + playerHalf > (*ennemi)[i].pos.z - enemyHalf &&
                      nextPos.z - playerHalf < (*ennemi)[i].pos.z + enemyHalf;

      if (collideX && collideY && collideZ) {
        // CAS 1 : On tombe sur la tête de l'autre joueur (on atterrit)
        if (player->velocityY < 0 && player->pos.y - playerHalf >= enemyTop) {
          nextPos.y = enemyTop + playerHalf;  // On se pose sur lui
          player->velocityY = 0;
          player->onGround = true;  // On peut sauter depuis sa tête !
        }
        // CAS 2 : On cogne l'autre joueur par le bas (en sautant)
        else if (player->velocityY > 0 &&
                 player->pos.y + playerHalf <= enemyBottom) {
          nextPos.y = enemyBottom - playerHalf;
          player->velocityY = 0;
        }
        // CAS 3 : Collision latérale (Glissement fluide)
        else {
          float overlapX =
              (enemyHalf + playerHalf) - fabsf(nextPos.x - (*ennemi)[i].pos.x);
          float overlapZ =
              (enemyHalf + playerHalf) - fabsf(nextPos.z - (*ennemi)[i].pos.z);

          if (overlapX < overlapZ) {
            if (nextPos.x < (*ennemi)[i].pos.x)
              nextPos.x = (*ennemi)[i].pos.x - enemyHalf - playerHalf;
            else
              nextPos.x = (*ennemi)[i].pos.x + enemyHalf + playerHalf;
          } else {
            if (nextPos.z < (*ennemi)[i].pos.z)
              nextPos.z = (*ennemi)[i].pos.z - enemyHalf - playerHalf;
            else
              nextPos.z = (*ennemi)[i].pos.z + enemyHalf + playerHalf;
          }
        }
      }
    }
  }
  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];
      float halfX = b.width / 2;
      float halfY = b.height / 2;
      float halfZ = b.depth / 2;

      // Vérification : Est-ce que le cube "joueur" chevauche le cube "bloc" sur
      // TOUS les axes ?
      bool collideX = nextPos.x + playerHalf > b.pos.x - halfX &&
                      nextPos.x - playerHalf < b.pos.x + halfX;
      bool collideY = nextPos.y + playerHalf > b.pos.y - halfY &&
                      nextPos.y - playerHalf < b.pos.y + halfY;
      bool collideZ = nextPos.z + playerHalf > b.pos.z - halfZ &&
                      nextPos.z - playerHalf < b.pos.z + halfZ;

      if (collideX && collideY && collideZ) {
        // Collision détectée ! On doit résoudre le conflit.
        float top = b.pos.y + halfY;
        float bottom = b.pos.y - halfY;

        // CAS 1 : On tombe sur le bloc (Vélocité négative et on était
        // au-dessus)
        if (player->velocityY < 0 && player->pos.y - playerHalf >= top) {
          nextPos.y = top + playerHalf;  // On se pose pile dessus
          player->velocityY = 0;         // On arrête de tomber
          player->onGround = true;       // On peut sauter à nouveau
        }
        // CAS 2 : On cogne le bloc par le bas (Saut)
        else if (player->velocityY > 0 &&
                 player->pos.y + playerHalf <= bottom) {
          nextPos.y = bottom - playerHalf;
          player->velocityY = 0;  // On s'arrête net (on se cogne la tête)
        }
        // CAS 3 : Collision latérale (Mur)
        else {
          // Calcule du taux de penetration dans le bloc sur chaque axe
          // fabsf() donne la valeur absolue
          float overlapX = (halfX + playerHalf) - fabsf(nextPos.x - b.pos.x);
          float overlapZ = (halfZ + playerHalf) - fabsf(nextPos.z - b.pos.z);

          // Choix de l'axe de moindre pénétration
          // "sortie la plus rapide" pour ne plus être en collision
          if (overlapX < overlapZ) {
            // --- CHOC SUR L'AXE X (Mur à gauche ou à droite) ---
            // Si on est à gauche du mur, on se met à sa gauche, sinon à sa
            // droite
            if (nextPos.x < b.pos.x)
              nextPos.x = b.pos.x - halfX - playerHalf;
            else
              nextPos.x = b.pos.x + halfX + playerHalf;

            // IMPORTANT : On ne touche PAS à nextPos.z ! (si vous y touchez je
            // vous bute)
          } else {
            // --- CHOC SUR L'AXE Z (Mur devant ou derrière) ---
            if (nextPos.z < b.pos.z)
              nextPos.z = b.pos.z - halfZ - playerHalf;
            else
              nextPos.z = b.pos.z + halfZ + playerHalf;

            // IMPORTANT : On ne touche PAS à nextPos.x !(idem)
          }
        }
      }
    }
  }

  // --- Gravité et sol réel (inclut les couloirs) ---
  float playerBottom = nextPos.y - playerHalf;
  float closestGround = 0.0f;  // Sol Raylib par défaut

  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      Block b = blocks[i][j];
      // On tombe sur tous les blocs "vides" (couloirs)
      if (b.color.a != 0) continue;

      float top = b.pos.y + b.height / 2.0f;
      if (playerBottom >= top && nextPos.y - playerHalf <= top) {
        if (top > closestGround) closestGround = top;
      }
    }
  }

  // Appliquer la gravité et la position sur le sol le plus haut sous le joueur
  if (playerBottom <= closestGround) {
    nextPos.y = closestGround + playerHalf;
    player->velocityY = 0;
    player->onGround = true;
  }

  // Validation finale de la position
  player->pos = nextPos;

  if (player->health < 20) PlayHeart();

  // --- Mise à jour de la Caméra Raylib ---
  // Calcul du vecteur direction 3D complet (sphérique -> cartésien)
  Vector3 camForward = {
      sinf(player->yaw) * cosf(player->pitch),  // X
      sinf(player->pitch),                      // Y
      cosf(player->yaw) * cosf(player->pitch)   // Z
  };

  // La caméra est placée sur le joueur (+0.5f pour être au niveau des yeux)
  camera->position =
      (Vector3){player->pos.x, player->pos.y + 0.5f, player->pos.z};
  // Le point visé est : Position + Direction
  camera->target = Vector3Add(camera->position, camForward);
}