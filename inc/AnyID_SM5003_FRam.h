#ifndef _ANYID_SM5003_FRAM_
#define _ANYID_SM5003_FRAM_

#include "AnyID_FRam.h"

#define FRAM_BOOT_APP_OK                    0x5555
#define FRAM_BOOT_APP_FAIL                  0xAAAA

typedef struct framBootDevicePar{
    u16 appState;
    u16 rfu;
    u16 addr;
    u16 crc;
}FRAM_BOOTPARAMS;
extern FRAM_BOOTPARAMS g_sFramBootParamenter;

#define FRAME_MEM_SIZE                      (8 << 10)    //8k
#define FRAME_INFO_ADDR                     0x0000
#define FRAME_INFO_BACKUP_ADDR              0x0300


#define FRAME_BOOT_INFO_BACKUP_ADDR         (FRAME_MEM_SIZE - 512) 
#define FRAME_BOOT_INFO_ADDR                (FRAME_MEM_SIZE - sizeof(FRAM_BOOTPARAMS))      //固件升级从480字节开始写，共512字节


void Fram_ReadBootParamenter(void);
BOOL Fram_WriteBootParamenter(void);

#endif