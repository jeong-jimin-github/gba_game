#include "lib/gba.h"
#include "scene.h"
#include "m3func.h"
#include "res.h"

extern int currentScene;
static ST_FONT f_JP;

void Setting_Init(int scene) {
    f_JP.pDat  = (u8*)&mplus_j10rBitmap;
    f_JP.imgCx = 69630 + 2;
    f_JP.cx    = 10;
    f_JP.cy    = 11;
    f_JP.pSheet= (u16*)&mplus_jfnt_txt;
    f_JP.cnt   = 6963;

    if(scene != SCENE_SETTING) return;

    for(int i=0; i<240; i++){
        for(int j=0; j<160; j++){
            Mode3PutPixel(i, j, RGB5(0,0,0));
        }
    }

}

void Setting_Update() {
    if(currentScene != SCENE_SETTING) return;

    if(!(REG_KEYINPUT & KEY_B)) {
        currentScene = SCENE_MENU;
        ChangeScene(currentScene);
    }
}

void Setting_Draw() {
    if(currentScene != SCENE_SETTING) return;
    Mode3DrawSJISStr(&f_JP, 5, 0, "Ý’è", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 5, 30, "ŠJ”­ŽÒƒ‚[ƒh", RGB5(31,31,31));
}
