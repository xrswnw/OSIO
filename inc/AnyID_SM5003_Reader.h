#ifndef _ANYID_SM5003_READER_
#define _ANYID_SM5003_READER_

#include "AnyID_SM5003_WDG_HL.h"
#include "AnyID_SM5003_SysTick_HL.h"
#include "AnyID_Stdlib.h"

extern void Sys_Delayms(u32 n);

#define READER_DEVICE_PASSWORD_START                (0x0800F800)
#define READER_DEVICE_PASSWORD_BACK_START           (0x0800F000)

//#define SAVE_MATCH_TAG_ADDR     (0x0800F800)
#define SAVE_MATCH_TAG_ADDR     (0x0800EF00)


#define READER_WRITE_KAY_CMD            0x0022C21D
#define READER_READ_KAY_CMD             0x0022C31D
#define READER_AUTH_CMD                 0x0022C71D
#define READER_LOCK_KEY_CMD             0x0022C41D
#define READER_GET_ROM_CMD              0x0022B21D
#define READER_CHECK_CMD                0x0022B31D
#define READER_SET_PERSON_CMD           0x0022B41D
#define READER_SET_CFG_CMD              0x0022D01D

#define READER_NO_WRITE_KAYA            0x00
#define READER_WRITE_KAYA               0x10
#define READER_WRITE_KAYB               0x11
#define READER_WRITE_KAYC               0x12
#define READER_WRITE_CRC                0x13
#define READER_PERSON_FLAG              0x0F
#define READER_TAG_FAST_INIT            0x0F
#define READER_CFG_READ_MODE            0x00
#define READER_CFG_WRITE_MODE           0x10
#define READER_SET_CFG_RDONLY           0x80

#define READER_LOCK_KEY1                0x0F
#define READER_LOCK_KEY2                0x1D
#define READER_KEY_LOCKED               0xFF
#define READER_WRITE_KAY_LEN            16
#define READER_KAY_LEN                  4
#define READER_SHORT_KAY_LEN            4
#define READER_LONG_KAY_LEN             6
#define READER_WRITE_KAY_TIME           10000
#define READER_TAG_NORMAL               0x00
#define READER_TAG_SP                   0xFF

#define READER_NORMAL_TAG               0x01
#define READER_SP_TAG                   0x02
#define READER_CHK_YES                  0xFF
#define TAG_BLOCK_NUM                   01
#define TAG_BLOCK_0                     00
#define TAG_BLOCK_1                     01
#define TAG_BLOCK_2                     02
#define READER_VERSION_SIZE      50
#define READER_OP_UID_NUM                   1
#define MH663_DEV_NUM                       1
#define READER_FRAME_BROADCAST_ADDR     0xFFFF
#define READER_GROUP_BROADCAST_ADDR     0xFF
#define READER_IMOP_REPEAT_NUM          2

#define RC663_USER_ERROR               0xFF

#define READER_AUTH_MASK            0xFF

    
#define READER_LED_DELAY_100ms  50
#define READER_LED_DELAY_1S     200
#define READER_LED_DELAY_2S     400

#define a_SetSyeLedParameter(OnTime, OffTime)  do{ \
                                                   *pOntime = (OnTime); \
                                                   *pOfftime = (OffTime); \
                                               }while(0)

#define READER_MODE_NOMATCH_TAG     0x00
#define READER_MODE_MATCH_TAG       0x01
#define READER_MODE_TAGILL          0x02
#define READER_MODE_NOTAG           0x03
#define READER_MODE_TAGLEGAL        0x04

#define READER_STAT_OK              0x00
#define READER_STAT_NOTAG           0x01
#define READER_STAT_TAGILL          0x02
#define READER_STAT_WAIT            0x04
#define READER_STAT_WRITE_FLASH     0x08
#define READER_STAT_PIN_HIGH        0x10
#define READER_STAT_PIN_LOW         0x20

#define READER_MATCH_DELAY_TICK     1000


void Reader_Init(void);
void Reader_ReadDeviceConfig(void);
void Reader_WriteDeviceConfig(void);
void Reader_Delayms(u32 n);
void Reader_CreateFrame(u8 *pBuf, u8 *pUid, u8 flag, u32 cmd);
void Reader_Ayth(void);
void Device_ReadFlash(void);
void Reader_WriteSpUid();

BOOL Reader_ReadFlash(void);
BOOL Reader_WriteFlash(void);
//void Reader_WriteUid(u8 uid[ISO15693_SIZE_UID]);
BOOL Reader_RC663Init(void);
u8 Reader_AutoUid(void);
u8 Reader_WriteAlgoKey(u8 *pBuf, u8 flag, u32 timeout);
u8 Reader_ReadAlgoKey(u8 *pRxFrame, u8 flag, u32 timeout);
u8 Reader_CheckAlgoKey(u8 *pWrKeyA, u8 *pWrKeyB, u8 *pWrKeyC);
void Reader_GetKeys();
u8 Reader_WriteKeys();
u8 Reader_ScanTag();
u8 Reader_ActAuth();
u8 Reader_LockKeys();
BOOL Reader_TagIsInit();
u8 Reader_EnableNDEFMirror(u8 *pArg);
u8 Reader_AuthTag();
u8 Reader_CheckTag();
BOOL Reader_ModeDeal(u8 state, u8 *pRes, u16 *pOntime, u16 *pOfftime);

//u8 Reader_WriteURL(u8 *pUrl, u8 len);
//u8 Reader_PasswdAuth();

//u8 Reader_ChkTag( u8 *pUid ,u8 blockData);
//u8 Reader_ChkUid( u8 *pUid);
//u8 Reader_CheckTag();

//u8 Reader_WriteKeyBlock();
u8 Reader_LockPerson();

/*
u8 Reader_ChkFlashKey();
u8 Reader_ChkFlashSpKey();
BOOL Device_ReadFlashParamenter(u32 addr) ;
BOOL Device_WriteFlashParamenter(u32 addr);
BOOL Reader_EraseFlash(u32 addr);
BOOL BL_WriteImagePage(u32 addr, u8 *pBuffer, u32 size);
BOOL BL_ReadImagePage(u32 addr, u8 *pBuffer, u32 size);
*/
#endif
