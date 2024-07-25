#include "AnyID_CR95HF.h"

CR95HF_FRAME g_sCr95HfFrame = {0};

void CR95HF_SendCommand(CR95HF_FRAME *pFrame)
{
    u8 i = 0;

    CR95HF_CS_Low();

    //send control
    CR95HF_WriteByte(CR95HF_COMM_CMD_WRITE);

    CR95HF_WriteByte(pFrame->param.cmd);
    CR95HF_WriteByte(pFrame->len);
    for(i = 0; i < pFrame->len; i++)
    {
        CR95HF_WriteByte(pFrame->frame[i]);
    }

    CR95HF_CS_High();
}

u8 CR95HF_Polling(u8 flag, u16 timeout)
{
    u8 reg = 0;
    u32 delay = 0;
    u8 rlt = CR95HF_ERR_SUCCESS;

    flag = flag & (CR95HF_POLLING_WRITE_FLAG | CR95HF_POLLING_READ_FLAG);

    timeout = timeout >> 3;
    CR95HF_CS_Low();
    CR95HF_WriteByte(CR95HF_COMM_CMD_POLLING);
    while(((reg & flag) == 0x00) && (delay < timeout))
    {
        reg = CR95HF_WriteByte(CR95HF_COMM_CMD_POLLING);
        delay++;
    }
    CR95HF_CS_High();

    if(delay >= timeout)
    {
        rlt = CR95HF_ERR_TIMEOUT;
    }

    return rlt;
}

u8 CR95HF_ReadData(CR95HF_FRAME *pFrame)
{
    u8 i = 0;
    u8 state = 0;
    
    state = CR95HF_ERR_SUCCESS;
    
    CR95HF_CS_Low();

    CR95HF_WriteByte(CR95HF_COMM_CMD_READ);
    pFrame->param.code = CR95HF_WriteByte(CR95HF_DUMMY_BYTE);
    pFrame->len = CR95HF_WriteByte(CR95HF_DUMMY_BYTE);
    if(pFrame->len > CR95HF_FRAME_MAX_LEN)
    {
        state = CR95HF_ERR_LENGTH;
    }
    else
    {
        for(i = 0; i < pFrame->len; i++)
        {
            pFrame->frame[i] = CR95HF_WriteByte(CR95HF_DUMMY_BYTE);
        }
    }
    CR95HF_CS_High();
    
    return state;
}

void CR95HF_Reset(void)
{
    //??CR95HF
    CR95HF_CS_Low();

    //send control
    CR95HF_WriteByte(CR95HF_COMM_CMD_RESET);

    CR95HF_CS_High();


    CR95HF_SelectInterface();
}

u8 CR95HF_ReadVersion(CR95HF_FRAME *pFrame)
{
    u8 state = CR95HF_ERR_SUCCESS;
    
    pFrame->param.cmd = CR95HF_CMD_IDN;
    pFrame->len = 0x00;
    state = CR95HF_SendReceive(pFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        if(pFrame->param.code != CR95HF_RSP_CODE_OK)
        {
            state = pFrame->param.code;
        }
        else if(pFrame->len != CR95HF_IDN_RSP_LEN)
        {
            state = CR95HF_ERR_LENGTH;
        }
    }
    
    return state;
}

u8 CR95HF_RfOff(void)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_IDN;
    g_sCr95HfFrame.len = 0x00;
    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEL_PROTOCOL;                 
        g_sCr95HfFrame.len = 2;                                       
        g_sCr95HfFrame.frame[0] = 0x00;          
        g_sCr95HfFrame.frame[1] = 0x00;                   
        state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);

        if(state == CR95HF_ERR_SUCCESS)
        {
            state = g_sCr95HfFrame.param.code;
        }
    }

    return state;
}


u8 CR95HF_SendReceive(CR95HF_FRAME *pFrame, u16 timeout)
{
    u8 err = 0x00;
    
    CR95HF_SendCommand(pFrame);

    err = CR95HF_Polling(CR95HF_POLLING_READ_FLAG, timeout);
    if(err == CR95HF_ERR_SUCCESS)
    {
        err = CR95HF_ReadData(pFrame);
    }

    return err;
}

u8 CR95HF_Iso14443ACheckResponse(CR95HF_FRAME *pFrame)
{
    u8 flag = 0;
    u8 state = CR95HF_ERR_SUCCESS;
    
    if(pFrame->param.code == CR95HF_SENDRCV_RSPCODE_DAT)
    {
        if(g_sCr95HfFrame.len >= CR95HF_SENDRCV_14443A_RSPTRIL_LEN)
        {
            flag = g_sCr95HfFrame.frame[g_sCr95HfFrame.len - CR95HF_SENDRCV_14443A_RSPTRIL_LEN];
            if(flag & CR95HF_SENDRCV_14443A_ERR_COL)
            {
                state = CR95HF_ERR_COLL;
            }
            else if(flag & CR95HF_SENDRCV_14443A_ERR_CRC)
            {
                state = CR95HF_ERR_CRC;
            }
            else if(flag & CR95HF_SENDRCV_14443A_ERR_PRITY)
            {
                state = CR95HF_ERR_PARITY;
            }
            else 
            {
                state = CR95HF_ERR_SUCCESS;
            }
        }
        else
        {
            state = CR95HF_ERR_LENGTH;
        }
    }
    else if(pFrame->param.code == CR95HF_SENDRCV_RSPCODE_ERR)
    {
        state = CR95HF_ERR_RSPCODE;
    }
    else 
    {
        state = pFrame->param.code;
    }
    
    return state;
}


u8 CR95HF_Iso15693CheckResponse(CR95HF_FRAME *pFrame)
{
    u8 flag = 0;
    u8 state = CR95HF_ERR_SUCCESS;
    
    if(pFrame->param.code == CR95HF_SENDRCV_RSPCODE_DAT)
    {
        //最后一个字节表示接收的数据帧是否有效
        flag = pFrame->frame[pFrame->len - 1];
        if(flag & CR95HF_SENDRCV_15693_ERR_COL)
        {
            state = CR95HF_ERR_COLL;
        }
        else if(flag & CR95HF_SENDRCV_15693_ERR_CRC)
        {
            state = CR95HF_ERR_CRC;
        }
        else
        {
            u8 vccRspFlag = 0;
            vccRspFlag = pFrame->frame[0x00];
            if((vccRspFlag & 0x01) == 0x00)
            {
                state = CR95HF_ERR_TAG;
            }
            else
            {
                state = CR95HF_ERR_TIMEOUT;
            }
        }
    }
    else if(pFrame->param.code == CR95HF_SENDRCV_RSPCODE_ERR)
    {
        state = CR95HF_ERR_RSPCODE;
    }
    else 
    {
        state = pFrame->param.code;
    }
    
    return state;
}

