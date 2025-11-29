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

const u32 wave[8*13] = { 
	// 0 ?
	0x02468ace,0xfdb97531,0x02468ace,0xfdb97531,
	0x2064a8ec,0xdf9b5713,0x2064a8ec,0xdf9b5713,
	// 1 double saw
	0x01234567,0x89abcdef,0x01234567,0x89abcdef,
	0x10325476,0x98badcfe,0x10325476,0x98badcfe,
	// 2 ?
	0x88808fff,0x88808880,0x00080888,0x80008000,
	0x8808f8ff,0x88088808,0x00808088,0x08000800,
	// 3 ?
	0x34455667,0x899aabbc,0x34455667,0x899aabbc,
	0x43546576,0x98a9bacd,0x43546576,0x98a9bacb,
	// 4 ?
	0x23345667,0x899abcde,0x23345667,0x899abcde,
	0x32436576,0x98a9cded,0x32436576,0x98a9cded,
	// 5 ?
	0xacddda48,0x3602cf16,0x2c04e52c,0xacddda48,
	0xcaddad84,0x6320fc61,0xc2405ec2,0xcaddad84,
	// 6 triangular
	0x10325476,0x67452301,0xefcdab89,0x98badcfe,
	0x01234567,0x76543210,0xfedcba98,0x89abcdef,
	// 7 ?
	0x01234567,0x89abcdef,0xffffffff,0xfedcba98,
	0x10325376,0x98badcfe,0xffffffff,0xefcdab89,
	// 8 ?
	0xf0eeca86,0x0a468ace,0xffeeca86,0x0a468ace,
	0x0feeac68,0xa064a8ec,0xffeeac68,0xa064a8ec,
	// 9 ?
	0x7ec9cea7,0xcfe8ab72,0x8d757203,0x85136317,
	0xe79cec7a,0xfc8eba27,0xd8572730,0x58313671,
	// 10 ?
	0xfedcba98,0x76543210,0x76543210,0x44002200,
	0xefcdab89,0x67452301,0x67452301,0x44002200,
	// 11 ?
	0x02468ace,0xffffffff,0xeca86420,0x00448844,
	0x2064a8ec,0xffffffff,0xce8a4602,0x00448844,
	// 12 ?
	0xFFFFFFFF,0xFFFFFFFF,0x00000000,0x00000000,
	0x00000000,0x00000000,0xFFFFFFFF,0xFFFFFFFF,
};

const u8 score[32*3] = {
	// 0
	19,8,15,8,19,15,8,15,19,15,8,15,19,8,15,19,5,12,17,12,5,17,12,17,5,17,12,17,5,12,17,5,
	// 1
	19,19,24,19,24,24,19,27,19,24,19,26,24,19,24,24,17,17,24,17,24,24,17,12,24,12,19,24,12,19,24,24,
	// 2
	31,31,36,31,36,36,31,39,31,36,31,38,36,31,36,36,29,29,36,29,36,36,29,24,36,24,31,36,24,31,36,36,
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
    u16 wave;
    u16 bank_number;
    u16 dimension;
    u16 sound_length;
    u16 sound_pattern;
    u16 pan;
    u16 tone;
    u16 bank0_wave;
    u16 bank1_wave;
} PARAM;

static PARAM param_data1 = {
    .sweep_shift = 0x0,
    .sweep_direction = 0x0,
    .sweep_time = 0x0,
    .length = 0x0,
    .duty_cycle = 0x0,
    .envelope_step_time = 0x0,
    .envelope_direction = 0x0,
    .initial_volume = 0x08,
    .frequency = 0x0,
    .length_enable = 0x0,
    .resampling_frequency = 0x0,
};
static PARAM param_data2 = {
    .length = 0x0,
    .duty_cycle = 0x0,
    .envelope_step_time = 0x7,
    .envelope_direction = 0x0,
    .initial_volume = 0x09,
    .frequency = 0x0,
    .length_enable = 0x0,
    .resampling_frequency = 0x3,
};
static PARAM param_data3 = {
    .wave = 0x1,
    .bank_number = 0x0,
    .dimension = 0x0,
    .sound_length = 0x03,
    .sound_pattern = 0x0,
    .pan = 0x0,
    .tone = 0x0,
    .bank0_wave = 0x0,
    .bank1_wave = 0x0,
};

static PARAM param_data4 = {
    .length = 0x0,
    .envelope_step_time = 0x7,
    .envelope_direction = 0x0,
    .initial_volume = 0x05,
    .frequency = 0x0,
    .counter_step = 0x1,
    .shift_freqency = 0x3,
    .length_enable = 0x1,
    .resampling_frequency = 0x3,
};

static SONG* song_ch1 = NULL;
static SONG* song_ch2 = NULL;
static SONG* song_ch3 = NULL;
static SONG* song_ch4 = NULL;

static int idx_ch1 = 0;
static int idx_ch2 = 0;
static int idx_ch3 = 0;
static int idx_ch4 = 0;

static int timer_ch1 = 0;
static int timer_ch2 = 0;
static int timer_ch3 = 0;
static int timer_ch4 = 0;

static int song_idx1 = 0;
static int song_idx2 = 0;
static int song_idx3 = 0;
static int song_idx4 = 0;
static int song_timer1 = 0;
static int song_timer2 = 0;
static int song_timer3 = 0;
static int song_timer4 = 0;

u32 PanMask = 0x4000;

int speed = 3;
int plus_speed = 5;

void LoadWave(PARAM* p, u32 select)
{
	u32 inst = p->wave * 8 + select * 4;

	DMA_Copy(3, (u32)&wave[inst], (u32)WAVE_RAM, DMA_SRC_INC | DMA_DST_INC | DMA32 | 4);

	// bank number
	if(p->bank_number == 0)
	{
		p->bank0_wave = inst;
	}
	else
	{
		p->bank1_wave = inst;
	}

	p->bank_number = (p->bank_number == 0) ? 1 : 0;
	REG_SOUND3CNT_L^= SOUND3_SETBANK(1);
}

void InitMusic()
{
    REG_SOUNDCNT_X = 0x80;
    REG_SOUNDCNT_L = 0xFF77;
    REG_SOUNDCNT_H = 2;
    REG_SOUND3CNT_L = SOUND3_PLAY | SOUND3_STEP32 | SOUND3_SETBANK(1);
    REG_SOUND3CNT_H = TRILENVOL_100;
    REG_SOUND3CNT_X = TRIFREQ_RESET;
    LoadWave(&param_data3, 0);

    song_idx1 = 0;
    song_idx2 = 0;
    song_idx3 = 0;
    song_idx4 = 0;
    song_timer1 = 0;
    song_timer2 = 0;
    song_timer3 = 0;
    song_timer4 = 0;
    timer_ch1 = 0;
    timer_ch2 = 0;
    timer_ch3 = 0;
    timer_ch4 = 0;
    song_ch1 = NULL;
    song_ch2 = NULL;
    song_ch3 = NULL;
    song_ch4 = NULL;
}

void PlayMusic(MUSIC_PLAYER* music_player)
{
    SONG* s1 = music_player->song_ch1;
    SONG* s2 = music_player->song_ch2;
    SONG* s3 = music_player->song_ch3;
    SONG* s4 = music_player->song_ch4;
    if (song_ch1 == NULL && s1 != NULL) {
        song_ch1 = s1;
        idx_ch1 = 0;
    }

    if (song_ch2 == NULL && s2 != NULL) {
        song_ch2 = s2;
        idx_ch2 = 0;
    }

    if (song_ch3 == NULL && s3 != NULL) {
        song_ch3 = s3;
        idx_ch3 = 0;
    }

    if (song_ch4 == NULL && s4 != NULL) {
        song_ch4 = s4;
        idx_ch4 = 0;
    }

    if (song_ch1 && idx_ch1 < song_ch1->size && timer_ch1 <= 0) {
        if (song_ch1->freq[idx_ch1] == 0) {
            // rest
            REG_SOUND1CNT_L = 0;
            REG_SOUND1CNT_H = 0;
            REG_SOUND1CNT_X = 0;
            timer_ch1 = song_ch1->length[idx_ch1] * speed;
            idx_ch1++;
            return;
        }

        if(song_ch1->freq[idx_ch1] == 130) {
            param_data1.sweep_shift = 0x7;
            param_data1.sweep_direction = 0x1;
            param_data1.sweep_time = 0x7;
            param_data1.frequency = 8;
        } else {
            param_data1.sweep_shift = 0x0;
            param_data1.sweep_direction = 0x0;
            param_data1.sweep_time = 0x0;
        }
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

        timer_ch1 = song_ch1->length[idx_ch1] * speed;
        idx_ch1++;
    }

    if (song_ch2 && idx_ch2 < song_ch2->size && timer_ch2 <= 0) {
        if (song_ch2->freq[idx_ch2] == 0) {
            // rest
            REG_SOUND2CNT_L = 0;
            REG_SOUND2CNT_H = 0;
            timer_ch2 = song_ch2->length[idx_ch2] * speed;
            idx_ch2++;
            return;
        }
        
        param_data2.frequency = song_ch2->freq[idx_ch2];

        u16 B2, L2, H2;
        B2 = (param_data2.resampling_frequency<<14) + (REG_SOUNDBIAS & 0x3fff);

        L2 =  (param_data2.initial_volume<<12)
            + (param_data2.envelope_direction<<11)
            + (param_data2.envelope_step_time<<8)
            + (param_data2.duty_cycle<<6)
            + (param_data2.length);

        H2 = freq[param_data2.frequency];

        REG_SOUNDBIAS = B2;
        REG_SOUND2CNT_L = L2;
        REG_SOUND2CNT_H = H2 + TRIFREQ_RESET;

        timer_ch2 = song_ch2->length[idx_ch2] * speed;
        idx_ch2++;
    }

    if (song_ch3 && idx_ch3 < song_ch3->size && timer_ch3 <= 0) {
        if(song_ch3->freq[idx_ch3] == 0) {
            // rest
            REG_SOUND3CNT_H = 0;
            REG_SOUND3CNT_X = 0;
            timer_ch3 = song_ch3->length[idx_ch3] * speed;
            idx_ch3++;
            return;
        }

        param_data3.frequency = song_ch3->freq[idx_ch3];
        REG_SOUND3CNT_H = TRILENVOL_100 | param_data3.sound_length;
		REG_SOUND3CNT_X = freq[param_data3.frequency] | TRIFREQ_TIMED | TRIFREQ_RESET;

        timer_ch3 = song_ch3->length[idx_ch3] * speed;
        idx_ch3++;
    }

    if (song_ch4 && idx_ch4 < song_ch4->size && timer_ch4 <= 0) {
        if(song_ch4->freq[idx_ch4] == 0) {
            // rest
            REG_SOUND4CNT_L = 0;
            REG_SOUND4CNT_H = 0;
            timer_ch4 = song_ch4->length[idx_ch4] * speed;
            idx_ch4++;
            return;
        }

        param_data4.frequency = 40;

        if(song_ch4->freq[idx_ch4] == 1) {
            param_data4.length = 0x04;
        }

        u16 B4, L4, H4;
        B4 = (param_data4.resampling_frequency<<14) + (REG_SOUNDBIAS & 0x3fff);

        L4 =  (param_data4.initial_volume<<12)
            + (param_data4.envelope_direction<<11)
            + (param_data4.envelope_step_time<< 8)
            + (param_data4.length);

        H4 = (param_data4.length_enable<<14)
           + (param_data4.shift_freqency<<4)
           + (param_data4.counter_step<<3)
           + (param_data4.frequency);

        REG_SOUNDBIAS = B4;
        REG_SOUND4CNT_L = L4;
        REG_SOUND4CNT_H = H4 + TRIFREQ_RESET;

        timer_ch4 = song_ch4->length[idx_ch4] * speed;
        idx_ch4++;
    }

    timer_ch1--;
    timer_ch2--;
    timer_ch3--;
    timer_ch4--;

    if (song_ch1 && idx_ch1 >= song_ch1->size)
        song_ch1 = NULL;
    if (song_ch2 && idx_ch2 >= song_ch2->size)
        song_ch2 = NULL;
    if (song_ch3 && idx_ch3 >= song_ch3->size)
        song_ch3 = NULL;
    if (song_ch4 && idx_ch4 >= song_ch4->size)
        song_ch4 = NULL;
}

void StopMusic()
{
    song_ch1 = NULL;
    song_ch2 = NULL;
    song_ch3 = NULL;
    song_ch4 = NULL;
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
