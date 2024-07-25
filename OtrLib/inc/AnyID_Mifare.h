#ifndef _MIFARE_
#define _MIFARE_

#define PICC_ACTIVATE_EN        	1       // 激活处于天线感应区的卡片
#define PCD_RFRESET_EN          	1       // RF场复位(关闭载波)


//Mifare 命令
#define MIFARE_CMD_AUTHENT_A        0x60   //AUTHENT A command
#define MIFARE_CMD_AUTHENT_B        0x61   //AUTHENT B command
#define MIFARE_CMD_READ             0x30   //READ command
#define MIFARE_CMD_WRITE16          0xA0   //WRITE 16 bytes command
#define MIFARE_CMD_WRITE4           0xA2   //WRITE 4 bytes command
#define MIFARE_CMD_INCREMENT        0xC1   //INCREMENT command
#define MIFARE_CMD_DECREMENT        0xC0   //DECREMENT command
#define MIFARE_CMD_RESTORE          0xC2   //RESTORE command
#define MIFARE_CMD_TRANSFER         0xB0   //TRANSFER command
#define MIFARE_CMD_NOCOMMAND        0x00   //VOID command (no MIFARE command)
#define MIFARE_CMD_HALTA            0x50   //将卡挂起
#define MIFARE_CMD_REQALL           0x52    // 唤醒命令
#define MIFARE_CMD_REQIDL           0x26    // 请求命令
#define MIFARE_CMD_ANTICOLL_L1      0x93    // 防碰撞/选择等级1
#define MIFARE_CMD_ANTICOLL_L2      0x95    // 防碰撞/选择等级2
#define MIFARE_CMD_ANTICOLL_L3      0x97    // 防碰撞/选择等级3

#define MIFARE_CMD_AUTHM0U2S1       0x1A    // M0ICU2授权操作第一步
#define MIFARE_CMD_AUTHM0U2S2       0xAF    // M0ICU2授权操作第一步



#define MIFARE_EXPECT_TIMEOUT       0x01   //Tells the library to expect a timeout
#define MIFARE_EXPECT_ACK           0x02   //Let the library expect an Acknowledge response
#define MIFARE_EXPECT_DATA          0x04   //The library shall expect data

#define MIFARE_REQUEST_BITS         0x07    // 请求命令的位数


//防碰撞/选择命令参数定义
#define MIFARE_BITS_PER_BYTE        0x08    //一个字节8位
#define MIFARE_UPPER_NVB_SHIFT      0x04    //NVB高位移动位数，高4位表示有效字节数，低4位表示最后一个字节(不包含在有效字节中)有效位数
#define MIFARE_COMPLETE_UID_BITS    0x28
#define MIFARE_NVB_MIN_PARAMETER    0x20    //NVB最小值，只有CMD和NVB两个字节
#define MIFARE_NVB_MAX_PARAMETER    0x70    //NVB最大值，CMD NVB UID(4B) BCC

//
#define MIFARE_MAX_ANTICOLL_LEVELS  0x03    // 最大防碰撞等级
#define MIFARE_SINGLE_UID_LENGTH    0x20     //uid共4字节，32位
#define MIFARE_COLL_BIT             0x04

//选择命令回应的字节数
#define MIFARE_SAK_LENGTH           0x01

//挂起命令参数定义
#define MIFARE_HALTA_PARAM          0x00

//cascade tag
#define MIFARE_CT                   0x88

//卡类型
#define MIFARE_TYPE_MF0_U1          0x0044
#define MIFARE_TYPE_MF1_S50         0x0004
#define MIFARE_TYPE_MF1_S70         0x0002
#define MIFARE_TYPE_MF3_D40         0x0344
#define MIFARE_TYPE_PROX            0x0008

#define MIFARE_MF0_U1_ADDR_MAX      0x0F        //512bits, 16Block, 4Bytes pre block
#define MIFARE_MF1_S50_ADDR_MAX     0x3F        //8Kbits,   64Block, 16Bytes pre block


#define MIFARE_UID_MAX_LEN          10
#define MIFARE_UID_L1_LEN           4
#define MIFARE_UID_L2_LEN           7
#define MIFARE_UID_L3_LEN           10


//帧信息
//帧长度，这里全部是标准帧，如果有CRC-A的，帧长度不包括CRC的长度
//请求帧格式
#define MIFARE_REQUEST_LEN          1   //请求帧长度
#define MIFARE_ATQA_LEN             2   //请求帧响应长度
#define MIFARE_REQUEST_CMD_POS      0



//接收到的ATL(防碰撞)命令帧格式
#define MIFARE_ATL_FRM_FPCD_LEN     2   //接收来之PCD的防碰撞帧
#define MIFARE_ATL_FPCD_CMD_POS     0
#define MIFARE_ATL_FPCD_NVB_POS     1
//响应PCD的ATL(防碰撞)命令帧格式
#define MIFARE_ATL_FRM_TPCD_LEN     5   //响应PCD的防碰撞帧
#define MIFARE_ATL_TPCD_CT_POS      0
#define MIFARE_ATL_TPCD_BCC_POS     4

#define MIFARE_ATL_TPCD_CT_LEN      1
#define MIFARE_ATL_TPCD_UID_LEN     4



//来之PCD的SEL命令帧格式
#define MIFARE_SEL_FRM_FPCD_LEN     7   //接收来之PCD的选择帧
#define MIFARE_SEL_FPCD_CMD_POS     0
#define MIFARE_SEL_FPCD_NVB_POS     1
#define MIFARE_SEL_FPCD_UID_POS     2
#define MIFARE_SEL_FPCD_BCC_POS     6
#define MIFARE_SEL_FPCD_UID_LEN     4
#define MIFARE_SAK_FRM_TPCD_LEN     1   //响应PCD的SEL命令


//AUTH命令帧格式
#define MIFARE_AUTH_FRM_FPCD_LEN    12
#define MIFARE_AUTH_FPCD_CMD_POS    0
#define MIFARE_AUTH_FPCD_ADDR_POS   1
#define MIFARE_AUTH_FPCD_PWD_POS    2
#define MIFARE_AUTH_FPCD_UID_POS    8
#define MIFARE_AUTH_FPCD_PWD_LEN    6
#define MIFARE_AUTH_FPCD_UID_LEN    4


//读写命令的应答帧格式
#define MIFARE_ACK_MASK             0x0F
#define MIFARE_ACK_OK               0x0A
#define MIFARE_ACK_NOT_ALLOW        0x04
#define MIFARE_ACK_TRS_ERROR        0x05
#define MIFARE_ACK_BIT              0x04

//Read命令帧格式
#define MIFARE_READ_FRM_FPCD_LEN    2
#define MIFARE_READ_FPCD_CMD_POS    0
#define MIFARE_READ_FPCD_ADDR_POS   1
#define MIFARE_READ_FRM_TPCD_LEN    16
#define MIFARE_READ_TPCD_DATA_POS   0

//Write4命令帧格式
#define MIFARE_WRITE4_FRM_FPCD_LEN  6
#define MIFARE_WRITE4_FPCD_CMD_POS  0
#define MIFARE_WRITE4_FPCD_ADDR_POS 1
#define MIFARE_WRITE4_FPCD_DATA_POS 2
#define MIFARE_WRITE4_FPCD_DATA_LEN 4

//Write16命令帧格式
#define MIFARE_WRITE16_S1FRM_FPCD_LEN  2
#define MIFARE_WRITE16_S1FPCD_CMD_POS  0
#define MIFARE_WRITE16_S1FPCD_ADDR_POS 1
#define MIFARE_WRITE16_S2FRM_FPCD_LEN  16
#define MIFARE_WRITE16_S2FPCD_DATA_POS 0
#define MIFARE_WRITE16_S2FPCD_DATA_LEN 16

//value命令帧格式
#define MIFARE_VALUE_S1FRM_FPCD_LEN  2
#define MIFARE_VALUE_S1FPCD_CMD_POS  0
#define MIFARE_VALUE_S1FPCD_ADDR_POS 1
#define MIFARE_VALUE_S2FRM_FPCD_LEN  4
#define MIFARE_VALUE_S2FPCD_VAL_POS  0
#define MIFARE_VALUE_S3FRM_FPCD_LEN  2
#define MIFARE_VALUE_S3FPCD_CMD_POS  0
#define MIFARE_VALUE_S3FPCD_ADDR_POS 1


//挂起命令帧
#define MIFARE_HALTA_CMD_LEN        0x02
#define MIFARE_HALTA_CMD_POS        0x00
#define MIFARE_HALTA_PAR_POS        0x01

#define MIFARE_MOBLOCKNUM_MAX            52
#define MIFARE_M1BLOCKNUM_MAX            13

#define MIFARE_MAX_FRAME_LEN             64
#define MIFARE_MAX_UID_LEN               10
#define MIFARE_MAX_ANTCOLL_LOOP          32
#define MIFARE_SIGNAL_UID_LEN            4
#define MIFARE_DOUBLE_UID_LEN            7
#define MIFARE_TRIPLE_UID_LEN            10
#define MIFARE_M1_KEY_LEN                6
#define MIFARE_M1BLOCK_LEN               16
#define MIFARE_M0BLOCK_LEN               4
#define MIFARE_M1VALUE_LEN               4
#define MIFARE_SECTOR_LEN                4
#define MIFARE_NTAGKEY_LEN               4
#define MIFARE_M0CNT_LEN                 3
#define MIFARE_M0SIG_LEN                 32
#define MIFARE_M0PCK_LEN                 2

#define MIFARE_M0U2KEY_LEN               16

#define MIFARE_NORMAL_TIME               10000 //
#define MIFARE_INVENTORY_TIME            10000
#define MIFARE_AUTH_TIME                 10000
#define MIFARE_READ_TIME                 10000
#define MIFARE_WRITE_TIME                10000
#define MIFARE_VALUE_TIME                15000
#define ISO14443A_APDU_TIME              100000



#define ISO14443A_CMD_RATS                  0xE0
#define ISO14443A_CMD_PPS                   0xD0

#define ISO14443A_FSD                       64
#define ISO14443A_FSDI_FSD                  0x80
#define ISO14443A_CID                       0x01


#define ISO14443A4_FSCI_DFT                 2
#define ISO14443A4_FWI_DFT                  4
typedef struct iso14443_4{
    u8 pcb;
    u16 fsc;                                 //tag能够接收的
    u32 fwt;
}ISO14443A_4;


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


#endif

