#include "AnyID_ENC28J60.h"


/******************************************************************************
** 函数名：ENC28J60_WriteOperationCode
** 说  明: 根据操作码和地址写入数据
**
** 输  入: code：操作码
           addr：地址
           data：数据
** 输  出: void
** 返  回: void
******************************************************************************/
void ENC28J60_WriteOperationCode(u8 code, u8 addr, u8 data)
{
    ENC28J60_CS_Low();

    code = code | (addr & ENC28J60_ADDR_MASK);
    ENC28J60_WriteByte(code);
    ENC28J60_WriteByte(data);

    ENC28J60_CS_High();
}

/******************************************************************************
** 函数名：ENC28J60_ReadOperationCode
** 说  明: 根据操作码和地址读数据
**
** 输  入: code：操作码
           addr：地址
** 输  出: void
** 返  回: u8：读取的数据
******************************************************************************/
u8 ENC28J60_ReadOperationCode(u8 code, u8 addr)
{
    u8 ret = 0;

    ENC28J60_CS_Low();

    code = code | (addr & ENC28J60_ADDR_MASK);
    ENC28J60_WriteByte(code);

    ret = ENC28J60_ReadByte();

    if(addr & ENC28J60_MAC_MII_ADDR_FLAG)
    {
        ret = ENC28J60_ReadByte();
    }

    ENC28J60_CS_High();

    return ret;
}

static u8 g_nMemoryBank = 0xFF;
/******************************************************************************
** 函数名：ENC28J60_SetMemoryBank
** 说  明: 设置读写的BANK区
**
** 输  入: bank：BANK区
** 输  出: void
** 返  回: void
******************************************************************************/
void ENC28J60_SetMemoryBank(u8 bank)
{
    // set the bank (if needed)
    if((bank & ENC28J60_BANK_MASK) != g_nMemoryBank)
    {
        // set the bank
        ENC28J60_WriteOperationCode(ENC28J60_BIT_FIELD_CLR, ENC28J60_ECON1, (ENC28J60_ECON1_BSEL1 | ENC28J60_ECON1_BSEL0));

        ENC28J60_WriteOperationCode(ENC28J60_BIT_FIELD_SET, ENC28J60_ECON1, (bank & ENC28J60_BANK_MASK) >> 5);

        g_nMemoryBank = (bank & ENC28J60_BANK_MASK);
    }
}

/******************************************************************************
** 函数名：ENC28J60_Write
** 说  明: 写数据
**
** 输  入: addr：地址
           data：写入的数据
** 输  出: void
** 返  回: void
******************************************************************************/
void ENC28J60_Write(u8 addr, u8 data)
{
    //select bank to write
    ENC28J60_SetMemoryBank(addr);

    //do the write
    ENC28J60_WriteOperationCode(ENC28J60_WRITE_CTRL_REG, addr, data);
}

/******************************************************************************
** 函数名：ENC28J60_Read
** 说  明: 读数据
**
** 输  入: addr：地址
** 输  出: void
** 返  回: u8：读取的数据
******************************************************************************/
u8 ENC28J60_Read(u8 addr)
{
    //select bank to reads
    ENC28J60_SetMemoryBank(addr);

    //do the read
    return ENC28J60_ReadOperationCode(ENC28J60_READ_CTRL_REG, addr);
}

/******************************************************************************
** 函数名：ENC28J60_WritePhy
** 说  明: 写物理区数据
**
** 输  入: addr：地址
           data：数据
** 输  出: void
** 返  回: void
******************************************************************************/
void ENC28J60_WritePhy(u8 addr, u16 data)
{
    // set the PHY register address
    ENC28J60_Write(ENC28J60_MIREGADR, addr);

    // write the PHY data, the low first
    ENC28J60_Write(ENC28J60_MIWRL, (data >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_MIWRH, (data >> 8) & 0xFF);

    // wait until the PHY write completes
    while(ENC28J60_Read(ENC28J60_MISTAT) & ENC28J60_MISTAT_BUSY);
}

/******************************************************************************
** 函数名：ENC28J60_ReadPhy
** 说  明: 读物理区数据
**
** 输  入: addr：地址
** 输  出: void
** 返  回: u16：读取的数据
******************************************************************************/
u16 ENC28J60_ReadPhy(u8 addr)
{
    u16 data = 0;
    u16 low = 0, high = 0;

    // set the PHY register address
    ENC28J60_Write(ENC28J60_MIREGADR, addr);

    ENC28J60_Write(ENC28J60_MICMD, ENC28J60_MICMD_MIIRD);

    // Loop to wait until the PHY register has been read through the MII
    // This requires 10.24us
    while((ENC28J60_Read(ENC28J60_MISTAT) & ENC28J60_MISTAT_BUSY));

    // Stop reading
    ENC28J60_Write(ENC28J60_MICMD, ENC28J60_MICMD_MIIRD);

    // Obtain results and return
    low = ENC28J60_Read(ENC28J60_MIRDL);
    high = (ENC28J60_Read(ENC28J60_MIRDH) << 8);

    data = (high << 8) | low;

    return data;
}

/******************************************************************************
** 函数名：ENC28J60_WriteBuffer
** 说  明: 写ENC28J60缓冲区
**
** 输  入: len：写入的数据长度
           pBuffer：数据源缓冲区首地址
** 输  出: void
** 返  回: void
******************************************************************************/
void ENC28J60_WriteBuffer(u16 len, u8 *pBuffer)
{
    u16 i = 0;

    ENC28J60_CS_Low();

    //write buffer cmd
    ENC28J60_WriteByte(ENC28J60_WRITE_BUF_MEM);

    for(i = 0; i < len; i++)
    {
        ENC28J60_WriteByte(pBuffer[i]);
    }

    ENC28J60_CS_High();
}

/******************************************************************************
** 函数名：ENC28J60_ReadBuffer
** 说  明: 读ENC28J60缓冲区
**
** 输  入: len：写入的数据长度
** 输  出: pBuffer：数据目的缓冲区首地址
** 返  回: void
******************************************************************************/
void ENC28J60_ReadBuffer(u16 len, u8 *pBuffer)
{
    u16 i = 0;

    ENC28J60_CS_Low();

    //read buffer cmd
    ENC28J60_WriteByte(ENC28J60_READ_BUF_MEM);

    for(i = 0; i < len; i++)
    {
        pBuffer[i] = ENC28J60_ReadByte();
    }

    ENC28J60_CS_High();
}

/******************************************************************************
** 函数名：ENC28J60_SendPacket
** 说  明: 发送数据帧
**
** 输  入: len：写入的数据长度
           pPacket：数据帧源缓冲区首地址
** 输  出: void
** 返  回: void
******************************************************************************/
void ENC28J60_SendPacket(u16 len, u8 *pPacket)
{
    //Set the write pointer to start of transmit buffer area
    ENC28J60_Write(ENC28J60_EWRPTL, (ENC28J60_TXSTART_INIT >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_EWRPTH, (ENC28J60_TXSTART_INIT >> 8) & 0xFF);

    //Set the TXND pointer to correspond to the packet size given
    //发送区开始位置已经确定
    ENC28J60_Write(ENC28J60_ETXNDL, ((ENC28J60_TXSTART_INIT + len) >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_ETXNDH, ((ENC28J60_TXSTART_INIT + len) >> 8) & 0xFF);

    //write per-packet control byte (0x00 means use macon3 settings)
    //发送缓冲区的第一个字节是包控制格式，说明是使用MACON3，还是当前寄存器中的包控制格式
    ENC28J60_WriteOperationCode(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // copy the packet into the transmit buffer
     ENC28J60_WriteBuffer(len, pPacket);

    // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
    if((ENC28J60_Read(ENC28J60_EIR) & ENC28J60_EIR_TXERIF))
    {
        ENC28J60_WriteOperationCode(ENC28J60_BIT_FIELD_CLR, ENC28J60_ECON1, ENC28J60_ECON1_TXRTS);
    }

    // send the contents of the transmit buffer onto the network
    ENC28J60_WriteOperationCode(ENC28J60_BIT_FIELD_SET, ENC28J60_ECON1, ENC28J60_ECON1_TXRTS);

    while(1)
    {
        u8 rcv = ENC28J60_Read(ENC28J60_ECON1) & ENC28J60_ECON1_TXRTS;
        if(rcv == 0)
        {
            break;
        }
    }
}


/******************************************************************************
** 函数名：ENC28J60_ReceivdPacket
** 说  明: 接收数据帧
**
** 输  入: void
** 输  出: pPacket：数据帧目的缓冲区首地址
** 返  回: u16：数据帧长度
******************************************************************************/
u16 g_nNextPacketPtr = 0;
u16 ENC28J60_ReceivdPacket(u8 *pPacket)
{
    u16 rxStat = 0;
    u16 len = 0;
    // check if a packet has been received and buffered
    //if( !(enc28j60Read(EIR) & EIR_PKTIF) ){

    // The above does not work. See Rev. B4 Silicon Errata point 6.
    if(ENC28J60_Read(ENC28J60_EPKTCNT) == 0)
    {
        len = 0;
    }
    else
    {
        // Set the read pointer to the start of the received packet
        // 设置接收缓冲区开始指针
        ENC28J60_Write(ENC28J60_ERDPTL, (g_nNextPacketPtr >> 0) & 0xFF);
        ENC28J60_Write(ENC28J60_ERDPTH, (g_nNextPacketPtr >> 8) & 0xFF);

        // read the next packet pointer
        // 接收数据包接口的开始两个字节是下个数据包开始的起始位置
        g_nNextPacketPtr  = ENC28J60_ReadOperationCode(ENC28J60_READ_BUF_MEM, 0);
        g_nNextPacketPtr |= (ENC28J60_ReadOperationCode(ENC28J60_READ_BUF_MEM, 0) << 8);

        // read the packet length (see datasheet page 43)
        len  = ENC28J60_ReadOperationCode(ENC28J60_READ_BUF_MEM, 0);
        len |= (ENC28J60_ReadOperationCode(ENC28J60_READ_BUF_MEM, 0) << 8);
        //remove the CRC count
        len -= 4;

        // read the receive status (see datasheet page 43)
        rxStat  = ENC28J60_ReadOperationCode(ENC28J60_READ_BUF_MEM, 0);
        rxStat |= ENC28J60_ReadOperationCode(ENC28J60_READ_BUF_MEM, 0) << 8;

        // limit retrieve length
        if(len > ENC28J60_MAX_FRAMELEN - 1)
        {
            len = ENC28J60_MAX_FRAMELEN - 1;
        }

        // check CRC and symbol errors (see datasheet page 44, table 7-3):
        // The ERXFCON.CRCEN is set by default. Normally we should not need to check this.
        if((rxStat & ENC28J60_RECEIVE_OK_FLAG) == 0)
        {
            // invalid
            len = 0;
        }
        else
        {
            // copy the packet from the receive buffer
            ENC28J60_ReadBuffer(len, pPacket);
        }

        // Move the RX read pointer to the start of the next received packet
        // This frees the memory we just read out
        ENC28J60_Write(ENC28J60_ERXRDPTL, (g_nNextPacketPtr >> 0) & 0xFF);
        ENC28J60_Write(ENC28J60_ERXRDPTH, (g_nNextPacketPtr >> 8) & 0xFF);

        // decrement the packet counter indicate we are done with this packet
        ENC28J60_WriteOperationCode(ENC28J60_BIT_FIELD_SET, ENC28J60_ECON2, ENC28J60_ECON2_PKTDEC);
    }

    return(len);
}

/******************************************************************************
** 函数名：ENC28J60_Init
** 说  明: 初始化ENC28J60
**
** 输  入: pMac：MAC地址，6字节
** 输  出: void
** 返  回: void
******************************************************************************/
void ENC28J60_Init(u8 *pMac)
{
    ENC28J60_Reset_Low();
    ENC28J60_Delayms(20);
    ENC28J60_Reset_High();
    ENC28J60_Delayms(400);


    ENC28J60_WriteOperationCode(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    ENC28J60_Delayms(50);

    //switch to bank 0
    ENC28J60_SetMemoryBank(ENC28J60_ERDPTL);

    //25M时钟输出
    ENC28J60_Write(ENC28J60_ECOCON, 0x02);

    //等待系统时钟稳定
    while((ENC28J60_Read(ENC28J60_ESTAT) & ENC28J60_ESTAT_CLKRDY) != ENC28J60_ESTAT_CLKRDY);



    // check CLKRDY bit to see if reset is complete
    // The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
    // while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
    // do bank 0 stuff
    // initialize receive buffer
    // 16-bit transfers, must write low byte first
    // set receive buffer start address
    g_nNextPacketPtr = ENC28J60_RXSTART_INIT;

    //接收循环队列
    // Rx start 接收Buffer开始的位置
    ENC28J60_Write(ENC28J60_ERXSTL, (ENC28J60_RXSTART_INIT >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_ERXSTH, (ENC28J60_RXSTART_INIT >> 8) & 0xFF);
    // set receive pointer address 接收指针
    ENC28J60_Write(ENC28J60_ERXRDPTL, (ENC28J60_RXSTART_INIT >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_ERXRDPTH, (ENC28J60_RXSTART_INIT >> 8) & 0xFF);
    // RX end   接收buffer结束的位置
    ENC28J60_Write(ENC28J60_ERXNDL, (ENC28J60_RXSTOP_INIT >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_ERXNDH, (ENC28J60_RXSTOP_INIT >> 8) & 0xFF);

    // TX start 发送Buffer开始的位置
    ENC28J60_Write(ENC28J60_ETXSTL, (ENC28J60_TXSTART_INIT >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_ETXSTH, (ENC28J60_TXSTART_INIT >> 8) & 0xFF);
    // TX end   发送Buffer结束的位置
    ENC28J60_Write(ENC28J60_ETXNDL, (ENC28J60_TXSTOP_INIT >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_ETXNDH, (ENC28J60_TXSTOP_INIT >> 8) & 0xFF);

    // do bank 2 stuff
    // bring MAC out of reset
    ENC28J60_Write(ENC28J60_MACON2, 0x00);

    // enable MAC receive，流量控制使能
    ENC28J60_Write(ENC28J60_MACON1, ENC28J60_MACON1_MARXEN | ENC28J60_MACON1_TXPAUS | ENC28J60_MACON1_RXPAUS);

    // enable automatic padding to 60bytes and CRC operations
    //使能补零、CRC，帧长度状态报告，全双工
    ENC28J60_Write(ENC28J60_MACON3, ENC28J60_MACON3_PADCFG0 | ENC28J60_MACON3_TXCRCEN | ENC28J60_MACON3_FRMLNEN | ENC28J60_MACON3_FULDPX);

    // Allow infinite deferals if the medium is continuously busy
    // (do not time out a transmission if the half duplex medium is
    // completely saturated with other people's data)
    ENC28J60_Write(ENC28J60_MACON4, ENC28J60_MACON4_DEFER);

    // Set the maximum packet size which the controller will accept
    // Do not send packets longer than MAX_FRAMELEN:
    // 最大帧1500
    ENC28J60_Write(ENC28J60_MAMXFLL, (ENC28J60_MAX_FRAMELEN >> 0) & 0xFF);
    ENC28J60_Write(ENC28J60_MAMXFLH, (ENC28J60_MAX_FRAMELEN >> 8) & 0xFF);

    // set inter-frame gap (back-to-back)
    // 0x12 半双工；0x15全双工
    ENC28J60_Write(ENC28J60_MABBIPG, 0x15);

    // Set non-back-to-back inter-packet gap to 9.6us.  The back-to-back
    // inter-packet gap (MABBIPG) is set by MACSetDuplex() which is called
    // later.
    ENC28J60_Write(ENC28J60_MAIPGL, 0x12);
    ENC28J60_Write(ENC28J60_MAIPGH, 0x0C);

    // Late collisions occur beyond 63+8 bytes (8 bytes for preamble/start of frame delimiter)
    // 55 is all that is needed for IEEE 802.3, but ENC28J60 B5 errata for improper link pulse
    // collisions will occur less often with a larger number.
    ENC28J60_Write(ENC28J60_MACLCON2, 63);

    // do bank 3 stuff
    // write MAC address
    // NOTE: MAC address in ENC28J60 is byte-backward
    // ENC28J60 is big-endian avr gcc is little-endian
    ENC28J60_Write(ENC28J60_MAADR5, pMac[0]);
    ENC28J60_Write(ENC28J60_MAADR4, pMac[1]);
    ENC28J60_Write(ENC28J60_MAADR3, pMac[2]);
    ENC28J60_Write(ENC28J60_MAADR2, pMac[3]);
    ENC28J60_Write(ENC28J60_MAADR1, pMac[4]);
    ENC28J60_Write(ENC28J60_MAADR0, pMac[5]);

    // Magjack leds configuration, see enc28j60 datasheet, page 11
    // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
    ENC28J60_WritePhy(ENC28J60_PHLCON, 0x0472);

    //半双/全双工由LEDB决定
    ENC28J60_WritePhy(ENC28J60_PHCON1, ENC28J60_PHCON1_PDPXMD);

    // no loopback of transmitted frames
    ENC28J60_WritePhy(ENC28J60_PHCON2, ENC28J60_PHCON2_HDLDIS);

    // do bank 1 stuff, packet filter:
    // For broadcast packets we allow only ARP packtets
    // All other packets should be unicast only for our mac (MAADR)
    //
    // The pattern to match on is therefore
    // Type     ETH.DST
    // ARP      BROADCAST
    // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
    // in binary these poitions are:11 0000 0011 1111
    // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
    ENC28J60_Write(ENC28J60_ERXFCON, ENC28J60_ERXFCON_UCEN | ENC28J60_ERXFCON_CRCEN | ENC28J60_ERXFCON_PMEN);
    ENC28J60_Write(ENC28J60_EPMM0, 0x3f);
    ENC28J60_Write(ENC28J60_EPMM1, 0x30);
    ENC28J60_Write(ENC28J60_EPMCSL, 0xf9);
    ENC28J60_Write(ENC28J60_EPMCSH, 0xf7);

    //enable interrutps
    ENC28J60_WriteOperationCode(ENC28J60_BIT_FIELD_SET, ENC28J60_EIE, ENC28J60_EIE_INTIE | ENC28J60_EIE_PKTIE | ENC28J60_EIE_RXERIE);

    // enable packet reception
    ENC28J60_WriteOperationCode(ENC28J60_BIT_FIELD_SET, ENC28J60_ECON1, ENC28J60_ECON1_RXEN);

    //自动增加缓冲区读写指针
    ENC28J60_Write(ENC28J60_ECON2, ENC28J60_ECON2_AUTOINC);

    ENC28J60_Delayms(20);
}



