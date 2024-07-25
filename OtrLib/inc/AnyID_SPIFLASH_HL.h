#ifndef _ANYID_SPIFLASH_HL_
#define _ANYID_SPIFLASH_HL_

#include "AnyID_R322_Config.h"

//#if defined(_ANYID_STM32_BCTRL_)

    extern const PORT_INF SPIFLASH_SPI_PORT;
    extern const PORT_INF SPIFLASH_CS;
    #define SPIFLASH_CS_Low()       GPIO_ResetBits(SPIFLASH_CS.Port, SPIFLASH_CS.Pin)
    #define SPIFLASH_CS_High()      GPIO_SetBits(SPIFLASH_CS.Port, SPIFLASH_CS.Pin)

    #define SPIFLASH_SPI        SPI2

//#endif

void SPIFLASH_InitInterface(void);
u8 SPIFLASH_WriteByte(u8 byte);

#endif

