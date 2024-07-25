#include "AnyID_RC522.h"
#include "AnyID_Des.h"
RC522_FRAME g_sRc522Frame = {0};
ISO14443A_4 g_sRc522ISO1444A4 = {0};

/******************************************************************************
** 函数名：RC522_WriteReg
** 说  明: 写入RC522寄存器
**
** 参  数：addr：地址
           val：值
** 返  回：void
******************************************************************************/
void RC522_WriteReg(u8 addr, u8 val)
{
    RC522_CS_Low();

    RC522_WriteByte(RC522_WADDR(addr));
    RC522_WriteByte(val);

    RC522_CS_High();
}

/******************************************************************************
** 函数名：RC522_ReadReg
** 说  明: 读RC522寄存器
**
** 参  数：addr：地址
** 返  回：寄存器值
******************************************************************************/
u8 RC522_ReadReg(u8 addr)
{
    u8 val = 0;

    RC522_CS_Low();

    RC522_WriteByte(RC522_RADDR(addr));
    val = RC522_WriteByte(0x00);

    RC522_CS_High();

    return val;
}

/******************************************************************************
** 函数名：RC522_SetRegBit
** 说  明: 设置寄存器的某一位
**
** 参  数：addr：地址
           bit：需要置1的位
** 返  回：void
******************************************************************************/
void RC522_SetRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = RC522_ReadReg(addr);
    val |= bit;
    RC522_WriteReg(addr, val);
}

/******************************************************************************
** 函数名：RC522_ClearRegBit
** 说  明: 清零寄存器的某一位
**
** 参  数：addr：地址
           bit：需清零要置1的位
** 返  回：void
******************************************************************************/
void RC522_ClearRegBit(u8 addr, u8 bit)
{
    u8 val = 0;

    val = RC522_ReadReg(addr);
    val &= (~bit);
    RC522_WriteReg(addr, val);
}

/******************************************************************************
** 函数名：RC522_ReadFIFO
** 说  明: 读RC522的FIFO
**
** 参  数：pBuffer：读取的数据缓存区
           len：需要读取的长度
** 返  回：void
******************************************************************************/
void RC522_ReadFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    RC522_CS_Low();

    RC522_WriteByte(RC522_RADDR(RC522_REG_FIFODATA));

    for(i = 0; i < (len - 1); i++)
    {
        pBuffer[i] = RC522_WriteByte(RC522_RADDR(RC522_REG_FIFODATA));
    }

    pBuffer[i] = RC522_WriteByte(0x00);

    RC522_CS_High();
}

/******************************************************************************
** 函数名：RC522_ReadFIFO
** 说  明: 写RC522的FIFO
**
** 参  数：pBuffer：写数据读取的缓存区
           len：需要写的长度
** 返  回：void
******************************************************************************/
void RC522_WriteFIFO(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    RC522_CS_Low();

    RC522_WriteByte(RC522_WADDR(RC522_REG_FIFODATA));

    for(i = 0; i < len; i++)
    {
        RC522_WriteByte(pBuffer[i]);
    }

    RC522_CS_High();
}

/******************************************************************************
** 函数名：RC522_Init
** 说  明: 初始化RC522
**
** 参  数：void
** 返  回：void
******************************************************************************/
u8 RC522_Init(void)
{
    u8 regVal = 0;

    //系统硬件复位
    RC522_Close();                                          //低电平脉冲宽度大于100ns即可
    RC522_Delay16us(1);
    RC522_Open();

    //晶振稳定时间37.76us
    RC522_Delay16us(10);

    //这个必须设置
    RC522_WriteReg(RC522_REG_TXAUTO, RC522_BIT_100ASK);     //force to 100% ASK
    regVal = RC522_ReadReg(RC522_REG_TXAUTO);
    if(regVal != RC522_BIT_100ASK)
    {
        return RC522_STAT_INIT_ERROR;
    }

    //因为CRC-A的校验码就是6363，所以这里必须设置成6363，否则以后的CRC校验总是错误
    RC522_WriteReg(RC522_REG_MODE, 0x3D);                   //CRC 6363

    //开启天线
    RC522_CloseAntenna();
    RC522_Delay16us(10);
    RC522_OpenAntenna();

    //不使用定时器，使用自己查询模式
    RC522_WriteReg(RC522_REG_TMODE, 0x00);
    RC522_WriteReg(RC522_REG_TRELOADLO, 0xFF);
    RC522_WriteReg(RC522_REG_TRELOADHI, 0XFF);

    RC522_WriteReg(RC522_REG_WATERLEVEL, RC522_FIFO_LEVEL);


    //系统置于空闲模式
    RC522_WriteReg(RC522_REG_COMMAND, RC522_CMD_IDLE);

    return RC522_STAT_OK;
}

u8 RC522_Command(void)
{
    u8 state = 0;
    u32 tick = 0;
    u8 reg = 0;
    u8 txPos = 0;
    u8 fifoLevel = 0;

    g_sRc522Frame.rxLen = 0;
    tick = g_sRc522Frame.timeout >> 4; //16us = tick

    //终止当前所有操作
    RC522_WriteReg(RC522_REG_COMMAND, RC522_CMD_IDLE);
    //清空FIFO
    RC522_WriteReg(RC522_REG_FIFOLEVEL, RC522_BIT_FLUSHBUFFER);
    //清除所有中断标志位
    RC522_WriteReg(RC522_REG_COMMIRQ, 0x7F);

    //希尔数据到FIFO，如果数据超过FIFO的长度，就等发送后FIFO空出位置再写
    if(g_sRc522Frame.txLen > RC522_SIZE_FIFO)
    {
        //将发送数据写入fifo
        RC522_WriteFIFO(g_sRc522Frame.frame, RC522_SIZE_FIFO);
        txPos += RC522_SIZE_FIFO;
        g_sRc522Frame.txLen -= RC522_SIZE_FIFO;
    }
    else
    {
        //将发送数据写入fifo
        RC522_WriteFIFO(g_sRc522Frame.frame, g_sRc522Frame.txLen);
        txPos += g_sRc522Frame.txLen;
        g_sRc522Frame.txLen -= g_sRc522Frame.txLen;
    }

    //清除所有中断标志位
    RC522_WriteReg(RC522_REG_COMMIRQ, 0x7F);

    //激活命令，RC522_CMD_TRANSCEIVE使用RC522_CMD_STARTSEND激活
    //          其他命令写入CMDReg后就自动激活
    RC522_WriteReg(RC522_REG_COMMAND, g_sRc522Frame.cmd & RC522_MASK_COMMAND);

    if(g_sRc522Frame.cmd == RC522_CMD_TRANSCEIVE)
    {
        //触发发送
        RC522_SetRegBit(RC522_REG_BITFRAMING, RC522_BIT_STARTSEND);
    }

    //发送其他数据帧
    while(g_sRc522Frame.txLen > 0)
    {
        reg = RC522_ReadReg(RC522_REG_COMMIRQ);
        //如果FIFO低中断标志出现，表示FIFO有空位，可以写入数据
        if(reg & RC522_BIT_LOALERTI)
        {
            if(g_sRc522Frame.txLen > (RC522_SIZE_FIFO - RC522_FIFO_LEVEL))
            {
                //将发送数据写入fifo
                RC522_WriteFIFO(g_sRc522Frame.frame + txPos, RC522_SIZE_FIFO - RC522_FIFO_LEVEL);
                txPos += (RC522_SIZE_FIFO - RC522_FIFO_LEVEL);
                g_sRc522Frame.txLen -= (RC522_SIZE_FIFO - RC522_FIFO_LEVEL);
            }
            else
            {
                //将发送数据写入fifo
                RC522_WriteFIFO(g_sRc522Frame.frame + txPos, g_sRc522Frame.txLen);
                txPos += g_sRc522Frame.txLen;
                g_sRc522Frame.txLen -= g_sRc522Frame.txLen;
            }
            RC522_WriteReg(RC522_REG_COMMIRQ, RC522_BIT_LOALERTI); //清空lo标志
        }
        else if(reg & RC522_BIT_ERRI)
        {
            tick = 0;
            break;
        }
    }

    //清除所有中断标志位
    RC522_WriteReg(RC522_REG_COMMIRQ, 0x7F);

    while(tick)
    {
        //这里耗时基本是10us
        RC522_Delay16us(1);
        reg = RC522_ReadReg(RC522_REG_COMMIRQ);
        if(reg & RC522_BIT_ERRI)
        {
            state = RC522_STAT_ERROR;
            g_sRc522Frame.rxLen = 0;
            break;
        }
        else if(reg & RC522_BIT_RXI)
        {
            fifoLevel = RC522_ReadReg(RC522_REG_FIFOLEVEL);
            RC522_ReadFIFO(g_sRc522Frame.frame + g_sRc522Frame.rxLen, fifoLevel);
            g_sRc522Frame.rxLen += fifoLevel;
            break;
        }
        else if(reg & RC522_BIT_TXI)
        {
            if(reg & RC522_BIT_HIALERTI)
            {
                fifoLevel = RC522_ReadReg(RC522_REG_FIFOLEVEL);
                RC522_ReadFIFO(g_sRc522Frame.frame + g_sRc522Frame.rxLen, fifoLevel);
                RC522_WriteReg(RC522_REG_COMMIRQ, RC522_BIT_HIALERTI);
                g_sRc522Frame.rxLen += fifoLevel;
            }
            else if(g_sRc522Frame.cmd == RC522_CMD_TRANSMIT)
            {
                break;
            }
        }
        else if(reg & RC522_BIT_IDLEI)
        {
            fifoLevel = RC522_ReadReg(RC522_REG_FIFOLEVEL);
            RC522_ReadFIFO(g_sRc522Frame.frame + g_sRc522Frame.rxLen, fifoLevel);
            g_sRc522Frame.rxLen += fifoLevel;
            break;
        }
        tick--;
    }

    //终止当前所有操作
    RC522_WriteReg(RC522_REG_COMMAND, RC522_CMD_IDLE);

    if(g_sRc522Frame.cmd == RC522_CMD_TRANSCEIVE)
    {
        //停止发送
        RC522_ClearRegBit(RC522_REG_BITFRAMING, RC522_BIT_STARTSEND);
    }

    if(tick == 0)
    {
        state = RC522_STAT_TIMEOUT;
    }
    return state;
}


/******************************************************************************
** 函数名：RC522_CheckACK
** 说  明：解析ACK数据
**
** 参  数：ack：ACK值
** 返  回：操作结果
******************************************************************************/
u8 RC522_CheckACK(u8 ack)
{
    u8 state = RC522_STAT_OK;

    ack = ack & MIFARE_ACK_MASK;

    switch(ack)
    {
        case MIFARE_ACK_OK:                     //应答:成功
            state = RC522_STAT_OK;
            break;
        case MIFARE_ACK_NOT_ALLOW:              //应答:不允许操作(授权验证错误)
            state = RC522_STAT_AUTH_ERROR;
            break;
        case MIFARE_ACK_TRS_ERROR:              //应答:传输错误
            state = RC522_STAT_TRANS_ERROR;
            break;
        default:
            state = RC522_STAT_ERROR;          //应答:其他错误
            break;
    }

    return state;
}


void RC522_Config(BOOL bClearCrypt, BOOL bTxCrc, BOOL bRxCrc)
{
    if(bClearCrypt)
    {
        RC522_ClearRegBit(RC522_REG_STATUS2, RC522_BIT_CRYPTO1ON);   //明文发送
    }

    if(bTxCrc)
    {
        RC522_SetRegBit(RC522_REG_TXMODE, RC522_BIT_CRCEN);
    }
    else
    {
        RC522_ClearRegBit(RC522_REG_TXMODE, RC522_BIT_CRCEN);
    }

    if(bRxCrc)
    {
        RC522_SetRegBit(RC522_REG_RXMODE, RC522_BIT_CRCEN);
    }
    else
    {
        RC522_ClearRegBit(RC522_REG_RXMODE, RC522_BIT_CRCEN);
    }

    //RC522_WriteReg(RC522_REG_BITFRAMING, txBit | (rxBit << 4));   //请求命令是短帧，只有7位
}

/******************************************************************************
** 函数名：RC522_RequestPICC
** 说  明：请求PICC，有两种请求模式，如果场内有PICC，标签会将自己的类型响应PCD
**
** 参  数：reqCode：请求类型
           pMifareType：请求结果：PICC类型码
** 返  回：操作结果
******************************************************************************/
u8 RC522_RequestPICC(u8 reqCode, u16 *pMifareType)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = reqCode;
    g_sRc522Frame.txLen = 1;

    g_sRc522Frame.timeout = MIFARE_NORMAL_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(TRUE, FALSE, FALSE);
    RC522_WriteReg(RC522_REG_BITFRAMING, MIFARE_REQUEST_BITS);

    state = RC522_Command();
    if(state == RC522_STAT_OK)
    {
        *pMifareType = g_sRc522Frame.frame[0];
        *pMifareType <<= 8;
        *pMifareType |= g_sRc522Frame.frame[1];
    }

   return state;

}




/******************************************************************************
** 函数名：RC522_AnticollPICC
** 说  明：防碰撞:最多可以分为三个等级 L1_UID_LEN = 4B L2_UID_LEN = 7B L3_UID_LEN = 10B
                  不同卡的类型，防碰撞等级不一样
                  这里可能还有需要修改的地方:当有冲突时，应该如何处理，这里做的还不完整
**
** 参  数：pSnr：返回PICC的UID
           cascadeLevel：防碰撞等级
** 返  回：操作结果
******************************************************************************/
u8 RC522_AnticollPICC(u8 *pSnr, u8 cascadeLevel)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = cascadeLevel;
    g_sRc522Frame.frame[1] = MIFARE_NVB_MIN_PARAMETER;
    g_sRc522Frame.txLen = 2;

    g_sRc522Frame.timeout = MIFARE_NORMAL_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(TRUE, FALSE, FALSE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    //UID:4个字节typeA
    if(state == RC522_STAT_OK)
    {
        state = RC522_CheckBcc(g_sRc522Frame.frame);
        if(state == RC522_STAT_OK)
        {
            memcpy(pSnr, g_sRc522Frame.frame, MIFARE_UID_L1_LEN + 1);
        }
    }

    return state;
}

/******************************************************************************
** 函数名：RC522_Select
** 说  明：根据防碰撞的结果，选择卡
**
** 参  数：pSnr：需要进行选择UID
           cascadeLevel：防碰撞等级
** 返  回：操作结果
******************************************************************************/
u8 RC522_Select(u8 *pSnr, u8 cascadeLevel, u8 *pAsk)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = cascadeLevel;
    g_sRc522Frame.frame[1] = MIFARE_NVB_MAX_PARAMETER;
    memcpy(g_sRc522Frame.frame + 2, pSnr, MIFARE_UID_L1_LEN + 1); //包含BCC
    g_sRc522Frame.txLen = 7;

    g_sRc522Frame.timeout = MIFARE_NORMAL_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(TRUE, TRUE, TRUE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    //UID:4个字节typeA
    if(state == RC522_STAT_OK)
    {
        *pAsk = g_sRc522Frame.frame[0];
    }

    return state;
}

/******************************************************************************
** 函数名：RC522_SelectPICC
** 说  明：选择卡，如果卡的UID超过4个字节是需要升级防碰撞的等级，然后重新选择
**
** 参  数：pSnr：需要进行选择UID
** 返  回：操作结果
******************************************************************************/
u8 RC522_SelectPICC(u8 mode, u16 *pMifareType, u8 *pSnr, u8 *pSnrLen, u8 *pAsk)
{
    u8 state = RC522_STAT_OK;
    u8 snr[MIFARE_UID_MAX_LEN] = {0};

    state = RC522_RequestPICC(mode, pMifareType);
    if(state == RC522_STAT_OK)
    {
        state = RC522_AnticollPICC(snr, MIFARE_CMD_ANTICOLL_L1);
        if(state == RC522_STAT_OK)
        {
            state = RC522_Select(snr, MIFARE_CMD_ANTICOLL_L1, pAsk);
            if(state == RC522_STAT_OK)
            {
                *pSnrLen = MIFARE_UID_L1_LEN;
                if(snr[0] == MIFARE_CT)
                {
                    memcpy(pSnr, snr + 1, MIFARE_UID_L1_LEN - 1);
                    state = RC522_AnticollPICC(snr, MIFARE_CMD_ANTICOLL_L2);
                    if(state == RC522_STAT_OK)
                    {
                        state = RC522_Select(snr, MIFARE_CMD_ANTICOLL_L2, pAsk);
                        if(state == RC522_STAT_OK)
                        {
                            *pSnrLen = MIFARE_UID_L2_LEN;
                            if(snr[0] == MIFARE_CT)
                            {
                                memcpy(pSnr + MIFARE_UID_L1_LEN - 1, snr + 1, MIFARE_UID_L1_LEN - 1);

                                state = RC522_AnticollPICC(snr, MIFARE_CMD_ANTICOLL_L3);
                                if(state == RC522_STAT_OK)
                                {
                                    state = RC522_Select(snr, MIFARE_CMD_ANTICOLL_L3, pAsk);
                                    if(state == RC522_STAT_OK)
                                    {
                                        *pSnrLen = MIFARE_UID_L3_LEN;
                                        memcpy(pSnr + 6, snr, MIFARE_UID_L1_LEN);
                                    }
                                }
                            }
                            else
                            {
                                memcpy(pSnr + MIFARE_UID_L1_LEN - 1, snr, MIFARE_UID_L1_LEN);
                            }
                        }
                    }
                }
                else
                {
                    memcpy(pSnr, snr, MIFARE_UID_L1_LEN);
                }
            }
        }
    }


    return state;
}

/******************************************************************************
** 函数名：RC522_AuthPICC
** 说  明：授权验证
**
** 参  数：authMode：授权模式（A/B）
		   pSnr：需要进行授权的UID
           pPassWord：密码
           addr：块地址
** 返  回：操作结果
******************************************************************************/
u8 RC522_AuthPICC(u8 authMode, u8 *pSnr, u8 *pPassWord, u8 addr)
{
    u8 state = RC522_STAT_OK;
    u8 reg = 0;

    g_sRc522Frame.frame[0] = authMode;
    g_sRc522Frame.frame[1] = addr;
    memcpy(g_sRc522Frame.frame + 2, pPassWord, MIFARE_M1_KEY_LEN);
    memcpy(g_sRc522Frame.frame + 8, pSnr, MIFARE_UID_L1_LEN);
    g_sRc522Frame.txLen = 12;

    g_sRc522Frame.timeout = MIFARE_AUTH_TIME;
    g_sRc522Frame.cmd = RC522_CMD_AUTHENT;

    RC522_Config(FALSE, FALSE, FALSE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    RC522_Command();

    reg = RC522_ReadReg(RC522_REG_STATUS2);
    if((reg & RC522_BIT_CRYPTO1ON) == 0)//
    {
        state = RC522_STAT_AUTH_ERROR;
    }

    return state;
}


/******************************************************************************
** 函数名：RC522_ReadPICC
** 说  明：读PICC数据块
**
** 参  数：addr：块地址
           pDataBuffer：读取的数据缓存区
** 返  回：操作结果
******************************************************************************/
u8 RC522_ReadPICC(u8 addr, u8 *pDataBuffer)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = MIFARE_CMD_READ;
    g_sRc522Frame.frame[1] = addr;
    g_sRc522Frame.txLen = 2;

    g_sRc522Frame.timeout = MIFARE_READ_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(FALSE, TRUE, TRUE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    //UID:4个字节typeA
    if(state == RC522_STAT_OK)
    {
        if(g_sRc522Frame.rxLen == MIFARE_M1BLOCK_LEN)
        {
            memcpy(pDataBuffer, g_sRc522Frame.frame, MIFARE_M1BLOCK_LEN);
        }
        else
        {
            state = RC522_STAT_ERROR;
        }
    }

    return state;
}

/******************************************************************************
** 函数名：RC522_Write4PICC
** 说  明：写PICC数据块，写入4字节
**
** 参  数：addr：块地址
           pDataBuffer：写读取的数据缓存区
** 返  回：操作结果
******************************************************************************/
u8 RC522_Write4PICC(u8 addr, u8 *pDataBuffer)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = MIFARE_CMD_WRITE4;
    g_sRc522Frame.frame[1] = addr;
    memcpy(g_sRc522Frame.frame + 2, pDataBuffer, MIFARE_M0BLOCK_LEN);
    g_sRc522Frame.txLen = 6;

    g_sRc522Frame.timeout = MIFARE_WRITE_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(FALSE, TRUE, FALSE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    //UID:4个字节typeA
    if(state == RC522_STAT_OK)
    {
        if(g_sRc522Frame.frame[0] != MIFARE_ACK_OK)
        {
            state = RC522_STAT_NAK;
        }
    }

    return state;
}


/******************************************************************************
** 函数名：RC522_Write16PICC
** 说  明：写入16字节，MIFARE Ultralight卡的写入16字节和写入4字节功能一样
           写入16字节分两步走:写入地址；写入16字节数据
**
** 参  数：addr：块地址
           pDataBuffer：写读取的数据缓存区
** 返  回：操作结果
******************************************************************************/
u8 RC522_Write16PICC(u8 addr, u8 *pDataBuffer)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = MIFARE_CMD_WRITE16;
    g_sRc522Frame.frame[1] = addr;
    g_sRc522Frame.txLen = 2;

    g_sRc522Frame.timeout = MIFARE_WRITE_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(FALSE, TRUE, FALSE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    //UID:4个字节typeA
    if(state == RC522_STAT_OK)
    {
        if(g_sRc522Frame.frame[0] != MIFARE_ACK_OK)
        {
            state = RC522_STAT_NAK;
        }
        else
        {
            memcpy(g_sRc522Frame.frame, pDataBuffer, MIFARE_M1BLOCK_LEN);
            g_sRc522Frame.txLen = MIFARE_M1BLOCK_LEN;

            state = RC522_Command();

            if(state == RC522_STAT_OK)
            {
                if(g_sRc522Frame.frame[0] != MIFARE_ACK_OK)
                {
                    state = RC522_STAT_NAK;
                }
            }
        }
    }

    return state;
}

/******************************************************************************
** 函数名：RC522_ValuePICC
** 说  明：钱包操作，分3步走
**
** 参  数：operationMode：钱包操作模式
           blockAddr：块地址
           pValue：操作值
           valueAddr：值地址
** 返  回：操作结果
******************************************************************************/
u8 RC522_ValuePICC(u8 operationMode, u8 blockAddr, u8 *pValue, u8 valueAddr)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = operationMode;
    g_sRc522Frame.frame[1] = blockAddr;
    g_sRc522Frame.txLen = 2;

    g_sRc522Frame.timeout = MIFARE_VALUE_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(FALSE, TRUE, FALSE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    //UID:4个字节typeA
    if(state == RC522_STAT_OK)
    {
        if(g_sRc522Frame.frame[0] != MIFARE_ACK_OK)
        {
            state = RC522_STAT_NAK;
        }
        else
        {
            memcpy(g_sRc522Frame.frame, pValue, MIFARE_M1VALUE_LEN);
            g_sRc522Frame.txLen = MIFARE_M1VALUE_LEN;

            state = RC522_Command();

            g_sRc522Frame.frame[0] = MIFARE_CMD_TRANSFER;
            g_sRc522Frame.frame[1] = valueAddr;
            g_sRc522Frame.txLen = 2;

            state = RC522_Command();

            if(state == RC522_STAT_OK)
            {
                if(g_sRc522Frame.frame[0] != MIFARE_ACK_OK)
                {
                    state = RC522_STAT_NAK;
                }
            }
        }
    }

    return state;
}

/******************************************************************************
** 函数名：RC522_Halt
** 说  明：卡挂起
**
** 参  数：无
** 返  回：操作结果
******************************************************************************/
u8 RC522_Halt(void)
{
    u8 state = RC522_STAT_OK;

    g_sRc522Frame.frame[0] = MIFARE_CMD_HALTA;
    g_sRc522Frame.frame[1] = 0x00;
    g_sRc522Frame.txLen = 2;

    g_sRc522Frame.timeout = MIFARE_NORMAL_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSMIT;

    RC522_Config(FALSE, TRUE, TRUE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    return state;
}


void RC522_ValueBlock(u8 *pValue, u8 addr, u8 *pBlock)
{
    u8 i = 0;
    for(i = 0; i < 4; i++)
    {
        pBlock[i] = pValue[i];
        pBlock[i + 8] = pValue[i];
        pBlock[i + 4] = ~pValue[i];
    }

    pBlock[12] = addr;
    pBlock[13] = ~addr;
    pBlock[14] = addr;
    pBlock[15] = ~addr;
}

BOOL RC522_CheckValueFormat(u8 *pBlock)
{
    BOOL b = TRUE;

    u32 value1 = 0, value2 = 0, value3 = 0;
    u8 addr1 = 0, addr2 = 0, addr3 = 0, addr4 = 0;

    value1 |= (pBlock[0]);
    value1 <<= 8;
    value1 |= (pBlock[1]);
    value1 <<= 8;
    value1 |= (pBlock[2]);
    value1 <<= 8;
    value1 |= (pBlock[3]);

    value2 |= (pBlock[4]);
    value2 <<= 8;
    value2 |= (pBlock[5]);
    value2 <<= 8;
    value2 |= (pBlock[6]);
    value2 <<= 8;
    value2 |= (pBlock[7]);
    value2 = ~value2;

    value3 |= (pBlock[8]);
    value3 <<= 8;
    value3 |= (pBlock[9]);
    value3 <<= 8;
    value3 |= (pBlock[10]);
    value3 <<= 8;
    value3 |= (pBlock[11]);

    addr1 = pBlock[12];
    addr2 = pBlock[13];
    addr2 = ~addr2;
    addr3 = pBlock[14];
    addr4 = pBlock[15];
    addr4 = ~addr4;

    if((value1 != value3) || (value2 != value3) ||
       (addr1 != addr2) || (addr2 != addr3) || (addr3 != addr4))
    {
        b = FALSE;
    }

    return b;
}


u8 RC522_CheckBcc(u8 *pSnr)
{
    u8 bcc = 0;

    bcc = pSnr[0] ^ pSnr[1] ^ pSnr[2] ^ pSnr[3] ^ pSnr[4];
    if(bcc == 0)
    {
        return RC522_STAT_OK;
    }
    else
    {
        return RC522_STAT_BCC_ERROR;
    }
}


const u16 fsdi[16] = {16, 24, 32, 40, 48, 64, 96, 128, 256, 257, 257, 257, 257, 257, 257, 257};
u8 RC522_Rats(void)
{
    u8 state = RC522_STAT_OK;

    memset(&g_sRc522ISO1444A4, 0, sizeof(g_sRc522ISO1444A4));

    g_sRc522Frame.frame[0] = ISO14443A_CMD_RATS;
    g_sRc522Frame.frame[1] = ISO14443A_FSDI_FSD | ISO14443A_CID;  //最多接收64字节
    g_sRc522Frame.txLen = 2;

    g_sRc522Frame.timeout = ISO14443A_APDU_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(TRUE, TRUE, TRUE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    if(state == RC522_STAT_OK)
    {
        u8 param = 0;

        param = g_sRc522Frame.frame[1];
        g_sRc522ISO1444A4.fsc = param & 0x0F;
        g_sRc522ISO1444A4.fsc = fsdi[g_sRc522ISO1444A4.fsc];
        if(param & 0x20)      //TB是否存在
        {
            if(param & 0x10)  //TA是否存在
            {
                g_sRc522ISO1444A4.fwt = (g_sRc522Frame.frame[3] >> 4) & 0x0F;
            }
            else
            {
                g_sRc522ISO1444A4.fwt = (g_sRc522Frame.frame[2] >> 4) & 0x0F;
            }
        }
        else
        {

            g_sRc522ISO1444A4.fwt = ISO14443A4_FWI_DFT;
        }
        //fwt = (256*16/fc) * 2^fwi = 2^(fwi + 12)/13.56 us
        g_sRc522ISO1444A4.fwt = (1 << (g_sRc522ISO1444A4.fwt + 12)) / 14;    //单位us
        g_sRc522ISO1444A4.fwt += 1000;  //多加1ms
    }

    return state;
}


u8 RC522_TransAPDU(u8 *pApdu, u8 len)
{
    u8 pcb = 0;
    u8 state = RC522_STAT_OK;

    RC522_Config(TRUE, TRUE, TRUE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);


    memcpy(g_sRc522Frame.frame, pApdu, len);
    g_sRc522Frame.txLen = len;

    g_sRc522Frame.timeout = g_sRc522ISO1444A4.fwt; //fwt单位是us
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    //因为有些处理需要比较长的时间，所以PICC有时会发送SBLOCK请求延时
    //如果响应帧是SBLOCK中的WTX，那么将WTX请求直接发送到PICC，一直到接收真正的响应帧
    do{
        state = RC522_Command();
        if(state == RC522_STAT_OK)
        {
            pcb = g_sRc522Frame.frame[0] & (~ISO14443A_IPCB_NUMMASK);
            if((pcb == ISO14443A_PCB_IBLOCK) || (pcb == ISO14443A_PCB_RBLOCK))
            {
                if((g_sRc522Frame.frame[0] & ISO14443A_IPCB_NUMMASK) == (g_sRc522ISO1444A4.pcb & ISO14443A_IPCB_NUMMASK))
                {
                    g_sRc522ISO1444A4.pcb = (~g_sRc522ISO1444A4.pcb) & ISO14443A_IPCB_NUMMASK;
                }
            }
            else if(pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX))
            {
                g_sRc522Frame.txLen = g_sRc522Frame.rxLen;
            }
            else
            {
                state = RC522_STAT_ERROR;
            }
        }
    }while((state == RC522_STAT_OK) && (pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX)));

    return state;
}

u8 RC522_AuthUltralightC(u8 *pKey)
{
    u8 state = RC522_STAT_OK;
    u8 iv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 rnd[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 rnda[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 rndb[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 t = 0;

    memcpy(rnd, rnda, 8);

    Des_InitializeKey(pKey, 0);
    Des_InitializeKey(pKey + 8, 1);

    g_sRc522Frame.frame[0] = MIFARE_CMD_AUTHM0U2S1;
    g_sRc522Frame.frame[1] = 0x00;
    g_sRc522Frame.txLen = 2;

    g_sRc522Frame.timeout = MIFARE_NORMAL_TIME;
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    RC522_Config(FALSE, TRUE, TRUE);
    RC522_WriteReg(RC522_REG_BITFRAMING, 0);

    state = RC522_Command();

    if(state == RC522_STAT_OK)
    {
        //解密rndb
        Des_DecryptAnyLength(g_sRc522Frame.frame + 1, 8, rndb, 0);
        Des_EncryptAnyLength(rndb, 8, temp, 1);
        Des_DecryptAnyLength(temp, 8, rndb, 0);
        //CBC模式
        a_xor(rndb, iv, 8);
        memcpy(iv, g_sRc522Frame.frame + 1, 8);

        //rndb循环左移8bit得到rndb1
        t = rndb[0];
        memcpy(rndb, rndb + 1, 7);
        rndb[7] = t;

        //CBC模式
        a_xor(rnda, iv, 8);
        //计算ek(rnda)
        Des_EncryptAnyLength(rnda, 8, g_sRc522Frame.frame + 1, 0);
        Des_DecryptAnyLength(g_sRc522Frame.frame + 1, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sRc522Frame.frame + 1, 0);
        memcpy(iv, g_sRc522Frame.frame + 1, 8);

        //CBC模式
        a_xor(rndb, iv, 8);
        //计算ek(rndb1)
        Des_EncryptAnyLength(rndb, 8, g_sRc522Frame.frame + 9, 0);
        Des_DecryptAnyLength(g_sRc522Frame.frame + 9, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sRc522Frame.frame + 9, 0);
        memcpy(iv, g_sRc522Frame.frame + 9, 8);

        g_sRc522Frame.txLen = 1 + 8 + 8;
        state = RC522_Command();

        if(state == RC522_STAT_OK)
        {
            //解密rnda
            Des_DecryptAnyLength(g_sRc522Frame.frame + 1, 8, rnda, 0);
            Des_EncryptAnyLength(rnda, 8, temp, 1);
            Des_DecryptAnyLength(temp, 8, rnda, 0);
            //CBC模式
            a_xor(rnda, iv, 8);

            //rndb循环右移8bit得到rnda
            t = rnd[0];
            memcpy(rnd, rnd + 1, 7);
            rnd[7] = t;
            if(memcmp(rnda, rnd, 8) != 0)
            {
                state = RC522_STAT_AUTH_ERROR;
            }
        }
    }

    return state;
}

u8 RC522_Dtu(u8 *pFrane, u8 len, u8 lastBit, u16 to)
{
    u8 pcb = 0;
    u8 state = RC522_STAT_OK;

    RC522_WriteReg(RC522_REG_BITFRAMING, lastBit);

    memcpy(g_sRc522Frame.frame, pFrane, len);
    g_sRc522Frame.txLen = len;

    g_sRc522Frame.timeout = to; //单位是us
    g_sRc522Frame.cmd = RC522_CMD_TRANSCEIVE;

    state = RC522_Command();

    return state;
}



