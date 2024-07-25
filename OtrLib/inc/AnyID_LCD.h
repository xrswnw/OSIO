#ifndef _ANYID_LCD_
#define _ANYID_LCD_

#if defined(_ANYID_BCTRL_)
    #include "AnyID_BCTRL_LCD_HL.h"
#elif defined(_ANYID_INFCOLLECTION_)
    #include "AnyID_InfCollection_LCD_HL.h"
#elif defined(_ANYID_GENERALDEVICE_)
    #include "AnyID_GeneralDevice_LCD_HL.h"
#elif defined(_ANYID_PDA_)
    #include "AnyID_PDA_LCD_HL.h"
#endif


#define LCD_COLOR       2

typedef struct point{
    u32 x;
    u32 y;
}LCD_POINT;

typedef struct size{
    u32 w;
    u32 h;
}LCD_SIZE;


typedef struct rect{
    u32 x0;
    u32 y0;

    u32 x1;
    u32 y1;
}LCD_RECT;


typedef struct line{
    u32 x0;
    u32 y0;

    u32 x1;
    u32 y1;
}LCD_LINE;

typedef struct circle{
    u32 x0;
    u32 y0;

    u32 r;
}LCD_CIRCLE;

typedef struct Lcd_Font
{
    LCD_SIZE fontSize;
#if LCD_COLOR > 2
    u32 nTextBkColor;
    u32 nTextColor;
#endif
}LCD_FONT;

extern LCD_FONT g_stFont;
void LCD_SetFontSize(u8 w, u8 h);




void LCD_Init(void);
void LCD_SetStartPos(u8 x, u8 y);
void LCD_ClearScreen(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

void LCD_DrawPix(u8 x, u8 y, BOOL bClear);
void LCD_DrawLine(u8 x0, u8 y0, u8 x1, u8 y1, BOOL bClear);
void LCD_DrawRect(u8 x0, u8 y0, u8 x1, u8 y1, BOOL bFill, BOOL bClear);
void LCD_DrawCircle(u8 x0, u8 y0, u8 r, BOOL bFill, BOOL bClear);
void LCD_DrawFont(u8 x0, u8 y0, u8 *pFont);
void LCD_DrawString(u8 x0, u8 y0, u8 maxNum, u8 num, u8 *pString, u8 *pFont);
//#define LCD_ASC_OFFSET      32




extern const unsigned char g_aLCDAscII[128][16];

#endif
