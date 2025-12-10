#ifndef _COMMONFUNC_H_
#define _COMMONFUNC_H_

typedef struct {
    u8*  pDat;
    s32  imgCx;
    s32  cx;
    s32  cy;

    u16* pSheet;
    s32  cnt;
} ST_FONT;

void LoadFont();
void LoadFontJP();
void WaitForVSync();
void Mode3PutPixel(u32 x, u32 y, u16 color);
void Mode3DrawImage(u32 x, u32 y, const u16* image);
void Mode3DrawSJISChr(ST_FONT* f_JP, s32 sx, s32 sy, u16 chr, u16 col);
void Mode3DrawSJISStr(ST_FONT* f_JP, s32 sx, s32 sy, char* str, u16 col);
void Mode3DrawString(ST_FONT* font, s32 sx, s32 sy, char* str, u16 color);
void Mode3DrawChar(ST_FONT* font, s32 sx, s32 sy, u16 chr, u16 color);

#endif // _COMMONFUNC_H_