#include "AnyID_Trf796x.h"

//u8 g_Trf796xRegs[32] = {0};
u8 Trf796x_Init(u8 pwrLv)
{
    u8 err = TRF796X_ERR_OK;
    
    Trf796x_CsLow();
    Trf796x_EnLow();
    Trf796x_Delayms(10);
    
    Trf796x_CsHigh();
    Trf796x_Delayms(10);
    Trf796x_EnHigh();
    Trf796x_Delayms(10);
    
    Trf796x_SoftReset();
    Trf796x_Delayms(10);

    //13.56M crystal---ask by b0-b2----sysclk disable----disable pin12 an out----100% ook
    Trf796x_WriteReg(TRF796X_REG_MODULATOR_CONTROL, TRF796X_REG_ASK_100);
    Trf796x_Delayms(1);
    //auto regulator---VDD-RF-5V VDDA-3.4V VDDX-3.4V
    Trf796x_WriteReg(TRF796X_REG_REGULATOR_CONTROL, TRF796X_REG_AUTO_REGULATOR | TRF796X_REG_VOL_5_3P4_3P4);
    //active mode--direct no-----rf off-----full power---select main rx_in1 input----bit5 control rf-----5V power
    if(pwrLv == TRF796X_PWR_LV_0)
    {
        Trf796x_WriteReg(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_HALFPWR | TRF796X_REG_CTRL_RXIN2);//TRF796X_REG_CTRL_5V | 
    }
    else
    {
        Trf796x_WriteReg(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_RXIN2);//TRF796X_REG_CTRL_5V | 
    }
    Trf796x_Delayms(1);
    
    //rx-crc----dirmode0(这里无效)----rfid mode--15693 26k onesubcarrier 1of4
    Trf796x_WriteReg(TRF796X_REG_ISO_CONTROL, TRF796X_REG_ISO15693_HIBR);
    //15693 424k rxbw---gain reduction 0dBm
    Trf796x_WriteReg(TRF796X_REG_RX_SPECIAL_SETTINGS, TRF796X_REG_RXBW_424);
    
    if((Trf796x_ReadReg(TRF796X_REG_MODULATOR_CONTROL) & TRF796X_REG_ASK_100) == TRF796X_REG_ASK_100)
    {
        err = TRF796X_ERR_OK;
    }
    return err;
}

void Trf796x_SoftReset(void)
{	
	Trf796x_WriteCmd(TRF796X_CMD_SOFT_INIT);
	Trf796x_WriteCmd(TRF796X_CMD_IDLE);
}

void Trf796x_RfClose(void)
{
    Trf796x_ClearRegBits(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_RFON);
}

void Trf796x_RfOpen(void)
{
    Trf796x_SetRegBit(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_RFON);
}

void Trf796x_EnableFullPower(void)
{
    Trf796x_ClearRegBits(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_HALFPWR);
}

void Trf796x_EnableHalfPower(void)
{
    Trf796x_SetRegBit(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_HALFPWR);
}

void Trf796x_WriteCmd(u8 cmd)
{
    cmd &= TRF796X_START_CMD_MSK;
    cmd |= TRF796X_START_FLAG_CMD;
    
    Trf796x_CsLow();
    Trf796x_WriteByte(cmd);
    Trf796x_CsHigh();
}

void Trf796x_WriteReg(u8 addr, u8 reg)
{
    Trf796x_CsLow();
    Trf796x_WriteByte(addr & TRF796X_START_ADDR_MSK);
    Trf796x_WriteByte(reg);
    Trf796x_CsHigh();
}

u8 Trf796x_ReadReg(u8 addr)
{
    u8 reg = 0;
    
    addr &= TRF796X_START_ADDR_MSK;
    addr |= TRF796X_START_FLAG_READ;
    
    Trf796x_CsLow();
    Trf796x_WriteByte(addr);
    reg = Trf796x_WriteByte(TRF796X_SPI_DUMMY);
    Trf796x_CsHigh();
    
    return reg;
}

void Trf796x_ClearRegBits(u8 addr, u8 msk)
{
    u8 reg = 0;
    
    reg = Trf796x_ReadReg(addr);
    reg &= (~msk);
    Trf796x_WriteReg(addr, reg);
}

void Trf796x_SetRegBits(u8 addr, u8 msk, u8 bitValue)
{
    u8 reg = 0;
    
    reg = Trf796x_ReadReg(addr);
    reg &= (~msk);
    reg |= bitValue;
    Trf796x_WriteReg(addr, reg);
}

void Trf796x_SetRegBit(u8 addr, u8 bitValue)
{
    u8 reg = 0;
    
    reg = Trf796x_ReadReg(addr);
    reg |= bitValue;
    Trf796x_WriteReg(addr, reg);
}

void Trf796x_ReadRegs(u8 addr, u8 len, u8 *pBuffer)
{
    u8 i = 0;
    
    addr &= TRF796X_START_ADDR_MSK;
    addr |= TRF796X_START_FLAG_CONT;
    addr |= TRF796X_START_FLAG_READ;
    
    Trf796x_CsLow();
    Trf796x_WriteByte(addr);
    for(i = 0; i < len; i++)
    {
        pBuffer[i] = Trf796x_WriteByte(TRF796X_SPI_DUMMY);
    }
    Trf796x_CsHigh();
}

void Trf796x_WriteRegs(u8 addr, u8 len, u8 *pBuffer)
{
    u8 i = 0;
    
    addr &= TRF796X_START_ADDR_MSK;
    addr |= TRF796X_START_FLAG_CONT;
    
    Trf796x_CsLow();
    Trf796x_WriteByte(addr);
    for(i = 0; i < len; i++)
    {
        Trf796x_WriteByte(pBuffer[i]);
    }
    Trf796x_CsHigh();
}

void Trf796x_ClearIrqFlag(void)
{
    u8 regs[2] = {0};
    Trf796x_ReadRegs(TRF796X_REG_IRQ_STATUS, 2, regs);
}

void Trf796x_TransFifo(u8 *pFrame, u16 txByteLen, u8 lastBytBitNum, BOOL bTxCrc)
{
    u8 cmd = 0;
    u8 addr = 0;
    u8 lenHi = 0;
    u8 lenLo = 0;
    u8 i = 0;
    u8 txLen = 0;
    
    if(bTxCrc)
    {
        cmd = TRF796X_CMD_TRANSMIT_CRC | TRF796X_START_FLAG_CMD;
    }
    else
    {
        cmd = TRF796X_CMD_TRANSMIT_NO_CRC | TRF796X_START_FLAG_CMD;
    }
    addr = TRF796X_REG_FIFO_TX | TRF796X_START_FLAG_CONT;
    
    txLen = txByteLen;
    if(lastBytBitNum)
    {
        txLen--;
    }
    lenHi = (txLen >> 4) & 0xFF;
    lenLo = (txLen << 4) & 0xF0;
    if(lastBytBitNum > 0)
    {
        lenLo |= TRF796X_REG_BROKEN_FLAG;
        lenLo |= ((lastBytBitNum << 1) & 0x0E);
    }
    
    Trf796x_CsLow();
    
    Trf796x_WriteByte(cmd);
    Trf796x_WriteByte(addr);
    Trf796x_WriteByte(lenHi);       //txLen11-txLen4
    Trf796x_WriteByte(lenLo);
    for(i = 0; i < txByteLen; i++)
    {
        Trf796x_WriteByte(pFrame[i]);
    }
    
    Trf796x_CsHigh();
}

u8 Trf796x_TransceiverFrame(TRF796X_FRAME *pTrf76xFrame)
{
    u8 err = TRF796X_ERR_NORSP;
    u32 tick = 0, waitTime = 0;
    u8 flag = 0;
    u8 rxFifoLen = 0;
    
    pTrf76xFrame->rxLen = 0;
    waitTime = TRF796X_SPI_WAIT_TICK;
    
    if(pTrf76xFrame->bRxCrc)
    {
        Trf796x_ClearRegBits(TRF796X_REG_ISO_CONTROL, TRF796X_REG_ISO_RXNOCRC);
    }
    else
    {
        Trf796x_SetRegBit(TRF796X_REG_ISO_CONTROL, TRF796X_REG_ISO_RXNOCRC);
    }
    
    if(pTrf76xFrame->to > TRF796X_RX_WAIT_MAX)
    {
        Trf796x_WriteReg(TRF796X_REG_RX_NO_RESPONSE_WAIT_TIME, 0x00);
    }
    else
    {
        Trf796x_WriteReg(TRF796X_REG_RX_NO_RESPONSE_WAIT_TIME, pTrf76xFrame->to);
    }
    if(pTrf76xFrame->to > waitTime)
    {
        waitTime = pTrf76xFrame->to;
    }
    
    if(pTrf76xFrame->rxFirstBitNum == 4)        //只考虑4bit问题
    {
        Trf796x_SetRegBit(TRF796X_REG_SPECIAL_FUNCTION, TRF796X_REG_RX_4BITS);
    }
    else
    {
        Trf796x_ClearRegBits(TRF796X_REG_SPECIAL_FUNCTION, TRF796X_REG_RX_4BITS);
    }
    
    Trf796x_WriteCmd(TRF796X_CMD_STOP_DECODERS);
    Trf796x_WriteCmd(TRF796X_CMD_ENABLE_DECODERS);
    Trf796x_WriteCmd(TRF796X_CMD_RESET_FIFO);   //清空FIFO
    Trf796x_ClearIrqFlag();
    if(pTrf76xFrame->txLen)
    {
        Trf796x_TransFifo(pTrf76xFrame->frame, pTrf76xFrame->txLen, pTrf76xFrame->txLastBitNum, pTrf76xFrame->bTxCrc);
    }
    else
    {
        Trf796x_WriteCmd(TRF796X_CMD_TRANSMIT_NEXT_SLOT);
    }
    waitTime = 100000; //Trf796x_Delay37P76us(1);
    while(tick < waitTime)
    {
        if(Trf796x_GetIrqState())
        {
            flag = Trf796x_ReadReg(TRF796X_REG_IRQ_STATUS);
            if(flag & TRF796X_REG_ERR_MSK)  //错误，注意fifolv的信息不需要管，目前用到的工作模式FIFO都足够
            {
                err = Trf796x_FmtErrInfo(flag & TRF796X_REG_ERR_MSK);
                break;
            }
            else if(flag & TRF796X_REG_IRQ_TX_END)
            {
                Trf796x_WriteCmd(TRF796X_CMD_RESET_FIFO);   //清空FIFO
            }
            else if(flag & TRF796X_REG_IRQ_FIFO_LV)
            {
                err = Trf796x_FmtErrInfo(flag & TRF796X_REG_ERR_MSK);
            }
            else 
            {
                if(flag & TRF796X_REG_IRQ_RX_START) //接收完成：这个虽然是start标志，但是是等到接收完成才设置该标签未的
                {
                    err = TRF796X_ERR_OK;
                    err = Trf796x_ReadReg(TRF796X_REG_IRQ_STATUS);
                    rxFifoLen = Trf796x_ReadReg(TRF796X_REG_FIFO_CONTROL) & TRF796X_REG_FIFO_RXLEN_MSK;
                    Trf796x_ReadRegs(TRF796X_REG_FIFO, rxFifoLen, pTrf76xFrame->frame + pTrf76xFrame->rxLen);
                    pTrf76xFrame->rxLen = rxFifoLen;
                    break;
                }
            }
        }
        tick++;
    }
    //Trf796x_ReadRegs(0, 32, g_Trf796xRegs);
    return  err;
}

u8 Trf796x_FmtErrInfo(u8 irq)
{
    u8 err = TRF796X_ERR_OK;
    if(irq == 0)
    {
        err = TRF796X_ERR_OK;
    }
    else if(irq & TRF796X_REG_IRQ_NORSP)
    {
        err = TRF796X_ERR_NORSP;
    }
    else if(irq & TRF796X_REG_IRQ_CRC_ERR)
    {
        err = TRF796X_ERR_CRC;
    }
    else if(irq & (TRF796X_REG_IRQ_COLLS_ERR | TRF796X_REG_IRQ_EOF_ERR))
    {
        err = TRF796X_ERR_COLLS;
    }
    else
    {
        err = TRF796X_ERR_TAG;
    }
    
    return err;
}
