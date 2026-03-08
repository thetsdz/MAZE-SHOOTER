#include "arme.h"

ModeleArme ObtenirModeleArme(TypeArme type) {
    ModeleArme m;
    switch (type) {
        case PISTOLET:
            m.type = PISTOLET;
            m.munitionsMax = 12;      // Par exemple
            m.cadenceTir = 0.25f;     // Cadence semi-auto confortable
            m.vitesseBalle = 50.0f;   // Ta valeur actuelle !
            m.degats = 20.0f;
            m.nom = "Pistolet de base";
            break;
        case FUSIL:
            m.type = FUSIL;
            m.munitionsMax = 30;
            m.cadenceTir = 0.1f;      // Tir rapide
            m.vitesseBalle = 70.0f;   // Plus rapide
            m.degats = 15.0f;
            m.nom = "Fusil d'assaut";
            break;
        case SNIPER:
            m.type = SNIPER;
            m.munitionsMax = 5;
            m.cadenceTir = 1.5f;      // Très lent
            m.vitesseBalle = 120.0f;  // Très rapide
            m.degats = 100.0f;
            m.nom = "Fusil de précision";
            break;
    }
    return m;
}