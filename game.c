#include "lib/gba.h"
#include "scene.h"
#include "m3func.h"
#include "res.h"
#include "gameres.h"
#include "sound.h"
#define BG_MAX_CNT 4

typedef struct {
    u32  mapBase;
    u16* mapBaseAdr;
    u32  tileBase;
    u16* tileBaseAdr;
} ST_BG;

ST_BG Bg[BG_MAX_CNT];

int gravity = 0;

extern int currentScene;
static ST_FONT f_JP;

void BgInitMem(void)
{
    const u32 mapBase[]  = {  8,  9, 10, 11 };
    const u32 tileBase[] = {  0,  0,  0,  0 };
    vs32 i;

    for(i=0; i<BG_MAX_CNT; i++)
    {
        Bg[i].mapBase     = MAP_BASE(mapBase[i]);
        Bg[i].mapBaseAdr  = MAP_BASE_ADR(mapBase[i]);
        Bg[i].tileBase    = TILE_BASE(tileBase[i]);
        Bg[i].tileBaseAdr = TILE_BASE_ADR(tileBase[i]);
    }

    for(i=0; i<32*32; i++)
    {
        Bg[0].mapBaseAdr[i] = 0;
        Bg[1].mapBaseAdr[i] = 0;
        Bg[2].mapBaseAdr[i] = 0;
        Bg[3].mapBaseAdr[i] = 0;
    }

    for(i=0; i<0x2000; i++)
    {
        Bg[0].tileBaseAdr[i] = 0;
        Bg[1].tileBaseAdr[i] = 0;
        Bg[2].tileBaseAdr[i] = 0;
        Bg[3].tileBaseAdr[i] = 0;
    }
}

void BgInit(void)
{
    BgInitMem();

    REG_BG0CNT  = (BG_SIZE_0 | BG_256_COLOR | Bg[0].tileBase | Bg[0].mapBase | 0);
}

void Bg0SetTile(u16* pDat, u32 size)
{
    vu32 i;
    for(i=0; i<size; i++) Bg[0].tileBaseAdr[i] = pDat[i];
}

void Bg0SetPal(u16* pDat)
{
    vu32 i;
    for(i=0; i<256; i++) BG_PALETTE[i] = pDat[i];
}

void Bg0SetMap(u16* pDat, u32 size)
{
    vu32 i;
    for(i=0; i<size; i++) Bg[0].mapBaseAdr[i] = pDat[i];
}

void SpriteMove(u32 num, s32 x, s32 y)
{
    OBJATTR* sp = (OBJATTR*)OAM + num;
    sp->attr1 &= 0xfe00;
    sp->attr0 &= 0xff00;
    sp->attr1 |= (x & 0x01ff);
    sp->attr0 |= (y & 0x00ff);
}

void SpriteSetSize(u32 num, u32 size, u32 form, u32 col)
{
    OBJATTR* sp = (OBJATTR*)OAM + num;
    sp->attr0 &= 0x1fff;
    sp->attr1 &= 0x3fff;
    sp->attr0 |= col | form;
    sp->attr1 |= size;
}

void SpriteSetChr(u32 num, u32 ch)
{
    OBJATTR* sp = (OBJATTR*)OAM + num;
    sp->attr2 &= 0xfc00;
    sp->attr2 |= ch;
}

void SpriteInit(void)
{
    u32 i;
    for(i=0; i<128; i++) SpriteMove(i, 240, 160);
}

// --- プレイヤー物理変数 ---
s32 x = 0;
s32 y = 104;
s32 vy = 0;

s32 vx = 0;

const int GROUND_Y = 104;

const int GRAV_ACC = 1;
const int JUMP_VEL = -14;
const int MAX_FALL = 10;

const int MOVE_ACC = 1;        // 加速度
const int MOVE_MAX = 3;        // 最大速度
const int FRICTION = 1;        // 摩擦

int cameraX = 0;

void Game_Init(int scene) {
    SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP | BG0_ON);
    f_JP.pDat  = (u8*)&mplus_j10rBitmap;
    f_JP.imgCx = 69630 + 2;
    f_JP.cx    = 10;
    f_JP.cy    = 11;
    f_JP.pSheet= (u16*)&mplus_jfnt_txt;
    f_JP.cnt   = 6963;

    if(scene != SCENE_GAME) return;

    for(int i=0; i<240; i++){
        for(int j=0; j<160; j++){
            Mode3PutPixel(i, j, RGB5(31,31,31));
        }
    }

    u16* oam = OBJ_BASE_ADR;
    u16* pal = OBJ_COLORS;
    u32 i;

    for(i=0; i<spr1TilesLen/2; i++) oam[i] = spr1Tiles[i];
    for(i=0; i<16; i++) pal[i] = spr1Pal[i];

    BgInit();
    Bg0SetTile((u16*)&bg0Tiles, bg0TilesLen/2);
    Bg0SetPal ((u16*)&bg0Pal);
    Bg0SetMap ((u16*)&ResBg0Map, BG0_MAP_SIZE/2);

    x = 0;
    y = GROUND_Y;
    vx = 0;
    vy = 0;

    cameraX = 0;

    SpriteInit();

    SpriteSetSize(0, OBJ_SIZE(2), OBJ_SQUARE, OBJ_16_COLOR);
    SpriteSetChr (0, 0);
    SpriteMove   (0, x, y);
	InitMusic();
}

void Game_Update() {
    if(currentScene != SCENE_GAME) return;

    if(!(REG_KEYINPUT & KEY_SELECT)) {
        currentScene = SCENE_MENU;
        SetMode(MODE_3 | BG2_ENABLE);
        ChangeScene(currentScene);
        return;
    }

    u32 key = ~(REG_KEYINPUT);

    if(key & KEY_RIGHT) {
        vx += MOVE_ACC;
        if(vx > MOVE_MAX) vx = MOVE_MAX;
    }
    else if(key & KEY_LEFT) {
        vx -= MOVE_ACC;
        if(vx < -MOVE_MAX) vx = -MOVE_MAX;
    }
    else {
        if(vx > 0) vx -= FRICTION;
        else if(vx < 0) vx += FRICTION;
    }

	x += vx;

	if(x < cameraX) {
		x = cameraX;
		vx = 0;
	}

    if(x < 0) x = 0;

    if(key & KEY_A) {
        if(y == GROUND_Y) vy = JUMP_VEL;
    }

    vy += GRAV_ACC;
    if(vy > MAX_FALL) vy = MAX_FALL;
    y += vy;

    if(y >= GROUND_Y) {
        y = GROUND_Y;
        vy = 0;
    }
    if(y < 0) {
        y = 0;
        vy = 0;
    }

	if(x - cameraX > 120) {
		int newCam = x - 120;
		if(newCam > cameraX) cameraX = newCam;
		REG_BG0HOFS = cameraX;
	}

	PlayMusic();
}

void Game_Draw() {
    if(currentScene != SCENE_GAME) return;

    SpriteMove(0, x - cameraX, y);
}
