#include "lib/gba.h"
#include "scene.h"
#include "commonfunc.h"
#include "res.h"

extern s32 currentScene;

static ST_FONT f_JP;
static ST_FONT f;
static u16 prev_key_input_manual = 0;

void Manual_Init(s32 scene) {
    if(scene != SCENE_MANUAL) return;

    f_JP.pDat  = (u8*)&mplus_j10rBitmap;
    f_JP.imgCx = 69630 + 2;
    f_JP.cx    = 10;
    f_JP.cy    = 11;
    f_JP.pSheet= (u16*)&mplus_jfnt_txt;
    f_JP.cnt   = 6963;
        
    f.pDat  = (u8*)&k6x10Bitmap;
    f.imgCx = 960;
    f.cx    = 6;
    f.cy    = 10;

    for(s32 i=0; i<240; i++){
        for(s32 j=0; j<160; j++){
            Mode3PutPixel(i, j, RGB5(0,0,0));
        }
    }
    
    Mode3DrawSJISStr(&f_JP, 10,10, "操作方法", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 30,25, "ボタン　ジャンプ", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 30,40, "ボタン　攻撃", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 70,55, "タイトルに戻る", RGB5(31,31,31));
    Mode3DrawString(&f, 20,25, "A", RGB5(31,31,31));
    Mode3DrawString(&f, 20,40, "B", RGB5(31,31,31));
    Mode3DrawString(&f, 20,55, "SELECT", RGB5(31,31,31));

}

void Manual_Update() {
    if(currentScene != SCENE_MANUAL) return;

    u16 current_key_input = REG_KEYINPUT;
    u16 pressed_keys = ~current_key_input & prev_key_input_manual; 

    if(pressed_keys & KEY_B) {
        ChangeScene(SCENE_MENU);
    }

    prev_key_input_manual = current_key_input;
}

void Manual_Draw() {
    if(currentScene != SCENE_MANUAL) return;
}