#include "lib/gba.h"
#include "scene.h"
#include "commonfunc.h"
#include "res.h"
#include <maxmod.h>
#include "soundbank.h"

extern s32 currentScene;

static ST_FONT f;
static u16 prev_key_input_credit = 0;

void Credit_Init(s32 scene) {
    if(scene != SCENE_CREDIT) return;

    f.pDat  = (u8*)&k6x10Bitmap;
    f.imgCx = 960;
    f.cx    = 6;
    f.cy    = 10;

    for(s32 i=0; i<240; i++){
        for(s32 j=0; j<160; j++){
            Mode3PutPixel(i, j, RGB5(0,0,0));
        }
    }
    mmStart( MOD_UNREEEAL_SUPERHERO_3, MM_PLAY_LOOP );
    Mode3DrawString(&f, 60, 50, "Made by Jeong Jimin", RGB5(31,31,31));
    Mode3DrawString(&f, 5, 80, "github.com/jeong-jimin-github/gba_game", RGB5(31,31,31));
}

void Credit_Update() {
    if(currentScene != SCENE_CREDIT) return;

    u16 current_key_input = REG_KEYINPUT;
    u16 pressed_keys = ~current_key_input & prev_key_input_credit; 

    if(pressed_keys & KEY_B) {
        mmStop();
        ChangeScene(SCENE_MENU);
    }

    prev_key_input_credit = current_key_input;
}

void Credit_Draw() {
    if(currentScene != SCENE_CREDIT) return;
}
