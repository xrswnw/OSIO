#ifndef _ANYID_MSD_HL_
#define _ANYID_MSD_HL_

#include "AnyID_config.h"

#define DUMMY               0xFF

#ifdef _ANYID_STM32_MTD_
extern const PORT_INF       MSD_CS;
#define MSD_CS_LOW()        {GPIO_ResetBits(MSD_CS.Port, MSD_CS.Pin); MSD_WriteByte(DUMMY);}
#define MSD_CS_HIGH()       {GPIO_SetBits(MSD_CS.Port, MSD_CS.Pin); MSD_WriteByte(DUMMY);}

extern const PORT_INF       MSD_Power;
#define MSD_POWER_ON()      GPIO_SetBits(MSD_Power.Port, MSD_Power.Pin)
#define MSD_POWER_OFF()     GPIO_ResetBits(MSD_Power.Port, MSD_Power.Pin)
#endif

void MSD_WriteByte(u8 byte);
u8 MSD_ReadByte(void);

void MSD_InterfaceInit(void);
void MSD_LowPower(LPMode lp);

void MSD_SPILowSpeed(void);
void MSD_SPIHighSpeed(void);

#endif
