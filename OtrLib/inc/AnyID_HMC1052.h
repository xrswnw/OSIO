#ifndef _ANYID_HMC1052_
#define _ANYID_HMC1052_

#if defined(_ANYID_WVDU_)
    #include "AnyID_WVDU_HMC1052_HL.h"
#endif

extern u32 g_nADValue_Y;
extern u16 g_nThreld_Y;
extern u16 g_nVref_Y;

extern u32 g_nADValue_X;
extern u16 g_nThreld_X;
extern u16 g_nVref_X;

BOOL HMC1052_DetectCar(u16 refValue, u16 threldValue, u16 curValue);



#endif
