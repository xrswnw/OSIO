#ifndef _ANYID_SPIFLASH_
#define _ANYID_SPIFLASH_

#include "AnyID_R327_SPIFLASH_HL.h"


#define DATA_ADDR_SIZE          4
#define DATA_CRC_SIZE           1

#define SPIFLASH_USERDATA_BLOCK_SIZE  64



//前两个sector分别换分成4K的小段

#define SPIFLASH_SIZE               ((u32)(1 << 24))
#define SPIFLASH_PAGE_SIZE          (1 << 8)
#define SPIFLASH_PAGE_NUM           ((u32)(1 << 16))
#define SPIFLASH_SECTOR_SIZE        ((u32)(1 << 16))
#define SPIFLASH_SECTOR_NUM         ((1 << 8))

//子段定义，子段一般保存参数
//它比较小，擦除速度快
#define SPIFLASH_SUB_SECTOR_SIZE    0x001000
#define SPIFLASH_SUB_SECTOR_START   0x000000
#define SPIFLASH_SUB_SECTOR_END     0x020000


#define SPIFLASH_DUMMY              0xFF
#define SPIFLASH_CMD_READ           0x03
#define SPIFLASH_CMD_RDID           0x9F
#define SPIFLASH_CMD_READ_ID        0x90
#define SPIFLASH_CMD_WREN           0x06
#define SPIFLASH_CMD_SE             0xD8
#define SPIFLASH_CMD_PP             0x02
#define SPIFLASH_CMD_RDSR           0x05
#define SPIFLASH_CMD_WRR            0x01
#define SPIFLASH_CMD_CLSR           0x30
#define SPIFLASH_CMD_RCR            0x35
#define SPIFLASH_CMD_P4E            0x20


#define SPIFLASH_STATE_BUSY     0x01

void SPIFLASH_WaitNoBusy(void);
void SPIFLASH_Read(u32 address, u8 *pBuffer, u32 len);
void SPIFLASH_ReadID(u8 *pSPIFLASHID);
void SPIFLASH_ReadDeviceID(u8 *pDeviceID, u8 *pManufacturerID);
void SPIFLASH_WriteEnable(void);
void SPIFLASH_SectorErase(u32 address);
void SPIFLASH_Parameter4kSectorErase(u32 address);
void SPIFLASH_PageProgram(u32 address, u8 *pBuffer, u16 len);
u8 SPIFLASH_ReadStatusRegister(void);
u8 SPIFLASH_ReadConfigRegister(void);
void SPIFLASH_WriteStatusRegister(u8 reg);
void SPIFLASH_ClearStatusRegister(void);

u32 SPIFLASH_ReadData(u32 address, u32 len);
void SPIFLASH_WriteData(u32 address, u32 data, u32 len);
void SPIFLASH_WriteBuffer(u32 address, u8 *pBuffer, u32 len);


#endif
