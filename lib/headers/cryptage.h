#ifndef CRYPTAGE_H
#define CRYPTAGE_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 05/02/2026
 * \brief Permet d'encrypter un fichier et de calculer un checksum pour verifier
 * l'integrite des donnees
 */

#include <stdint.h>
#include <string.h>

/** @brief Convertie du texte en un texte illisible pour un etre humain*/
void rc4_crypt(unsigned char *data, size_t data_len, const char *key,
               size_t key_len);

/** @brief Calcule un checksum pour verifier l'integrite des donnees*/
uint32_t calculate_checksum(const unsigned char *data, size_t len);

#endif