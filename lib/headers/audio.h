#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

/**
 * \brief Gestion de l'audio du jeu (musiques et effets sonores)
 * \author Thomas Dequirez
 * \version 1.0
 * \date 2026
 */

/**
 * \struct GameAudio
 * \brief Contient toutes les ressources audio du jeu
 */
typedef struct {
    Music menuMusic;    /**< Musique du menu principal */
    Music gameMusic;    /**< Musique en jeu */
    Sound pistolet;     /**< Son du pistolet */
    Sound mitraillette; /**< Son de la mitraillette */
    Sound pompe;        /**< Son du fusil à pompe */
    Sound walk;         /**< Son des pas du joueur */
    Sound heart;        /**< Son du battement de cœur (vie faible) */
    Sound reload;       /**< Son du rechargement */
    Sound explosion;       /**< Son de l'explosion de la grenade */

} GameAudio;

/* ----------------------------- Cycle de vie ----------------------------- */

/** \brief Initialise le système audio et charge toutes les ressources */
void InitGameAudio(void);

/** \brief Libère toutes les ressources audio chargées */
void UnloadGameAudio(void);

/** \brief Met à jour le flux des musiques (à appeler chaque frame) */
void UpdateGameAudio(void);

/* -------------------------------- Musiques ------------------------------ */

/** \brief Lance la musique du menu principal */
void PlayMenuMusic(void);

/** \brief Lance la musique en jeu */
void PlayGameMusic(void);

/** \brief Arrête toutes les musiques en cours */
void StopAllMusic(void);

/* ----------------------------- Sons (Play) ------------------------------ */

/** \brief Joue le son du pistolet */
void PlayPistolet(void);

/** \brief Joue le son de la mitraillette */
void PlayMitraillette(void);

/** \brief Joue le son du fusil à pompe */
void PlayPompe(void);

/** \brief Joue le son des pas */
void PlayWalk(void);

/** \brief Joue le son du battement de cœur */
void PlayHeart(void);

/** \brief Joue le son du rechargement */
void PlayReload(void);

/* ----------------------------- Sons (Pause) ----------------------------- */

/** \brief Met en pause le son du pistolet */
void PausePistolet(void);

/** \brief Met en pause le son de la mitraillette */
void PauseMitraillette(void);

/** \brief Met en pause le son du fusil à pompe */
void PausePompe(void);

/** \brief Met en pause le son des pas */
void PauseWalk(void);

/** \brief Met en pause le son du battement de cœur */
void PauseHeart(void);

#endif