#include "AnyID_Fm17xx.h"
#include "AnyID_Des.h"

FM17XX_FRAME g_sFm17xxFrame = {0};

/******************************************************************************
** 函数名：FM17xx_WriteReg
** 说  明: 写入FM17xx寄存器
**
** 参  数：addr：地址
           val：值
** 返  回：void
******************************************************************************/
void FM17xx_WriteReg(u8 addr, u8 val)
{
    FM17xx_CS_Low();

    FM17xx_WriteByte(FM17XX_WADDR(addr));
    FM17xx_WriteByte(val);

    FM17xx_CS_High();
}

/******************************************************************************
** 函数名：FM17xx_ReadReg
** 说  明: 读FM17xx寄存器
**
** 参  数：addr：地址
** 返  回：寄存器值
******************************************************************************/
u8 FM17xx_ReadReg(u8 addr)
{
    u8 val = 0;

    FM17xx_CS_Low();

    FM17xx_WriteByte(FM17XX_RADDR(addr));
    val = FM17xx_WriteByte(0x00);

    FM17xx_CS_High();

    return val;
}

/******************************************************************************
** 函数名：FM17xx_SetRegBit
** 说  明: 设置寄存器的某一位
**
** 参  数：addr：地址
           bit：需要置1的位
** 返  回：void
******************************************************************************/
void FM17xx_SetRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = FM17xx_ReadReg(addr);
    val |= bit;
    FM17xx_WriteReg(addr, val);
}

/******************************************************************************
** 函数名：FM17xx_ClearRegBit
** 说  明: 清零寄存器的某一位
**
** 参  数：addr：地址
           bit：需清零要置1的位
** 返  回：void
******************************************************************************/
void FM17xx_ClearRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = FM17xx_ReadReg(addr);
    val &= (~bit);
    FM17xx_WriteReg(addr, val);
}

/******************************************************************************
** 函数名：FM17xx_ReadFIFO
** 说  明: 读FM17xx的FIFO
**
** 参  数：pBuffer：读取的数据缓存区
           len：需要读取的长度
** 返  回：void
******************************************************************************/
void FM17xx_ReadFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    FM17xx_CS_Low();

    FM17xx_WriteByte(FM17XX_RADDR(FM17XX_REG_FIFODATA));

    for(i = 0; i < (len - 1); i++)
    {
        pBuffer[i] = FM17xx_WriteByte(FM17XX_RADDR(FM17XX_REG_FIFODATA));
    }

    pBuffer[i] = FM17xx_WriteByte(0x00);

    FM17xx_CS_High();
}

/******************************************************************************
** 函数名：FM17xx_ReadFIFO
** 说  明: 写FM17xx的FIFO
**
** 参  数：pBuffer：写数据读取的缓存区
           len：需要写的长度
** 返  回：void
******************************************************************************/
void FM17xx_WriteFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    FM17xx_CS_Low();

    FM17xx_WriteByte(FM17XX_WADDR(FM17XX_REG_FIFODATA));

    for(i = 0; i < len; i++)
    {
        FM17xx_WriteByte(pBuffer[i]);
    }

    FM17xx_CS_High();
}

/******************************************************************************
** 函数名：FM17xx_Reset
** 说  明: 初始化FM17xx
**
** 参  数：void
** 返  回：void
******************************************************************************/
BOOL FM17xx_Reset(void)
{
    u8 regVal = 0;
    BOOL b = FALSE;

    //系统硬件复位
    FM17xx_PowerLow();                                          //低电平脉冲宽度大于1ms
    FM17xx_Delay16us(2560);
    FM17xx_PowerHigh();

    //晶振稳定时间800us，建议2ms
    FM17xx_Delay16us(5120);

    //这个必须设置
    FM17xx_WriteReg(FM17XX_REG_TXAUTO, FM17XX_BIT_100ASK);     //force to 100% ASK
    regVal = FM17xx_ReadReg(FM17XX_REG_TXAUTO);
    if(regVal == FM17XX_BIT_100ASK)
    {
        b = TRUE;
    }

    return b;
}

u8 FM17xx_Command(void)
{
    u8 state = 0;
    u32 tick = 0;
    u8 reg = 0;
    u8 txPos = 0;
    u8 fifoLevel = 0;

    g_sFm17xxFrame.rxLen = 0;
    tick = g_sFm17xxFrame.timeout >> 4; //16us = tick

    //终止当前所有操作
    FM17xx_WriteReg(FM17XX_REG_COMMAND, FM17XX_CMD_IDLE);
    //清空FIFO
    FM17xx_WriteReg(FM17XX_REG_FIFOLEVEL, FM17XX_BIT_FLUSHBUFFER);
    //清除所有中断标志位
    FM17xx_WriteReg(FM17XX_REG_COMMIRQ, 0x7F);

    //希尔数据到FIFO，如果数据超过FIFO的长度，就等发送后FIFO空出位置再写
    if(g_sFm17xxFrame.txLen > FM17XX_SIZE_FIFO)
    {
        //将发送数据写入fifo
        FM17xx_WriteFIFO(g_sFm17xxFrame.frame, FM17XX_SIZE_FIFO);
        txPos += FM17XX_SIZE_FIFO;
        g_sFm17xxFrame.txLen -= FM17XX_SIZE_FIFO;
    }
    else
    {
        //将发送数据写入fifo
        FM17xx_WriteFIFO(g_sFm17xxFrame.frame, g_sFm17xxFrame.txLen);
        txPos += g_sFm17xxFrame.txLen;
        g_sFm17xxFrame.txLen -= g_sFm17xxFrame.txLen;
    }

    //清除所有中断标志位
    FM17xx_WriteReg(FM17XX_REG_COMMIRQ, 0x7F);

    //激活命令，FM17XX_CMD_TRANSCEIVE使用FM17XX_CMD_STARTSEND激活
    //          其他命令写入CMDReg后就自动激活
    FM17xx_WriteReg(FM17XX_REG_COMMAND, g_sFm17xxFrame.cmd & FM17XX_MASK_COMMAND);

    if(g_sFm17xxFrame.cmd == FM17XX_CMD_TRANSCEIVE)
    {
        //触发发送
        FM17xx_SetRegBit(FM17XX_REG_BITFRAMING, FM17XX_BIT_STARTSEND);
    }

    //发送其他数据帧
    while(g_sFm17xxFrame.txLen > 0)
    {
        reg = FM17xx_ReadReg(FM17XX_REG_COMMIRQ);
        //如果FIFO低中断标志出现，表示FIFO有空位，可以写入数据
        if(reg & FM17XX_BIT_LOALERTI)
        {
            if(g_sFm17xxFrame.txLen > (FM17XX_SIZE_FIFO - FM17XX_FIFO_LEVEL))
            {
                //将发送数据写入fifo
                FM17xx_WriteFIFO(g_sFm17xxFrame.frame + txPos, FM17XX_SIZE_FIFO - FM17XX_FIFO_LEVEL);
                txPos += (FM17XX_SIZE_FIFO - FM17XX_FIFO_LEVEL);
                g_sFm17xxFrame.txLen -= (FM17XX_SIZE_FIFO - FM17XX_FIFO_LEVEL);
            }
            else
            {
                //将发送数据写入fifo
                FM17xx_WriteFIFO(g_sFm17xxFrame.frame + txPos, g_sFm17xxFrame.txLen);
                txPos += g_sFm17xxFrame.txLen;
                g_sFm17xxFrame.txLen -= g_sFm17xxFrame.txLen;
            }
            FM17xx_WriteReg(FM17XX_REG_COMMIRQ, FM17XX_BIT_LOALERTI); //清空lo标志
        }
        else if(reg & FM17XX_BIT_ERRI)
        {
            tick = 0;
            break;
        }
    }

    //清除所有中断标志位
    FM17xx_WriteReg(FM17XX_REG_COMMIRQ, 0x7F);

    while(tick)
    {
        //这里耗时基本是10us
        FM17xx_Delay16us(1);
        reg = FM17xx_ReadReg(FM17XX_REG_COMMIRQ);
        if(reg & FM17XX_BIT_ERRI)
        {
            state = FM17XX_STAT_ERROR;
            g_sFm17xxFrame.rxLen = 0;
            break;
        }
        else if(reg & FM17XX_BIT_RXI)
        {
            fifoLevel = FM17xx_ReadReg(FM17XX_REG_FIFOLEVEL);
            FM17xx_ReadFIFO(g_sFm17xxFrame.frame + g_sFm17xxFrame.rxLen, fifoLevel);
            g_sFm17xxFrame.rxLen += fifoLevel;
            break;
        }
        else if(reg & FM17XX_BIT_TXI)
        {
            if(reg & FM17XX_BIT_HIALERTI)
            {
                fifoLevel = FM17xx_ReadReg(FM17XX_REG_FIFOLEVEL);
                FM17xx_ReadFIFO(g_sFm17xxFrame.frame + g_sFm17xxFrame.rxLen, fifoLevel);
                FM17xx_WriteReg(FM17XX_REG_COMMIRQ, FM17XX_BIT_HIALERTI);
                g_sFm17xxFrame.rxLen += fifoLevel;
            }
            else if(g_sFm17xxFrame.cmd == FM17XX_CMD_TRANSMIT)
            {
                break;
            }
        }
        else if(reg & FM17XX_BIT_IDLEI)
        {
            fifoLevel = FM17xx_ReadReg(FM17XX_REG_FIFOLEVEL);
            FM17xx_ReadFIFO(g_sFm17xxFrame.frame + g_sFm17xxFrame.rxLen, fifoLevel);
            g_sFm17xxFrame.rxLen += fifoLevel;
            break;
        }
        tick--;
    }

    //终止当前所有操作
    FM17xx_WriteReg(FM17XX_REG_COMMAND, FM17XX_CMD_IDLE);

    if(g_sFm17xxFrame.cmd == FM17XX_CMD_TRANSCEIVE)
    {
        //停止发送
        FM17xx_ClearRegBit(FM17XX_REG_BITFRAMING, FM17XX_BIT_STARTSEND);
    }

    if(tick == 0)
    {
        state = FM17XX_STAT_TIMEOUT;
    }
    return state;
}


void FM17xx_Config(BOOL bClearCrypt, BOOL bTxCrc, BOOL bRxCrc, u8 txLastBits, u8 rxAlignBits)
{
    if(bClearCrypt)
    {
        FM17xx_ClearRegBit(FM17XX_REG_STATUS2, FM17XX_BIT_CRYPTO1ON);   //明文发送
    }

    if(bTxCrc)
    {
        FM17xx_SetRegBit(FM17XX_REG_TXMODE, FM17XX_BIT_CRCEN);
    }
    else
    {
        FM17xx_ClearRegBit(FM17XX_REG_TXMODE, FM17XX_BIT_CRCEN);
    }

    if(bRxCrc)
    {
        FM17xx_SetRegBit(FM17XX_REG_RXMODE, FM17XX_BIT_CRCEN);
    }
    else
    {
        FM17xx_ClearRegBit(FM17XX_REG_RXMODE, FM17XX_BIT_CRCEN);
    }

    FM17xx_WriteReg(FM17XX_REG_BITFRAMING, txLastBits | (rxAlignBits << 4));
}




