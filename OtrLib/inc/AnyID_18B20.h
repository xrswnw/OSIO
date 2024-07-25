#ifndef _ANYID_18B20_
#define _ANYID_18B20_

#if defined(_ANYID_M340_)
    #include "AnyID_M340_18B20_HL.h"
#elif defined(_ANYID_M345_)
    #include "AnyID_M345_18B20_HL.h"
#elif defined(_ANYID_R542A_)
    #include "AnyID_R542_18B20_HL.h"
#elif defined(_ANYID_R5422_)
    #include "AnyID_R542_18B20_HL.h"
#elif defined(_ANYID_D543FNX_)
    #include "AnyID_R542_18B20_HL.h"
#endif

BOOL DS18B20_Reset(void);
void DS18B20_WriteByte(u8 t);
u8 DS18B20_ReadByte(void);
void DS18B20_Init(u16 alarmT, u8 config);
u16 DS18B20_ReadTempr(void);
void DS18B20_StartConvert(void);


#define DS18B20_CMD_COVN            0x44
#define DS18B20_CMD_READ            0xBE
#define DS18B20_CMD_WRITE           0x4E
#define DS18B20_CMD_COPY            0x48
#define DS18B20_CMD_RECALLE2        0xB8
#define DS18B20_CMD_RPOWER          0xB4

#define DS18B20_CMD_SEARCH_ALARM    0xEC
#define DS18B20_CMD_SKIP_ROM        0xCC
#define DS18B20_CMD_MATCH_ROM       0x55
#define DS18B20_CMD_READ_ROM        0x33
#define DS18B20_CMD_SEARCH_ROM      0xF0

#define DS18B20_CFG_ADBIT9          0x00
#define DS18B20_CFG_ADBIT10         0x20
#define DS18B20_CFG_ADBIT11         0x40
#define DS18B20_CFG_ADBIT12         0x60

#define DS18B20_T_MAX               0x07D0  //125…„ œ∂»


#endif

