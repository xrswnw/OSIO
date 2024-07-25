#include "AnyID_ST25R391x_ISO15693.h"

u8 g_aIso15693Code[ST25R3916_FRAME_LEN << 3] = {0};
u8 ISO15693_Init(void)
{
    u8 err = ISO15693_ERR_SUC;
    //??д????-subCarrier stream mode-OOK-nfc ar ???
    St25r3916_WriteReg(ST25R3916_REG_MODE, ST25R3916_REG_MODE_targ_init | ST25R3916_REG_MODE_om_subcarrier_stream | ST25R3916_REG_MODE_tr_am_ook | ST25R3916_REG_MODE_nfc_ar_off);      //100 %ask
    
    //St25r3916_WriteReg(ST25R3916_REG_MODE, ST25R3916_REG_MODE_targ_init | ST25R3916_REG_MODE_om_subcarrier_stream | ST25R3916_REG_MODE_tr_am | ST25R3916_REG_MODE_nfc_ar_off);   //10 %ask
    // Disable wake up mode, if set
    St25r3916_ClearRegBits(ST25R3916_REG_EMD_SUP_CONF, ST25R3916_REG_EMD_SUP_CONF_emd_emv); //emvco?????15693????????
    //stream mode definition
    //rx-???????424K-2.6us??8??????????tx-106kbps
    St25r3916_WriteReg(ST25R3916_REG_STREAM_MODE, ST25R3916_REG_STREAM_MODE_scf_sc424 | ST25R3916_REG_STREAM_MODE_scp_8pulses | ST25R3916_REG_STREAM_MODE_stx_106); 
    //???????
    St25r3916_WriteReg(ST25R3916_REG_AUX, 0x00);
    //config rx
    //AM???-600K low pass-      //AM/PM demoudle peak detector默认配置
   /*         
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF1, 0x13);  
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF2, 0x2D);
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF3, 0x00);
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF4, 0x00);
    */ 
    
   /*  
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF1, 0x13);  
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF2, 0x2C);            //关闭acg 
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF3, 0xFC);           //+5.5db调整
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF4, 0x00);          //衰减增益
    */
    /* 
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF1, 0x53);          //低通7Mhz 
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF2, 0x2C);            //关闭acg 
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF3, 0xFC);           //+5.5db调整
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF4, 0x00);          //衰减增益
    */
    
    

    St25r3916_WriteReg(ST25R3916_REG_RX_CONF1, 0x03);          //低通2Mhz 
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF2, 0x2D);            //关闭acg 
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF3, 0x00);           //+5.5db调整
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF4, 0x00);          //衰减增益

    
    
    St25r3916_WriteReg(ST25R3916_REG_P2P_RX_CONF, 0x0C);
    St25r3916_WriteReg(ST25R3916_REG_CORR_CONF1, 0x13);
    St25r3916_WriteReg(ST25R3916_REG_CORR_CONF2, 0x01);

    St25r3916_WriteReg(ST25R3916_REG_AUX_MOD, 0x10);
    St25r3916_WriteReg(ST25R3916_REG_RES_AM_MOD, 0x80);
    //mask??????????8*4.72----????????
    St25r3916_WriteReg(ST25R3916_REG_MASK_RX_TIMER, 0x08);  
    
    //RFO???????裬0x02----4.1??---Demo ???
    St25r3916_SetRegBits(ST25R3916_REG_TX_DRIVER, ST25R3916_REG_TX_DRIVER_am_mod_mask, ST25R3916_REG_TX_DRIVER_am_mod_10percent);
    //St25r3916_SetRegBits(ST25R3916_REG_TX_DRIVER, ST25R3916_REG_TX_DRIVER_d_res_mask, ST25R3916_REG_TX_DRIVER_d_res0);
    
    return err;
}



#define PCD_ISO15693_POLYNOMIAL                      0x8408   //x^16 + x^12 + x^5 + 1
#define PCD_ISO15693_PRESET_VALUE                    0xFFFF
u16 ISO15693_CalCrc(u8 *pFrame, u16 len)
{
    u16 crc = 0;
    u16 i = 0, j = 0;
    crc = PCD_ISO15693_PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pFrame[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ PCD_ISO15693_POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);

            }
        }
    }
    crc = ~crc;
    return crc;
}

BOOL ISO15693_CheckCrc(u8 *pFrame, u8 len)
{
    u16 crc1 = 0, crc2 = 0;
    BOOL b = FALSE;
    if(len > 2)
    {
        crc1 = ISO15693_CalCrc(pFrame, len - 2);
        crc2 = pFrame[len - 2] & 0xFF;
        crc2 |= ((pFrame[len - 1] << 8) & 0xFF00);

        if(crc1 == crc2)
        {
            b = TRUE;
        }
    }

    return b;
}

#define ISO15693_CODE_SOF               0x21
#define ISO15693_CODE_EOF               0x04
const u8 g_aIso15693Code2Bits[4] = {0x02, 0x08, 0x20, 0x80};
u8 ISO15693_Code(u8 *pFrame, u8 len)
{
    u16 txCodeLen = 0;
    u8 i = 0, j = 0;
    u8 t = 0;
    
    if(len)
    {
        u16 crc = 0;
        crc = ISO15693_CalCrc(pFrame, len);
        pFrame[len++] = (crc >> 0) & 0xFF;
        pFrame[len++] = (crc >> 8) & 0xFF;
        
        g_aIso15693Code[txCodeLen++] = ISO15693_CODE_SOF;
    }
    for(i = 0; i < len; i++)
    {
        t = pFrame[i];
        for(j = 0; j < 4; j++)
        {
            g_aIso15693Code[txCodeLen++] = g_aIso15693Code2Bits[t & 0x03];
            t >>= 2;
        }
    }
    g_aIso15693Code[txCodeLen++] = ISO15693_CODE_EOF;
    
    return txCodeLen;
}

#define ISO15693_DECODE_SOF_BIT_NUM             5
#define ISO15693_DECODE_EOF_BIT_NUM             5
#define ISO15693_DECODE_SOF_EOF_MSK             0x1F
#define ISO15693_DECODE_SOF_VAL                 0x17
#define ISO15693_DECODE_EOF_VAL                 0x1D
#define ISO15693_DECODE_BYT_BITNUM              8
u8 ISO15693_Decode(ST25R3916_FRAME *pSt25r3916Frame, u8 *pPiccCode, u16 len)
{
    u8 manBits = 0;
    u16 t = 0;
    u16 bytPos = 0, bitPos = 0;
    u8 rxByt = 0, rxByteBits = 0;
    u8 err = ISO15693_ERR_TAG;
    
    pSt25r3916Frame->rxLen = 0;
    pSt25r3916Frame->collisionBitNum = 0;
    
    //??????????????????????0???е???????1
    //???????????λ??????
    //SOF---11101????????????????xxx10111
    //EOF---10111????????????????11101xxx
    //????????????????0??------10
    //????????????????1??------01
    if((pPiccCode[0] & ISO15693_DECODE_SOF_EOF_MSK) == ISO15693_DECODE_SOF_VAL)                 //SOF
    {
        bitPos = ISO15693_DECODE_BYT_BITNUM - ISO15693_DECODE_SOF_BIT_NUM;                      //???????????3bits
        t = pPiccCode[0] >> ISO15693_DECODE_SOF_BIT_NUM;                                        //???SOF
        for(bytPos = 1; bytPos < len; bytPos++)
        {
            t |= (pPiccCode[bytPos] << bitPos);         //?????2???
            bitPos += ISO15693_DECODE_BYT_BITNUM;
            while(bitPos >= 2)
            {
                manBits = t & 0x03;
                if(manBits == 0x02)
                {
                    rxByt |= 0x80;
                }
                else if(manBits == 0x01)
                {
                    rxByt |= 0x00;
                }
                else
                {
                    pSt25r3916Frame->collisionBitNum++;
                }
                t >>= 2; 
                bitPos -= 2;
                
                rxByteBits++;
                if(rxByteBits == ISO15693_DECODE_BYT_BITNUM)
                {
                    pSt25r3916Frame->frame[pSt25r3916Frame->rxLen++] = rxByt;
                    rxByt = 0;
                    rxByteBits = 0;
                }
                else
                {
                    rxByt >>= 1;
                }
            }
        }
        if(pSt25r3916Frame->rxLen < 3)       //??????3?????
        {
            err = ISO15693_ERR_TAG;
            pSt25r3916Frame->rxLen = 0;
        }
        else
        {
            if(!ISO15693_CheckCrc(pSt25r3916Frame->frame, pSt25r3916Frame->rxLen))
            {
                err = ISO15693_ERR_CRC;
                //pSt25r3916Frame->rxLen = 0;
            }
            else
            {
                err = ISO15693_ERR_SUC;
            }
        }
    }

    return err;
}

u8 ISO15693_Command(ST25R3916_FRAME *pSt25r3916Frame)
{
    u8 err = ISO15693_ERR_NORSP;
    u16 txCodeLen = 0;
    u32 irqReg = 0, irqFlag = 0;
    u32 to = 0;

    to = ISO15693_OP_TIME >> 4;       //16us???
    txCodeLen = ISO15693_Code(pSt25r3916Frame->frame, pSt25r3916Frame->txLen);

    pSt25r3916Frame->norspTime >>= 2;   //tick=4.72us
    pSt25r3916Frame->norspTime++;

    //?ж?
    //??????????????????????????????framing????norsptime????
    irqReg = ST25R3916_IRQ_MASK_NONE; 
    St25r3916_WriteRegs(ST25R3916_REG_IRQ_MASK_MAIN, (u8 *)&irqReg, ST25R3916_INT_REGS_LEN);    //???/????ж?
    St25r3916_ReadRegs(ST25R3916_REG_IRQ_MAIN, (u8 *)&irqReg, ST25R3916_INT_REGS_LEN);          //????????????????м?????ж???
    
    St25r3916_FlushFifo();                              //???fifo
    St25r3916_WriteFifo(g_aIso15693Code, txCodeLen);    //д??fifo
    
    txCodeLen--;            //???EOF????????4bit
    txCodeLen <<= 3;
    St25r3916_WriteReg(ST25R3916_REG_NUM_TX_BYTES1, (txCodeLen >> 8) & 0xFF);
    St25r3916_WriteReg(ST25R3916_REG_NUM_TX_BYTES2, ((txCodeLen >> 0) & 0xFF) | 0x04);

    St25r3916_WriteReg(ST25R3916_REG_NO_RESPONSE_TIMER1, (pSt25r3916Frame->norspTime >> 8) & 0xFF);
    St25r3916_WriteReg(ST25R3916_REG_NO_RESPONSE_TIMER2, (pSt25r3916Frame->norspTime >> 0) & 0xFF);

    St25r3916_WriteCmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC); //
    while(to > 0)
    {
        St25r3916_Delay16us(1);
        St25r3916_ReadRegs(ST25R3916_REG_IRQ_MAIN, (u8 *)&irqReg, ST25R3916_INT_REGS_LEN);
        irqFlag |= irqReg;
        if(irqFlag & ST25R3916_IRQ_MASK_TXE)                 //???????
        {
            irqFlag &= (~ST25R3916_IRQ_MASK_TXE);
            St25r3916_FlushFifo();                          //???fifo
            St25r3916_WriteCmd(ST25R3916_CMD_UNMASK_RECEIVE_DATA);  //????????????RX??????
        }
        
        if(irqFlag & ST25R3916_IRQ_MASK_RXE)                 //???????
        {
            err = ISO15693_ERR_SUC;
            break;
        }
        else if(irqFlag & ST25R3916_IRQ_MASK_NRE)            //?????
        {
            err = ISO15693_ERR_NORSP;
            break;
        }
        else if(irqFlag & ST25R3916_IRQ_MASK_COL)
        {
            err = ISO15693_ERR_TAG;
            break;
        }
        to--;
    }
    if(to == 0)         //???
    {
        err = ISO15693_ERR_NORSP;
    }
    else
    {
        if(err == ISO15693_ERR_SUC)
        {
            u16 rxCodeLen = 0;
            rxCodeLen = St25r3916_ReadReg(ST25R3916_REG_FIFO_STATUS2);
            rxCodeLen &= 0xC0;
            rxCodeLen <<= 2;
            rxCodeLen |= St25r3916_ReadReg(ST25R3916_REG_FIFO_STATUS1);
            rxCodeLen &= 0x3FF;
            
            St25r3916_ReadFifo(g_aIso15693Code, rxCodeLen);
            if(rxCodeLen)
            {
                err = ISO15693_Decode(pSt25r3916Frame, g_aIso15693Code, rxCodeLen);
            }
            else
            {
                err = ISO15693_ERR_TAG;
            }
        }
        pSt25r3916Frame->rssi = St25r3916_ReadReg(ST25R3916_REG_RSSI_RESULT);
        pSt25r3916Frame->rssi >>= 4;
    }
    
    return err;
}

u8 ISO15693_Inventory(u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 err = ISO15693_ERR_SUC;
    u8 flag = 0;
    u8 txLen = 0;

    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_INVENTORY);   // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_1);
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }

    g_sSt25r3916Frame.txLen = 0;

    g_sSt25r3916Frame.frame[txLen++] = flag;
    g_sSt25r3916Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sSt25r3916Frame.frame[txLen++] = afiValue;
    }
    g_sSt25r3916Frame.frame[txLen++] = 0x00;

    g_sSt25r3916Frame.txLen = txLen;
    g_sSt25r3916Frame.norspTime = ISO15693_INVENTORY_TIME;
    
    g_sSt25r3916Frame.collisionBitNum = 0;
    g_sSt25r3916Frame.rssi = 0;

    err = ISO15693_Command(&g_sSt25r3916Frame);

    if(err == ISO15693_ERR_SUC)
    {
        memcpy(pUid, g_sSt25r3916Frame.frame + 2, ISO15693_SIZE_UID);
    }
    else if(err != ISO15693_ERR_NORSP)
    {
        if(g_sSt25r3916Frame.collisionBitNum > 1)
        {
            err = ISO15693_ERR_COLL;
        }
    }

    return err;
}

u8 ISO15693_InventorySlot16(ISO15693_MASK *pMask, u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 err = ISO15693_ERR_SUC;
    u8 flag = 0;
    u8 txLen = 0;
    u8 mskByteLen = 0;

    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));

    
    if(pMask->length & 0x07)
    {
        mskByteLen = (pMask->length >> 3) + 1;
    }
    else
    {
        mskByteLen = pMask->length >> 3  ;
    }
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_INVENTORY);   // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }

    g_sSt25r3916Frame.txLen = 0;

    g_sSt25r3916Frame.frame[txLen++] = flag;
    g_sSt25r3916Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sSt25r3916Frame.frame[txLen++] = afiValue;
    }
    g_sSt25r3916Frame.frame[txLen++] = pMask->length & 0xFC;
    memcpy(g_sSt25r3916Frame.frame + txLen, pMask->value, mskByteLen);   
    txLen += mskByteLen;

    g_sSt25r3916Frame.txLen = txLen;
    g_sSt25r3916Frame.norspTime = ISO15693_INVENTORY_TIME;
    
    g_sSt25r3916Frame.collisionBitNum = 0;

    err = ISO15693_Command(&g_sSt25r3916Frame);

    if(err == ISO15693_ERR_SUC)
    {
        memcpy(pUid, g_sSt25r3916Frame.frame + 2, ISO15693_SIZE_UID);
    }
    else if(err != ISO15693_ERR_NORSP)
    {
        if(g_sSt25r3916Frame.collisionBitNum > 1)
        {
            err = ISO15693_ERR_COLL;
        }
    }

    return err;
}


u8 ISO15693_StayQuiet(u8 *pUID)
{
    u8 err = ISO15693_ERR_SUC;
    u8 flag = 0;
    u8 txLen = 0;

    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }
    
    g_sSt25r3916Frame.frame[txLen++] = flag;
    g_sSt25r3916Frame.frame[txLen++] = ISO15693_CMD_STAYQUIET;
    memcpy(g_sSt25r3916Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    
    g_sSt25r3916Frame.txLen = txLen;
    g_sSt25r3916Frame.norspTime = ISO15693_READ_TIME;
    
    err = ISO15693_Command(&g_sSt25r3916Frame);
    
    return ISO15693_ERR_SUC;
}


u8 ISO15693_OpCommand(u8 *pUID, u8 cmd, u8 *pTxParams, u8 txParamsLen, u8 *pRxParams, u8 rxParamsLen, BOOL bOpt)
{
    u8 err = ISO15693_ERR_SUC;
    u8 flag = 0;
    u8 txLen = 0;

    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(bOpt)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode          //TI???????????
    }
    
    g_sSt25r3916Frame.frame[txLen++] = flag;
    g_sSt25r3916Frame.frame[txLen++] = cmd;
    memcpy(g_sSt25r3916Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    if(txParamsLen > 0)
    {
        memcpy(g_sSt25r3916Frame.frame + txLen, pTxParams, txParamsLen);
        txLen += txParamsLen;
    }
    
    g_sSt25r3916Frame.txLen = txLen;
    g_sSt25r3916Frame.norspTime = ISO15693_OP_TIME;
    
    //?????????????
    err = ISO15693_Command(&g_sSt25r3916Frame);
    if(a_CheckStateBit(flag, ISO15693_REQ_FLAG_OPT))
    {
        txLen = 0;

        memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));
        g_sSt25r3916Frame.norspTime = ISO15693_READ_TIME;

        //?????????????
        err = ISO15693_Command(&g_sSt25r3916Frame);
    }
    if(err == ISO15693_ERR_SUC)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sSt25r3916Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag == 0)
        {
            if(g_sSt25r3916Frame.rxLen == 1 + rxParamsLen + 2)  //flag + params, 2?????CRC
            {
                if(rxParamsLen > 0 && (pRxParams != NULL))
                {
                    memcpy(pRxParams, g_sSt25r3916Frame.frame + ISO15693_RSP_FRAME_POS_PAR, rxParamsLen);
                }
            }
            else
            {
                err = ISO15693_ERR_TAG;
            }
        }
        else
        {
            err = rcvFlag;
        }
    }

    return err;
}


u8 ISO15693_ReadBlock(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 err = ISO15693_ERR_SUC;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        err = ISO15693_OpCommand(pUID, ISO15693_CMD_READSINGLEBLOCK, &addr, 1, pData + i * size, size, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));
        if(err == ISO15693_ERR_SUC)
        {
            addr++;
        }
        else
        {
            break;
        }
    }

    return err;
}


u8 ISO15693_WriteBlock(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 params[1 + ISO15693_SIZE_BLOCK + ISO15693_SIZE_BLOCK] = {0};
    u8 err = ISO15693_ERR_SUC;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        params[0] = addr;
        memcpy(params + 1, pData + i * size, size);
        
        err = ISO15693_OpCommand(pUID, ISO15693_CMD_WRITESINGLEBLOCK, params, 1 + size, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));
        if(err == ISO15693_ERR_SUC)
        {
            addr++;
        }
        else
        {
            break;
        }
    }

    return err;
}

u8 ISO15693_ReadBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 err = ISO15693_ERR_SUC;
    u8 params[2] = {0};

    params[0] = addr;
    params[1] = num - 1;
    err = ISO15693_OpCommand(pUID, ISO15693_CMD_READMULBLOCKS, params, 2, pData, num * size, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return err;
}

u8 ISO15693_WriteBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 err = ISO15693_ERR_SUC;
    u8 params[1 + 1 + ((ISO15693_SIZE_BLOCK + ISO15693_SIZE_BLOCK) << 2)] = {0};

    params[0] = addr;
    params[1] = num - 1;
    memcpy(params + 2, pData, size * num);
    
    err = ISO15693_OpCommand(pUID, ISO15693_CMD_WRITEMULBLOCKS, params, 2 + size * num, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return err;
}

u8 ISO15693_WriteMemory(u8 *pUID, u8 cmd, u8 value)
{
    u8 err = ISO15693_ERR_SUC;
    if(cmd == ISO15693_CMD_LOCKBLOCK)
    {
        err = ISO15693_LockBlock(pUID, value);
    }
    else if(cmd == ISO15693_CMD_WRITEAFI)
    {
        err = ISO15693_WriteAFI(pUID, value);
    }
    else if(cmd == ISO15693_CMD_WRITEDSFID)
    {
        err = ISO15693_WriteDSFID(pUID, value);
    }
    else if(cmd == ISO15693_CMD_LOCKAFI)
    {
        err = ISO15693_LockAFI(pUID);
    }
    else if(cmd == ISO15693_CMD_LOCKDSFID)
    {
        err = ISO15693_LockDSFID(pUID);
    }

    return err;
}


u8 ISO15693_WriteAFI(u8 *pUID, u8 afi)
{
    u8 err = ISO15693_ERR_SUC;

    err = ISO15693_OpCommand(pUID, ISO15693_CMD_WRITEAFI, &afi, 1, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return err;
}

u8 ISO15693_LockAFI(u8 *pUID)
{
    u8 err = ISO15693_ERR_SUC;

    err = ISO15693_OpCommand(pUID, ISO15693_CMD_LOCKAFI, NULL, 0, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return err;
}

u8 ISO15693_WriteDSFID(u8 *pUID, u8 dsfid)
{
    u8 err = ISO15693_ERR_SUC;

    err = ISO15693_OpCommand(pUID, ISO15693_CMD_WRITEDSFID, &dsfid, 1, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return err;
}

u8 ISO15693_LockDSFID(u8 *pUID)
{
    u8 err = ISO15693_ERR_SUC;

    err = ISO15693_OpCommand(pUID, ISO15693_CMD_LOCKDSFID, NULL, 0, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return err;
}

u8 ISO15693_LockBlock(u8 *pUID, u8 blockAddr)
{
    u8 err = ISO15693_ERR_SUC;

    err = ISO15693_OpCommand(pUID, ISO15693_CMD_LOCKBLOCK, &blockAddr, 1, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return err;
}

u8 ISO15693_OpEas(u8 *pUID, u8 cmd)
{
    u8 err = ISO15693_ERR_SUC;
    u8 flag = 0;
    u8 txLen = 0;
    
    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sSt25r3916Frame.frame[txLen++] = flag;
    g_sSt25r3916Frame.frame[txLen++] = cmd;
    g_sSt25r3916Frame.frame[txLen++] = pUID[ISO15693_UID_TYPE_POS];
    memcpy(g_sSt25r3916Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sSt25r3916Frame.txLen = txLen;
    g_sSt25r3916Frame.norspTime = ISO15693_WRITE_TIME;
    
    err = ISO15693_Command(&g_sSt25r3916Frame);
    
    return err;
}



u8 ISO15693_GetTagInf(u8 *pUID, ISO15693_TAGINF *pTagInfo)
{
    u8 err = ISO15693_ERR_SUC;
    u8 flag = 0;
    u8 txLen = 0;

    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sSt25r3916Frame.frame[txLen++] = flag;
    g_sSt25r3916Frame.frame[txLen++] = ISO15693_CMD_GETSYSINFO;
    memcpy(g_sSt25r3916Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sSt25r3916Frame.txLen = txLen;
    g_sSt25r3916Frame.norspTime = ISO15693_READ_TIME;
    
    err = ISO15693_Command(&g_sSt25r3916Frame);

    if(err == ISO15693_ERR_SUC)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sSt25r3916Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            err = rcvFlag;
        }
        else
        {
            u8 temp = 10;
            pTagInfo->infFlag = g_sSt25r3916Frame.frame[1];
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_DSFID)
            {
                pTagInfo->dsfid = g_sSt25r3916Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_AFI)
            {
                pTagInfo->afi = g_sSt25r3916Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_VICC)
            {
                pTagInfo->vicc = g_sSt25r3916Frame.frame[temp++];
                pTagInfo->vicc += g_sSt25r3916Frame.frame[temp++] * 256;
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_ICREF)
            {
                pTagInfo->icRef = g_sSt25r3916Frame.frame[temp++];
            }
        }
    }

    return err;
}



u8 ISO15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout)
{
    u8 err = ISO15693_ERR_SUC;

    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));
    
    memcpy(g_sSt25r3916Frame.frame, pTxFrame, txLen);
    g_sSt25r3916Frame.txLen = txLen;
    g_sSt25r3916Frame.norspTime = timeout;
    
    err = ISO15693_Command(&g_sSt25r3916Frame);

    return err;
}

void ISO15693_AddMask(ISO15693_MSKBUF *pMaskBuf, u8 slot)
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

u8 ISO15693_InventoryEof(u8 *pUid)
{
    u8 err = ISO15693_ERR_SUC;

    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));
    
    g_sSt25r3916Frame.norspTime = ISO15693_INVENTORY_TIME;
    
    g_sSt25r3916Frame.collisionBitNum = 0;

    err = ISO15693_Command(&g_sSt25r3916Frame);

    if(err == ISO15693_ERR_SUC)
    {
        memcpy(pUid, g_sSt25r3916Frame.frame + 2, ISO15693_SIZE_UID);
    }
    else if(err != ISO15693_ERR_NORSP)
    {
        if(g_sSt25r3916Frame.collisionBitNum > 1)
        {
            err = ISO15693_ERR_COLL;
        }
    }

    return err;
}


