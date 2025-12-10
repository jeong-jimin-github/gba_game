#include "lib/gba.h"
#include "commonfunc.h"
#include "scene.h"

s32 currentScene = SCENE_MENU;

void ChangeScene(int next) {
    currentScene = next;

    if(next == SCENE_MENU) Menu_Init(next);
    else if(next == SCENE_SETTING) Setting_Init(next);
    else if(next == SCENE_CREDIT) Credit_Init(next);
    else if(next == SCENE_GAME) Game_Init(next);
    else if(next == SCENE_MANUAL) Manual_Init(next);
}

int main() {
    SetMode(MODE_3 | BG2_ENABLE);
    ChangeScene(SCENE_MENU);

    while(1){
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
