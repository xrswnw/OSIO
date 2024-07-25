#ifndef _ANYID_CR95HF_ISO15693_
#define _ANYID_CR95HF_ISO15693_


#include "AnyID_CR95HF.h"

// data rates constants fot select commands
#define ISO15693_MAX_FRAME_LEN                  64


#define ISO15693_CMD_INVENTORY                  0x01
#define ISO15693_CMD_STAYQUIET                  0x02
#define ISO15693_CMD_READSINGLEBLOCK            0x20
#define ISO15693_CMD_WRITESINGLEBLOCK           0x21
#define ISO15693_CMD_LOCKBLOCK                  0x22
#define ISO15693_CMD_READMULBLOCKS              0x23
#define ISO15693_CMD_WRITEMULBLOCKS             0x24
#define ISO15693_CMD_SELECT                     0x25
#define ISO15693_CMD_RESETTOREADY               0x26
#define ISO15693_CMD_WRITEAFI                   0x27
#define ISO15693_CMD_LOCKAFI                    0x28
#define ISO15693_CMD_WRITEDSFID                 0x29
#define ISO15693_CMD_LOCKDSFID                  0x2A
#define ISO15693_CMD_GETSYSINFO                 0x2B
#define ISO15693_CMD_GETSECURITYINFO            0x2C

#define ISO15693_ANTICOLLISION_SLOT             16

#define ISO15693_SIZE_UID                       0x08
#define ISO15693_SIZE_BLOCK                     0x04
#define ISO15693_SIZE_CRC16                     0x02
#define ISO15693_SIZE_DSFID                     0x01
#define ISO15693_SIZE_AFI                       0x01
#define ISO15693_SIZE_BLOCKSECURITY             0x01
#define ISO15693_SIZE_REPLYFLAG                 0x01
#define ISO15693_SIZE_INFOFLAG                  0x01
#define ISO15693_SIZE_MEMORYSIZE                0x02
#define ISO15693_SIZE_ICREF                     0x01
#define ISO15693_SIZE_REQUESTFLAG               0x01
#define ISO15693_SIZE_MASK                      8


#define ISO15693_REQ_FRAME_POS_FLAG             0x00
#define ISO15693_REQ_FRAME_POS_CMD              0x01
#define ISO15693_REQ_FRAME_POS_PAR              0x02

#define ISO15693_REQ_FLAG_SIGCARRIER            0x00
#define ISO15693_REQ_FLAG_SUBCARRIER            0x01
#define ISO15693_REQ_FLAG_LRATE                 0x00
#define ISO15693_REQ_FLAG_HRATE                 0x02
#define ISO15693_REQ_FLAG_NOINVT                0x00
#define ISO15693_REQ_FLAG_INVENTORY             0x04
#define ISO15693_REQ_FLAG_NOEXT                 0x00
#define ISO15693_REQ_FLAG_EXTPROTOCOL           0x08
#define ISO15693_REQ_FLAG_NOSELECTM             0x00
#define ISO15693_REQ_FLAG_SELECTM               0x10
#define ISO15693_REQ_FLAG_NOADDRM               0x00
#define ISO15693_REQ_FLAG_ADDRM                 0x20
#define ISO15693_REQ_FLAG_NOOPT                 0x00
#define ISO15693_REQ_FLAG_OPT                   0x40
#define ISO15693_REQ_FLAG_RFU                   0x00


#define ISO15693_REQ_FLAG_AFI                   0x10
#define ISO15693_REQ_FLAG_NOAFI                 0x00
#define ISO15693_REQ_FLAG_SLOTS_1               0x20
#define ISO15693_REQ_FLAG_SLOTS_16              0x00

#define ISO15693_RSP_FRAME_POS_FLAG             0x00
#define ISO15693_RSP_FRAME_POS_PAR              0x01
#define ISO15693_RSP_FRAME_POS_ERR              0x01
#define ISO15693_RSP_FLAG_ERR                   0x01
#define ISO15693_RSP_FLAG_COLLERR               0x01
#define ISO15693_RSP_FLAG_CRCERR                0x02

#define ISO15693_UID_TYPE_POS                   0x06
#define ISO15693_TAG_TYPE_TI                    0x07        //0x03
#define ISO15693_TAG_TYPE_NXP                   0x04        //0x02
#define ISO15693_TAG_TYPE_ST                    0x02        //0x01
#define ISO15693_TAG_TYPE_DFT                   0x00
#define ISO15693_TAG_TYPE_ERR                   0xFF
#define ISO15693_TAG_TYPE_READ                  0xFF

#define ISO15693_INVENTORY_TIME                 10000
#define ISO15693_READ_TIME                      15000
#define ISO15693_WRITE_TIME                     20000

#define ISO15693_OPUID_MAX_NUM                  25
#define ISO15693_OPBLOCK_MAX_NUM                32

typedef struct iso15693TagInf{
    u8 afi;
    u8 dsfid;
    u16 vicc;
    u8 icRef;
    u8 infFlag;
}ISO15693_TAGINF;
#define ISO15693_INFO_FLAG_DSFID                0x01
#define ISO15693_INFO_FLAG_AFI                  0x02
#define ISO15693_INFO_FLAG_VICC                 0x04
#define ISO15693_INFO_FLAG_ICREF                0x08

u8 ISO15693_Init(void);

u8 ISO15693_SendEof(u8 *pUid);

u8 ISO15693_CheckResponse(CR95HF_FRAME *pFrame);

u8 ISO15693_Inventory(u8 afiCtrl, u8 afiValue, u8 *pUid);
u8 ISO15693_StayQuiet(u8 *pUid);
u8 ISO15693_ReadBlock(u8 *pUid, u8 num, u8 addr, u8 *pData);
u8 ISO15693_WriteBlock(u8 *pUid, u8 num, u8 addr, u8 *pData);
u8 ISO15693_LockBlock(u8 *pUid, u8 addr);
u8 ISO15693_WriteAFI(u8 *pUid, u8 afi);
u8 ISO15693_LockAFI(u8 *pUid);
u8 ISO15693_WriteDSFID(u8 *pUid, u8 dsfid);
u8 ISO15693_LockDSFID(u8 *pUid);
u8 ISO15693_GetTagInf(u8 *pUid, ISO15693_TAGINF *pTagInfo);
u8 ISO15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout);


#endif

