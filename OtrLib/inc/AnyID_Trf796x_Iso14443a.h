#ifndef _ANYID_TRF796X_ISO14443A_
#define _ANYID_TRF796X_ISO14443A_

#include "AnyID_Trf796x.h"
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
#define ISO14443A_MOBLOCKNUM_MAX            52
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
#define ISO14443A_M0PAGE_NUM                4
#define ISO14443A_M0KEY_LEN                 4
#define ISO14443A_M0CNT_LEN                 3
#define ISO14443A_M0SIG_LEN                 32
#define ISO14443A_M0PCK_LEN                 2
#define ISO14443A_M0U2KEY_LEN               16
#define ISO14443A_TOPAZ_BLOCK_LEN           8
#define ISO14443A_TOPAZ_UID_LEN             7
#define ISO14443A_TOPAZBLOCKNUM_MAX         26  //8BYTE/BLOCK
//#define ISO14443A_NAK_AK_LEN                4

//时间(单位:us)
#define ISO14443A_TO_TIME                   100000
#define ISO14443A_FDT_INV                   1000
#define ISO14443A_FDT_READ                  10000
#define ISO14443A_FDT_WRITE                 10000

//request返回的标签类型双字节表示的含义
#define ISO14443A_TAGTYPE_UIDSIZE_MASK      0x00C0
#define ISO14443A_TAGTYPE_UIDSIZE_SINGLE    0x0000
#define ISO14443A_TAGTYPE_UIDSIZE_DOUBLE    0x0040
#define ISO14443A_TAGTYPE_UIDSIZE_TRIPLE    0x0080

//防冲突
#define ISO14443A_ANTICOLLISION_NVB_START   0x20
#define ISO14443A_GetNVB(nvb, byte, bit)    ((nvb) = (((byte) << 4) & 0xF0) | ((bit) & 0x0F))

#define ISO14443A_TAGTYPE_TOPAZ             0x0C00

#define ISO14443A_TAGSAK_MINI               0x09


#define ISO14443A_TAGTYPE_S50               0x0004
#define ISO14443A_TAGSAK_S50                0x08

#define ISO14443A_TAGTYPE_S70               0x0002
#define ISO14443A_TAGSAK_S70                0x18

#define ISO14443A_TAGTYPE_DESFIRE           0x0344
#define ISO14443A_TAGSAK_DESFIRE            0x20

#define ISO14443A_TAGTYPE_NFCT2             0x0044
#define ISO14443A_TAGSAK_NFCT2              0x00

//PLUS
#define ISO14443A_TAGTYPE_PLUS2K            0x0044
#define ISO14443A_TAGTYPE_PLUS4K            0x0042

#define ISO14443A_TAGSAK_PLUS2KSL2          0x10
#define ISO14443A_TAGSAK_PLUS4KSL2          0x11
#define ISO14443A_TAGSAK_PLUSSL3            0x20

#define ISO14443A_TAGSAK_DEP                0x20

//select
#define ISO14443A_SELECT_NVB                0x70
#define ISO14443A_SELECT_RSP_LEN            0x03    //sak crc
#define ISO14443A_SELECT_SAK_UID_UNCOMP     0x04
#define ISO14443A_SELECT_SAK_UID_COMP4      0x20
#define ISO14443A_SELECT_SAK_UID_COMP3      0x00

//halt
#define ISO14443A_HALT_PAR                  0x00
#define ISO14443A_HALT_RSP_LEN              0x00    //sak crc

typedef struct iso14443aUid{
    u16 type;
    u8 len;                                 //
    u8 uid[ISO14443A_MAX_UID_LEN];          //
    u8 sak;
}ISO14443A_UID;
#define ISO14443A_SIZE_UID                  14
#define ISO14443A_UID_CT                    0x88

#define ISO14443A4_FSCI_DFT                 2
#define ISO14443A4_FWI_DFT                  4
typedef struct iso14443_4{
    u8 pcb;
    u16 fsc;                                 //tag能够接收的
    u32 fwt;
}ISO14443A_4;
extern ISO14443A_4 g_sIso14443a4;

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

//读写命令的应答帧格式
#define ISO14443A_ACK_LEN                   0x04
#define ISO14443A_ACK_MASK                  0x0F
#define ISO14443A_ACK_OK                    0x0A
#define ISO14443A_ACK_NOT_ALLOW             0x04
#define ISO14443A_ACK_TRS_ERROR             0x05

#define ISO14443A_RX_WAIT_MIN               20      //tick=37.73
#define ISO14443A_RX_WAIT_MAX               500     //最大支持20ms = 20000 / 37.76

#define ISO14443A_APDU_LEN                  256     //最大支持20ms = 20000 / 37.76

typedef struct iso14443aApdu{
    u8 buf[ISO14443A_APDU_LEN];
    u8 len;
}ISO14443A_APDU;

void Iso14443a_Init(void);
u8 Is014443a_CheckBcc(u8 *pUid);
u8 Iso14443a_RequestTag(u8 mode, u16 *pType);
u8 Iso14443a_AnticollisionTag(u8 lv, u8 *pUid);
u8 Iso14443a_SelectTag(u8 lv, u8 *pUid, u8 *pSak);
u8 Iso14443a_GetUid(ISO14443A_UID *pUid, u8 mode);

u8 Iso14443a_ReadPage(u8 blockAddr, u8 *pPage);
u8 Iso14443a_WritePage(u8 blockAddr, u8 *pPage);

u8 Iso14443a_Rats(u8 *pAts, u8 *pLen);
u8 Iso14443a_TransAPDU(u8 *pApdu, u8 len, u8 *pAsk, u8 *pAskLen);
u8 Iso14443a_Dtu(u8 *pframe, u8 txLen, u8 txBit, u8 *pRxBit, u32 timeout);
#endif

