#include "lib/gba.h"
#include "scene.h"
#include "commonfunc.h"
#include "res.h"

extern s32 currentScene;
static ST_FONT f;
static u16 prev_key_input_setting = 0;

void Setting_Init(int scene) {
    f.pDat  = (u8*)&k6x10Bitmap;
    f.imgCx = 960;
    f.cx    = 6;
    f.cy    = 10;

    if(scene != SCENE_SETTING) return;

    for(int i=0; i<240; i++){
        for(int j=0; j<160; j++){
            Mode3PutPixel(i, j, RGB5(0,0,0));
        }
    }
    Mode3DrawString(&f, 60, 20, "Audio TEST", RGB5(31,31,31));
}

void Setting_Update() {
    if(currentScene != SCENE_SETTING) return;

    u16 current_key_input = REG_KEYINPUT;
    u16 pressed_keys = ~current_key_input & prev_key_input_setting; 

    if(pressed_keys & KEY_B) {
        ChangeScene(SCENE_MENU);
    }

    prev_key_input_setting = current_key_input;
}

void Setting_Draw() {
    if(currentScene != SCENE_SETTING) return;
}
