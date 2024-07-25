#ifndef _ANYID_FM17XX_
#define _ANYID_FM17XX_

#include "AnyID_M345_FM17xx_HL.h"

#define FM17XX_READ_FLAG     0x80
#define FM17XX_WRITE_FLAG    0x7F
#define FM17XX_RADDR(addr)   (FM17XX_READ_FLAG | (addr << 1))
#define FM17XX_WADDR(addr)   (FM17XX_WRITE_FLAG & (addr << 1))

#define FM17xx_OpenAntenna()         FM17xx_SetRegBit(FM17XX_REG_TXCONTROL, FM17XX_BIT_TX2RFEN | FM17XX_BIT_TX1RFEN)
#define FM17xx_CloseAntenna()        FM17xx_ClearRegBit(FM17XX_REG_TXCONTROL, FM17XX_BIT_TX2RFEN | FM17XX_BIT_TX1RFEN)

void FM17xx_WriteReg(u8 addr, u8 val);
u8 FM17xx_ReadReg(u8 addr);
void FM17xx_SetRegBit(u8 addr, u8 bit);
void FM17xx_ClearRegBit(u8 addr, u8 bit);
void FM17xx_ReadFIFO(u8 *pBuffer, u8 len);
void FM17xx_WriteFIFO(u8 *pBuffer, u8 len);

#define FM17XX_RXINF_LEN           2
#define FM17XX_BUFFER_POS_RXLEN    0
#define FM17XX_BUFFER_POS_RXLBIT   1
BOOL FM17xx_Reset(void);
u8 FM17xx_Command(void);
void FM17xx_Config(BOOL bClearCrypt, BOOL bTxCrc, BOOL bRxCrc, u8 txLastBits, u8 rxAlignBits);


#define FM17XX_SIZE_FIFO             64
#define FM17XX_FIFO_LEVEL            16
//Page 0
#define     FM17XX_REG_RFU00           0x00      //Currently not used
#define     FM17XX_REG_COMMAND         0x01      //Contains Command bits, PowerDown bit and bit to  switch receiver off
#define     FM17XX_REG_COMMIEN         0x02      //Contains Communication interrupt enable bits and bit for Interrupt inversion
#define     FM17XX_REG_DIVIEN          0x03      //Contains RfOn, RfOff, CRC and Mode Interrupt enable and bit to switch Interrupt pin to PushPull mode
#define     FM17XX_REG_COMMIRQ         0x04      //Contains Communication interrupt request bits
#define     FM17XX_REG_DIVIRQ          0x05      //Contains RfOn, RfOff, CRC and Mode Interrupt request
#define     FM17XX_REG_ERROR           0x06      //Contains Protocol, Parity, CRC, Collision, Buffer overflow, Temperature and RF error flags
#define     FM17XX_REG_STATUS1         0x07      //Contains status information about Lo- and HiAlert, RF-field on, Timer, Interrupt request and CRC status
#define     FM17XX_REG_STATUS2         0x08      //Contains information about internal states (Modemstate), Mifare states and possibility to switch Temperature sensor off
#define     FM17XX_REG_FIFODATA        0x09      //Gives access to FIFO. Writing to register increments the FIFO level (register 0x0A), reading decrements it
#define     FM17XX_REG_FIFOLEVEL       0x0A      //Contains the actual level of the FIFO
#define     FM17XX_REG_WATERLEVEL      0x0B      //Contains the Waterlevel value for the FIFO
#define     FM17XX_REG_CONTROL         0x0C      //Contains information about last received bits and to Start and stop the Timer unit
#define     FM17XX_REG_BITFRAMING      0x0D      //Contains information of last bits to send, to align received bits in FIFO and activate sending in Transceive
#define     FM17XX_REG_COLL            0x0E      //Contains all necessary bits for Collission handling
#define     FM17XX_REG_RFU0F           0x0F      //Currently not used


//Page 1
#define     FM17XX_REG_RFU10           0x10      //Currently not used
#define     FM17XX_REG_MODE            0x11      //Contains bits for auto wait on Rf, to detect SYNC byte in NFC mode and MSB first for CRC calculation
#define     FM17XX_REG_TXMODE          0x12      //Contains Transmit Framing, Speed, CRC enable, bit for inverse mode and TXMix bit
#define     FM17XX_REG_RXMODE          0x13      //Contains Transmit Framing, Speed, CRC enable, bit for multiple receive and to filter errors
#define     FM17XX_REG_TXCONTROL       0x14      //Contains bits to activate and configure Tx1 and Tx2 and bit to activate 100% modulation
#define     FM17XX_REG_TXAUTO          0x15      
#define     FM17XX_REG_TXSEL           0x16      //Contains SigoutSel, DriverSel and LoadModSel bits
#define     FM17XX_REG_RXSEL           0x17      //Contains UartSel and RxWait bits
#define     FM17XX_REG_RXTRESHOLD      0x18      //Contains MinLevel and CollLevel for detection
#define     FM17XX_REG_DEMOD           0x19      //Contains bits for time constants, hysteresis and IQ demodulator settings
#define     FM17XX_REG_RFU1A           0x1A      
#define     FM17XX_REG_RFU1B           0x1B      
#define     FM17XX_REG_MIFNFC          0x1C      
#define     FM17XX_REG_RFU1D           0x1D      
#define     FM17XX_REG_RFU1E           0x1E      //Currently not used
#define     FM17XX_REG_SERIALSPEED     0x1F      //Contains speed settings for serila interface

//Page 2
#define     FM17XX_REG_RFU20           0x20      //Currently not used
#define     FM17XX_REG_CRCRESULTM      0x21      //Contains MSByte of CRC Result
#define     FM17XX_REG_CRCRESULTL      0x22      //Contains LSByte of CRC Result
#define     FM17XX_REG_GSNOFF          0x23      //Currently not used
#define     FM17XX_REG_MODWIDTH        0x24      //Contains modulation width setting
#define     FM17XX_REG_TXBITPHASE      0x25      //Currently not used
#define     FM17XX_REG_RFCFG           0x26      //Contains sensitivity of Rf Level detector, the receiver gain factor and the RfLevelAmp
#define     FM17XX_REG_GSN             0x27      //Contains the conductance and the modulation settings for the N-MOS transistor
#define     FM17XX_REG_CWGSP           0x28      //Contains the conductance for the P-Mos transistor
#define     FM17XX_REG_MODGSP          0x29      //Contains the modulation index for the PMos transistor
#define     FM17XX_REG_TMODE           0x2A      //Contains all settings for the timer and the highest 4 bits of the prescaler
#define     FM17XX_REG_TPRESCALER      0x2B      //Contais the lowest byte of the prescaler
#define     FM17XX_REG_TRELOADHI       0x2C      //Contains the high byte of the reload value
#define     FM17XX_REG_TRELOADLO       0x2D      //Contains the low byte of the reload value
#define     FM17XX_REG_TCOUNTERVALHI   0x2E      //Contains the high byte of the counter value
#define     FM17XX_REG_TCOUNTERVALLO   0x2F      //Contains the low byte of the counter value

//Page 3
#define     FM17XX_REG_RFU30           0x30      //Currently not used
#define     FM17XX_REG_TESTSEL1        0x31      //Test register
#define     FM17XX_REG_TESTSEL2        0x32      //Test register
#define     FM17XX_REG_TESTPINEN       0x33      //Test register
#define     FM17XX_REG_TESTPINVALUE    0x34      //Test register
#define     FM17XX_REG_TESTBUS         0x35      //Test register
#define     FM17XX_REG_AUTOTEST        0x36      //Test register
#define     FM17XX_REG_VERSION         0x37      //Contains the product number and the version
#define     FM17XX_REG_ANALOGTEST      0x38      //Test register
#define     FM17XX_REG_TESTDAC1        0x39      //Test register
#define     FM17XX_REG_TESTDAC2        0x3A      //Test register
#define     FM17XX_REG_TESTADC         0x3B      //Test register
#define     FM17XX_REG_ANALOGUETEST1   0x3C      //Test register
#define     FM17XX_REG_RFT3D           0x3D      //Test register
#define     FM17XX_REG_RFT3E           0x3E      //Test register
#define     FM17XX_REG_RFT3F           0x3F      //Test register

//Possible commands
#define     FM17XX_CMD_IDLE            0x00      //No action: cancel current command or home state
#define     FM17XX_CMD_CONFIG          0x01
#define     FM17XX_CMD_GENRANDOM       0x02
#define     FM17XX_CMD_CALCCRC         0x03      /*!< Activate the CRC-Coprocessor \n<em><strong>
                                                Remark: </strong>The result of the CRC calculation can
                                                be read from the register CRCResultXXX </em>*/
#define     FM17XX_CMD_TRANSMIT        0x04      /*Transmit data from FIFO to he card \n<em>
                                                <strong>Remark: </strong>If data is already in
                                                the FIFO when the command is activated, this data is
                                                transmitted immediately. It is possible to
                                                write data to the FIFO while the Transmit
                                                command is active. Thus it is possible to
                                                transmit an unlimited number of bytes in one
                                                stream by writting them to the FIFO in time.</em>*/
#define     FM17XX_CMD_NOCMDCHANGE     0x07      /*!< This command does not change the actual commant of
                                                the RC522 and can only be written. \n<em><strong>
                                                Remark: </strong>This command is used for WakeUp procedure
                                                of RC522 to not change the current state. </em>*/
#define     FM17XX_CMD_RECEIVE         0x08      /*!< Activate Receiver Circuitry. Before the
                                                receiver actually starts, the state machine
                                                waits until the time configured in the
                                                register RxWait has passed. \n<em><strong>
                                                Remark: </strong>It is possible to read any received
                                                data from the FIFO while the Receive command
                                                is active. Thus it is possible to receive an
                                                unlimited number of bytes by reading them
                                                from the FIFO in time. </em>*/
#define     FM17XX_CMD_TRANSCEIVE      0x0C      /*!< This Command has two modes:\n
                                                Transmits data from FIFO to the card and after
                                                that automatically activates
                                                the receiver. Before the receiver actually
                                                starts,the state machine waits until the
                                                time configured in the register RxWait has
                                                passed. \n <em><strong>
                                                Remark: </strong>This command is the combination of
                                                Transmit and Receive.</em> */
#define     FM17XX_CMD_AUTOCOLL       0x0d
#define     FM17XX_CMD_AUTHENT        0x0E       /*!< Perform the card uthentication using the
                                                Crypto1 algorithm.
                                                \n <em><strong>Remark: </strong></em>*/
#define     FM17XX_CMD_SOFTRESET      0x0F       /*!< Runs the Reset- and Initialisation Phase
                                                \n <em><strong>Remark:</strong> This command can
                                                be activated by software, but only by a Power-On
                                                or Hard Reset </em>*/


//Bit Definitions Page 0
//Command Register                  (01)
#define     FM17XX_BIT_RCVOFF             0x20   //Switches the receiver on/off
#define     FM17XX_BIT_POWERDOWN          0x10   //Switches RC522 to Power Down mode

//CommIEn Register                  (02)
#define     FM17XX_BIT_IRQINV             0x80   //Inverts the output of IRQ Pin

//DivIEn Register                   (03)
#define     FM17XX_BIT_IRQPUSHPULL        0x80   //Sets the IRQ pin to Push Pull mode

//CommIEn and CommIrq Register      (02, 04)
#define     FM17XX_BIT_TXI                0x40   //Bit position for Transmit Interrupt Enable/Request
#define     FM17XX_BIT_RXI                0x20   //Bit position for Receive Interrupt Enable/Request
#define     FM17XX_BIT_IDLEI              0x10   //Bit position for Idle Interrupt Enable/Request
#define     FM17XX_BIT_HIALERTI           0x08   //Bit position for HiAlert Interrupt Enable/Request
#define     FM17XX_BIT_LOALERTI           0x04   //Bit position for LoAlert Interrupt Enable/Request
#define     FM17XX_BIT_ERRI               0x02   //Bit position for Error Interrupt Enable/Request
#define     FM17XX_BIT_TIMERI             0x01   //Bit position for Timer Interrupt Enable/Request

//DivIEn and DivIrq Register        (03, 05)
#define     FM17XX_BIT_SIGINACTI          0x10   //Bit position for SiginAct Interrupt Enable/Request
#define     FM17XX_BIT_CRCI               0x04   //Bit position for CRC Interrupt Enable/Request

//CommIrq and DivIrq Register       (04, 05)
#define     FM17XX_BIT_SET                0x80   //Bit position to set/clear dedicated IRQ bits

//Error Register                    (06)
#define     FM17XX_BIT_WRERR              0x80   //Bit position for Write Access Error
#define     FM17XX_BIT_TEMPERR            0x40   //Bit position for Temerature Error
#define     FM17XX_BIT_BUFFEROVFL         0x10   //Bit position for Buffer Overflow Error
#define     FM17XX_BIT_COLLERR            0x08   //Bit position for Collision Error
#define     FM17XX_BIT_CRCERR             0x04   //Bit position for CRC Error
#define     FM17XX_BIT_PARITYERR          0x02   //Bit position for Parity Error
#define     FM17XX_BIT_PROTERR            0x01   //Bit position for Protocol Error

//Status 1 Register                 (07)
#define     FM17XX_BIT_CRCOK              0x40   //Bit position for status CRC OK
#define     FM17XX_BIT_CRCREADY           0x20   //Bit position for status CRC Ready
#define     FM17XX_BIT_IRQ                0x10   //Bit position for status IRQ is active
#define     FM17XX_BIT_TRUNNUNG           0x08   //Bit position for status Timer is running
#define     FM17XX_BIT_HIALERT            0x02   //Bit position for status HiAlert
#define     FM17XX_BIT_LOALERT            0x01   //Bit position for status LoAlert

//Status 2 Register                 (08)
#define     FM17XX_BIT_TEMPSENSOFF        0x80   //Bit position to switch Temperture sensors on/off
#define     FM17XX_BIT_I2CFORCEHS         0x40   //Bit position to forece High speed mode for I2C Interface
#define     FM17XX_BIT_TARGETACTIVATED    0x10
#define     FM17XX_BIT_CRYPTO1ON          0x08   //Bit position for reader status Crypto is on

//FIFOLevel Register                (0A)
#define     FM17XX_BIT_FLUSHBUFFER        0x80   //Clears FIFO buffer if set to 1

//Control Register                  (0C)
#define     FM17XX_BIT_TSTOPNOW           0x80   //Stops timer if set to 1
#define     FM17XX_BIT_TSTARTNOW          0x40   //Starts timer if set to 1
#define     FM17XX_BIT_WRNFCIDTOFIFO      0x20
#define     FM17XX_BIT_INITIATOR          0X10

//BitFraming Register               (0D)
#define     FM17XX_BIT_STARTSEND          0x80   //Starts transmission in transceive command if set to 1

//BitFraming Register               (0E)
#define     FM17XX_BIT_VALUESAFTERCOLL    0x80   //Activates mode to keep data after collision


//Bit definitions of Page 1
//Mode Register                     (11)
#define     FM17XX_BIT_TXWAITRF           0x20   //Tx waits until Rf is enabled until transmit is startet, else transmit is started immideately
#define     FM17XX_BIT_POLSIGIN           0x08   //Inverts polarity of SiginActIrq, if bit is set to 1 IRQ occures when Sigin line is 0

//TxMode Register                   (12)
#define     FM17XX_BIT_INVMOD             0x08   //Activates inverted transmission mode

//RxMode Register                   (13)
#define     FM17XX_BIT_RXNOERR            0x08   //If 1, receiver does not receive less than 4 bits

//Definitions for Tx and Rx         (12, 13)
#define     FM17XX_BIT_106KBPS            0x00   //Activates speed of 106kbps
#define     FM17XX_BIT_212KBPS            0x10   //Activates speed of 212kbps
#define     FM17XX_BIT_424KBPS            0x20   //Activates speed of 424kbps

#define     FM17XX_BIT_CRCEN              0x80   //Activates transmit or receive CRC

//TxControl Register                (14)
#define     FM17XX_BIT_INVTX2ON           0x80   //Inverts the Tx2 output if drivers are switched on
#define     FM17XX_BIT_INVTX1ON           0x40   //Inverts the Tx1 output if drivers are switched on
#define     FM17XX_BIT_INVTX2OFF          0x20   //Inverts the Tx2 output if drivers are switched off
#define     FM17XX_BIT_INVTX1OFF          0x10   //Inverts the Tx1 output if drivers are switched off
#define     FM17XX_BIT_TX2CW              0x08   //Does not modulate the Tx2 output, only constant wave
#define     FM17XX_BIT_TX2RFEN            0x02   //Switches the driver for Tx2 pin on
#define     FM17XX_BIT_TX1RFEN            0x01   //Switches the driver for Tx1 pin on

#define     FM17XX_BIT_100ASK             0x40   //force 100 ASK

//Demod Register                   (19)
#define     FM17XX_BIT_FIXIQ              0x20   //If set to 1 and the lower bit of AddIQ is set to 0, the receiving is fixed to I channel
                                                 //If set to 1 and the lower bit of AddIQ is set to 1, the receiving is fixed to Q channel

//Bit definitions of Page 2
//TMode Register                   (2A)
#define     FM17XX_BIT_TAUTO              0x80   //Sets the Timer start/stop conditions to Auto mode
#define     FM17XX_BIT_TAUTORESTART       0x10   //Restarts the timer automatically after finished counting down to 0



//Bitmask Definitions
//Command register                 (0x01)
#define     FM17XX_MASK_COMMAND           0x0F   //Bitmask for Command bits in Register FM17XX_REG_COMMAND

//Waterlevel register              (0x0B)
#define     FM17XX_MASK_WATERLEVEL        0x3F   //Bitmask for Waterlevel bits in register FM17XX_REG_WATERLEVEL

//Control register                 (0x0C)
#define     FM17XX_MASK_RXBITS            0x07   //Bitmask for RxLast bits in register FM17XX_REG_CONTROL

//Mode register                    (0x11)
#define     FM17XX_MASK_CRCPRESET         0x03   //Bitmask for CRCPreset bits in register FM17XX_REG_MODE

//TxMode register                  (0x12, 0x13)
#define     FM17XX_MASK_SPEED             0x70   //Bitmask for Tx/RxSpeed bits in register FM17XX_REG_TXMODE and FM17XX_REG_RXMODE

//TxSel register                   (0x16)
#define     FM17XX_MASK_DRIVERSEL         0x30   //Bitmask for DriverSel bits in register FM17XX_REG_TXSEL
#define     FM17XX_MASK_SIGOUTSEL         0x0F   //Bitmask for SigoutSel bits in register FM17XX_REG_TXSEL

//RxSel register                   (0x17)
#define     FM17XX_MASK_UARTSEL           0xC0   //Bitmask for UartSel bits in register FM17XX_REG_RXSEL
#define     FM17XX_MASK_RXWAIT            0x3F   //Bitmask for RxWait bits in register FM17XX_REG_RXSEL

//RxThreshold register             (0x18)
#define     FM17XX_MASK_MINLEVEL          0xF0   //Bitmask for MinLevel bits in register FM17XX_REG_RXTHRESHOLD
#define     FM17XX_MASK_COLLEVEL          0x07   //Bitmask for CollLevel bits in register FM17XX_REG_RXTHRESHOLD

//Demod register                   (0x19)
#define     FM17XX_MASK_ADDIQ             0xC0   //Bitmask for ADDIQ bits in register FM17XX_REG_DEMOD
#define     FM17XX_MASK_TAURCV            0x0C   //Bitmask for TauRcv bits in register FM17XX_REG_DEMOD
#define     FM17XX_MASK_TAUSYNC           0x03   //Bitmask for TauSync bits in register FM17XX_REG_DEMOD

//RFCfg register                   (0x26)
#define     FM17XX_MASK_RXGAIN            0x70   //Bitmask for RxGain bits in register FM17XX_REG_RFCFG

//GsN register                     (0x27)
#define     FM17XX_MASK_CWGSN             0xF0   //Bitmask for CWGsN bits in register FM17XX_REG_GSN
#define     FM17XX_MASK_MODGSN            0x0F   //Bitmask for ModGsN bits in register FM17XX_REG_GSN

//CWGsP register                   (0x28)
#define     FM17XX_MASK_CWGSP             0x3F   //Bitmask for CWGsP bits in register FM17XX_REG_CWGSP

//ModGsP register                  (0x29)
#define     FM17XX_MASK_MODGSP            0x3F   //Bitmask for ModGsP bits in register FM17XX_REG_MODGSP

//TMode register                   (0x2A)
#define     FM17XX_MASK_TGATED            0x60   //Bitmask for TGated bits in register FM17XX_REG_TMODE
#define     FM17XX_MASK_TPRESCALER_HI     0x0F   //Bitmask for TPrescalerHi bits in register FM17XX_REG_TMODE



#define     FM17XX_STAT_OK                0x00   //正确
#define     FM17XX_STAT_INIT_ERROR        0x01   //系统初始化错误
#define     FM17XX_STAT_CMD_ERROR         0x02   //错误的RC522命令
#define     FM17XX_STAT_TIMEOUT           0x03   //RC522命令执行超时
#define     FM17XX_STAT_COLLISION_ERROR   0x04   //碰撞错误 STATUS_FM17XX_TEMP_ERROR
#define     FM17XX_STAT_PARITY_ERROR      0x05   //校验错误
#define     FM17XX_STAT_PROTOCOL_ERROR    0x06   //协议错误
#define     FM17XX_STAT_FIFOOF_ERROR      0x07   //FIFO溢出
#define     FM17XX_STAT_CRC_ERROR         0x08   //CRC错误
#define     FM17XX_STAT_TEMP_ERROR        0x09   //温度过高
#define     FM17XX_STAT_WFIFO_ERROR       0x0A   //写FIFO错误
#define     FM17XX_STAT_ACK               0x0B   //ASK
#define     FM17XX_STAT_ERROR             0x0C   //错误
#define     FM17XX_STAT_PICC_TYPE_ERROR   0x0D   //PICC类型不存在
#define     FM17XX_STAT_AUTH_ERROR        0x0F   //授权认证错误
#define     FM17XX_STAT_TRANS_ERROR       0x10   //传输错误
#define     FM17XX_STAT_BCC_ERROR         0x11   //BCC错误
#define     FM17XX_STAT_NAK               0x12   //BCC错误
#define     FM17XX_STAT_AUTHM0U2_ERR      0x13   //ultralightC授权错误


#define FM17XX_FRAME_LEN                  FM17XX_SIZE_FIFO
typedef struct fm17xxFrame{
    u8 cmd;
    u8 frame[FM17XX_SIZE_FIFO];
    u8 txLen;
    u8 rxLen;
    u32 timeout;
}FM17XX_FRAME;
extern FM17XX_FRAME g_sFm17xxFrame;
#endif

