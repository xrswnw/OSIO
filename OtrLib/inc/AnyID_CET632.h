#ifndef _ANYID_CET632_
#define _ANYID_CET632_

#if defined(_ANYID_R321_)
    #include "AnyID_R321_CET632_HL.h"
#endif

#define CET632_FRAME_MAX_LEN        256
typedef struct cet632Frame{
    u8 rxLen;
    u8 rxFrame[CET632_FRAME_MAX_LEN];
    u8 txLen;
    u8 txFrame[CET632_FRAME_MAX_LEN];
}CET632_FRAME;
extern CET632_FRAME g_sCET632Frame;

#define CET632_TYPE_ISO15693    0x00
#define CET632_TYPE_ISO14443A   0x10
#define CET632_TYPE_ISO14443B   0x20


#define CET632_SIZE_FIFO        64              //FIFO深度
#define CET632_ADDR_MASK        0x3F            //最多64个寄存器

//************ Register addr
//Page0
#define CET632_REG_CMD          0x01
#define CET632_REG_FIFO         0x02
#define CET632_REG_PRIMSTATUS   0x03
#define CET632_REG_FIFOLEN      0x04
#define CET632_REG_SECONDSTAT   0x05
#define CET632_REG_INTEN        0x06
#define CET632_REG_INTRQ        0x07

//Page1
#define CET632_REG_CTRL         0x09
#define CET632_REG_ERR          0x0A
#define CET632_REG_COLL         0x0B
#define CET632_REG_TIMER        0x0C
#define CET632_REG_CRCLSB       0x0D
#define CET632_REG_CRCMSB       0x0E
#define CET632_REG_BITFRAME     0x0F

//Page2
#define CET632_REG_TXCTRL       0x11
#define CET632_REG_TXCW         0x12
#define CET632_REG_TXMOD        0x13
#define CET632_REG_CODERCTRL    0x14
#define CET632_REG_MODWIDTH     0x15
#define CET632_REG_MODWSOF      0x16
#define CET632_REG_TYPEBCFG     0x17

//Page3
#define CET632_REG_RXCTRL1      0x19
#define CET632_REG_DECODECTRL   0x1A
#define CET632_REG_BITPHASE     0x1B
#define CET632_REG_RXTHLD       0x1C
#define CET632_REG_BPSKCTRL     0x1D
#define CET632_REG_RXCTRL2      0x1E

//Page4
#define CET632_REG_RXWAIT       0x21
#define CET632_REG_CRCCTRL      0x22
#define CET632_REG_CRCPRESETLSB 0x23
#define CET632_REG_CRCPRESETMSB 0x24

//Page5
#define CET632_REG_FIFOLV       0x29
#define CET632_REG_TIMERCLK     0x2A
#define CET632_REG_TIMERCTRL    0x2B
#define CET632_REG_TIMERRELOAD  0x2C
#define CET632_REG_FILTERCTRL   0x2E
#define CET632_REG_FILTERADJ    0x2F

#define CET632_FIFO_LEVEL       16

//Page6
#define CET632_REG_IOCFG        0x31

//Page7
#define CET632_REG_SSIF         0x37

//Page8
#define CET632_REG_TESTANT      0x3A
#define CET632_REG_TXDISCTRL    0x3B
#define CET632_REG_ISO15693HD   0x3D
#define CET632_REG_ALGADJ1      0x3E
#define CET632_REG_ALGADJ2      0x3F


//************** CET632 Command
#define CET632_CMD_IDLE         0x00
#define CET632_CMD_TRANSMIT     0x1A
#define CET632_CMD_RECEIVE      0x16
#define CET632_CMD_TRANCEIVE    0x1E
#define CET632_CMD_WRITEE2      0x01
#define CET632_CMD_READE2       0x03
#define CET632_CMD_LOADCFG      0x07
#define CET632_CMD_CALCRC       0x12
#define CET632_CMD_LOADKEY      0x19
#define CET632_CMD_LOADKEYE2    0x0B
#define CET632_CMD_AUTH         0x1C

//******* Mask for Primary Status Reg (Reg 0x03)
#define CET632_BIT_LOALERT      0x01
#define CET632_BIT_HIALERT      0x02
#define CET632_BIT_ERROR        0x04
#define CET632_BIT_IRQ          0x08


//******* Mask for Interrupt Enable Reg (Reg 0x06)
#define CET632_BIT_FIFOLOIEN    0x01
#define CET632_BIT_FIFOHIIEN    0x02
#define CET632_BIT_IDLEIEN      0x04
#define CET632_BIT_RXIEN        0x08
#define CET632_BIT_TXIEN        0x10
#define CET632_BIT_TIMEIEN      0x20
#define CET632_BIT_SETIEN       0x80

//******* Mask for Interrupt Enable Reg (Reg 0x07)
#define CET632_BIT_FIFOLOIRQ    0x01
#define CET632_BIT_FIFOHIIRQ    0x02
#define CET632_BIT_IDLEIRQ      0x04
#define CET632_BIT_RXIRQ        0x08
#define CET632_BIT_TXIRQ        0x10
#define CET632_BIT_TIMEIRQ      0x20
#define CET632_BIT_SETIRQ       0x80




//******* Mask for Control Reg (Reg 0x09)
#define CET632_BIT_FLUSHFIFO    0x01
#define CET632_BIT_TSTART       0x02
#define CET632_BIT_TSTOP        0x04
#define CET632_BIT_CRYPTO1ON    0x08
#define CET632_BIT_POWERDOWN    0x10
#define CET632_BIT_STANDBY      0x20


//******* Mask for Err Reg (Reg 0x0A)
#define CET632_BIT_COLLERR      0x01
#define CET632_BIT_PARITYERR    0x02
#define CET632_BIT_FRAMEERR     0x04
#define CET632_BIT_CRCERR       0x08
#define CET632_BIT_FIFOOV       0x10
#define CET632_BIT_ACCESSERR    0x20
#define CET632_BIT_KEYERR       0x40
#define CET632_BIT_E2ERR        0x80

//******* Mask for TxControl Reg (Reg 0x11)
#define CET632_BIT_TX1RFEN      0x01  
#define CET632_BIT_TX2RFEN      0x02
#define CET632_BIT_TX2CW        0x04
#define CET632_BIT_TX2INV       0x08
#define CET632_BIT_100ASK       0x10
#define CET632_BIT_MODSRC_Z     0x00
#define CET632_BIT_MODSRC_H     0x20
#define CET632_BIT_MODSRC_IN    0x40
#define CET632_BIT_MODSRC_S0    0x60
#define CET632_BIT_TX1INV       0x80

//******* Mask for CoderControl Reg (Reg 0x14)
#define CET632_BIT_NRZ          0x00
#define CET632_BIT_MILLER       0x01
#define CET632_BIT_1P256        0x06
#define CET632_BIT_1P4          0x07
#define CET632_BIT_A_424K       0x08
#define CET632_BIT_B_848K       0x08
#define CET632_BIT_A_212K       0x10
#define CET632_BIT_B_424K       0x10
#define CET632_BIT_A_106K       0x18
#define CET632_BIT_B_212K       0x18
#define CET632_BIT_B_106K       0x20
#define CET632_BIT_ISO15693K    0x28
#define CET632_BIT_SENDEOF      0x80

//******* Mask for TypeBFraming Reg (Reg 0x17)
#define CET632_BIT_SOF10L2HETU  0x00
#define CET632_BIT_SOF10L3HETU  0x01
#define CET632_BIT_SOF11L2HETU  0x02
#define CET632_BIT_SOF11L3HETU  0x03
#define CET632_BIT_EGTMASK      0x1C
#define CET632_BIT_EOF10ETU     0x00
#define CET632_BIT_EOF11ETU     0x20
#define CET632_BIT_NOTXEOF      0x40
#define CET632_BIT_NOTXSOF      0x80

//******* Mask for RxContrl1 Reg (Reg 0x19)
#define CET632_BIT_GAIN12DB     0x00
#define CET632_BIT_GAIN24DB     0x01
#define CET632_BIT_GAIN36DB     0x02
#define CET632_BIT_GAIN48DB     0x03
#define CET632_BIT_LPOFF        0x04
#define CET632_BIT_SUB8CLK      0x00
#define CET632_BIT_SUB16CLK     0x08
#define CET632_BIT_SUB32CLK     0x10
#define CET632_BIT_SUB64CLK     0x18
#define CET632_BIT_SUB1PLS      0x00
#define CET632_BIT_SUB2PLS      0x20
#define CET632_BIT_SUB4PLS      0x40
#define CET632_BIT_SUB8PLS      0x60
#define CET632_BIT_SUB16PLS     0x80
#define CET632_BIT_SUB32PLS     0xA0
#define CET632_BIT_SUB64PLS     0xC0


//******* Mask for Decoder Control Reg (Reg 0x1A)
#define CET632_BIT_RXCODE_MCST  0x00
#define CET632_BIT_RXCODE_BPSK  0x01
#define CET632_BIT_RXCODE_FSK   0x02
#define CET632_BIT_RXCODE_RFU   0x03
#define CET632_BIT_RXFRAM_RFU   0x00
#define CET632_BIT_RXFRAM_A     0x08
#define CET632_BIT_RXFRAM_15693 0x10
#define CET632_BIT_RXFRAM_B     0x18
#define CET632_BIT_COLL_ZERO    0x20
#define CET632_BIT_COLL_BIT0    0x00
#define CET632_BIT_COLL_BIT1    0x40
#define CET632_BIT_BPSK_FELICA  0x80


//******* BPSKDemContrl Reg (Reg 0x1D)
#define CET632_BIT_AGCEN        0x02


//******* Mask for CRCSetting Reg (Reg 0x22) 
#define CET632_BIT_PARITYEN     0x01
#define CET632_BIT_PARITYODD    0x02
#define CET632_BIT_PARITYEVN    0x00
#define CET632_BIT_CRCTXEN      0x04
#define CET632_BIT_CRCRXEN      0x08
#define CET632_BIT_CRC8         0x10
#define CET632_BIT_CRC3309      0x20

//******* Mask for TimerClock Reg (Reg 0x2A)
#define CET632_BIT_TPRESCALMASK 0x1F
#define CET632_BIT_TAUTOSTART   0x20

//******* Mask for TimerControl Reg (Reg 0x2B)
#define CET632_BIT_TSTART_TXS   0x01
#define CET632_BIT_TSTART_TXE   0x02
#define CET632_BIT_TSTOP_RXS    0x04
#define CET632_BIT_TSTOP_RXE    0x08


//************* Interrupt Value for CET632
#define CET632_BIT_IRQMASK      0x3F
#define CET632_BIT_IRQ_TIMER    0x20
#define CET632_BIT_IRQ_TX       0x10
#define CET632_BIT_IRQ_RX       0x08
#define CET632_BIT_IRQ_IDLE     0x04
#define CET632_BIT_IRQ_FIFOH    0x02
#define CET632_BIT_IRQ_FIFOL    0x01



#define CET632_ERR_SUCCESS      0x00
#define CET632_ERR_TIMEOUT      0x01
#define CET632_ERR_COLL         0x02
#define CET632_ERR_PARITY       0x03
#define CET632_ERR_FRAME        0x04
#define CET632_ERR_CRC          0x05
#define CET632_ERR_FIFO         0x06
#define CET632_ERR_ACCESS       0x07
#define CET632_ERR_KEY          0x08
#define CET632_ERR_EEPROM       0x09
#define CET632_ERR_BCC          0x0A
#define CET632_ERR_AUTH         0x0B
#define CET632_ERR_NAK          0x0C
#define CET632_ERR_AUTHM0U2     0x0D

#define CET632_ERR_OTHER        0xFF



#define CET632_TIME_WEEPROM     4900 //us







u8 CET632_ReadReg(u8 addr);
void CET632_WriteReg(u8 addr, u8 reg);
void CET632_ClearRegBit(u8 addr, u8 reg);
void CET632_SetRegBit(u8 addr, u8 reg);
void CET632_ReadFifo(u8 *pBuffer, u8 len);
void CET632_WriteFifo(u8 *pBuffer, u8 len);
u8 CET632_CheckErr(u8 err);
u8 CET632_Command(u8 cmd, u8 *pTxBuffer, u8 txLen, u8 *pRxBuffer, u8 *pRxLen, u32 usTimeOut);
u8 CET632_Transceive(u8 *pTxBuffer, u8 txLen, u8 txLastBits, u8 rxAlignBit, u8 *pRxBuffer, u8 *pRxLen, u32 usTimeOut);
u8 CET632_LoadKey(u8 *pKey, u8 keyLen);



#endif

