#ifndef _SCENE_H_
#define _SCENE_H_

enum {
    SCENE_MENU = 0,
    SCENE_SETTING = 1,
    SCENE_CREDIT = 2,
    SCENE_GAME = 3
};

void Menu_Init(s32 scene);
void Menu_Update();
void Menu_Draw();

void Setting_Init(s32 scene);
void Setting_Update();
void Setting_Draw();

void Credit_Init(s32 scene);
void Credit_Update();
void Credit_Draw();

void Game_Init(s32 scene);
void Game_Update();
void Game_Draw();

extern void ChangeScene(s32 next);

#endif
