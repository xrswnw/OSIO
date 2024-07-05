#ifndef _ANYID_SM5003_SYSTICK_HL_
#define _ANYID_SM5003_SYSTICK_HL_


#include "AnyID_SM5003_Config.h"


extern vu32 g_nSysTick;
extern vs32 g_nSysDelay;

#define STICK_1MS                           72000                    //72M��ϵͳʱ��


void STick_InitSysTick(void);

#define STick_StartSysTick()                SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk
#define STick_StopSysTick()                 SysTick->VAL = 0;  SysTick->CTRL  &= (~SysTick_CTRL_ENABLE_Msk)

#define STick_SetDelayTime(delay)           (g_nSysDelay = delay) //��һ��TickΪ��λ
#define STick_IsDelayTimeOver()             (g_nSysDelay <= 0)



#define STICK_TIME_MS                       5

#define STICK_1S                            (1000 / STICK_TIME_MS)

#define STICK_5MS_COUNT                     (STICK_1MS * STICK_TIME_MS) 

#endif
