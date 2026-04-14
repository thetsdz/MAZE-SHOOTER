
#include "arme.h"

ModeleArme ObtenirModeleArme(TypeArme type) {
    ModeleArme m;
    switch (type) {
    case PISTOLET:
        m.type = PISTOLET;
        m.munitionsMax = 12;
        m.cadenceTir = 0.25f;
        m.vitesseProj = 9.0f;
        m.degats = 20;
        m.nom = "Pistolet";
        m.tailleProjectile = 0.015f;  // Taille standard
        m.couleurProjectile = YELLOW; // Jaune classique
        break;

    case FUSIL:
        m.type = FUSIL;
        m.munitionsMax = 30;
        m.cadenceTir = 0.1f;
        m.vitesseProj = 10.50f;
        m.degats = 15;
        m.nom = "Fusil d'assaut";
        m.tailleProjectile =0.02f; // un peu plus petit que pistolet car moins précis
        m.couleurProjectile = ORANGE; // Orange pour les traçantes
        break;

    case SNIPER:
        m.type = SNIPER;
        m.munitionsMax = 5;
        m.cadenceTir = 3.0f;
        m.vitesseProj = 20.0f; // Très rapide !
        m.degats = 100;
        m.nom = "Fusil de precision";
        // --- AJOUTS ---
        m.tailleProjectile = 0.03f; // Grosse balle i
        m.couleurProjectile = RED;  // Rouge "danger"
        break;

    case GRENADE:
        m.type = GRENADE;
        m.munitionsMax = 3;    // On en a peu
        m.cadenceTir = 1.0f;   // Temps de lancer
        m.vitesseProj = 20.0f; // Très lent par rapport à une balle !
        m.degats =0; // fera des degats de zone  gerer directement dans projectiles.c
        m.nom = "Grenade MK2";
        // --- AJOUTS ---
        m.tailleProjectile = 0.05f;
        m.couleurProjectile = GREEN; // Vert militaire
        break;

    default: // Sécurité
        m = ObtenirModeleArme(PISTOLET);
        break;
    }
    return m;
}


void ChangementArme(Entity* joueur) {
  if (joueur->score>=0) joueur->armeUnlock[0]=0;
  if (joueur->score>=5) joueur->armeUnlock[1]=0;
  if (joueur->score>=10) joueur->armeUnlock[2]=0;
  if (joueur->score>=20) joueur->armeUnlock[3]=0;
  // F1 : Pistolet
  if (IsKeyPressed(KEY_F1) && joueur->armeUnlock[0]==0 && joueur->armeEquipee.type!=PISTOLET) {
    joueur->armeEquipee = ObtenirModeleArme(PISTOLET);
    joueur->ammo =joueur->tabammo[0];  
  }
  // F2 : Fusil
  if (IsKeyPressed(KEY_F3) &&  joueur->armeUnlock[2]==0 && joueur->armeEquipee.type!=FUSIL) {
    joueur->armeEquipee = ObtenirModeleArme(FUSIL);
    joueur->ammo = joueur->tabammo[2];
  }
  // F3 : Sniper
  if (IsKeyPressed(KEY_F2) && joueur->armeUnlock[1]==0 && joueur->armeEquipee.type!=SNIPER ) {
    joueur->armeEquipee = ObtenirModeleArme(SNIPER);
    joueur->ammo = joueur->tabammo[1];
  }
  if (IsKeyPressed(KEY_F4) &&  joueur->armeUnlock[2]==0 && joueur->armeEquipee.type!=GRENADE ) { 
    joueur->armeEquipee = ObtenirModeleArme(GRENADE);
    joueur->ammo = joueur->tabammo[3];
  }
}
