#include "lib/gba.h"
#include "scene.h"
#include "commonfunc.h"
#include "res.h"
#include <maxmod.h>
#include "soundbank.h"

extern s32 currentScene;

static ST_FONT f_JP;
static int lastest_cursor = 0;
static int cursor = 0;
static u16 prev_key_input = 0;

void Change_Menu() {
    if(lastest_cursor == 0){
        Mode3DrawSJISStr(&f_JP, 100,100, "スタート", RGB5(0,0,0));
    }
    if(lastest_cursor == 1){
        Mode3DrawSJISStr(&f_JP, 100,115, "設定", RGB5(0,0,0));
    }
    if(lastest_cursor == 2){
        Mode3DrawSJISStr(&f_JP, 100,130, "クレジット", RGB5(0,0,0));
    }
    if(lastest_cursor == 3){
        Mode3DrawSJISStr(&f_JP, 100,145, "操作方法", RGB5(0,0,0));
    }

    if (cursor == 0)
    {
        Mode3DrawSJISStr(&f_JP, 100,100, "スタート", RGB5(31, 0, 0));
    }
    if (cursor == 1)
    {
        Mode3DrawSJISStr(&f_JP, 100,115, "設定", RGB5(31, 0, 0));
    }
    if (cursor == 2)
    {
        Mode3DrawSJISStr(&f_JP, 100,130, "クレジット", RGB5(31, 0, 0));
    }
    if (cursor == 3)
    {
        Mode3DrawSJISStr(&f_JP, 100,145, "操作方法", RGB5(31, 0, 0));
    }
    lastest_cursor = cursor;
}

void Menu_Init(int scene) {
    if(scene != SCENE_MENU) return;
    
    f_JP.pDat  = (u8*)&mplus_j10rBitmap;
    f_JP.imgCx = 69630 + 2;
    f_JP.cx    = 10;
    f_JP.cy    = 11;
    f_JP.pSheet= (u16*)&mplus_jfnt_txt;
    f_JP.cnt   = 6963;

    Mode3DrawImage(0,0, menubgBitmap);

    Mode3DrawSJISStr(&f_JP, 100,100, "スタート", RGB5(0,0,0));
    Mode3DrawSJISStr(&f_JP, 100,115, "設定", RGB5(0,0,0));
    Mode3DrawSJISStr(&f_JP, 100,130, "クレジット", RGB5(0,0,0));
    Mode3DrawSJISStr(&f_JP, 100,145, "操作方法", RGB5(0,0,0));
    mmStart( MOD_SECONDHEAVEN, MM_PLAY_LOOP );
    Change_Menu();
}

void Menu_Update() {
    if(currentScene != SCENE_MENU) return;

    u16 current_key_input = REG_KEYINPUT;
    u16 pressed_keys = ~current_key_input & prev_key_input;

    if(pressed_keys & KEY_UP) {
        if(cursor > 0) {
            cursor--;
            Change_Menu();
        }
        else {
        }
    }
    if(pressed_keys & KEY_DOWN) {
        if(cursor < 3){
            cursor++;
            Change_Menu();
        }
        else {
        }
    }

    if(pressed_keys & KEY_A) {
        mmStop();
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
}

