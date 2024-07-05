#ifndef _ANYID_SM5003_FIRE_HL_
#define _ANYID_SM5003_FIRE_HL_

#include "AnyID_SM5003_SysCfg.h"

#define FIRE_EG_NUM    6
#define FIRE_EG_MASK   0x3F

#define FIRE_SWITCH_CLOSE        0x00
#define FIRE_SWITCH_OPEN         0xFF


typedef struct fireEgInfo{
	u8 state;
	u8 index;
}FIRE_EGINFO;

extern const PORT_INF EG_CTLx_COM;
extern const u16 g_aMapTal[FIRE_EG_NUM];
extern FIRE_EGINFO g_sFireEgInfo;

#define Fire_EgCtr(index, stat)		do{\
											if(stat == FIRE_SWITCH_OPEN)\
											{\
												EG_CTLx_COM.Port->BSRR = g_aMapTal[index];\
												g_sFireEgInfo.state |= (1 << index);\
											}\
											else \
											{\
												EG_CTLx_COM.Port->BRR = g_aMapTal[index];\
												g_sFireEgInfo.state &= ~(1 << index);\
											}\
										  }while(0)
void Fire_Interface(void);
void Fire_CtlEg(u8 ctlBits);
//void Fire_ClkDelay(void);
//void Fire_SerialToParallel(u8 serDat1, u8 serDat2);
//u16 Fire_ParallelToSerial(void);
void Fire_Delayms(u32 n);
void PWM_SetCompare1(u16 Compare);
void Fire_Init();
#endif