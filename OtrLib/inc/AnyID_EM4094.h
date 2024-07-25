#ifndef _ANYID_EM4094_
#define _ANYID_EM4094_

#if defined(_ANYID_HMR100_)
    #include "AnyID_HMR100_EM4094_HL.h"
#elif defined(_ANYID_M330_)
    #include "AnyID_M330_EM4094_HL.h"
#elif defined(_ANYID_R320_)
    #include "AnyID_R320_EM4094_HL.h"
#elif defined(_ANYID_HLR600_)
    #include "AnyID_HLR600_EM4094_HL.h"
#elif defined(_ANYID_ODF_)
    #include "AnyID_ODF_EM4094_HL.h"
#elif defined(_ANYID_D320_)
    #include "AnyID_D320_EM4094_HL.h"
#elif defined(_ANYID_T851_)
    #include "AnyID_T851_EM4094_HL.h"
#elif defined(_ANYID_D320_)
    #include "AnyID_D320_EM4094_HL.h"
#elif defined(_ANYID_M343_)
    #include "AnyID_M343_EM4094_HL.h"
#endif

void EM4094_Init(void);

#endif