#include "AnyID_FM17660.h"

FM17660_FRAME g_sFm17660Frame = {0};

const u8 g_sFM17660RegAdd[FM17660_PROTOCOL_CONFIG_SIZE] =
{
    FM17660_REG_TXAMP,
    FM17660_REG_TXCON,
    FM17660_REG_TXI,
    FM17660_REG_FRAMECON,
    FM17660_REG_RXANA,
    FM17660_REG_THNADJ,
    FM17660_REG_THNSET,
    FM17660_REG_THNMIN
};

void FM17660_WriteReg(u8 addr, u8 val)
{
    FM17660_CS_Low();

    FM17660_WriteByte(FM17660_WADDR(addr));
    FM17660_WriteByte(val);

    FM17660_CS_High();
}

u8 FM17660_ReadReg(u8 addr)
{
    u8 val = 0;

    FM17660_CS_Low();

    FM17660_WriteByte(FM17660_RADDR(addr));
    val = FM17660_WriteByte(0xFF);

    FM17660_CS_High();

    return val;
}

void FM17660_SetRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = FM17660_ReadReg(addr);
    val |= bit;
    FM17660_WriteReg(addr, val);
}

void FM17660_ClearRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = FM17660_ReadReg(addr);
    val &= (~bit);
    FM17660_WriteReg(addr, val);
}

void FM17660_SetRegBits(u8 addr, u8 msk, u8 bit)
{
    u8 val = 0;

    val = FM17660_ReadReg(addr);
    val &= (~msk);
    val |= bit;
    FM17660_WriteReg(addr, val);
}

void FM17660_ReadFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    FM17660_CS_Low();

    FM17660_WriteByte(FM17660_RADDR(FM17660_REG_FIFODATA));

    for(i = 0; i < (len - 1); i++)
    {
        pBuffer[i] = FM17660_WriteByte(FM17660_RADDR(FM17660_REG_FIFODATA));
    }

    pBuffer[i] = FM17660_WriteByte(0xFF);

    FM17660_CS_High();
}

void FM17660_WriteFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    FM17660_CS_Low();

    FM17660_WriteByte(FM17660_WADDR(FM17660_REG_FIFODATA));

    for(i = 0; i < len; i++)
    {
        FM17660_WriteByte(pBuffer[i]);
    }

    FM17660_CS_High();
}

void FM17660_ClearFIFO(void)
{
    u8 fifoLen = 0;
    
    fifoLen = FM17660_ReadReg(FM17660_REG_FIFOLENGTH);
    if(fifoLen != 0)
    {
        FM17660_SetRegBits(FM17660_REG_FIFOCONTROL, FM17660_BIT_FLUSHFIFO, FM17660_BIT_FLUSHFIFO);
    }
}

BOOL FM17660_Reset(void)
{
    u8 regVal = FM17660_STAT_ERROR;
    BOOL bOk = FALSE;
    u8 t = 0;

    //系统硬件复位
    FM17660_CS_High();
    
    FM17660_PowerHigh();                                          //低电平脉冲宽度大于100ns即可
    FM17660_Delay16us(60);
    FM17660_PowerLow();

    //等待系统稳定
    do{
        FM17660_Delay16us(120);
        t++;
        regVal = FM17660_ReadReg(FM17660_REG_COMMAND);
        if(regVal == FM17660_BIT_MODEMOFF)
        {
            bOk = TRUE;
            break;
        }
    }while(t < 100);

    FM17660_Delay16us(120);
    return bOk;
}

u8  FM17660_Init(void)
{
    u8 state = FM17660_STAT_ERROR;
    if(FM17660_Reset())
    {
        state = FM17660_STAT_OK;
    }
        
    return state;
}

void FM17660_Config(u8 bClearCrypt, u8 bEnTxCrc, u8 bEnRxCrc, u8 txLastBits, u8 rxAlignBits)
{
    u8 reg = 0;

    if(bClearCrypt)
    {
        FM17660_ClearRegBit(FM17660_REG_STATUS, FM17660_BIT_CRYPTO1ON);
    }

    if(bEnTxCrc)
    {
        FM17660_SetRegBit(FM17660_REG_TXCRCCON, FM17660_BIT_CRCEN);
    }
    else
    {
        FM17660_ClearRegBit(FM17660_REG_TXCRCCON, FM17660_BIT_CRCEN);
    }

    if(bEnRxCrc)
    {
        FM17660_SetRegBit(FM17660_REG_RXCRCCON, FM17660_BIT_CRCEN);
    }
    else
    {
        FM17660_ClearRegBit(FM17660_REG_RXCRCCON, FM17660_BIT_CRCEN);
    }

    reg = FM17660_ReadReg(FM17660_REG_TXDATANUM);
    reg &= (~FM17660_MASK_TXLASTBITS);
    reg |= (FM17660_MASK_TXLASTBITS & txLastBits);
    FM17660_WriteReg(FM17660_REG_TXDATANUM, reg);

    reg = FM17660_ReadReg(FM17660_REG_RXBITCTRL);
    reg &= (~FM17660_MASK_RXALIGN);
    reg |= ((rxAlignBits << 4) & FM17660_MASK_RXALIGN);
    FM17660_WriteReg(FM17660_REG_RXBITCTRL, reg);
}

u8 FM17660_SelectProtocol(u8 tx, u8 rx)
{
    u8 regVal = 0;
    g_sFm17660Frame.frame[0] = rx;
    g_sFm17660Frame.frame[1] = tx;
    g_sFm17660Frame.txLen = 2;

    g_sFm17660Frame.timeout = 10000;
    g_sFm17660Frame.cmd = FM17660_CMD_LOADPROTOCOL;
    
    //终止当前所有操作
    FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_IDLE);
    
    //清空FIFO
    FM17660_ClearFIFO();
    
    //将发送数据写入fifo
    FM17660_WriteFIFO(g_sFm17660Frame.frame, g_sFm17660Frame.txLen);
    
    //命令写入CMDReg后就自动激活
    FM17660_WriteReg(FM17660_REG_COMMAND, g_sFm17660Frame.cmd & FM17660_MASK_COMMAND);
    
    do
    {
        regVal = FM17660_ReadReg(FM17660_REG_COMMAND);
    }while(regVal != FM17660_CMD_IDLE);

    return FM17660_STAT_OK;//FM17660_Command();//
}

u8 FM17660_TopazCommand(void)
{
    u8 pos = 0;
    u8 state = FM17660_STAT_OK;
    u8 reg = 0;
    u32 tick = 0;

    reg = FM17660_ReadReg(FM17660_REG_FRAMECON);
    FM17660_WriteReg(FM17660_REG_FRAMECON, reg & (~FM17660_BIT_TXPARITYEN));

    //终止当前所有操作
    FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_IDLE);
    //清空FIFO
    FM17660_FlushFifo();


    FM17660_Config(FALSE, FALSE, FALSE, 7, 0);
    FM17660_WriteFIFO(g_sFm17660Frame.frame, 1);
    FM17660_ClearAllIrq0Flag();
    FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_TRANSMIT);
    tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
    while(tick--)
    {
        reg = FM17660_ReadReg(FM17660_REG_IRQ0);
        if(reg & FM17660_BIT_TXIRQ)
        {
            break;
        }
    }

    if(tick == 0)
    {
        state = FM17660_STAT_TIMEOUT;
    }
    else
    {
        FM17660_Config(FALSE, FALSE, FALSE, 0, 0);
        for(pos = 1; pos < g_sFm17660Frame.txLen - 1; pos++)
        {
            //终止当前所有操作
            FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_IDLE);
            //清空FIFO
            FM17660_FlushFifo();
            FM17660_WriteFIFO(g_sFm17660Frame.frame + pos, 1);
            FM17660_ClearAllIrq0Flag();
            FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_TRANSMIT);
            tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
            while(tick--)
            {
                reg = FM17660_ReadReg(FM17660_REG_IRQ0);
                if(reg & FM17660_BIT_TXIRQ)
                {
                    break;
                }
            }
            if(tick == 0)
            {
                state = FM17660_STAT_TIMEOUT;
            }
        }
    }

    if(state == FM17660_STAT_OK)
    {
        g_sFm17660Frame.frame[0] = g_sFm17660Frame.frame[pos];
        g_sFm17660Frame.txLen = 1;
        g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;
        state = FM17660_Command();
    }
    //还原
    reg = FM17660_ReadReg(FM17660_REG_FRAMECON);
    FM17660_WriteReg(FM17660_REG_FRAMECON, reg | FM17660_BIT_TXPARITYEN);

    return state;
}

u8 FM17660_Command(void)
{
    u8 state = FM17660_STAT_OK;
    u32 tick = 0;
    u8 reg = 0;
    u8 fifoLevel = 0;

    g_sFm17660Frame.rxLen = 0;

    //终止当前所有操作
    FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_IDLE & FM17660_MASK_COMMAND);

    //清空FIFO
    FM17660_ClearFIFO();

    FM17660_WriteReg(FM17660_REG_IRQ0EN, FM17660_BIT_IDLEIRQ | FM17660_BIT_TXIRQ | FM17660_BIT_RXIRQ | FM17660_BIT_ERRIRQ | FM17660_BIT_SOFIRQ);

    //这里必须把sofDetected和subc_Detected寄存器清空，否则影响SOFIRQ标志
    FM17660_WriteReg(FM17660_REG_RXSOFD, FM17660_BIT_SOF_EN);
    
    //清除所有中断标志位
    FM17660_ClearAllIrq0Flag();
    
    //将发送数据写入fifo
    FM17660_WriteFIFO(g_sFm17660Frame.frame, g_sFm17660Frame.txLen);
    
    //命令写入CMDReg后就自动激活
    FM17660_WriteReg(FM17660_REG_COMMAND, g_sFm17660Frame.cmd & FM17660_MASK_COMMAND);
    
    tick = FM17660_TX_FRAME_TIMEOUT;
    while(tick) //发送
    {
        FM17660_Delay16us(1);
        //这里耗时基本是10us
        reg = FM17660_ReadReg(FM17660_REG_IRQ0);
        if(reg & FM17660_BIT_ERRIRQ)
        {
            reg = FM17660_ReadReg(FM17660_REG_ERROR);  //0x08 NoDataErr
            state = FM17660_STAT_ERROR;
            if(reg & FM17660_STAT_COLLISION_ERROR) //发生碰撞错误
            {
                state = FM17660_STAT_COLLISION_ERROR;
            }
            g_sFm17660Frame.rxLen = 0;
            
            break;
        }
        else if(reg & FM17660_BIT_TXIRQ)
        {
            //清空FIFO
            FM17660_ClearFIFO();
            break;
        }
        tick--;
    }
    
    //如果需要接收，就判断SOF标志
    if(state == FM17660_STAT_OK && tick > 0 && (g_sFm17660Frame.cmd != FM17660_CMD_TRANSMIT))
    {
        tick = g_sFm17660Frame.timeout >> 4;  //注意这里要除16，协议都是用us作为单位
        while(tick)
        {
            FM17660_Delay16us(1);
            //这里耗时基本是10us
            reg = FM17660_ReadReg(FM17660_REG_IRQ0);
            if(reg & FM17660_BIT_ERRIRQ)              //发生错误
            {
                reg = FM17660_ReadReg(FM17660_REG_ERROR);  //0x08 NoDataErr
                state = FM17660_STAT_ERROR;
                if(reg & FM17660_STAT_COLLISION_ERROR) //发生碰撞错误
                {
                    state = FM17660_STAT_COLLISION_ERROR;
                }
                g_sFm17660Frame.rxLen = 0;
                break;
            }
            else if(reg & FM17660_BIT_SOFIRQ)         //接收到SOF
            {
                g_sFm17660Frame.rxLen = 0;
                break;
            }
            else if(reg & FM17660_BIT_RXIRQ)          //接收完成
            {
                break;
            }
            tick--;
        }
    }
    
    //接收数据
    if(state == FM17660_STAT_OK && tick > 0 && (g_sFm17660Frame.cmd != FM17660_CMD_TRANSMIT))
    {
        tick = FM17660_RX_FRAME_TIMEOUT;
        while(tick)
        {
            FM17660_Delay16us(1);
            //这里耗时基本是10us
            reg = FM17660_ReadReg(FM17660_REG_IRQ0);
            if(reg & FM17660_BIT_ERRIRQ)
            {
                reg = FM17660_ReadReg(FM17660_REG_ERROR);  //0x08 NoDataErr
                state = FM17660_STAT_ERROR;
                if(reg & FM17660_STAT_COLLISION_ERROR) //发生碰撞错误
                {
                    state = FM17660_STAT_COLLISION_ERROR;
                }
                g_sFm17660Frame.rxLen = 0;
                break;
            }
            else if(reg & FM17660_BIT_RXIRQ)
            {
                fifoLevel = FM17660_ReadReg(FM17660_REG_FIFOLENGTH);
                FM17660_Delay16us(50);
                FM17660_ReadFIFO(g_sFm17660Frame.frame + g_sFm17660Frame.rxLen, fifoLevel);
                g_sFm17660Frame.rxLen += fifoLevel;
                break;
            }
            else if(reg & FM17660_BIT_IDLEIRQ)
            {
                fifoLevel = FM17660_ReadReg(FM17660_REG_FIFOLENGTH);
                FM17660_ReadFIFO(g_sFm17660Frame.frame + g_sFm17660Frame.rxLen, fifoLevel);
                g_sFm17660Frame.rxLen += fifoLevel;
                break;
            }
            tick--;
        }
    }

    //终止当前所有操作
    FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_IDLE & FM17660_MASK_COMMAND);

    if(tick == 0)
    {
        state = FM17660_STAT_TIMEOUT;
    }
    else if(state == FM17660_STAT_OK)
    {
        if(g_sFm17660Frame.rxLen == 0)
        {
            state = FM17660_STAT_NAK;
        }
    }

    return state;
}

#define FM17660_PRODUCTID_LEN         3
u8 FM17660_ReadProductID(void)
{
    //256字节、清空FIFO、waterLevel8bit，fifoLength8bit
    FM17660_WriteReg(FM17660_REG_FIFOCONTROL, FM17660_BIT_FIFOSIZE | FM17660_BIT_FLUSHFIFO);
    
    g_sFm17660Frame.frame[0] = 0;
    g_sFm17660Frame.frame[1] = 0;
    g_sFm17660Frame.frame[2] = FM17660_PRODUCTID_LEN;
    FM17660_WriteFIFO(g_sFm17660Frame.frame, FM17660_PRODUCTID_LEN);

    FM17660_WriteReg(FM17660_REG_COMMAND, FM17660_CMD_READE2);
    FM17660_Delay16us(100);
    g_sFm17660Frame.rxLen = FM17660_ReadReg(FM17660_REG_FIFOLENGTH);
    if(g_sFm17660Frame.rxLen == FM17660_PRODUCTID_LEN)
    {
        FM17660_ReadFIFO(g_sFm17660Frame.frame, FM17660_PRODUCTID_LEN);
        return g_sFm17660Frame.frame[1];
    }
    else
    {
        return 0xFF;
    }
}