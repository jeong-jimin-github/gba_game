#include "lib/gba.h"
#include "scene.h"
#include "commonfunc.h"
#include "res.h"

extern s32 currentScene;

static ST_FONT f_JP;
static int cursor = 0;
static u16 prev_key_input = 0;

void Menu_Init(int scene) {
    if(scene != SCENE_MENU) return;
    
    f_JP.pDat  = (u8*)&mplus_j10rBitmap;
    f_JP.imgCx = 69630 + 2;
    f_JP.cx    = 10;
    f_JP.cy    = 11;
    f_JP.pSheet= (u16*)&mplus_jfnt_txt;
    f_JP.cnt   = 6963;

    Mode3DrawImage(0,0, menubgBitmap);
}

void Menu_Update() {
    if(currentScene != SCENE_MENU) return;

    u16 current_key_input = REG_KEYINPUT;
    u16 pressed_keys = ~current_key_input & prev_key_input;

    if(pressed_keys & KEY_UP) {
        if(cursor > 0) {
            cursor--;
        }
        else {
        }
    }
    if(pressed_keys & KEY_DOWN) {
        if(cursor < 3){
            cursor++;
        }
        else {
        }
    }

    if(pressed_keys & KEY_A) {
        if(cursor == 0) {
            currentScene = SCENE_GAME;
            ChangeScene(currentScene);
        } else if(cursor == 1) {
            currentScene = SCENE_SETTING;
            ChangeScene(currentScene);
        } else if(cursor == 2) {
            currentScene = SCENE_CREDIT;
            ChangeScene(currentScene);
        } else if(cursor == 3) {
            currentScene = SCENE_MANUAL;
            ChangeScene(currentScene);
        }
    }

    prev_key_input = current_key_input;
}

void Menu_Draw() {
    if(currentScene != SCENE_MENU) return;

    u16 col_s = (cursor==0)? RGB5(31, 0, 0) : RGB5(0, 0, 0);
    u16 col_t = (cursor==1)? RGB5(31, 0, 0) : RGB5(0, 0, 0);
    u16 col_c = (cursor==2)? RGB5(31, 0, 0) : RGB5(0, 0, 0);
    u16 col_m = (cursor==3)? RGB5(31, 0, 0) : RGB5(0, 0, 0);

    Mode3DrawSJISStr(&f_JP, 100,100, "スタート", col_s);
    Mode3DrawSJISStr(&f_JP, 100,115, "設定", col_t);
    Mode3DrawSJISStr(&f_JP, 100,130, "クレジット", col_c);
    Mode3DrawSJISStr(&f_JP, 100,145, "操作方法", col_m);
}
