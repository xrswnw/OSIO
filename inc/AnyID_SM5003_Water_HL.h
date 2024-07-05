#ifndef _ANYID_SM5003_WATER_HL_
#define _ANYID_SM5003_WATER_HL_

#include "AnyID_SM5003_SysCfg.h"

#define WATER_POSITION_LOW       0


#define WATER_PERIPH_POSITION_TOUCH			0x01
#define WATER_PERIPH_PROTECT_TOUCH			0x02


#define WATER_SAMPLE_TICK					5
typedef struct waterInfo{
	u8 tempState;
	u8 state;
	u8 flag;
	u32 tick;
}WATER_INFO;
extern WATER_INFO g_sWaterIngo;

extern const PORT_INF WATER_POSITION_COM;
#define Get_WaterPosition()     (WATER_POSITION_COM.Port->IDR & WATER_POSITION_COM.Pin)

extern const PORT_INF WATER_PROTECT_COM;
#define Get_WaterProtect()      (WATER_PROTECT_COM.Port->IDR & WATER_PROTECT_COM.Pin)

extern const PORT_INF WATER_PUMP_CTRL_COM;
#define Water_PumpOpen()       (WATER_PUMP_CTRL_COM.Port->BSRR = WATER_PUMP_CTRL_COM.Pin)
#define Water_PumpClose()      (WATER_PUMP_CTRL_COM.Port->BRR = WATER_PUMP_CTRL_COM.Pin)

void Water_Interface(void);
u8 Water_GetPeriphInfo();
#endif