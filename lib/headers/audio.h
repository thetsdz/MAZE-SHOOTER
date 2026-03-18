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
    Sound reload;
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
void PlayReload(void);

void PausePistolet(void);
void PauseMitraillette(void);
void PausePompe(void);
void PauseWalk(void);
void PauseHeart(void);



#endif