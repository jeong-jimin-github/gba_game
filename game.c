#include "lib/gba.h"
#include "scene.h"
#include "commonfunc.h"
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

#define GROUND_Y 104
#define GRAV_ACC 1
#define JUMP_VEL -14
#define MAX_FALL 10

#define MOVE_ACC 1 // 移動加速度
#define MOVE_MAX 3
#define FRICTION 1 // 摩擦による減速

#define MOVE_ACC_WEAPON 2 // 武器の移動加速度
#define MOVE_MAX_WEAPON 6

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
    s32 LR; // 0:左 1:右
    s32 isActive;
} Weapon;

typedef struct {
    u32  mapBase;
    u16* mapBaseAdr;
    u32  tileBase;
    u16* tileBaseAdr;
} ST_BG;

typedef struct {
    u32 startx;
    u32 starty;
    u32 endx;
    u32 endy;
} Block;


//---------------- グローバル変数 ----------------

static ST_FONT f; // ローマ字フォント
static ST_FONT f_JP; // 日本語フォント

Weapon weapon;
ST_BG Bg[BG_MAX_CNT];
Block BLC[1];

extern s32 currentScene;

s32 animtimer = 0;

s32 cameraX = 0;
s32 cameraCtr = 0;
s32 bulletTimer = 100;
s32 enemyRespawnTimer = 0;

s32 canjump = 1;
s32 canaction = 1;

// ---------------- プレイヤー位置・速度 ----------------
s32 px = 0;
s32 py = GROUND_Y - 24;
s32 vx = 0;
s32 vy = 0;
s32 iswalk = 0;
s32 pLR = 1; // 0:左 1:右

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

void SpriteMirror(u32 num, u32 flag)
{
    OBJATTR* sp = (OBJATTR*)OAM + num;
    if (flag)
        sp->attr1 |= (1 << 12);
    else
        sp->attr1 &= ~(1 << 12);
}

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
    for (s32 i = 0; i < ENEMY_MAX; i++) {
        enemy[i].x = 200 + ENEMY_SPACING * i;
        enemy[i].y = GROUND_Y - 24;

        SpriteSetSize(1 + i, OBJ_SIZE(3), OBJ_TALL, OBJ_16_COLOR);
        SpriteSetChr(1 + i, 32);
        SpriteMove(1 + i, 240, 160);
        SpriteMirror(1 + i, 1);
    }
}

void InitBullets()
{
    for (s32 i = 0; i < BULLET_MAX; i++) {
        bullet[i].x = cameraX - 1000;
        bullet[i].y = GROUND_Y;

        SpriteSetSize(11 + i, OBJ_SIZE(1), OBJ_SQUARE, OBJ_16_COLOR);
        SpriteSetChr(11 + i, 72);
        SpriteMove(11 + i, 240, 160);
    }
}

void UpdateEnemies()
{
    if (enemyRespawnTimer > 300) {
        s32 farthest = enemy[0].x;
        for (s32 i = 1; i < ENEMY_MAX; i++) {
            if (enemy[i].x > farthest) farthest = enemy[i].x;
        }

        for (s32 i = 0; i < ENEMY_MAX; i++) {
            if (enemy[i].x < cameraX - ENEMY_DESPAWN_L) {
                if (farthest < cameraX + RESPAWN_AHEAD)
                    farthest = cameraX + RESPAWN_AHEAD;

                enemy[i].x = farthest + ENEMY_SPACING;
                enemy[i].y = GROUND_Y - 24;
                break;
            }
        }
        enemyRespawnTimer = 0;
    }
}

void SpawnBullet(s32 ex, s32 ey)
{
    for (s32 i = 0; i < BULLET_MAX; i++) {
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
        for (s32 i = 0; i < ENEMY_MAX; i++) {
            s32 sx = enemy[i].x - cameraX;
            if (sx >= 0 && sx < 240) {
                SpawnBullet(enemy[i].x, enemy[i].y + 24);
            }
        }
        bulletTimer = 0;
    }

    for (s32 i = 0; i < BULLET_MAX; i++) {
        bullet[i].x -= 1;
    }
}

void CheckBlockCollision()
{
    s32 player_left = px - cameraX;
    s32 player_right = px + 32 - cameraX;
    s32 player_top = py;
    s32 player_bottom = py + 32;

    for (s32 i = 0; i < 1; i++) {
        s32 block_left = BLC[i].startx - cameraCtr;
        s32 block_right = BLC[i].endx - cameraCtr;
        s32 block_top = BLC[i].starty;
        s32 block_bottom = BLC[i].endy;

        if (player_right > block_left && player_left < block_right &&
            player_bottom > block_top && player_top < block_bottom) {
        
            s32 overlap_left = player_right - block_left;
            s32 overlap_right = block_right - player_left;
            s32 overlap_top = player_bottom - block_top;
            s32 overlap_bottom = block_bottom - player_top;

            s32 min_overlap = overlap_left;
            if (overlap_right < min_overlap) min_overlap = overlap_right;
            if (overlap_top < min_overlap) min_overlap = overlap_top;
            if (overlap_bottom < min_overlap) min_overlap = overlap_bottom;

            // 上
            if (min_overlap == overlap_top) {
                py = block_top - 32;
                vy = 0;
            }
            // 下
            else if (min_overlap == overlap_bottom) {
                py = block_bottom;
                vy = 0;
            }
            // 左
            else if (min_overlap == overlap_left) {
                px = block_left - 32;
            }
            // 右
            else if (min_overlap == overlap_right) {
                px = block_right;
            }
        }
    }
}

//---------------- 武器初期化・更新 ----------------

void WeaponInit(Weapon* w)
{
    w->isActive = 0;
    SpriteSetSize(40, OBJ_SIZE(2), OBJ_WIDE, OBJ_16_COLOR);
    SpriteSetChr(40, 64);
    w->x = px;
    w->y = py + 24;
    SpriteMove(40, w->x - cameraX, w->y);
}

void WeaponUpdate(Weapon* w)
{
    if (w->isActive == 1) {
        if(w->LR == 0){
            w->vx -= MOVE_ACC_WEAPON;
            if (w->vx < -MOVE_MAX_WEAPON) w->vx = -MOVE_MAX_WEAPON;
            w->x += w->vx;
            if(w->x < cameraX){
                w->isActive = 2;
            }
        }
        if (w->LR == 1){
            w->vx += MOVE_ACC_WEAPON;
            if (w->vx > MOVE_MAX_WEAPON) w->vx = MOVE_MAX_WEAPON;
            w->x += w->vx;
            if(w->x > cameraX + 240){
                w->isActive = 2;
            }
        }
    }
    if(w->isActive == 2){
        if((w->x > px - 32 && w->x < px + 32)  &&  (w->y > py - 32 && w->y < py + 32)){
            w->isActive = 0;
            w->x = px;
            w->y = py + 24;
        }
        if(w->x > px){
            w->vx -= MOVE_ACC_WEAPON;
            if (w->vx < -MOVE_MAX_WEAPON) w->vx = -MOVE_MAX_WEAPON;
        }
        if (w->x < px)
        {
            w->vx += MOVE_ACC_WEAPON;
            if (w->vx > MOVE_MAX) w->vx = MOVE_MAX_WEAPON;
        }
        if(w->y < py + 24){
            w->vy += MOVE_ACC_WEAPON;
            if (w->vy > MOVE_MAX_WEAPON) w->vy = MOVE_MAX_WEAPON;
        }
        if (w->y > py + 24)
        {
            w->vy -= MOVE_ACC_WEAPON;
            if (w->vy < -MOVE_MAX_WEAPON) w->vy = -MOVE_MAX_WEAPON;
        }
        w->x += w->vx;
        w->y += w->vy;
    }
    if(w->isActive == 0){ {
        if(pLR == 0){
            SpriteMirror(40, 1);
        }
        if(pLR == 1){
            SpriteMirror(40, 0);
        }
        w->x = px;
        w->y = py + 24;
    }}
}

void GameOver()
{
    StopMusic();
    SetMode(MODE_3 | BG2_ENABLE);
    for (s32 i = 0; i < 240; i++)
        for (s32 j = 0; j < 160; j++)
            Mode3PutPixel(i, j, RGB5(0,0,0));
    Mode3DrawString(&f, 90, 70, "Game Over", RGB5(31,31,31));
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

void Game_Init(s32 scene)
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

    BLC[0].startx = 104;
    BLC[0].endx = 128;
    BLC[0].starty = 32;
    BLC[0].endy = 64;

    px = 0;
    py = GROUND_Y - 24;
    vx = 0;
    vy = 0;

    cameraX = 0;
    cameraCtr = 0;

    SpriteInit();

    SpriteSetSize(50, OBJ_SIZE(2), OBJ_SQUARE, OBJ_16_COLOR);
    SpriteSetChr (50, 0);
    SpriteMove   (50, px, py);

    SpriteSetSize(51, OBJ_SIZE(2), OBJ_SQUARE, OBJ_16_COLOR);
    SpriteSetChr (51, 16);
    SpriteMove   (51, px, py + 32);

    InitEnemies();
    InitBullets();
    WeaponInit(&weapon);

    InitMusic();
}

void Game_Update()
{
    if (currentScene != SCENE_GAME) return;
    u32 key = ~(REG_KEYINPUT);

    if (key& KEY_SELECT) {
        currentScene = SCENE_MENU;
        StopMusic();
        SetMode(MODE_3 | BG2_ENABLE);
        ChangeScene(currentScene);
        return;
    }

    if ((key & KEY_A) && (canjump)) {
        canjump = 0;
        if (py == GROUND_Y - 24) vy = JUMP_VEL;
    }

    if(!canjump && (~(key) & KEY_A)){
        canjump = 1;
    }

    if((key & KEY_B) && (canaction)) {
        canaction = 0;
        if(weapon.isActive == 0){
            weapon.LR = pLR;
            weapon.isActive = 1;
        }
    }

    if(!canaction && (~(key) & KEY_B)){
        canaction = 1;
    }

    if (key & KEY_RIGHT) {
        iswalk = 1;
        pLR = 1;
        vx += MOVE_ACC;
        if (vx > MOVE_MAX) vx = MOVE_MAX;
    }
    else if (key & KEY_LEFT) {
        iswalk = 1;
        pLR = 0;
        vx -= MOVE_ACC;
        if (vx < -MOVE_MAX) vx = -MOVE_MAX;
    }
    else {
        iswalk = 0;
        if (vx > 0) vx -= FRICTION;
        else if (vx < 0) vx += FRICTION;
    }

    px += vx;

    if(px-cameraX < 0) {
        px = cameraX;
    }

    vy += GRAV_ACC;
    if (vy > MAX_FALL) vy = MAX_FALL;

    py += vy;

    if (py >= GROUND_Y - 24) {
        py = GROUND_Y - 24;
        vy = 0;
    }

    CheckBlockCollision();

    if (px - cameraX > 80) {
        cameraX = px - 80;
        if (cameraX < 0) cameraX = 0;
        cameraCtr = cameraX;
        while (cameraCtr > 256)
        {
            cameraCtr = cameraCtr - 256;
        }
        
        REG_BG0HOFS = cameraX;
    }
    enemyRespawnTimer++;

    if(pLR == 0){
        SpriteMirror(50, 1);
        SpriteMirror(51, 1);
    }
    if(pLR == 1){
        SpriteMirror(50, 0);
        SpriteMirror(51, 0);
    }

    if(iswalk == 1){
        animtimer++;
        if(animtimer > 10){
            if(animtimer > 20){
                animtimer = 0;
            }
            else {
                animtimer++;
            }
            SpriteSetChr(51, 16);
        }
        else{
            SpriteSetChr(51, 80);
        }
    }
    
    else{
        SpriteSetChr(51, 16);
    }

    UpdateEnemies();
    UpdateBullets();
    WeaponUpdate(&weapon);

    PlayMusic(&UnrealSuperHero3);

    char buf[256];
    _Sprintf(buf, "x: %d, y: %d, cameraX: %d, cameractr: %d", px, py, cameraX, cameraCtr);
    MgbaLog(buf);
}

void Game_Draw()
{
    if (currentScene != SCENE_GAME) return;

    SpriteMove(50, px - cameraX, py);
    SpriteMove(51, px - cameraX, py + 32);
    SpriteMove(40, weapon.x - cameraX, weapon.y);

    for (s32 i = 0; i < ENEMY_MAX; i++) {
        s32 sx = enemy[i].x - cameraX;
        if (sx < -32 || sx >= 240) {
            SpriteMove(1 + i, 240, 160);
        } else {
            SpriteMove(1 + i, sx, enemy[i].y);
        }
    }

    if(weapon.isActive == 1 || weapon.isActive == 2){
        for (s32 ii = 0; ii < ENEMY_MAX; ii++) {
            if((weapon.x - cameraX < enemy[ii].x - cameraX + 32 && weapon.x - cameraX > enemy[ii].x - cameraX) && (weapon.y >= enemy[ii].y && weapon.y < enemy[ii].y + 32))
            {
                enemy[ii].x = cameraX - ENEMY_DESPAWN_L - 100;
            }
        }
    }

    for (s32 i = 0; i < BULLET_MAX; i++) {
        s32 bx = bullet[i].x - cameraX;
        if (bx < -16 || bx >= 240) {
            SpriteMove(11 + i, 240, 160);
        } else {
            SpriteMove(11 + i, bx, bullet[i].y);
        }
    }

    for (s32 i = 0; i < BULLET_MAX; i++) {
        s32 bx = bullet[i].x - cameraX;
        s32 by = bullet[i].y;
        if ((bx < px - cameraX +32 && bx > px - cameraX) && (by >= py && by < py + 32)) {
            GameOver();
        }
    }
}