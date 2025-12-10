#include "lib/gba.h"
#include "scene.h"
#include "commonfunc.h"
#include "sound.h"
#include "music.h"
#include "res.h"

extern s32 currentScene;
static ST_FONT f;
MUSIC_PLAYER music;

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
    InitMusic();
}

void Setting_Update() {
    if(currentScene != SCENE_SETTING) return;

    if(!(REG_KEYINPUT & KEY_B)) {
        currentScene = SCENE_MENU;
        ChangeScene(currentScene);
    }
    if(!(REG_KEYINPUT & KEY_L)) {
        StopMusic();
        InitMusic();
        music = UnrealSuperHero3;
    }
    if(!(REG_KEYINPUT & KEY_R)) {
        StopMusic();
        InitMusic();
        music = Owen;
    }
    PlayMusic(&music);
}

void Setting_Draw() {
    if(currentScene != SCENE_SETTING) return;
    Mode3DrawString(&f, 60, 20, "Audio TEST", RGB5(31,31,31));
    Mode3DrawString(&f, 60, 30, "L Button Main theme", RGB5(31,31,31));
    Mode3DrawString(&f, 60, 40, "R Button Credit theme", RGB5(31,31,31));
}
