#ifndef _ANYID_ISO14443A_
#define _ANYID_ISO14443A_

#define ISO14443A_PICC_ACTIVATE_EN        	1       // 激活处于天线感应区的卡片
#define ISO14443A_PCD_RFRESET_EN          	1       // RF场复位(关闭载波)


//Mifare 命令
#define ISO14443A_CMD_IDLE             0x00   //idle command
#define ISO14443A_CMD_AUTHENT_A        0x60   //AUTHENT A command
#define ISO14443A_CMD_AUTHENT_B        0x61   //AUTHENT B command
#define ISO14443A_CMD_READ             0x30   //READ command
#define ISO14443A_CMD_WRITE16          0xA0   //WRITE 16 bytes command
#define ISO14443A_CMD_WRITE4           0xA2   //WRITE 4 bytes command
#define ISO14443A_CMD_INCREMENT        0xC1   //INCREMENT command
#define ISO14443A_CMD_DECREMENT        0xC0   //DECREMENT command
#define ISO14443A_CMD_RESTORE          0xC2   //RESTORE command
#define ISO14443A_CMD_TRANSFER         0xB0   //TRANSFER command
#define ISO14443A_CMD_NOCOMMAND        0x00   //VOID command (no MIFARE command)
#define ISO14443A_CMD_HALTA            0x50   //将卡挂起
#define ISO14443A_CMD_REQALL           0x52    // 唤醒命令
#define ISO14443A_CMD_REQIDL           0x26    // 请求命令
#define ISO14443A_CMD_ANTICOLL_L1      0x93    // 防碰撞/选择等级1
#define ISO14443A_CMD_ANTICOLL_L2      0x95    // 防碰撞/选择等级2
#define ISO14443A_CMD_ANTICOLL_L3      0x97    // 防碰撞/选择等级3


#define ISO14443A_EXPECT_TIMEOUT       0x01   //Tells the library to expect a timeout
#define ISO14443A_EXPECT_ACK           0x02   //Let the library expect an Acknowledge response
#define ISO14443A_EXPECT_DATA          0x04   //The library shall expect data  

#define ISO14443A_REQUEST_BITS         0x07    // 请求命令的位数


//防碰撞/选择命令参数定义
#define ISO14443A_BITS_PER_BYTE        0x08    //一个字节8位
#define ISO14443A_UPPER_NVB_SHIFT      0x04    //NVB高位移动位数，高4位表示有效字节数，低4位表示最后一个字节(不包含在有效字节中)有效位数
#define ISO14443A_COMPLETE_UID_BITS    0x28
#define ISO14443A_NVB_MIN_PARAMETER    0x20    //NVB最小值，只有CMD和NVB两个字节
#define ISO14443A_NVB_MAX_PARAMETER    0x70    //NVB最大值，CMD NVB UID(4B) BCC

//
#define ISO14443A_MAX_ANTICOLL_LEVELS  0x03    // 最大防碰撞等级
#define ISO14443A_SINGLE_UID_LENGTH    0x20     //uid共4字节，32位
#define ISO14443A_COLL_BIT             0x04

//选择命令回应的字节数
#define ISO14443A_SAK_LENGTH           0x01

//挂起命令参数定义
#define ISO14443A_HALTA_PARAM          0x00

//cascade tag
#define ISO14443A_CT                   0x88

//卡类型
#define ISO14443A_TYPE_MF0_U1          0x0044
#define ISO14443A_TYPE_MF1_S50         0x0004
#define ISO14443A_TYPE_MF1_S70         0x0002
#define ISO14443A_TYPE_MF3_D40         0x0344
#define ISO14443A_TYPE_PROX            0x0008

#define ISO14443A_MF0_U1_ADDR_MAX      0x0F        //512bits, 16Block, 4Bytes pre block
#define ISO14443A_MF1_S50_ADDR_MAX     0x3F        //8Kbits,   64Block, 16Bytes pre block


#define ISO14443A_UID_MAX_LEN          10
#define ISO14443A_UID_L1_LEN           4
#define ISO14443A_UID_L2_LEN           7
#define ISO14443A_UID_L3_LEN           10


//帧信息
//帧长度，这里全部是标准帧，如果有CRC-A的，帧长度不包括CRC的长度
//请求帧格式
#define ISO14443A_REQUEST_LEN          1   //请求帧长度
#define ISO14443A_ATQA_LEN             2   //请求帧响应长度
#define ISO14443A_REQUEST_CMD_POS      0



//接收到的ATL(防碰撞)命令帧格式
#define ISO14443A_ATL_FRM_FPCD_LEN     2   //接收来之PCD的防碰撞帧
#define ISO14443A_ATL_FPCD_CMD_POS     0
#define ISO14443A_ATL_FPCD_NVB_POS     1
//响应PCD的ATL(防碰撞)命令帧格式
#define ISO14443A_ATL_FRM_TPCD_LEN     5   //响应PCD的防碰撞帧
#define ISO14443A_ATL_TPCD_CT_POS      0
#define ISO14443A_ATL_TPCD_BCC_POS     4

#define ISO14443A_ATL_TPCD_CT_LEN      1
#define ISO14443A_ATL_TPCD_UID_LEN     4



//来之PCD的SEL命令帧格式
#define ISO14443A_SEL_FRM_FPCD_LEN     7   //接收来之PCD的选择帧
#define ISO14443A_SEL_FPCD_CMD_POS     0
#define ISO14443A_SEL_FPCD_NVB_POS     1
#define ISO14443A_SEL_FPCD_UID_POS     2
#define ISO14443A_SEL_FPCD_BCC_POS     6
#define ISO14443A_SEL_FPCD_UID_LEN     4
#define ISO14443A_SAK_FRM_TPCD_LEN     1   //响应PCD的SEL命令


//AUTH命令帧格式
#define ISO14443A_AUTH_FRM_FPCD_LEN    12
#define ISO14443A_AUTH_FPCD_CMD_POS    0
#define ISO14443A_AUTH_FPCD_ADDR_POS   1
#define ISO14443A_AUTH_FPCD_PWD_POS    2
#define ISO14443A_AUTH_FPCD_UID_POS    8
#define ISO14443A_AUTH_FPCD_PWD_LEN    6
#define ISO14443A_AUTH_FPCD_UID_LEN    4


//读写命令的应答帧格式
#define ISO14443A_ACK_MASK             0x0F
#define ISO14443A_ACK_OK               0x0A
#define ISO14443A_ACK_NOT_ALLOW        0x04
#define ISO14443A_ACK_TRS_ERROR        0x05


//Read命令帧格式
#define ISO14443A_READ_FRM_FPCD_LEN    2
#define ISO14443A_READ_FPCD_CMD_POS    0
#define ISO14443A_READ_FPCD_ADDR_POS   1
#define ISO14443A_READ_FRM_TPCD_LEN    16
#define ISO14443A_READ_TPCD_DATA_POS   0

//Write4命令帧格式
#define ISO14443A_WRITE4_FRM_FPCD_LEN  6
#define ISO14443A_WRITE4_FPCD_CMD_POS  0
#define ISO14443A_WRITE4_FPCD_ADDR_POS 1
#define ISO14443A_WRITE4_FPCD_DATA_POS 2
#define ISO14443A_WRITE4_FPCD_DATA_LEN 4

//Write16命令帧格式
#define ISO14443A_WRITE16_S1FRM_FPCD_LEN  2
#define ISO14443A_WRITE16_S1FPCD_CMD_POS  0
#define ISO14443A_WRITE16_S1FPCD_ADDR_POS 1
#define ISO14443A_WRITE16_S2FRM_FPCD_LEN  16
#define ISO14443A_WRITE16_S2FPCD_DATA_POS 0
#define ISO14443A_WRITE16_S2FPCD_DATA_LEN 16

//value命令帧格式
#define ISO14443A_VALUE_S1FRM_FPCD_LEN  2
#define ISO14443A_VALUE_S1FPCD_CMD_POS  0
#define ISO14443A_VALUE_S1FPCD_ADDR_POS 1
#define ISO14443A_VALUE_S2FRM_FPCD_LEN  4
#define ISO14443A_VALUE_S2FPCD_VAL_POS  0
#define ISO14443A_VALUE_S3FRM_FPCD_LEN  2
#define ISO14443A_VALUE_S3FPCD_CMD_POS  0
#define ISO14443A_VALUE_S3FPCD_ADDR_POS 1


//挂起命令帧
#define ISO14443A_HALTA_CMD_LEN        0x02
#define ISO14443A_HALTA_CMD_POS        0x00
#define ISO14443A_HALTA_PAR_POS        0x01




#endif

