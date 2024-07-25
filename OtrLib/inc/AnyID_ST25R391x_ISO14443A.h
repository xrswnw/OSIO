#ifndef _ANYID_ST25R391X_ISO14443A_
#define _ANYID_ST25R391X_ISO14443A_


#include "AnyID_ST25R391x.h"
#include "AnyID_Mifare_Auth.h"

#define ST25R3916_STAT_OK                0x00   //正确
#define ST25R3916_STAT_INIT_ERROR        0x01   //系统初始化错误
#define ST25R3916_STAT_CMD_ERROR         0x02   //错误的RC522命令
#define ST25R3916_STAT_TIMEOUT           0x03   //RC522命令执行超时
#define ST25R3916_STAT_COLLISION_ERROR   0x04   //碰撞错误 STATUS_RC522_TEMP_ERROR
#define ST25R3916_STAT_PARITY_ERROR      0x05   //校验错误
#define ST25R3916_STAT_PROTOCOL_ERROR    0x06   //协议错误
#define ST25R3916_STAT_FIFOOF_ERROR      0x07   //FIFO溢出
#define ST25R3916_STAT_CRC_ERROR         0x08   //CRC错误
#define ST25R3916_STAT_TEMP_ERROR        0x09   //温度过高
#define ST25R3916_STAT_WFIFO_ERROR       0x0A   //写FIFO错误
#define ST25R3916_STAT_ACK               0x0B   //ASK
#define ST25R3916_STAT_ERROR             0x0C   //错误
#define ST25R3916_STAT_PICC_TYPE_ERROR   0x0D   //PICC类型不存在
#define ST25R3916_STAT_AUTH_ERROR        0x0F   //授权认证错误
#define ST25R3916_STAT_TRANS_ERROR       0x10   //传输错误
#define ST25R3916_STAT_BCC_ERROR         0x11   //BCC错误
#define ST25R3916_STAT_NAK               0x12   //BCC错误
#define ST25R3916_STAT_AUTHM0U2_ERR      0x13   //ultralightC授权错误
#define ST25R3916_STAT_HANDLE_ERR        0x14   //ultralightC授权错误


//14443命令
#define ISO14443A_CMD_IDLE                  0x00    // IDLE command
#define ISO14443A_CMD_REQALL                0x52    // 唤醒命令
#define ISO14443A_CMD_REQIDL                0x26    // 请求命令
#define ISO14443A_CMD_SELECT0               0x93
#define ISO14443A_CMD_SELECT1               0x95
#define ISO14443A_CMD_SELECT2               0x97
#define ISO14443A_CMD_HALTA                 0x50   //将卡挂起
#define ISO14443A_CMD_AUTHENT_A             0x60   //AUTHENT A command
#define ISO14443A_CMD_AUTHENT_B             0x61   //AUTHENT B command
#define ISO14443A_CMD_READ                  0x30   //READ command
#define ISO14443A_CMD_WRITE16               0xA0   //WRITE 16 bytes command
#define ISO14443A_CMD_WRITE4                0xA2   //WRITE 4 bytes command
#define ISO14443A_CMD_INCREMENT             0xC1   //INCREMENT command,
                                                    //将block的Value模式数据加上命令帧中的数据
                                                    //然后将结果保存在标签内部的寄存器区
#define ISO14443A_CMD_DECREMENT             0xC0   //DECREMENT command
                                                    //将block的Value模式数据减去命令帧中的数据
                                                    //然后将结果保存在标签内部的寄存器区

#define ISO14443A_CMD_RESTORE               0xC2   //RESTORE command
                                                    //将指定的block内的数据读取到标签内部寄存器区
#define ISO14443A_CMD_TRANSFER              0xB0   //TRANSFER command
                                                    //将标签内部寄存器区内的数据写入指定的block内
#define ISO14443A_CMD_RATS                  0xE0
#define ISO14443A_CMD_PPS                   0xD0

#define ISO14443A_CMD_SIG                   0x3C
#define ISO14443A_CMD_CNT                   0x39
#define ISO14443A_CMD_PWD                   0x1B

#define ISO14443A_CMD_AUTHM0U2S1            0x1A
#define ISO14443A_CMD_AUTHM0U2S2            0xAF

#define ISO14443A_CMD_TPZRID                0x78
#define ISO14443A_CMD_TPZRALL               0x00
#define ISO14443A_CMD_TPZREAD               0x01
#define ISO14443A_CMD_TPZREADBLK            0x02
#define ISO14443A_CMD_TPZWE                 0x53
#define ISO14443A_CMD_TPZWEBLK              0x54
#define ISO14443A_CMD_TPZWNE                0x1A
#define ISO14443A_CMD_TPZWNEBLK             0x1B

#define ISO14443A_FSD                       64
#define ISO14443A_FSDI_FSD                  0x80
#define ISO14443A_CID                       0x01


//长度

#define ISO14443A_MOBLOCKNUM_MAX            52  //4BYTE/PAGE
#define ISO14443A_MAX_FRAME_LEN             64
#define ISO14443A_MAX_UID_LEN               10
#define ISO14443A_MAX_ANTCOLL_LOOP          32
#define ISO14443A_SIGNAL_UID_LEN            4
#define ISO14443A_DOUBLE_UID_LEN            7
#define ISO14443A_TRIPLE_UID_LEN            10
#define ISO14443A_M1_KEY_LEN                6
#define ISO14443A_M1BLOCK_LEN               16
#define ISO14443A_M0BLOCK_LEN               4
#define ISO14443A_M1VALUE_LEN               4
#define ISO14443A_SECTOR_LEN                4
#define ISO14443A_M0KEY_LEN                 4
#define ISO14443A_M0CNT_LEN                 3
#define ISO14443A_M0SIG_LEN                 32
#define ISO14443A_M0PCK_LEN                 2
#define ISO14443A_M0U2KEY_LEN               16
#define ISO14443A_TOPAZ_BLOCK_LEN           8
#define ISO14443A_TOPAZ_UID_LEN             7
#define ISO14443A_TOPAZBLOCKNUM_MAX         26  //8BYTE/BLOCK

#define ISO14443A_ANTI_LV_MAX               32

//时间(单位:us)
#define ISO14443A_INVENTORY_TIME            500
#define ISO14443A_INVENTORY_TIME_MIN        500
#define ISO14443A_NORMAL_TIME               10000
#define ISO14443A_AUTH_TIME                 2000
#define ISO14443A_READ_TIME                 10000
#define ISO14443A_WRITE_TIME                25000
#define ISO14443A_VALUE_TIME                15000



//发送控制
#define ISO14443A_FRAME_LSB_BIT1            0x01
#define ISO14443A_FRAME_LSB_BIT2            0x02
#define ISO14443A_FRAME_LSB_BIT3            0x03
#define ISO14443A_FRAME_LSB_BIT4            0x04
#define ISO14443A_FRAME_LSB_BIT5            0x05
#define ISO14443A_FRAME_LSB_BIT6            0x06
#define ISO14443A_FRAME_LSB_BIT7            0x07
#define ISO14443A_FRAME_LSB_BIT8            0x08

//request返回的标签类型双字节表示的含义
#define ISO14443A_TAGTYPE_UIDSIZE_MASK      0x00C0
#define ISO14443A_TAGTYPE_UIDSIZE_SINGLE    0x0000
#define ISO14443A_TAGTYPE_UIDSIZE_DOUBLE    0x0040
#define ISO14443A_TAGTYPE_UIDSIZE_TRIPLE    0x0080

#define ISO14443A_TAGTYPE_TOPAZ             0x0C00
#define ISO14443A_TAGTYPE_FM11RF005         0x0003

#define ISO14443A_TAGTYPE_S50               0x0004
#define ISO14443A_TAGSAK_S50                0x08

#define ISO14443A_TAGTYPE_S70               0x0002
#define ISO14443A_TAGSAK_S70                0x18

#define ISO14443A_TAGTYPE_DESFIRE           0x0344
#define ISO14443A_TAGSAK_DESFIRE            0x20

#define ISO14443A_TAGTYPE_NFCT2             0x0044
#define ISO14443A_TAGSAK_NFCT2              0x00

//PLUS
#define ISO14443A_TAGTYPE_PLUS2KC1          0x0004
#define ISO14443A_TAGTYPE_PLUS4KC1          0x0002
#define ISO14443A_TAGTYPE_PLUS2KC2          0x0044
#define ISO14443A_TAGTYPE_PLUS4KC2          0x0042

#define ISO14443A_TAGSAK_PLUS2KSL1          0x08
#define ISO14443A_TAGSAK_PLUS4KSL1          0x18
#define ISO14443A_TAGSAK_PLUS2KSL2          0x10
#define ISO14443A_TAGSAK_PLUS4KSL2          0x11
#define ISO14443A_TAGSAK_PLUSSL3            0x20



//防冲突
#define ISO14443A_ANTICOLLISION_NVB_START   0x20
#define ISO14443A_GetNVB(nvb, byte, bit)    ((nvb) = (((byte) << 4) & 0xF0) | ((bit) & 0x0F))

//select
#define ISO14443A_SELECT_NVB                0x70
#define ISO14443A_SELECT_RSP_LEN            0x03    //sak crc
#define ISO14443A_SELECT_SAK_UID_UNCOMP     0x04
#define ISO14443A_SELECT_SAK_UID_COMP4      0x20
#define ISO14443A_SELECT_SAK_UID_COMP3      0x00

//halt
#define ISO14443A_HALT_PAR                  0x00
#define ISO14443A_HALT_RSP_LEN              0x00    //sak crc

#pragma pack(1)
typedef struct iso14443aUid{
    u16 type;
    u8 len;                                 //
    u8 uid[ISO14443A_MAX_UID_LEN];          //
    u8 sak;
}ISO14443A_UID;
#pragma pack()

#define ISO14443A_SIZE_UID                  13
#define ISO14443A_UID_CT                    0x88

//读写命令的应答帧格式
#define ISO14443A_ACK_LEN                   0x04
#define ISO14443A_ACK_MASK                  0x0F
#define ISO14443A_ACK_OK                    0x0A
#define ISO14443A_ACK_NOT_ALLOW             0x04
#define ISO14443A_ACK_TRS_ERROR             0x05

u8 ISO14443A_Init(void);
u8 ISO14443A_Cmd(u8 cmd, ST25R3916_FRAME *pFrame);
u8 ISO14443A_RequestTag(u8 mode, u16 *pTagType);
u8 ISO14443A_AnticollisionTag(u8 lv, u8 *pUID);
u8 ISO14443A_SelectTag(u8 lv, u8 *pUID, u8 *pSak);
u8 ISO14443A_GetUid(ISO14443A_UID *pUid, u8 mode);
u8 ISO14443A_Halt(void);

u8 ISO14443A_AuthM1(u8 *pUID, u8 authMode, u8 *pKey, u8 blockAddr);
u8 ISO14443A_ReadMifareBlock(u8 blockAddr, u8 *pBlock);
u8 ISO14443A_WriteMifareBlock16(u8 blockAddr, u8 *pBlock);
u8 ISO14443A_WriteMifareBlock4(u8 blockAddr, u8 *pBlock);
u8 ISO14443A_Value(u8 opMode, u8 *pDataBuffer, u8 transAddr, u8 valueAddr);
void ISO14443A_FormatValue(u8 backUpAddr, int value, u8 *pBlockBuffer);
int ISO14443A_CheckValueFormat(u8 *pBlockBuffer);

u8 ISO14443A_Dtu(u8 *pTxFrame, u8 txLen, u8 txBit, u8 *pRxBit, u32 timeout);
u8 ISO14443A_AuthUltralightC(u8 *pKey);


#define ISO14443A_TAGTYPE_TOPAZ96       0x11
#define ISO14443A_TAGTYPE_TOPAZ512      0x12
BOOL ISO14443A_TopazCheckCrc(u8 *pFrame, u8 len);
u16 ISO14443A_TopazCalCrc(u8 *pFrame, u8 len);
u8 ISO14443A_TopazGetUid(ISO14443A_UID *pUid, u8 mode);
u8 ISO14443A_TopazReadBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock);
u8 ISO14443A_TopazWriteBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock);

#define ISO14443A4_FSCI_DFT                 2
#define ISO14443A4_FWI_DFT                  4
typedef struct iso14443_4{
    u8 pcb;
    u16 fsc;                                 //tag能够接收的
    u32 fwt;
}ISO14443A_4;
extern ISO14443A_4 g_sISO1444A4;

//iso14443a-4
#define ISO14443A_PCB_IBLOCK                0x0A
#define ISO14443A_IPCB_CHAIN                0x10
#define ISO14443A_IPCB_NUMMASK              0x01

#define ISO14443A_PCB_RBLOCK                0xAA
#define ISO14443A_RPCB_ACK                  0x10
#define ISO14443A_RPCB_NUMMASK              0x01

#define ISO14443A_PCB_SBLOCK                0xCA
#define ISO14443A_SPCB_DSEL                 0x00
#define ISO14443A_SPCB_WTX                  0x30
u8 ISO14443A_Rats(u8 *pAts, u8 *pLen);
u8 ISO14443A_TransAPDU(u8 *pApdu, u8 len, u8 *pAsk, u8 *pAskLen);

u8 ISO14443A__AuthCommand(void);
u8 ISO14443A_DirectCommand(u8 *pFrame, u8 len, u8 *pAsk, u8 *pAskLen);

#endif

