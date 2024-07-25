#ifndef _ANYID_RC663_ISO14443B_
#define _ANYID_RC663_ISO14443B_

#include "AnyID_RC663.h"

#define ISO14443B_OP_TIME           20000
#define ISO14443B_UID_LEN           8

u8 ISO14443B_Init(void);
u8 ISO14443B_Dtu(void);
u8 ISO14443B_GetUid(u8 *pUid);



#endif

