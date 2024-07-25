#include "AnyID_FM17660_ISO15693.h"

BOOL g_bDoubleCarry = FALSE;

const u8 g_aFM17660I15693High[FM17660_PROTOCOL_CONFIG_SIZE] =
{
    0xC0,
    0x01,
    0x06,
    0x0F,
    0x05,
    0x09,
    0xFF,
    0x80
};


u8 ISO15693_Init(void)
{
    u8 i = 0;
    FM17660_SelectProtocol(FM17660_TX_I15693_1OO4, FM17660_RX_I15693_HIGH_SSC);
    for(i = 0; i < FM17660_PROTOCOL_CONFIG_SIZE; i++)
    {
        FM17660_WriteReg(g_sFM17660RegAdd[i], g_aFM17660I15693High[i]);
    }
    
    FM17660_Config(TRUE, TRUE, TRUE, 0, 0);
    //自动滤除小于4bit的噪声帧
    FM17660_SetRegBit(FM17660_REG_RXSUPCFG, FM17660_BIT_RXNOERR);
    g_bDoubleCarry = FALSE;
    return TRUE;
}

u8 ISO15693_InitDoubleMod(void)
{
    u8 i = 0;
    FM17660_SelectProtocol(FM17660_TX_I15693_1OO4, FM17660_RX_I15693_HIGH_DSC);
    for(i = 0; i < FM17660_PROTOCOL_CONFIG_SIZE; i++)
    {
        FM17660_WriteReg(g_sFM17660RegAdd[i], g_aFM17660I15693High[i]);
    }
    
    FM17660_Config(TRUE, TRUE, TRUE, 0, 0);
    //自动滤除小于4bit的噪声帧
    FM17660_SetRegBit(FM17660_REG_RXSUPCFG, FM17660_BIT_RXNOERR);
    g_bDoubleCarry = TRUE;
    return TRUE;
}

u8 ISO15693_InitCompaRc610(void)
{
    u8 i = 0;
    FM17660_SelectProtocol(0x00, 0x00);
    for(i = 0; i < FM17660_PROTOCOL_CONFIG_SIZE; i++)
    {
        FM17660_WriteReg(g_sFM17660RegAdd[i], g_aFM17660I15693High[i]);
    }
    
    FM17660_Config(TRUE, TRUE, TRUE, 0, 0);
    //自动滤除小于4bit的噪声帧
    FM17660_SetRegBit(FM17660_REG_RXSUPCFG, FM17660_BIT_RXNOERR);
    g_bDoubleCarry = FALSE;
    return TRUE;
}

u8 ISO15693_Inventory(u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = FM17660_STAT_OK;
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

    g_sFm17660Frame.txLen = 0;

    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sFm17660Frame.frame[txLen++] = afiValue;
    }
    g_sFm17660Frame.frame[txLen++] = 0x00;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    state = FM17660_Command();

    if(state == FM17660_STAT_OK)
    {
        memcpy(pUid, g_sFm17660Frame.frame + 2, ISO15693_SIZE_UID);
        num++;
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
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_STAYQUIET;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    FM17660_Command();

    return FM17660_STAT_OK;
}
        
u8 ISO15693_Command(u8 *pUID, u8 cmd, u8 *pTxParams, u8 txParamsLen, u8 *pRxParams, u8 rxParamsLen, BOOL bOpt)
{
    u8 state = FM17660_STAT_OK;
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
    
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = cmd;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    if(txParamsLen > 0)
    {
        memcpy(g_sFm17660Frame.frame + txLen, pTxParams, txParamsLen);
        txLen += txParamsLen;
    }
    
    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_OP_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;
    
    //该命令没有响应帧
    state = FM17660_Command();
    if(a_CheckStateBit(flag, ISO15693_REQ_FLAG_OPT))
    {
        //disableSOF
        u8 reg = 0;
        reg = FM17660_ReadReg(FM17660_REG_FRAMECON);
        reg &= (~FM17660_MASK_STARTSYM);
        FM17660_WriteReg(FM17660_REG_FRAMECON, reg);

        //only send symbol
        FM17660_ClearRegBit(FM17660_REG_TXDATANUM, FM17660_BIT_DATAEN);

        txLen = 0;
        g_sFm17660Frame.txLen = txLen;
        g_sFm17660Frame.timeout = ISO15693_READ_TIME;
        g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

        //该命令没有响应帧
        state = FM17660_Command();

        //enablesof
        reg = FM17660_ReadReg(FM17660_REG_FRAMECON);
        reg &= (~FM17660_MASK_STARTSYM);
        FM17660_WriteReg(FM17660_REG_FRAMECON, reg | FM17660_VALUE_START_SYM2);

        //only send symbol
        FM17660_SetRegBit(FM17660_REG_TXDATANUM, FM17660_BIT_DATAEN);
    }
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag == 0)
        {
            if(g_sFm17660Frame.rxLen == 1 + rxParamsLen)  //flag + params
            {
                memcpy(pRxParams, g_sFm17660Frame.frame + ISO15693_RSP_FRAME_POS_PAR, rxParamsLen);
            }
            else
            {
                state = FM17660_STAT_ERROR;
            }
        }
        else
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_CmdSelected(u8 cmd, u8 *pTxParams, u8 txParamsLen, u8 *pRxParams, u8 rxParamsLen, BOOL bOpt)
{
    u8 state = FM17660_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);           // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    if(bOpt)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);         // addr mode          //TI标签的特殊模式
    }
    
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = cmd;
    if(txParamsLen > 0)
    {
        memcpy(g_sFm17660Frame.frame + txLen, pTxParams, txParamsLen);
        txLen += txParamsLen;
    }
    
    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_OP_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;
    
    //该命令没有响应帧
    state = FM17660_Command();
    if(a_CheckStateBit(flag, ISO15693_REQ_FLAG_OPT))
    {
        //disableSOF
        u8 reg = 0;
        reg = FM17660_ReadReg(FM17660_REG_FRAMECON);
        reg &= (~FM17660_MASK_STARTSYM);
        FM17660_WriteReg(FM17660_REG_FRAMECON, reg);

        //only send symbol
        FM17660_ClearRegBit(FM17660_REG_TXDATANUM, FM17660_BIT_DATAEN);

        txLen = 0;
        g_sFm17660Frame.txLen = txLen;
        g_sFm17660Frame.timeout = ISO15693_READ_TIME;
        g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

        //该命令没有响应帧
        state = FM17660_Command();

        //enablesof
        reg = FM17660_ReadReg(FM17660_REG_FRAMECON);
        reg &= (~FM17660_MASK_STARTSYM);
        FM17660_WriteReg(FM17660_REG_FRAMECON, reg | FM17660_VALUE_START_SYM2);

        //only send symbol
        FM17660_SetRegBit(FM17660_REG_TXDATANUM, FM17660_BIT_DATAEN);
    }
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag == 0)
        {
            if(g_sFm17660Frame.rxLen == 1 + rxParamsLen)  //flag + params
            {
                memcpy(pRxParams, g_sFm17660Frame.frame + ISO15693_RSP_FRAME_POS_PAR, rxParamsLen);
            }
            else
            {
                state = FM17660_STAT_ERROR;
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
    u8 state = FM17660_STAT_OK;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        state = ISO15693_Command(pUID, ISO15693_CMD_READSINGLEBLOCK, &addr, 1, pData + i * size, size, FALSE);
        if(state == FM17660_STAT_OK)
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
    u8 state = FM17660_STAT_OK;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        params[0] = addr;
        memcpy(params + 1, pData + i * size, size);
        
        state = ISO15693_Command(pUID, ISO15693_CMD_WRITESINGLEBLOCK, params, 1 + size, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));
        if(state == FM17660_STAT_OK)
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
    u8 state = FM17660_STAT_OK;
    u8 params[2] = {0};

    params[0] = addr;
    params[1] = num - 1;
    state = ISO15693_Command(pUID, ISO15693_CMD_READMULBLOCKS, params, 2, pData, num * size, FALSE);

    return state;
}

u8 ISO15693_WriteBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 state = FM17660_STAT_OK;
    u8 params[1 + 1 + ((ISO15693_SIZE_BLOCK + ISO15693_SIZE_BLOCK) << 2)] = {0};

    params[0] = addr;
    params[1] = num - 1;
    memcpy(params + 2, pData, size * num);
    
    state = ISO15693_Command(pUID, ISO15693_CMD_WRITEMULBLOCKS, params, 2 + size * num, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return state;
}

u8 ISO15693_WriteMemory(u8 *pUID, u8 cmd, u8 value)
{
    u8 state = FM17660_STAT_ERROR;
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
    u8 state = FM17660_STAT_OK;
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
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_LOCKBLOCK;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sFm17660Frame.frame[txLen++] = addr;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_WRITE_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_WriteAFI(u8 *pUID, u8 afi)
{
    u8 state = FM17660_STAT_OK;
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
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_WRITEAFI;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sFm17660Frame.frame[txLen++] = afi;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_WRITE_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_LockAFI(u8 *pUID)
{
    u8 state = FM17660_STAT_OK;
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
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_LOCKAFI;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_WRITE_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_WriteDSFID(u8 *pUID, u8 dsfid)
{
    u8 state = FM17660_STAT_OK;;
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
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_WRITEDSFID;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sFm17660Frame.frame[txLen++] = dsfid;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_WRITE_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_LockDSFID(u8 *pUID)
{
    u8 state = FM17660_STAT_OK;
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
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_LOCKDSFID;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_WRITE_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_OpEas(u8 *pUID, u8 cmd)
{
    u8 state = FM17660_STAT_OK;
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
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = cmd;
    g_sFm17660Frame.frame[txLen++] = ISO15693_TAG_TYPE_NXP;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_WRITE_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();

    return state;
}

u8 ISO15693_GetTagInf(u8 *pUID, ISO15693_TAGINF *pTagInfo)
{
    u8 state = FM17660_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(g_bDoubleCarry)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_DUBCARRIER); 
    }
    //frame
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_GETSYSINFO;
    memcpy(g_sFm17660Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = ISO15693_READ_TIME;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();
    if(state == FM17660_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
        else
        {
            u8 temp = 10;
            pTagInfo->infFlag = g_sFm17660Frame.frame[1];
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_DSFID)
            {
                pTagInfo->dsfid = g_sFm17660Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_AFI)
            {
                pTagInfo->afi = g_sFm17660Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_VICC)
            {
                pTagInfo->vicc = g_sFm17660Frame.frame[temp++];
                pTagInfo->vicc += g_sFm17660Frame.frame[temp++] * 256;
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_ICREF)
            {
                pTagInfo->icRef = g_sFm17660Frame.frame[temp++];
            }
        }
    }

    return state;
}


u8 ISO15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout)
{
    u8 state = FM17660_STAT_OK;

    if(txLen > 0)
    {
        memcpy(g_sFm17660Frame.frame, pTxFrame, txLen);
    }
    g_sFm17660Frame.txLen = txLen;
    g_sFm17660Frame.timeout = timeout;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;

    //该命令没有响应帧
    state = FM17660_Command();

    return state;
}

u8 ISO15693_InventorySlot16(ISO15693_MASK *pMask, u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = FM17660_STAT_OK;
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
    g_sFm17660Frame.txLen = 0;
    g_sFm17660Frame.frame[txLen++] = flag;
    g_sFm17660Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sFm17660Frame.frame[txLen++] = afiValue;
    }
    g_sFm17660Frame.frame[txLen++] = pMask->length & 0xFC;
    memcpy(g_sFm17660Frame.frame + txLen, pMask->value, mskByteLen);   
    txLen += mskByteLen;

    g_sFm17660Frame.txLen     = txLen;
    g_sFm17660Frame.timeout   = ISO15693_INVENTORY_TIME;
    g_sFm17660Frame.cmd       = FM17660_CMD_TRANSCEIVE;

    state = FM17660_Command();
    
    if(state == FM17660_STAT_OK)
    {
        memcpy(pUid, g_sFm17660Frame.frame + 2, ISO15693_SIZE_UID);
    }
    
    return state;
}

u8 ISO15693_InventoryEof(u8 *pUid)
{
    u8 err = FM17660_STAT_OK;
    
    g_sFm17660Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sFm17660Frame.txLen = 0x00;
    g_sFm17660Frame.cmd = FM17660_CMD_TRANSCEIVE;
    
    FM17660_ClearRegBit(FM17660_REG_TXDATANUM, FM17660_BIT_DATAEN);
    FM17660_ClearRegBit(FM17660_REG_FRAMECON, FM17660_MASK_STARTSYM); 
    
    err = FM17660_Command();//接收到數據
    
    if(err == FM17660_STAT_OK)
    {
        if(g_sFm17660Frame.frame[ISO15693_RSP_FRAME_POS_FLAG] == 0)
        {
            if(pUid != NULL)
            {
                memcpy(pUid, g_sFm17660Frame.frame + 2, ISO15693_SIZE_UID);    //将响应帧中的UID取出来放入UID的Buffer中 
            }
        }
        else
        {
            err = FM17660_STAT_ERROR;
        }
    }
    
    FM17660_SetRegBit(FM17660_REG_TXDATANUM, FM17660_BIT_DATAEN);
    FM17660_SetRegBit(FM17660_REG_FRAMECON, FM17660_MASK_STARTSYM);    
    
    return err;
}

#if (defined(_ANYID_D320PQ_) || defined(_ANYID_M3238_) || defined(_ANYID_M3281_) || defined(_ANYID_M326X_) || defined(_ANYID_M3237_) || defined(_ANYID_FR321_))
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
#endif