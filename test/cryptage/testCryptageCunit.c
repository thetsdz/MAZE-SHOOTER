#include <CUnit/Basic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../lib/headers/cryptage.h"

/* --- Structures de test --- */
typedef struct {
  int id;
  char data[1024];  // Test avec un gros buffer
  uint32_t magic_number;
} BigData;

/* --- Fonctions d'aide --- */
int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/* --- TESTS UNITAIRES --- */

/**
 * @brief Test de robustesse : Clé vide ou très courte
 */
void test_rc4_edge_cases_keys(void) {
  unsigned char data[] = "Données de test";
  unsigned char copy[20];
  memcpy(copy, data, sizeof(data));

  rc4_crypt(data, sizeof(data), "A", 1);
  CU_ASSERT_NSTRING_NOT_EQUAL(data, copy, sizeof(data));

  rc4_crypt(data, sizeof(data), "A", 1);
  CU_ASSERT_NSTRING_EQUAL(data, copy, sizeof(data));
}

/**
 * @brief Test avec des données nulles ou vides
 * Vérifie que la fonction ne plante pas si la taille est 0
 */
void test_rc4_zero_length(void) {
  unsigned char* data = NULL;

  rc4_crypt(data, 0, "Key", 3);
  CU_PASS("Gestion de la taille zéro réussie");
}

/**
 * @brief Test de collision et sensibilité du Checksum (FNV-1a)
 * Un seul bit de différence doit changer radicalement le hash
 */
void test_checksum_sensitivity(void) {
  const char* msg1 = "Pseudo: Joueur1, Or: 100";
  const char* msg2 = "Pseudo: Joueur1, Or: 101";  // 1 seul bit de différence

  uint32_t hash1 = calculate_checksum((unsigned char*)msg1, strlen(msg1));
  uint32_t hash2 = calculate_checksum((unsigned char*)msg2, strlen(msg2));

  CU_ASSERT_NOT_EQUAL(hash1, hash2);
}

/**
 * @brief Test de "Round-trip" sur une structure complexe
 */
void test_big_data_integrity(void) {
  BigData original = {42, "Ceci est un test de gros volume", 0xDEADBEEF};
  BigData buffer;
  memcpy(&buffer, &original, sizeof(BigData));
  const char* key = "PhraseSecreteTresLonguePourTesterLaStabilite";

  rc4_crypt((unsigned char*)&buffer, sizeof(BigData), key, strlen(key));

  CU_ASSERT_NOT_EQUAL(original.magic_number, buffer.magic_number);

  rc4_crypt((unsigned char*)&buffer, sizeof(BigData), key, strlen(key));

  CU_ASSERT_EQUAL(original.id, buffer.id);
  CU_ASSERT_EQUAL(original.magic_number, buffer.magic_number);

  CU_ASSERT_STRING_EQUAL(original.data, buffer.data);
}

/**
 * @brief Test de sécurité : Deux messages identiques avec deux clés différentes
 */
void test_rc4_different_output(void) {
  char plain[] = "Secret123";
  char cipher1[10], cipher2[10];

  memcpy(cipher1, plain, 10);
  memcpy(cipher2, plain, 10);

  rc4_crypt((unsigned char*)cipher1, 10, "KEY_A", 5);
  rc4_crypt((unsigned char*)cipher2, 10, "KEY_B", 5);

  CU_ASSERT_NSTRING_NOT_EQUAL(cipher1, cipher2, 10);
}

/**
 * @brief Test de sécurité : Déchiffrement avec une mauvaise clé.
 * Vérifie que l'utilisation d'une clé incorrecte produit des données corrompues
 * et que le checksum ne correspond plus.
 */
void test_rc4_invalid_key(void) {
  char original[] = "DonneesUltraSecretes123";
  char buffer[32];
  strcpy(buffer, original);
  size_t len = strlen(buffer);

  const char* REAL_KEY = "MaCleCorrecte";
  const char* WRONG_KEY = "MauvaiseCle";

  uint32_t original_checksum =
      calculate_checksum((unsigned char*)original, len);

  rc4_crypt((unsigned char*)buffer, len, REAL_KEY, strlen(REAL_KEY));
  rc4_crypt((unsigned char*)buffer, len, WRONG_KEY, strlen(WRONG_KEY));

  CU_ASSERT_NSTRING_NOT_EQUAL(buffer, original, len);

  uint32_t corrupted_checksum = calculate_checksum((unsigned char*)buffer, len);

  CU_ASSERT_NOT_EQUAL(corrupted_checksum, original_checksum);
}

/* --- MAIN --- */
int main() {
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  CU_pSuite pSuite =
      CU_add_suite("Suite_Tests_Avances_Cryptage", init_suite, clean_suite);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_add_test(pSuite, "Edge Case: Cles courtes", test_rc4_edge_cases_keys);
  CU_add_test(pSuite, "Securite: Taille Zero", test_rc4_zero_length);
  CU_add_test(pSuite, "Integrite: Sensibilite Checksum",
              test_checksum_sensitivity);
  CU_add_test(pSuite, "Integrite: Gros volumes et structures",
              test_big_data_integrity);
  CU_add_test(pSuite, "Securite: Differentes cles", test_rc4_different_output);
  CU_add_test(pSuite, "Securite: Echec Mauvaise Cle", test_rc4_invalid_key);

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}