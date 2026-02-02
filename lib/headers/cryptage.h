#ifndef CRYPTAGE_H
#define CRYPTAGE_H

#include <string.h>
#include <stdint.h>

// convertie du texte en un texte illisible pour un etre humain
// mais pouvant etre reconverti en texte lisible grace a la meme fonction du a la symetrie de l'algorithme RC4
void rc4_crypt(unsigned char *data, size_t data_len, const char *key, size_t key_len);

// calcule un checksum pour verifier l'integrite des donnees
uint32_t calculate_checksum(const unsigned char *data, size_t len);

#endif

