#ifndef _ANYID_ENC28J60_
#define _ANYID_ENC28J60_

#if defined(_ANYID_INFCOLLECTION_)
    #include "AnyID_InfCollection_ENC28J60_HL.h"
#elif defined(_ANYID_GENERALDEVICE_)
    #include "AnyID_GeneralDevice_ENC28J60_HL.h"
#endif



// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
#define ENC28J60_ADDR_MASK        0x1F
#define ENC28J60_BANK_MASK        0x60
#define ENC28J60_SPRD_MASK        0x80
// All-bank registers
#define ENC28J60_EIE              0x1B
#define ENC28J60_EIR              0x1C
#define ENC28J60_ESTAT            0x1D
#define ENC28J60_ECON2            0x1E
#define ENC28J60_ECON1            0x1F
// Bank 0 registers
#define ENC28J60_ERDPTL           (0x00|0x00)
#define ENC28J60_ERDPTH           (0x01|0x00)
#define ENC28J60_EWRPTL           (0x02|0x00)
#define ENC28J60_EWRPTH           (0x03|0x00)
#define ENC28J60_ETXSTL           (0x04|0x00)
#define ENC28J60_ETXSTH           (0x05|0x00)
#define ENC28J60_ETXNDL           (0x06|0x00)
#define ENC28J60_ETXNDH           (0x07|0x00)
#define ENC28J60_ERXSTL           (0x08|0x00)
#define ENC28J60_ERXSTH           (0x09|0x00)
#define ENC28J60_ERXNDL           (0x0A|0x00)
#define ENC28J60_ERXNDH           (0x0B|0x00)
#define ENC28J60_ERXRDPTL         (0x0C|0x00)
#define ENC28J60_ERXRDPTH         (0x0D|0x00)
#define ENC28J60_ERXWRPTL         (0x0E|0x00)
#define ENC28J60_ERXWRPTH         (0x0F|0x00)
#define ENC28J60_EDMASTL          (0x10|0x00)
#define ENC28J60_EDMASTH          (0x11|0x00)
#define ENC28J60_EDMANDL          (0x12|0x00)
#define ENC28J60_EDMANDH          (0x13|0x00)
#define ENC28J60_EDMADSTL         (0x14|0x00)
#define ENC28J60_EDMADSTH         (0x15|0x00)
#define ENC28J60_EDMACSL          (0x16|0x00)
#define ENC28J60_EDMACSH          (0x17|0x00)
// Bank 1 registers
#define ENC28J60_EHT0             (0x00|0x20)
#define ENC28J60_EHT1             (0x01|0x20)
#define ENC28J60_EHT2             (0x02|0x20)
#define ENC28J60_EHT3             (0x03|0x20)
#define ENC28J60_EHT4             (0x04|0x20)
#define ENC28J60_EHT5             (0x05|0x20)
#define ENC28J60_EHT6             (0x06|0x20)
#define ENC28J60_EHT7             (0x07|0x20)
#define ENC28J60_EPMM0            (0x08|0x20)
#define ENC28J60_EPMM1            (0x09|0x20)
#define ENC28J60_EPMM2            (0x0A|0x20)
#define ENC28J60_EPMM3            (0x0B|0x20)
#define ENC28J60_EPMM4            (0x0C|0x20)
#define ENC28J60_EPMM5            (0x0D|0x20)
#define ENC28J60_EPMM6            (0x0E|0x20)
#define ENC28J60_EPMM7            (0x0F|0x20)
#define ENC28J60_EPMCSL           (0x10|0x20)
#define ENC28J60_EPMCSH           (0x11|0x20)
#define ENC28J60_EPMOL            (0x14|0x20)
#define ENC28J60_EPMOH            (0x15|0x20)
#define ENC28J60_EWOLIE           (0x16|0x20)
#define ENC28J60_EWOLIR           (0x17|0x20)
#define ENC28J60_ERXFCON          (0x18|0x20)
#define ENC28J60_EPKTCNT          (0x19|0x20)

//这里是为了便于写程序，因为MAC和MII的读操作有别于其他寄存器，详细见手册
#define ENC28J60_MAC_MII_ADDR_FLAG                  0x80
// Bank 2 registers
#define ENC28J60_MACON1           (0x00|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MACON2           (0x01|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MACON3           (0x02|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MACON4           (0x03|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MABBIPG          (0x04|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAIPGL           (0x06|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAIPGH           (0x07|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MACLCON1         (0x08|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MACLCON2         (0x09|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAMXFLL          (0x0A|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAMXFLH          (0x0B|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAPHSUP          (0x0D|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MICON            (0x11|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MICMD            (0x12|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MIREGADR         (0x14|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MIWRL            (0x16|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MIWRH            (0x17|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MIRDL            (0x18|0x40|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MIRDH            (0x19|0x40|ENC28J60_MAC_MII_ADDR_FLAG)

// Bank 3 registers
#define ENC28J60_MAADR1           (0x00|0x60|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAADR0           (0x01|0x60|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAADR3           (0x02|0x60|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAADR2           (0x03|0x60|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAADR5           (0x04|0x60|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_MAADR4           (0x05|0x60|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_EBSTSD           (0x06|0x60)
#define ENC28J60_EBSTCON          (0x07|0x60)
#define ENC28J60_EBSTCSL          (0x08|0x60)
#define ENC28J60_EBSTCSH          (0x09|0x60)
#define ENC28J60_MISTAT           (0x0A|0x60|ENC28J60_MAC_MII_ADDR_FLAG)
#define ENC28J60_EREVID           (0x12|0x60)
#define ENC28J60_ECOCON           (0x15|0x60)
#define ENC28J60_EFLOCON          (0x17|0x60)
#define ENC28J60_EPAUSL           (0x18|0x60)
#define ENC28J60_EPAUSH           (0x19|0x60)

// PHY registers
#define ENC28J60_PHCON1           0x00
#define ENC28J60_PHSTAT1          0x01
#define ENC28J60_PHHID1           0x02
#define ENC28J60_PHHID2           0x03
#define ENC28J60_PHCON2           0x10
#define ENC28J60_PHSTAT2          0x11
#define ENC28J60_PHIE             0x12
#define ENC28J60_PHIR             0x13
#define ENC28J60_PHLCON           0x14

// ENC28J60 ERXFCON Register Bit Definitions
#define ENC28J60_ERXFCON_UCEN     0x80
#define ENC28J60_ERXFCON_ANDOR    0x40
#define ENC28J60_ERXFCON_CRCEN    0x20
#define ENC28J60_ERXFCON_PMEN     0x10
#define ENC28J60_ERXFCON_MPEN     0x08
#define ENC28J60_ERXFCON_HTEN     0x04
#define ENC28J60_ERXFCON_MCEN     0x02
#define ENC28J60_ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define ENC28J60_EIE_INTIE        0x80
#define ENC28J60_EIE_PKTIE        0x40
#define ENC28J60_EIE_DMAIE        0x20
#define ENC28J60_EIE_LINKIE       0x10
#define ENC28J60_EIE_TXIE         0x08
#define ENC28J60_EIE_WOLIE        0x04
#define ENC28J60_EIE_TXERIE       0x02
#define ENC28J60_EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define ENC28J60_EIR_PKTIF        0x40
#define ENC28J60_EIR_DMAIF        0x20
#define ENC28J60_EIR_LINKIF       0x10
#define ENC28J60_EIR_TXIF         0x08
#define ENC28J60_EIR_WOLIF        0x04
#define ENC28J60_EIR_TXERIF       0x02
#define ENC28J60_EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ENC28J60_ESTAT_INT        0x80
#define ENC28J60_ESTAT_LATECOL    0x10
#define ENC28J60_ESTAT_RXBUSY     0x04
#define ENC28J60_ESTAT_TXABRT     0x02
#define ENC28J60_ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ENC28J60_ECON2_AUTOINC    0x80
#define ENC28J60_ECON2_PKTDEC     0x40
#define ENC28J60_ECON2_PWRSV      0x20
#define ENC28J60_ECON2_VRPS       0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ENC28J60_ECON1_TXRST      0x80
#define ENC28J60_ECON1_RXRST      0x40
#define ENC28J60_ECON1_DMAST      0x20
#define ENC28J60_ECON1_CSUMEN     0x10
#define ENC28J60_ECON1_TXRTS      0x08
#define ENC28J60_ECON1_RXEN       0x04
#define ENC28J60_ECON1_BSEL1      0x02
#define ENC28J60_ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define ENC28J60_MACON1_LOOPBK    0x10
#define ENC28J60_MACON1_TXPAUS    0x08
#define ENC28J60_MACON1_RXPAUS    0x04
#define ENC28J60_MACON1_PASSALL   0x02
#define ENC28J60_MACON1_MARXEN    0x01
// ENC28J60 MACON2 Register Bit Definitions
#define ENC28J60_MACON2_MARST     0x80
#define ENC28J60_MACON2_RNDRST    0x40
#define ENC28J60_MACON2_MARXRST   0x08
#define ENC28J60_MACON2_RFUNRST   0x04
#define ENC28J60_MACON2_MATXRST   0x02
#define ENC28J60_MACON2_TFUNRST   0x01
// ENC28J60 MACON3 Register Bit Definitions
#define ENC28J60_MACON3_PADCFG2   0x80
#define ENC28J60_MACON3_PADCFG1   0x40
#define ENC28J60_MACON3_PADCFG0   0x20
#define ENC28J60_MACON3_TXCRCEN   0x10
#define ENC28J60_MACON3_PHDRLEN   0x08
#define ENC28J60_MACON3_HFRMLEN   0x04
#define ENC28J60_MACON3_FRMLNEN   0x02
#define ENC28J60_MACON3_FULDPX    0x01
// ENC28J60 MACON4 Register Bit Definitions
#define ENC28J60_MACON4_DEFER   (1<<6)
#define ENC28J60_MACON4_BPEN    (1<<5)
#define ENC28J60_MACON4_NOBKOFF (1<<4)

// ENC28J60 MICMD Register Bit Definitions
#define ENC28J60_MICMD_MIISCAN    0x02
#define ENC28J60_MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define ENC28J60_MISTAT_NVALID    0x04
#define ENC28J60_MISTAT_SCAN      0x02
#define ENC28J60_MISTAT_BUSY      0x01
// ENC28J60 PHY PHCON1 Register Bit Definitions
#define ENC28J60_PHCON1_PRST      0x8000
#define ENC28J60_PHCON1_PLOOPBK   0x4000
#define ENC28J60_PHCON1_PPWRSV    0x0800
#define ENC28J60_PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define ENC28J60_PHSTAT1_PFDPX    0x1000
#define ENC28J60_PHSTAT1_PHDPX    0x0800
#define ENC28J60_PHSTAT1_LLSTAT   0x0004
#define ENC28J60_PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define ENC28J60_PHCON2_FRCLINK   0x4000
#define ENC28J60_PHCON2_TXDIS     0x2000
#define ENC28J60_PHCON2_JABBER    0x0400
#define ENC28J60_PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define ENC28J60_PKTCTRL_PHUGEEN  0x08
#define ENC28J60_PKTCTRL_PPADEN   0x04
#define ENC28J60_PKTCTRL_PCRCEN   0x02
#define ENC28J60_PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF


//这里应该在RXBUFFER后面加7个字节的接收向量表的空间
//因为很少接收1500字节的内容，这里不可以不做处理
//应该是一个Bug
// The RXSTART_INIT should be zero. See Rev. B4 Silicon Errata
// buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated
//
// start with recbuf at 0/
#define ENC28J60_RXSTART_INIT     0x0
// receive buffer end
#define ENC28J60_RXSTOP_INIT      (0x1FFF-0x0600-1)
// start TX buffer at 0x1FFF-0x0600, pace for one full ethernet frame (~1500 bytes)
#define ENC28J60_TXSTART_INIT     (0x1FFF-0x0600)
// stp TX buffer at end of mem
#define ENC28J60_TXSTOP_INIT      0x1FFF
//
// max frame length which the conroller will accept:
#define ENC28J60_MAX_FRAMELEN        1500        // (note: maximum ethernet frame length would be 1518)
//#define MAX_FRAMELEN     600

#define ENC28J60_RECEIVE_OK_FLAG        0x0080

void ENC28J60_WriteOperationCode(u8 code, u8 addr, u8 data);
u8 ENC28J60_ReadOperationCode(u8 code, u8 addr);
void ENC28J60_SetMemoryBank(u8 bank);
void ENC28J60_Write(u8 addr, u8 data);
u8 ENC28J60_Read(u8 addr);
void ENC28J60_WritePhy(u8 addr, u16 data);
u16 ENC28J60_ReadPhy(u8 addr);
void ENC28J60_WriteBuffer(u16 len, u8 *pBuffer);
void ENC28J60_ReadBuffer(u16 len, u8 *pBuffer);
void ENC28J60_SendPacket(u16 len, u8 *pPacket);
u16 ENC28J60_ReceivdPacket(u8 *pPacket);
void ENC28J60_Init(u8 *pMac);

#endif