#include "AnyID_CR95HF_ISO15693.h"

u8 ISO15693_Init(void)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEL_PROTOCOL;               
    g_sCr95HfFrame.len = 2;                                      
    g_sCr95HfFrame.frame[0] = CR95HF_PROTOCOL_ISO15693;         
    g_sCr95HfFrame.frame[1] = CR95HF_ISO15693_RATA_26 |
                              CR95HF_ISO15693_WAIT_SOF |
                              CR95HF_ISO15693_MODL_100 |
                              CR95HF_ISO15693_SINGLE_SUB |
                              CR95HF_ISO15693_APPEND_CRC;                                  
    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);

    if(state == CR95HF_ERR_SUCCESS)
    {
        if(g_sCr95HfFrame.param.code != CR95HF_RSP_CODE_OK)
        {
            state = g_sCr95HfFrame.param.code;
        }
    }
        
    return state;
}

u8 ISO15693_SendEof(u8 *pUid)
{
    u8 state = CR95HF_ERR_SUCCESS;

    /*g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    state = CR95HF_SendReceive(&g_sCr95HfFrame.param, &g_sCr95HfFrame);
    //成功接收数据帧
    if(state == CR95HF_ERR_SUCCESS)
    {
        //最后一个字节表示接收的数据帧是否有效
        u8 flag = g_sCr95HfFrame.frame[g_sCr95HfFrame.len - 1];
        if(flag & ISO15693_RSP_FLAG_COLLERR)
        {
            state = CR95HF_ERR_COLL;
        }
        else if(flag & ISO15693_RSP_FLAG_CRCERR)
        {
            state = CR95HF_ERR_CRC;
        }
        else
        {
            u8 vccRspFlag = g_sCr95HfFrame.frame[ISO15693_RSP_FRAME_POS_FLAG];
            if((vccRspFlag & ISO15693_RSP_FLAG_ERR) == 0x00)
            {
                a_memcpy(g_sCr95HfFrame.frame + 2, pUid, ISO15693_SIZE_UID);
                state = CR95HF_ERR_SUCCESS;
            }
            else
            {
                state = CR95HF_ERR_TIMEOUT;
            }
        }
    }
*/
    return state;
}

u8 ISO15693_Inventory(u8 afiCtrl, u8 afiValue, u8 *pUid)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_INVENTORY);   // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used
    a_SetStateBit(flag, ISO15693_REQ_FLAG_SLOTS_1);
    if(afiCtrl)
    {
        a_SetStateBit(flag, ISO15693_REQ_FLAG_AFI);   // inventory command
    }
    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_INVENTORY; //CMD
    //afi
    if(afiCtrl)
    {
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = afiValue;
    }
    //mask
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
    
    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_INVENTORY_TIME);
    //成功接收数据帧
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
        if(state == CR95HF_ERR_SUCCESS)
        {
            memcpy(pUid, g_sCr95HfFrame.frame + 2, ISO15693_SIZE_UID);
        }
    }
    
    return state;
}

u8 ISO15693_StayQuiet(u8 *pUid)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_STAYQUIET; //CMD
    //地址模式
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
    g_sCr95HfFrame.len += ISO15693_SIZE_UID;

    CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_READ_TIME);

    return state;
}

u8 ISO15693_ReadBlock(u8 *pUid, u8 num, u8 addr, u8 *pData)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;
    u8 i = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    for(i = 0; i < num; i++)
    {
        g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
        g_sCr95HfFrame.len = 0;

        //flag cmd
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_READSINGLEBLOCK; //CMD
        //地址模式
        memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
        g_sCr95HfFrame.len += ISO15693_SIZE_UID;
        //块地址
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = addr + i;

        state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_READ_TIME);
        if(state == CR95HF_ERR_SUCCESS)
        {
            state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
            if(state == CR95HF_ERR_SUCCESS)
            {
                memcpy(pData + i * ISO15693_SIZE_BLOCK, g_sCr95HfFrame.frame + ISO15693_RSP_FRAME_POS_PAR, ISO15693_SIZE_BLOCK);
            }
        }
        if(state != CR95HF_ERR_SUCCESS)
        {
            break;
        }
    }

    return state;
}
u8 ISO15693_WriteBlock(u8 *pUid, u8 num, u8 addr, u8 *pData)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;
    u8 i = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    for(i = 0; i < num; i++)
    {
        g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
        g_sCr95HfFrame.len = 0;

        //flag cmd
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_WRITESINGLEBLOCK; //CMD
        //地址模式
        memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
        g_sCr95HfFrame.len += ISO15693_SIZE_UID;
        //块地址
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = addr + i;
        //块
        memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pData + i * ISO15693_SIZE_BLOCK, ISO15693_SIZE_BLOCK);
        g_sCr95HfFrame.len += ISO15693_SIZE_BLOCK;

        state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_WRITE_TIME);
        if(state == CR95HF_ERR_SUCCESS)
        {
            state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
        }
        if(state != CR95HF_ERR_SUCCESS)
        {
            break;
        }
    }

    return state;
}


u8 ISO15693_LockBlock(u8 *pUid, u8 addr)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_LOCKBLOCK; //CMD
    //地址模式
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
    g_sCr95HfFrame.len += ISO15693_SIZE_UID;
    //块地址
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = addr;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_WRITE_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
    }

    return state;
}

u8 ISO15693_WriteAFI(u8 *pUid, u8 afi)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_WRITEAFI; //CMD
    //地址模式
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
    g_sCr95HfFrame.len += ISO15693_SIZE_UID;
    //v
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = afi;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_WRITE_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
    }

    return state;
}

u8 ISO15693_LockAFI(u8 *pUid)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_LOCKAFI; //CMD
    //地址模式
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
    g_sCr95HfFrame.len += ISO15693_SIZE_UID;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_WRITE_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
    }

    return state;
}

u8 ISO15693_WriteDSFID(u8 *pUid, u8 dsfid)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_WRITEDSFID; //CMD
    //地址模式
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
    g_sCr95HfFrame.len += ISO15693_SIZE_UID;
    //v
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = dsfid;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_WRITE_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
    }

    return state;
}

u8 ISO15693_LockDSFID(u8 *pUid)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_LOCKDSFID; //CMD
    //地址模式
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
    g_sCr95HfFrame.len += ISO15693_SIZE_UID;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_WRITE_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
    }

    return state;
}


u8 ISO15693_GetTagInf(u8 *pUid, ISO15693_TAGINF *pTagInfo)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 flag = 0;

    a_SetStateBit(flag, ISO15693_REQ_FLAG_ADDRM);       // inventory command
    a_SetStateBit(flag, ISO15693_REQ_FLAG_HRATE);       // hight data rate is used

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    //flag cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = flag;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO15693_CMD_GETSYSINFO; //CMD
    //地址模式
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUid, ISO15693_SIZE_UID);
    g_sCr95HfFrame.len += ISO15693_SIZE_UID;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_READ_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso15693CheckResponse(&g_sCr95HfFrame);
        if(state == CR95HF_ERR_SUCCESS)
        {
            u8 pos = 10;
            pTagInfo->infFlag = g_sCr95HfFrame.frame[1];
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_DSFID)
            {
                pTagInfo->dsfid = g_sCr95HfFrame.frame[pos++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_AFI)
            {
                pTagInfo->afi = g_sCr95HfFrame.frame[pos++];
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_VICC)
            {
                pTagInfo->vicc = g_sCr95HfFrame.frame[pos++];
                pTagInfo->vicc += g_sCr95HfFrame.frame[pos++] * 256;
            }
            if(pTagInfo->infFlag & ISO15693_INFO_FLAG_ICREF)
            {
                pTagInfo->icRef = g_sCr95HfFrame.frame[pos++];
            }
        }
    }

    return state;
}

u8 ISO15693_Dtu(u8 *pTxFrame, u8 txLen, u32 timeout)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;

    memcpy(g_sCr95HfFrame.frame, pTxFrame, txLen);
    g_sCr95HfFrame.len = txLen;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO15693_WRITE_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_DAT)
        {
            //最后一个字节表示接收的数据帧是否有效
            u8 flag = 0;
            
            flag = g_sCr95HfFrame.frame[g_sCr95HfFrame.len - 1];
            if(flag & ISO15693_RSP_FLAG_COLLERR)
            {
                state = CR95HF_ERR_COLL;
            }
            else if(flag & ISO15693_RSP_FLAG_CRCERR)
            {
                state = CR95HF_ERR_CRC;
            }
        }
        else if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_ERR)
        {
            state = CR95HF_ERR_RSPCODE;
        }
        else
        {
            state = g_sCr95HfFrame.param.code;
        }
    }

    return state;
}
