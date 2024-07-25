#include "AnyID_MH663.h"
MH663_FRAME g_sMH663Frame = {0};

#if 0
const u8 g_sRc663RegAdd[MH663_PROTOCOL_CONFIG_SIZE] =
{
    MH663_REG_DRVMODE,
    MH663_REG_TXAMP,
    MH663_REG_DRVCON,
    MH663_REG_TXI,
    MH663_REG_TXCRCCON,
    MH663_REG_RXCRCCON,
    MH663_REG_TXDATANUM,
    MH663_REG_TXMODWIDTH,
    MH663_REG_TXSYM10BURSTLEN,
    MH663_REG_TXWAITCTRL,
    MH663_REG_FRAMECON,
    MH663_REG_RXCTRL,
    MH663_REG_RXTHRESHOLD,
    MH663_REG_RCV,
    MH663_REG_RXANA
};
#endif

void MH663_WriteReg(u8 addr, u8 val)
{
    MH663_CS_Low();

    MH663_WriteByte(MH663_WADDR(addr));
    MH663_WriteByte(val);

    MH663_CS_High();
}

u8 MH663_ReadReg(u8 addr)
{
    u8 val = 0;

    MH663_CS_Low();

    MH663_WriteByte(MH663_RADDR(addr));
    val = MH663_WriteByte(0xFF);

    MH663_CS_High();

    return val;
}

void MH663_SetRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = MH663_ReadReg(addr);
    val |= bit;
    MH663_WriteReg(addr, val);
}

void MH663_ClearRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = MH663_ReadReg(addr);
    val &= (~bit);
    MH663_WriteReg(addr, val);
}

void MH663_SetRegBits(u8 addr, u8 msk, u8 bit)
{
    u8 val = 0;

    val = MH663_ReadReg(addr);
    val &= (~msk);
    val |= bit;
    MH663_WriteReg(addr, val);
}

void MH663_ReadFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    for(i = 0; i < len; i++)
    {
        pBuffer[i] = MH663_ReadReg(MH663_REG_FIFODAT);
    }
}

void MH663_WriteFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;
    for(i = 0; i < len; i++)
    {
        MH663_WriteReg(MH663_REG_FIFODAT, pBuffer[i]);
    }
}

BOOL MH663_Reset(void)
{
    u8 regVal = MH663_USER_ERROR;
    BOOL bOk = FALSE;
    u8 t = 0;

    //系统硬件复位
    MH663_PowerHigh();                                          //低电平脉冲宽度大于100ns即可
    MH663_Delay16us(10);
    MH663_PowerLow();

    //等待系统稳定
    do{
        MH663_Delay16us(1000);
        t++;
        regVal = MH663_ReadReg(MH663_REG_VERSION);
        if(regVal == MH663_VERSION_INFO)
        {
            bOk = TRUE;
            break;
        }
    }while(t < 100);

    return bOk;
}

u8 MH663_Init(void)
{
    u8 state = MH663_USER_ERROR;
    if(MH663_Reset())
    {
        state = MH663_STAT_OK;

        MH663_WriteReg(MH663_REG_CMD, MH663_PCD_IDLE);              //至于空闲状态
        //清空FIFO
        MH663_FlushFifo();        
        //WaterLevel = 16字节
        MH663_WriteReg(MH663_REG_FIFOWLEV, MH663_FIFO_LEVEL);
    }

    return state;
}

u8 MH663_Command(void)
{
    u8 state = MH663_STAT_OK;
    u32 tick = 0;
    u8 reg = 0;
    u8 fifoLevel = 0;

    g_sMH663Frame.rxLen = 0;
    
    //终止当前所有操作
    MH663_WriteReg(MH663_REG_CMD, MH663_PCD_IDLE);
    MH663_FlushFifo();      //清空FLASH

    //将发送数据写入fifo(一般指令的数据是不会超过64字节的)
    MH663_WriteFIFO(g_sMH663Frame.frame, g_sMH663Frame.txLen);
    //使能中断
    MH663_WriteReg(MH663_REG_COMIEN, MH663_COMIEN_ERRIEN | MH663_COMIEN_IDLEIEN | MH663_COMIEN_RXIEN | MH663_COMIEN_TXIEN);
    //清除所有中断标志位
    MH663_ClearComIrqFlag();
    //命令写入CMDReg后就自动激活
    MH663_WriteReg(MH663_REG_CMD, g_sMH663Frame.cmd & MH663_MASK_CMD);
    if(g_sMH663Frame.cmd == MH663_PCD_TRANSCEIVE)   //只有MH663_PCD_TRANSCEIVE指令需要
    {
        MH663_SetRegBit(MH663_REG_BITFRAME, MH663_BITFRAM_START_SEND);
    }
    tick = MH663_TX_FRAME_TIMEOUT;
    while(tick) //发送
    {
        MH663_Delay16us(1);
        //这里耗时基本是10us
        reg = MH663_ReadReg(MH663_REG_COMIRQ);
        if(reg & MH663_COMI_ERR)
        {
            reg = MH663_ReadReg(MH663_REG_ERROR);  //0x08 NoDataErr
            state = MH663_USER_ERROR;
            if(reg & MH663_ERR_COLL) //发生碰撞错误
            {
                state = MH663_STAT_COLLERR;
            }
            g_sMH663Frame.rxLen = 0;
            
            break;
        }
        else if(reg & MH663_COMI_TX)    //发送完成
        {
            break;
        }
        tick--;
    }
    //如果需要接收，就判断SOF标志
    if(state == MH663_STAT_OK && tick > 0 && (g_sMH663Frame.cmd != MH663_PCD_TRANSMIT))
    {
        tick = g_sMH663Frame.timeout >> 4;  //注意这里要除16，协议都是用us作为单位
        while(tick)
        {
            MH663_Delay16us(1);
            //这里耗时基本是10us
            reg = MH663_ReadReg(MH663_REG_COMIRQ);
            if(reg & MH663_COMI_ERR)              //发生错误
            {
                reg = MH663_ReadReg(MH663_REG_ERROR);  //0x08 NoDataErr
                state = MH663_USER_ERROR;
                if(reg & MH663_ERR_COLL) //发生碰撞错误
                {
                    state = MH663_STAT_COLLERR;
                }
                g_sMH663Frame.rxLen = 0;
                break;
            }
            else if(reg & MH663_COMI_RX)          //接收完成
            {
                break;
            }
            else
            {
                fifoLevel = MH663_ReadReg(MH663_REG_FIFOLEV) & MH663_FIFOLV_MASK;
                if(fifoLevel > 0)
                {
                    break;
                }
            }
            tick--;
        }
    }
    
    //接收数据
    if(state == MH663_STAT_OK && tick > 0 && (g_sMH663Frame.cmd != MH663_PCD_TRANSMIT))
    {
        tick = MH663_RX_FRAME_TIMEOUT;
        while(tick)
        {
            MH663_Delay16us(1);
            //这里耗时基本是10us
            reg = MH663_ReadReg(MH663_REG_COMIRQ);
            if(reg & MH663_COMI_ERR)              //发生错误
            {
                reg = MH663_ReadReg(MH663_REG_ERROR);  //0x08 NoDataErr
                state = MH663_USER_ERROR;
                if(reg & MH663_ERR_COLL) //发生碰撞错误
                {
                    state = MH663_STAT_COLLERR;
                }
                g_sMH663Frame.rxLen = 0;
                break;
            }
            else if(reg & MH663_COMI_RX)          //接收完成
            {
                fifoLevel = MH663_ReadReg(MH663_REG_FIFOLEV) & MH663_FIFOLV_MASK;
                MH663_ReadFIFO(g_sMH663Frame.frame + g_sMH663Frame.rxLen, fifoLevel);
                g_sMH663Frame.rxLen += fifoLevel;
                break;
            }
            else if(reg & MH663_COMI_IDLE)
            {
                fifoLevel = MH663_ReadReg(MH663_REG_FIFOLEV) & MH663_FIFOLV_MASK;
                MH663_ReadFIFO(g_sMH663Frame.frame + g_sMH663Frame.rxLen, fifoLevel);
                g_sMH663Frame.rxLen += fifoLevel;
                break;
            }
            tick--;
        }
    }
        
    //终止当前所有操作
    MH663_WriteReg(MH663_REG_CMD, MH663_PCD_IDLE);

    if(tick == 0)
    {
        state = MH663_STAT_TIMEOUT;
    }
    else if(state == MH663_STAT_OK)
    {
        if(g_sMH663Frame.rxLen == 0)
        {
            state = MH663_STAT_NOTAGERR;
        }
    }

    return state;
}

void MH663_Config(u8 bClearCrypt, u8 bEnTxCrc, u8 bEnRxCrc, u8 txLastBits, u8 rxAlignBits)
{
    u8 reg = 0;

    if(bClearCrypt)
    {
        MH663_ClearRegBit(MH663_REG_STATUS2, MH663_STATUS_CRYPTO_ON);
    }

    if(bEnTxCrc)
    {
        MH663_SetRegBit(MH663_REG_TXMODE, MH663_TXMODE_CRCEN);
    }
    else
    {
        MH663_ClearRegBit(MH663_REG_TXMODE, MH663_TXMODE_CRCEN);
    }

    if(bEnRxCrc)
    {
        MH663_SetRegBit(MH663_REG_RXMODE, MH663_RXMODE_CRCEN);
    }
    else
    {
        MH663_ClearRegBit(MH663_REG_RXMODE, MH663_RXMODE_CRCEN);
    }

    reg = MH663_ReadReg(MH663_REG_BITFRAME);
    reg &= (~MH663_BITFRAM_TXLAST_MASK);
    reg |= (MH663_BITFRAM_TXLAST_MASK & txLastBits);

    reg &= (~MH663_BITFRAM_RXALIGN_MASK);
    reg |= ((rxAlignBits << 4) & MH663_BITFRAM_RXALIGN_MASK);
    
    MH663_WriteReg(MH663_REG_BITFRAME, reg);
}
#if 0


u8 MH663_SelectProtocol(u8 tx, u8 rx)
{
    g_sMH663Frame.frame[0] = rx;
    g_sMH663Frame.frame[1] = tx;
    g_sMH663Frame.txLen = 2;

    g_sMH663Frame.timeout = 10000;
    g_sMH663Frame.cmd = MH663_CMD_LOADPROTOCOL;
    
    //终止当前所有操作
    MH663_WriteReg(MH663_REG_COMMAND, MH663_CMD_IDLE);
    
    //将发送数据写入fifo
    MH663_WriteFIFO(g_sMH663Frame.frame, g_sMH663Frame.txLen);
    
    //命令写入CMDReg后就自动激活
    MH663_WriteReg(MH663_REG_COMMAND, g_sMH663Frame.cmd & MH663_MASK_COMMAND);

    return MH663_STAT_OK;//MH663_Command();//
}

u8 MH663_TopazCommand(void)
{
    u8 pos = 0;
    u8 state = MH663_STAT_OK;
    u8 reg = 0;
    u32 tick = 0;

    reg = MH663_ReadReg(MH663_REG_FRAMECON);
    MH663_WriteReg(MH663_REG_FRAMECON, reg & (~MH663_BIT_TXPARITYEN));

    //终止当前所有操作
    MH663_WriteReg(MH663_REG_COMMAND, MH663_CMD_IDLE);
    //清空FIFO
    MH663_FlushFifo();


    MH663_Config(FALSE, FALSE, FALSE, 7, 0);
    MH663_WriteFIFO(g_sMH663Frame.frame, 1);
    MH663_ClearAllIrq0Flag();
    MH663_WriteReg(MH663_REG_COMMAND, MH663_CMD_TRANSMIT);
    tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
    while(tick--)
    {
        reg = MH663_ReadReg(MH663_REG_COMIRQ);
        if(reg & MH663_BIT_TXIRQ)
        {
            break;
        }
    }

    if(tick == 0)
    {
        state = MH663_STAT_TIMEOUT;
    }
    else
    {
        MH663_Config(FALSE, FALSE, FALSE, 0, 0);
        for(pos = 1; pos < g_sMH663Frame.txLen - 1; pos++)
        {
            //终止当前所有操作
            MH663_WriteReg(MH663_REG_COMMAND, MH663_CMD_IDLE);
            //清空FIFO
            MH663_FlushFifo();
            MH663_WriteFIFO(g_sMH663Frame.frame + pos, 1);
            MH663_ClearAllIrq0Flag();
            MH663_WriteReg(MH663_REG_COMMAND, MH663_CMD_TRANSMIT);
            tick = 10; //106k, 9.44us/bit, 94.4us/byte, tick = 10us
            while(tick--)
            {
                reg = MH663_ReadReg(MH663_REG_COMIRQ);
                if(reg & MH663_BIT_TXIRQ)
                {
                    break;
                }
            }
            if(tick == 0)
            {
                state = MH663_STAT_TIMEOUT;
            }
        }
    }

    if(state == MH663_STAT_OK)
    {
        g_sMH663Frame.frame[0] = g_sMH663Frame.frame[pos];
        g_sMH663Frame.txLen = 1;
        g_sMH663Frame.cmd = MH663_CMD_TRANSCEIVE;
        state = MH663_Command();
    }
    //还原
    reg = MH663_ReadReg(MH663_REG_FRAMECON);
    MH663_WriteReg(MH663_REG_FRAMECON, reg | MH663_BIT_TXPARITYEN);

    return state;
}



#define MH663_PRODUCTID_LEN         3
u8 MH663_ReadProductID(void)
{
    //256字节、清空FIFO、waterLevel8bit，fifoLength8bit
    MH663_WriteReg(MH663_REG_FIFOCONTROL, MH663_BIT_FIFOSIZE | MH663_BIT_FLUSHFIFO);
    
    g_sMH663Frame.frame[0] = 0;
    g_sMH663Frame.frame[1] = 0;
    g_sMH663Frame.frame[2] = MH663_PRODUCTID_LEN;
    MH663_WriteFIFO(g_sMH663Frame.frame, MH663_PRODUCTID_LEN);

    MH663_WriteReg(MH663_REG_COMMAND, MH663_CMD_READE2);
    MH663_Delay16us(100);
    g_sMH663Frame.rxLen = MH663_ReadReg(MH663_REG_FIFOLENGTH);
    if(g_sMH663Frame.rxLen == MH663_PRODUCTID_LEN)
    {
        MH663_ReadFIFO(g_sMH663Frame.frame, MH663_PRODUCTID_LEN);
        return g_sMH663Frame.frame[1];
    }
    else
    {
        return 0xFF;
    }
}
#endif