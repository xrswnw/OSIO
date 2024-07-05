#ifndef _ANYID_SM5003_WDG_HL_
#define _ANYID_SM5003_WDG_HL_

#include "AnyID_SM5003_Config.h"


void WDG_InitIWDG(void);
BOOL WDG_IsIWDGReset(void);


#define WDG_FeedIWDog()	IWDG_ReloadCounter()


#endif

