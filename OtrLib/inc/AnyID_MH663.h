#ifndef _ANYID_MH663_
#define _ANYID_MH663_

#include "AnyID_M345_MH663_HL.h"

#define MH663_PROTOCOL_CONFIG_SIZE      15
extern const u8 g_sRc663RegAdd[MH663_PROTOCOL_CONFIG_SIZE];

#define MH663_VERSION_INFO          0x18
#define MH663_READ_FLAG             0x80
#define MH663_WRITE_FLAG            0x7F
#define MH663_RADDR(addr)           (MH663_READ_FLAG | (addr << 1))
#define MH663_WADDR(addr)           (MH663_WRITE_FLAG & (addr << 1))

#define MH663_OpenAntenna()         MH663_SetRegBit(MH663_REG_TXCTRL, MH663_TXCTRL_TX1EN | MH663_TXCTRL_TX2EN)
#define MH663_CloseAntenna()        MH663_ClearRegBit(MH663_REG_TXCTRL, MH663_TXCTRL_TX1EN | MH663_TXCTRL_TX2EN)

#define MH663_FlushFifo()           MH663_SetRegBit(MH663_REG_FIFOLEV, MH663_FIFO_CLEAR)

//清空所有中断标志
#define MH663_ClearComIrqFlag()     MH663_WriteReg(MH663_REG_COMIRQ, 0x7F)
#define MH663_ClearDivIrqFlag()     MH663_WriteReg(MH663_REG_DIVIRQ, 0x7F)

#define MH663_SetTxLastBits(b)      //do{\
                                    //    u8 reg = 0;\
                                    //    reg = MH663_ReadReg(MH663_REG_TXDATANUM) & (~MH663_MASK_TXLASTBITS);\
                                    //    reg |= (MH663_MASK_TXLASTBITS & (b));\
                                    //    MH663_WriteReg(MH663_REG_TXDATANUM, reg);\
                                    //}while(0)


void MH663_WriteReg(u8 addr, u8 val);
u8 MH663_ReadReg(u8 addr);
void MH663_SetRegBit(u8 addr, u8 bit);
void MH663_SetRegBits(u8 addr, u8 msk, u8 bit);
void MH663_ClearRegBit(u8 addr, u8 bit);
void MH663_ReadFIFO(u8 *pBuffer, u8 len);
void MH663_WriteFIFO(u8 *pBuffer, u8 len);
void MH663_Config(u8 bClearCrypt, u8 bEnTxCrc, u8 bEnRxCrc, u8 txLastBits, u8 rxAlignBits);

#define MH663_RXINF_LEN           2
#define MH663_BUFFER_POS_RXLEN    0
#define MH663_BUFFER_POS_RXLBIT   1

BOOL MH663_Reset(void);
u8 MH663_Init(void);
u8 MH663_SelectProtocol(u8 tx, u8 rx);
u8 MH663_Command(void);
u8 MH663_TopazCommand(void);
u8 MH663_ReadProductID(void);

#define MH663_SIZE_FIFO             64
#define MH663_FIFO_LEVEL            16
#define MH663_MASK_CMD              0x0F
// PAGE 0
#define MH663_REG_RFU00             0x00    
#define MH663_REG_CMD               0x01        //starts and stops command execution 
#define MH663_REG_COMIEN            0x02    
#define MH663_REG_DIVIEN            0x03    
#define MH663_REG_COMIRQ            0x04    
#define MH663_REG_DIVIRQ            0x05
#define MH663_REG_ERROR             0x06    
#define MH663_REG_STATUS1           0x07    
#define MH663_REG_STATUS2           0x08    
#define MH663_REG_FIFODAT           0x09
#define MH663_REG_FIFOLEV           0x0A
#define MH663_REG_FIFOWLEV          0x0B
#define MH663_REG_CONTROL           0x0C
#define MH663_REG_BITFRAME          0x0D
#define MH663_REG_COLL              0x0E
#define MH663_REG_RFU0F             0x0F
// PAGE 1     
#define MH663_REG_RFU10             0x10
#define MH663_REG_MODE              0x11
#define MH663_REG_TXMODE            0x12
#define MH663_REG_RXMODE            0x13
#define MH663_REG_TXCTRL            0x14
#define MH663_REG_TXASK             0x15
#define MH663_REG_TXAUTO            0x15
#define MH663_REG_TXSEL             0x16
#define MH663_REG_RXSEL             0x17
#define MH663_REG_RXTHRLD           0x18
#define MH663_REG_DEMOD             0x19
#define MH663_REG_RFU1A             0x1A
#define MH663_REG_RFU1B             0x1B
#define MH663_REG_MFTX              0x1C
#define MH663_REG_MFRX              0x1D
#define MH663_REG_TYPEB             0x1E
#define MH663_REG_SERBR             0x1F
// PAGE 2    
#define MH663_REG_RFU20             0x20  
#define MH663_REG_CRCRLTH           0x21
#define MH663_REG_CRCRLTL           0x22
#define MH663_REG_RFU23             0x23
#define MH663_REG_MODWIDTH          0x24    //defines the width of the Miller modulation as multiples of the carrier frequency (ModWidth + 1 / fclk).
#define MH663_REG_RFU25             0x25
#define MH663_REG_RFCFG             0x26
#define MH663_REG_GSN               0x27
#define MH663_REG_CWGSP             0x28
#define MH663_REG_MODGSP            0x29
#define MH663_REG_TMODE             0x2A
#define MH663_REG_TPRESCALER        0x2B
#define MH663_REG_TRELOADH          0x2C
#define MH663_REG_TRELOADL          0x2D
#define MH663_REG_TCOUNTH           0x2E
#define MH663_REG_TCOUNTL           0x2F
// PAGE 3      
#define MH663_REG_RFU30             0x30
#define MH663_REG_TESTSEL1          0x31
#define MH663_REG_TESTSEL2          0x32
#define MH663_REG_TESTPINEN         0x33
#define MH663_REG_TESTPINVAL        0x34
#define MH663_REG_TESTBUS           0x35
#define MH663_REG_AUTOTEST          0x36
#define MH663_REG_VERSION           0x37
#define MH663_REG_ANALOGTest        0x38
#define MH663_REG_TESTDAC1          0x39  
#define MH663_REG_TESTDAC2          0x3A   
#define MH663_REG_TESTADC           0x3B   
#define MH663_REG_RFU3C             0x3C   
#define MH663_REG_RFU3D             0x3D   
#define MH663_REG_RFU3E             0x3E   
#define MH663_REG_SPECIAL           0x3F


//reg bits
#define MH663_POS_BIT7              (1 << 7)
#define MH663_POS_BIT6              (1 << 6)
#define MH663_POS_BIT5              (1 << 5)
#define MH663_POS_BIT4              (1 << 4)
#define MH663_POS_BIT3              (1 << 3)
#define MH663_POS_BIT2              (1 << 2)
#define MH663_POS_BIT1              (1 << 1)
#define MH663_POS_BIT0              (1 << 0)

//DEFINES Registers bits
//****************************************************************
#define MH663_COMIEN_TXIEN          (1 << 6)
#define MH663_COMIEN_RXIEN          (1 << 5)
#define MH663_COMIEN_IDLEIEN        (1 << 4)
#define MH663_COMIEN_ERRIEN         (1 << 1)
#define MH663_COMIEN_TIMERIEN       (1 << 0)

#define MH663_COMI_TX               (1 << 6)
#define MH663_COMI_RX               (1 << 5)
#define MH663_COMI_IDLE             (1 << 4)
#define MH663_COMI_ERR              (1 << 1)
#define MH663_COMI_TIMER            (1 << 0)

#define MH663_ERR_COLL              (1 << 3)
#define MH663_ERR_CRC               (1 << 2)
#define MH663_ERR_PARITY            (1 << 1)
#define MH663_ERR_PROTOCOL          (1 << 0)

//status 2 reg
#define MH663_STATUS_CRYPTO_ON      (1 << 3)
#define MH663_STATUS_STATMSK        0x07
#define MH663_STATUS_IDLE           0x00
#define MH663_STATUS_WAIT_START     0x01
#define MH663_STATUS_WAIT_TX        0x02
#define MH663_STATUS_TXING          0x03
#define MH663_STATUS_WAIT_RX        0x04
#define MH663_STATUS_WAIT_DAT       0x05
#define MH663_STATUS_RXING          0x06


//coll reg
#define MH663_COLL_VAL_COLLPOS      (1 << 7)
#define MH663_COLL_POS_INVALID      (1 << 5)
#define MH663_COLL_POS_MASK         0x1F

//bitFramming reg
#define MH663_BITFRAM_START_SEND    (1 << 7)
#define MH663_BITFRAM_RXALIGN_MASK  0x70
#define MH663_BITFRAM_TXLAST_MASK   0x07

//fifo level reg
#define MH663_FIFO_CLEAR            (1 << 7)
#define MH663_FIFOLV_MASK           0x7F

//tx contrl reg
#define MH663_TXCTRL_TX1EN          (1 << 0)
#define MH663_TXCTRL_TX2EN          (2 << 0)

//mode reg
#define MH663_MODE_MSBF             (1 << 7)
#define MH663_MODE_TXWAITRF         (1 << 5)
#define MH663_MODE_POLMFIN          (1 << 3)
#define MH663_MODE_CRCMASK          0x03
#define MH663_MODE_CRC0000          0x00
#define MH663_MODE_CRC6363          0x01
#define MH663_MODE_CRCA671          0x02
#define MH663_MODE_CRCFFFF          0x03

//tx mode reg 
#define MH663_TXMODE_CRCEN          (1 << 7)
#define MH663_TXMODE_SPDMSK         0x70
#define MH663_TXMODE_106            0x00
#define MH663_TXMODE_212            0x10
#define MH663_TXMODE_424            0x20
#define MH663_TXMODE_848            0x30
#define MH663_TXMODE_INV            (1 << 3)    // modulation of transmitted data is inverted
#define MH663_TXMODE_FRMMSK         0x07        // defines the framing used for data transmission
#define MH663_TXMODE_TYPEA          0x00
#define MH663_TXMODE_TYPEF          0x02
#define MH663_TXMODE_TYPEB          0x03

//tx ask reg
#define MH663_TXASK_100             (1 << 6)    // forces 100 % ASK modulation independently of the ModGsPReg register setting

//rx mode reg
#define MH663_RXMODE_CRCEN          (1 << 7)
#define MH663_RXMODE_SPDMSK         0x70
#define MH663_RXMODE_106            0x00
#define MH663_RXMODE_212            0x10
#define MH663_RXMODE_424            0x20
#define MH663_RXMODE_848            0x30
#define MH663_RXMODE_NOERR          (1 << 3)    // an invalid received data stream (less than 4 bits received) will be ignored and the receiver remains active
#define MH663_RXMODE_MULT           (1 << 2)
#define MH663_RXMODE_FRMMSK         0x07        // defines the framing used for data transmission
#define MH663_RXMODE_TYPEA          0x00
#define MH663_RXMODE_15693          0x02
#define MH663_RXMODE_TYPEB          0x03

//rx sel reg
#define MH663_RXSEL_UARTMSK         0xC0
#define MH663_RXSEL_UART_LOW        0x00
#define MH663_RXSEL_UART_MANMFIN    0x40
#define MH663_RXSEL_UART_INANA      0x80
#define MH663_RXSEL_UART_NRZMFIN    0xC0
#define MH663_RXSEL_RXWAIT_MASK     0x3F

//MF rx reg
#define MH663_MFRX_PARITY_DIS       (1 << 4)    //generation of the parity bit for transmission and the parity check for receiving is switched off.




// PCD命令字
//****************************************************************
#define MH663_PCD_IDLE              0x00                // 取消当前命令
#define MH663_PCD_AUTHENT           0x0E                // 验证密钥
#define MH663_PCD_RECEIVE           0x08                // 接收数据
#define MH663_PCD_TRANSMIT          0x04                // 发送数据
#define MH663_PCD_TRANSCEIVE        0x0C                // 发送并接收数据
#define MH663_PCD_RESETPHASE        0x0F                // 复位
#define MH663_PCD_CALCCRC           0x03                // CRC计算
#define MH663_PCD_CMD_MASK          0x0F                // 命令字掩码


// 
// Mifare Error Codes
// Each function returns a status value, which corresponds to 
// the mifare error
// codes. 
// ****************************************************************
#define MH663_STAT_OK                               0
#define MH663_STAT_CHK_OK                           0
#define MH663_STAT_CRC_ZERO                         0

#define MH663_STAT_CRC_NOTZERO                      1

#define MH663_STAT_TIMEOUT                          (1)
#define MH663_STAT_CRCERR                           (2)
#define MH663_STAT_NOTAGERR                         (3)
#define MH663_STAT_AUTHERR                          (4)
#define MH663_STAT_PARITYERR                        (5)
#define MH663_STAT_CODEERR                          (6)
#define MH663_STAT_SERNRERR                         (8)
#define MH663_STAT_KEYERR                           (9)
#define MH663_STAT_NOTAUTHERR                       (10)
#define MH663_STAT_BITCOUNTERR                      (11)
#define MH663_STAT_BYTECOUNTERR                     (12)
#define MH663_STAT_IDLE                             (13)
#define MH663_STAT_TRANSERR                         (14)
#define MH663_STAT_WRITEERR                         (15)
#define MH663_STAT_INCRERR                          (16)
#define MH663_STAT_DECRERR                          (17)
#define MH663_STAT_READERR                          (18)
#define MH663_STAT_OVFLERR                          (19)
#define MH663_STAT_POLLING                          (20)
#define MH663_STAT_FRAMINGERR                       (21)
#define MH663_STAT_ACCESSERR                        (22)
#define MH663_STAT_UNKNOWN_COMMAND                  (23)
#define MH663_STAT_COLLERR                          (24)
#define MH663_STAT_RESETERR                         (25)
#define MH663_STAT_INITERR                          (25)
#define MH663_STAT_INTERFACEERR                     (26)
#define MH663_STAT_ACCESSTIMEOUT                    (27)
#define MH663_STAT_NOBITWISEANTICOLL                (28)
#define MH663_STAT_QUIT                             (30)
#define MH663_STAT_INTEGRITY_ERR                    (35) //完整性错误(crc/parity/protocol)
#define MH663_STAT_RECBUF_OVERFLOW                  (50) 
#define MH663_STAT_SENDBYTENR                       (51)    
#define MH663_STAT_SENDBUF_OVERFLOW                 (53)
#define MH663_STAT_BAUDRATE_NOT_SUPPORTED           (54)
#define MH663_STAT_SAME_BAUDRATE_REQUIRED           (55)
#define MH663_STAT_WRONG_PARAMETER_VALUE            (60)
#define MH663_STAT_BREAK                            (99)
#define MH663_STAT_NY_IMPLEMENTED                   (100)
#define MH663_STAT_NO_MFRC                          (101)
#define MH663_STAT_MFRC_NOTAUTH                     (102)
#define MH663_STAT_WRONG_DES_MODE                   (103)
#define MH663_STAT_HOST_AUTH_FAILED                 (104)
#define MH663_STAT_WRONG_LOAD_MODE                  (106)
#define MH663_STAT_WRONG_DESKEY                     (107)
#define MH663_STAT_MKLOAD_FAILED                    (108)
#define MH663_STAT_FIFOERR                          (109)
#define MH663_STAT_WRONG_ADDR                       (110)
#define MH663_STAT_DESKEYLOAD_FAILED                (111)
#define MH663_STAT_WRONG_SEL_CNT                    (114)
#define MH663_STAT_WRONG_TEST_MODE                  (117)
#define MH663_STAT_TEST_FAILED                      (118)
#define MH663_STAT_TOC_ERROR                        (119)
#define MH663_STAT_COMM_ABORT                       (120)
#define MH663_STAT_INVALID_BASE                     (121)
#define MH663_STAT_MFRC_RESET                       (122)
#define MH663_STAT_WRONG_VALUE                      (123)
#define MH663_STAT_VALERR                           (124)
#define MH663_STAT_COM_ERR                          (125)
#define MH663_STAT_PROTOCOL_ERR                     (126)

///用户使用错误
#define MH663_USER_ERROR                            (127)

#define MH663_FRAME_LEN                             255


typedef struct MH663_FRAME{
    u8 cmd;
    u8 frame[MH663_FRAME_LEN];
    u8 txLen;
    u8 rxLen;
    u32 timeout;
}MH663_FRAME;
extern MH663_FRAME g_sMH663Frame;


#define MH663_RX_FRAME_TIMEOUT      1000        //1000*16us = 16ms
#define MH663_TX_FRAME_TIMEOUT      1000
#endif


