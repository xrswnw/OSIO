#ifndef _ANYID_RC522_HL_
#define _ANYID_RC522_HL_

#include "AnyID_Config.h"

#if (defined(_ANYID_PIC18_HSR100_))
    #define RC522_SPI_SPEED         4
    
    #define RC522_CS_TRIS          TRISCbits.TRISC2
    #define RC522_CS               LATCbits.LATC2
    #define RC522_RST_TRIS         TRISCbits.TRISC1
    #define RC522_RST              LATCbits.LATC1

    #define RC522_CS_High()         (RC522_CS = 1)
    #define RC522_CS_Low()          (RC522_CS = 0)

    #define RC522_Open()            (RC522_RST = 1)
    #define RC522_Close()           (RC522_RST = 0)


    #define RC522_RST_DELAY         160
    #define RC522_OSC_DELAY         16000
    #define RC522_ANTENNA_DELAY     64000

#endif

void RC522_InitInterface(void);
u8 RC522_WriteByte(u8 byte);

#endif