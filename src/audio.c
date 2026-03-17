#include "../lib/headers/audio.h"

static GameAudio g_audio = {0};

// --- Cycle de vie ---

void InitGameAudio(void) {
    g_audio.menuMusic   = LoadMusicStream("../assets/audio/menu.mp3");
    g_audio.gameMusic   = LoadMusicStream("../assets/audio/game.wav");
    g_audio.pistolet    = LoadSound("../assets/audio/pistolet.mp3");
    g_audio.mitraillette = LoadSound("../assets/audio/mitraillette.mp3");
    g_audio.pompe       = LoadSound("../assets/audio/pompe.wav");
    g_audio.walk        = LoadSound("../assets/audio/walk.wav");
    g_audio.heart       = LoadSound("../assets/audio/heart.wav");
}

void UnloadGameAudio(void) {
    UnloadMusicStream(g_audio.menuMusic);
    UnloadMusicStream(g_audio.gameMusic);
    UnloadSound(g_audio.pistolet);
    UnloadSound(g_audio.mitraillette);
    UnloadSound(g_audio.pompe);
    UnloadSound(g_audio.walk);
    UnloadSound(g_audio.heart);
}

void UpdateGameAudio(void) {
    if (IsMusicStreamPlaying(g_audio.menuMusic))
        UpdateMusicStream(g_audio.menuMusic);
    if (IsMusicStreamPlaying(g_audio.gameMusic))
        UpdateMusicStream(g_audio.gameMusic);
}

// --- Musiques ---

void PlayMenuMusic(void) {
    StopMusicStream(g_audio.gameMusic);
    if (!IsMusicStreamPlaying(g_audio.menuMusic))
        PlayMusicStream(g_audio.menuMusic);
}

void PlayGameMusic(void) {
    StopMusicStream(g_audio.menuMusic);
    if (!IsMusicStreamPlaying(g_audio.gameMusic))
        PlayMusicStream(g_audio.gameMusic);
}

void StopAllMusic(void) {
    StopMusicStream(g_audio.menuMusic);
    StopMusicStream(g_audio.gameMusic);
}

// --- Sons ---

void PlayPistolet(void) {
    if (!IsSoundPlaying(g_audio.pistolet)) {
        PlaySound(g_audio.pistolet);
    }
}

void PlayMitraillette(void) {
    if (!IsSoundPlaying(g_audio.mitraillette)) {
        PlaySound(g_audio.mitraillette);
    }
}

void PlayPompe(void) {
    if (!IsSoundPlaying(g_audio.pompe)) {
        PlaySound(g_audio.pompe);
    }
}

void PlayWalk(void) {
    if (!IsSoundPlaying(g_audio.walk)) {
        PlaySound(g_audio.walk);
    }
}

void PlayHeart(void) {
    if (!IsSoundPlaying(g_audio.heart)) {
        PlaySound(g_audio.heart);
    }
}




void PausePistolet(void)     { PauseSound(g_audio.pistolet);     }
void PauseMitraillette(void) { PauseSound(g_audio.mitraillette); }
void PausePompe(void)        { PauseSound(g_audio.pompe);        }
void PauseWalk(void)         { PauseSound(g_audio.walk);         }
void PauseHeart(void)        { PauseSound(g_audio.heart);        }