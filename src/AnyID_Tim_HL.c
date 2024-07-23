#include "AnyID_Tim_HL.h"

void Tim_Init(u32 period, u32 prescaler)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    TIM_TimeBaseStructure.TIM_Period = period;             
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler;         
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);   
}


void ConfigureTimeForRunTimeStats()
{

   Tim_Init(10 - 1, 72 -1);
}