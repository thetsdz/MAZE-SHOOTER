/**
 * \file asset.c
 */

#include "../lib/headers/asset.h"
#include <stdio.h>

Texture2D ChargerTexture(const char *fileName) {
    Texture2D texture = LoadTexture(fileName);
    if (texture.id == 0) {
        printf("ERREUR: Impossible de charger l'image : %s\n", fileName);
    }
    return texture;
}

void DessinerViseur(Texture2D texture, int screenWidth, int screenHeight) {
    int posX = (screenWidth / 2) - (texture.width / 2);
    int posY = (screenHeight / 2) - (texture.height / 2);
    DrawTexture(texture, posX, posY, WHITE);
}

void DessinerArme(Model ModeleArme, int indexArme) {
    if (ModeleArme.meshes == NULL)
        return;

    Camera3D cameraHUD = {0};
    cameraHUD.position = (Vector3){-0.25f, 0.15f, 8.5f};
    cameraHUD.target = (Vector3){0.0f, 0.0f, 0.0f};
    cameraHUD.up = (Vector3){0.0f, 1.0f, 0.0f};
    cameraHUD.fovy = 50.0f;
    cameraHUD.projection = CAMERA_PERSPECTIVE;

    BeginMode3D(cameraHUD);

    // --- ÉTAPE A : CENTRE ET DISTANCE 2m ---
    // X=0, Y=0 (Plein centre)
    // Z=8.0f (Distance de 2.0 par rapport à la caméra qui est à 10.0)
    Vector3 position = {0.0f, 0.0f, 8.0f};

    float angleRotation = 90.0f;
    Vector3 axeRotation = {0.0f, 1.0f, 0.0f};
    float echelle = 0.05f;
    if (indexArme == 3)
        echelle = 0.1f;
    DrawModelEx(ModeleArme, position, axeRotation, angleRotation,
                (Vector3){echelle, echelle, echelle}, WHITE);

    EndMode3D();
}