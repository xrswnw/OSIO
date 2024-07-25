#include "AnyID_MH663_ISO15693.h"

void ISO15693_Init(void)
{
    MH663_WriteReg(MH663_REG_COMIEN, 0x00); //不需要中断
    MH663_WriteReg(MH663_REG_DIVIEN, 0x00);

    MH663_ClearComIrqFlag();                //清空中断寄存器
    MH663_ClearDivIrqFlag();

    MH663_WriteReg(MH663_REG_MODE, 0x3F);
    MH663_WriteReg(MH663_REG_RXMODE, MH663_RXMODE_CRCEN);
	MH663_WriteReg(MH663_REG_TXMODE, MH663_TXMODE_CRCEN);
	
	MH663_WriteReg(MH663_REG_RXSEL, MH663_RXSEL_UART_INANA | (0x18));   //内部编码器编解码
	MH663_WriteReg(MH663_REG_RFCFG, 0x48);
	MH663_WriteReg(MH663_REG_MFRX, 0x05);
	
	MH663_WriteReg(MH663_REG_CONTROL, 0x00);
	
	MH663_WriteReg(MH663_REG_TXASK, MH663_TXASK_100);
	
	MH663_WriteReg(MH663_REG_GSN, 0xF8);
	MH663_WriteReg(MH663_REG_CWGSP, 0x3F);
	MH663_WriteReg(MH663_REG_MODGSP, 0x12);

    //兼容设计，MH663芯片要求
	MH663_WriteReg(0x37, 0x5e);	
	MH663_WriteReg(0x33, 0xD1);
	MH663_WriteReg(0x34, 0x7E);
	MH663_WriteReg(0x35, 0xE0);
	MH663_WriteReg(0x38, 0x9C);
	MH663_WriteReg(0x3A, 0x10);
	MH663_WriteReg(0x3B, 0x35);

	MH663_WriteReg(0x37, 0xA5);
	MH663_WriteReg(0x31, 0xB2);

	MH663_WriteReg(0x37, 0x55);
	MH663_WriteReg(0x31, 0xC1);
	
    MH663_WriteReg(0x37, 0x00);

    MH663_Config(TRUE, TRUE, TRUE, 0, 0);
}

u8 ISO15693_Inventory(u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = MH663_STAT_OK;
    u8 num = 0;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_INVENTORY);   // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_1);
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }

    g_sMH663Frame.txLen = 0;

    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sMH663Frame.frame[txLen++] = afiValue;
    }
    g_sMH663Frame.frame[txLen++] = 0x00;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    state = MH663_Command();

    if(state == MH663_STAT_OK)
    {
        memcpy(pUid, g_sMH663Frame.frame + 2, ISO15693_SIZE_UID);
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
    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_STAYQUIET;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    MH663_Command();

    return MH663_STAT_OK;
}
        
u8 ISO15693_Command(u8 *pUID, u8 cmd, u8 *pTxParams, u8 txParamsLen, u8 *pRxParams, u8 rxParamsLen, BOOL bOpt)
{
    u8 state = MH663_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(bOpt)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode          //TI标签的特殊模式
    }
    
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = cmd;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    if(txParamsLen > 0)
    {
        memcpy(g_sMH663Frame.frame + txLen, pTxParams, txParamsLen);
        txLen += txParamsLen;
    }
    
    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_OP_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;
    
    //该命令没有响应帧
    state = MH663_Command();
    if(state == MH663_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag == 0)
        {
            if(g_sMH663Frame.rxLen == 1 + rxParamsLen)  //flag + params
            {
                memcpy(pRxParams, g_sMH663Frame.frame + ISO15693_RSP_FRAME_POS_PAR, rxParamsLen);
            }
            else
            {
                state = MH663_USER_ERROR;
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
    u8 state = MH663_STAT_OK;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        state = ISO15693_Command(pUID, ISO15693_CMD_READSINGLEBLOCK, &addr, 1, pData + i * size, size, FALSE);
        if(state == MH663_STAT_OK)
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
    u8 state = MH663_STAT_OK;
    u8 i = 0;

    for(i = 0; i < num; i++)
    {
        params[0] = addr;
        memcpy(params + 1, pData + i * size, size);
        
        state = ISO15693_Command(pUID, ISO15693_CMD_WRITESINGLEBLOCK, params, 1 + size, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));
        if(state == MH663_STAT_OK)
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
    u8 state = MH663_STAT_OK;
    u8 params[2] = {0};

    params[0] = addr;
    params[1] = num - 1;
    state = ISO15693_Command(pUID, ISO15693_CMD_READMULBLOCKS, params, 2, pData, num * size, FALSE);

    return state;
}

u8 ISO15693_WriteBlocks(u8 *pUID, u8 num, u8 addr, u8 *pData, u8 size)
{
    u8 state = MH663_STAT_OK;
    u8 params[1 + 1 + ((ISO15693_SIZE_BLOCK + ISO15693_SIZE_BLOCK) << 2)] = {0};

    params[0] = addr;
    params[1] = num - 1;
    memcpy(params + 2, pData, size * num);
    
    state = ISO15693_Command(pUID, ISO15693_CMD_WRITEMULBLOCKS, params, 2 + size * num, NULL, 0, (BOOL)(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI));

    return state;
}

u8 ISO15693_WriteMemory(u8 *pUID, u8 cmd, u8 value)
{
    u8 state = MH663_USER_ERROR;
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
    u8 state = MH663_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_LOCKBLOCK;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sMH663Frame.frame[txLen++] = addr;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_WRITE_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    state = MH663_Command();
    if(state == MH663_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_WriteAFI(u8 *pUID, u8 afi)
{
    u8 state = MH663_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_WRITEAFI;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sMH663Frame.frame[txLen++] = afi;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_WRITE_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    state = MH663_Command();
    if(state == MH663_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_LockAFI(u8 *pUID)
{
    u8 state = MH663_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_LOCKAFI;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_WRITE_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    state = MH663_Command();
    if(state == MH663_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_WriteDSFID(u8 *pUID, u8 dsfid)
{
    u8 state = MH663_STAT_OK;;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_WRITEDSFID;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;
    g_sMH663Frame.frame[txLen++] = dsfid;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_WRITE_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    state = MH663_Command();
    if(state == MH663_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_LockDSFID(u8 *pUID)
{
    u8 state = MH663_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_LOCKDSFID;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_WRITE_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    state = MH663_Command();
    if(state == MH663_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
    }

    return state;
}

u8 ISO15693_OpEas(u8 *pUID, u8 cmd)
{
    u8 state = MH663_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    if(pUID[ISO15693_UID_TYPE_POS] == ISO15693_TAG_TYPE_TI)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_OPT);       // addr mode
    }

    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = cmd;
    g_sMH663Frame.frame[txLen++] = ISO15693_TAG_TYPE_NXP;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_WRITE_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    state = MH663_Command();

    return state;
}

u8 ISO15693_GetTagInf(u8 *pUID, ISO15693_TAGINF *pTagInfo)
{
    u8 state = MH663_STAT_OK;
    u8 flag = 0;
    u8 txLen = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    //frame
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_GETSYSINFO;
    memcpy(g_sMH663Frame.frame + txLen, pUID, ISO15693_SIZE_UID);
    txLen += ISO15693_SIZE_UID;

    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = ISO15693_READ_TIME;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;

    //该命令没有响应帧
    state = MH663_Command();
    if(state == MH663_STAT_OK)
    {
        u8 rcvFlag = 0;
        rcvFlag = g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG];
        if(rcvFlag != 0)
        {
            state = rcvFlag;
        }
        else
        {
            u8 temp = 10;
            pTagInfo->infFlag = g_sMH663Frame.frame[1];
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_DSFID)
            {
                pTagInfo->dsfid = g_sMH663Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_AFI)
            {
                pTagInfo->afi = g_sMH663Frame.frame[temp++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_VICC)
            {
                pTagInfo->vicc = g_sMH663Frame.frame[temp++];
                pTagInfo->vicc += g_sMH663Frame.frame[temp++] * 256;
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_ICREF)
            {
                pTagInfo->icRef = g_sMH663Frame.frame[temp++];
            }
        }
    }

    return state;
}


u8 ISO15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout)
{
    u8 state = MH663_STAT_OK;

    if(txLen > 0)
    {
        memcpy(g_sMH663Frame.frame, pTxFrame, txLen);
    }
    g_sMH663Frame.txLen = txLen;
    g_sMH663Frame.timeout = timeout;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;


    //该命令没有响应帧
    state = MH663_Command();

    return state;
}


u8 ISO15693_InventorySlot16(ISO15693_MASK *pMask,u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = MH663_STAT_OK;
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
    
    a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_16);//Nb_slots 标志

    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }
    g_sMH663Frame.txLen = 0;
    g_sMH663Frame.frame[txLen++] = flag;
    g_sMH663Frame.frame[txLen++] = ISO15693_CMD_INVENTORY;
    if(afiCtrl)
    {
        g_sMH663Frame.frame[txLen++] = afiValue;
    }
    g_sMH663Frame.frame[txLen++] = pMask->length & 0xFC;
    memcpy(g_sMH663Frame.frame + txLen, pMask->value, mskByteLen);   
    txLen += mskByteLen;

    g_sMH663Frame.txLen     = txLen;
    g_sMH663Frame.timeout   = ISO15693_INVENTORY_TIME;
    g_sMH663Frame.cmd       = MH663_PCD_TRANSCEIVE;

    state = MH663_Command();
    
    if(state == MH663_STAT_OK)
    {
        memcpy(pUid, g_sMH663Frame.frame + 2, ISO15693_SIZE_UID);
    }
    
    return state;
}

u8 ISO15693_InventoryEof(u8 *pUid)
{
    u8 err = MH663_STAT_OK;
#if 0 
    g_sMH663Frame.timeout = ISO15693_INVENTORY_TIME;
    g_sMH663Frame.txLen = 0x00;
    g_sMH663Frame.cmd = MH663_PCD_TRANSCEIVE;
    
    MH663_ClearRegBit(MH663_REG_TXDATANUM, MH663_BIT_DATAEN);
    MH663_ClearRegBit(MH663_REG_FRAMECON, MH663_MASK_STARTSYM); 
    
    err = MH663_Command();//接收到數據
    
    if(err == MH663_STAT_OK)
    {
        if(g_sMH663Frame.frame[ISO15693_RSP_FRAME_POS_FLAG] == 0)
        {
            if(pUid != NULL)
            {
                memcpy(pUid, g_sMH663Frame.frame + 2, ISO15693_SIZE_UID);    //将响应帧中的UID取出来放入UID的Buffer中 
            }
        }
        else
        {
            err = MH663_STAT_ERROR;
        }
    }
    
    MH663_SetRegBit(MH663_REG_TXDATANUM, MH663_BIT_DATAEN);
    MH663_SetRegBit(MH663_REG_FRAMECON, MH663_MASK_STARTSYM);    
#endif
    return err;
}
#if (defined(_ANYID_D320PQ_) || defined(_ANYID_M3238_) || defined(_ANYID_M3281_) || defined(_ANYID_M326X_))
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
