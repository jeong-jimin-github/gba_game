#include "lib/gba.h"
#include "m3func.h"

void WaitForVSync(void) {
    while(*(vu16*)0x4000006 >= 160) {};
    while(*(vu16*)0x4000006 < 160) {};
}

void Mode3PutPixel(u32 x, u32 y, u16 color) {
    u16* videoBuffer = (u16*)0x6000000;
    videoBuffer[y * 240 + x] = color;
}

void Mode3DrawImage(u32 x, u32 y, const u16* image) {
    for(u32 row = 0; row < 160; row++) {
        for(u32 col = 0; col < 240; col++) {
            Mode3PutPixel(x + col, y + row, image[row * 240 + col]);
        }
    }
}

void Mode3DrawSJISChr(ST_FONT* p, s32 sx, s32 sy, u16 chr, u16 col)
{
	u8* pDat = p->pDat;
	s32 i;

	for(i=0; i<p->cnt; i++)
	{
		if(chr == p->pSheet[i])
		{
			pDat += i * p->cx;
			break;
		}
	}

	u16* pScreen = (u16*)VRAM + (sy * 240) + sx;
	s32  x, y;

	for(y=0; y<p->cy; y++)
	{
		for(x=0; x<p->cx; x++)
		{
			if(pDat[y*p->imgCx + x] == 0x00)
			{
				continue;
			}

			pScreen[y*240 + x] = col;
		}
	}
}

void Mode3DrawSJISStr(ST_FONT* p, s32 sx, s32 sy, char* str, u16 col)
{
	u16 chr;
	s32 i=0, x=0, y=0;

	for(;;)
	{
		chr = str[i] | (str[i+1] << 8);
		i += 2;

		if(chr == '\0')
		{
			return;
		}

		if(sx + x >= 240)
		{
			x  = 0;
			y += p->cy;

			if(sy + y + p->cy >= 160)
			{
				return;
			}
		}

		Mode3DrawSJISChr(p, sx + x, sy + y, chr, col);
		x += p->cx;
	}
}

void Mode3DrawChar(ST_FONT* font, s32 sx, s32 sy, u16 chr, u16 color) {
	u16* pScreen = (u16*)VRAM + (sy * 240) + sx;

    if(chr == 0x20) {
        return;
    }
	u8* bitmapData = font->pDat + (chr - 0x20) * font->cx;

	s32 x, y;
	for(y=0; y<font->cy; y++)
	{
		for(x=0; x<font->cx; x++)
		{
			if(bitmapData[y*font->imgCx + x] == 0x00)
			{
				continue;
			}

			pScreen[y*240 + x] = color;
		}
	}
}

void Mode3DrawString(ST_FONT* font, s32 sx, s32 sy, char* str, u16 color) {
	u16 chr;
	s32 i=0, x=0, y=0;

	for(;;)
	{
		chr = str[i++];

		if(chr == '\0')
		{
            return;
		}

		if(sx + x >= 240)
		{
			x  = 0;
			y += font->cx;

			if(sy + y + font->cy >= 160)
			{
				return;
			}
		}

		Mode3DrawChar(font, sx + x, sy + y, chr, color);
		x += font->cx;
	}
}