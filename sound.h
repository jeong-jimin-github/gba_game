#ifndef _SOUND_H_
#define _SOUND_H_
#include "lib/gba.h"

typedef struct {
    u16* freq;
    int* length;
    int size;
} SONG;

void InitMusic();
void PlayMusic(SONG* s1, SONG* s2);
void PlayFX(int pitch);
void StopMusic();

#endif // _SOUND_H_
