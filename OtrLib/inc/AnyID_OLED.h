#ifndef _ANYID_OLED_
#define _ANYID_OLED_

#if defined(_ANYID_BCTRL_)
    #include "AnyID_BCTRL_OLED_HL.h"
#elif defined(_ANYID_R320_)
    #include "AnyID_R320_OLED_HL.h"
#elif defined(_ANYID_R321_)
    #include "AnyID_R321_OLED_HL.h"
#endif


extern const u8 g_aOLEDASCII[96 * 16];

//enable gray scale table
#define OLED_CMD_EN_GRAY_TABLE              0x00
#define OLED_CMD_SET_COL_ADDR               0x15
#define OLED_CMD_SET_ROW_ADDR               0x75
#define OLED_CMD_SET_WRAM                   0x5C
#define OLED_CMD_SET_RRAM                   0x5D
#define OLED_CMD_SET_REMAP_MODE             0xA0
#define OLED_CMD_SET_DSTART_LINE            0xA1
#define OLED_CMD_SET_DOFFSET                0xA2
#define OLED_CMD_SET_DIS_MODE               0xA4
#define OLED_CMD_SET_PART_DIS               0xA8

#define OLED_CMD_SEL_FUNCTION               0xAB
#define OLED_CMD_ON_SLEEP                   0xAE
#define OLED_CMD_OFF_SLEEP                  0xAF
#define OLED_CMD_SET_PHASE1_2               0xB1
#define OLED_CMD_SET_CLK_DIV                0xB3
#define OLED_CMD_SET_ENHANCE_A              0xB4
#define OLED_CMD_SET_GPIO                   0xB5
#define OLED_CMD_SET_SEC_PRECHARGE          0xB6
#define OLED_CMD_SET_GRAY_TABLE             0xB8
#define OLED_CMD_SEL_LINEAR_GRAY_TABLE      0xB9
#define OLED_CMD_SET_PRECHARGE_VOL          0xBB
#define OLED_CMD_SET_VCOMH                  0xBE
#define OLED_CMD_SET_CONTRAST_CUR           0xC1
#define OLED_CMD_SET_MASTER_CUR             0xC7
#define OLED_CMD_SET_MUX_RATIO              0xCA
#define OLED_CMD_SET_ENHANCE_B              0xD1
#define OLED_CMD_LOK_CMD                    0xFD

#define OLED_COL_MAX                        0x3F
#define OLED_ROW_MAX                        0x3F
#define OLED_COL_OFFSET                     0x1C
#define OLED_BRIGHTNESS	                    0x0F

void OLED_SetColAdd(u8 start, u8 end);



void OLED_SetRowAdd(u8 start, u8 end);
void OLED_SetWriteRAM(void);
void OLED_SetReadRAM(void);

#define OLED_REMAP_ADDR_VER_INC             0x01
#define OLED_REMAP_ENABLE_COL               0x02
#define OLED_REMAP_ENABLE_NIBBLE            0x04
#define OLED_REMAP_SCAN_N_2_1               0x10
#define OLED_REMAP_ENABLE_COM_SPLIT_OLD     0x20
#define OLED_REMAP_ENABLE_DUAL_COM          0x11
#define OLED_REMAP_DISABLE_DUAL_COM         0x01
void OLED_SetRemapFormat(u8 fmt);


#define OLED_LINE_MAX                       64
void OLED_SetStartLine(u8 start);

#define OLED_OFFSET_MAX                     OLED_LINE_MAX
void OLED_SetDisplayOffset(u8 offset);


#define OLED_DIS_OFF                        0x00
#define OLED_DIS_ON                         0x01
#define OLED_DIS_NORMAL                     0x02
#define OLED_DIS_INVERSE                    0x03
void OLED_SetDisplayMode(u8 mode);


#define OLED_PART_DIS_ENABLE                0x00
#define OLED_PART_DIS_DISABLE               0x01
void OLED_SetPartialDisplay(u8 bEnalbe, u8 start, u8 end);

#define OLED_VDD_EXTERNAL                   0x00
#define OLED_VDD_INTERNAL                   0x01
void OLED_SetFunctionSelection(u8 fun);

#define OLED_SLEEP_ON                       0x00
#define OLED_SLEEP_OFF                      0x01
void OLED_SetSleep(u8 on);
#define OLED_SetDisplayOn()                 OLED_SetSleep(OLED_SLEEP_OFF)
#define OLED_SetDisplayOff()                OLED_SetSleep(OLED_SLEEP_ON)

void OLED_SetPhaseLength(u8 len);

void OLED_SetDisplayClock(u8 div);

#define OLED_VSL_EXTERNAL                   0xA0
#define OLED_VSL_INTERNAL                   0xA2
#define OLED_GRAY_NORMAL                    0xB5
#define OLED_GRAY_LOW                       0xFD
void OLED_SetDisplayEnhancementA(u8 vsl, u8 grayMode);

#define OLED_GPIO0_DISIN_HIZ                 0x00
#define OLED_GPIO0_ENIN_HIZ                  0x01
#define OLED_GPIO0_OUT_LOW                   0x02
#define OLED_GPIO0_OUT_HIGH                  0x03
#define OLED_GPIO1_DISIN_HIZ                 0x00
#define OLED_GPIO1_ENIN_HIZ                  0x04
#define OLED_GPIO1_OUT_LOW                   0x08
#define OLED_GPIO1_OUT_HIGH                  0x0C

void OLED_SetGPIO(u8 mode);


void OLED_SetPrechargePeriod(u8 period);
void OLED_SetPrechargeVoltage(u8 vol);
void OLED_SetVCOMH(u8 vol);
void OLED_SetContrastCurrent(u8 cur);
void OLED_SetMasterCurrent(u8 cur);

#define OLED_MUX_RATIO_MAX                  OLED_ROW_MAX
void OLED_SetMultiplexRatio(u8 ratio);

#define OLED_ENHANCEB_RESERVED              0x82
#define OLED_ENHANCEB_NORMAL                0xA2
void OLED_SetDisplayEnhancementB(u8 bNormal);

#define OLED_LOCK_CMD                       0x16
#define OLED_UNLOCK_CMD                     0x12
void OLED_SetCommandLock(u8 lock);

void OLED_SetGrayScaleTable(void);

void OLED_FillRAM(u16 x, u16 y, u8 data);
void OLED_Init(void);

#if OLED_CONTROL_SSD1306
#define OLED_X_PIX_MAX                      128     //每行256个Pix
#define OLED_Y_PIX_MAX                      32      //每列64个Pix
#else
#define OLED_X_PIX_MAX                      256     //每行256个Pix
#define OLED_Y_PIX_MAX                      64      //每列64个Pix
#endif
#define OLED_PIX_2_BIT                      4       //4Bit控制1个Pix

#define OLED_CHAR_MODE_UTF8                 0x00
#define OLED_CHAR_MODE_UTF16                0x01
//ASC码用12*32
//汉字用24*32
//子模设置:阴码、顺向、逐行和黑体
#define OLED_UTF8_W                         8      //12*32
#define OLED_UTF8_H                         16
#define OLED_UTF16_W                        24      //24*32
#define OLED_UTF16_H                        32

#define OLED_UTF8_ROW_BYTE                  1       //每行12个Pix，需要2个字节
#define OLED_UTF16_ROW_BYTE                 3       //每行24个Pix，需要3个字节
#define OLED_UTF8_BUF_LEN                   (OLED_UTF8_ROW_BYTE * OLED_UTF8_H)
#define OLED_UTF16_BUF_LEN                  (OLED_UTF16_ROW_BYTE * OLED_UTF16_H)



#define OLED_GetStartCol(StartPix)          ((StartPix) >> 2)       //每一个COL占用4个Pix
#define OLED_GetUTF8FontEndCol(StartPix)    ((StartPix) + 1)        //12Pix宽的字体占用3个COL，所以COL = X~X+2，占用3个COL
#define OLED_GetUTF16FontEndCol(StartPix)   ((StartPix) + 5)        //24Pix宽的字体占用6个COL，所以COL = X~X+5，占用6个COL


void OLED_DrawFont(u16 x, u16 y, u8 mode, const u8 *pFont);
void OLED_DrawString(u16 x, u16 y, u8 mode, u8 *pStr, u8 len);

#define OLED_ClearScreen()                  OLED_FillRAM(OLED_X_PIX_MAX, OLED_Y_PIX_MAX, 0x00)

#endif

