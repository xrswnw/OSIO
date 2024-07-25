#include "AnyID_HMC1052.h"


u32 g_nADValue_Y = 0xffff;
u16 g_nThreld_Y = 0xffff;
u16 g_nVref_Y = 0xffff;

u32 g_nADValue_X = 0xffff;
u16 g_nThreld_X = 0xffff;
u16 g_nVref_X = 0xffff;


BOOL HMC1052_DetectCar(u16 refValue, u16 threldValue, u16 curValue)
{
    u8 b = FALSE;
    
    if(curValue > refValue)
    {
        curValue -= refValue;
    }
    else
    {
        curValue = refValue - curValue;
    }

    if(curValue > threldValue)
    {
        b = TRUE;
    }

    return b;
}

