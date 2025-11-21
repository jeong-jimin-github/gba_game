#include "lib/gba.h"
#include "scene.h"
#include "m3func.h"
#include "sound.h"
#include "res.h"
#include "music.h"

extern int currentScene;

static ST_FONT f;
static u16 prev_key_input_credit = 0;

void Credit_Init(int scene) {
    if(scene != SCENE_CREDIT) return;

    f.pDat  = (u8*)&k6x10Bitmap;
    f.imgCx = 960;
    f.cx    = 6;
    f.cy    = 10;

    for(int i=0; i<240; i++){
        for(int j=0; j<160; j++){
            Mode3PutPixel(i, j, RGB5(0,0,0));
        }
    }

    Mode3DrawString(&f, 10, 50, "Made by Jeong Jimin", RGB5(31,31,31));
    InitMusic();
}

void Credit_Update() {
    if(currentScene != SCENE_CREDIT) return;

    PlayMusic(&un_owen_was_her_1, &un_owen_was_her_2, &un_owen_was_her_4);

    u16 current_key_input = REG_KEYINPUT;
    u16 pressed_keys = ~current_key_input & prev_key_input_credit; 

    if(pressed_keys & KEY_B) {
        StopMusic();
        ChangeScene(SCENE_MENU);
    }

    prev_key_input_credit = current_key_input;
}

void Credit_Draw() {
    if(currentScene != SCENE_CREDIT) return;
}
