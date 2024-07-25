#ifndef _ANYID_RC663_I18000P3M3_
#define _ANYID_RC663_I18000P3M3_

#include "AnyID_RC663.h"

#define EPC_T1_MAX_US           78U
#define EPC_T2_MIN_US           151U
#define EPC_T3_MIN_US           25U
#define EPC_T4_MIN_US           100U

#define EPC_CAL_ACTION_TID          0x00
#define EPC_CAL_ACTION_EPC          0x01
#define EPC_CAL_ACTION_CRC          0x02
#define EPC_CAL_ACTION_LEDOFF_CRC   0x04
#define EPC_CAL_ACTION_LEDON_CRC    0x06

#define EPC_SEL_ACTION_2A_NO    0x01
#define EPC_SEL_ACTION_NO_2B    0x02
#define EPC_SEL_ACTION_AB_NO    0x03
#define EPC_SEL_ACTION_2B_2A    0x04
#define EPC_SEL_ACTION_2B_NO    0x05
#define EPC_SEL_ACTION_NO_2A    0x06
#define EPC_SEL_ACTION_NO_AB    0x07

#define EPC_SEL_TARGET_S0       0x00
#define EPC_SEL_TARGET_S1       0x01
#define EPC_SEL_TARGET_S2       0x02
#define EPC_SEL_TARGET_S3       0x03
#define EPC_SEL_TARGET_SL       0x04

#define EPC_SEL_ACTION_2A_2B    0x00
#define EPC_SEL_ACTION_2A_NO    0x01
#define EPC_SEL_ACTION_NO_2B    0x02
#define EPC_SEL_ACTION_AB_NO    0x03
#define EPC_SEL_ACTION_2B_2A    0x04
#define EPC_SEL_ACTION_2B_NO    0x05
#define EPC_SEL_ACTION_NO_2A    0x06
#define EPC_SEL_ACTION_NO_AB    0x07

#define EPC_SEL_MEM_RFU         0x00
#define EPC_SEL_MEM_EPC         0x01
#define EPC_SEL_MEM_TID         0x02
#define EPC_SEL_MEM_USR         0x03

#define EPC_SEL_ADDR_LEN_08BIT  0x00
#define EPC_SEL_ADDR_LEN_16BIT  0x01
#define EPC_SEL_ADDR_LEN_24BIT  0x02
#define EPC_SEL_ADDR_LEN_32BIT  0x03

#define EPC_SEL_TRUNCT_DISBLE   0x00
#define EPC_SEL_TRUNCT_ENBLE    0x01

#define EPC_PICC_SELECT         0x00
#define EPC_PICC_BEGIN          0x01
#define EPC_PICC_NXTSLOT        0x02
#define EPC_PICC_RESIZE         0x03
#define EPC_PICC_ACK            0x04
#define EPC_PICC_REQRN          0x05
#define EPC_PICC_READ           0x06
#define EPC_PICC_WRITE          0x07
#define EPC_PICC_LOCK           0x08
#define EPC_PICC_KILL           0x09
#define EPC_PICC_AUTH1          0x0A
#define EPC_PICC_AUTH2          0x0B
#define EPC_PICC_CFGEAS         0x0C
#define EPC_PICC_CALL           0x0D


#define EPC_CMD_CALL            0x0B
#define EPC_CMD_SELECT          0x0A
#define EPC_CMD_BEGIN           0x08
#define EPC_CMD_RESIZE          0x09
#define EPC_CMD_NXTSLOT         0x00
#define EPC_CMD_ACK             0x01
#define EPC_CMD_NAK             0xC0
#define EPC_CMD_REQRN           0xC1
#define EPC_CMD_READ            0xC2
#define EPC_CMD_WRITE           0xC3
#define EPC_CMD_KILL            0xC4
#define EPC_CMD_LOCK            0xC5
#define EPC_CMD_ACCESS          0xC6
#define EPC_CMD_CFGEAS          0xE007

#define EPC_SEL_TARGET_S0       0x00
#define EPC_SEL_TARGET_S2       0x02
#define EPC_SEL_TARGET_SL       0x04
#define EPC_SEL_TARGET_MSK      0x07

#define EPC_SEL_ACTION_MSK      0x07

#define EPC_MASK_LEN            32
typedef struct epcMask{
    u8 bank;
    u8 addrLen;
    u32 addr;
    u8 len;
    u8 value[EPC_MASK_LEN];
}EPC_MASK;

#define EPC_BANK_RFU            0
#define EPC_BANK_EPC            1
#define EPC_BANK_TID            2
#define EPC_BANK_USR            3

#define EPC_ADDR_LEN_08         0
#define EPC_ADDR_LEN_16         1
#define EPC_ADDR_LEN_24         2
#define EPC_ADDR_LEN_32         3

typedef struct epcSelectInfo{
    u8 target;
    u8 action;
    EPC_MASK msk;
}EPC_SELINFO;

#define EPC_ROUND_SEL_ALL       0x00
#define EPC_ROUND_SEL_0         0x02
#define EPC_ROUND_SEL_1         0x03

#define EPC_ROUND_S0            0x00
#define EPC_ROUND_S1            0x01
#define EPC_ROUND_S2            0x02
#define EPC_ROUND_Q_UP          0x06
#define EPC_ROUND_Q_DN          0x03
#define EPC_ROUND_Q_NC          0x00

#define EPC_ROUND_FLAG_A        0
#define EPC_ROUND_FLAG_B        1
typedef struct epcRoundInfo{
    u8 flag;
    u8 sel;
    u8 session;
    u8 q;
    u8 upDn;
}EPC_ROUNDINFO;

#define EPC_BANK_SIZE               16
#define EPC_BANK_KEY_LEN            4
typedef struct epcBank{
    u8 index;
    u8 addr;
    u8 count;
    u8 buf[EPC_BANK_SIZE];
    u8 key[EPC_BANK_KEY_LEN];
}EPC_BANK;

typedef struct tagParams{
    u16 storedCrc;
    u16 pc;
    u8 epcLen;
    u8 epc[EPC_BANK_SIZE];
    u8 memory[EPC_BANK_SIZE];
}EPC_TAG;

#define EPC_Q_INIT                          4//8
#define EPC_QFP_INIT                        400
#define EPC_QFP_MAX                         1500
#define EPC_C1_VALUE                        15
#define EPC_C2_VALUE                        35
typedef struct epcAnticoll{
    u16 tagNum;
    u8 colliCount;
    u8 noRspCount;
    u16 slot;
    //u8 q;
    u16 qfp;
    u8 c;
}EPC_ANTICOLL;


 //最长可能超过256字节，主要是APDU模式时候
#define EPC_FRAME_BIT_LEN               (528)
#define EPC_FRAME_BYT_LEN               ((EPC_FRAME_BIT_LEN >> 3) + 1)
#define EPC_CRC_NO                      0
#define EPC_CRC_5                       1
#define EPC_CRC_16                      2

#define EPC_CRC16_LEN                   16
#define EPC_CRC5_LEN                    5
#define Epc_ResetFrame(p)               memset((p), 0, sizeof(EPC_FRAME))
typedef struct epcFrame{
    u8 crc5;
    u16 crc16;

    BOOL bCryptOn;
    BOOL bPreamble;
    BOOL bEpcReply;

    u16 waitTime;
    u8 txCrcMode;
    u8 rxCrcMode;

    u16 txbitsNum;
    u16 rxbitsNum;

    u16 rn;

    u8 bits[EPC_FRAME_BIT_LEN];
    
    //u8 bytNum;
    u8 lastBitNum;
    //u8 byts[EPC_FRAME_BYT_LEN];

    u32 to;
}EPC_FRAME;

#define EPC_RN_LEN                  16
#define EPC_BLOCK_LEN               16

#define EPC_ERR_SUC          0
#define EPC_ERR_TAG          1
#define EPC_ERR_CRC          2
#define EPC_ERR_NORSP        3
typedef struct epcInfonition{
    EPC_TAG t;
    //什么命令发起了该操作
    EPC_SELINFO s;
    EPC_ROUNDINFO r;
    EPC_BANK b;
    u8 nWordIndex;
    EPC_ANTICOLL anti;

    EPC_FRAME frame;

    u16 rn;
    u8 rlt;
    u16 collisionCount;
}EPC_INFO;
extern EPC_INFO g_sEpcInfo;

u8 Epc_Init(void);
u16 Epc_ForamtBit(u8 *pBits, u16 pos, u32 dat, u8 num);
u16 Epc_ForamtBitLeftAlign(u8 *pBits, u16 pos, u8 dat, u8 num);
u32 Epc_FormatByt(u8 *pBits, u8 len);
u8 Epc_FormatBuffer(u8 *pBuffer, u8 *pBits, u16 len);
u8 Epc_CalCrc5(u8 *pBits, u16 bitNum);
u16 Epc_CalCrc16(u8 *pBits, u16 bitNum);
BOOL Epc_CheckCrc5(u8 *pBits, u16 len);
BOOL Epc_CheckCrc16(u8 *pBits, u16 len);

u8 Epc_Command(EPC_FRAME *pEpcFrame);
u8 Epc_CalPicc(EPC_INFO *pEpcInfo);
u8 Epc_SelectPicc(EPC_INFO *pEpcInfo);
u8 Epc_BeginRoundPicc(EPC_INFO *pEpcInfo);
u8 Epc_ResizeRoundPicc(EPC_INFO *pEpcInfo);
u8 Epc_NextSlotPicc(EPC_INFO *pEpcInfo);
u8 Epc_AckPicc(EPC_INFO *pEpcInfo);
u8 Epc_NakPicc(EPC_INFO *pEpcInfo);
u8 Epc_ReqRnPicc(EPC_INFO *pEpcInfo);
u8 Epc_ReadPicc(EPC_INFO *pEpcInfo);
u8 Epc_WritePicc(EPC_INFO *pEpcInfo);
u8 Epc_ConfigEas(EPC_INFO *pEpcInfo, BOOL bEnable);
void Epc_ResetAntiColl(EPC_INFO *pEpcInfo, BOOL bBeginRound);
u8 Epc_RoundQfp(u8 qfp);
void Epc_ResizeQ(EPC_INFO *pEpcInfo);
#endif
