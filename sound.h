#ifndef _SOUND_H_
#define _SOUND_H_
#include "lib/gba.h"

typedef struct {
    u16* freq;
    int* length;
    int size;
} SONG;

typedef struct {
    SONG* song_ch1;
    SONG* song_ch2;
    SONG* song_ch3;
    SONG* song_ch4;
} MUSIC_PLAYER;

void InitMusic();
void PlayMusic(MUSIC_PLAYER* music_player);
void PlayFX(int pitch);
void StopMusic();

#endif // _SOUND_H_
