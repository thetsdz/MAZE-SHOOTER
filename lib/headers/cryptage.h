#ifndef CRYPTAGE_H
#define CRYPTAGE_H

#include <string.h>
#include <stdint.h>

/** @brief Convertie du texte en un texte illisible pour un etre humain*/
void rc4_crypt(unsigned char *data, size_t data_len, const char *key, size_t key_len);

/** @brief Calcule un checksum pour verifier l'integrite des donnees*/
uint32_t calculate_checksum(const unsigned char *data, size_t len);

#endif