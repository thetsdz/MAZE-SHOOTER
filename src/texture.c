/**
 * \file texture.c
 * \brief Chargement, déchargement et rechargement des assets graphiques.
 */

#include "../lib/headers/texture.h"
#include "../lib/headers/menu.h"
#include "../lib/headers/option.h"
#include "raylib.h"
#include "rlgl.h"

/* ------------------------------------------------------------------ */
/*  Helpers internes                                                    */
/* ------------------------------------------------------------------ */

/** Charge les textures et modèles du niveau (murs + sol) dans assets. */
static void ChargerModeleNiveau(GameAssets *assets, const char *wallPath,
                                const char *floorPath) {
    assets->wallTex = LoadTexture(wallPath);
    assets->floorTex = LoadTexture(floorPath);

    Mesh wallMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    assets->wallModel = LoadModelFromMesh(wallMesh);
    assets->wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
        assets->wallTex;

    Mesh floorMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    assets->floorModel = LoadModelFromMesh(floorMesh);
    assets->floorModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
        assets->floorTex;
}

/** Décharge uniquement les modèles et textures du niveau. */
static void DechargerModeleNiveau(GameAssets *assets) {
    UnloadTexture(assets->wallTex);
    UnloadTexture(assets->floorTex);
    UnloadModel(assets->wallModel);
    UnloadModel(assets->floorModel);
}

/* ------------------------------------------------------------------ */
/*  API publique                                                        */
/* ------------------------------------------------------------------ */

void LoadGameAssets(GameAssets *assets) {
    /* --- Armes --- */
    assets->tabArmes[0] = LoadModel("../assets/models/armes/Pistolet.glb");
    assets->tabArmes[1] = LoadModel("../assets/models/armes/Fusil_assault.glb");
    assets->tabArmes[2] = LoadModel("../assets/models/armes/Sniper.glb");
    assets->tabArmes[3] = LoadModel("../assets/models/armes/Grenade.glb");

    assets->iconesArmes[0] =
        LoadTexture("../assets/images/icone_arme/icone_pistolet.png");
    assets->iconesArmes[1] =
        LoadTexture("../assets/images/icone_arme/icone_sniper.png");
    assets->iconesArmes[2] =
        LoadTexture("../assets/images/icone_arme/icone_fusil.png");
    assets->iconesArmes[3] =
        LoadTexture("../assets/images/icone_arme/icone_grenade.png");

    /* --- HUD --- */
    assets->viseur = LoadTexture("../assets/images/crosshair.png");

    /* --- Entités --- */
    assets->botModel = LoadModel("../assets/models/robots/Robot.glb");
    assets->bossModel = LoadModel("../assets/models/boss/boss.glb");
    assets->healModel = LoadModel("../assets/models/coffre/Chest2.glb");

    /* --- Projectiles --- */
    assets->tabProjModels[0] =
        LoadModel("../assets/models/projectiles/Bullet_pistolet.glb");
    assets->tabProjModels[1] =
        LoadModel("../assets/models/projectiles/Bullet_fusil_assault.glb");
    assets->tabProjModels[2] =
        LoadModel("../assets/models/projectiles/Bullet_sniper3.glb");
    assets->tabProjModels[3] =
        LoadModel("../assets/models/projectiles/Grenade.glb");
    assets->tabProjModels[4] =
        LoadModel("../assets/models/projectiles/Explosion.glb");

    /* --- Niveau (thème par défaut) --- */
    ChargerModeleNiveau(assets, "../assets/images/brick.png",
                        "../assets/images/concrete.png");

    /* --- Skybox --- */
    Mesh skyMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    assets->skyModel = LoadModelFromMesh(skyMesh);
    assets->skyModel.materials[0].shader = LoadShader(
        "../assets/shaders/skybox.vs", "../assets/shaders/skybox.fs");

    Image skyImg = LoadImage("../assets/images/sky.png");
    ImageFormat(&skyImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    TextureCubemap cubemap =
        LoadTextureCubemap(skyImg, CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE);
    UnloadImage(skyImg);

    assets->skyModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubemap;

    Shader skyShader = assets->skyModel.materials[0].shader;

    int envMapLoc = GetShaderLocation(skyShader, "environmentMap");
    SetShaderValue(skyShader, envMapLoc, (int[]){MATERIAL_MAP_CUBEMAP},
                   SHADER_UNIFORM_INT);

    int val0 = 0;
    int doGammaLoc = GetShaderLocation(skyShader, "doGamma");
    int vflippedLoc = GetShaderLocation(skyShader, "vflipped");
    SetShaderValue(skyShader, doGammaLoc, &val0, SHADER_UNIFORM_INT);
    SetShaderValue(skyShader, vflippedLoc, &val0, SHADER_UNIFORM_INT);
}

void UnloadGameAssets(GameAssets *assets) {
    UnloadTexture(assets->viseur);

    for (int i = 0; i < 4; i++)
        UnloadModel(assets->tabArmes[i]);
    for (int i = 0; i < 4; i++)
        UnloadTexture(assets->iconesArmes[i]);
    for (int i = 0; i < 5; i++)
        UnloadModel(assets->tabProjModels[i]);

    UnloadModel(assets->botModel);
    UnloadModel(assets->bossModel);
    UnloadModel(assets->healModel);

    DechargerModeleNiveau(assets);

    UnloadShader(assets->skyModel.materials[0].shader);
    UnloadTexture(
        assets->skyModel.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture);
    UnloadModel(assets->skyModel);
}

void RechargerTheme(GameAssets *assets) {
    const ThemeInfo *theme = GetSelectedTheme();
    DechargerModeleNiveau(assets);
    ChargerModeleNiveau(assets, theme->wallTexPath, theme->floorTexPath);
}