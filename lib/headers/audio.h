#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

typedef struct {
    Music menuMusic;
    Music gameMusic;
    Sound pistolet;
    Sound mitraillette;
    Sound pompe;
    Sound walk;
    Sound heart;
} GameAudio;

// --- Cycle de vie ---
void InitGameAudio(void);
void UnloadGameAudio(void);
void UpdateGameAudio(void);

// --- Musiques ---
void PlayMenuMusic(void);
void PlayGameMusic(void);
void StopAllMusic(void);

// --- Sons ---
void PlayPistolet(void);
void PlayMitraillette(void);
void PlayPompe(void);
void PlayWalk(void);
void PlayHeart(void);

#endif