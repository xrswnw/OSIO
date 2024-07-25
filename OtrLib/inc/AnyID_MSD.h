#ifndef _ANYID_MSD_
#define _ANYID_MSD_

#include "AnyID_config.h"
#include "AnyID_MSD_HL.h"

#define BLOCK_SIZE                          512

#define MSD_START_DATA_SINGLE_BLOCK_READ    0xFE  /* Data token start byte, Start Single Block Read */
#define MSD_START_DATA_MULTIPLE_BLOCK_READ  0xFE  /* Data token start byte, Start Multiple Block Read */
#define MSD_START_DATA_SINGLE_BLOCK_WRITE   0xFE  /* Data token start byte, Start Single Block Write */
#define MSD_START_DATA_MULTIPLE_BLOCK_WRITE 0xFC  /* Data token start byte, Start Multiple Block Write */
#define MSD_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xFD  /* Data toke stop byte, Stop Multiple Block Write */

#define MSD_RESPONSE_NO_ERROR               0x00
#define MSD_IN_IDLE_STATE                   0x01
#define MSD_ERASE_RESET                     0x02
#define MSD_ILLEGAL_COMMAND                 0x04
#define MSD_COM_CRC_ERROR                   0x08
#define MSD_ERASE_SEQUENCE_ERROR            0x10
#define MSD_ADDRESS_ERROR                   0x20
#define MSD_PARAMETER_ERROR                 0x40
#define MSD_RESPONSE_FAILURE                0xFF

#define MSD_DATA_OK                         0x05
#define MSD_DATA_CRC_ERROR                  0x0B
#define MSD_DATA_WRITE_ERROR                0x0D
#define MSD_DATA_OTHER_ERROR                0xFF

/* Commands: CMDxx = CMD-number | 0x40 */
#define MSD_GO_IDLE_STATE                   0   /* CMD0=0x40 */
#define MSD_SEND_OP_COND                    1   /* CMD1=0x41 */
#define MSD_SEND_CSD                        9   /* CMD9=0x49 */
#define MSD_SEND_CID                        10  /* CMD10=0x4A */
#define MSD_STOP_TRANSMISSION               12  /* CMD12=0x4C */
#define MSD_SEND_STATUS                     13  /* CMD13=0x4D */
#define MSD_SET_BLOCKLEN                    16  /* CMD16=0x50 */
#define MSD_READ_SINGLE_BLOCK               17  /* CMD17=0x51 */
#define MSD_READ_MULTIPLE_BLOCK             18  /* CMD18=0x52 */
#define MSD_SET_BLOCK_COUNT                 23  /* CMD23=0x57 */
#define MSD_WRITE_BLOCK                     24  /* CMD24=0x58 */
#define MSD_WRITE_MULTIPLE_BLOCK            25  /* CMD25=0x59 */
#define MSD_PROGRAM_CSD                     27  /* CMD27=0x5B */
#define MSD_SET_WRITE_PROT                  28  /* CMD28=0x5C */
#define MSD_CLR_WRITE_PROT                  29  /* CMD29=0x5D */
#define MSD_SEND_WRITE_PROT                 30  /* CMD30=0x5E */
#define MSD_TAG_SECTOR_START                32  /* CMD32=0x60 */
#define MSD_TAG_SECTOR_END                  33  /* CMD33=0x61 */
#define MSD_UNTAG_SECTOR                    34  /* CMD34=0x62 */
#define MSD_TAG_ERASE_GROUP_START           35  /* CMD35=0x63 */
#define MSD_TAG_ERASE_GROUP_END             36  /* CMD36=0x64 */
#define MSD_UNTAG_ERASE_GROUP               37  /* CMD37=0x65 */
#define MSD_ERASE                           38  /* CMD38=0x66 */
#define MSD_READ_OCR                        39  /* CMD39=0x67 */
#define MSD_CRC_ON_OFF                      40  /* CMD40=0x68 */

typedef struct _MSD_CSD         /*Card Specific Data*/
{
  vu8  CSDStruct;               /* CSD structure */
  vu8  SysSpecVersion;          /* System specification version */
  vu8  Reserved1;               /* Reserved */
  vu8  TAAC;                    /* Data read access-time 1 */
  vu8  NSAC;                    /* Data read access-time 2 in CLK cycles */
  vu8  MaxBusClkFrec;           /* Max. bus clock frequency */
  vu16 CardComdClasses;         /* Card command classes */
  vu8  RdBlockLen;              /* Max. read data block length */
  vu8  PartBlockRead;           /* Partial blocks for read allowed */
  vu8  WrBlockMisalign;         /* Write block misalignment */
  vu8  RdBlockMisalign;         /* Read block misalignment */
  vu8  DSRImpl;                 /* DSR implemented */
  vu8  Reserved2;               /* Reserved */
  vu16 DeviceSize;              /* Device Size */
  vu8  MaxRdCurrentVDDMin;      /* Max. read current @ VDD min */
  vu8  MaxRdCurrentVDDMax;      /* Max. read current @ VDD max */
  vu8  MaxWrCurrentVDDMin;      /* Max. write current @ VDD min */
  vu8  MaxWrCurrentVDDMax;      /* Max. write current @ VDD max */
  vu8  DeviceSizeMul;           /* Device size multiplier */
  vu8  EraseGrSize;             /* Erase group size */
  vu8  EraseGrMul;              /* Erase group size multiplier */
  vu8  WrProtectGrSize;         /* Write protect group size */
  vu8  WrProtectGrEnable;       /* Write protect group enable */
  vu8  ManDeflECC;              /* Manufacturer default ECC */
  vu8  WrSpeedFact;             /* Write speed factor */
  vu8  MaxWrBlockLen;           /* Max. write data block length */
  vu8  WriteBlockPaPartial;     /* Partial blocks for write allowed */
  vu8  Reserved3;               /* Reserded */
  vu8  ContentProtectAppli;     /* Content protection application */
  vu8  FileFormatGrouop;        /* File format group */
  vu8  CopyFlag;                /* Copy flag (OTP) */
  vu8  PermWrProtect;           /* Permanent write protection */
  vu8  TempWrProtect;           /* Temporary write protection */
  vu8  FileFormat;              /* File Format */
  vu8  ECC;                     /* ECC code */
  vu8  CRC;                     /* CRC */
  vu8  Reserved4;               /* always 1*/
}
sMSD_CSD, MSD_CSD;

typedef struct _MSD_CID        /*Card Identification Data*/
{
  vu8  ManufacturerID;          /* ManufacturerID */
  vu16 OEM_AppliID;             /* OEM/Application ID */
  vu32 ProdName1;               /* Product Name part1 */
  vu8  ProdName2;               /* Product Name part2*/
  vu8  ProdRev;                 /* Product Revision */
  vu32 ProdSN;                  /* Product Serial Number */
  vu8  Reserved1;               /* Reserved1 */
  vu16 ManufactDate;            /* Manufacturing Date */
  vu8  CRC;                     /* CRC */
  vu8  Reserved2;               /* always 1*/
}
sMSD_CID;

#define CMD0    (0x40+0)	    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)	    /* SEND_OP_COND (MMC) */
#define ACMD41  (0xC0+41)	    /* SEND_OP_COND (SDC) */
#define CMD8    (0x40+8)	    /* SEND_IF_COND */
#define CMD9    (0x40+9)	    /* SEND_CSD */
#define CMD10   (0x40+10)	    /* SEND_CID */
#define CMD12   (0x40+12)	    /* STOP_TRANSMISSION */
#define ACMD13  (0xC0+13)	    /* SD_STATUS (SDC) */
#define CMD16   (0x40+16)	    /* SET_BLOCKLEN */
#define CMD17   (0x40+17)	    /* READ_SINGLE_BLOCK */
#define CMD18   (0x40+18)	    /* READ_MULTIPLE_BLOCK */
#define CMD23   (0x40+23)	    /* SET_BLOCK_COUNT (MMC) */
#define ACMD23  (0xC0+23)	    /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24   (0x40+24)	    /* WRITE_BLOCK */
#define CMD25   (0x40+25)	    /* WRITE_MULTIPLE_BLOCK */
#define CMD55   (0x40+55)	    /* APP_CMD */
#define CMD58   (0x40+58)	    /* READ_OCR */
#define CMD32   (0x40+32)
#define CMD33   (0x40+33)
#define CMD38   (0x40+38)

/* MSD functions return */
#define MSD_SUCCESS      0x00
#define MSD_BUSY         0x01
#define MSD_FAIL         0xFF
#define MSD_NOBUSY       0xFF

#define MSD_BUS_BUSY     0x00

#define MSD_CSD_LENGTH   16
#define MSD_READ_DELAY   0xfff
#define MSD_CMD_DELAY    0xffff
#define MSD_START_DELAY  0xffff

u8 MSD_Init(void);

u8 MSD_WaitReady(void);
u8 MSD_ReadCSDRegister(MSD_CSD* pCsd);

u8 MSD_SendCommand(u8 cmd, u32 arg);
u8 MSD_ReadBlock(u8 *pBuffer, u32 addr, u32 len);
u8 MSD_ReadBlocks(u8 *pBuffer, u32 addr, u32 len);

u8 MSD_WriteBlock(u8 *pBuffer, u32 addr, u32 len);
u8 MSD_WriteBlocks(u8 *pBuffer, u32 addr, u32 len);

u8 MSD_Reset(void);

#endif
