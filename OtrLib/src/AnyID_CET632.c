#include "AnyID_CET632.h"

CET632_FRAME g_sCET632Frame = {0};

u8 CET632_ReadReg(u8 addr)
{
    u8 reg = 0;
    
    addr &= CET632_ADDR_MASK;
    addr <<= 1;
    addr |= 0x80;
    
    CET632_CsLow();
    CET632_WriteByte(addr);
    reg = CET632_ReadByte();
    CET632_CsHigh();
    
    return reg;
}
void CET632_WriteReg(u8 addr, u8 reg)
{
    addr &= CET632_ADDR_MASK;
    addr <<= 1;
    addr &= 0x7E;
    
    CET632_CsLow();
    CET632_WriteByte(addr);
    CET632_WriteByte(reg);
    CET632_CsHigh();
}

void CET632_ClearRegBit(u8 addr, u8 reg)
{
    u8 r = 0;
    r = CET632_ReadReg(addr);
    r &= (~reg);
    CET632_WriteReg(addr, r);
}
void CET632_SetRegBit(u8 addr, u8 reg)
{
    u8 r = 0;
    r = CET632_ReadReg(addr);
    r |= reg;
    CET632_WriteReg(addr, r);
}


void CET632_ReadFifo(u8 *pBuffer, u8 len)
{
    u8 i = 0;
    if(len > 0)
    {
        CET632_CsLow();
        CET632_WriteByte((CET632_REG_FIFO << 1) | 0x80);
        for(i = 0; i < len - 1; i++)
        {
            pBuffer[i] = CET632_WriteReadByte((CET632_REG_FIFO << 1) & 0x7E);
        }
        pBuffer[i] = CET632_ReadByte();
        CET632_CsHigh();
    }
}

void CET632_WriteFifo(u8 *pBuffer, u8 len)
{
    u8 i = 0;
    if(len > 0)
    {
        CET632_CsLow();
        CET632_WriteByte((CET632_REG_FIFO << 1) & 0x7E);
        for(i = 0; i < len; i++)
        {
            CET632_WriteByte(pBuffer[i]);
        }
        CET632_CsHigh();
    }
}

u8 CET632_CheckErr(u8 err)
{
    if(err & CET632_BIT_COLLERR)
    {
        return CET632_ERR_COLL;
    }
    else if(err & CET632_BIT_PARITYERR)
    {
        return CET632_ERR_PARITY;
    }
    else if(err & CET632_BIT_FRAMEERR)
    {
        return CET632_ERR_FRAME;
    }
    else if(err & CET632_BIT_CRCERR)
    {
        return CET632_ERR_CRC;
    }
    else if(err & CET632_BIT_FIFOOV)
    {
        return CET632_ERR_FIFO;
    }
    else if(err & CET632_BIT_ACCESSERR)
    {
        return CET632_ERR_ACCESS;
    }
    else if(err & CET632_BIT_KEYERR)
    {
        return CET632_ERR_KEY;
    }
    else if(err & CET632_BIT_E2ERR)
    {
        return CET632_ERR_EEPROM;
    }

    return CET632_ERR_SUCCESS;
}


u8 CET632_Transceive(u8 *pTxBuffer, u8 txLen, u8 txLastBits, u8 rxAlignBit, u8 *pRxBuffer, u8 *pRxLen, u32 usTimeOut)
{
    u8 reg = 0;
    u8 irq = 0;
    u8 state = CET632_ERR_SUCCESS;
    u16 tick = 0;
    
    //清空中断标志
    CET632_WriteReg(CET632_REG_INTRQ, 0x3F);
    //清空FIFO
    CET632_SetRegBit(CET632_REG_CTRL, CET632_BIT_FLUSHFIFO);

    irq = CET632_BIT_TXIEN | CET632_BIT_RXIEN | CET632_BIT_IDLEIEN;
    //将发送数据写入fifo
    CET632_WriteFifo(pTxBuffer, txLen);
    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, (rxAlignBit << 4) | txLastBits); 
    //超时时间，将单位ms转换成Tick = 100us
    tick = usTimeOut / 10;
    //启动发送
    CET632_WriteReg(CET632_REG_CMD, CET632_CMD_TRANCEIVE);
    while(tick)
    {
        CET632_Delay100us(1);
        reg = CET632_ReadReg(CET632_REG_INTRQ);
        if((reg & irq) == irq)
        {
            *pRxLen = CET632_ReadReg(CET632_REG_FIFOLEN);
            CET632_ReadFifo(pRxBuffer, *pRxLen);
            break; 
        }
        tick--;
    }
    CET632_WriteReg(CET632_REG_CMD, CET632_CMD_IDLE);
    if(tick == 0)
    {
        state = CET632_ERR_TIMEOUT;
    }

    return state;
}


u8 CET632_Command(u8 cmd, u8 *pTxBuffer, u8 txLen, u8 *pRxBuffer, u8 *pRxLen, u32 usTimeOut)
{
    u8 state = CET632_ERR_SUCCESS;
    u32 tick = 0;
    u8 reg = 0;
    u8 rxLen = 0;
    u8 txPos = 0;
    
    //清空中断标志
    CET632_WriteReg(CET632_REG_INTRQ, 0x3F);
    //清空FIFO
    CET632_SetRegBit(CET632_REG_CTRL, CET632_BIT_FLUSHFIFO);

    //超时时间，将单位us转换成Tick = 100us
    tick = usTimeOut / 100;
    if(tick == 0)
    {
        tick = 1;
    }

    //希尔数据到FIFO，如果数据超过FIFO的长度，就等发送后FIFO空出位置再写
    if(txLen > CET632_SIZE_FIFO)
    {
        //将发送数据写入fifo
        CET632_WriteFifo(pTxBuffer, CET632_SIZE_FIFO);
        txPos += CET632_SIZE_FIFO;
        txLen -= CET632_SIZE_FIFO;
    }
    else
    {
        //将发送数据写入fifo
        CET632_WriteFifo(pTxBuffer, txLen);
        txPos += txLen;
        txLen -= txLen;
    }
    //清空FIFO低中断标志
    CET632_ClearRegBit(CET632_REG_INTRQ, CET632_BIT_SETIRQ);
    reg = CET632_ReadReg(CET632_REG_INTRQ);
    //启动发送
    CET632_WriteReg(CET632_REG_CMD, cmd);
    //发送其他数据帧
    while(txLen > 0)
    {
        reg = CET632_ReadReg(CET632_REG_INTRQ);
        //如果FIFO低中断标志出现，表示FIFO有空位，可以写入数据
        if((reg & CET632_BIT_FIFOLOIRQ) == CET632_BIT_FIFOLOIRQ)
        {
            if(txLen > (CET632_SIZE_FIFO - CET632_FIFO_LEVEL))
            {
                //将发送数据写入fifo
                CET632_WriteFifo(pTxBuffer + txPos, CET632_SIZE_FIFO - CET632_FIFO_LEVEL);
                txPos += (CET632_SIZE_FIFO - CET632_FIFO_LEVEL);
                txLen -= (CET632_SIZE_FIFO - CET632_FIFO_LEVEL);
            }
            else
            {
                //将发送数据写入fifo
                CET632_WriteFifo(pTxBuffer + txPos, txLen);
                txPos += txLen;
                txLen -= txLen;
            }
            //清空FIFO高标志
            CET632_ClearRegBit(CET632_REG_INTRQ, CET632_BIT_SETIRQ);
        }
    }
    
    
    while(tick)
    {
        CET632_Delay100us(1);
        reg = CET632_ReadReg(CET632_REG_INTRQ);
        if((reg & CET632_BIT_IDLEIRQ) == CET632_BIT_IDLEIRQ)
        {
            if(pRxBuffer)
            {
                rxLen = CET632_ReadReg(CET632_REG_FIFOLEN);
                CET632_ReadFifo(pRxBuffer + *pRxLen, rxLen);
                *pRxLen += rxLen;
            }
            break; 
        }
        else if((reg & CET632_BIT_TXIRQ) == CET632_BIT_TXIRQ)
        {
            //如果FIFO接近满，马上读取FIFO，空出空间
            if((reg & CET632_BIT_HIALERT) == CET632_BIT_FIFOHIIRQ)
            {
                if(pRxBuffer)
                {
                    rxLen = CET632_ReadReg(CET632_REG_FIFOLEN);
                    CET632_ReadFifo(pRxBuffer + *pRxLen, rxLen);
                    *pRxLen += rxLen;
                }
            }
        }
        tick--;
    }
    CET632_WriteReg(CET632_REG_CMD, CET632_CMD_IDLE);
    if(tick == 0)
    {
        state = CET632_ERR_TIMEOUT;
    }

    return state;
}



u8 CET632_LoadKey(u8 *pKey, u8 keyLen)
{
    u8 reg = 0;
    u8 irq = 0;
    u8 state = CET632_ERR_SUCCESS;
    u16 tick = 0;
    
    //清空中断标志
    CET632_WriteReg(CET632_REG_INTRQ, 0x3F);
    //清空FIFO
    CET632_SetRegBit(CET632_REG_CTRL, CET632_BIT_FLUSHFIFO);

    irq = CET632_BIT_TXIEN | CET632_BIT_IDLEIEN;
    //将发送数据写入fifo
    CET632_WriteFifo(pKey, keyLen);
    //超时时间，将单位ms转换成Tick = 100us
    tick = CET632_TIME_WEEPROM / 10;
    //启动写EEPROM
    CET632_WriteReg(CET632_REG_CMD, CET632_CMD_WRITEE2);
    while(tick)
    {
        CET632_Delay100us(1);
        reg = CET632_ReadReg(CET632_REG_INTRQ);
        if((reg & irq) == irq)
        {
            u8 err = 0;
            err = CET632_ReadReg(CET632_REG_ERR);
            state = CET632_CheckErr(err);
            break; 
        }
        tick--;
    }
    CET632_WriteReg(CET632_REG_CMD, CET632_CMD_IDLE);
    if(tick == 0)
    {
        state = CET632_ERR_TIMEOUT;
    }

    return state;
}
