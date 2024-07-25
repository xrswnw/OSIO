#include "AnyID_LCD_HL.h"



#if (defined(_ANYID_STM32_BCTRL_) | defined(_ANYID_STM32_BBOOT_))
    #if (defined(LCD_CONTROL_ST7565))
        const PORT_INF LCD_CS = {GPIOB, GPIO_Pin_5};
        const PORT_INF LCD_RESET = {GPIOB, GPIO_Pin_6};
        const PORT_INF LCD_A0 = {GPIOB, GPIO_Pin_7};
        const PORT_INF LCD_SCL = {GPIOB, GPIO_Pin_8};
        const PORT_INF LCD_SI = {GPIOB, GPIO_Pin_9};
        const PORT_INF LCD_BLA = {GPIOD, GPIO_Pin_2};
    #endif
#endif


	
void LCD_InitInterface(void)
{

#if (defined(_ANYID_STM32_))

    GPIO_InitTypeDef GPIO_InitStructure;

    #if (defined(LCD_CONTROL_ST7565))

        GPIO_InitStructure.GPIO_Pin = LCD_SCL.Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(LCD_SCL.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = LCD_SI.Pin;
        GPIO_Init(LCD_SI.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = LCD_CS.Pin;
        GPIO_Init(LCD_CS.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = LCD_RESET.Pin;
        GPIO_Init(LCD_RESET.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = LCD_A0.Pin;
        GPIO_Init(LCD_A0.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = LCD_BLA.Pin;
        GPIO_Init(LCD_BLA.Port, &GPIO_InitStructure);
    #endif
#endif
		
	LCD_BLA_OFF();
}


void LCD_Delay(u32 delay)      //72M时，最快指令13ns
{
    while(delay--);
}


#if (defined(LCD_CONTROL_ST7565))

void LCD_WriteByte(u8 data, u8 type)
{
    u8 i = 0;

    LCD_CS_L();

    if(type == LCD_WTYPE_CMD)
    {
        LCD_A0_CMD();
    }
    else
    {
        LCD_A0_DAT();
    }

    for(i = 0; i < 8; i++)
    {
        LCD_SCL_L();
        if(data & 0x80)
        {
            LCD_SI_H();
        }
        else
        {
            LCD_SI_L();
        }
        LCD_SCL_H();
        data <<= 1;
    }
    LCD_CS_H();
}


#endif


