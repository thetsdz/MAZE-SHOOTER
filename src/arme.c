


#include "arme.h"

ModeleArme ObtenirModeleArme(TypeArme type) {
    ModeleArme m;
    switch (type) {
        case PISTOLET:
            m.type = PISTOLET;
            m.munitionsMax = 12;
            m.cadenceTir = 0.25f;
            m.vitesseProj = 0.05f;
            m.degats = 20;
            m.nom = "Pistolet";
            m.tailleProjectile = 0.05f;    // Taille standard
            m.couleurProjectile = YELLOW; // Jaune classique
            break;

        case FUSIL:
            m.type = FUSIL;
            m.munitionsMax = 30;
            m.cadenceTir = 0.1f;
            m.vitesseProj = 005.0f;
            m.degats = 15;
            m.nom = "Fusil d'assaut";
            m.tailleProjectile = 0.05f;   // Un peu plus fin
            m.couleurProjectile = ORANGE;  // Orange pour les traçantes
            break;

        case SNIPER:
            m.type = SNIPER;
            m.munitionsMax = 5;
            m.cadenceTir = 1.5f;
            m.vitesseProj = 0.1f;      // Très rapide !
            m.degats = 100;
            m.nom = "Fusil de precision";
            // --- AJOUTS ---
            m.tailleProjectile = 0.9f;    // Grosse balle imposante
            m.couleurProjectile = RED;     // Rouge "danger"
            break;
            
        case GRENADE:
            m.type = GRENADE;
            m.munitionsMax = 3;           // On en a peu
            m.cadenceTir = 1.0f;          // Temps de lancer
            m.vitesseProj = 20.0f;       // Très lent par rapport à une balle !
            m.degats = 0;            // Dégâts de zone (théoriques) gerer directement dans projectiles.c
            m.nom = "Grenade MK2";
            // --- AJOUTS ---
            m.tailleProjectile = 0.1f;    // 
            m.couleurProjectile = GREEN;   // Vert militaire
            break;
            
        default: // Sécurité
            m = ObtenirModeleArme(PISTOLET);
            break;
    }
    return m;
}