#include "AnyID_SysTick_HL.h"

vu32 g_nSysTick = 0;
vs32 g_nSysDelay = 0;

void STick_InitSysTick(void)
{
#ifdef _ANYID_STM32_

    SysTick_CounterCmd(SysTick_Counter_Disable);

    //Disable the SysTick Interrupt
    SysTick_ITConfig(DISABLE);

    //Configure HCLK clock as SysTick clock source  48M
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    //SysTick interrupt each 1000 Hz with HCLK equal
#if (defined(_ANYID_STM32_BCTRL_) | defined(_ANYID_STM32_BLDC_))
    SysTick_SetReload(STICK_1MS * 50);
#else
    SysTick_SetReload(STICK_1MS * 10);
#endif
    //Set SysTick Interrupt Priority
    NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 1, 0);

#elif _ANYID_PIC18_

    //Enable the timer
    T2CONbits.T2OUTPS0 = 1;
    T2CONbits.T2OUTPS1 = 1;
    T2CONbits.T2OUTPS2 = 1;
    T2CONbits.T2OUTPS3 = 1;
    T2CONbits.T2CKPS1 = 1;

    PR2 = STICK_5MS;				//fosc / (4 * 16 * 16 * STICK_5MS) = 200hz

#endif

}

void STick_StopSysTick(void)
{
#ifdef _ANYID_STM32_

    //Disable SysTick Counter
    SysTick_CounterCmd(SysTick_Counter_Disable);
    //Clear SysTick Counter
    SysTick_CounterCmd(SysTick_Counter_Clear);

#elif _ANYID_PIC18_

    T2CONbits.TMR2ON = 0;

#endif
}


#ifdef _ANYID_PIC18_
void STick_EnableInt(FunctionalState bEnable)
{
    if(bEnable)
    {
        PIE1bits.TMR2IE = 1;
        IPR1bits.TMR2IP = 0;
    }
    else
    {
        PIE1bits.TMR2IE = 0;
        IPR1bits.TMR2IP = 0;
    }
}
#endif

