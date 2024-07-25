#include "AnyID_RC610.h"
RC610_FRAME g_sRc610Frame = {0};

const u8 g_sRc610RegAdd[RC610_PROTOCOL_CONFIG_SIZE] =
{
    RC610_REG_DRVMODE,
    RC610_REG_TXAMP,
    RC610_REG_DRVCON,
    RC610_REG_TXI,
    RC610_REG_TXCRCCON,
    RC610_REG_RXCRCCON,
    RC610_REG_TXDATANUM,
    RC610_REG_TXMODWIDTH,
    RC610_REG_TXSYM10BURSTLEN,
    RC610_REG_TXWAITCTRL,
    RC610_REG_FRAMECON,
    RC610_REG_RXCTRL,
    RC610_REG_RXTHRESHOLD,
    RC610_REG_RCV,
    RC610_REG_RXANA
};


void RC610_WriteReg(u8 addr, u8 val)
{
    RC610_CS_Low();

    RC610_WriteByte(RC610_WADDR(addr));
    RC610_WriteByte(val);

    RC610_CS_High();
}

u8 RC610_ReadReg(u8 addr)
{
    u8 val = 0;

    RC610_CS_Low();

    RC610_WriteByte(RC610_RADDR(addr));
    val = RC610_WriteByte(0xFF);

    RC610_CS_High();

    return val;
}

void RC610_SetRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = RC610_ReadReg(addr);
    val |= bit;
    RC610_WriteReg(addr, val);
}

void RC610_ClearRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = RC610_ReadReg(addr);
    val &= (~bit);
    RC610_WriteReg(addr, val);
}

void RC610_ReadFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    RC610_CS_Low();

    RC610_WriteByte(RC610_RADDR(RC610_REG_FIFODATA));

    for(i = 0; i < (len - 1); i++)
    {
        pBuffer[i] = RC610_WriteByte(RC610_RADDR(RC610_REG_FIFODATA));
    }

    pBuffer[i] = RC610_WriteByte(0xFF);

    RC610_CS_High();
}

void RC610_WriteFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    RC610_CS_Low();

    RC610_WriteByte(RC610_WADDR(RC610_REG_FIFODATA));

    for(i = 0; i < len; i++)
    {
        RC610_WriteByte(pBuffer[i]);
    }

    RC610_CS_High();
}

BOOL RC610_Reset(void)
{
    u8 regVal = RC610_STAT_ERROR;
    BOOL bOk = FALSE;
    u8 t = 0;

    //系统硬件复位
    RC610_PowerHigh();                                          //低电平脉冲宽度大于100ns即可
    RC610_Delay16us(1000);
    RC610_PowerLow();

    //等待系统稳定
    do{
        RC610_Delay16us(10);
        t++;
        regVal = RC610_ReadReg(RC610_REG_VERSION);
        if((regVal != 0) && (regVal != 0xFF))
        {
            bOk = TRUE;
            break;
        }
    }while(t < 100);

    return bOk;
}

u8 RC610_Init(void)
{
    u8 state = RC610_STAT_ERROR;
    if(RC610_Reset())
    {
        state = RC610_STAT_OK;
    }

    RC610_WriteReg(RC610_REG_COMMAND, 0x00);
    //256字节、清空FIFO、waterLevel8bit，fifoLength8bit
    RC610_WriteReg(RC610_REG_FIFOCONTROL, RC610_BIT_FIFOSIZE | RC610_BIT_FLUSHFIFO);
    //WaterLevel = 64字节
    RC610_WriteReg(RC610_REG_WATERLEVEL, RC610_FIFO_LEVEL);
    
    
    return state;
}

void RC610_Config(u8 bClearCrypt, u8 bEnTxCrc, u8 bEnRxCrc, u8 txLastBits, u8 rxAlignBits)
{
    u8 reg = 0;

    if(bClearCrypt)
    {
        RC610_ClearRegBit(RC610_REG_STATUS, RC610_BIT_CRYPTO1ON);
    }

    if(bEnTxCrc)
    {
        RC610_SetRegBit(RC610_REG_TXCRCCON, RC610_BIT_CRCEN);
    }
    else
    {
        RC610_ClearRegBit(RC610_REG_TXCRCCON, RC610_BIT_CRCEN);
    }

    if(bEnRxCrc)
    {
        RC610_SetRegBit(RC610_REG_RXCRCCON, RC610_BIT_CRCEN);
    }
    else
    {
        RC610_ClearRegBit(RC610_REG_RXCRCCON, RC610_BIT_CRCEN);
    }

    reg = RC610_ReadReg(RC610_REG_TXDATANUM);
    reg &= (~RC610_MASK_TXLASTBITS);
    reg |= (RC610_MASK_TXLASTBITS & txLastBits);
    RC610_WriteReg(RC610_REG_TXDATANUM, reg);

    reg = RC610_ReadReg(RC610_REG_RXBITCTRL);
    reg &= (~RC610_MASK_RXALIGN);
    reg |= ((rxAlignBits << 4) & RC610_MASK_RXALIGN);
    RC610_WriteReg(RC610_REG_RXBITCTRL, reg);
}

u8 RC610_SelectProtocol(u8 tx, u8 rx)
{
    g_sRc610Frame.frame[0] = rx;
    g_sRc610Frame.frame[1] = tx;
    g_sRc610Frame.txLen = 2;

    g_sRc610Frame.timeout = 10000;
    g_sRc610Frame.cmd = RC610_CMD_LOADPROTOCOL;

    return RC610_Command();
}

u8 RC610_TopazCommand(void)
{
    u8 pos = 0;
    u8 state = RC610_STAT_OK;
    u8 reg = 0;
    u32 tick = 0;

    reg = RC610_ReadReg(RC610_REG_FRAMECON);
    RC610_WriteReg(RC610_REG_FRAMECON, reg & (~RC610_BIT_TXPARITYEN));

    //终止当前所有操作
    RC610_WriteReg(RC610_REG_COMMAND, RC610_CMD_IDLE);
    //清空FIFO
    RC610_FlushFifo();


    RC610_Config(FALSE, FALSE, FALSE, 7, 0);
    RC610_WriteFIFO(g_sRc610Frame.frame, 1);
    RC610_ClearAllIrq0Flag();
    RC610_WriteReg(RC610_REG_COMMAND, RC610_CMD_TRANSMIT);
    tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
    while(tick--)
    {
        reg = RC610_ReadReg(RC610_REG_IRQ0);
        if(reg & RC610_BIT_TXIRQ)
        {
            break;
        }
    }

    if(tick == 0)
    {
        state = RC610_STAT_TIMEOUT;
    }
    else
    {
        RC610_Config(FALSE, FALSE, FALSE, 0, 0);
        for(pos = 1; pos < g_sRc610Frame.txLen - 1; pos++)
        {
            //终止当前所有操作
            RC610_WriteReg(RC610_REG_COMMAND, RC610_CMD_IDLE);
            //清空FIFO
            RC610_FlushFifo();
            RC610_WriteFIFO(g_sRc610Frame.frame + pos, 1);
            RC610_ClearAllIrq0Flag();
            RC610_WriteReg(RC610_REG_COMMAND, RC610_CMD_TRANSMIT);
            tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
            while(tick--)
            {
                reg = RC610_ReadReg(RC610_REG_IRQ0);
                if(reg & RC610_BIT_TXIRQ)
                {
                    break;
                }
            }
            if(tick == 0)
            {
                state = RC610_STAT_TIMEOUT;
            }
        }
    }

    if(state == RC610_STAT_OK)
    {
        g_sRc610Frame.frame[0] = g_sRc610Frame.frame[pos];
        g_sRc610Frame.txLen = 1;
        g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;
        state = RC610_Command();
    }
    //还原
    reg = RC610_ReadReg(RC610_REG_FRAMECON);
    RC610_WriteReg(RC610_REG_FRAMECON, reg | RC610_BIT_TXPARITYEN);

    return state;
}

u8 RC610_Command(void)
{
    u8 state = RC610_STAT_OK;
    u32 tick = 0;
    u8 reg = 0;
    u8 fifoLevel = 0;

    g_sRc610Frame.rxLen = 0;
    

    //终止当前所有操作
    RC610_WriteReg(RC610_REG_COMMAND, RC610_CMD_IDLE);


    //将发送数据写入fifo
    RC610_WriteFIFO(g_sRc610Frame.frame, g_sRc610Frame.txLen);

    RC610_WriteReg(RC610_REG_IRQ0EN, RC610_BIT_IDLEIRQ | RC610_BIT_TXIRQ | RC610_BIT_RXIRQ | RC610_BIT_ERRIRQ | RC610_BIT_SOFIRQ);
   
    //这里必须把sofDetected和subc_Detected寄存器清空，否则影响SOFIRQ标志
    RC610_WriteReg(RC610_REG_RXSOFD, RC610_BIT_SOF_EN);
    
    //清除所有中断标志位
    RC610_ClearAllIrq0Flag();
    
    //命令写入CMDReg后就自动激活
    RC610_WriteReg(RC610_REG_COMMAND, g_sRc610Frame.cmd & RC610_MASK_COMMAND);
    
    tick = RC610_TX_FRAME_TIMEOUT;

    while(tick) //发送
    {
        RC610_Delay16us(1);
        //这里耗时基本是10us
        reg = RC610_ReadReg(RC610_REG_IRQ0); //0x28
        if(reg & RC610_BIT_ERRIRQ)
        {
            reg = RC610_ReadReg(RC610_REG_ERROR);  //0x08 NoDataErr
            state = RC610_STAT_ERROR;
            if(reg & RC610_STAT_COLLISION_ERROR) //发生碰撞错误
            {
                state = RC610_STAT_COLLISION_ERROR;
            }
            g_sRc610Frame.rxLen = 0;
            break;
        }
        else if(reg & RC610_BIT_TXIRQ) //等待发送中断
        {
            break;
        }
        tick--;
    }
    
    
    //如果需要接收，就判断SOF标志
    if(state == RC610_STAT_OK && tick > 0 && (g_sRc610Frame.cmd != RC610_CMD_TRANSMIT))
    {
        tick = g_sRc610Frame.timeout >> 4;  //注意这里要除16，协议都是用us作为单位
        while(tick)
        {
            RC610_Delay16us(1);
            //这里耗时基本是10us
            reg = RC610_ReadReg(RC610_REG_IRQ0);//0x3d    00111101   //0x28 00101000

            if(reg & RC610_BIT_ERRIRQ)
            {
                state = RC610_STAT_ERROR;
                reg = RC610_ReadReg(RC610_REG_ERROR);
                if(reg & RC610_STAT_COLLISION_ERROR) //发生碰撞错误
                {
                    state = RC610_STAT_COLLISION_ERROR;
                }
                g_sRc610Frame.rxLen = 0;
                break;
            }
            else if(reg & RC610_BIT_SOFIRQ) //等待开始中断 开始应答数据
            {
                g_sRc610Frame.rxLen = 0;
                break;
            }
            else if(reg & RC610_BIT_RXIRQ) //接收中断
            {
                reg = RC610_ReadReg(RC610_REG_RXCOLL); //
            }

            tick--;
        }
    }

    //接收数据
    if(state == RC610_STAT_OK && tick > 0 && (g_sRc610Frame.cmd != RC610_CMD_TRANSMIT))
    {
        tick = RC610_RX_FRAME_TIMEOUT;
        while(tick)
        {
            RC610_Delay16us(1);
            //这里耗时基本是10us
            reg = RC610_ReadReg(RC610_REG_IRQ0); // 0X3D  //0x3f
            if(reg & RC610_BIT_ERRIRQ)
            {
                state = RC610_STAT_ERROR;
                reg = RC610_ReadReg(RC610_REG_ERROR);
                if(reg & RC610_STAT_COLLISION_ERROR) //发生碰撞错误
                {
                    state = RC610_STAT_COLLISION_ERROR;
                }
                g_sRc610Frame.rxLen = 0;
                break;
            }
            else if(reg & RC610_BIT_RXIRQ)
            {
                fifoLevel = RC610_ReadReg(RC610_REG_FIFOLENGTH);
                RC610_Delay16us(50);
                RC610_ReadFIFO(g_sRc610Frame.frame + g_sRc610Frame.rxLen, fifoLevel);
                g_sRc610Frame.rxLen += fifoLevel;
                break;
            }
            else if(reg & RC610_BIT_IDLEIRQ)
            {
                fifoLevel = RC610_ReadReg(RC610_REG_FIFOLENGTH);
                RC610_ReadFIFO(g_sRc610Frame.frame + g_sRc610Frame.rxLen, fifoLevel);
                g_sRc610Frame.rxLen += fifoLevel;
                break;
            }
            
            tick--;
        }
    }

    //终止当前所有操作
    RC610_WriteReg(RC610_REG_COMMAND, RC610_CMD_IDLE);

    if(tick == 0)
    {
        state = RC610_STAT_TIMEOUT;
    }

    return state;
}
