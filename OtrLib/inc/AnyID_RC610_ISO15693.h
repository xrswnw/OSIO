#ifndef _ANYID_RC610_ISO15693_
#define _ANYID_RC610_ISO15693_

#include "AnyID_RC610.h"

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

#define ISO15693_CMD_SETEAS                     0x2D
#define ISO15693_CMD_RESETEAS                   0x2E
#define ISO15693_CMD_LOCKEAS                    0x2F



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
#define ISO15693_REQ_FLAG_DUBCARRIER            0x01
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

#define ISO15693_ERR_LOCKED                     0x11




#define ISO15693_UID_TYPE_POS                   0x06
#define ISO15693_TAG_TYPE_TI                    0x07        //0x03
#define ISO15693_TAG_TYPE_NXP                   0x04        //0x02
#define ISO15693_TAG_TYPE_ST                    0x02        //0x01
#define ISO15693_TAG_TYPE_DFT                   0x00
#define ISO15693_TAG_TYPE_ERR                   0xFF
#define ISO15693_TAG_TYPE_READ                  0xFF

#define ISO15693_INVENTORY_TIME                 500
#define ISO15693_READ_TIME                      15000
#define ISO15693_WRITE_TIME                     25000
#define ISO15693_OP_TIME                        25000

#define ISO15693_OPUID_MAX_NUM                  25
#define ISO15693_OPBLOCK_MAX_NUM                32


typedef struct iso15693Mask{
    u8 value[ISO15693_SIZE_UID];//掩码值
    u8 length;                  //掩码长度
}ISO15693_MASK;

#define ISO15693_MASK_NUM                       32
//防冲突
typedef struct iso15693MaskBuffer{
    ISO15693_MASK mask[ISO15693_MASK_NUM];  //存储掩码
    ISO15693_MASK cur;                      //当前的
    u8 index;                               //计数
    u8 num;                                 //总数
}ISO15693_MSKBUF;

typedef struct iso15693Dut{
    u8 txLen;
    u8 rxLen;
    u8 frame[32];
    u16 rspTime;
}ISO15693_DTU;

#define ISO15693_SIZE_EASALARM                  32

#define ISO15693_OP_BLOCK_NUM                   32

typedef struct iso15693Block{
    u8 block[(ISO15693_SIZE_BLOCK + ISO15693_SIZE_BLOCK) << 2];     //最多一次要求多个（4个）数据块
    u8 state;
    u8 rAddr[ISO15693_OP_BLOCK_NUM];
    u8 wAddr[ISO15693_OP_BLOCK_NUM];
    u8 rIndex;
    u8 rNum;
    u8 wIndex;
    u8 wNum;
    u8 opNum;
}ISO15693_BLOCK;

typedef struct iso15693Infonition{
    u8 c;
    u8 slot;
    //mask
    ISO15693_MSKBUF mskBuf;
    u8 optionFlag;
    u8 errCode;
    //底层的操作信息
   // PCD_INFO pcd;
    //操作标签需要的信息
    //uid
   u8 uid[ISO15693_SIZE_UID];
    //block
    ISO15693_BLOCK b;
    u8 afi;
    u8 dsfid;
    ISO15693_DTU dtu;
    //eas
    u8 eas[ISO15693_SIZE_EASALARM];
    int rlt;
}ISO15693_INFO;

typedef struct iso15693TagInfo{
    u8 flag;
    BOOL bDsfid;
    BOOL bAfi;
    BOOL bViccMemoryInf;
    BOOL bICRef;
    u8 afi;
    u8 dsfid;
    u8 blockSize;
    u8 blockNum;
    u8 icRef;
}ISO15693_TAGINFO;

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
u8 ISO15693_InitDoubleMod(void);
u8 ISO15693_Inventory(u8 afiCtrl, u8 afiValue, u8 *pUid);
u8 ISO15693_Inventory_Slot1(u8 afiCtrl, u8 afiValue, u8 *pUid);
u8 ISO15693_Inventory_Slot16(ISO15693_MASK *pMask,u8 afiCtrl, u8 afiValue, u8 *pUid);
u8 ISO15693_WriteMemory(u8 *pUID, u8 cmd, u8 value);
u8 ISO15693_StayQuiet(u8 *pUID);
u8 ISO15693_ReadBlock(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size);
u8 ISO15693_WriteBlock(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size);
u8 ISO15693_LockBlock(u8 *pUID, u8 addr);
u8 ISO15693_WriteAFI(u8 *pUID, u8 afi);
u8 ISO15693_LockAFI(u8 *pUID);
u8 ISO15693_WriteDSFID(u8 *pUID, u8 dsfid);
u8 ISO15693_LockDSFID(u8 *pUID);
u8 ISO15693_GetTagInf(u8 *pUID, ISO15693_TAGINF *pTagInfo);
u8 ISO15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout);
u8 ISO15693_Command(u8 *pUID, u8 cmd, u8 *pTxParams, u8 txParamsLen, u8 *pRxParams, u8 rxParamsLen, BOOL bOpt);

u8 ISO15693_ReadBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size);
u8 ISO15693_WriteBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size);\
    
u8 Iso15693_InventoryEof(u8 *pUid);    
void Iso15693_AddMask(ISO15693_MSKBUF *pMaskBuf, u8 slot);    
    
#endif

