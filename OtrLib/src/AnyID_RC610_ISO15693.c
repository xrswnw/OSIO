#include "AnyID_RC610_ISO15693.h"

BOOL g_bDoubleCarry = FALSE;
//RC610功率可配置
const u8 g_aRc610I15693High[RC610_PROTOCOL_CONFIG_SIZE] =
{
    0x8E,
    0x10,
    0x01,
    0x06,
    0x7B,
    0x7B,
    0x08,
    0x00,
    0x00,
    0x88,
    0x0F,
    0x02,
    0x44,
    0x12,
    
   // 0x06,
#ifdef _RC610_PWR_H_
    0x03, //
#else
    0x06,
#endif
    
    
};


u8 ISO15693_Init(void)
{
    u8 i = 0;
    RC610_SelectProtocol(RC610_TX_I15693_1OO4, RC610_RX_I15693_HIGH_SSC);
    
    for(i = 0; i < RC610_PROTOCOL_CONFIG_SIZE; i++)
    {
        RC610_WriteReg(g_sRc610RegAdd[i], g_aRc610I15693High[i]);
    }
    
   // RC610_WriteReg(0x47, 0x06);
    
    RC610_Config(TRUE, TRUE, TRUE, 0, 0);
    g_bDoubleCarry = FALSE;
    return TRUE;
}

u8 ISO15693_InitDoubleMod(void)
{
    u8 i = 0;
    RC610_SelectProtocol(RC610_TX_I15693_1OO4, RC610_RX_I15693_HIGH_DSC);
    for(i = 0; i < RC610_PROTOCOL_CONFIG_SIZE; i++)
    {
        RC610_WriteReg(g_sRc610RegAdd[i], g_aRc610I15693High[i]);
    }
    
    RC610_Config(TRUE, TRUE, TRUE, 0, 0);
    g_bDoubleCarry = TRUE;
    return TRUE;
}


u8 ISO15693_Inventory_Slot1(u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = RC610_STAT_OK;
    u8 num = 0;
    u8 flag = 0;
    u8 txLen = 0;
    
    
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_INVENTORY);   // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); //两个副载波
    }
    
    //a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_16);//Nb_slots 标志
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_1);
    
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }

    g_sRc610Frame.txLen = 0;

    g_sRc610Frame.frame[txLen++] = flag;
    
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    
    
    if(afiCtrl)
    {
        g_sRc610Frame.frame[txLen++] = afiValue;
    }
    

    g_sRc610Frame.frame[txLen++] = 0x00;

    g_sRc610Frame.txLen     = txLen;
    g_sRc610Frame.timeout   = ISO15693_INVENTORY_TIME;
    g_sRc610Frame.cmd       = RC610_CMD_TRANSCEIVE;

    state = RC610_Command();

    if(state == RC610_STAT_OK)
    {
        memcpy(pUid, g_sRc610Frame.frame + 2, ISO15693_SIZE_UID);
       // num++;
    }
    else if(state == RC610_STAT_COLLISION_ERROR)
    {
    
        return RC610_STAT_COLLISION_ERROR;
    }
    
    return state;
    //return num;
}



u8 ISO15693_Inventory_Slot16(ISO15693_MASK *pMask,u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = RC610_STAT_OK;
    //u8 num = 0;
    u8 flag = 0;
    u8 txLen = 0;
    u8 mskByteLen = 0;
    
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
    
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); //两个副载波
    }
    a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_16);//Nb_slots 标志

    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }
    g_sRc610Frame.txLen = 0;
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sRc610Frame.frame[txLen++] = afiValue;
    }
    g_sRc610Frame.frame[txLen++] = pMask->length & 0xFC;
    memcpy(g_sRc610Frame.frame + txLen, pMask->value, mskByteLen);   
    txLen += mskByteLen;

    g_sRc610Frame.txLen     = txLen;
    g_sRc610Frame.timeout   = ISO15693_INVENTORY_TIME;
    g_sRc610Frame.cmd       = RC610_CMD_TRANSCEIVE;

    state = RC610_Command();
    
    if(state == RC610_STAT_OK)
    {
        memcpy(pUid, g_sRc610Frame.frame + 2, ISO15693_SIZE_UID);
    }
    else if(state == RC610_STAT_COLLISION_ERROR)
    {
        return RC610_STAT_COLLISION_ERROR;
    }
    return state;
}





u8 ISO15693_Inventory(u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = RC610_STAT_OK;
    u8 num = 0;
    u8 flag = 0;
    u8 txLen = 0;
    
   

    a_SetStateBit(flag, ISO15693_REQ_FLAG_INVENTORY);   // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_1);
    
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }

    g_sRc610Frame.txLen = 0;

    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sRc610Frame.frame[txLen++] = afiValue;
    }

    g_sRc610Frame.frame[txLen++] = 0x00;

    g_sRc610Frame.txLen     = txLen;
    g_sRc610Frame.timeout   = ISO15693_INVENTORY_TIME;
    g_sRc610Frame.cmd       = RC610_CMD_TRANSCEIVE;

    state = RC610_Command();

    if(state == RC610_STAT_OK)
    {
        memcpy(pUid, g_sRc610Frame.frame + 2, ISO15693_SIZE_UID);
        num++;
    }
    else
    {
    
    }

    return num;
}

u8 ISO15693_StayQuiet(u8 *pUID)
{
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    //frame
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_STAYQUIET;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

    //该命令没有响应帧
    RC610_Command();

    return RC610_STAT_OK;
}
        
u8 ISO15693_Command(u8 *pUID, u8 cmd, u8 *pTxParams, u8 txParamsLen, u8 *pRxParams, u8 rxParamsLen, BOOL bOpt)
{
    u8 state = RC610_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    if(bOpt)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode          //TI标签的特殊模式
    }
    
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = cmd;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    if(txParamsLen > 0)
    {
        memcpy(g_sRc610Frame.frame + txLen, pTxParams, txParamsLen);
        txLen += txParamsLen;
    }
    
    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_OP_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;
    
    //该命令没有响应帧
    state = RC610_Command();
    if(a_CheckStateBit(flag, ISO15693_REQ_FLAG_OPT))
    {
        //disableSOF
        u8 reg = 0;
        reg = RC610_ReadReg(RC610_REG_FRAMECON);
        reg &= (~RC610_MASK_STARTSYM);
        RC610_WriteReg(RC610_REG_FRAMECON, reg);

        //only send symbol
        RC610_ClearRegBit(RC610_REG_TXDATANUM, RC610_BIT_DATAEN);

        txLen = 0;
        g_sRc610Frame.txLen = txLen;
        g_sRc610Frame.timeout = ISO15693_READ_TIME;
        g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

        //该命令没有响应帧
        state = RC610_Command();


        //enable  sof
        reg = RC610_ReadReg(RC610_REG_FRAMECON);
        reg &= (~RC610_MASK_STARTSYM);
        RC610_WriteReg(RC610_REG_FRAMECON, reg | RC610_VALUE_START_SYM2);

        //only send symbol
        RC610_SetRegBit(RC610_REG_TXDATANUM, RC610_BIT_DATAEN);
    }
    if(state == RC610_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag == 0)
        {
            if(g_sRc610Frame.rxLen == 1 + rxParamsLen)  //flag + params
            {
                memcpy(pRxParams, g_sRc610Frame.frame + ISO15693_RSP_FRAME_POS_PAR, rxParamsLen);
            }
            else
            {
                state = RC610_STAT_ERROR;
            }
        }
        else
        {
            state = rcvFlag;
        }
    }

    return state;
}


u8 ISO15693_ReadBlock(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 state = RC610_STAT_OK;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        state = ISO15693_Command(pUID, ISO15693_CMD_READSINGLEBLOCK, &addr, 1, pData + i * size, size, FALSE);
        if(state == RC610_STAT_OK)
        {
            addr++;
        }
        else
        {
            break;
        }
    }

    return state;
}

u8 ISO15693_WriteBlock(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 params[1 + ISO15693_SIZE_BLOCK + ISO15693_SIZE_BLOCK] = {0};
    u8 state = RC610_STAT_OK;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        params[0] = addr;
        memcpy(params + 1, pData + i * size, size);
        
        state = ISO15693_Command(pUID, ISO15693_CMD_WRITESINGLEBLOCK, params, 1 + size, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));
        if(state == RC610_STAT_OK)
        {
            addr++;
        }
        else
        {
            break;
        }
    }

    return state;
}

u8 ISO15693_ReadBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 state = RC610_STAT_OK;
    u8 params[2] = {0};

    params[0] = addr;
    params[1] = num - 1;
    state = ISO15693_Command(pUID, ISO15693_CMD_READMULBLOCKS, params, 2, pData, num * size, FALSE);

    return state;
}

u8 ISO15693_WriteBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 state = RC610_STAT_OK;
    u8 params[1 + 1 + ((ISO15693_SIZE_BLOCK + ISO15693_SIZE_BLOCK) << 2)] = {0};

    params[0] = addr;
    params[1] = num - 1;
    memcpy(params + 2, pData, size * num);
    
    state = ISO15693_Command(pUID, ISO15693_CMD_WRITEMULBLOCKS, params, 2 + size * num, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return state;
}

u8 ISO15693_WriteMemory(u8 *pUID, u8 cmd, u8 value)
{
    u8 state = RC610_STAT_ERROR;
    if(cmd == ISO15693_CMD_LOCKBLOCK)
    {
        state = ISO15693_LockBlock(pUID, value);
    }
    else if(cmd == ISO15693_CMD_WRITEAFI)
    {
        state = ISO15693_WriteAFI(pUID, value);
    }
    else if(cmd == ISO15693_CMD_WRITEDSFID)
    {
        state = ISO15693_WriteDSFID(pUID, value);
    }
    else if(cmd == ISO15693_CMD_LOCKAFI)
    {
        state = ISO15693_LockAFI(pUID);
    }
    else if(cmd == ISO15693_CMD_LOCKDSFID)
    {
        state = ISO15693_LockDSFID(pUID);
    }

    return state;
}


u8 ISO15693_LockBlock(u8 *pUID, u8 addr)
{
    u8 state = RC610_STAT_OK;;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_LOCKBLOCK;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sRc610Frame.frame[txLen++] = addr;

    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_WRITE_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = RC610_Command();
    if(state == RC610_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_WriteAFI(u8 *pUID, u8 afi)
{
    u8 state = RC610_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_WRITEAFI;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sRc610Frame.frame[txLen++] = afi;

    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_WRITE_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = RC610_Command();
    if(state == RC610_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_LockAFI(u8 *pUID)
{
    u8 state = RC610_STAT_OK;;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_LOCKAFI;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_WRITE_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = RC610_Command();
    if(state == RC610_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_WriteDSFID(u8 *pUID, u8 dsfid)
{
    u8 state = RC610_STAT_OK;;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_WRITEDSFID;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sRc610Frame.frame[txLen++] = dsfid;

    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_WRITE_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = RC610_Command();
    if(state == RC610_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_LockDSFID(u8 *pUID)
{
    u8 state = RC610_STAT_OK;;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_LOCKDSFID;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_WRITE_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = RC610_Command();
    if(state == RC610_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}


u8 ISO15693_GetTagInf(u8 *pUID, ISO15693_TAGINF *pTagInfo)
{
    u8 state = RC610_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    //frame
    g_sRc610Frame.frame[txLen++] = flag;
    g_sRc610Frame.frame[txLen++] = ISO15693_CMD_GETSYSINFO;
    memcpy(g_sRc610Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = ISO15693_READ_TIME;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = RC610_Command();
    if(state == RC610_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
        else
        {
            u8 temp = 10;
            pTagInfo->infFlag = g_sRc610Frame.frame[1];
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_DSFID)
            {
                pTagInfo->dsfid = g_sRc610Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_AFI)
            {
                pTagInfo->afi = g_sRc610Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_VICC)
            {
                pTagInfo->vicc = g_sRc610Frame.frame[temp++];
                pTagInfo->vicc += g_sRc610Frame.frame[temp++] * 256;
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_ICREF)
            {
                pTagInfo->icRef = g_sRc610Frame.frame[temp++];
            }
        }
    }

    return state;
}


u8 ISO15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout)
{
    u8 state = RC610_STAT_OK;

    if(txLen > 0)
    {
        memcpy(g_sRc610Frame.frame, pTxFrame, txLen);
    }
    g_sRc610Frame.txLen = txLen;
    g_sRc610Frame.timeout = timeout;
   
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;


    //该命令没有响应帧
    state = RC610_Command();

    return state;
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
    u8 err = RC610_STAT_OK;
    
    g_sRc610Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sRc610Frame.txLen = 0x00;
    g_sRc610Frame.cmd = RC610_CMD_TRANSCEIVE;
    
    RC610_ClearRegBit(RC610_REG_TXDATANUM,RC610_BIT_DATAEN);
    RC610_ClearRegBit(RC610_REG_FRAMECON,RC610_MASK_STARTSYM); 
    
    err = RC610_Command();//接收到數據
    
    if(err == RC610_STAT_OK)
    {
        if(g_sRc610Frame.frame[ISO15693_RSP_FRAME_POS_FLAG] == 0)
        {
            memcpy(pUid, g_sRc610Frame.frame + 2, ISO15693_SIZE_UID);    //将响应帧中的UID取出来放入UID的Buffer中 
        }
        else
        {
            err = RC610_STAT_ERROR;
        }
    }
    
    RC610_SetRegBit(RC610_REG_TXDATANUM,RC610_BIT_DATAEN);
    RC610_SetRegBit(RC610_REG_FRAMECON, RC610_MASK_STARTSYM);    
    
    return err;
}

