#include "lib/gba.h"
#include "music.h"

static u16 OwenLeadFreq[] = {22,29,24,29,25,27,29,27,32,34,29,36,37,36,37,36,34,32,29,32,27,29,25,22,29,24,29,25,27,29,27,32,34,29,36,37,36,37,36,34,32,34};
static int OwenLeadLen[] = {8,8,8,8,8,4,4,8,8,4,4,4,4,4,2,2,4,4,4,4,4,4,16,8,8,8,8,8,4,4,8,8,4,4,4,4,4,2,2,4,4,32};
static u16 OwenChordsFreq[] = {6,8,10,7,6,8,10,6,8,10,7,6,8,10};
static int OwenChordsLen[] = {16,16,16,16,16,16,32,16,16,16,16,16,16,32};
static u16 OwenKickFreq[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};
static int OwenKickLen[] = {16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16};
static u16 OwenSnareFreq[] = {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0};
static int OwenSnareLen[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

static u16 UnrealSH3ArpFreq[] = {17,20,24,29,32,29,24,20,17,20,24,29,32,29,24,20,17,24,27,32,36,32,27,24,20,24,27,32,36,32,27,24,22,26,29,34,38,34,29,26,22,26,29,34,38,34,29,26,17,24,29,36,41,36,29,24,17,24,29,36,41,36,29,24};
static int UnrealSH3ArpLen[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
static u16 UnrealSH3LeadFreq[] = {29,29,36,29,36,41,39,39,36,29,39,41,38,38,34,38,36,34,36};
static int UnrealSH3LeadLen[] = {4,4,12,4,4,4,4,4,12,4,4,4,4,4,12,6,2,4,32};
static u16 UnrealSH3KickFreq[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};
static int UnrealSH3KickLen[] = {16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16};
static u16 UnrealSH3SnareFreq[] = {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0};
static int UnrealSH3SnareLen[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

SONG UnrealSH3Arp = { UnrealSH3ArpFreq, UnrealSH3ArpLen, 64 };
SONG UnrealSH3Lead = { UnrealSH3LeadFreq, UnrealSH3LeadLen, 19 };
SONG UnrealSH3Kick = { UnrealSH3KickFreq, UnrealSH3KickLen, 16 };
SONG UnrealSH3Snare = { UnrealSH3SnareFreq, UnrealSH3SnareLen, 32 };

SONG OwenLead = {OwenLeadFreq, OwenLeadLen, 42};
SONG OwenChord = {OwenChordsFreq, OwenChordsLen, 14};
SONG OwenKick = {OwenKickFreq, OwenKickLen, 16 };
SONG OwenSnare = {OwenSnareFreq, OwenSnareLen, 32 };

MUSIC_PLAYER Owen = {&OwenKick, &OwenChord, &OwenLead, &OwenSnare};
MUSIC_PLAYER UnrealSuperHero3 = {&UnrealSH3Kick, &UnrealSH3Arp, &UnrealSH3Lead, &UnrealSH3Snare};