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

/** @brief Convertie du texte en un texte illisible pour un etre humain
 * @param data Le texte à encrypter (modifié en place)
 * @param data_len La longueur du texte à encrypter
 * @param key La clé d'encryption (doit être la même pour encrypter et décrypter)
 * @param key_len La longueur de la clé d'encryption
*/
void rc4_crypt(unsigned char *data, size_t data_len, const char *key,
               size_t key_len);

/** @brief Calcule un checksum pour verifier l'integrite des donnees
 * @param data Les données pour lesquelles calculer le checksum
 * @param len La longueur des données
*/
uint32_t calculate_checksum(const unsigned char *data, size_t len);

#endif