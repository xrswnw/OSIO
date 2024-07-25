#include "AnyID_Key_HL.h"

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MPD_))

const PORT_INF KEY_CFG_PORT = {GPIOC, GPIO_Pin_13};
const PORT_INF KEY_CLR_PORT = {GPIOC, GPIO_Pin_14};


#elif (defined(_ANYID_STM32_MCD_))

const PORT_INF KEY_LG_PORT = {GPIOC, GPIO_Pin_13};
const PORT_INF KEY_DEV_PORT = {GPIOC, GPIO_Pin_14};

#elif (defined(_ANYID_STM32_MTD_))

const PORT_INF KEY_DOWN_PORT = {GPIOC, GPIO_Pin_12};
const PORT_INF KEY_UP_PORT = {GPIOC, GPIO_Pin_13};
const PORT_INF KEY_REPEAT_PORT = {GPIOC, GPIO_Pin_14};
const PORT_INF KEY_MOD_PORT = {GPIOC, GPIO_Pin_15};

#endif

u8 g_nKeyValue = KEY_NO_VAL;

void Key_Init(void)
{
#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MPD_))

    GPIO_InitTypeDef  GPIO_InitStructure;  //所有按键IO设置为：GPIO_Mode_IN_FLOATING

    GPIO_InitStructure.GPIO_Pin = KEY_CFG_PORT.Pin | KEY_CLR_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);

#elif (defined(_ANYID_STM32_MCD_))

    GPIO_InitTypeDef  GPIO_InitStructure;  //所有按键IO设置为：GPIO_Mode_IN_FLOATING

    GPIO_InitStructure.GPIO_Pin = KEY_LG_PORT.Pin | KEY_DEV_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);

#elif (defined(_ANYID_STM32_MTD_))

    GPIO_InitTypeDef  GPIO_InitStructure;  //所有按键IO设置为：GPIO_Mode_IN_FLOATING

    GPIO_InitStructure.GPIO_Pin = KEY_DOWN_PORT.Pin | KEY_UP_PORT.Pin | KEY_REPEAT_PORT.Pin | KEY_MOD_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);

#endif
}

