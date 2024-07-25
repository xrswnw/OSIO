#ifndef _ANYID_CC2500_
#define _ANYID_CC2500_

#include "AnyID_Type.h"

#if defined(_ANYID_BWCOM_)
    #include "AnyID_BWCOM_CC2500_HL.h"
#elif defined(_ANYID_WVDU_)
    #include "AnyID_WVDU_CC2500_HL.h"
#elif defined(_ANYID_WVCU_)
    #include "AnyID_WVCU_CC2500_HL.h"
#elif defined(_ANYID_FLY_)
    #include "AnyID_FLY_CC2500_HL.h"
#endif


#if CC2500_PA_2591

#define CC2500_TXPA_Enable()    do{\
                                     CC2500_LNA_Disable();\
                                     CC2500_PA_Enable();\
                                 }while(0)

#define CC2500_RXPA_Enable()    do{                               \
                                     CC2500_PA_Disable();          \
                                     CC2500_LNA_Enable();          \
                                 }while(0)


#define CC2500_TXRXPA_Close()    do{                              \
                                     CC2500_PA_Disable();          \
                                     CC2500_LNA_Disable();         \
                                 }while(0)


#endif

#define CC2500_FRAME_CRC_MASK           0x80


#define CC2500_CRC_INIT                0xFFFF
#define CC2500_CRC16_POLY              0x8005
#define CC2500_CRC_MASK                0x80
#define CC2500_RSSI_POS                0
#define CC2500_LQI_POS                 1

#define CC2500_BYTES_IN_RXFIFO         0x7F
#define CC2500_WRITE_BURST             0x40
#define CC2500_READ_SINGLE             0x80
#define CC2500_READ_BURST              0xC0


#define CC2500_FIFO_THRESHOLD          0x0F
#define CC2500_FIFO_THRESHOLD_BYTES    64
#define CC2500_FIFO_SIZE               64


// CC2500/CC1100 STROBE, CONTROL AND STATUS REGSITER
#define CC2500_IOCFG2           0x00        // GDO2 output pin configuration
#define CC2500_IOCFG1           0x01        // GDO1 output pin configuration
#define CC2500_IOCFG0           0x02        // GDO0 output pin configuration
#define CC2500_FIFOTHR          0x03        // RX FIFO and TX FIFO thresholds
#define CC2500_SYNC1            0x04        // Sync word, high byte
#define CC2500_SYNC0            0x05        // Sync word, low byte
#define CC2500_PKTLEN           0x06        // Packet length
#define CC2500_PKTCTRL1         0x07        // Packet automation control
#define CC2500_PKTCTRL0         0x08        // Packet automation control
#define CC2500_ADDR             0x09        // Device address
#define CC2500_CHANNR           0x0A        // Channel number
#define CC2500_FSCTRL1          0x0B        // Frequency synthesizer control
#define CC2500_FSCTRL0          0x0C        // Frequency synthesizer control
#define CC2500_FREQ2            0x0D        // Frequency control word, high byte
#define CC2500_FREQ1            0x0E        // Frequency control word, middle byte
#define CC2500_FREQ0            0x0F        // Frequency control word, low byte
#define CC2500_MDMCFG4          0x10        // Modem configuration
#define CC2500_MDMCFG3          0x11        // Modem configuration
#define CC2500_MDMCFG2          0x12        // Modem configuration
#define CC2500_MDMCFG1          0x13        // Modem configuration
#define CC2500_MDMCFG0          0x14        // Modem configuration
#define CC2500_DEVIATN          0x15        // Modem deviation setting
#define CC2500_MCSM2            0x16        // Main Radio Control State Machine configuration
#define CC2500_MCSM1            0x17        // Main Radio Control State Machine configuration
#define CC2500_MCSM0            0x18        // Main Radio Control State Machine configuration
#define CC2500_FOCCFG           0x19        // Frequency Offset Compensation configuration
#define CC2500_BSCFG            0x1A        // Bit Synchronization configuration
#define CC2500_AGCCTRL2         0x1B        // AGC control
#define CC2500_AGCCTRL1         0x1C        // AGC control
#define CC2500_AGCCTRL0         0x1D        // AGC control
#define CC2500_WOREVT1          0x1E        // High byte Event 0 timeout
#define CC2500_WOREVT0          0x1F        // Low byte Event 0 timeout
#define CC2500_WORCTRL          0x20        // Wake On Radio control
#define CC2500_FREND1           0x21        // Front end RX configuration
#define CC2500_FREND0           0x22        // Front end TX configuration
#define CC2500_FSCAL3           0x23        // Frequency synthesizer calibration
#define CC2500_FSCAL2           0x24        // Frequency synthesizer calibration
#define CC2500_FSCAL1           0x25        // Frequency synthesizer calibration
#define CC2500_FSCAL0           0x26        // Frequency synthesizer calibration
#define CC2500_RCCTRL1          0x27        // RC oscillator configuration
#define CC2500_RCCTRL0          0x28        // RC oscillator configuration
#define CC2500_FSTEST           0x29        // Frequency synthesizer calibration control
#define CC2500_PTEST            0x2A        // Production test
#define CC2500_AGCTEST          0x2B        // AGC test
#define CC2500_TEST2            0x2C        // Various test settings
#define CC2500_TEST1            0x2D        // Various test settings
#define CC2500_TEST0            0x2E        // Various test settings

// Strobe commands
#define CC2500_SRES             0x30        // Reset chip.
#define CC2500_SFSTXON          0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CC2500_SXOFF            0x32        // Turn off crystal oscillator.
#define CC2500_SCAL             0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CC2500_SRX              0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CC2500_STX              0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CC2500_SIDLE            0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CC2500_SAFC             0x37        // Perform AFC adjustment of the frequency synthesizer
#define CC2500_SWOR             0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CC2500_SPWD             0x39        // Enter power down mode when CSn goes high.
#define CC2500_SFRX             0x3A        // Flush the RX FIFO buffer.
#define CC2500_SFTX             0x3B        // Flush the TX FIFO buffer.
#define CC2500_SWORRST          0x3C        // Reset real time clock.
#define CC2500_SNOP             0x3D        // No operation. May be used to pad strobe commands to two
                                        // bytes for simpler software.

#define CC2500_PARTNUM          0x30
#define CC2500_VERSION          0x31
#define CC2500_FREQEST          0x32
#define CC2500_LQI              0x33
#define CC2500_RSSI             0x34
#define CC2500_MARCSTATE        0x35
#define CC2500_WORTIME1         0x36
#define CC2500_WORTIME0         0x37
#define CC2500_PKTSTATUS        0x38
#define CC2500_VCO_VC_DAC       0x39
#define CC2500_TXBYTES          0x3A
#define CC2500_RXBYTES          0x3B
#define CC2500_PATABLE          0x3E

#define CC2500_TXFIFO           0x3F
#define CC2500_RXFIFO           0x3F


// cc2500 status
#define CC2500_STATUS_SLEEP         0x00
#define CC2500_STATUS_IDLE          0x01
#define CC2500_STATUS_XOFF          0x02
#define CC2500_STATUS_VCOON_MC      0x03
#define CC2500_STATUS_REGON_MC      0x04
#define CC2500_STATUS_MANCAL	     0x05
#define CC2500_STATUS_VCOON         0x06
#define CC2500_STATUS_REGON         0x07
#define CC2500_STATUS_STARTCAL      0x08
#define CC2500_STATUS_BWBOOST       0x09
#define CC2500_STATUS_FS_LOCK       0x0A
#define CC2500_STATUS_IFADCON       0x0B
#define CC2500_STATUS_ENDCAL        0x0C
#define CC2500_STATUS_RX            0x0D
#define CC2500_STATUS_RX_END        0x0E
#define CC2500_STATUS_RX_RST        0x0F
#define CC2500_STATUS_TXRX_SWITCH   0x10
#define CC2500_STATUS_RX_OVERFLOW   0x11
#define CC2500_STATUS_FSTXON        0x12
#define CC2500_STATUS_TX            0x13
#define CC2500_STATUS_TX_END        0x14
#define CC2500_STATUS_RXTX_SWITCH   0x15
#define CC2500_STATUS_TX_UNDERFLOW  0x16

// RF_SETTINGS is a data structure which contains all relevant CC2500 registers
typedef struct S_RF_SETTINGS{
    u8 FSCTRL1;   // Frequency synthesizer control.
    u8 FSCTRL0;   // Frequency synthesizer control.
    u8 FREQ2;     // Frequency control word, high byte.
    u8 FREQ1;     // Frequency control word, middle byte.
    u8 FREQ0;     // Frequency control word, low byte.
    u8 MDMCFG4;   // Modem configuration.
    u8 MDMCFG3;   // Modem configuration.
    u8 MDMCFG2;   // Modem configuration.
    u8 MDMCFG1;   // Modem configuration.
    u8 MDMCFG0;   // Modem configuration.
    u8 CHANNR;    // Channel number.
    u8 DEVIATN;   // Modem deviation setting (when FSK modulation is enabled).
    u8 FREND1;    // Front end RX configuration.
    u8 FREND0;    // Front end RX configuration.
    u8 MCSM0;     // Main Radio Control State Machine configuration.
    u8 FOCCFG;    // Frequency Offset Compensation Configuration.
    u8 BSCFG;     // Bit synchronization Configuration.
    u8 AGCCTRL2;  // AGC control.
//  u8 AGCCTRL1;  // AGC control.
    u8 AGCCTRL0;  // AGC control.
    u8 FSCAL3;    // Frequency synthesizer calibration.
    u8 FSCAL2;    // Frequency synthesizer calibration.
//  u8 FSCAL1;    // Frequency synthesizer calibration.
    u8 FSCAL0;    // Frequency synthesizer calibration.
    u8 FSTEST;    // Frequency synthesizer calibration control
    u8 TEST2;     // Various test settings.
    u8 TEST1;     // Various test settings.
    u8 TEST0;     // Various test settings.
    u8 IOCFG2;    // GDO2 output pin configuration
    u8 IOCFG0;    // GDO0 output pin configuration
    u8 PKTCTRL1;  // Packet automation control.
    u8 PKTCTRL0;  // Packet automation control.
    u8 ADDR;      // Device address.
    u8 PKTLEN;    // Packet length.
} RF_SETTINGS;

//-------------------------------------------------------------------------------------------------------
//  Global Variables

// Chipcon
// Product = CC2500
// Chip version = D
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 1.000000
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 2433.000000 MHz
// Channel spacing = 199.950000 kHz
// Channel number = 0
// Sync mode = (2) 16/16 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 25
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = Event0
// GDO2 signal selection = (6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet

void CC2500_RegConfig(u8 powerLv);
void CC2500_Init(u8 powerLv);

void CC2500_Reset(void);


void CC2500_WriteReg(u8 reg, u8 value);
u8 CC2500_ReadReg(u8 reg);
u8 CC2500_ReadStatusReg(u8 reg);
u8 CC2500_ReadRXFIFO(u8 *rxBuffer, u8 length);
void CC2500_WriteTXFIFO(u8 *txBuffer, u8 length);
void CC2500_ReadBurst(u8 addr, u8 *buffer, u8 count);
void CC2500_WriteBurst(u8 addr, const u8 *buffer, u8 count);

void CC2500_WriteCmd(u8 cmd);
BOOL CC2500_RcvMode(void);
void CC2500_SetRxState(void);
void CC2500_Wakeup(void);
BOOL CC2500_CheckRxOver(void);
void CC2500_SetAddrCheckMode(u8 addr);
void CC2500_SetFixLenPacket(u8 len);
void CC2500_SetPowerLevel(u8 lv);

#define CC2500_GDO0_WMODE_HRXTHR_LRXTHR             0x00    //1_高于RXFIFO_THR，0_低于RXFIFO_THR
#define CC2500_GDO0_WMODE_HRXTHR_RXEMPTY            0x01    //1_高于RXFIFO_THR或接收完成，0_RXFIFO空
#define CC2500_GDO0_WMODE_HTXTHR_LTXTHR             0x02    //1_高于TXFIFO_THR，0_低于TXFIFO_THR
#define CC2500_GDO0_WMODE_HTXTHR_RTEMPTY            0x03    //1_高于TXFIFO_THR或接收完成，0_TXFIFO空
#define CC2500_GDO0_WMODE_RXFLOW_RXFLUSH            0x04    //1_RXFIFO溢出，0_RXFIFO被清空
#define CC2500_GDO0_WMODE_TXFLOW_TXFLUSH            0x05    //1_TXFIFO溢出，0_TXFIFO被清空
#define CC2500_GDO0_WMODE_FRMSTART_FRMOVER          0x06    //1_开始发送/接收帧，0_完成发送接收帧
//设置GDO0的工作模式
#define CC2500_SetGDO0WorkMode(mode)                CC2500_WriteReg(CC2500_IOCFG0, (mode))
#define CC2500_SetGDO2WorkMode(mode)                CC2500_WriteReg(CC2500_IOCFG2, (mode))

void CC2500_SetRxRIFOThr(u8 len);

#define CC2500_RXTXOFF_MODE_IDLE                    0x00
#define CC2500_RXTXOFF_MODE_FSTXON                  0x01
#define CC2500_RXTXOFF_MODE_TX                      0x02
#define CC2500_RXTXOFF_MODE_RX                      0x03

#define CC2500_RXOFF_MODE_MASK                      0x0C
#define CC2500_TXOFF_MODE_MASK                      0x03



void CC2500_SetRxTxOFFMode(u8 rxOFFMode, u8 txOFFMode);

#define CC2500_AUTOCAL_NEVER            0x00
#define CC2500_AUTOCAL_IDLE2RXTX        0x10
#define CC2500_AUTOCAL_RXTX2IDLE        0x20
#define CC2500_AUTOCAL_4TH_RXTX2IDLE    0x30
#define CC2500_AUTOCAL_MASK             0x30
void CC2500_SetAutoCalibration(u8 mode);

s8 CC2500_GetRssi(u8 rssi);

#endif
