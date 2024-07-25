#ifndef _AnyID_KEY_HL_
#define _AnyID_KEY_HL_

#include "AnyID_Config.h"

extern u8 g_nKeyValue;
#define KEY_NO_VAL                  0x00

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MPD_))

#define KEY_PORT                    GPIOC
#define KEY_MASK                    0x6000

#define KEY_CFG_VAL                 0x01
#define KEY_CLR_VAL                 0x02

#elif (defined(_ANYID_STM32_MCD_))

#define KEY_PORT                    GPIOC
#define KEY_MASK                    0x6000

#define KEY_LG_VAL                  0x01
#define KEY_MOD_VAL                 0x02

#elif (defined(_ANYID_STM32_MTD_))

const extern PORT_INF               KEY_REPEAT_PORT;
const extern PORT_INF               KEY_MOD_PORT;
const extern PORT_INF               KEY_DOWN_PORT;
const extern PORT_INF               KEY_UP_PORT;

#define KEY_PORT                    GPIOC
#define KEY_MASK                    0xF000

#define KEY_DOWN_VAL                0x10
#define KEY_UP_VAL                  0x20
#define KEY_RPT_VAL                 0x40
#define KEY_MOD_VAL                 0x80

#endif


void Key_Init(void);
void Key_GetKeyValue(u32 time);

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MPD_))
    #define Key_Read()	((u8)((KEY_MASK & (~GPIO_ReadInputData(KEY_PORT))) >> 13))
#elif (defined(_ANYID_STM32_MCD_))
    #define Key_Read()	((u8)((KEY_MASK & (~GPIO_ReadInputData(KEY_PORT))) >> 13))
#elif (defined(_ANYID_STM32_MTD_))
    #define Key_Read()	((u8)((KEY_MASK & (~GPIO_ReadInputData(KEY_PORT))) >> 8))
#endif

#endif
