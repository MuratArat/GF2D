#include "sounds.h"

void Music_Init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        slog("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

void Music_Quit() {
    Mix_Quit();
}

Music *Music_Load(const char *filename) {
    Music *music = (Music *)malloc(sizeof(Music));
    if (music == NULL) {
        slog("Unable to allocate memory for Music structure\n");
        return NULL;
    }

    music->music = Mix_LoadMUS(filename);
    if (music->music == NULL) {
        slog("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        free(music);
        return NULL;
    }

    return music;
}

void Music_Free(Music *music) {
    if (music == NULL) {
        return;
    }

    Mix_FreeMusic(music->music);
    free(music);
}

void Music_Play(Music *music, int loops) {
    if (music == NULL) {
        return;
    }

    Mix_PlayMusic(music->music, loops);
}

void Music_Pause() {
    Mix_PauseMusic();
}

void Music_Resume() {
    Mix_ResumeMusic();
}

void Music_Stop() {
    Mix_HaltMusic();
}
