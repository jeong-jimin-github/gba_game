#include "lib/gba.h"
#include "music.h"

// Owen was her

// リードメロディ
static u16 OwenLeadFreq[] = {
    23,30,25,30,
    26,28,30,28,33,
    35,30,37,38,
    37,38,37,35,33,
    30,33,28,30,26,
    23,30,25,30,
    26,28,30,28,33,
    35,30,37,38,
    37,38,37,35,33,
    35
};

static int OwenLeadLen[] = {
    4,4,4,4,
    4,2,2,4,4,
    2,2,2,2,
    2,1,1,2,2,
    2,2,2,2,8,
    4,4,4,4,
    4,2,2,4,4,
    2,2,2,2,
    2,1,1,2,2,
    16
};

// ベース
static u16 OwenChordsFreq[] = {
    7,9,11,8,
    7,9,11,
    7,9,11,8,
    7,9,11
};

static int OwenChordsLen[] = {
    8,8,8,8,
    8,8,16,
    8,8,8,8,
    8,8,16
};

// キック
static u16 OwenKickFreq[] = {
    130,130,130,130,
    130,130,130,130,
    130,130,130,130,
    130,130,130,130
};

static int OwenKickLen[] = {
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8
};

// スネア
static u16 OwenSnareFreq[] = {
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0
};

static int OwenSnareLen[] = {
    4,4,4,4,
    4,4,4,4,
    4,4,4,4,
    4,4,4,4,
    4,4,4,4,
    4,4,4,4,
    4,4,4,4,
    4,4,4,4
};

// Unreal Super Hero 3

// アルペジオ
static u16 UnrealSH3ArpFreq[] = {
    17,20,24,29,32,29,24,20,
    17,20,24,29,32,29,24,20,
    17,24,27,32,36,32,27,24,
    20,24,27,32,36,32,27,24,
    22,26,29,34,38,34,29,26,
    22,26,29,34,38,34,29,26,
    17,24,29,36,41,36,29,24,
    17,24,29,36,41,36,29,24
};

static int UnrealSH3ArpLen[] = {
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2
};

// リードメロディ
static u16 UnrealSH3LeadFreq[] = {
    0,
    29,29,36,
    29,36,41,39,39,36,
    29,39,41,38,38,34,
    38,36,34,36
};

static int UnrealSH3LeadLen[] = {
    256,
    4,4,12,
    4,4,4,4,4,12,
    4,4,4,4,4,12,
    6,2,4,32
};

// ベース
static u16 UnrealSH3BassFreq[] = {
    0,
    5,17,8,20,10,10,20,19,15,15,17,
    5,17,5,17,5,17,5,17,
    8,20,8,20,8,20,8,20,
    10,22,10,22,10,22,10,22,
    5,17,5,17,5,17,5,17
};

static int UnrealSH3BassLen[] = {
    128,
    24,8,24,8,24,8,6,6,8,4,8,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4,
    4,4,4,4,4,4,4,4
};

// スネア
static u16 UnrealSH3SnareFreq[] = {
    0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0,
    1,0,0,0
};

static int UnrealSH3SnareLen[] = {
    256,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8,
    8,8,8,8
};

SONG UnrealSH3Arp = { UnrealSH3ArpFreq, UnrealSH3ArpLen, 64 };
SONG UnrealSH3Lead = { UnrealSH3LeadFreq, UnrealSH3LeadLen, 20 };
SONG UnrealSH3Bass = { UnrealSH3BassFreq, UnrealSH3BassLen, 44 };
SONG UnrealSH3Snare = { UnrealSH3SnareFreq, UnrealSH3SnareLen, 33 };

SONG OwenLead = {OwenLeadFreq, OwenLeadLen, 42};
SONG OwenChord = {OwenChordsFreq, OwenChordsLen, 14};
SONG OwenKick = {OwenKickFreq, OwenKickLen, 16 };
SONG OwenSnare = {OwenSnareFreq, OwenSnareLen, 32 };

MUSIC_PLAYER Owen = {&OwenKick, &OwenChord, &OwenLead, &OwenSnare};
MUSIC_PLAYER UnrealSuperHero3 = {&UnrealSH3Bass, &UnrealSH3Arp, &UnrealSH3Lead, &UnrealSH3Snare};