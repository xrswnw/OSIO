#ifndef _ANYID_VS10XX_HL_
#define _ANYID_VS10XX_HL_

#include "AnyID_config.h"


#if defined(_ANYID_STM32_)

    extern const PORT_INF               VS10XX_DREQ_PORT;
    extern const PORT_INF               VS10XX_XCCS_PORT;
    extern const PORT_INF               VS10XX_RST_PORT;
    extern const PORT_INF               VS10XX_XDCS_PORT;
    extern const PORT_INF               VS10XX_SPI_PORT;

    #define VS10XX_PutInReset()         GPIO_ResetBits(VS10XX_RST_PORT.Port, VS10XX_RST_PORT.Pin)
    #define VS10XX_ReleaseFromReset()   GPIO_SetBits(VS10XX_RST_PORT.Port, VS10XX_RST_PORT.Pin)

    #define VS10XX_SelectControl()      GPIO_ResetBits(VS10XX_XCCS_PORT.Port, VS10XX_XCCS_PORT.Pin)
    #define VS10XX_DeselectControl()    GPIO_SetBits(VS10XX_XCCS_PORT.Port, VS10XX_XCCS_PORT.Pin)

    #define VS10XX_SelectData()         GPIO_ResetBits(VS10XX_XDCS_PORT.Port, VS10XX_XDCS_PORT.Pin)
    #define VS10XX_DeselectData()       GPIO_SetBits(VS10XX_XDCS_PORT.Port, VS10XX_XDCS_PORT.Pin)

    #define VS10XX_DREQ                 GPIO_ReadInputDataBit(VS10XX_DREQ_PORT.Port, VS10XX_DREQ_PORT.Pin)
#endif


#if defined(_ANYID_STM32_MTD_)
    #define VS10XX_SPI SPI2
#elif defined(_ANYID_STM32_BCTRL_)
    #define VS10XX_SPI SPI2


    extern const PORT_INF               VS10XX_PA_PORT;
    #define VS10XX_PA_On()               GPIO_ResetBits(VS10XX_PA_PORT.Port, VS10XX_PA_PORT.Pin)
    #define VS10XX_PA_Off()              GPIO_SetBits(VS10XX_PA_PORT.Port, VS10XX_PA_PORT.Pin)
#endif

void VS10XX_InitInterface(void);
void VS10XX_InitSPI(u8 speed);

u8 VS10XX_WriteByte(u8 byte);

#endif
