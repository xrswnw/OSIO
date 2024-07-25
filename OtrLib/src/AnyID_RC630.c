#include "AnyID_RC630.h"
RC630_FRAME g_sRc630Frame = {0};

const u8 g_sRc630RegAdd[RC630_PROTOCOL_CONFIG_SIZE] =
{
    RC630_REG_DRVMODE,
    RC630_REG_TXAMP,
    RC630_REG_DRVCON,
    RC630_REG_TXI,
    RC630_REG_TXCRCCON,
    RC630_REG_RXCRCCON,
    RC630_REG_TXDATANUM,
    RC630_REG_TXMODWIDTH,
    RC630_REG_TXSYM10BURSTLEN,
    RC630_REG_TXWAITCTRL,
    RC630_REG_FRAMECON,
    RC630_REG_RXCTRL,
    RC630_REG_RXTHRESHOLD,
    RC630_REG_RCV,
    RC630_REG_RXANA
};  


void RC630_WriteReg(u8 addr, u8 val)
{
    RC630_CS_Low();

    RC630_WriteByte(RC630_WADDR(addr));
    RC630_WriteByte(val);

    RC630_CS_High();
}

u8 RC630_ReadReg(u8 addr)
{
    u8 val = 0;

    RC630_CS_Low();
   
    RC630_WriteByte(RC630_RADDR(addr));
    val = RC630_WriteByte(0xFF);

    RC630_CS_High();

    return val;
}

void RC630_SetRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = RC630_ReadReg(addr);   
    val |= bit;
    RC630_WriteReg(addr, val);
}

void RC630_ClearRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = RC630_ReadReg(addr);
    val &= (~bit);
    RC630_WriteReg(addr, val);
}

void RC630_ReadFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    RC630_CS_Low();

    RC630_WriteByte(RC630_RADDR(RC630_REG_FIFODATA));

    for(i = 0; i < (len - 1); i++)
    {
        pBuffer[i] = RC630_WriteByte(RC630_RADDR(RC630_REG_FIFODATA));
    }

    pBuffer[i] = RC630_WriteByte(0xFF);

    RC630_CS_High();
}

void RC630_WriteFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    RC630_CS_Low();

    RC630_WriteByte(RC630_WADDR(RC630_REG_FIFODATA));

    for(i = 0; i < len; i++)
    {
        RC630_WriteByte(pBuffer[i]);
    }

    RC630_CS_High();
}

BOOL RC630_Reset(void)
{
    u8 regVal = RC630_STAT_ERROR;
    BOOL bOk = FALSE;
    u8 t = 0;

    //系统硬件复位
    RC630_PowerHigh();                                          //低电平脉冲宽度大于100ns即可
    RC630_Delay16us(10);
    RC630_PowerLow();

    //等待系统稳定
    do{
        RC630_Delay16us(1000);
        t++;
        regVal = RC630_ReadReg(RC630_REG_VERSION);
        if((regVal != 0) && (regVal != 0xFF))
        {
            bOk = TRUE;
            break;
        }
    }while(t < 100);

    return bOk;
}

u8 RC630_Init(void)
{
    u8 state = RC630_STAT_ERROR;
    if(RC630_Reset())
    {
        state = RC630_STAT_OK;
    }

    RC630_WriteReg(RC630_REG_COMMAND, 0x00);
    //256字节、清空FIFO、waterLevel8bit，fifoLength8bit
    RC630_WriteReg(RC630_REG_FIFOCONTROL, RC630_BIT_FIFOSIZE | RC630_BIT_FLUSHFIFO);
    //WaterLevel = 64字节
    RC630_WriteReg(RC630_REG_WATERLEVEL, RC630_FIFO_LEVEL);
    
    
    return state;
}

void RC630_Config(u8 bClearCrypt, u8 bEnTxCrc, u8 bEnRxCrc, u8 txLastBits, u8 rxAlignBits)
{
    u8 reg = 0;

    if(bClearCrypt)
    {
        RC630_ClearRegBit(RC630_REG_STATUS, RC630_BIT_CRYPTO1ON);
    }

    if(bEnTxCrc)
    {
        RC630_SetRegBit(RC630_REG_TXCRCCON, RC630_BIT_CRCEN);
    }
    else
    {
        RC630_ClearRegBit(RC630_REG_TXCRCCON, RC630_BIT_CRCEN);
    }

    if(bEnRxCrc)
    {
        RC630_SetRegBit(RC630_REG_RXCRCCON, RC630_BIT_CRCEN);
    }
    else
    {
        RC630_ClearRegBit(RC630_REG_RXCRCCON, RC630_BIT_CRCEN);
    }

    reg = RC630_ReadReg(RC630_REG_TXDATANUM);
    reg &= (~RC630_MASK_TXLASTBITS);
    reg |= (RC630_MASK_TXLASTBITS & txLastBits);
    RC630_WriteReg(RC630_REG_TXDATANUM, reg);

    reg = RC630_ReadReg(RC630_REG_RXBITCTRL);
    reg &= (~RC630_MASK_RXALIGN);
    reg |= ((rxAlignBits << 4) & RC630_MASK_RXALIGN);
    RC630_WriteReg(RC630_REG_RXBITCTRL, reg);
}

u8 RC630_SelectProtocol(u8 tx, u8 rx)
{
    g_sRc630Frame.frame[0] = rx;
    g_sRc630Frame.frame[1] = tx;
    g_sRc630Frame.txLen = 2;

    g_sRc630Frame.timeout = 10000;
    g_sRc630Frame.cmd = RC630_CMD_LOADPROTOCOL;

    return RC630_Command();
}

u8 RC630_TopazCommand(void)
{
    u8 pos = 0;
    u8 state = RC630_STAT_OK;
    u8 reg = 0;
    u32 tick = 0;

    reg = RC630_ReadReg(RC630_REG_FRAMECON);
    RC630_WriteReg(RC630_REG_FRAMECON, reg & (~RC630_BIT_TXPARITYEN));

    //终止当前所有操作
    RC630_WriteReg(RC630_REG_COMMAND, RC630_CMD_IDLE);
    //清空FIFO
    RC630_FlushFifo();


    RC630_Config(FALSE, FALSE, FALSE, 7, 0);
    RC630_WriteFIFO(g_sRc630Frame.frame, 1);
    RC630_ClearAllIrq0Flag();
    RC630_WriteReg(RC630_REG_COMMAND, RC630_CMD_TRANSMIT);
    tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
    while(tick--)
    {
        reg = RC630_ReadReg(RC630_REG_IRQ0);
        if(reg & RC630_BIT_TXIRQ)
        {
            break;
        }
    }

    if(tick == 0)
    {
        state = RC630_STAT_TIMEOUT;
    }
    else
    {
        RC630_Config(FALSE, FALSE, FALSE, 0, 0);
        for(pos = 1; pos < g_sRc630Frame.txLen - 1; pos++)
        {
            //终止当前所有操作
            RC630_WriteReg(RC630_REG_COMMAND, RC630_CMD_IDLE);
            //清空FIFO
            RC630_FlushFifo();
            RC630_WriteFIFO(g_sRc630Frame.frame + pos, 1);
            RC630_ClearAllIrq0Flag();
            RC630_WriteReg(RC630_REG_COMMAND, RC630_CMD_TRANSMIT);
            tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
            while(tick--)
            {
                reg = RC630_ReadReg(RC630_REG_IRQ0);
                if(reg & RC630_BIT_TXIRQ)
                {
                    break;
                }
            }
            if(tick == 0)
            {
                state = RC630_STAT_TIMEOUT;
            }
        }
    }

    if(state == RC630_STAT_OK)
    {
        g_sRc630Frame.frame[0] = g_sRc630Frame.frame[pos];
        g_sRc630Frame.txLen = 1;
        g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;
        state = RC630_Command();
    }
    //还原
    reg = RC630_ReadReg(RC630_REG_FRAMECON);
    RC630_WriteReg(RC630_REG_FRAMECON, reg | RC630_BIT_TXPARITYEN);

    return state;
}

u8 RC630_Command(void)
{
    u8 state = RC630_STAT_OK;
    u32 tick = 0;
    u8 reg = 0;
    u8 fifoLevel = 0;

    g_sRc630Frame.rxLen = 0;
    

    //终止当前所有操作
    RC630_WriteReg(RC630_REG_COMMAND, RC630_CMD_IDLE);


    //将发送数据写入fifo
    RC630_WriteFIFO(g_sRc630Frame.frame, g_sRc630Frame.txLen);

    RC630_WriteReg(RC630_REG_IRQ0EN, RC630_BIT_IDLEIRQ | RC630_BIT_TXIRQ | RC630_BIT_RXIRQ | RC630_BIT_ERRIRQ | RC630_BIT_SOFIRQ);
    //这里必须把sofDetected和subc_Detected寄存器清空，否则影响SOFIRQ标志
    RC630_WriteReg(RC630_REG_RXSOFD, RC630_BIT_SOF_EN);
    //清除所有中断标志位
    RC630_ClearAllIrq0Flag();
    //命令写入CMDReg后就自动激活
    RC630_WriteReg(RC630_REG_COMMAND, g_sRc630Frame.cmd & RC630_MASK_COMMAND);
    
    tick = RC630_TX_FRAME_TIMEOUT;
    while(tick) //发送
    {
        RC630_Delay16us(1);
        //这里耗时基本是10us
        reg = RC630_ReadReg(RC630_REG_IRQ0);
        if(reg & RC630_BIT_ERRIRQ)
        {
            state = RC630_STAT_ERROR;
            g_sRc630Frame.rxLen = 0;
            break;
        }
        else if(reg & RC630_BIT_TXIRQ)
        {
            break;
        }
        tick--;
    }
    //如果需要接收，就判断SOF标志
    if(state == RC630_STAT_OK && tick > 0 && (g_sRc630Frame.cmd != RC630_CMD_TRANSMIT))
    {
        tick = g_sRc630Frame.timeout >> 4;  //注意这里要除16，协议都是用us作为单位
        while(tick)
        {
            RC630_Delay16us(1);
            //这里耗时基本是10us
            reg = RC630_ReadReg(RC630_REG_IRQ0);
            if(reg & RC630_BIT_ERRIRQ)
            {
                state = RC630_STAT_ERROR;
                g_sRc630Frame.rxLen = 0;
                break;
            }
            else if(reg & RC630_BIT_SOFIRQ)
            {
                g_sRc630Frame.rxLen = 0;
                break;
            }
            else if(reg & RC630_BIT_RXIRQ)          //接收完成
            {
                break;
            }
            tick--;
        }
    }
    //接收数据
    if(state == RC630_STAT_OK && tick > 0 && (g_sRc630Frame.cmd != RC630_CMD_TRANSMIT))
    {
        tick = RC630_RX_FRAME_TIMEOUT;
        while(tick)
        {
            RC630_Delay16us(1);
            //这里耗时基本是10us
            reg = RC630_ReadReg(RC630_REG_IRQ0);
            if(reg & RC630_BIT_ERRIRQ)
            {
                state = RC630_STAT_ERROR;
                g_sRc630Frame.rxLen = 0;
                break;
            }
            else if(reg & RC630_BIT_RXIRQ)
            {
                fifoLevel = RC630_ReadReg(RC630_REG_FIFOLENGTH);
                RC630_Delay16us(100);
                RC630_ReadFIFO(g_sRc630Frame.frame + g_sRc630Frame.rxLen, fifoLevel);
                g_sRc630Frame.rxLen += fifoLevel;
                break;
            }
            else if(reg & RC630_BIT_IDLEIRQ)
            {
                fifoLevel = RC630_ReadReg(RC630_REG_FIFOLENGTH);
                RC630_ReadFIFO(g_sRc630Frame.frame + g_sRc630Frame.rxLen, fifoLevel);
                g_sRc630Frame.rxLen += fifoLevel;
                break;
            }
            tick--;
        }
    }

    //终止当前所有操作
    RC630_WriteReg(RC630_REG_COMMAND, RC630_CMD_IDLE);

    if(tick == 0)
    {
        state = RC630_STAT_TIMEOUT;
    }

    return state;
}
