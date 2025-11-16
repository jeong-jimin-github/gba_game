#include "lib/gba.h"
#include "music.h"

static u16 song_freq[] = {
22,
29,
24,
29,
25,
27,
29,
27,
32,
34,
29,
36,
37,
36,
37,
36,
34,
32,
29,
32,
27,
29,
25,
22,
29,
24,
29,
25,
27,
29,
27,
32,
34,
29,
36,
37,
36,
37,
36,
34,
32,
34
};

static int song_length[] = {
    4,4,4,4,4,2,2,4,4,2,2,2,2,2,1,1,2,2,2,2,2,2,8,4,4,4,4,4,2,2,4,4,2,2,2,2,2,1,1,2,2,16
};

static u16 song_freq2[] = {    
    6, 8, 10, 7, 6, 8, 10, 6, 8, 10, 7, 6, 8, 10
};

static int song_length2[] = {
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 16
};

static u16 unreal_superhero_3_freq[] = {17, 20, 24, 29, 32, 29, 24, 20, 17, 20, 24, 29, 32, 29, 24, 20, 17, 24, 27, 32, 36, 32, 27, 24, 20, 24, 27, 32, 36, 32, 27, 24, 22, 26, 29, 34, 38, 34, 29, 26, 22, 26, 29, 34, 38, 34, 29, 26, 17, 24, 29, 36, 41, 36, 29, 24, 17, 24, 29, 36, 41, 36, 29, 24};
static int unreal_superhero_3_length[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static u16 unreal_superhero_3_lead_freq[] = {29, 29, 36, 29, 36, 41, 39, 39, 36, 29, 39, 41, 38, 38, 34, 38, 36, 34, 36};
static int unreal_superhero_3_lead_length[] = {2,2,6,2,2,2,2,2,6,2,2,2,2,2,6,3,1,2,16};
SONG unreal_superhero_3_1 = { unreal_superhero_3_freq, unreal_superhero_3_length, 64 };
SONG unreal_superhero_3_2 = { unreal_superhero_3_lead_freq, unreal_superhero_3_lead_length, 19 };

SONG un_owen_was_her_1 = {song_freq, song_length, 42};
SONG un_owen_was_her_2 = {song_freq2, song_length2, 14};