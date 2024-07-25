#include "AnyID_Trf796x_Iso14443a.h"

ISO14443A_4 g_sIso14443a4 = {0};

void Iso14443a_Init(void)
{
    Trf796x_SetRegBits(TRF796X_REG_ISO_CONTROL, TRF796X_REG_ISO_MSK, TRF796X_REG_ISO14443A_106K);   //106kbps
    Trf796x_SetRegBits(TRF796X_REG_MODULATOR_CONTROL, TRF796X_REG_ASK_MSK, TRF796X_REG_ASK_100);    //100%调制
    //14443a 848k rxbw---gain reduction 0dBm
    Trf796x_SetRegBits(TRF796X_REG_RX_SPECIAL_SETTINGS, TRF796X_REG_RXBW_848 | TRF796X_REG_RXG_REDUMSK, TRF796X_REG_RXBW_848 | TRF796X_REG_RXG_REDU00);
    //14443a读操作响应比较快，一般只有66us左右
    Trf796x_WriteReg(TRF796X_REG_RX_WAIT_TIME, 6);         
    //中断
    Trf796x_WriteReg(TRF796X_REG_IRQ_MASK, TRF796X_REG_ERR_MSK);     
}

u8 Is014443a_CheckBcc(u8 *pUid)
{
    u8 bcc = 0;

    bcc = pUid[0] ^ pUid[1] ^ pUid[2] ^ pUid[3] ^ pUid[4];
    if(bcc == 0)
    {
        return TRF796X_ERR_OK;
    }
    else
    {
        return TRF796X_ERR_TAG;
    }
}

u8 Iso14443a_RequestTag(u8 mode, u16 *pType)
{
    u8 err = TRF796X_ERR_OK;
    
    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = mode;
    
    g_sTrf796xFrame.txLastBitNum = 7;
    g_sTrf796xFrame.to = ISO14443A_RX_WAIT_MIN;
    
    g_sTrf796xFrame.bRxCrc = FALSE;
    g_sTrf796xFrame.bTxCrc = FALSE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        *pType = g_sTrf796xFrame.frame[1];
        *pType = *pType << 8;
        *pType = *pType + g_sTrf796xFrame.frame[0];
    }
    return err;
}

u8 Iso14443a_AnticollisionTag(u8 lv, u8 *pUid)
{
    u8 err = TRF796X_ERR_OK;

    Trf796x_ClearFrame(&g_sTrf796xFrame);

    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = lv;
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO14443A_ANTICOLLISION_NVB_START;
    
    g_sTrf796xFrame.to = ISO14443A_RX_WAIT_MIN;
    
    g_sTrf796xFrame.bRxCrc = FALSE;
    g_sTrf796xFrame.bTxCrc = FALSE;

    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        err = Is014443a_CheckBcc(g_sTrf796xFrame.frame);
        if(err == TRF796X_ERR_OK)
        {
            memcpy(pUid, g_sTrf796xFrame.frame, g_sTrf796xFrame.rxLen); //一起拷贝bcc
        }
    }

    return err;
}

u8 Iso14443a_SelectTag(u8 lv, u8 *pUid, u8 *pSak)
{
    u8 err = TRF796X_ERR_OK;
    
    Trf796x_ClearFrame(&g_sTrf796xFrame);

    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = lv;
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO14443A_ANTICOLLISION_NVB_START + ((ISO14443A_SIGNAL_UID_LEN + 1) << 4);
    memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pUid, ISO14443A_SIGNAL_UID_LEN + 1);   //UID + BCC
    g_sTrf796xFrame.txLen += ISO14443A_SIGNAL_UID_LEN + 1;

    g_sTrf796xFrame.to = ISO14443A_RX_WAIT_MIN;
    
    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = TRUE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        *pSak = g_sTrf796xFrame.frame[0];
    }

    return err;
}

u8 Iso14443a_GetUid(ISO14443A_UID *pUid, u8 mode)
{
    u8 err = TRF796X_ERR_OK;

    err = Iso14443a_RequestTag(mode, &(pUid->type));
    if(err == TRF796X_ERR_OK)
    {
        if((pUid->type != ISO14443A_TAGTYPE_TOPAZ))
        {
            u8 uid[ISO14443A_MAX_UID_LEN] = {0};
            err = Iso14443a_AnticollisionTag(ISO14443A_CMD_SELECT0, uid);
            if(err == TRF796X_ERR_OK)
            {
                err = Iso14443a_SelectTag(ISO14443A_CMD_SELECT0, uid, &(pUid->sak));
                if(err == TRF796X_ERR_OK)
                {
                    pUid->len = ISO14443A_SIGNAL_UID_LEN;
                    if(uid[0] == ISO14443A_UID_CT)
                    {
                        memcpy(pUid->uid, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);
                        err = Iso14443a_AnticollisionTag(ISO14443A_CMD_SELECT1, uid);
                        if(err == TRF796X_ERR_OK)
                        {
                            err = Iso14443a_SelectTag(ISO14443A_CMD_SELECT1, uid, &(pUid->sak));
                            if(err == TRF796X_ERR_OK)
                            {
                                pUid->len = ISO14443A_DOUBLE_UID_LEN;
                                if(uid[0] == ISO14443A_UID_CT)
                                {
                                    memcpy(pUid->uid + 3, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);

                                    err = Iso14443a_AnticollisionTag(ISO14443A_CMD_SELECT2, uid);
                                    if(err == TRF796X_ERR_OK)
                                    {
                                        err = Iso14443a_SelectTag(ISO14443A_CMD_SELECT2, uid, &(pUid->sak));
                                        if(err == TRF796X_ERR_OK)
                                        {
                                            pUid->len = ISO14443A_TRIPLE_UID_LEN;
                                            memcpy(pUid->uid + 6, uid, ISO14443A_SIGNAL_UID_LEN);
                                        }
                                    }
                                }
                                else
                                {
                                    memcpy(pUid->uid + 3, uid, ISO14443A_SIGNAL_UID_LEN);
                                }
                            }
                        }
                    }
                    else
                    {
                        memcpy(pUid->uid, uid, ISO14443A_SIGNAL_UID_LEN);
                    }
                }
            }
        }
    }


    return err;
}


u8 Iso14443a_Halt(void)
{
    u8 err = TRF796X_ERR_OK;
    
    Trf796x_ClearFrame(&g_sTrf796xFrame);

    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO14443A_CMD_HALTA;         //halt0x52
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = 0x00;                        //ctrl_0x00默认就是0x00
    
    g_sTrf796xFrame.to = ISO14443A_RX_WAIT_MIN;
    
    g_sTrf796xFrame.bRxCrc = FALSE;
    g_sTrf796xFrame.bTxCrc = TRUE;

    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    err = TRF796X_ERR_OK;               //halt没有响应
    
    return err;
}

u8 Iso14443a_ReadPage(u8 blockAddr, u8 *pPage)
{
    u8 err = TRF796X_ERR_OK;

    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO14443A_CMD_READ;         
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = blockAddr;                
    
    g_sTrf796xFrame.to = ISO14443A_RX_WAIT_MIN;
    
    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = TRUE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        if(g_sTrf796xFrame.rxLen == ISO14443A_M1BLOCK_LEN)
        {
            memcpy(pPage, g_sTrf796xFrame.frame, ISO14443A_M1BLOCK_LEN);
        }
        else
        {
            err = TRF796X_ERR_NORSP;
        }
    }
    return err;
}

u8 Iso14443a_WritePage(u8 blockAddr, u8 *pPage)
{
    u8 err = TRF796X_ERR_OK;

    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO14443A_CMD_WRITE4;         
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = blockAddr;    
    memcpy(g_sTrf796xFrame.frame + g_sTrf796xFrame.txLen, pPage, ISO14443A_M0BLOCK_LEN);
    g_sTrf796xFrame.txLen += ISO14443A_M0BLOCK_LEN;
    
    g_sTrf796xFrame.rxFirstBitNum = ISO14443A_ACK_LEN;
    
    g_sTrf796xFrame.to = ISO14443A_RX_WAIT_MAX;
    
    g_sTrf796xFrame.bRxCrc = FALSE;
    g_sTrf796xFrame.bTxCrc = TRUE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        if(((g_sTrf796xFrame.frame[0] >> 4) & 0xFF) != ISO14443A_ACK_OK)
        {
            err = TRF796X_ERR_TAG;
        }
    }

    return err;
}


const u16 fsdi[16] = {16, 24, 32, 40, 48, 64, 96, 128, 256, 257, 257, 257, 257, 257, 257, 257};
u8 Iso14443a_Rats(u8 *pAts, u8 *pLen)
{
    u8 err = TRF796X_ERR_OK;

    memset(&g_sIso14443a4, 0, sizeof(ISO14443A_4));
    
    Trf796x_ClearFrame(&g_sTrf796xFrame);

    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO14443A_CMD_RATS;
    g_sTrf796xFrame.frame[g_sTrf796xFrame.txLen++] = ISO14443A_FSDI_FSD | ISO14443A_CID;  //最多接收64字节

    g_sTrf796xFrame.to = ISO14443A_RX_WAIT_MAX;
    
    g_sTrf796xFrame.bRxCrc = TRUE;
    g_sTrf796xFrame.bTxCrc = TRUE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        u8 t = 0;

        *pLen = g_sTrf796xFrame.rxLen;
        memcpy(pAts, g_sTrf796xFrame.frame, *pLen);

        t = pAts[1];
        g_sIso14443a4.fsc = pAts[1] & 0x0F;
        g_sIso14443a4.fsc = fsdi[g_sIso14443a4.fsc];
        if(t & 0x20)                    //TB是否存在
        {
            if(t & 0x10)                //TA是否存在
            {
                g_sIso14443a4.fwt = (pAts[3] >> 4) & 0x0F;
            }
            else
            {
                g_sIso14443a4.fwt = (pAts[2] >> 4) & 0x0F;
            }
        }
        else
        {
            g_sIso14443a4.fwt = ISO14443A4_FWI_DFT;
        }
        
        g_sIso14443a4.fwt = (1 << (g_sIso14443a4.fwt + 12)) / 13560;    //单位ms
        g_sIso14443a4.fwt += 1;                                         //多加1ms
        g_sIso14443a4.fwt *= 1000;                                      //单位us
        g_sIso14443a4.fwt /= TRF796X_RX_WAIT_UNIT;                      //改成tick
    }
    return err;
}


u8 Iso14443a_TransAPDU(u8 *pApdu, u8 len, u8 *pAsk, u8 *pAskLen)
{
    u8 err = TRF796X_ERR_OK;
    u8 txLen = 0;
    u8 pcb = 0;

    Trf796x_ClearFrame(&g_sTrf796xFrame);
    
    //因为有些处理需要比较长的时间，所以PICC有时会发送SBLOCK请求延时
    //如果响应帧是SBLOCK中的WTX，那么将WTX请求直接发送到PICC，一直到接收真正的响应帧
    do{
        *pAskLen = 0;
        txLen = 0;
        
        memcpy(g_sTrf796xFrame.frame, pApdu, len);
        txLen += len;
        g_sTrf796xFrame.txLen = txLen;
        
        g_sTrf796xFrame.to = g_sIso14443a4.fwt;
        
        g_sTrf796xFrame.bRxCrc = TRUE;
        g_sTrf796xFrame.bTxCrc = TRUE;
        
        err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
        if(err == TRF796X_ERR_OK)
        {
            *pAskLen = g_sTrf796xFrame.rxLen;
            memcpy(pAsk, g_sTrf796xFrame.frame, *pAskLen);

            pcb = pAsk[0] & (~ISO14443A_IPCB_NUMMASK);
            if((pcb == ISO14443A_PCB_IBLOCK) || (pcb == ISO14443A_PCB_RBLOCK))
            {
                if((pAsk[0] & ISO14443A_IPCB_NUMMASK) == (g_sIso14443a4.pcb & ISO14443A_IPCB_NUMMASK))
                {
                    g_sIso14443a4.pcb = (~g_sIso14443a4.pcb) & ISO14443A_IPCB_NUMMASK;
                }
            }
            else if(pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX))
            {
                len = *pAskLen;
                memcpy(pApdu, pAsk, len);
            }
            else
            {
                err = TRF796X_ERR_TAG;
            }
        }
    }while((err == TRF796X_ERR_OK) && (pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX)));

    return err;
}


u8 Iso14443a_Dtu(u8 *pframe, u8 txLen, u8 txBit, u8 *pRxBit, u32 timeout)
{
    u8 err = TRF796X_ERR_OK;

    Trf796x_ClearFrame(&g_sTrf796xFrame);

    memcpy(g_sTrf796xFrame.frame, pframe, txLen);
    g_sTrf796xFrame.txLen += txLen;
    
    g_sTrf796xFrame.txLastBitNum = txBit & 0x07;
    g_sTrf796xFrame.rxFirstBitNum = *pRxBit & 0x07;
    
    timeout /= TRF796X_RX_WAIT_UNIT;
    if(timeout < ISO14443A_RX_WAIT_MIN)
    {
        timeout = ISO14443A_RX_WAIT_MIN;
    }
    g_sTrf796xFrame.to = timeout;
    g_sTrf796xFrame.bTxCrc = FALSE;
    g_sTrf796xFrame.bRxCrc = FALSE;
    
    err = Trf796x_TransceiverFrame(&g_sTrf796xFrame);
    if(err == TRF796X_ERR_OK)
    {
        //g_sTrf796xFrame.len -= ST95HF_SENDRCV_14443A_RSPTRIL_LEN;
        //*pRxBit = g_sTrf796xFrame.frame[g_sTrf796xFrame.len] & 0x0F;
        //if(*pRxBit == ST95HF_SENDRCV_14443A_FLG_LSB_BIT8)
        //{
        //    *pRxBit = 0;
        //}
    }
    return err;
}