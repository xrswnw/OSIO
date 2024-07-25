#ifndef _ANYID_WDG_HL_
#define _ANYID_WDG_HL_

#include "AnyID_HLR600_Config.h"


void WDG_InitIWDG(void);
BOOL WDG_IsIWDGReset(void);

#ifdef _ANYID_STM32_
#define WDG_FeedIWDog()	IWDG_ReloadCounter()
#endif

#endif

