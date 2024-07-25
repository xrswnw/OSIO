#ifndef _ANYID_FM17660_
#define _ANYID_FM17660_

#include "AnyID_M345_FM17660_HL.h"

#define FM17660_PROTOCOL_CONFIG_SIZE      8
extern const u8 g_sFM17660RegAdd[FM17660_PROTOCOL_CONFIG_SIZE];

#define FM17660_READ_FLAG             0x01
#define FM17660_WRITE_FLAG            0xFE
#define FM17660_RADDR(addr)           (FM17660_READ_FLAG | (addr << 1))
#define FM17660_WADDR(addr)           (FM17660_WRITE_FLAG & (addr << 1))

#define FM17660_OpenAntenna(protocol)     do{\
                                              FM17660_Reset(); \
                                              ((protocol & READER_PROTOCOL_MASK) == READER_PROTOCOL_ISO15693) ? ISO15693_Init() : ISO14443A_Init(); \
                                              FM17660_ClearRegBit(FM17660_REG_COMMAND, FM17660_BIT_MODEMOFF); \
                                              FM17660_SetRegBit(FM17660_REG_TXMODE, FM17660_BIT_TXEN); \
                                          }while(0)
                                              
#define FM17660_CloseAntenna()        do{\
                                          FM17660_SetRegBit(FM17660_REG_COMMAND, FM17660_BIT_MODEMOFF); \
                                          FM17660_ClearRegBit(FM17660_REG_TXMODE, FM17660_BIT_TXEN); \
                                      }while(0)

#define FM17660_FlushFifo()           FM17660_SetRegBit(FM17660_REG_FIFOCONTROL, FM17660_BIT_FLUSHFIFO)

//清空所有中断标志
#define FM17660_ClearAllIrq0Flag()    FM17660_WriteReg(FM17660_REG_IRQ0, 0x7F)
//最高位是0，则清空中断标志
#define FM17660_ClearIrq0Flag(f)      FM17660_SetRegBit(FM17660_REG_IRQ0, f)

#define FM17660_SetTxLastBits(b)      do{\
                                        u8 reg = 0;\
                                        reg = FM17660_ReadReg(FM17660_REG_TXDATANUM) & (~FM17660_MASK_TXLASTBITS);\
                                        reg |= (FM17660_MASK_TXLASTBITS & (b));\
                                        FM17660_WriteReg(FM17660_REG_TXDATANUM, reg);\
                                    }while(0)


void FM17660_WriteReg(u8 addr, u8 val);
u8 FM17660_ReadReg(u8 addr);
void FM17660_SetRegBit(u8 addr, u8 bit);
void FM17660_SetRegBits(u8 addr, u8 msk, u8 bit);
void FM17660_ClearRegBit(u8 addr, u8 bit);
void FM17660_ReadFIFO(u8 *pBuffer, u8 len);
void FM17660_WriteFIFO(u8 *pBuffer, u8 len);
void FM17660_ClearFIFO(void);
void FM17660_Config(u8 bClearCrypt, u8 bEnTxCrc, u8 bEnRxCrc, u8 txLastBits, u8 rxAlignBits);

#define FM17660_RXINF_LEN           2
#define FM17660_BUFFER_POS_RXLEN    0
#define FM17660_BUFFER_POS_RXLBIT   1

BOOL FM17660_Reset(void);
u8 FM17660_Init(void);
u8 FM17660_SelectProtocol(u8 tx, u8 rx);
u8 FM17660_Command(void);
u8 FM17660_TopazCommand(void);
u8 FM17660_ReadProductID(void);

#define FM17660_SIZE_FIFO               256
#define FM17660_FIFO_LEVEL              64
// Command Register.
#define FM17660_REG_COMMAND              0x00
// Host-Control Register.
#define FM17660_REG_HOSTCTRL             0x01
// FIFO-Control Register.
#define FM17660_REG_FIFOCONTROL          0x02
// WaterLevel Register.
#define FM17660_REG_WATERLEVEL           0x03
// FIFO-Length Register.
#define FM17660_REG_FIFOLENGTH           0x04
// FIFO-Data Register.
#define FM17660_REG_FIFODATA             0x05
// IRQ0 Register.
#define FM17660_REG_IRQ0                 0x06
// IRQ1 Register.
#define FM17660_REG_IRQ1                 0x07
// IRQ0EN Register.
#define FM17660_REG_IRQ0EN               0x08
// IRQ1EN Register.
#define FM17660_REG_IRQ1EN               0x09
// Error Register.
#define FM17660_REG_ERROR                0x0A
// Status Register.
#define FM17660_REG_STATUS               0x0B
// Rx-Bit-Control Register.
#define FM17660_REG_RXBITCTRL            0x0C
// Rx-Coll Register.
#define FM17660_REG_RXCOLL               0x0D
// Timer Control Register.
#define FM17660_REG_TCONTROL             0x0E
// Timer0 Control Register.
#define FM17660_REG_T0CONTROL            0x0F
// Timer0 Reload(High) Register.
#define FM17660_REG_T0RELOADHI           0x10
// Timer0 Reload(Low) Register.
#define FM17660_REG_T0RELOADLO           0x11
// Timer0 Counter(High) Register.
#define FM17660_REG_T0COUNTERVALHI       0x12
// Timer0 Counter(Low) Register.
#define FM17660_REG_T0COUNTERVALLO       0x13
// Timer1 Control Register.
#define FM17660_REG_T1CONTROL            0x14
// Timer1 Reload(High) Register.
#define FM17660_REG_T1RELOADHI           0x15
// Timer1 Reload(Low) Register.
#define FM17660_REG_T1RELOADLO           0x16
// Timer1 Counter(High) Register.
#define FM17660_REG_T1COUNTERVALHI       0x17
// Timer1 Counter(Low) Register.
#define FM17660_REG_T1COUNTERVALLO       0x18
// Timer2 Control Register.
#define FM17660_REG_T2CONTROL            0x19
// Timer2 Reload(High) Register.
#define FM17660_REG_T2RELOADHI           0x1A
// Timer2 Reload(Low) Register.
#define FM17660_REG_T2RELOADLO           0x1B
// Timer2 Counter(High) Register.
#define FM17660_REG_T2COUNTERVALHI       0x1C
// Timer2 Counter(Low) Register.
#define FM17660_REG_T2COUNTERVALLO       0x1D
// Timer3 Control Register.
#define FM17660_REG_T3CONTROL            0x1E
// Timer3 Reload(High) Register.
#define FM17660_REG_T3RELOADHI           0x1F
// Timer3 Reload(Low) Register.
#define FM17660_REG_T3RELOADLO           0x20
// Timer3 Counter(High) Register.
#define FM17660_REG_T3COUNTERVALHI       0x21
// Timer3 Counter(Low) Register.
#define FM17660_REG_T3COUNTERVALLO       0x22
// Timer4 Control Register.
#define FM17660_REG_T4CONTROL            0x23
// Timer4 Reload(High) Register.
#define FM17660_REG_T4RELOADHI           0x24
// Timer4 Reload(Low) Register.
#define FM17660_REG_T4RELOADLO           0x25
// Timer4 Counter(High) Register.
#define FM17660_REG_T4COUNTERVALHI       0x26
// Timer4 Counter(Low) Register.
#define FM17660_REG_T4COUNTERVALLO       0x27
// Tx Mode Register.
#define FM17660_REG_TXMODE               0x28
// Tx Amplifier Register.
#define FM17660_REG_TXAMP                0x29
// Tx Control Register.
#define FM17660_REG_TXCON                0x2A
// TxI Register.
#define FM17660_REG_TXI                  0x2B
// Tx-CRC Control Register.
#define FM17660_REG_TXCRCCON             0x2C
// Rx-CRC Control Register.
#define FM17660_REG_RXCRCCON             0x2D
// Tx-DataNum Register.
#define FM17660_REG_TXDATANUM            0x2E
// Transmitter Data Modulation Wideband Register.
#define FM17660_REG_TXMODWIDTH           0x2F
// TxSymBurstLen Register.
#define FM17660_REG_TXSYM10BURSTLEN      0x30
// Tx-Wait Control Register.
#define FM17660_REG_TXWAITCTRL           0x31
// TxWaitLo Register.
#define FM17660_REG_TXWAITLO             0x32
// Frame control register.
#define FM17660_REG_FRAMECON             0x33
// RxSOFD Register.
#define FM17660_REG_RXSOFD               0x34
// Rx Control Register.
#define FM17660_REG_RXCTRL               0x35
// Rx-Wait Register.
#define FM17660_REG_RXWAIT               0x36
// RxThreshold Register.
#define FM17660_REG_RXTHRESHOLD          0x37
// Receiver Register.
#define FM17660_REG_RCV                  0x38
// Rx-Analog Register.
#define FM17660_REG_RXANA                0x39
// LPCD-OPTIONS Register.
#define FM17660_REG_LPCD_OPTIONS         0x3A
// Serial Speed Register.
#define FM17660_REG_SERIALSPEED          0x3B
// LPO_TRIMM Register.
#define FM17660_REG_EMV_TB_WAVECTRL      0x3C
// PLL Control Register.
#define FM17660_REG_CLKOUT_CTRL          0x3D
// LPCD THRESHOLD Register.
#define FM17660_REG_LPCD_THRESHOLD       0x3E
// LPCD_Qmin Register.
#define FM17660_REG_LPCD_QMIN            0x3F
// LPCD Result(I) Register.
#define FM17660_REG_LPCD_RESULT_I        0x42
// LPCD Result(Q) Register.
#define FM17660_REG_LPCD_RESULT_Q        0x43
// PinEn Register.
#define FM17660_REG_PINEN                0x44
// PinOut Register.
#define FM17660_REG_PINOUT               0x45
// PinIn Register.
#define FM17660_REG_PININ                0x46
// SigOut Register.
#define FM17660_REG_SIGOUT               0x47
// Receiver Modulation Register.
#define FM17660_REG_RXMOD                0x5D
// Receiver Correlation Register.
#define FM17660_REG_RXCORR               0x5E
// Noise Threshold Configuration Register.
#define FM17660_REG_THNADJ               0x5F
// Noise Threshold Configuration Register.
#define FM17660_REG_THNSET               0x61
// Minimum Noise Threshold Configuration Register.
#define FM17660_REG_THNMIN               0x62
// Automatic Signal Threshold Sensitivity Register.
#define FM17660_REG_THSAUTOSEN           0x63
// DSP Receive Protocol Channel Configuration Register 1.
#define FM17660_REG_DSP_CTRL1            0x64
// DSP Receive Protocol Channel Configuration Register 2.
#define FM17660_REG_DSP_CTRL2            0x65
// Type A Control Register.
#define FM17660_REG_TYPEACTRL            0x66
// Type B Control Register.
#define FM17660_REG_TYPEBCTRL            0x67
// Rx Supplementary Configuration Register.
#define FM17660_REG_RXSUPCFG             0x6E
// Fires The Extended Configuration Register.
#define FM17660_REG_EXTTXCON             0x6F
// Gets The Field Intensity Amplitude Register.
#define FM17660_REG_GET_AMPLITUDEEXTREG  0x70
// Receive Channel Observation Signal Selection Register.
#define FM17660_REG_RXAUXSEL             0x73
// Receive And Transmit Control Register.
#define FM17660_REG_RXTXCONREG           0x77
// Error Extension Register.
#define FM17660_REG_EXTERROR             0x7E
// Version Register.
#define FM17660_REG_VERSION              0x7F


// Command Register Contents (0x00)
#define FM17660_BIT_STANDBY              0x80U   // Standby bit; If set, the IC transits to standby mode.
#define FM17660_BIT_MODEMOFF             0x40U   // Transceiver controller off control.
#define FM17660_CMD_IDLE                 0x00U   // No action; cancels current command execution.
#define FM17660_CMD_LPCD                 0x01U   // Low Power Card Detection.
#define FM17660_CMD_LOADKEY              0x02U   // Reads a key from the FIFO buffer and puts it into the key buffer.
#define FM17660_CMD_MFAUTHENT            0x03U   // Performs the Mifare standard authentication (in Mifare Reader/Writer mode only).
#define FM17660_CMD_RECEIVE              0x05U   // Activates the receiver circuitry.
#define FM17660_CMD_TRANSMIT             0x06U   // Transmits data from the FIFO buffer to Card.
#define FM17660_CMD_TRANSCEIVE           0x07U   // Like #FM17660_CMD_TRANSMIT but automatically activates the receiver after transmission is finished.
#define FM17660_CMD_WRITEE2              0x08U   // Gets one byte from FIFO buffer and writes it to the internal EEPROM.
#define FM17660_CMD_WRITEE2PAGE          0x09U   // Gets up to 64 Bytes from FIFO buffer and writes it to the EEPROM.
#define FM17660_CMD_READE2               0x0AU   // Reads data from EEPROM and puts it into the FIFO buffer.
#define FM17660_CMD_LOADREG              0x0CU   // Reads data from EEPROM and initializes the registers.
#define FM17660_CMD_LOADPROTOCOL         0x0DU   // Reads data from EEPROM and initializes the registers needed for a protocol change.
#define FM17660_CMD_LOADKEYE2            0x0EU   // Copies a Mifare key from the EEPROM into the key buffer.
#define FM17660_CMD_STOREKEYE2           0x0FU   // Stores a Mifare key into the EEPROM.
#define FM17660_CMD_GETRFAMPLITUDE       0x11U   // Gets current carrier amplitude information.
#define FM17660_CMD_CRCCALC              0x1BU   // CrcCalc.
#define FM17660_CMD_GETRNR               0x1CU   // GetRnr.
#define FM17660_CMD_SOFTRESET            0x1FU   // Resets the IC.
#define FM17660_MASK_COMMAND             0x1FU   // Mask for Command-bits.


// Host-Control Register Contents (0x01)
#define FM17660_BIT_REGEN                0x80U
#define FM17660_BIT_BUSHOST              0x40U
#define FM17660_BIT_BUSSAM               0x20U
#define FM17660_BIT_I2CFORCEHS           0x01U
#define FM17660_MASK_SAMINTERFACE        0x0CU

// FIFO-Control Register Contents (0x02)
#define FM17660_BIT_FIFOSIZE             0x80U
#define FM17660_BIT_HIALERT              0x40U
#define FM17660_BIT_LOALERT              0x20U
#define FM17660_BIT_FLUSHFIFO            0x10U
#define FM17660_BIT_WATERLEVEL_HI        0x04U
#define FM17660_MASK_FIFOLENGTH_HI       0x03U

// IRQ0 Register(s) Contents (0x06/0x08)
#define FM17660_BIT_SET                  0x80U
#define FM17660_BIT_IRQINV               0x80U
#define FM17660_BIT_HIALERTIRQ           0x40U
#define FM17660_BIT_LOALERTIRQ           0x20U
#define FM17660_BIT_IDLEIRQ              0x10U
#define FM17660_BIT_TXIRQ                0x08U
#define FM17660_BIT_RXIRQ                0x04U
#define FM17660_BIT_ERRIRQ               0x02U
#define FM17660_BIT_SOFIRQ               0x01U

// IRQ1 Register(s) Contents (0x07/0x09)
#define FM17660_BIT_IRQPUSHPULL          0x80U
#define FM17660_BIT_GLOBALIRQ            0x40U
#define FM17660_BIT_IRQPINEN             0x40U
#define FM17660_BIT_LPCDIRQ              0x20U
#define FM17660_BIT_TIMER4IRQ            0x10U
#define FM17660_BIT_TIMER3IRQ            0x08U
#define FM17660_BIT_TIMER2IRQ            0x04U
#define FM17660_BIT_TIMER1IRQ            0x02U
#define FM17660_BIT_TIMER0IRQ            0x01U

// Error Register Contents (0x0A)
#define FM17660_BIT_EE_ERR               0x80U
#define FM17660_BIT_FIFOWRERR            0x40U
#define FM17660_BIT_FIFOOVL              0x20U
#define FM17660_BIT_MINFRAMEERR          0x10U
#define FM17660_BIT_NODATAERR            0x08U
#define FM17660_BIT_COLLDET              0x04U
#define FM17660_BIT_PROTERR              0x02U
#define FM17660_BIT_INTEGERR             0x01U

// Status Register Contents (0x0B)
#define FM17660_BIT_CRYPTO1ON            0x20U
#define FM17660_MASK_COMMSTATE           0x07U

// Rx-Bit-Control Register Contents (0x0C)
#define FM17660_BIT_VALUESAFTERCOLL      0x80U
#define FM17660_BIT_NOCOLL               0x08U
#define FM17660_MASK_RXALIGN             0x70U
#define FM17660_MASK_RXLASTBITS          0x07U

// Rx-Coll Register Contents (0x0D)
#define FM17660_BIT_COLLPOSVALID         0x80U
#define FM17660_MASK_COLLPOS             0x7FU

// Timer-Control Register Contents (0x0E)
#define FM17660_BIT_T3RUNNING            0x80U
#define FM17660_BIT_T2RUNNING            0x40U
#define FM17660_BIT_T1RUNNING            0x20U
#define FM17660_BIT_T0RUNNING            0x10U
#define FM17660_BIT_T3STARTSTOPNOW       0x08U
#define FM17660_BIT_T2STARTSTOPNOW       0x04U
#define FM17660_BIT_T1STARTSTOPNOW       0x02U
#define FM17660_BIT_T0STARTSTOPNOW       0x01U

// T[0-3]-Control Register Contents (0x0F/0x14/0x19/0x1E)
#define FM17660_BIT_TSTOP_RX             0x80U   // Stop timer on receive interrupt.
#define FM17660_BIT_TAUTORESTARTED       0x08U   // Auto-restart timer after underflow.
#define FM17660_BIT_TSTART_TX            0x10U   // Start timer on transmit interrupt.
#define FM17660_BIT_TSTART_LFO           0x20U   // Use this timer for LFO trimming.
#define FM17660_BIT_TSTART_LFO_UV        0x30U   // Use this timer for LFO trimming (generate UV at a trimming event).
#define FM17660_MASK_TSTART              0x30U   // Mask for TSTART bits.
#define FM17660_VALUE_TCLK_1356_MHZ      0x00U   // Use 13.56MHz as input clock.
#define FM17660_VALUE_TCLK_212_KHZ       0x01U   // Use 212KHz as input clock.
#define FM17660_VALUE_TCLK_T0            0x02U   // Use timer0 as input clock.
#define FM17660_VALUE_TCLK_T1            0x03U   // Use timer1 as input clock.

// T4-Control Register Contents (0x23)
#define FM17660_BIT_T4RUNNING            0x80U
#define FM17660_BIT_T4STARTSTOPNOW       0x40U
#define FM17660_BIT_T4AUTOTRIMM          0x20U
#define FM17660_BIT_T4AUTOLPCD           0x10U
#define FM17660_BIT_T4AUTORESTARTED      0x08U
#define FM17660_BIT_T4AUTOWAKEUP         0x04U
#define FM17660_MASK_TSTART              0x30U
#define FM17660_VALUE_TCLK_LFO_64_KHZ    0x00U
#define FM17660_VALUE_TCLK_LFO_8_KHZ     0x01U
#define FM17660_VALUE_TCLK_LFO_4_KHZ     0x02U
#define FM17660_VALUE_TCLK_LFO_2_KHZ     0x03U

// Driver Mode Register Contents (0x28)
#define FM17660_BIT_TX2INV               0x80U
#define FM17660_BIT_TX1INV               0x40U
#define FM17660_BIT_TXEN                 0x08U
#define FM17660_VALUE_TXCLKMODE_HIGHIMPEDANCE    0x00U
#define FM17660_VALUE_TXCLKMODE_OUTPULL0         0x01U
#define FM17660_VALUE_TXCLKMODE_OUTPULL1         0x02U
#define FM17660_VALUE_TXCLKMODE_RFLOWPULL        0x05U
#define FM17660_VALUE_TXCLKMODE_RFHIGHPUSH       0x06U
#define FM17660_VALUE_TXCLKMODE_PUSHPULL         0x07U
#define FM17660_BIT_RFON                 0x04U
#define FM17660_BIT_TPUSHON              0x02U
#define FM17660_BIT_TPULLON              0x01U

// Tx Amplifier Register Contents (0x29)
#define FM17660_MASK_CW_AMPLITUDE        0xC0U
#define FM17660_MASK_RESIDUAL_CARRIER    0x1FU

// Driver Control Register Contents (0x2A)
#define FM17660_BIT_CWMAX                0x08U
#define FM17660_BIT_DRIVERINV            0x04U
#define FM17660_VALUE_DRIVERSEL_LOW      0x00U
#define FM17660_VALUE_DRIVERSEL_TXENV    0x01U
#define FM17660_VALUE_DRIVERSEL_SIGIN    0x02U

// Tx-/Rx-CRC Control Register Contents (0x2C/0x2D)
#define FM17660_BIT_RXFORCECRCWRITE      0x80U
#define FM17660_BIT_CRCINVERT            0x02U
#define FM17660_BIT_CRCEN                0x01U
#define FM17660_MASK_CRCPRESETVAL        0x70U
#define FM17660_MASK_CRCTYPE             0x0CU
#define FM17660_MASK_CRCTYPE5            0x00U
#define FM17660_MASK_CRCTYPE16           0x08U

// Tx-DataNum Register Contents (0x2E)
#define FM17660_BIT_KEEPBITGRID          0x10U
#define FM17660_BIT_DATAEN               0x08U
#define FM17660_MASK_TXLASTBITS          0x07U
#define FM17660_MASK_SYMBOL_SEND         0x08U

// Tx-Wait Control Register Contents (0x31)
#define FM17660_BIT_TXWAIT_START_RX      0x80U
#define FM17660_BIT_TXWAIT_DBFREQ        0x40U
#define FM17660_MASK_TXWAITHI            0x38U
#define FM17660_MASK_TXSTOPBITLEN        0x07U

// Frame Control Register Contents (0x33)
#define FM17660_BIT_TXPARITYEN           0x80U
#define FM17660_BIT_RXPARITYEN           0x40U
#define FM17660_VALUE_STOP_SYM3          0x0CU
#define FM17660_VALUE_STOP_SYM2          0x08U
#define FM17660_VALUE_STOP_SYM1          0x04U
#define FM17660_VALUE_START_SYM2         0x03U
#define FM17660_VALUE_START_SYM1         0x02U
#define FM17660_VALUE_START_SYM0         0x01U
#define FM17660_MASK_STARTSYM            0x03U
#define FM17660_MASK_STOPSYM             0x0CU

// sof detect Register Contents (0x34)
#define FM17660_BIT_SOF_EN               0x20U
#define FM17660_BIT_SUBC_EN              0x04U

// Rx Control Register Contents (0x35)
#define FM17660_BIT_RXALLOWBITS          0x80U
#define FM17660_BIT_RXMULTIPLE           0x40U
#define FM17660_BIT_RXEOFTYPE            0x20U
#define FM17660_BIT_EGT_CHECK            0x10U
#define FM17660_BIT_EMD_SUPPRESSION      0x08U
#define FM17660_MASK_RXBAUDRATE          0x07U

// Rx-Wait Register Contents (0x36)
#define FM17660_BIT_RXWAITDBFREQ         0x80U
#define FM17660_MASK_RXWAIT              0x7FU

// Rx-Threshold Register Contents (0x37)
#define FM17660_MASK_MINLEVEL            0xF0U
#define FM17660_MASK_MINLEVELP           0x0FU

// Rx-Receiver Register Contents (0x38)
#define FM17660_BIT_RX_SINGLE            0x80U
#define FM17660_BIT_RX_SHORT_MIX2ADC     0x40U
#define FM17660_BIT_USE_SMALL_EVAL       0x04U
#define FM17660_MASK_RX_SIGPRO_IN_SEL    0x30U
#define FM17660_MASK_COLLLEVEL           0x03U

// Rx-Analog Register Contents (0x39)
#define FM17660_BIT_RX_OC_ENABLE         0x20U
#define FM17660_BIT_RX_HP_LOWF           0x10U
#define FM17660_MASK_VMID_R_SEL          0xC0U
#define FM17660_MASK_RCV_HPCF            0x0CU
#define FM17660_MASK_RCV_GAIN            0x03U

// Serial-Speed Register Contents (0x3B)
#define FM17660_MASK_BR_T0               0xE0U
#define FM17660_MASK_BR_T1               0x1FU

// LPCD Result(Q) Register Contents (0x43)
#define FM17660_BIT_LPCDIRQ_CLR          0x40U

// Tx-BitMod Register Contents (0x48)
#define FM17660_BIT_TXMSBFIRST           0x80U
#define FM17660_BIT_TXPARITYTYPE         0x20U
#define FM17660_BIT_TXSTOPBITTYPE        0x08U
#define FM17660_BIT_TXSTARTBITTYPE       0x02U
#define FM17660_BIT_TXSTARTBITEN         0x01U

// Rx-BitMod Register Contents (0x58)
#define FM17660_BIT_RXSTOPONINVPAR       0x20U
#define FM17660_BIT_RXSTOPONLEN          0x10U
#define FM17660_BIT_RXMSBFIRST           0x08U
#define FM17660_BIT_RXSTOPBITEN          0x04U
#define FM17660_BIT_RXPARITYTYPE         0x02U

// Rx-Mod Register Contents (0x5D)
#define FM17660_BIT_PREFILTER            0x20U
#define FM17660_BIT_RECTFILTER           0x10U
#define FM17660_BIT_SYNCHIGH             0x08U
#define FM17660_BIT_CORRINV              0x04U
#define FM17660_BIT_FSK                  0x02U
#define FM17660_BIT_BPSK                 0x01U

// RxSupCfg Register Contents (0x6E)
#define FM17660_BIT_RXNOERR              0x80U

// RxTxConReg Register Contents (0x77)
#define FM17660_BIT_SHMODE               0x08U


#define FM17660_RXTX_I14443A_106          0x00U   // ISO14443A Operating mode at 106kbit/s.
#define FM17660_RXTX_I14443A_212          0x01U   // ISO14443A Operating mode at 212kbit/s.
#define FM17660_RXTX_I14443A_424          0x02U   // ISO14443A Operating mode at 414kbit/s.
#define FM17660_RXTX_I14443A_848          0x03U   // ISO14443A Operating mode at 848kbit/s.

#define FM17660_RXTX_I14443B_106          0x04U   // ISO14443B Operating mode at 106kbit/s.
#define FM17660_RXTX_I14443B_212          0x05U   // ISO14443B Operating mode at 212kbit/s.
#define FM17660_RXTX_I14443B_424          0x06U   // ISO14443B Operating mode at 414kbit/s.
#define FM17660_RXTX_I14443B_848          0x07U   // ISO14443B Operating mode at 848kbit/s.

#define FM17660_RXTX_FELICA_212           0x13U   // FeliCa Operating mode at 212kbit/s.
#define FM17660_RXTX_FELICA_424           0x14U   // FeliCa Operating mode at 424kbit/s.

#define FM17660_TX_I15693_1OO4            0x0AU   // ISO15693 One-Out-Of-Four Operating mode.
#define FM17660_TX_I15693_1OO256          0x0CU   // ISO15693 One-Out-Of-256 Operating mode.
#define FM17660_RX_I15693_HIGH_SSC        0x0AU   // ISO15693 High-Speed (RX) Operating mode (Single Subcarrier).
#define FM17660_RX_I15693_FAST            0x0BU   // ISO15693 Fast-Speed (RX) Operating mode (Single Subcarrier).
#define FM17660_RX_I15693_HIGH_DSC        0x0CU   // ISO15693 High-Speed (RX) Operating mode (Dual Subcarrier).

#define FM17660_RXTX_EPC_UID              0x0DU   // ICode EPC/UID.

#define FM17660_RX_I18000P3M3_DS_M2       0x0EU   // ICode ISO18000-3 Mode3 424 kBit/s (M=2).
#define FM17660_RX_I18000P3M3_DS_M4       0x0FU   // ICode ISO18000-3 Mode3 424 kBit/s (M=4).
#define FM17660_RX_I18000P3M3_QS_M2       0x10U   // ICode ISO18000-3 Mode3 848 kBit/s (M=2).
#define FM17660_RX_I18000P3M3_QS_M4       0x11U   // ICode ISO18000-3 Mode3 848 kBit/s (M=4).

#define FM17660_TX_I18000P3M3_ASK         0x0EU


#define FM17660_STAT_OK                0x00   //正确
#define FM17660_STAT_INIT_ERROR        0x01   //系统初始化错误
#define FM17660_STAT_CMD_ERROR         0x02   //错误的RC522命令
#define FM17660_STAT_TIMEOUT           0x03   //RC522命令执行超时
#define FM17660_STAT_COLLISION_ERROR   0x04   //碰撞错误 STATUS_RC522_TEMP_ERROR
#define FM17660_STAT_PARITY_ERROR      0x05   //校验错误
#define FM17660_STAT_PROTOCOL_ERROR    0x06   //协议错误
#define FM17660_STAT_FIFOOF_ERROR      0x07   //FIFO溢出
#define FM17660_STAT_CRC_ERROR         0x08   //CRC错误
#define FM17660_STAT_TEMP_ERROR        0x09   //温度过高
#define FM17660_STAT_WFIFO_ERROR       0x0A   //写FIFO错误
#define FM17660_STAT_ACK               0x0B   //ASK
#define FM17660_STAT_ERROR             0x0C   //错误
#define FM17660_STAT_PICC_TYPE_ERROR   0x0D   //PICC类型不存在
#define FM17660_STAT_AUTH_ERROR        0x0F   //授权认证错误
#define FM17660_STAT_TRANS_ERROR       0x10   //传输错误
#define FM17660_STAT_BCC_ERROR         0x11   //BCC错误
#define FM17660_STAT_NAK               0x12   //BCC错误
#define FM17660_STAT_AUTHM0U2_ERR      0x13   //ultralightC授权错误
#define FM17660_STAT_HANDLE_ERR        0x14   //ultralightC授权错误

#define FM17660_PRODUCTID_663          0x01
#define FM17660_PRODUCTID_631          0xC0
#define FM17660_PRODUCTID_630          0x80
#define FM17660_PRODUCTID_610          0x20

#ifdef _ANYID_STM8_
#define FM17660_FRAME_LEN                  FM17660_SIZE_FIFO
#else
#define FM17660_FRAME_LEN                  255
#endif

typedef struct FM17660_FRAME{
    u8 cmd;
#ifdef _ANYID_STM8_
    u8 frame[FM17660_FRAME_LEN];
#else
    u8 frame[FM17660_FRAME_LEN];
#endif
    u8 txLen;
    u8 rxLen;
    u32 timeout;
}FM17660_FRAME;
extern FM17660_FRAME g_sFm17660Frame;


#define FM17660_RX_FRAME_TIMEOUT      1000        //1000*16us = 16ms
#define FM17660_TX_FRAME_TIMEOUT      1000
#endif