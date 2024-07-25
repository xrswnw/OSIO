#ifndef _ANYID_LCD_HL_
#define _ANYID_LCD_HL_

#include "AnyID_Config.h"

#define LCD_CONTROL_ST7565              1


#if (defined(_ANYID_STM32_BCTRL_) | defined(_ANYID_STM32_BBOOT_))
    #if (defined(LCD_CONTROL_ST7565))
        extern const PORT_INF LCD_SCL;
        extern const PORT_INF LCD_SI;
        extern const PORT_INF LCD_CS;
        extern const PORT_INF LCD_RESET;
        extern const PORT_INF LCD_A0;
        extern const PORT_INF LCD_BLA;


        #define LCD_SCL_H()                 GPIO_SetBits(LCD_SCL.Port, LCD_SCL.Pin)
        #define LCD_SCL_L()                 GPIO_ResetBits(LCD_SCL.Port, LCD_SCL.Pin)

        #define LCD_SI_H()                  GPIO_SetBits(LCD_SI.Port, LCD_SI.Pin)
        #define LCD_SI_L()                  GPIO_ResetBits(LCD_SI.Port, LCD_SI.Pin)

        #define LCD_CS_H()                  GPIO_SetBits(LCD_CS.Port, LCD_CS.Pin)
        #define LCD_CS_L()                  GPIO_ResetBits(LCD_CS.Port, LCD_CS.Pin)

        #define LCD_RST_H()                 GPIO_SetBits(LCD_RESET.Port, LCD_RESET.Pin)
        #define LCD_RST_L()                 GPIO_ResetBits(LCD_RESET.Port, LCD_RESET.Pin)

        #define LCD_A0_DAT()                GPIO_SetBits(LCD_A0.Port, LCD_A0.Pin)
        #define LCD_A0_CMD()                GPIO_ResetBits(LCD_A0.Port, LCD_A0.Pin)

        #define LCD_BLA_ON()                GPIO_ResetBits(LCD_BLA.Port, LCD_BLA.Pin)
        #define LCD_BLA_OFF()               GPIO_SetBits(LCD_BLA.Port, LCD_BLA.Pin)

    #endif
#endif


#if (defined(LCD_CONTROL_ST7565))

    #define LCD_LINE_NUM                64
    #define LCD_COLUMN_NUM              128
    #define LCD_PAGE_NUM                8
    #define LCD_PIX_PRE_PAGE            8
    #define LCD_X_START                 4

    #define LCD_WTYPE_CMD               0
    #define LCD_WTYPE_DATA              1


    void LCD_WriteByte(u8 data, u8 type);
#endif

void LCD_Delay(u32 delay);
void LCD_InitInterface(void);






#endif
