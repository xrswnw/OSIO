#ifndef _ANYID_EEPROM_
#define _ANYID_EEPROM_

#if defined(_ANYID_PDA_)
    #include "AnyID_PDA_EEPROM_HL.h"
#endif

#define EEPROM_AT24C08	                    1

#define EEPROM_AT24C128	                    0

#if EEPROM_AT24C08
    #define EEPROM_PAGE_SIZE                16
    #define EEPROM_DEV_ADDR                 0xA0
    #define EEPROM_SIZE                     0x400
#endif

BOOL EEPROM_WriteByte(u16 addr, u8 data);
BOOL EEPROM_WritePage(u16 addr, u8 *pBuffer, u8 len);

BOOL EEPROM_ReadCurrenAddr(u8 *pData);
BOOL EEPROM_ReadRandomAddr(u16 addr, u8 *pData);
BOOL EEPROM_ReadSequential(u16 addr, u8 *pBuffer, u16 len);
#endif

