#include "arme.h"

ModeleArme ObtenirModeleArme(TypeArme type) {
    ModeleArme m;
    switch (type) {
        case PISTOLET:
            m.type = PISTOLET;
            m.munitionsMax = 12;
            m.cadenceTir = 0.25f;
            m.vitesseProj = 50.0f;
            m.degats = 20.0f;
            m.nom = "Pistolet";
            m.tailleProjectile = 0.2f;    // Taille standard
            m.couleurProjectile = YELLOW; // Jaune classique
            break;

        case FUSIL:
            m.type = FUSIL;
            m.munitionsMax = 30;
            m.cadenceTir = 0.1f;
            m.vitesseProj = 70.0f;
            m.degats = 15.0f;
            m.nom = "Fusil d'assaut";
            m.tailleProjectile = 0.15f;   // Un peu plus fin
            m.couleurProjectile = ORANGE;  // Orange pour les traçantes
            break;

        case SNIPER:
            m.type = SNIPER;
            m.munitionsMax = 5;
            m.cadenceTir = 1.5f;
            m.vitesseProj = 150.0f;      // Très rapide !
            m.degats = 100.0f;
            m.nom = "Fusil de precision";
            // --- AJOUTS ---
            m.tailleProjectile = 0.4f;    // Grosse balle imposante
            m.couleurProjectile = RED;     // Rouge "danger"
            break;
            
        case GRENADE:
            m.type = GRENADE;
            m.munitionsMax = 3;           // On en a peu
            m.cadenceTir = 1.0f;          // Temps de lancer
            m.vitesseProj = 20.0f;       // Très lent par rapport à une balle !
            m.degats = 500.0f;            // Dégâts de zone (théoriques)
            m.nom = "Grenade MK2";
            // --- AJOUTS ---
            m.tailleProjectile = 0.1f;    // Bien plus gros qu'une balle
            m.couleurProjectile = GREEN;   // Vert militaire
            break;
            
        default: // Sécurité
            m = ObtenirModeleArme(PISTOLET);
            break;
    }
    return m;
}