#ifndef _ANYID_CET632_I18000P3M3_
#define _ANYID_CET632_I18000P3M3_

#include "AnyID_RC663.h"


#define I18000P3M3_CMD_REQ_RN          0xC1    // ReqRn command code
#define I18000P3M3_CMD_READ            0xC2    // Read command code
#define I18000P3M3_CMD_WRITE           0xC3    // Write command code
#define I18000P3M3_CMD_KILL            0xC4    // Kill command code
#define I18000P3M3_CMD_LOCK            0xC5    // Lock command code
#define I18000P3M3_CMD_ACCESS          0xC6    // Access command code
#define I18000P3M3_CMD_BLOCKWRITE      0xC7    // BlockWrite command code
#define I18000P3M3_CMD_BLOCKERASE      0xC8    // BlockErase command code
#define I18000P3M3_CMD_BLOCKPERMALOCK  0xC9    // BlockPermaLock command code

#define I18000P3M3_MEMBANCK_RSV     0x00
#define I18000P3M3_MEMBANCK_EPC     0x01
#define I18000P3M3_MEMBANCK_TID     0x02
#define I18000P3M3_MEMBANCK_USE     0x03
#define I18000P3M3_MEMBANCK_MSK     0x03


#define I18000P3M3_MEMBANCK_WPLMSK  0x03


u8 I18000p3m3_Init(void);
u8 I18000p3m3_AckReq(void);
u8 I18000p3m3_Read(u8 memBank, u8 *pWordPtr, u8 wordPtrLength, u8 wordCount);

#endif
