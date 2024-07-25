#ifndef _ANYID_RC630_
#define _ANYID_RC630_

#if defined(_ANYID_FR322_)
   #include "AnyID_M345_RC630_HL.h"
#elif defined (_ANYID_FR321AP_)
    #include "AnyID_M345_Rc630_HL.h"
#elif defined (_ANYID_FR321A_)
    #include "AnyID_M345_Rc630_HL.h"
#endif




#define RC630_PROTOCOL_CONFIG_SIZE      15
extern const u8 g_sRc630RegAdd[RC630_PROTOCOL_CONFIG_SIZE];

#define RC630_READ_FLAG             0x01
#define RC630_WRITE_FLAG            0xFE
#define RC630_RADDR(addr)           (RC630_READ_FLAG | (addr << 1))
#define RC630_WADDR(addr)           (RC630_WRITE_FLAG & (addr << 1))

#define RC630_OpenAntenna()         RC630_SetRegBit(RC630_REG_DRVMODE,   RC630_BIT_TXEN)
#define RC630_CloseAntenna()        RC630_ClearRegBit(RC630_REG_DRVMODE, RC630_BIT_TXEN)

#define RC630_FlushFifo()           RC630_SetRegBit(RC630_REG_FIFOCONTROL, RC630_BIT_FLUSHFIFO)

//清空所有中断标志
#define RC630_ClearAllIrq0Flag()    RC630_WriteReg(RC630_REG_IRQ0, 0x7F)
//最高位是0，则清空中断标志
#define RC630_ClearIrq0Flag(f)      RC630_SetRegBit(RC630_REG_IRQ0, f)

#define RC630_SetTxLastBits(b)      do{\
                                        u8 reg = 0;\
                                        reg = RC630_ReadReg(RC630_REG_TXDATANUM) & (~RC630_MASK_TXLASTBITS);\
                                        reg |= (RC630_MASK_TXLASTBITS & (b));\
                                        RC630_WriteReg(RC630_REG_TXDATANUM, reg);\
                                    }while(0)


void RC630_WriteReg(u8 addr, u8 val);
u8 RC630_ReadReg(u8 addr);
void RC630_SetRegBit(u8 addr, u8 bit);
void RC630_ClearRegBit(u8 addr, u8 bit);
void RC630_ReadFIFO(u8 *pBuffer, u8 len);
void RC630_WriteFIFO(u8 *pBuffer, u8 len);
void RC630_Config(u8 bClearCrypt, u8 bEnTxCrc, u8 bEnRxCrc, u8 txLastBits, u8 rxAlignBits);

#define RC630_RXINF_LEN           2
#define RC630_BUFFER_POS_RXLEN    0
#define RC630_BUFFER_POS_RXLBIT   1

BOOL RC630_Reset(void);
u8 RC630_Init(void);
u8 RC630_SelectProtocol(u8 tx, u8 rx);
u8 RC630_Command(void);
u8 RC630_TopazCommand(void);

#define RC630_SIZE_FIFO               256
#define RC630_FIFO_LEVEL              64
// Command Register
#define RC630_REG_COMMAND              0x00
// Host-Control Register.
#define RC630_REG_HOSTCTRL             0x01
// FIFO-Control Register.
#define RC630_REG_FIFOCONTROL          0x02
// WaterLevel Register.
#define RC630_REG_WATERLEVEL           0x03
// FIFO-Length Register.
#define RC630_REG_FIFOLENGTH           0x04
// FIFO-Data Register.
#define RC630_REG_FIFODATA             0x05
// IRQ0 Register.
#define RC630_REG_IRQ0                 0x06
// IRQ1 Register.
#define RC630_REG_IRQ1                 0x07
// IRQ0EN Register.
#define RC630_REG_IRQ0EN               0x08
// IRQ1EN Register.
#define RC630_REG_IRQ1EN               0x09
// Error Register.
#define RC630_REG_ERROR                0x0A
// Status Register.
#define RC630_REG_STATUS               0x0B
// Rx-Bit-Control Register.
#define RC630_REG_RXBITCTRL            0x0C
// Rx-Coll Register.
#define RC630_REG_RXCOLL               0x0D
// Timer Control Register.
#define RC630_REG_TCONTROL             0x0E
// Timer0 Control Register.
#define RC630_REG_T0CONTROL            0x0F
// Timer0 Reload(High) Register.
#define RC630_REG_T0RELOADHI           0x10
// Timer0 Reload(Low) Register.
#define RC630_REG_T0RELOADLO           0x11
// Timer0 Counter(High) Register.
#define RC630_REG_T0COUNTERVALHI       0x12
// Timer0 Counter(Low) Register.
#define RC630_REG_T0COUNTERVALLO       0x13
// Timer1 Control Register.
#define RC630_REG_T1CONTROL            0x14
// Timer1 Reload(High) Register.
#define RC630_REG_T1RELOADHI           0x15
// Timer1 Reload(Low) Register.
#define RC630_REG_T1RELOADLO           0x16
// Timer1 Counter(High) Register.
#define RC630_REG_T1COUNTERVALHI       0x17
// Timer1 Counter(Low) Register.
#define RC630_REG_T1COUNTERVALLO       0x18
// Timer2 Control Register.
#define RC630_REG_T2CONTROL            0x19
// Timer2 Reload(High) Register.
#define RC630_REG_T2RELOADHI           0x1A
// Timer2 Reload(Low) Register.
#define RC630_REG_T2RELOADLO           0x1B
// Timer2 Counter(High) Register.
#define RC630_REG_T2COUNTERVALHI       0x1C
// Timer2 Counter(Low) Register.
#define RC630_REG_T2COUNTERVALLO       0x1D
// Timer3 Control Register.
#define RC630_REG_T3CONTROL            0x1E
// Timer3 Reload(High) Register.
#define RC630_REG_T3RELOADHI           0x1F
// Timer3 Reload(Low) Register.
#define RC630_REG_T3RELOADLO           0x20
// Timer3 Counter(High) Register.
#define RC630_REG_T3COUNTERVALHI       0x21
// Timer3 Counter(Low) Register.
#define RC630_REG_T3COUNTERVALLO       0x22
// Timer4 Control Register.
#define RC630_REG_T4CONTROL            0x23
// Timer4 Reload(High) Register.
#define RC630_REG_T4RELOADHI           0x24
// Timer4 Reload(Low) Register.
#define RC630_REG_T4RELOADLO           0x25
// Timer4 Counter(High) Register.
#define RC630_REG_T4COUNTERVALHI       0x26
// Timer4 Counter(Low) Register.
#define RC630_REG_T4COUNTERVALLO       0x27
// Driver Mode Register.
#define RC630_REG_DRVMODE              0x28
// Tx Amplifier Register.
#define RC630_REG_TXAMP                0x29
// Driver Control Register.
#define RC630_REG_DRVCON               0x2A
// TxI Register.
#define RC630_REG_TXI                  0x2B
// Tx-CRC Control Register.
#define RC630_REG_TXCRCCON             0x2C
// Rx-CRC Control Register.
#define RC630_REG_RXCRCCON             0x2D
// Tx-DataNum Register.
#define RC630_REG_TXDATANUM            0x2E
// Tx-Modwidth Register.
#define RC630_REG_TXMODWIDTH           0x2F
// Symbol 0 and 1 Register.
#define RC630_REG_TXSYM10BURSTLEN      0x30
// Tx-Wait Control Register.
#define RC630_REG_TXWAITCTRL           0x31
// TxWaitLo Register.
#define RC630_REG_TXWAITLO             0x32
// Frame control register.
#define RC630_REG_FRAMECON             0x33
// RxSOFD Register.
#define RC630_REG_RXSOFD               0x34
// Rx Control Register.
#define RC630_REG_RXCTRL               0x35
// Rx-Wait Register.
#define RC630_REG_RXWAIT               0x36
// Rx-Threshold Register.
#define RC630_REG_RXTHRESHOLD          0x37
// Receiver Register.
#define RC630_REG_RCV                  0x38
// Rx-Analog Register.
#define RC630_REG_RXANA                0x39
// Serial Speed Register.
#define RC630_REG_SERIALSPEED          0x3B
// LPO_TRIMM Register.
#define RC630_REG_LPO_TRIMM            0x3C
// PLL Control Register.
#define RC630_REG_PLL_CTRL             0x3D
// PLL DivO Register.
#define RC630_REG_PLL_DIV              0x3E
// LPCD QMin Register.
#define RC630_REG_LPCD_QMIN            0x3F
// LPCD QMax Register.
#define RC630_REG_LPCD_QMAX            0x40
// LPCD IMin Register.
#define RC630_REG_LPCD_IMIN            0x41
// LPCD Result(I) Register.
#define RC630_REG_LPCD_RESULT_I        0x42
// LPCD Result(Q) Register.
#define RC630_REG_LPCD_RESULT_Q        0x43
// Transmitter bit modulus Register.
#define RC630_TXBITMOD                 0x48
// Transmitter data configuration register.
#define RC630_REG_TXDATACON            0x4A
// Transmitter data modulation Register.
#define RC630_REG_TXDATAMOD            0x4B
// Transmitter Symbol Frequency.
#define RC630_REG_TXSYMFREQ            0x4C
// Transmitter Symbol 0 High Register.
#define RC630_REG_TXSYM0H              0x4D
// Transmitter Symbol 0 Low Register.
#define RC630_REG_TXSYM0L              0x4E
// Transmitter Symbol 1 High Register.
#define RC630_REG_TXSYM1H              0x4F
// Transmitter Symbol 1 Low Register.
#define RC630_REG_TXSYM1L              0x50
// Tx Symbol 2 Register.
#define RC630_REG_TXSYM2               0x51
// Tx Symbol 3 Register.
#define RC630_REG_TXSYM3               0x52
// Transmitter Symbol 0 + Symbol 1 Length Register.
#define RC630_REG_TXSYM10LEN           0x53
// Transmitter symbol 3 + symbol 2 length register.
#define RC630_REG_TXSYM32LEN           0x54
// Symbol 0 and 1 Register.
#define RC630_REG_TXSYM10BURSTCTRL     0x55
// Symbol 0 and 1 Register.
#define RC630_REG_TXSYM10MOD           0x56
// Transmitter symbol 3 + symbol 2 modulation register.
#define RC630_REG_TXSYM32MOD           0x57
// Receiver bit modulation register.
#define RC630_REG_RXBITMOD             0x58
// Receiver end of frame symbol register.
#define RC630_REG_RXEOFSYM             0x59
// Receiver synchronisation value high register.
#define RC630_REG_RXSYNCVAlH           0x5A
// Receiver synchronisation value low register.
#define RC630_REG_RXSYNCVAlL           0x5B
// Receiver synchronisation mode register.
#define RC630_REG_RXSYNCMOD            0x5C
// Receiver modulation register.
#define RC630_REG_RXMOD                0x5D
// Receiver Correlation Register.
#define RC630_REG_RXCORR               0x5E
// Calibration register of the receiver.
#define RC630_REG_RXSVETTE             0x5F
// DAC value Register.
#define RC630_REG_DACVAL               0x64
// Test mode Register.
#define RC630_REG_TESTMOD              0x66
// Analog Xtal register.
#define RC630_REG_ANAXTAL              0x71
// EPCv2 mode Register.
#define RC630_REG_SIGPROTEST           0x72
// Version Register.
#define RC630_REG_VERSION              0x7F


// Command Register Contents (0x00)
#define RC630_BIT_STANDBY              0x80U   // Standby bit; If set, the IC transits to standby mode.
#define RC630_CMD_IDLE                 0x00U   // No action; cancels current command execution.
#define RC630_CMD_LPCD                 0x01U   // Low Power Card Detection.
#define RC630_CMD_LOADKEY              0x02U   // Reads a key from the FIFO buffer and puts it into the key buffer.
#define RC630_CMD_MFAUTHENT            0x03U   // Performs the Mifare standard authentication (in Mifare Reader/Writer mode only).
#define RC630_CMD_ACKREQ               0x04U   // Performs a Query, a Ack and a Req-Rn for EPC V2.
#define RC630_CMD_RECEIVE              0x05U   // Activates the receiver circuitry.
#define RC630_CMD_TRANSMIT             0x06U   // Transmits data from the FIFO buffer to Card.
#define RC630_CMD_TRANSCEIVE           0x07U   // Like #RC630_CMD_TRANSMIT but automatically activates the receiver after transmission is finished.
#define RC630_CMD_WRITEE2              0x08U   // Gets one byte from FIFO buffer and writes it to the internal EEPROM.
#define RC630_CMD_WRITEE2PAGE          0x09U   // Gets up to 64 Bytes from FIFO buffer and writes it to the EEPROM.
#define RC630_CMD_READE2               0x0AU   // Reads data from EEPROM and puts it into the FIFO buffer.
#define RC630_CMD_LOADREG              0x0CU   // Reads data from EEPROM and initializes the registers.
#define RC630_CMD_LOADPROTOCOL         0x0DU   // Reads data from EEPROM and initializes the registers needed for a protocol change.
#define RC630_CMD_LOADKEYE2            0x0EU   // Copies a Mifare key from the EEPROM into the key buffer.
#define RC630_CMD_STOREKEYE2           0x0FU   // Stores a Mifare key into the EEPROM.
#define RC630_CMD_SOFTRESET            0x1FU   // Resets the IC.
#define RC630_MASK_COMMAND             0x1FU   // Mask for Command-bits.


// Host-Control Register Contents (0x01)
#define RC630_BIT_REGEN                0x80U
#define RC630_BIT_BUSHOST              0x40U
#define RC630_BIT_BUSSAM               0x20U
#define RC630_MASK_SAMINTERFACE        0x0CU

// FIFO-Control Register Contents (0x02)
#define RC630_BIT_FIFOSIZE             0x80U
#define RC630_BIT_HIALERT              0x40U
#define RC630_BIT_LOALERT              0x20U
#define RC630_BIT_FLUSHFIFO            0x10U
#define RC630_BIT_WATERLEVEL_HI        0x04U
#define RC630_MASK_FIFOLENGTH_HI       0x03U

// IRQ0 Register(s) Contents (0x06/0x08)
#define RC630_BIT_SET                  0x80U
#define RC630_BIT_IRQINV               0x80U
#define RC630_BIT_HIALERTIRQ           0x40U
#define RC630_BIT_LOALERTIRQ           0x20U
#define RC630_BIT_IDLEIRQ              0x10U
#define RC630_BIT_TXIRQ                0x08U
#define RC630_BIT_RXIRQ                0x04U
#define RC630_BIT_ERRIRQ               0x02U
#define RC630_BIT_SOFIRQ               0x01U

// IRQ1 Register(s) Contents (0x07/0x09)
#define RC630_BIT_IRQPUSHPULL          0x80U
#define RC630_BIT_GLOBALIRQ            0x40U
#define RC630_BIT_IRQPINEN             0x40U
#define RC630_BIT_LPCDIRQ              0x20U
#define RC630_BIT_TIMER4IRQ            0x10U
#define RC630_BIT_TIMER3IRQ            0x08U
#define RC630_BIT_TIMER2IRQ            0x04U
#define RC630_BIT_TIMER1IRQ            0x02U
#define RC630_BIT_TIMER0IRQ            0x01U

// Error Register Contents (0x0A)
#define RC630_BIT_EE_ERR               0x80U
#define RC630_BIT_FIFOWRERR            0x40U
#define RC630_BIT_FIFOOVL              0x20U
#define RC630_BIT_MINFRAMEERR          0x10U
#define RC630_BIT_NODATAERR            0x08U
#define RC630_BIT_COLLDET              0x04U
#define RC630_BIT_PROTERR              0x02U
#define RC630_BIT_INTEGERR             0x01U

// Status Register Contents (0x0B)
#define RC630_BIT_CRYPTO1ON            0x20U
#define RC630_MASK_COMMSTATE           0x07U

// Rx-Bit-Control Register Contents (0x0C)
#define RC630_BIT_VALUESAFTERCOLL      0x80U
#define RC630_BIT_NOCOLL               0x08U
#define RC630_MASK_RXALIGN             0x70U
#define RC630_MASK_RXLASTBITS          0x07U

// Rx-Coll Register Contents (0x0D)
#define RC630_BIT_COLLPOSVALID         0x80U
#define RC630_MASK_COLLPOS             0x7FU

// Timer-Control Register Contents (0x0E)
#define RC630_BIT_T3RUNNING            0x80U
#define RC630_BIT_T2RUNNING            0x40U
#define RC630_BIT_T1RUNNING            0x20U
#define RC630_BIT_T0RUNNING            0x10U
#define RC630_BIT_T3STARTSTOPNOW       0x08U
#define RC630_BIT_T2STARTSTOPNOW       0x04U
#define RC630_BIT_T1STARTSTOPNOW       0x02U
#define RC630_BIT_T0STARTSTOPNOW       0x01U

// T[0-3]-Control Register Contents (0x0F/0x14/0x19/0x1E)
#define RC630_BIT_TSTOP_RX             0x80U   // Stop timer on receive interrupt.
#define RC630_BIT_TAUTORESTARTED       0x08U   // Auto-restart timer after underflow.
#define RC630_BIT_TSTART_TX            0x10U   // Start timer on transmit interrupt.
#define RC630_BIT_TSTART_LFO           0x20U   // Use this timer for LFO trimming.
#define RC630_BIT_TSTART_LFO_UV        0x30U   // Use this timer for LFO trimming (generate UV at a trimming event).
#define RC630_MASK_TSTART              0x30U   // Mask for TSTART bits.
#define RC630_VALUE_TCLK_1356_MHZ      0x00U   // Use 13.56MHz as input clock.
#define RC630_VALUE_TCLK_212_KHZ       0x01U   // Use 212KHz as input clock.
#define RC630_VALUE_TCLK_T0            0x02U   // Use timer0 as input clock.
#define RC630_VALUE_TCLK_T1            0x03U   // Use timer1 as input clock.

// T4-Control Register Contents (0x23)
#define RC630_BIT_T4RUNNING            0x80U
#define RC630_BIT_T4STARTSTOPNOW       0x40U
#define RC630_BIT_T4AUTOTRIMM          0x20U
#define RC630_BIT_T4AUTOLPCD           0x10U
#define RC630_BIT_T4AUTORESTARTED      0x08U
#define RC630_BIT_T4AUTOWAKEUP         0x04U
#define RC630_MASK_TSTART              0x30U
#define RC630_VALUE_TCLK_LFO_64_KHZ    0x00U
#define RC630_VALUE_TCLK_LFO_8_KHZ     0x01U
#define RC630_VALUE_TCLK_LFO_4_KHZ     0x02U
#define RC630_VALUE_TCLK_LFO_2_KHZ     0x03U

// Driver Mode Register Contents (0x28)
#define RC630_BIT_TX2INV               0x80U
#define RC630_BIT_TX1INV               0x40U
#define RC630_BIT_TXEN                 0x08U
#define RC630_BIT_RFON                 0x04U
#define RC630_BIT_TPUSHON              0x02U
#define RC630_BIT_TPULLON              0x01U

// Tx Amplifier Register Contents (0x29)
#define RC630_MASK_CW_AMPLITUDE        0xC0U
#define RC630_MASK_RESIDUAL_CARRIER    0x1FU

// Driver Control Register Contents (0x2A)
#define RC630_BIT_CWMAX                0x10U
#define RC630_BIT_DISOVSHTPREV         0x08U
#define RC630_BIT_DRIVERINV            0x04U
#define RC630_VALUE_DRIVERSEL_LOW      0x00U
#define RC630_VALUE_DRIVERSEL_TXENV    0x01U
#define RC630_VALUE_DRIVERSEL_SIGIN    0x02U

// Tx-/Rx-CRC Control Register Contents (0x2C/0x2D)
#define RC630_BIT_RXFORCECRCWRITE      0x80U
#define RC630_BIT_CRCINVERT            0x02U
#define RC630_BIT_CRCEN                0x01U
#define RC630_MASK_CRCPRESETVAL        0x70U
#define RC630_MASK_CRCTYPE             0x0CU

// Tx-DataNum Register Contents (0x2E)
#define RC630_BIT_KEEPBITGRID          0x10U
#define RC630_BIT_DATAEN               0x08U
#define RC630_MASK_TXLASTBITS          0x07U

// Tx-Wait Control Register Contents (0x31)
#define RC630_BIT_TXWAIT_START_RX      0x80U
#define RC630_BIT_TXWAIT_DBFREQ        0x40U
#define RC630_MASK_TXWAITHI            0x38U
#define RC630_MASK_TXSTOPBITLEN        0x07U

// Frame Control Register Contents (0x33)
#define RC630_BIT_TXPARITYEN           0x80U
#define RC630_BIT_RXPARITYEN           0x40U
#define RC630_VALUE_STOP_SYM3          0x0CU
#define RC630_VALUE_STOP_SYM2          0x08U
#define RC630_VALUE_STOP_SYM1          0x04U
#define RC630_VALUE_START_SYM2         0x03U
#define RC630_VALUE_START_SYM1         0x02U
#define RC630_VALUE_START_SYM0         0x01U
#define RC630_MASK_STARTSYM            0x03U
#define RC630_MASK_STOPSYM             0x0CU

// sof detect Register Contents (0x34)
#define RC630_BIT_SOF_EN               0x20U
#define RC630_BIT_SUBC_EN              0x04U

// Rx Control Register Contents (0x35)
#define RC630_BIT_RXALLOWBITS          0x80U
#define RC630_BIT_RXMULTIPLE           0x40U
#define RC630_BIT_RXEOFTYPE            0x20U
#define RC630_BIT_EGT_CHECK            0x10U
#define RC630_BIT_EMD_SUPPRESSION      0x08U
#define RC630_MASK_RXBAUDRATE          0x07U

// Rx-Wait Register Contents (0x36)
#define RC630_BIT_RXWAITDBFREQ         0x80U
#define RC630_MASK_RXWAIT              0x7FU

// Rx-Threshold Register Contents (0x37)
#define RC630_MASK_MINLEVEL            0xF0U
#define RC630_MASK_MINLEVELP           0x0FU

// Rx-Receiver Register Contents (0x38)
#define RC630_BIT_RX_SINGLE            0x80U
#define RC630_BIT_RX_SHORT_MIX2ADC     0x40U
#define RC630_BIT_USE_SMALL_EVAL       0x04U
#define RC630_MASK_RX_SIGPRO_IN_SEL    0x30U
#define RC630_MASK_COLLLEVEL           0x03U

// Rx-Analog Register Contents (0x39)
#define RC630_BIT_RX_OC_ENABLE         0x20U
#define RC630_BIT_RX_HP_LOWF           0x10U
#define RC630_MASK_VMID_R_SEL          0xC0U
#define RC630_MASK_RCV_HPCF            0x0CU
#define RC630_MASK_RCV_GAIN            0x03U

// Serial-Speed Register Contents (0x3B)
#define RC630_MASK_BR_T0               0xE0U
#define RC630_MASK_BR_T1               0x1FU

// LPCD Result(Q) Register Contents (0x43)
#define RC630_BIT_LPCDIRQ_CLR          0x40U

// Tx-BitMod Register Contents (0x48)
#define RC630_BIT_TXMSBFIRST           0x80U
#define RC630_BIT_TXPARITYTYPE         0x20U
#define RC630_BIT_TXSTOPBITTYPE        0x08U
#define RC630_BIT_TXSTARTBITTYPE       0x02U
#define RC630_BIT_TXSTARTBITEN         0x01U

// Rx-BitMod Register Contents (0x58)
#define RC630_BIT_RXSTOPONINVPAR       0x20U
#define RC630_BIT_RXSTOPONLEN          0x10U
#define RC630_BIT_RXMSBFIRST           0x08U
#define RC630_BIT_RXSTOPBITEN          0x04U
#define RC630_BIT_RXPARITYTYPE         0x02U

// Rx-Mod Register Contents (0x5D)
#define RC630_BIT_PREFILTER            0x20U
#define RC630_BIT_RECTFILTER           0x10U
#define RC630_BIT_SYNCHIGH             0x08U
#define RC630_BIT_CORRINV              0x04U
#define RC630_BIT_FSK                  0x02U
#define RC630_BIT_BPSK                 0x01U


#define RC630_RXTX_I14443A_106          0x00U   // ISO14443A Operating mode at 106kbit/s.
#define RC630_RXTX_I14443A_212          0x01U   // ISO14443A Operating mode at 212kbit/s.
#define RC630_RXTX_I14443A_424          0x02U   // ISO14443A Operating mode at 414kbit/s.
#define RC630_RXTX_I14443A_848          0x03U   // ISO14443A Operating mode at 848kbit/s.


#define RC630_STAT_OK                0x00   //正确
#define RC630_STAT_INIT_ERROR        0x01   //系统初始化错误
#define RC630_STAT_CMD_ERROR         0x02   //错误的RC522命令
#define RC630_STAT_TIMEOUT           0x03   //RC522命令执行超时
#define RC630_STAT_COLLISION_ERROR   0x04   //碰撞错误 STATUS_RC522_TEMP_ERROR
#define RC630_STAT_PARITY_ERROR      0x05   //校验错误
#define RC630_STAT_PROTOCOL_ERROR    0x06   //协议错误
#define RC630_STAT_FIFOOF_ERROR      0x07   //FIFO溢出
#define RC630_STAT_CRC_ERROR         0x08   //CRC错误
#define RC630_STAT_TEMP_ERROR        0x09   //温度过高
#define RC630_STAT_WFIFO_ERROR       0x0A   //写FIFO错误
#define RC630_STAT_ACK               0x0B   //ASK
#define RC630_STAT_ERROR             0x0C   //错误
#define RC630_STAT_PICC_TYPE_ERROR   0x0D   //PICC类型不存在
#define RC630_STAT_AUTH_ERROR        0x0F   //授权认证错误
#define RC630_STAT_TRANS_ERROR       0x10   //传输错误
#define RC630_STAT_BCC_ERROR         0x11   //BCC错误
#define RC630_STAT_NAK               0x12   //BCC错误
#define RC630_STAT_AUTHM0U2_ERR      0x13   //ultralightC授权错误
#define RC630_STAT_HANDLE_ERR        0x14   //ultralightC授权错误

#ifdef _ANYID_STM8_
#define RC630_FRAME_LEN                  RC630_SIZE_FIFO
#else
#define RC630_FRAME_LEN                  255
#endif

typedef struct RC630_FRAME{
    u8 cmd;
#ifdef _ANYID_STM8_
    u8 frame[RC630_FRAME_LEN];
#else
    u8 frame[RC630_FRAME_LEN];
#endif
    u8 txLen;
    u8 rxLen;
    u32 timeout;
}RC630_FRAME;
extern RC630_FRAME g_sRc630Frame;


#define RC630_RX_FRAME_TIMEOUT      1000        //1000*16us = 16ms
#define RC630_TX_FRAME_TIMEOUT      1000
#endif


