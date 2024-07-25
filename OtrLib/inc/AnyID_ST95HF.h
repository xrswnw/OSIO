#ifndef _ANYID_ST95HF_
#define _ANYID_ST95HF_

#if defined(_ANYID_HSR101_)
    #include "AnyID_HSR101_ST95HF_HL.h"
#elif defined(_ANYID_M321_)
    #include "AnyID_M321_ST95HF_HL.h"
#elif defined(_ANYID_R322_)
    #include "AnyID_R322_ST95HF_HL.h"
#elif defined(_ANYID_T320_)
    #include "AnyID_T320_ST95HF_HL.h"
#elif defined(_ANYID_R323_)
    #include "AnyID_R323_ST95HF_HL.h"
#elif defined(_ANYID_R327_)
    #include "AnyID_R327_ST95HF_HL.h"
#elif defined(_ANYID_M328_)
    #include "AnyID_M328_ST95HF_HL.h"
#endif

#define ST95HF_DUMMY_BYTE					        0xFF


//ST95HF Comm HEADER command definition
#define ST95HF_COMM_CMD_WRITE                       0x00
#define ST95HF_COMM_CMD_RESET                       0x01
#define ST95HF_COMM_CMD_READ                        0x02
#define ST95HF_COMM_CMD_POLLING                     0x03
#define ST95HF_COMM_CMD_LISTEN                      0x05
#define ST95HF_COMM_CMD_TRANS                       0x06
#define ST95HF_COMM_CMD_ACFILTER                    0x0D
#define ST95HF_COMM_CMD_ECHO                        0x55

//polling flag
#define ST95HF_POLLING_WRITE_FLAG                   0x04
#define ST95HF_POLLING_READ_FLAG                    0x08
#define ST95HF_POLLING_DELAY                        10000   //10ms



//ST95HF command definition
#define ST95HF_CMD_IDN                              0x01
#define ST95HF_CMD_SEL_PROTOCOL                     0x02
#define ST95HF_CMD_POLL_FIELD                       0x03
#define ST95HF_CMD_SEND_RECEIVE                     0x04
#define ST95HF_CMD_LISTEN                           0x05
#define ST95HF_CMD_SEND                             0x06
#define ST95HF_CMD_IDLE                             0x07
#define ST95HF_CMD_READ_REG                         0x08
#define ST95HF_CMD_WRITE_REG                        0x09
#define ST95HF_CMD_BAUD_RATE                        0x0A
#define ST95HF_CMD_SUB_FREQ_RES                     0x0B
#define ST95HF_CMD_AC_FILTER                        0x0D
#define ST95HF_CMD_TEST_MODE                        0x0E
#define ST95HF_CMD_SLEEP_MODE                       0x0F
#define ST95HF_CMD_ECHO                             0x55
#define ST95HF_CMD_ECHORESPONSE                     0x55



#define ST95HF_ERR_SUCCESS                          0x00
#define ST95HF_ERR_RSPCODE                          0x06
#define ST95HF_ERR_TAG                              0x07
#define ST95HF_ERR_PARITY                           0x08
#define ST95HF_ERR_BCC                              0x09
#define ST95HF_ERR_ISO14443A4                       0x0A
#define ST95HF_ERR_NAK                              0x0B
#define ST95HF_ERR_AUTH                             0x0C
#define ST95HF_ERR_TRANS                            0x0D
#define ST95HF_ERR_COLL                             0x0F
#define ST95HF_ERR_OTHER                            0x10
#define ST95HF_ERR_TIMEOUT                          0x11
#define ST95HF_ERR_LENGTH                           0x12
#define ST95HF_ERR_CRC                              0x13
#define ST95HF_ERR_RFOFF                            0x14
#define ST95HF_ERR_15693_COL                        0x15


//command resp code
#define ST95HF_RSP_CODE_OK                          0x00
//IDN
#define ST95HF_IDN_RSP_LEN                          0x0F
//send_receive命令的响应
#define ST95HF_SENDRCV_RSPCODE_ERR                  0x00
#define ST95HF_SENDRCV_RSPCODE_BIT                  0x90
#define ST95HF_SENDRCV_RSPCODE_DAT                  0x80
//15693返回标志
#define ST95HF_SENDRCV_15693_ERR_COL                0x01
#define ST95HF_SENDRCV_15693_ERR_CRC                0x02
//14443A请求标志
#define ST95HF_SENDRCV_14443A_FLG_TOPAZ             0x80
#define ST95HF_SENDRCV_14443A_FLG_SPLIT             0x40
#define ST95HF_SENDRCV_14443A_FLG_APPEND_CRC        0x20
#define ST95HF_SENDRCV_14443A_FLG_PARITY_MOD        0x10
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT1          0x01
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT2          0x02
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT3          0x03
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT4          0x04
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT5          0x05
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT6          0x06
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT7          0x07
#define ST95HF_SENDRCV_14443A_FLG_LSB_BIT8          0x08
//14443A返回标志
#define ST95HF_SENDRCV_14443A_RSPTRIL_LEN           0x03
#define ST95HF_SENDRCV_14443A_ERR_OK                0x00
#define ST95HF_SENDRCV_14443A_ERR_COL               0x80
#define ST95HF_SENDRCV_14443A_ERR_CRC               0x20
#define ST95HF_SENDRCV_14443A_ERR_PRITY             0x10
#define ST95HF_SENDRCV_14443A_ERR_BITMASK           0x07
//18092返回标志
#define ST95HF_SENDRCV_18092_ERR_CRC                0x02
//14443B返回标志
#define ST95HF_SENDRCV_14443B_ERR_CRC               0x02


//Listen response code
#define ST95HF_LISTEN_IVLD_LEN                      0x82
#define ST95HF_LISTEN_IVLD_PROTOCOL                 0x83
#define ST95HF_LISTEN_RF_OFF                        0x8F


//protocol allowed
#define ST95HF_PROTOCOL_FIELDOFF                    0x00
#define ST95HF_PROTOCOL_ISO15693                    0x01
#define ST95HF_PROTOCOL_ISO14443A                   0x02
#define ST95HF_PROTOCOL_ISO14443B                   0x03
#define ST95HF_PROTOCOL_ISO18092                    0x04

//14443a params
#define ST95HF_ISO14443A_TXRATE_106                 0x00
#define ST95HF_ISO14443A_TXRATE_212                 0x40
#define ST95HF_ISO14443A_RXRATE_106                 0x00
#define ST95HF_ISO14443A_RXRATE_212                 0x10

//14443b params
#define ST95HF_ISO14443B_TXRATE_106                 0x00
#define ST95HF_ISO14443B_TXRATE_212                 0x40
#define ST95HF_ISO14443B_TXRATE_424                 0x80
#define ST95HF_ISO14443B_TXRATE_848                 0xC0
#define ST95HF_ISO14443B_RXRATE_106                 0x00
#define ST95HF_ISO14443B_RXRATE_212                 0x10
#define ST95HF_ISO14443B_RXRATE_424                 0x20
#define ST95HF_ISO14443B_RXRATE_848                 0x30
#define ST95HF_ISO14443B_APPEND_CRC                 0x01

//18092 params
#define ST95HF_ISO18092_TXRATE_424                  0x80
#define ST95HF_ISO18092_TXRATE_212                  0x40
#define ST95HF_ISO18092_RXRATE_424                  0x20
#define ST95HF_ISO18092_RXRATE_212                  0x10
#define ST95HF_ISO18092_APPEND_CRC                  0x01
//slot1需要等待2.4ms；其他每增加一个slot，增加1.2ms
#define ST95HF_ISO18092_FWT_2P4MS                   0x00 //slot1
#define ST95HF_ISO18092_FWT_PPMM                    0x01 //
#define ST95HF_ISO18092_SLOT_MASK                   0x0F //1~16SLOT
#define ST95HF_ISO18092_SLOT_1                      0x00
#define ST95HF_ISO18092_SLOT_2                      0x01
#define ST95HF_ISO18092_SLOT_3                      0x02
#define ST95HF_ISO18092_SLOT_4                      0x03
#define ST95HF_ISO18092_SLOT_5                      0x04
#define ST95HF_ISO18092_SLOT_6                      0x05
#define ST95HF_ISO18092_SLOT_7                      0x06
#define ST95HF_ISO18092_SLOT_8                      0x07
#define ST95HF_ISO18092_SLOT_9                      0x08
#define ST95HF_ISO18092_SLOT_10                     0x09
#define ST95HF_ISO18092_SLOT_11                     0x0A
#define ST95HF_ISO18092_SLOT_12                     0x0B
#define ST95HF_ISO18092_SLOT_13                     0x0C
#define ST95HF_ISO18092_SLOT_14                     0x0D
#define ST95HF_ISO18092_SLOT_15                     0x0E
#define ST95HF_ISO18092_SLOT_16                     0x0F

//15693 protocol params
#define ST95HF_ISO15693_RATA_26                     0x00
#define ST95HF_ISO15693_RATA_52                     0x10
#define ST95HF_ISO15693_RATA_6                      0x20
#define ST95HF_ISO15693_DELAY_312US                 0x00
#define ST95HF_ISO15693_WAIT_SOF                    0x08
#define ST95HF_ISO15693_MODL_100                    0x00
#define ST95HF_ISO15693_MODL_10                     0x04
#define ST95HF_ISO15693_SINGLE_SUB                  0x00
#define ST95HF_ISO15693_DOUBLE_SUB                  0x02
#define ST95HF_ISO15693_NO_CRC                      0x00
#define ST95HF_ISO15693_APPEND_CRC                  0x01



void ST95HF_WriteFrame(ST95HF_FRAME *pFrame);
u8 ST95HF_Polling(u8 flag, u32 timeout);
u8 ST95HF_ReadFrame(ST95HF_FRAME *pFrame);
void ST95HF_Reset(void);

u8 ST95HF_Command(ST95HF_FRAME *pFrame);
u8 ST95HF_Receive(ST95HF_FRAME *pFrame);
u8 ST95HF_Transmit(ST95HF_FRAME *pFrame);
void ST95HF_CloseRf(ST95HF_FRAME *pFrame);
void ST95HF_OpenRf(ST95HF_FRAME *pFrame);

u8 ST95HF_DeviceCheckResponse(ST95HF_FRAME *pFrame);
u8 ST95HF_Iso15693CheckResponse(ST95HF_FRAME *pFrame);
u8 ST95HF_Iso14443ACheckResponse(ST95HF_FRAME *pFrame);
u8 ST95HF_Iso14443BCheckResponse(ST95HF_FRAME *pFrame);
u8 ST95HF_Iso18092CheckResponse(ST95HF_FRAME *pFrame);
u8 ST95HF_Echo(ST95HF_FRAME *pFrame);
#endif
