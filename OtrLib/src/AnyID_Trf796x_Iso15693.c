#include "AnyID_Trf796x_Iso15693.h"

ISO15693_FLAG g_sIso15693Flag = {FALSE, FALSE};

#define Iso15693_GetFlag(f)     do{if(g_sIso15693Flag.bHighBr) (f) |= ISO15693_REQ_FLAG_HRATE; if(g_sIso15693Flag.bDoubleCarrier) (f) |= ISO15693_REQ_FLAG_SUBCARRIER; }while(0)
void Iso15693_Init(BOOL bHighBr, BOOL bDoubleCarrier, BOOL b1Of256, BOOL bOok, BOOL bOpBlock8B, u8 pwrLv)
{
    u8 flag = 0;
    u8 ask = 0;

    if(bOpBlock8B)
    {
        g_sIso15693Flag.blockSize = ISO15693_BLOCK_SIZE_8B;
    }
    else
    {
        g_sIso15693Flag.blockSize = ISO15693_BLOCK_SIZE_4B;
    }
    
    g_sIso15693Flag.bHighBr = bHighBr;
    if(bHighBr)
    {
        flag |= TRF796X_REG_ISO15693_HIBR;
    }
    g_sIso15693Flag.bDoubleCarrier = bDoubleCarrier;
    if(bDoubleCarrier)
    {
        flag |= TRF796X_REG_ISO15693_DOUBLESUB;
    }
    if(b1Of256)
    {
        flag |= TRF796X_REG_ISO15693_1OF256;
    }
    
    if(bOok)
    {
        ask = TRF796X_REG_ASK_100;
    }
    else
    {
        ask = TRF796X_REG_ASK_10;
    }    
    
    //auto regulator---VDD-RF-5V VDDA-3.4V VDDX-3.4V
    //06-VDD_RF = 3.3 V, VDD_A = 3.3 V, VDD_X = 3.3 V
    Trf796x_WriteReg(TRF796X_REG_REGULATOR_CONTROL, 0x06);//TRF796X_REG_VOL_5_3P4_3P4 | TRF796X_REG_AUTO_REGULATOR
    //active mode--direct no-----rf off-----full power---select main rx_in1 input----bit5 control rf-----5V power
    if(pwrLv == TRF796X_PWR_LV_0)
    {
        Trf796x_WriteReg(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_HALFPWR | TRF796X_REG_CTRL_RXIN2);//
    }
    else
    {
        Trf796x_WriteReg(TRF796X_REG_CHIP_STATE_CONTROL, TRF796X_REG_CTRL_RXIN2);//
    }
    
    Trf796x_SetRegBits(TRF796X_REG_ISO_CONTROL, TRF796X_REG_ISO_MSK, flag);
    Trf796x_Delayms(1);
    Trf796x_SetRegBits(TRF796X_REG_MODULATOR_CONTROL, TRF796X_REG_ASK_MSK, ask);
    Trf796x_SetRegBits(TRF796X_REG_MODULATOR_CONTROL, TRF796X_REG_P12_ENANA, TRF796X_REG_P12_ENANA);
    //Trf796x_SetRegBits(TRF796X_REG_TEST_SETTINGS_1, 0x40, 0x40);
    
    Trf796x_Delayms(1);
    //15693 424k rxbw---gain reduction 0dBm
    Trf796x_SetRegBits(TRF796X_REG_RX_SPECIAL_SETTINGS, TRF796X_REG_RXBW_424 | TRF796X_REG_RXG_REDUMSK, TRF796X_REG_RXBW_424 | TRF796X_REG_RXG_REDU15);
    //发送完成EOF之后，24*9.44us是不需要解析SOF的，可以避免干扰信号
    Trf796x_WriteReg(TRF796X_REG_RX_WAIT_TIME, 24);         
    //SLOT 发送间隔37.77us
    Trf796x_SetRegBit(TRF796X_REG_SPECIAL_FUNCTION, TRF796X_REG_SLOT_GRID_37P77); 
    //中断
    Trf796x_WriteReg(TRF796X_REG_IRQ_MASK, TRF796X_REG_ERR_MSK);     
}


u8 Iso15693_Inventory(ISO15693_MASK *pMask, u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 err = TRF796X_ERR_OK;
    u8 flag = 0;
    u8 mskByteLen = 0;

    //mask
    if(pMask->length & 0x07)
    {
        mskByteLen = (pMask->length >> 3) + 1;
    }
    else
    {
        mskByteLen = pMask->length >> 3;
    }
    
    Iso15693_GetFlag(flag);
    flag |= ISO15693_REQ_FLAG_INVENTORY;
    flag |= ISO15693_REQ_FLAG_SLOTS_16;
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }
    
    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = flag;
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO15693_CMD_INVENTORY;        //      
    if(afiCtrl)
    {
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = afiValue;                  //如果AFI存在，将AFI值放入帧中
    }
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = pMask->length & 0xFC;
    memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pMask->value, mskByteLen);
    g_sTrf796xFrame.txLen += mskByteLen;
    
    g_sTrf796xFrame.to = ISO15693_RX_WAIT_MIN;
    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = TRUE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        if(g_sTrf796xFrame.frame[0] == ISO15693_RSP_FLAG_OK)
        {
            memcpy(pUid, g_sTrf796xFrame.frame + 2, ISO15693_SIZE_UID);    //将响应帧中的UID取出来放入UID的Buffer中
        }
        else
        {
            err = TRF796X_ERR_TAG;
        }
    }
    return err;
}

void Iso15693_AddMask(ISO15693_MSKBUF *pMaskBuf, u8 slot)
{
    if(pMaskBuf->num < ISO15693_MASK_NUM)
    {
        u8 pos = 0;
        u8 index = 0;

        index = pMaskBuf->num;

        memcpy(&pMaskBuf->mask[index], &pMaskBuf->cur, sizeof(ISO15693_MASK));

        pos = pMaskBuf->mask[index].length >> 3;
        if(pMaskBuf->mask[index].length & 0x07)
        {
            pMaskBuf->mask[index].value[pos] &= 0x0F;
            pMaskBuf->mask[index].value[pos] |= ((slot & 0x0F) << 4);
        }
        else
        {
            pMaskBuf->mask[index].value[pos] = slot & 0x0F;
        }
        pMaskBuf->mask[index].length += 4;

        pMaskBuf->num = index + 1;
    }
}

u8 Iso15693_InventoryEof(u8 *pUid)
{
    u8 err = TRF796X_ERR_OK;
    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    g_sTrf796xFrame.txLen = 0x00;                   //发 EOF
    g_sTrf796xFrame.to = ISO15693_RX_WAIT_MIN;
    
    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = FALSE;

    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        if(g_sTrf796xFrame.frame[0] == ISO15693_RSP_FLAG_OK)
        {
            memcpy(pUid, g_sTrf796xFrame.frame + 2, ISO15693_SIZE_UID);    //将响应帧中的UID取出来放入UID的Buffer中
        }
        else
        {
            err = TRF796X_ERR_TAG;
        }
    }
    return err;
}


u8 Iso15693_InventorySlot1(u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 err = TRF796X_ERR_OK;
    u8 flag = 0;
    
    Iso15693_GetFlag(flag);
    flag |= ISO15693_REQ_FLAG_INVENTORY;
    flag |= ISO15693_REQ_FLAG_SLOTS_1;
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }
    
    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = flag;
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO15693_CMD_INVENTORY;        //      
    if(afiCtrl)
    {
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = afiValue;                  //如果AFI存在，将AFI值放入帧中
    }
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = 0x00;  //maskLen
    
    g_sTrf796xFrame.to = ISO15693_RX_WAIT_MIN;
    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = TRUE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        if(g_sTrf796xFrame.frame[0] == ISO15693_RSP_FLAG_OK)
        {
            memcpy(pUid, g_sTrf796xFrame.frame + 2, ISO15693_SIZE_UID);    //将响应帧中的UID取出来放入UID的Buffer中
        }
        else
        {
            err = TRF796X_ERR_TAG;
        }
    }
    return err;
}

u8 Iso15693_Op(u8 *pUid, u8 cmd, u8 *pTxParams1, u8 *pTxParams2, u8 *pTxParams3, u8 *pRxParams, u8 *pRxParamsLen)
{
    u8 err = TRF796X_ERR_OK;
    u8 flag = 0;

    Iso15693_GetFlag(flag);
    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    if(cmd == ISO15693_CMD_WRITESINGLEBLOCK || 
       cmd == ISO15693_CMD_WRITEMULBLOCKS ||
       cmd == ISO15693_CMD_LOCKBLOCK || 
       cmd == ISO15693_CMD_WRITEAFI || 
       cmd == ISO15693_CMD_LOCKAFI || 
       cmd == ISO15693_CMD_WRITEDSFID || 
       cmd == ISO15693_CMD_LOCKDSFID ||
       cmd == ISO15693_CMD_SETEAS ||
       cmd == ISO15693_CMD_RESETEAS ||
       cmd == ISO15693_CMD_LOCKEAS)
    {
        if(pUid[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
        {
            a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
        }
        g_sTrf796xFrame.to = ISO15693_RX_WAIT_MAX;
    }
    else
    {
        g_sTrf796xFrame.to = ISO15693_RX_WAIT_MIN;
    }

    //flag cmd
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = flag;
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = cmd; //CMD
    //地址模式
    memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pUid, ISO15693_SIZE_UID);
    g_sTrf796xFrame.txLen += ISO15693_SIZE_UID;
    if(cmd == ISO15693_CMD_READSINGLEBLOCK || cmd == ISO15693_CMD_WRITEAFI || cmd == ISO15693_CMD_WRITEDSFID || cmd == ISO15693_CMD_LOCKBLOCK)
    {
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = *pTxParams1;
    }
    else if(cmd == ISO15693_CMD_WRITESINGLEBLOCK)
    {
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = *pTxParams1;
        if(ISO15693_BLOCK_SIZE_4B == g_sIso15693Flag.blockSize)
        {
            memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pTxParams2, ISO15693_SIZE_BLOCK);
            g_sTrf796xFrame.txLen += ISO15693_SIZE_BLOCK;
        }
        else
        {
            memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pTxParams2, ISO15693_SIZE_BLOCK << 1);
            g_sTrf796xFrame.txLen += (ISO15693_SIZE_BLOCK << 1);
        }
    }
    else if(cmd == ISO15693_CMD_READMULBLOCKS)
    {
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = *pTxParams1;
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = *pTxParams2 - 1;
    }
    else if(cmd == ISO15693_CMD_WRITEMULBLOCKS)
    {
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = *pTxParams1;
        g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = *pTxParams2 - 1;
        if(ISO15693_BLOCK_SIZE_4B == g_sIso15693Flag.blockSize)
        {
            memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pTxParams3, ISO15693_SIZE_BLOCK * (*pTxParams2));
            g_sTrf796xFrame.txLen += ISO15693_SIZE_BLOCK * (*pTxParams2);
        }
        else
        {
            memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pTxParams3, (ISO15693_SIZE_BLOCK << 1) * (*pTxParams2));
            g_sTrf796xFrame.txLen += (ISO15693_SIZE_BLOCK << 1) * (*pTxParams2);
        }
    }

    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = TRUE;

    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    
    if(a_CheckStateBit(flag, ISO15693_REQ_FLAG_OPT))
    {
        g_sTrf796xFrame.txLen = 0x00;                   //发 EOF
        g_sTrf796xFrame.to = ISO15693_RX_WAIT_MIN;
        
        err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    }
    if(err == TRF796X_ERR_OK)
    {
        if(g_sTrf796xFrame.frame[0] != ISO15693_RSP_FLAG_OK)
        {
            err = TRF796X_ERR_TAG;
        }
        else
        {
            if(pRxParams != NULL)
            {
                memcpy(pRxParams, g_sTrf796xFrame.frame + ISO15693_RSP_FRAME_POS_PAR, g_sTrf796xFrame.rxLen - 1);   //去掉FLAG
                if(pRxParamsLen)
                {
                    *pRxParamsLen = g_sTrf796xFrame.rxLen;
                }
            }
        }
    }

    return err;
}

u8 Iso15693_GetTagInf(u8 *pUid, ISO15693_TAGINF *pTagInfo)
{
    u8 err = TRF796X_ERR_OK;
    
    err = Iso15693_Op(pUid, ISO15693_CMD_GETSYSINFO, NULL, NULL, NULL, NULL, NULL);
    if(err == TRF796X_ERR_OK)
    {
        u8 pos = 0;
        pos = ISO15693_RSP_FRAME_POS_PAR;
        pTagInfo->flag = g_sTrf796xFrame.frame[pos++];
        pos += ISO15693_SIZE_UID;
        
        if(pTagInfo->flag & ISO15693_INFO_FLAG_DSFID)
        {
            pTagInfo->bDsfid = TRUE;
            pTagInfo->dsfid = g_sTrf796xFrame.frame[pos++];
        }

        if(pTagInfo->flag & ISO15693_INFO_FLAG_AFI)
        {
            pTagInfo->bAfi = TRUE;
            pTagInfo->afi = g_sTrf796xFrame.frame[pos++];
        }

        if(pTagInfo->flag & ISO15693_INFO_FLAG_VICC)
        {
            pTagInfo->bViccMemoryInf = TRUE;
            pTagInfo->blockNum = g_sTrf796xFrame.frame[pos++];
            pTagInfo->blockSize = g_sTrf796xFrame.frame[pos++] & 0x1F;
        }

        if(pTagInfo->flag & ISO15693_INFO_FLAG_ICREF)
        {
            pTagInfo->bICRef = TRUE;
            pTagInfo->icRef = g_sTrf796xFrame.frame[pos++];
        }
    }
    return err;
}

u8 Iso15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout)
{
    u8 err = TRF796X_ERR_OK;

    Trf796x_ClearFrame(&g_sTrf796xFrame);
    if(txLen > 0)
    {
        memcpy(g_sTrf796xFrame.frame, pTxFrame, txLen);
    }
    g_sTrf796xFrame.txLen = txLen;

    timeout /= TRF796X_RX_WAIT_UNIT;
    if(timeout < ISO15693_RX_WAIT_MIN)
    {
        timeout = ISO15693_RX_WAIT_MIN;
    }
    g_sTrf796xFrame.to = timeout;
    g_sTrf796xFrame.bTxCrc = FALSE;
    g_sTrf796xFrame.bRxCrc = FALSE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);

    return err;
}

u8 Iso15693_WriteEof(void)
{
    u8 err = TRF796X_ERR_OK;
    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    g_sTrf796xFrame.txLen = 0x00;                   //发 EOF
    g_sTrf796xFrame.to = ISO15693_RX_WAIT_MIN;
    
    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = FALSE;

    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        if(g_sTrf796xFrame.frame[0] != ISO15693_RSP_FLAG_OK)
        {
            err = TRF796X_ERR_TAG;
        }
    }
    return err;

}

