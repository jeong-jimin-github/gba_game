#include "sound.h"

u16 freq[12*6] = {
	// C      C+       D      D+       E       F      F+       G      G+       A      A+       B
	  44,    156,    262,    363,    457,    547,    631,    710,    786,    854,    923,    986,   // o3
	1046,   1102,   1155,   1205,   1253,   1297,   1339,   1379,   1417,   1452,   1486,   1517,   // o4
	1546,   1575,   1602,   1627,   1650,   1673,   1694,   1714,   1732,   1750,   1767,   1783,   // o5
	1798,   1812,   1825,   1837,   1849,   1860,   1871,   1881,   1890,   1899,   1907,   1915,   // o6
	1923,   1930,   1936,   1943,   1949,   1954,   1959,   1964,   1969,   1974,   1978,   1982,   // o7
	1985,   1988,   1992,   1995,   1998,   2001,   2004,   2006,   2009,   2011,   2013,   2015,   // o8
};

typedef struct {
    u16 sweep_shift; // 0-7
    u16 sweep_direction;
    u16 sweep_time;
    u16 length; // 0-63 (0x00 - 0x3F)
    u16 duty_cycle; // 0-3 (12.5%, 25%, 50%, 75%)
    u16 envelope_step_time; // 0-7
    u16 envelope_direction; // 0: decrease, 1: increase
    u16 initial_volume; // 0-15
    u16 frequency; // 0-71
    u16 length_enable; // 0: disable, 1: enable
    u16 resampling_frequency; // 0-3
    u16 counter_step;
    u16 shift_freqency;
} PARAM;

static PARAM param_data1 = {
    .sweep_shift = 0x0,
    .sweep_direction = 0x0,
    .sweep_time = 0x0,
    .length = 0x0,
    .duty_cycle = 0x0,
    .envelope_step_time = 0x7,
    .envelope_direction = 0x0,
    .initial_volume = 0xF,
    .frequency = 0x0,
    .length_enable = 0x1,
    .resampling_frequency = 0x3,
};
static PARAM param_data2 = {
    .length = 0x0,
    .duty_cycle = 0x0,
    .envelope_step_time = 0x7,
    .envelope_direction = 0x0,
    .initial_volume = 0xF,
    .frequency = 0x0,
    .length_enable = 0x0,
    .resampling_frequency = 0x3,
};



static SONG* song_ch1 = NULL;
static SONG* song_ch2 = NULL;

static int idx_ch1 = 0;
static int idx_ch2 = 0;

static int timer_ch1 = 0;
static int timer_ch2 = 0;

#define SONG1_LEN 42
#define SONG2_LEN 14

static int song_idx1 = 0;
static int song_idx2 = 0;
static int song_timer1 = 0;
static int song_timer2 = 0;

void InitMusic()
{
    REG_SOUNDCNT_X = 0x80;
    REG_SOUNDCNT_L = 0xFF77;
    REG_SOUNDCNT_H = 2;

    song_idx1 = 0;
    song_idx2 = 0;
    song_timer1 = 0;
    song_timer2 = 0;
    timer_ch1 = 0;
    timer_ch2 = 0;
}

void PlayMusic(SONG* s1, SONG* s2)
{
    if (song_ch1 == NULL && s1 != NULL) {
        song_ch1 = s1;
        idx_ch1 = 0;
    }

    if (song_ch2 == NULL && s2 != NULL) {
        song_ch2 = s2;
        idx_ch2 = 0;
    }

    if (song_ch1 && idx_ch1 < song_ch1->size && timer_ch1 <= 0) {
        param_data1.frequency = song_ch1->freq[idx_ch1];

        u16 B, L, H, X;
        B = (param_data1.resampling_frequency<<14) + (REG_SOUNDBIAS & 0x3fff);
        L =  (param_data1.sweep_time<<4) + (param_data1.sweep_direction<<3) + (param_data1.sweep_shift);
        H = (param_data1.initial_volume<<12) + (param_data1.envelope_direction<<11)
          + (param_data1.envelope_direction<<8) + (param_data1.envelope_step_time<<6)
          + (param_data1.length);
        X = (param_data1.length_enable<<14) + freq[param_data1.frequency];

        REG_SOUNDBIAS = B;
        REG_SOUND1CNT_L = L;
        REG_SOUND1CNT_H = H;
        REG_SOUND1CNT_X = X + TRIFREQ_RESET;

        timer_ch1 = song_ch1->length[idx_ch1] * 5;
        idx_ch1++;
    }

    if (song_ch2 && idx_ch2 < song_ch2->size && timer_ch2 <= 0) {
        param_data2.frequency = song_ch2->freq[idx_ch2];

        u16 B2, L2, H2;
        B2 = (param_data2.resampling_frequency<<14) + (REG_SOUNDBIAS & 0x3fff);

        L2 =  (param_data2.initial_volume<<12)
            + (param_data2.envelope_direction<<11)
            + (param_data2.envelope_step_time<< 8)
            + (param_data2.duty_cycle<< 6)
            + (param_data2.length);

        H2 = freq[param_data2.frequency];

        REG_SOUNDBIAS = B2;
        REG_SOUND2CNT_L = L2;
        REG_SOUND2CNT_H = H2 + TRIFREQ_RESET;

        timer_ch2 = song_ch2->length[idx_ch2] * 5;
        idx_ch2++;
    }

    timer_ch1--;
    timer_ch2--;

    if (song_ch1 && idx_ch1 >= song_ch1->size)
        song_ch1 = NULL;
    if (song_ch2 && idx_ch2 >= song_ch2->size)
        song_ch2 = NULL;
}

void StopMusic()
{
    song_ch1 = NULL;
    song_ch2 = NULL;
    REG_SOUNDCNT_X = 0x00;
    REG_SOUND1CNT_L = 0;
    REG_SOUND1CNT_H = 0;
    REG_SOUND1CNT_X = 0;
    REG_SOUND2CNT_L = 0;
    REG_SOUND2CNT_H = 0;
    REG_SOUND3CNT_L = 0;
    REG_SOUND3CNT_H = 0;
    REG_SOUND3CNT_X = 0;
    REG_SOUND4CNT_L = 0;
    REG_SOUND4CNT_H = 0;
}

void PlayFX(int pitch)
{
    REG_SOUNDCNT_X = 0x80;
    REG_SOUNDCNT_L = 0xFF77;
    REG_SOUNDCNT_H = 2;
    REG_SOUNDBIAS = (0xC000) + (REG_SOUNDBIAS & 0x3fff);
    REG_SOUND2CNT_L = (0xF<<12) + (0x1<< 8);
    REG_SOUND2CNT_H = freq[pitch] + TRIFREQ_RESET;
}
