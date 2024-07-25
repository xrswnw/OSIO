#ifndef _ANYID_TRF796X_
#define _ANYID_TRF796X_

#ifdef _ANYID_X321_  
    #include "AnyID_R322_Trf796x_HL.h"
#elif (defined(_ANYID_FR321_) || defined(_ANYID_FR322_)  || defined(_ANYID_D321M_) || defined(_ANYID_FR321P_) || defined(_ANYID_FR321A_) || defined(_ANYID_FR321AP_) || defined(_ANYID_D321M_) || defined(_ANYID_M326X_))
    #include "AnyID_M345_Trf796x_HL.h"
#elif (defined(_ANYID_D321_) || defined(_ANYID_D320S_))
    #include "AnyID_M345_Trf796x_HL.h"
#endif

#define TRF796X_PWR_LV_0                            0
#define TRF796X_PWR_LV_1                            1
#define TRF796X_PWR_LV_MAX                          1

#define TRF796X_START_FLAG_CMD                      0x80
#define TRF796X_START_FLAG_READ                     0x40
#define TRF796X_START_FLAG_CONT                     0x20
#define TRF796X_START_CMD_MSK                       0x1F
#define TRF796X_START_ADDR_MSK                      0x1F

#define TRF796X_CMD_IDLE                            0x00
#define TRF796X_CMD_SOFT_INIT                       0x03
#define TRF796X_CMD_INITIAL_RF_COLLISION            0x04
#define TRF796X_CMD_RESPONSE_RF_COLLISION_N         0x05
#define TRF796X_CMD_RESPONSE_RF_COLLISION_0         0x06
#define TRF796X_CMD_RESET_FIFO                      0x0F
#define TRF796X_CMD_TRANSMIT_NO_CRC                 0x10
#define TRF796X_CMD_TRANSMIT_CRC                    0x11
#define TRF796X_CMD_DELAY_TRANSMIT_NO_CRC           0x12
#define TRF796X_CMD_DELAY_TRANSMIT_CRC              0x13
#define TRF796X_CMD_TRANSMIT_NEXT_SLOT              0x14
#define TRF796X_CMD_CLOSE_SLOT_SEQUENCE             0x15
#define TRF796X_CMD_STOP_DECODERS                   0x16
#define TRF796X_CMD_ENABLE_DECODERS                 0x17
#define TRF796X_CMD_CHECK_INTERNAL_RF               0x18
#define TRF796X_CMD_CHECK_EXTERNAL_RF               0x19
#define TRF796X_CMD_ADJUST_GAIN                     0x1A

//---- Reader register ------------------------------------------

#define TRF796X_REG_CHIP_STATE_CONTROL              0x00
#define TRF796X_REG_ISO_CONTROL                     0x01
#define TRF796X_REG_ISO_14443B_OPTIONS              0x02
#define TRF796X_REG_ISO_14443A_OPTIONS              0x03
#define TRF796X_REG_TX_TIMER_EPC_HIGH               0x04
#define TRF796X_REG_TX_TIMER_EPC_LOW                0x05
#define TRF796X_REG_TX_PULSE_LENGTH_CONTROL         0x06
#define TRF796X_REG_RX_NO_RESPONSE_WAIT_TIME        0x07
#define TRF796X_REG_RX_WAIT_TIME                    0x08    //从发送的EOF开始，rxWaitTime时间不需要解析接收信号，避免在raWaitTime时间里面有干扰信号
#define TRF796X_REG_MODULATOR_CONTROL               0x09    
#define TRF796X_REG_RX_SPECIAL_SETTINGS             0x0A
#define TRF796X_REG_REGULATOR_CONTROL               0x0B
#define TRF796X_REG_IRQ_STATUS                      0x0C    // IRQ Status Register
#define TRF796X_REG_IRQ_MASK                        0x0D    // Collision Position and Interrupt Mask Register
#define TRF796X_REG_COLLISION_POSITION              0x0E
#define TRF796X_REG_RSSI_LEVELS                     0x0F
#define TRF796X_REG_SPECIAL_FUNCTION                0x10
#define TRF796X_REG_RAM_START_ADDRESS               0x11    //RAM is 6 bytes long (0x11 - 0x16)
#define TRF796X_REG_NFC_LOW_DETECTION               0x16
#define TRF796X_REG_NFCID                           0x17
#define TRF796X_REG_NFC_TARGET_LEVEL                0x18
#define TRF796X_REG_NFC_TARGET_PROTOCOL             0x19
#define TRF796X_REG_TEST_SETTINGS_1                 0x1A
#define TRF796X_REG_TEST_SETTINGS_2                 0x1B
#define TRF796X_REG_FIFO_CONTROL                    0x1C
#define TRF796X_REG_FIFO_TX                         0x1D
#define TRF796X_REG_TX_LENGTH_BYTE_2                0x1E
#define TRF796X_REG_FIFO                            0x1F

//===============================================================

#define TRF796X_REG_CTRL_STANDBY                    0x80
#define TRF796X_REG_CTRL_DIRMOD                     0x40
#define TRF796X_REG_CTRL_RFON                       0x20
#define TRF796X_REG_CTRL_HALFPWR                    0x10
#define TRF796X_REG_CTRL_RXIN2                      0x08
#define TRF796X_REG_CTRL_EXFIDTEST                  0x02
#define TRF796X_REG_CTRL_5V                         0x01

#define TRF796X_REG_ISO_RXNOCRC                     0x80
#define TRF796X_REG_ISO_DIRMOD1                     0x40
#define TRF796X_REG_ISO_MSK                         0x1F

#define TRF796X_REG_ISO15693_MSK                    0x07
#define TRF796X_REG_ISO15693_1OF256                 0x01    //
#define TRF796X_REG_ISO15693_HIBR                   0x02    //高速
#define TRF796X_REG_ISO15693_DOUBLESUB              0x04    //双副载波

#define TRF796X_REG_ISO14443A_106K                  0x08
#define TRF796X_REG_ISO14443A_212K                  0x09
#define TRF796X_REG_ISO14443A_424K                  0x0A
#define TRF796X_REG_ISO14443A_848K                  0x0B

#define TRF796X_REG_CRYTCLK_27MHZ                   0x80
#define TRF796X_REG_ASK_EXEN                        0x40
#define TRF796X_REG_SYSCLK_DISABLE                  0x00
#define TRF796X_REG_SYSCLK_6P78                     0x10
#define TRF796X_REG_SYSCLK_13P56                    0x20
#define TRF796X_REG_SYSCLK_27P12                    0x30
#define TRF796X_REG_P12_ENANA                       0x08
#define TRF796X_REG_ASK_MSK                         0x07
#define TRF796X_REG_ASK_10                          0x00
#define TRF796X_REG_ASK_100                         0x01
#define TRF796X_REG_ASK_7                           0x02
#define TRF796X_REG_ASK_8P5                         0x03
#define TRF796X_REG_ASK_13                          0x04
#define TRF796X_REG_ASK_16                          0x05
#define TRF796X_REG_ASK_22                          0x06
#define TRF796X_REG_ASK_30                          0x07

#define TRF796X_REG_RXBW_212                        0x80    //felic
#define TRF796X_REG_RXBW_424                        0x40    //15693
#define TRF796X_REG_RXBW_848                        0x20    //14443-106
#define TRF796X_REG_RXBW_HBT                        0x10    //14443高速848Kbps
#define TRF796X_REG_RXG_REDUMSK                     0x1C
#define TRF796X_REG_RXG_REDU00                      0x00
#define TRF796X_REG_RXG_REDU05                      0x04
#define TRF796X_REG_RXG_REDU10                      0x08
#define TRF796X_REG_RXG_REDU15                      0x0C

#define TRF796X_REG_AUTO_REGULATOR                  0x80
#define TRF796X_REG_VOL_5_3P4_3P4                   0x07

#define TRF796X_REG_IRQ_TX_END                      0x80
#define TRF796X_REG_IRQ_RX_START                    0x40
#define TRF796X_REG_IRQ_FIFO_LV                     0x20
#define TRF796X_REG_IRQ_CRC_ERR                     0x10
#define TRF796X_REG_IRQ_PARITY_ERR                  0x08
#define TRF796X_REG_IRQ_EOF_ERR                     0x04
#define TRF796X_REG_IRQ_COLLS_ERR                   0x02
#define TRF796X_REG_IRQ_NORSP                       0x01
#define TRF796X_REG_ERR_MSK                         0x1F

#define TRF796X_REG_FIFO_RXLEN_MSK                  0x7F

#define TRF796X_REG_RSSI_CRYOK                      0x40
#define TRF796X_REG_RSSI_IN2MSK                     0x38
#define TRF796X_REG_RSSI_IN1MSK                     0x07

#define TRF796X_REG_DISABLE_PARITY                  0x20
#define TRF796X_REG_SLOT_GRID_37P77                 0x10
#define TRF796X_REG_RX_4BITS                        0x04
#define TRF796X_REG_NO_BROKEN_FRAM                  0x02
#define TRF796X_REG_NO_RSP                          0x01

#define TRF796X_REG_BROKEN_FLAG                     0x01

#define TRF796X_RX_WAIT_MAX                         255
#define TRF796X_RX_WAIT_UNIT                        37      //37.76

#define TRF796X_ERR_OK                              0x00
#define TRF796X_ERR_TAG                             0x01
#define TRF796X_ERR_CRC                             0x02
#define TRF796X_ERR_NORSP                           0x03
#define TRF796X_ERR_OTHER                           0x04
#define TRF796X_ERR_COLLS                           0x05

void Trf796x_WriteCmd(u8 cmd);
void Trf796x_WriteReg(u8 addr, u8 reg);
u8 Trf796x_ReadReg(u8 addr);
void Trf796x_ReadRegs(u8 addr, u8 len, u8 *pBuffer);
void Trf796x_WriteRegs(u8 addr, u8 len, u8 *pBuffer);
void Trf796x_ClearRegBits(u8 addr, u8 msk);
void Trf796x_SetRegBits(u8 addr, u8 msk, u8 bitValue);
void Trf796x_SetRegBit(u8 addr, u8 bitValue);
void Trf796x_ClearIrqFlag(void); 

u8 Trf796x_Init(u8 pwrLv);
void Trf796x_SoftReset(void);

void Trf796x_RfClose(void);
void Trf796x_RfOpen(void);
void Trf796x_EnableFullPower(void);
void Trf796x_EnableHalfPower(void);

u8 Trf796x_FmtErrInfo(u8 irq);

void Trf796x_TransFifo(u8 *pFrame, u16 txByteLen, u8 lastBytBitNum, BOOL bTxCrc);
u8 Trf796x_TransceiverFrame(TRF796X_FRAME *pTrf76xFrame);
#endif
