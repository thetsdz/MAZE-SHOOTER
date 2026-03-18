#ifndef LOG_H
#define LOG_H

/** \version 1.0
 * \author Corentin Jammes
 * \date 11/02/2026
 * \brief Permet d'écrire des logs dans un fichier pour faciliter le débuggage
 */

#include <stdarg.h>

/**
 * @brief Initialise le fichier log
 * @param filename Chemin du fichier log
 * @return 1 si succès, 0 si erreur
 */
int InitLog(const char* filename);

/**
 * @brief Ferme le fichier log
 */
void CloseLog(void);

/**
 * @brief Fonction callback pour écrire les logs dans le fichier
 * @param logLevel Niveau de log (LOG_INFO, LOG_ERROR, etc.)
 * @param text Format string
 * @param args Arguments variables
 */
void LogToFile(int logLevel, const char* text, va_list args);

#endif
