#include "lib/gba.h"
#include "commonfunc.h"
#include "scene.h"
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

s32 currentScene = SCENE_MENU;

void irqInit();
void irqEnable(int);
void irqDisable(int);
void irqSet(int, void (*)(void));

void ChangeScene(int next) {
    currentScene = next;

    if(next == SCENE_MENU) {Menu_Init(next); irqDisable(IRQ_VBLANK);}
    else if(next == SCENE_SETTING) {Setting_Init(next); irqDisable(IRQ_VBLANK);}
    else if(next == SCENE_CREDIT) {Credit_Init(next); irqEnable(IRQ_VBLANK);}
    else if(next == SCENE_GAME) {Game_Init(next); irqEnable(IRQ_VBLANK);}
    else if(next == SCENE_MANUAL) {Manual_Init(next); irqDisable(IRQ_VBLANK);}
}

int main() {
    SetMode(MODE_3 | BG2_ENABLE);

    irqInit();
    mmInitDefault( (void*)soundbank_bin, 10 );

    irqSet(IRQ_VBLANK, mmVBlank);
    irqEnable(IRQ_VBLANK);
    
    ChangeScene(SCENE_MENU);

    while(1){
        mmFrame();
        WaitForVSync();

        if(currentScene == SCENE_MENU) Menu_Update();
        else if(currentScene == SCENE_SETTING) Setting_Update();
        else if(currentScene == SCENE_CREDIT) Credit_Update();
        else if(currentScene == SCENE_GAME) Game_Update();
        else if(currentScene == SCENE_MANUAL) Manual_Update();

        if(currentScene == SCENE_MENU) Menu_Draw();
        else if(currentScene == SCENE_SETTING) Setting_Draw();
        else if(currentScene == SCENE_CREDIT) Credit_Draw();
        else if(currentScene == SCENE_GAME) Game_Draw();
        else if(currentScene == SCENE_MANUAL) Manual_Draw();
    }
}
