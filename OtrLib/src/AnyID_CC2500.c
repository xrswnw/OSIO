#include "AnyID_CC2500.h"

//CC2500寄存器参数表
const u8 CC2500Settings[34] = {
        0x0C,                               // FSCTRL1   Frequency synthesizer control.
        0x00,                               // FSCTRL0   Frequency synthesizer control.
        0x5D,                               // FREQ2     Frequency control word, high byte.
        0x93,                               // FREQ1     Frequency control word, middle byte.
        0xB1,                               // FREQ0     Frequency control word, low byte.
        0x2D,                               // MDMCFG4   Modem configuration.
        0xF8,                               //0x3B,   // MDMCFG3   Modem configuration.
        0x73,                               // MDMCFG2   Modem configuration.
        0x22,                               // MDMCFG1   Modem configuration.
        0xF8,                               // MDMCFG0   Modem configuration.
        0x00,                               // CHANNR    Channel number.
        0x01,                               // DEVIATN   Modem deviation setting (when FSK modulation is enabled).
        0x56,                               // FREND1    Front end RX configuration.
        0x10,                               // FREND0    Front end RX configuration.
        0x08,                               // MCSM0     Main Radio Control State Machine configuration.
        0x15,                               // FOCCFG    Frequency Offset Compensation Configuration.
        0x6C,                               // BSCFG     Bit synchronization Configuration.
        0xC3,                               // AGCCTRL2  AGC control.
//      0x00,                               // AGCCTRL1  AGC control.
        0x91,                               // AGCCTRL0  AGC control.
        0xEA,                               // FSCAL3    Frequency synthesizer calibration.
        0x0A,                               // FSCAL2    Frequency synthesizer calibration.
//      0x00,                               // FSCAL1    Frequency synthesizer calibration.
        0x11,                               // FSCAL0    Frequency synthesizer calibration.
        0x59,                               // FSTEST    Frequency synthesizer calibration.
        0x8F,                               // TEST2     Various test settings.
        0x21,                               // TEST1     Various test settings.
        0x0B,                               // TEST0     Various test settings.
        0x06,                               // IOCFG2    GDO2 output pin configuration.
        0x06,                               // IOCFG0    GDO0 output pin configuration. Refer to SmartRF?Studio User Manual for detailed pseudo register explanantion.
        0x04,                               // PKTCTRL1  Packet automation control.
        0x05,                               // PKTCTRL0  Packet automation control.
        0x00,                               // ADDR      Device address.
        0xFF                                // PKTLEN    Packet length.
/*
    //TI例程设置
    0x09,   // FSCTRL1
    0x00,   // FSCTRL0
    0x5D,   // FREQ2
    0x93,   // FREQ1
    0xB1,   // FREQ0
    0x2D,   // MDMCFG4
    0x3B,   // MDMCFG3
    0x73,   // MDMCFG2
    0x22,   // MDMCFG1
    0xF8,   // MDMCFG0
    0x00,   // CHANNR
    0x01,   // DEVIATN
    0xB6,   // FREND1
    0x10,   // FREND0
    0x18,   // MCSM0
    0x1D,   // FOCCFG
    0x1C,   // BSCFG
    0xC7,   // AGCCTRL2
    0x00,   // AGCCTRL1
    0xB2,   // AGCCTRL0
    0xEA,   // FSCAL3
    0x0A,   // FSCAL2
    0x00,   // FSCAL1
    0x11,   // FSCAL0
    0x59,   // FSTEST
    0x88,   // TEST2
    0x31,   // TEST1
    0x0B,   // TEST0
    0x06,   // IOCFG2
    0x06,   // IOCFG0D
    0x04,   // PKTCTRL1
    0x05,   // PKTCTRL0
    0x00,   // ADDR
    0xFF    // PKTLEN
*/
    };

//CC2500功率表
#define CC2500_POWERLV_MAX      17
static const u8 PowerTable[] = {0x00, 0x50, 0x44, 0xC0, 0x84, 0x81, 0x46, 0x93, 0x55, 0x8D, 0xC5, 0x97, 0x6E, 0x7F, 0xA9, 0xBB, 0xFE, 0xFF};

#define CC2500_RSSI_OFFSET      72

s8 CC2500_GetRssi(u8 rssi)
{
    s8 r = 0;
    if(rssi >= 128)
    {
        r = ((rssi - 256) / 2) - CC2500_RSSI_OFFSET;
    }
    else
    {
        r = (rssi / 2) - CC2500_RSSI_OFFSET;
    }

    return r;
}
/******************************************************************************
** 函数名：CC2500_Init
** 说  明：初始化CC2500，软复位，设置寄存器，使能GDO0中断
**
** 参  数：void
** 返  回：void
******************************************************************************/
void CC2500_Init(u8 powerLv)
{
    //如果不屏蔽中断，则可能GDO0引脚会输出的脉冲让CPU一直处于中断服务内
    CC2500_IntConfig(DISABLE);
    CC2500_Reset();
    if(powerLv > CC2500_POWERLV_MAX)
    {
        powerLv = CC2500_POWERLV_MAX;
    }
    CC2500_RegConfig(powerLv);

    CC2500_WriteCmd(CC2500_SCAL);
    CC2500_Delay10us(100);  //手动校准772us

    CC2500_IntConfig(ENABLE);
}

/******************************************************************************
** 函数名：CC2500_Reset
** 说  明：软复位CC2500
**
** 参  数：void
** 返  回：void
******************************************************************************/
void CC2500_Reset(void)
{
    CC2500_SoftReset();
    CC2500_WriteCmd(CC2500_SRES);
}


/******************************************************************************
** 函数名：CC2500_RegConfig
** 说  明：配置CC2500寄存器
**
** 参  数：void
** 返  回：void
******************************************************************************/
void CC2500_RegConfig(u8 powerLv)
{
    CC2500_WriteReg(CC2500_FSCTRL1,  CC2500Settings[0]);     //0xOC   f(if)=(26M*0x0C)/(2e10)
    CC2500_WriteReg(CC2500_FSCTRL0,  CC2500Settings[1]);   //0x00   f(offser)=26M/(2e14)=1.5869KHz
    CC2500_WriteReg(CC2500_FREQ2,    CC2500Settings[2]);   //0x5D   f(carrier)
    CC2500_WriteReg(CC2500_FREQ1,    CC2500Settings[3]);   //0x93    =(26M*0x5D93B1)/(2e16)
    CC2500_WriteReg(CC2500_FREQ0,    CC2500Settings[4]);   //0xB1    =2432.99MHz
    CC2500_WriteReg(CC2500_MDMCFG4,  CC2500Settings[5]);   //0x2D   BW(channnel)=26M/(8*(4+2)*(2e0))=0.54MHz,exponent=13
    CC2500_WriteReg(CC2500_MDMCFG3,  CC2500Settings[6]);   //0xF8   R(data)=(256+0xf8)*(2e13)*26M/(2e28)=400K
    CC2500_WriteReg(CC2500_MDMCFG2,  CC2500Settings[7]);   //0x72   MSK modulation,enable Manchester encoding/decoding,16/16 sync word bits detected
    CC2500_WriteReg(CC2500_MDMCFG1,  CC2500Settings[8]);   //0x22   preamble bytes number is 4, 2 bitexponent of channel space
    CC2500_WriteReg(CC2500_MDMCFG0,  CC2500Settings[9]);   //0xF8   ^f(channel)=26M*(256+0xf8)*(2e2)*CHAN
    CC2500_WriteReg(CC2500_CHANNR,   CC2500Settings[10]);  //0x00   the 8 bit unsigned packet number
    CC2500_WriteReg(CC2500_DEVIATN,  CC2500Settings[11]);  //0x01   f(dev)=26M*(8+0x1)*(2e0)/(2e17)=1.785KHz
    CC2500_WriteReg(CC2500_FREND1,   CC2500Settings[12]);  //0x56   Front end RX configuration
    CC2500_WriteReg(CC2500_FREND0,   CC2500Settings[13]);  //0x10   Front end TX configuration
    CC2500_WriteReg(CC2500_MCSM0 ,   CC2500Settings[14]);  //0x18   calibrate time is when going from IDEL to RX or TX(or fstxon)
    CC2500_WriteReg(CC2500_FOCCFG,   CC2500Settings[15]);  //0x15   frequency offest compensation configuraion
    CC2500_WriteReg(CC2500_BSCFG,    CC2500Settings[16]);  //0x6C   bit synchronization configuration
    CC2500_WriteReg(CC2500_AGCCTRL2, CC2500Settings[17]);  //0xC3   AGC control
    CC2500_WriteReg(CC2500_AGCCTRL0, CC2500Settings[18]);  //0x91
    CC2500_WriteReg(CC2500_FSCAL3,   CC2500Settings[19]);  //0xEA   frequency synthesizer calibration
    CC2500_WriteReg(CC2500_FSCAL2,   CC2500Settings[20]);  //0x0A
    CC2500_WriteReg(CC2500_FSCAL0,   CC2500Settings[21]);  //0x11
    CC2500_WriteReg(CC2500_FSTEST,   CC2500Settings[22]);  //0x59   frequency synthesizer calibration control
    CC2500_WriteReg(CC2500_TEST2,    CC2500Settings[23]);  //0x8F   various test settings
    CC2500_WriteReg(CC2500_TEST1,    CC2500Settings[24]);  //0x21
    CC2500_WriteReg(CC2500_TEST0,    CC2500Settings[25]);  //0x0B
    CC2500_WriteReg(CC2500_IOCFG2,   CC2500Settings[26]);  //0x24   invert output
    CC2500_WriteReg(CC2500_IOCFG0,   CC2500Settings[27]);  //0x06
    CC2500_WriteReg(CC2500_PKTCTRL1, CC2500Settings[28]);  //0x04   NO address check,
    CC2500_WriteReg(CC2500_PKTCTRL0, CC2500Settings[29]);  //0x05   variable length packets,enable CRC,Normal mode use FIFOS for rx and tx,whitening off
    CC2500_WriteReg(CC2500_ADDR,     CC2500Settings[30]);  //0x00
    CC2500_WriteReg(CC2500_PKTLEN,   CC2500Settings[31]);  //0x19   packet length when fixed length packers are enabled
/*
    //TI例程设置
    CC2500_WriteReg(CC2500_FSCTRL1,  CC2500Settings[0]);     //0xOC   f(if)=(26M*0x0C)/(2e10)
    CC2500_WriteReg(CC2500_FSCTRL0,  CC2500Settings[1]);   //0x00   f(offser)=26M/(2e14)=1.5869KHz
    CC2500_WriteReg(CC2500_FREQ2,    CC2500Settings[2]);   //0x5D   f(carrier)
    CC2500_WriteReg(CC2500_FREQ1,    CC2500Settings[3]);   //0x93    =(26M*0x5D93B1)/(2e16)
    CC2500_WriteReg(CC2500_FREQ0,    CC2500Settings[4]);   //0xB1    =2432.99MHz
    CC2500_WriteReg(CC2500_MDMCFG4,  CC2500Settings[5]);   //0x2D   BW(channnel)=26M/(8*(4+2)*(2e0))=0.54MHz,exponent=13
    CC2500_WriteReg(CC2500_MDMCFG3,  CC2500Settings[6]);   //0xF8   R(data)=(256+0xf8)*(2e13)*26M/(2e28)=400K
    CC2500_WriteReg(CC2500_MDMCFG2,  CC2500Settings[7]);   //0x72   MSK modulation,enable Manchester encoding/decoding,16/16 sync word bits detected
    CC2500_WriteReg(CC2500_MDMCFG1,  CC2500Settings[8]);   //0x22   preamble bytes number is 4, 2 bitexponent of channel space
    CC2500_WriteReg(CC2500_MDMCFG0,  CC2500Settings[9]);   //0xF8   ^f(channel)=26M*(256+0xf8)*(2e2)*CHAN
    CC2500_WriteReg(CC2500_CHANNR,   CC2500Settings[10]);  //0x00   the 8 bit unsigned packet number
    CC2500_WriteReg(CC2500_DEVIATN,  CC2500Settings[11]);  //0x01   f(dev)=26M*(8+0x1)*(2e0)/(2e17)=1.785KHz
    CC2500_WriteReg(CC2500_FREND1,   CC2500Settings[12]);  //0x56   Front end RX configuration
    CC2500_WriteReg(CC2500_FREND0,   CC2500Settings[13]);  //0x10   Front end TX configuration
    CC2500_WriteReg(CC2500_MCSM0 ,   CC2500Settings[14]);  //0x18   calibrate time is when going from IDEL to RX or TX(or fstxon)
    CC2500_WriteReg(CC2500_FOCCFG,   CC2500Settings[15]);  //0x15   frequency offest compensation configuraion
    CC2500_WriteReg(CC2500_BSCFG,    CC2500Settings[16]);  //0x6C   bit synchronization configuration
    CC2500_WriteReg(CC2500_AGCCTRL2, CC2500Settings[17]);  //0xC3   AGC control
    CC2500_WriteReg(CC2500_AGCCTRL1, CC2500Settings[18]);  //0xC3   AGC control
    CC2500_WriteReg(CC2500_AGCCTRL0, CC2500Settings[19]);  //0x91
    CC2500_WriteReg(CC2500_FSCAL3,   CC2500Settings[20]);  //0xEA   frequency synthesizer calibration
    CC2500_WriteReg(CC2500_FSCAL2,   CC2500Settings[21]);  //0x0A
    CC2500_WriteReg(CC2500_FSCAL1,   CC2500Settings[22]);  //0x0A
    CC2500_WriteReg(CC2500_FSCAL0,   CC2500Settings[23]);  //0x11
    CC2500_WriteReg(CC2500_FSTEST,   CC2500Settings[24]);  //0x59   frequency synthesizer calibration control
    CC2500_WriteReg(CC2500_TEST2,    CC2500Settings[25]);  //0x8F   various test settings
    CC2500_WriteReg(CC2500_TEST1,    CC2500Settings[26]);  //0x21
    CC2500_WriteReg(CC2500_TEST0,    CC2500Settings[27]);  //0x0B
    CC2500_WriteReg(CC2500_IOCFG2,   CC2500Settings[28]);  //0x24   invert output
    CC2500_WriteReg(CC2500_IOCFG0,   CC2500Settings[29]);  //0x06
    CC2500_WriteReg(CC2500_PKTCTRL1, CC2500Settings[30]);  //0x04   NO address check,
    CC2500_WriteReg(CC2500_PKTCTRL0, CC2500Settings[31]);  //0x05   variable length packets,enable CRC,Normal mode use FIFOS for rx and tx,whitening off
    CC2500_WriteReg(CC2500_ADDR,     CC2500Settings[32]);  //0x00
    CC2500_WriteReg(CC2500_PKTLEN,   CC2500Settings[33]);  //0x19   packet length when fixed length packers are enabled
*/
    CC2500_SetPowerLevel(powerLv);
}

void CC2500_SetPowerLevel(u8 lv)
{
    u8 i = 0;
    u8 pwTable[8] = {0};

    if(lv > CC2500_POWERLV_MAX)
    {
        lv = CC2500_POWERLV_MAX;
    }

    for(i = 0; i < 8; i++)
    {
        pwTable[i] = PowerTable[lv];
    }

    CC2500_WriteBurst(CC2500_PATABLE, pwTable, 8);
}

/******************************************************************************
** 函数名：CC2500_WriteReg
** 说  明：写CC2500寄存器
**
** 参  数：输入：reg：寄存器
                 value：写入寄存器的值
** 返  回：void
******************************************************************************/
void CC2500_WriteReg(u8 reg, u8 value)
{
    if(reg < 0x30)
    {
        CC2500_CS_Low();
        CC2500_Delayus();
        CC2500_WriteByte(reg);
        CC2500_WriteByte(value);
        CC2500_CS_High();
    }
}

/******************************************************************************
** 函数名：CC2500_ReadReg
** 说  明：写CC2500寄存器
**
** 参  数：输入：reg：寄存器
** 返  回：寄存器值
******************************************************************************/
u8 CC2500_ReadReg(u8 reg)
{
    u8 value = 0;
    if(reg < 0x30 || reg == CC2500_RXFIFO)
    {
        CC2500_CS_Low();
        CC2500_Delayus();
        CC2500_WriteByte(reg | CC2500_READ_SINGLE);
        value = CC2500_WriteByte (0xff);
        CC2500_CS_High();
    }
    return value;
}

/******************************************************************************
** 函数名：CC2500_ReadStatusReg
** 说  明：读取状态寄存器
**
** 参  数：输入：reg：寄存器
** 返  回：状态寄存器值
******************************************************************************/
u8 CC2500_ReadStatusReg(u8 reg)
{
    u8 value = 0;
    if(reg >= 0x30)
    {
        CC2500_CS_Low();
        CC2500_Delayus();
        CC2500_WriteByte(reg | CC2500_READ_BURST);
        value = CC2500_WriteByte(0xff);
        CC2500_CS_High();
    }
    return value;
}

void CC2500_WriteCmd(u8 cmd)
{
    if(cmd >= 0x30)
    {
        CC2500_CS_Low();
        CC2500_Delayus();
        CC2500_WriteByte(cmd);
        CC2500_CS_High();
    }
}


u8 CC2500_ReadRXFIFO(u8 *rxBuffer, u8 length)
{
    u8 packetLen = 0;

    if((CC2500_ReadStatusReg(CC2500_RXBYTES) & CC2500_BYTES_IN_RXFIFO))             //检测缓冲区FIFO中是否有数据
    {
        //Read packet length byte
        packetLen = CC2500_ReadReg(CC2500_RXFIFO);                                   //读取该帧数据的长度参数

        // Read data from RX FIFO and store in rxBuffer
        if(packetLen <= length)                                                     //如果设定读取的值比发送的实际长度大，读取数据
        {
            rxBuffer[0] = packetLen;
            CC2500_ReadBurst(CC2500_RXFIFO, rxBuffer + 1, packetLen);                   //读取数据
            CC2500_ReadBurst(CC2500_RXFIFO, rxBuffer + 1 + packetLen, 2);               //读取CRC校验信息和信号强度信息
        }
        else                                                                        //否则认定数据错误，丢弃
            packetLen = 0;
    }
    else
        packetLen = 0;

    return packetLen;                                                             //返回实际读取的数据长度
}


void CC2500_WriteTXFIFO(u8 *txBuffer, u8 length)
{
    CC2500_WriteCmd(CC2500_SIDLE);     //
    CC2500_Delay10us(1);
    while(CC2500_ReadStatusReg(CC2500_MARCSTATE) != CC2500_STATUS_IDLE);
    CC2500_WriteCmd(CC2500_SFTX);
    CC2500_WriteBurst(CC2500_TXFIFO, txBuffer, length); //
    CC2500_WriteCmd(CC2500_STX);     //
    CC2500_Delay10us(50);
}

void CC2500_ReadBurst(u8 addr, u8 *buffer, u8 count)
{
    u8 i;
    addr = addr | CC2500_READ_BURST;
    CC2500_CS_Low();
    CC2500_Delayus();
    CC2500_WriteByte(addr);
    for (i = 0; i < count; i++)   // read data
    {
        buffer[i] = CC2500_WriteByte(addr);
    }
    CC2500_CS_High();
}


void CC2500_WriteBurst(u8 addr, const u8 *buffer, u8 count)
{
    u8 i = 0;
    addr = addr | CC2500_WRITE_BURST;
    CC2500_CS_Low();
    CC2500_Delayus();
    CC2500_WriteByte(addr);
    for (i = 0; i < count; i++)   // read data
    {
        CC2500_WriteByte(buffer[i]);
    }
    CC2500_CS_High();
}

BOOL CC2500_RcvMode(void)
{
    BOOL b = TRUE;
    u8 state = CC2500_ReadStatusReg(CC2500_MARCSTATE);
    if(state == CC2500_STATUS_IDLE)
    {
        CC2500_WriteCmd(CC2500_SRX);
    }
    else if(state == CC2500_STATUS_RX_OVERFLOW)
    {
        CC2500_WriteCmd(CC2500_SFRX);
        CC2500_WriteCmd(CC2500_SRX);
    }
    else if(state == CC2500_STATUS_SLEEP || state == 0xFF)
    {
        b = FALSE;
    }
    else if(state == CC2500_STATUS_RX)
    {
        u8 gdo2State = 0;
        u8 delay = 100;
        do{
            CC2500_Delay10us(3);        //30us
            delay--;
            gdo2State = CC2500_ReadStatusReg(CC2500_PKTSTATUS) & 0x04;
        }while((delay != 0) && (gdo2State == 0x04));

        if(delay == 0)
        {
            b = FALSE;
        }
    }

    return b;
}

void CC2500_SetRxState(void)
{
    CC2500_WriteCmd(CC2500_SIDLE);
    CC2500_Delay10us(1);
    while(CC2500_ReadStatusReg(CC2500_MARCSTATE) != CC2500_STATUS_IDLE);
    //清空接收缓冲区，避免上次接收的数据存在缓冲区中
    CC2500_WriteCmd(CC2500_SFRX);
    CC2500_WriteCmd(CC2500_SFTX);

    CC2500_WriteCmd(CC2500_SRX);
    CC2500_Delay10us(100);
}


void CC2500_Wakeup(void)
{
    CC2500_CS_Low();
    CC2500_Delay10us(10);
    CC2500_CS_High();

    CC2500_Delay10us(80);

}

//系统设置是250kpbs
//所以接收64(外加8个前导字节)个字节数据的时间 = 72 * 8 / 250_000 = 2.3ms
BOOL CC2500_CheckRxOver(void)
{
    BOOL b = TRUE;

    u8 i = 0;
    u8 state = 0;
	for(i = 0; i < 100; i++)
    {
        state = CC2500_ReadStatusReg(CC2500_PKTSTATUS) & 0x01;
        if(state == 0x00)
        {
            break;
        }

        CC2500_Delay10us(2);
    }
    if(i == 50)
    {
        b = FALSE;
    }

    return b;
}


void CC2500_SetAddrCheckMode(u8 addr)
{
    u8 reg = 0;

    reg = CC2500_ReadReg(CC2500_PKTCTRL1);
    reg &= 0xFC;          //CC2500_PKTCTRL1<1:0> = 01 地址检测
    reg |= 0x01;
    CC2500_WriteReg(CC2500_PKTCTRL1, reg);

    CC2500_WriteReg(CC2500_ADDR, addr);
}

void CC2500_SetFixLenPacket(u8 len)
{
    u8 reg = 0;

    reg = CC2500_ReadReg(CC2500_PKTCTRL0);
    reg &= 0xFC;          //CC2500_PKTCTRL0<1:0> = 00 表示固定长度接收数据帧
    CC2500_WriteReg(CC2500_PKTCTRL0, reg);

    CC2500_WriteReg(CC2500_PKTLEN, len);
}


void CC2500_SetRxRIFOThr(u8 len)
{
    if(len > 0)
    {
        if(len > CC2500_FIFO_SIZE)
        {
            len = CC2500_FIFO_SIZE;
        }
        len = len >> 2;
        len -= 1;
    }

    CC2500_WriteReg(CC2500_FIFOTHR, len);
}

void CC2500_SetRxTxOFFMode(u8 rxOFFMode, u8 txOFFMode)
{
    u8 reg = 0;

    reg = CC2500_ReadReg(CC2500_MCSM1);

    //CC2500_MCSM1<3:0>表示TXRX_OFF Mode
    reg &= (CC2500_RXOFF_MODE_MASK | CC2500_TXOFF_MODE_MASK);
    reg |= ((rxOFFMode << 2) & CC2500_RXOFF_MODE_MASK);
    reg |= ((txOFFMode) & CC2500_TXOFF_MODE_MASK);

    CC2500_WriteReg(CC2500_MCSM1, reg);
}


void CC2500_SetAutoCalibration(u8 mode)
{
    u8 state = CC2500_ReadReg(CC2500_MCSM0);

    mode &= CC2500_AUTOCAL_MASK;
    state |= mode;
    CC2500_WriteReg(CC2500_MCSM0, state);
}

