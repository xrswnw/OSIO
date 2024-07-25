#ifndef _ANYID_CR95HF_
#define _ANYID_CR95HF_

#if defined(_ANYID_HSR101_)
    #include "AnyID_HSR101_CR95HF_HL.h"
#elif defined(_ANYID_M321_)
    #include "AnyID_M321_CR95HF_HL.h"
#endif

#define CR95HF_DUMMY_BYTE					         0xFF


//CR95HF Comm HEADER command definition
#define CR95HF_COMM_CMD_WRITE                       0x00
#define CR95HF_COMM_CMD_RESET                       0x01
#define CR95HF_COMM_CMD_READ                        0x02
#define CR95HF_COMM_CMD_POLLING                     0x03

//polling flag
#define CR95HF_POLLING_WRITE_FLAG                   0x04
#define CR95HF_POLLING_READ_FLAG                    0x08
#define CR95HF_POLLING_DELAY                        10000   //10ms

//CR95HF command definition
#define CR95HF_CMD_IDN                              0x01
#define CR95HF_CMD_SEL_PROTOCOL                     0x02
#define CR95HF_CMD_POLL_FIELD                       0x03
#define CR95HF_CMD_SEND_RECEIVE                     0x04
#define CR95HF_CMD_LISTEN                           0x05
#define CR95HF_CMD_SEND                             0x06
#define CR95HF_CMD_IDLE                             0x07
#define CR95HF_CMD_READ_REG                         0x08
#define CR95HF_CMD_WRITE_REG                        0x09
#define CR95HF_CMD_BAUD_RATE                        0x0A
#define CR95HF_CMD_SUB_FREQ_RES                     0x0B
#define CR95HF_CMD_AC_FILTER                        0x0D
#define CR95HF_CMD_TEST_MODE                        0x0E
#define CR95HF_CMD_SLEEP_MODE                       0x0F
#define CR95HF_CMD_ECHO                             0x55
#define CR95HF_CMD_ECHORESPONSE                     0x55

#define CR95HF_CMD_HEADER_LEN                       0x02

//IDN
#define CR95HF_CMD_IDN_DLEN                         0x00
#define CR95HF_CMD_IDN_RSPCODE                      0x00



#define CR95HF_ERR_SUCCESS                          0x00
#define CR95HF_ERR_RSPCODE                          0x06
#define CR95HF_ERR_TAG                              0x07
#define CR95HF_ERR_PARITY                           0x08
#define CR95HF_ERR_BCC                              0x09
#define CR95HF_ERR_ISO14443A4                       0x0A
#define CR95HF_ERR_SAK                              0x0B
#define CR95HF_ERR_AUTH                             0x0C
#define CR95HF_ERR_TRANS                            0x0D
#define CR95HF_ERR_COLL                             0x0F
#define CR95HF_ERR_OTHER                            0x10
#define CR95HF_ERR_TIMEOUT                          0x11
#define CR95HF_ERR_LENGTH                           0x12
#define CR95HF_ERR_CRC                              0x13

#define CR95HF_RSP_CODE_OK                          0x00
//IDN
#define CR95HF_IDN_RSP_LEN                          0x0F

//send_receive命令的响应
#define CR95HF_SENDRCV_RSPCODE_ERR                  0x00
#define CR95HF_SENDRCV_RSPCODE_BIT                  0x90
#define CR95HF_SENDRCV_RSPCODE_DAT                  0x80
//15693返回标志
#define CR95HF_SENDRCV_15693_ERR_COL                0x01
#define CR95HF_SENDRCV_15693_ERR_CRC                0x02
//14443A请求标志
#define CR95HF_SENDRCV_14443A_FLG_TOPAZ             0x80
#define CR95HF_SENDRCV_14443A_FLG_SPLIT             0x40
#define CR95HF_SENDRCV_14443A_FLG_APPEND_CRC        0x20
#define CR95HF_SENDRCV_14443A_FLG_PARITY_MOD        0x10
//14443A返回标志
#define CR95HF_SENDRCV_14443A_RSPTRIL_LEN           0x03 
#define CR95HF_SENDRCV_14443A_ERR_OK                0x00
#define CR95HF_SENDRCV_14443A_ERR_COL               0x80
#define CR95HF_SENDRCV_14443A_ERR_CRC               0x20
#define CR95HF_SENDRCV_14443A_ERR_PRITY             0x10
#define CR95HF_SENDRCV_14443A_ERR_BITMASK           0x07


//protocol allowed
#define CR95HF_PROTOCOL_FIELDOFF                    0x00
#define CR95HF_PROTOCOL_ISO15693                    0x01
#define CR95HF_PROTOCOL_ISO14443A                   0x02
#define CR95HF_PROTOCOL_ISO14443B                   0x03
#define CR95HF_PROTOCOL_ISO18092                    0x04

//15693
#define CR95HF_ISO15693_RATA_26                     0x00
#define CR95HF_ISO15693_RATA_52                     0x10
#define CR95HF_ISO15693_RATA_6                      0x20
#define CR95HF_ISO15693_DELAY_312US                 0x00
#define CR95HF_ISO15693_WAIT_SOF                    0x08
#define CR95HF_ISO15693_MODL_100                    0x00
#define CR95HF_ISO15693_MODL_10                     0x04
#define CR95HF_ISO15693_SINGLE_SUB                  0x00
#define CR95HF_ISO15693_DOUBLE_SUB                  0x02
#define CR95HF_ISO15693_NO_CRC                      0x00
#define CR95HF_ISO15693_APPEND_CRC                  0x01

//14443a
#define CR95HF_ISO14443A_TXRATE_106                 0x00
#define CR95HF_ISO14443A_TXRATE_212                 0x40
#define CR95HF_ISO14443A_RXRATE_106                 0x00
#define CR95HF_ISO14443A_RXRATE_212                 0x10



// CR95HF Command Frame Offset definitions
#define CR95HF_FRAME_COMMAND_OFFSET                 0x00
#define CR95HF_FRAME_LENGTH_OFFSET                  0x01
#define CR95HF_FRAME_DATA_OFFSET                    0x02


typedef struct cr95hf_SendFrame{
    union {u8 cmd; u8 code;}param;
    u8 len;
    u8 frame[CR95HF_FRAME_MAX_LEN];
}CR95HF_FRAME;

extern CR95HF_FRAME g_sCr95HfFrame;

void CR95HF_SendCommand(CR95HF_FRAME *pFrame);
u8 CR95HF_Polling(u8 flag, u16 timeout);
u8 CR95HF_ReadData(CR95HF_FRAME *pFrame);
void CR95HF_Reset(void);
u8 CR95HF_RfOff(void);
u8 CR95HF_ReadVersion(CR95HF_FRAME *pFrame);
u8 CR95HF_SendReceive(CR95HF_FRAME *pFrame, u16 timeout);
u8 CR95HF_Iso15693CheckResponse(CR95HF_FRAME *pFrame);
u8 CR95HF_Iso14443ACheckResponse(CR95HF_FRAME *pFrame);



#endif
