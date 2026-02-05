#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../lib/headers/cryptage.h"

// -- PARTIE 1 : L'Algorithme de cryptage RC4 --

// Echange les valeurs des deux pointeurs
static void swap(unsigned char *a, unsigned char *b) {
    unsigned char temp = *a; //unsigned char permet de travailler sur 255 valeurs (0-255) au lieu de 127 (-128 à 127) sur un char classique
    *a = *b;
    *b = temp;
}

// Fonction principale RC4 (Chiffre ET Déchiffre car c'est symétrique)
// data: le buffer de données de sauvegarde
// data_len: la taille des données
// key: le mdp
// key_len: longueur du mdp
void rc4_crypt(unsigned char *data, size_t data_len, const char *key, size_t key_len) {
    unsigned char S[256];
    int i, j = 0;
    size_t k; 

    // 1. Initialisation du tableau d'état (KSA)
    for (i = 0; i < 256; i++) {
        S[i] = (unsigned char)i;
    }

    for (i = 0; i < 256; i++) {
        j = (j + S[i] + key[i % key_len]) % 256;
        swap(&S[i], &S[j]);
    }

    // 2. Génération du flux et cryptage (PRGA)
    i = 0;
    j = 0;
    for (k = 0; k < data_len; k++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        swap(&S[i], &S[j]);
        
        // XOR entre la donnée et le flux généré
        unsigned char keystream_byte = S[(S[i] + S[j]) % 256];
        data[k] = data[k] ^ keystream_byte;
    }
}

// --- PARTIE 2 : Checksum (Anti-Triche) ---
// Algorithme FNV-1a
uint32_t calculate_checksum(const unsigned char *data, size_t len) {
    uint32_t hash = 2166136261u; // Offset basis
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619u; // Prime
    }
    return hash;
}
