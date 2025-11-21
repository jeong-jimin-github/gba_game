#include "lib/gba.h"
#include "scene.h"
#include "m3func.h"
#include "res.h"
#include "gameres.h"
#include "sound.h"
#include "music.h"

//---------------- 定数定義 ----------------

#define BG_MAX_CNT   4
#define ENEMY_MAX    3 // 画面に表示される敵の最大数
#define BULLET_MAX   10 // 画面に表示される弾の最大数

#define ENEMY_SPACING     240 // 敵の出現間隔
#define ENEMY_DESPAWN_L   120 // 敵の消える位置
#define RESPAWN_AHEAD     320 // 敵の再出現位置（X座標）

//---------------- 構造体定義 ----------------

typedef struct
{
    s32 x;
    s32 y;
} ObjPos;

typedef struct
{
    s32 x;
    s32 y;
    s32 vx;
    s32 vy;
    int isActive;
} Weapon;

typedef struct {
    u32  mapBase;
    u16* mapBaseAdr;
    u32  tileBase;
    u16* tileBaseAdr;
} ST_BG;

//---------------- グローバル変数 ----------------

static ST_FONT f; // ローマ字フォント
static ST_FONT f_JP; // 日本語フォント

Weapon weapon;
ST_BG Bg[BG_MAX_CNT];

extern int currentScene;

const int GROUND_Y = 104; // 地面のY座標
const int GRAV_ACC = 1;
const int JUMP_VEL = -14;
const int MAX_FALL = 10;

const int MOVE_ACC = 1; // 移動加速度
const int MOVE_MAX = 3;
const int FRICTION = 1; // 摩擦による減速

int cameraX = 0;
int bulletTimer = 0;
int enemyRespawnTimer = 0;

// ---------------- プレイヤー位置・速度 ----------------
s32 px = 0;
s32 py = GROUND_Y;
s32 vx = 0;
s32 vy = 0;

// ---------------- 敵・弾位置 ----------------

ObjPos enemy[ENEMY_MAX];
ObjPos bullet[BULLET_MAX];

//---------------- 背景関連 ----------------

void BgInitMem(void)
{
    const u32 mapBase[]  = { 8, 9, 10, 11 };
    const u32 tileBase[] = { 0, 0, 0, 0 };
    vs32 i;

    for (i = 0; i < BG_MAX_CNT; i++)
    {
        Bg[i].mapBase     = MAP_BASE(mapBase[i]);
        Bg[i].mapBaseAdr  = MAP_BASE_ADR(mapBase[i]);
        Bg[i].tileBase    = TILE_BASE(tileBase[i]);
        Bg[i].tileBaseAdr = TILE_BASE_ADR(tileBase[i]);
    }

    for (i = 0; i < 32 * 32; i++)
    {
        Bg[0].mapBaseAdr[i] = 0;
        Bg[1].mapBaseAdr[i] = 0;
        Bg[2].mapBaseAdr[i] = 0;
        Bg[3].mapBaseAdr[i] = 0;
    }

    for (i = 0; i < 0x2000; i++)
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
    REG_BG0CNT = (BG_SIZE_0 | BG_256_COLOR | Bg[0].tileBase | Bg[0].mapBase);
}

void Bg0SetTile(u16* pDat, u32 size)
{
    vu32 i;
    for (i = 0; i < size; i++) Bg[0].tileBaseAdr[i] = pDat[i];
}

void Bg0SetPal(u16* pDat)
{
    vu32 i;
    for (i = 0; i < 256; i++) BG_PALETTE[i] = pDat[i];
}

void Bg0SetMap(u16* pDat, u32 size)
{
    vu32 i;
    for (i = 0; i < size; i++) Bg[0].mapBaseAdr[i] = pDat[i];
}

//---------------- スプライト関連 ----------------

void SpriteMove(u32 num, s32 x, s32 y)
{
    OBJATTR* sp = (OBJATTR*)OAM + num;
    sp->attr1 &= 0xfe00;
    sp->attr0 &= 0xff00;
    sp->attr1 |= (x & 0x01FF);
    sp->attr0 |= (y & 0x00FF);
}

void SpriteSetSize(u32 num, u32 size, u32 form, u32 col)
{
    OBJATTR* sp = (OBJATTR*)OAM + num;
    sp->attr0 &= 0x1FFF;
    sp->attr1 &= 0x3FFF;
    sp->attr0 |= col | form;
    sp->attr1 |= size;
}

void SpriteSetChr(u32 num, u32 ch)
{
    OBJATTR* sp = (OBJATTR*)OAM + num;
    sp->attr2 &= 0xFC00;
    sp->attr2 |= ch;
}

void SpriteInit(void)
{
    u32 i;
    for (i = 0; i < 128; i++) SpriteMove(i, 240, 160);
}

//---------------- 敵・弾初期化・更新 ----------------

void InitEnemies()
{
    for (int i = 0; i < ENEMY_MAX; i++) {
        enemy[i].x = 200 + ENEMY_SPACING * i;
        enemy[i].y = GROUND_Y;

        SpriteSetSize(1 + i, OBJ_SIZE(2), OBJ_SQUARE, OBJ_16_COLOR);
        SpriteSetChr(1 + i, 16);

        SpriteMove(1 + i, 240, 160);
    }
}

void InitBullets()
{
    for (int i = 0; i < BULLET_MAX; i++) {
        bullet[i].x = cameraX - 1000;
        bullet[i].y = GROUND_Y;

        SpriteSetSize(11 + i, OBJ_SIZE(1), OBJ_SQUARE, OBJ_16_COLOR);
        SpriteSetChr(11 + i, 36);
        SpriteMove(11 + i, 240, 160);
    }
}

void UpdateEnemies()
{
    if (enemyRespawnTimer > 300) {
        s32 farthest = enemy[0].x;
        for (int i = 1; i < ENEMY_MAX; i++) {
            if (enemy[i].x > farthest) farthest = enemy[i].x;
        }

        for (int i = 0; i < ENEMY_MAX; i++) {
            if (enemy[i].x < cameraX - ENEMY_DESPAWN_L) {
                if (farthest < cameraX + RESPAWN_AHEAD)
                    farthest = cameraX + RESPAWN_AHEAD;

                enemy[i].x = farthest + ENEMY_SPACING;
                enemy[i].y = GROUND_Y;
                break;
            }
        }
        enemyRespawnTimer = 0;
    }
}

void SpawnBullet(s32 ex, s32 ey)
{
    for (int i = 0; i < BULLET_MAX; i++) {
        if (bullet[i].x < cameraX - 60) {
            bullet[i].x = ex;
            bullet[i].y = ey;
            return;
        }
    }
}

void UpdateBullets()
{
    bulletTimer++;

    if (bulletTimer >= 100) {
        for (int i = 0; i < ENEMY_MAX; i++) {
            int sx = enemy[i].x - cameraX;
            if (sx >= 0 && sx < 240) {
                SpawnBullet(enemy[i].x, enemy[i].y);
            }
        }
        bulletTimer = 0;
    }

    for (int i = 0; i < BULLET_MAX; i++) {
        bullet[i].x -= 1;
    }
}

//---------------- 武器初期化・更新 ----------------

void WeaponInit(Weapon* w)
{
    w->isActive = 0;
    SpriteSetSize(40, OBJ_SIZE(1), OBJ_SQUARE, OBJ_16_COLOR);
    SpriteSetChr(40, 40);
    w->x = px;
    w->y = py;
    SpriteMove(40, w->x - cameraX, w->y);
}

void WeaponUpdate(Weapon* w)
{
    if (w->isActive == 1) {
        w->vx += MOVE_ACC;
        if (w->vx > MOVE_MAX) w->vx = MOVE_MAX;
        w->x += w->vx;
        if(w->x < cameraX - 16 || w->x > cameraX + 240){
            w->isActive = 2;
        }
    }
    if(w->isActive == 2){
        if((w->x > px - 10 && w->x < px + 10)  &&  (w->y > py - 10 && w->y < py + 10)){
            w->isActive = 0;
            w->x = px;
            w->y = py;
        }
        if(w->x > px){
            w->vx -= MOVE_ACC;
            if (w->vx < -MOVE_MAX) w->vx = -MOVE_MAX;
        }
        if (w->x < px)
        {
            w->vx += MOVE_ACC;
            if (w->vx > MOVE_MAX) w->vx = MOVE_MAX;
        }
        if(w->y < py){
            w->vy += MOVE_ACC;
            if (w->vy > MOVE_MAX) w->vy = MOVE_MAX;
        }
        if (w->y > py)
        {
            w->vy -= MOVE_ACC;
            if (w->vy < -MOVE_MAX) w->vy = -MOVE_MAX;
        }
        w->x += w->vx;
        w->y += w->vy;
    }
    if(w->isActive == 0){ {
        w->x = px;
        w->y = py;
    }}
}

void GameOver()
{
    StopMusic();
    SetMode(MODE_3 | BG2_ENABLE);
    for (int i = 0; i < 240; i++)
        for (int j = 0; j < 160; j++)
            Mode3PutPixel(i, j, RGB5(0,0,0));
    Mode3DrawString(&f, 80, 70, "Game Over", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 10, 90, "もう一度プレイ", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 10, 110, "メニュー画面に戻る", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 160, 90, "スタートボタン", RGB5(31,31,31));
    Mode3DrawSJISStr(&f_JP, 160, 110, "セレクトボタン", RGB5(31,31,31));

    while(1){
        u32 key = ~(REG_KEYINPUT);
        if (key & KEY_SELECT) {
            currentScene = SCENE_MENU;
            ChangeScene(currentScene);
            break;
        }

        if (key & KEY_START) {
            Game_Init(SCENE_GAME);
            break;
    }}
}

//---------------- ゲーム初期化・更新・描画 ----------------

void Game_Init(int scene)
{
    SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP | BG0_ON);
    f_JP.pDat  = (u8*)&mplus_j10rBitmap;
    f_JP.imgCx = 69632;
    f_JP.cx    = 10;
    f_JP.cy    = 11;
    f_JP.pSheet= (u16*)&mplus_jfnt_txt;
    f_JP.cnt   = 6963;

    f.pDat  = (u8*)&k6x10Bitmap;
    f.imgCx = 960;
    f.cx    = 6;
    f.cy    = 10;

    if (scene != SCENE_GAME) return;

    u16* oam = OBJ_BASE_ADR;
    u16* pal = OBJ_COLORS;

    for (u32 i = 0; i < spr1TilesLen/2; i++) oam[i] = spr1Tiles[i];
    for (u32 i = 0; i < 256; i++) pal[i] = spr1Pal[i];

    BgInit();
    Bg0SetTile((u16*)&bg0Tiles, bg0TilesLen/2);
    Bg0SetPal ((u16*)&bg0Pal);
    Bg0SetMap ((u16*)&ResBg0Map, BG0_MAP_SIZE/2);

    px = 0;
    py = GROUND_Y;
    vx = 0;
    vy = 0;

    cameraX = 0;

    SpriteInit();

    SpriteSetSize(50, OBJ_SIZE(2), OBJ_SQUARE, OBJ_16_COLOR);
    SpriteSetChr (50, 0);
    SpriteMove   (50, px, py);

    InitEnemies();
    InitBullets();
    WeaponInit(&weapon);

    InitMusic();
}

void Game_Update()
{
    if (currentScene != SCENE_GAME) return;

    if (!(REG_KEYINPUT & KEY_SELECT)) {
        currentScene = SCENE_MENU;
        StopMusic();
        SetMode(MODE_3 | BG2_ENABLE);
        ChangeScene(currentScene);
        return;
    }

    u32 key = ~(REG_KEYINPUT);

    if(key & KEY_B) {
        if(weapon.isActive == 0){
            weapon.isActive = 1;
        }
    }

    if (key & KEY_RIGHT) {
        vx += MOVE_ACC;
        if (vx > MOVE_MAX) vx = MOVE_MAX;
    }
    else if (key & KEY_LEFT) {
        vx -= MOVE_ACC;
        if (vx < -MOVE_MAX) vx = -MOVE_MAX;
    }
    else {
        if (vx > 0) vx -= FRICTION;
        else if (vx < 0) vx += FRICTION;
    }

    px += vx;

    if(px-cameraX < 0) {
        px = cameraX;
    }

    if (key & KEY_A) {
        if (py == GROUND_Y) vy = JUMP_VEL;
    }

    vy += GRAV_ACC;
    if (vy > MAX_FALL) vy = MAX_FALL;

    py += vy;

    if (py >= GROUND_Y) {
        py = GROUND_Y;
        vy = 0;
    }

    if (px - cameraX > 120) {
        cameraX = px - 120;
        if (cameraX < 0) cameraX = 0;
        REG_BG0HOFS = cameraX;
    }
    enemyRespawnTimer++;

    UpdateEnemies();
    UpdateBullets();
    WeaponUpdate(&weapon);

    PlayMusic(&UnrealSuperHero3);
}

void Game_Draw()
{
    if (currentScene != SCENE_GAME) return;

    SpriteMove(50, px - cameraX, py);
    SpriteMove(40, weapon.x - cameraX, weapon.y);

    for (int i = 0; i < ENEMY_MAX; i++) {
        int sx = enemy[i].x - cameraX;
        if (sx < -32 || sx >= 240) {
            SpriteMove(1 + i, 240, 160);
        } else {
            SpriteMove(1 + i, sx, enemy[i].y);
        }
    }

    for (int i = 0; i < BULLET_MAX; i++) {
        int bx = bullet[i].x - cameraX;
        if (bx < -16 || bx >= 240) {
            SpriteMove(11 + i, 240, 160);
        } else {
            SpriteMove(11 + i, bx, bullet[i].y);
        }
    }

    for (int i = 0; i < BULLET_MAX; i++) {
        int bx = bullet[i].x - cameraX;
        int by = bullet[i].y;
        if ((bx < px - cameraX +32 && bx > px - cameraX) && (by >= py && by < py + 32)) {
            GameOver();
        }
    }
}