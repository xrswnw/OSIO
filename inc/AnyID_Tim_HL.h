#ifndef _ANYID_TIM_HL_
#define _ANYID_TIM_HL_

#include "stm32f10x_tim.h"



extern uint32_t FreeRTOSRunTimeTicks ;
void Tim_Init(u32 period, u32 prescaler);
void ConfigureTimeForRunTimeStats();
#endif

