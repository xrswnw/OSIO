#include "AnyID_CR95HF_ISO14443A.h"

ISO14443A_UID g_sISO14443AUid = {0};
ISO14443A_AUTHFRAME g_sISO14443AAuthFrame = {0};
u8 ISO14443A_Init(void)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEL_PROTOCOL;                                     //选择协议命令
    g_sCr95HfFrame.len = 4;                                                                 //选择协议命令长度
    g_sCr95HfFrame.frame[0] = CR95HF_PROTOCOL_ISO14443A;                                    //14443A
    g_sCr95HfFrame.frame[1] = CR95HF_ISO14443A_TXRATE_106 | CR95HF_ISO14443A_RXRATE_106;    //设置发送和接收的波特率
    //fwt = (256 * 16 / fc) * (2 ^ 3) * (3 + 1) = 9666us
    g_sCr95HfFrame.frame[2] = 0x03;                                                         //pp
    g_sCr95HfFrame.frame[3] = 0x03;                                                         //mm

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

#define ISO14443A_PRESET_VALUE      0x6363
#define ISO14443A_POLYNOMIAL        0x8408   //x^16 + x^12 + x^5 + 1
u16 ISO14443A_CalCrc(u8 *pFrame, u8 len)
{
    u16 crc = 0;     
    u8 i = 0, j = 0;
    u8 temp = 0;
    crc = ISO14443A_PRESET_VALUE;    
    for(i = 0; i < len; i++)
    {
        temp = pFrame[i] & 0xFF;
        crc = crc ^ temp;
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ ISO14443A_POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);
            }    
        }        
    }    
    return crc;
}	


u16 ISO14443A_GetOdd(u8 dat)
{
    u8 odd = 0;
    u16 temp = 0;
    u8 i = 0;

    temp = dat;
    for(i = 0; i < 8; i++)
    {
        odd ^= (temp & 0x01);
        temp >>= 1;
    }
    temp = dat;
    temp |= (((~odd) & 0x01) << 8);
    return temp;
}

BOOL ISO14443A_CheckParity(u16 *pFrame, u8 len)
{
    BOOL b = TRUE;
    u8 i = 0, j = 0;
    u8 odd = 0;
    u16 dat = 0;
    for(i = 0; i < len; i++)
    {
        odd = 0;
        dat = pFrame[i] & 0x1FF;
        for(j = 0; j < 9; j++)
        {
            odd ^= (dat & 0x01);
            dat >>= 1;
        }
        if((odd & 0x01) == 0x00)
        {
            b = FALSE;
            break;
        }
    }
    return b;
}


u8 ISO14443A_RequestTag(u8 mode, u16 *pTagType)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = mode;        //mode(0x26/0x52)
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_FRAME_LSB_BIT7;    //知否送7bits

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_INVENTORY_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso14443ACheckResponse(&g_sCr95HfFrame);
        if(state == CR95HF_ERR_SUCCESS)
        {
            *pTagType = g_sCr95HfFrame.frame[1];
            *pTagType = *pTagType << 8;
            *pTagType = *pTagType + g_sCr95HfFrame.frame[0];
        }
    }

    return state;
}


u8 ISO14443A_AnticollisionTag(u8 cmd, u8 *pUID)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = cmd;                          
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_ANTICOLLISION_NVB_START;   
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_FRAME_LSB_BIT8;   

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_INVENTORY_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso14443ACheckResponse(&g_sCr95HfFrame);
        if(state == CR95HF_ERR_SUCCESS)
        {
            state = ISO14443A_CheckBcc(g_sCr95HfFrame.frame);
            if(state == CR95HF_ERR_SUCCESS)
            {
                memcpy(pUID, g_sCr95HfFrame.frame, ISO14443A_SIGNAL_UID_LEN + 1); //一起拷贝bcc
            }
        }
    }

    return state;
}



u8 ISO14443A_CheckACK(u8 ack)
{
    u8 state = CR95HF_ERR_SUCCESS;

    ack = ack & ISO14443A_ACK_MASK;

    switch(ack)
    {
        case ISO14443A_ACK_OK:                     //应答:成功
            state = CR95HF_ERR_SUCCESS;
            break;
        case ISO14443A_ACK_NOT_ALLOW:              //应答:不允许操作(授权验证错误)
            state = CR95HF_ERR_AUTH;
            break;
        case ISO14443A_ACK_TRS_ERROR:              //应答:传输错误
            state = CR95HF_ERR_TRANS;
            break;
        default:
            state = CR95HF_ERR_OTHER;          //应答:其他错误
            break;
    }

    return state;
}

u8 ISO14443A_CheckBcc(u8 *pUID)
{
    u8 bcc = 0;

    bcc = pUID[0] ^ pUID[1] ^ pUID[2] ^ pUID[3] ^ pUID[4];
    if(bcc == 0)
    {
        return CR95HF_ERR_SUCCESS;
    }
    else
    {
        return CR95HF_ERR_BCC;
    }
}



u8 ISO14443A_SelectTag(u8 lv, u8 *pUID, u8 *pSak)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = lv;                          
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_ANTICOLLISION_NVB_START + ((ISO14443A_SIGNAL_UID_LEN + 1) << 4);   
    memcpy(g_sCr95HfFrame.frame + g_sCr95HfFrame.len, pUID, ISO14443A_SIGNAL_UID_LEN + 1);   //UID + BCC
    g_sCr95HfFrame.len += ISO14443A_SIGNAL_UID_LEN + 1;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;   

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_INVENTORY_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso14443ACheckResponse(&g_sCr95HfFrame);
        if(state == CR95HF_ERR_SUCCESS)
        {
            *pSak = g_sCr95HfFrame.frame[0];
        }
    }

    return state;
}


u8 ISO14443A_GetUid(ISO14443A_UID *pUid, u8 mode)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 len = 0;
    u8 cmd = 0;
    u8 sak = 0;
    u8 uid[ISO14443A_MAX_UID_LEN] = {0};
    u8 i = 0;

    memset(&g_sISO14443AMifareAuth, 0, sizeof(g_sISO14443AMifareAuth));   //清空mifare1的auth参数
    state = ISO14443A_RequestTag(mode, &(pUid->type));                      //请求
    if(state == CR95HF_ERR_SUCCESS && (pUid->type != ISO14443A_TAGTYPE_TOPAZ))
    {
        for(i = 0; i < 3; i++)
        {
            cmd = ISO14443A_CMD_SELECT0 + i * 2;
            state = ISO14443A_AnticollisionTag(cmd, uid);       //防冲突
            if(state == CR95HF_ERR_SUCCESS)
            {
                state = ISO14443A_SelectTag(cmd, uid, &sak);
                if(state == CR95HF_ERR_SUCCESS)
                {
                    if(i == 0)
                    {
                        pUid->sak = sak;
                    }
                    //UID不完整
                    if(uid[0] == ISO14443A_UID_CT)
                    {
                        if(cmd == ISO14443A_CMD_SELECT2)
                        {
                            state = CR95HF_ERR_SAK;
                            break;
                        }
                        else
                        {
                            memcpy(pUid->uid + len, uid + 1, 3);
                            len = 3 + len;
                        }
                    }
                    else
                    {
                        memcpy(pUid->uid + len, uid, 4);
                        len = 4 + len;
                        break;
                    }
                }
            }
            if(state != CR95HF_ERR_SUCCESS)
            {
                break;
            }
        }
    }

    if(state == CR95HF_ERR_SUCCESS)
    {
        pUid->len = len;
    }

    return state;
}

u8 ISO14443A_Halt(void)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;  //halt par ctrl
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_HALTA;         //halt0x52
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;                        //ctrl_0x00默认就是0x00
    //append crc  最后一个字节全部发送
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8; 

    CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_INVENTORY_TIME);

    return state;
}

u8 ISO14443A_Rats(u8 *pAts, u8 *pLen)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;  //halt par ctrl
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_RATS;                        //rats
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_FSDI_FSD | ISO14443A_CID;        //fsdi cid
    //append crc  最后一个字节全部发送
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_INVENTORY_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        state = CR95HF_Iso14443ACheckResponse(&g_sCr95HfFrame);
        if(state == CR95HF_ERR_SUCCESS)
        {
            *pLen = g_sCr95HfFrame.frame[0];
            if(*pLen <= g_sCr95HfFrame.len - CR95HF_SENDRCV_14443A_RSPTRIL_LEN)
            {
                memcpy(pAts, g_sCr95HfFrame.frame, *pLen);
            }
            else
            {
                state = CR95HF_ERR_LENGTH;
            }
        }
    }

    return state;
}

u8 ISO14443A_AuthM1(u8 *pUID, u8 authMode, u8 *pKey, u8 blockAddr)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u32 iv = 0;
    u16 crc = 0;
    int i = 0;  //下面需要判断负数

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;                                                 //CMD + ADDR + PWD + UID + ctrl
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = authMode;                  //Key KeyB
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = blockAddr;                 //block地址，注意不是sector地址
    crc = ISO14443A_CalCrc(g_sCr95HfFrame.frame, g_sCr95HfFrame.len);
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = (crc >> 0) & 0xFF;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = (crc >> 8) & 0xFF;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_FRAME_LSB_BIT8;  
    if(g_sISO14443AMifareAuth.crypto1On == 1)
    {
        for(i = 0; i < 4; i++)
        {
            g_sISO14443AAuthFrame.frame[i] = ISO14443A_GetOdd(g_sCr95HfFrame.frame[i]);
        }
        Mifare_Cipher(g_sISO14443AAuthFrame.frame, 4, 0);
        for(i = 0; i < 4; i++)
        {
            g_sCr95HfFrame.frame[i * 2 + 0] = g_sISO14443AAuthFrame.frame[i] & 0xFF;
            g_sCr95HfFrame.frame[i * 2 + 1] = (g_sISO14443AAuthFrame.frame[i] >> 1) & 0x80;
        }
        g_sCr95HfFrame.len = 8;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_PARITY_MOD | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
    }   

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_AUTH_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        if(g_sISO14443AMifareAuth.crypto1On == 1)
        {
            for(i = 0; i < 4; i++)
            {
                g_sISO14443AAuthFrame.frame[i] = g_sCr95HfFrame.frame[i * 2 + 1] & 0x80;
                g_sISO14443AAuthFrame.frame[i] <<= 1;
                g_sISO14443AAuthFrame.frame[i] |= (g_sCr95HfFrame.frame[i * 2 + 0] & 0xFF);
            }
            
            for(i = 0; i < 4; i++)
            {
                g_sCr95HfFrame.frame[i] = g_sISO14443AAuthFrame.frame[i];
            }
            
            g_sISO14443AMifareAuth.nt = ARRAY_TO_UINT32(g_sCr95HfFrame.frame);
            
            g_sISO14443AMifareAuth.lfsr = *((u64 *)pKey);
            iv = g_sISO14443AMifareAuth.uid ^ g_sISO14443AMifareAuth.nt;
            g_sISO14443AMifareAuth.nt = g_sISO14443AMifareAuth.nt ^ MIFARE_update_word(iv, 1);
            g_sISO14443AMifareAuth.uid = ARRAY_TO_UINT32(pUID);
            g_sISO14443AMifareAuth.nr = 0xFFFFFFFF;
        }
        else
        {
            g_sISO14443AMifareAuth.nt = ARRAY_TO_UINT32(g_sCr95HfFrame.frame);
            g_sISO14443AMifareAuth.lfsr = *((u64 *)pKey);
            g_sISO14443AMifareAuth.uid = ARRAY_TO_UINT32(pUID);
            g_sISO14443AMifareAuth.nr = 0xFFFFFFFF;
        }
    }

    if(g_sISO14443AMifareAuth.crypto1On == 0) 
    {
        //初始化编码器和ks1
        iv = g_sISO14443AMifareAuth.uid ^ g_sISO14443AMifareAuth.nt;
        MIFARE_update_word(iv, 0);
        
        //nr xor ks1，并生成ks2
        UINT32_TO_ARRAY_WITH_PARITY(g_sISO14443AMifareAuth.nr, g_sISO14443AAuthFrame.frame);
        for(i = 3; i >= 0; i--)
        {                           /* Same as in MIFARE_update_word, but with added parity */
            g_sISO14443AAuthFrame.frame[3 - i] = g_sISO14443AAuthFrame.frame[3 - i] ^ MIFARE_update_byte((g_sISO14443AMifareAuth.nr >> (i * 8)) & 0xff, 0);
            g_sISO14443AAuthFrame.frame[3 - i] ^= mf20(g_sISO14443AMifareAuth.lfsr) << 8;
        } 
        
        //suc2(nt) xor ks2，并生成ks3，以后的编码和解码都用ks3完成
        g_sISO14443AMifareAuth.ar = prng_next(64);
        UINT32_TO_ARRAY_WITH_PARITY(g_sISO14443AMifareAuth.ar, g_sISO14443AAuthFrame.frame + 4);
        for(i = 0; i < 4; i++)
        {
            g_sISO14443AAuthFrame.frame[i + 4] = g_sISO14443AAuthFrame.frame[i + 4] ^ MIFARE_update_byte(0, 0);
            g_sISO14443AAuthFrame.frame[i + 4] = g_sISO14443AAuthFrame.frame[i + 4] ^ (mf20(g_sISO14443AMifareAuth.lfsr) << 8);
        }
    }
    else
    {        
        //nr xor ks1，并生成ks2
        UINT32_TO_ARRAY_WITH_PARITY(g_sISO14443AMifareAuth.nr, g_sISO14443AAuthFrame.frame);
        for(i = 3; i >= 0; i--)
        {                           /* Same as in MIFARE_update_word, but with added parity */
            g_sISO14443AAuthFrame.frame[3 - i] = g_sISO14443AAuthFrame.frame[3 - i] ^ MIFARE_update_byte((g_sISO14443AMifareAuth.nr >> (i * 8)) & 0xff, 0);
            g_sISO14443AAuthFrame.frame[3 - i] ^= mf20(g_sISO14443AMifareAuth.lfsr) << 8;
        } 
        
        //suc2(nt) xor ks2，并生成ks3，以后的编码和解码都用ks3完成
        g_sISO14443AMifareAuth.ar = prng_next(64);
        UINT32_TO_ARRAY_WITH_PARITY(g_sISO14443AMifareAuth.ar, g_sISO14443AAuthFrame.frame + 4);
        for(i = 0; i < 4; i++)
        {
            g_sISO14443AAuthFrame.frame[i + 4] = g_sISO14443AAuthFrame.frame[i + 4] ^ MIFARE_update_byte(0, 0);
            g_sISO14443AAuthFrame.frame[i + 4] = g_sISO14443AAuthFrame.frame[i + 4] ^ (mf20(g_sISO14443AMifareAuth.lfsr) << 8);
        }
    }
    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;                     //CMD + ADDR + PWD + UID + ctrl
    for(i = 0; i < 8; i++)
    {
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = g_sISO14443AAuthFrame.frame[i] & 0xFF;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = (g_sISO14443AAuthFrame.frame[i] >> 1) & 0x80;
    }
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_PARITY_MOD | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
    
    a_memset(&g_sCr95HfFrame, 0, sizeof(g_sCr95HfFrame));
    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_AUTH_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        u32 ta1 = 0, ta2 = 0;
        for(i = 0; i < 4; i++)
        {
            g_sISO14443AAuthFrame.frame[i] = g_sCr95HfFrame.frame[i * 2 + 1] & 0x80;
            g_sISO14443AAuthFrame.frame[i] <<= 1;
            g_sISO14443AAuthFrame.frame[i] |= (g_sCr95HfFrame.frame[i * 2 + 0] & 0xFF);
        }
        ta1 = ARRAY_TO_UINT32(g_sISO14443AAuthFrame.frame);
        ta2 = prng_next(96) ^ MIFARE_update_word(0, 0);
        if(ta1 == ta2)
        {
            g_sISO14443AMifareAuth.crypto1On = 1;
        }
        else
        {
            g_sISO14443AMifareAuth.crypto1On = 0;
        }
    }
    else
    {
        state = g_sCr95HfFrame.param.code;
    }
    

    return state;
}

u8 ISO14443A_ReadMifareBlock(u8 blockAddr, u8 *pBlock)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 i = 0;
    u16 crc = 0;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;                      // cmd addr ctrl
    
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_READ;        //cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = blockAddr;        //adddr
    crc = ISO14443A_CalCrc(g_sCr95HfFrame.frame, g_sCr95HfFrame.len);
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = (crc >> 0) & 0xFF;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = (crc >> 8) & 0xFF;
    for(i = 0; i < g_sCr95HfFrame.len; i++)
    {
        g_sISO14443AAuthFrame.frame[i] = ISO14443A_GetOdd(g_sCr95HfFrame.frame[i]);
    }
    Mifare_Cipher(g_sISO14443AAuthFrame.frame, g_sCr95HfFrame.len, 0);
    for(i = 0; i < g_sCr95HfFrame.len; i++)
    {
        g_sCr95HfFrame.frame[i * 2 + 0] = g_sISO14443AAuthFrame.frame[i] & 0xFF;
        g_sCr95HfFrame.frame[i * 2 + 1] = (g_sISO14443AAuthFrame.frame[i] >> 1) & 0x80;
    }
    g_sCr95HfFrame.len *= 2;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_PARITY_MOD | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc

    state = CR95HF_SendReceive(&g_sCr95HfFrame, ISO14443A_READ_TIME);
    if(state == CR95HF_ERR_SUCCESS)
    {
        for(i = 0; i < ISO14443A_M1BLOCK_LEN + 2; i++)
        {
            g_sISO14443AAuthFrame.frame[i] = g_sCr95HfFrame.frame[i * 2 + 1] & 0x80;
            g_sISO14443AAuthFrame.frame[i] <<= 1;
            g_sISO14443AAuthFrame.frame[i] |= (g_sCr95HfFrame.frame[i * 2 + 0] & 0xFF);
        }
        Mifare_Cipher(g_sISO14443AAuthFrame.frame, ISO14443A_M1BLOCK_LEN + 2, 0);
        if(ISO14443A_CheckParity(g_sISO14443AAuthFrame.frame, ISO14443A_M1BLOCK_LEN + 2))
        {
            for(i = 0; i < ISO14443A_M1BLOCK_LEN; i++)
            {
                pBlock[i] = g_sISO14443AAuthFrame.frame[i] & 0xFF;
            }
        }
        else
        {
            state = CR95HF_ERR_PARITY;
        }
    }

    return state;
}

u8 ISO14443A_WriteMifareBlock16(u8 blockAddr, u8 *pBlock)
{
    u8 state = CR95HF_ERR_SUCCESS;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;                                                     //CMD + ADDR + ctrl
    
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_WRITE16;         //Key KeyB
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = blockAddr;                     //block地址，注意不是sector地址
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc

    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        //存储区操作命令响应ACK
        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_BIT)
        {
            //ACK只有4bit
            if(g_sCr95HfFrame.len == ISO14443A_ACK_LEN)
            {
                u8 ack = g_sCr95HfFrame.frame[0];
                state = ISO14443A_CheckACK(ack);
            }
            else
            {
                state = CR95HF_ERR_TIMEOUT;
            }
        }
        else
        {
            state = g_sCr95HfFrame.param.code;
        }
    }

    if(state != CR95HF_ERR_SUCCESS)
    {
        break;
    }

    g_sCr95HfFrame.len = ISO14443A_M1BLOCK_LEN + 1;          //DATA + ctrl
    a_memcpy(buffer + i * ISO14443A_M1BLOCK_LEN,
             g_sCr95HfFrame.frame,
             ISO14443A_M1BLOCK_LEN);
    g_sCr95HfFrame.frame[ISO14443A_M1BLOCK_LEN] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc

    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        //存储区操作命令响应ACK
        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_BIT)
        {
            //ACK只有4bit
            if(g_sCr95HfFrame.len == ISO14443A_ACK_LEN)
            {
                u8 ack = g_sCr95HfFrame.frame[0];
                state = ISO14443A_CheckACK(ack);
            }
            else
            {
                state = CR95HF_ERR_TIMEOUT;
            }
        }
        else
        {
            state = g_sCr95HfFrame.param.code;
        }
    }




    return state;

}

u8 ISO14443A_Write4Bytes(u8 *buffer, u8 addr, u8 num)
{
    u8 state = CR95HF_ERR_SUCCESS;
    u8 i = 0;

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    for(i = 0; i < num; i++)
    {
        g_sCr95HfFrame.len = 0;                                       //CMD + ADDR + ctrl
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_WRITE4;        //Key KeyB
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = addr + i;                    //block地址，注意不是sector地址
        a_memcpy(buffer + i * ISO14443A_M0BLOCK_LEN,
                 g_sCr95HfFrame.frame + g_sCr95HfFrame.len,
                 ISO14443A_M0BLOCK_LEN);
        g_sCr95HfFrame.len += ISO14443A_M0BLOCK_LEN;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc

        state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
        if(state == CR95HF_ERR_SUCCESS)
        {
            //存储区操作命令响应ACK
            if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_BIT)
            {
                //ACK只有4bit
                if(g_sCr95HfFrame.len == ISO14443A_ACK_LEN)
                {
                    u8 ack = g_sCr95HfFrame.frame[0];
                    state = ISO14443A_CheckACK(ack);
                }
                else
                {
                    state = CR95HF_ERR_TIMEOUT;
                }
            }
            else
            {
                state = g_sCr95HfFrame.param.code;
            }
        }

        if(state != CR95HF_ERR_SUCCESS)
        {
            break;
        }
    }




    return state;

}

u8 ISO14443A_Value(u8 opMode, u8 *buffer, u8 transAddr, u8 valueAddr)
{
    u8 state = CR95HF_ERR_SUCCESS;

    //第一步:发送命令
    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;                      // cmd addr ctrl
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = opMode;        //cmd
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = valueAddr;        //adddr
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc

    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_BIT)
        {
            //ACK只有4bit
            if(g_sCr95HfFrame.len == ISO14443A_ACK_LEN)
            {
                u8 ack = g_sCr95HfFrame.frame[0];
                state = ISO14443A_CheckACK(ack);
            }
            else
            {
                state = CR95HF_ERR_TIMEOUT;
            }
        }
        else
        {
            state = g_sCr95HfFrame.param.code;
        }
    }

    if(state != CR95HF_ERR_SUCCESS)
    {
        return state;
    }

    //第二步，写入value值
    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = ISO14443A_M1VALUE_LEN + 1;          //DATA + ctrl
    a_memcpy(buffer, g_sCr95HfFrame.frame, ISO14443A_M1VALUE_LEN);
    g_sCr95HfFrame.frame[ISO14443A_M1VALUE_LEN] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc

    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        //存储区操作命令响应ACK
        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_BIT)
        {
            //ACK只有4bit
            if(g_sCr95HfFrame.len == ISO14443A_ACK_LEN)
            {
                u8 ack = g_sCr95HfFrame.frame[0];
                state = ISO14443A_CheckACK(ack);
            }
            else
            {
                state = CR95HF_ERR_TIMEOUT;
            }
        }
        else
        {
            state = g_sCr95HfFrame.param.code;
        }
    }

    if(state != CR95HF_ERR_SUCCESS)
    {
        return state;
    }


    //第三步，transfer:将标签寄存器中的数据写入到存储区中

    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 3;                      // cmd addr ctrl
    g_sCr95HfFrame.frame[0] = ISO14443A_CMD_TRANSFER;        //cmd
    g_sCr95HfFrame.frame[1] = transAddr;        //adddr
    g_sCr95HfFrame.frame[2] = CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc

    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_BIT)
        {
            //ACK只有4bit
            if(g_sCr95HfFrame.len == ISO14443A_ACK_LEN)
            {
                u8 ack = g_sCr95HfFrame.frame[0];
                state = ISO14443A_CheckACK(ack);
            }
            else
            {
                state = CR95HF_ERR_TIMEOUT;
            }
        }
        else
        {
            state = g_sCr95HfFrame.param.code;
        }
    }

    return state;
}

u8 ISO14443A_TopazGetUid(ISO14443A_UID *pUid, u8 mode)
{
    u8 state = CR95HF_ERR_SUCCESS;
    
    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
    g_sCr95HfFrame.len = 0;                      // cmd addr ctrl
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_TPZRID;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_TOPAZ | CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
    
    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
    if(state == CR95HF_ERR_SUCCESS)
    {
        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_DAT)
        {
            //
            if(g_sCr95HfFrame.frame[g_sCr95HfFrame.len - 2] == 0x08)
            {
                u8 i = 0;
                pUid->sak = g_sCr95HfFrame.frame[0]; //head0
                memcpy(pUid->uid, g_sCr95HfFrame.frame + 2, 4);
                pUid->len = 4;

                for(i = 0; i < 3; i++)
                {
                    g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
                    g_sCr95HfFrame.len = 0;                      // cmd addr ctrl
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_TPZREAD;
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 4 + i;
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[0];
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[1];
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[2];
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[3];
                    g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_TOPAZ | CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
                    state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
                    if(state == CR95HF_ERR_SUCCESS)
                    {
                        if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_DAT)
                        {
                            pUid->uid[pUid->len++] = g_sCr95HfFrame.frame[1];
                        }
                        else
                        {
                            state = g_sCr95HfFrame.param.code;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                
                            
            }
        }
        else
        {
            state = g_sCr95HfFrame.param.code;
        }
    }

    return state;
}
u8 ISO14443A_TopazReadBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock)
{
    u8 state = CR95HF_ERR_SUCCESS;
    
    if(pUid->sak == ISO14443A_TAGTYPE_TOPAZ96)
    {
        u8 i = 0;
        for(i = 0; i < ISO14443A_TOPAZ_BLOCK_LEN; i++)
        {
            g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
            g_sCr95HfFrame.len = 0;
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_TPZREAD;
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ((addr & 0x0F) << 3) | i;
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[0];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[1];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[2];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[3];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_TOPAZ | CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
            
            state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
            if(state == CR95HF_ERR_SUCCESS)
            {
                if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_DAT)
                {
                    pBlock[i] = g_sCr95HfFrame.frame[1];
                }
                else
                {
                    state = g_sCr95HfFrame.param.code;
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
        g_sCr95HfFrame.len = 0;
            
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_TPZREADBLK;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = addr & 0x7F;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = 0x00;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[0];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[1];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[2];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[3];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_TOPAZ | CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
            
        state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
        if(state == CR95HF_ERR_SUCCESS)
        {
            if(g_sCr95HfFrame.param.code == CR95HF_SENDRCV_RSPCODE_DAT)
            {
                memcpy(pBlock, g_sCr95HfFrame.frame + 1, ISO14443A_TOPAZ_BLOCK_LEN);
            }
            else
            {
                state = g_sCr95HfFrame.param.code;
            }
        }
    }
    
    return state;
}
u8 ISO14443A_TopazWriteBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock)
{
    u8 state = CR95HF_ERR_SUCCESS;
    
    if(pUid->sak == ISO14443A_TAGTYPE_TOPAZ96)
    {
        u8 i = 0;
        for(i = 0; i < ISO14443A_TOPAZ_BLOCK_LEN; i++)
        {
            g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
            g_sCr95HfFrame.len = 0;
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_TPZWE;
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ((addr & 0x0F) << 3) | i;
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[i];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[0];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[1];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[2];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[3];
            g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_TOPAZ | CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
            
            state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
            if(state == CR95HF_ERR_SUCCESS)
            {
                if(g_sCr95HfFrame.param.code != CR95HF_SENDRCV_RSPCODE_DAT)
                {
                    state = g_sCr95HfFrame.param.code;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        g_sCr95HfFrame.param.cmd = CR95HF_CMD_SEND_RECEIVE;
        g_sCr95HfFrame.len = 0;
            
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = ISO14443A_CMD_TPZWEBLK;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = addr & 0x7F;
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[0];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[1];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[2];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[3];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[4];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[5];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[6];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pBlock[7];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[0];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[1];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[2];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = pUid->uid[3];
        g_sCr95HfFrame.frame[g_sCr95HfFrame.len++] = CR95HF_SENDRCV_14443A_FLG_TOPAZ | CR95HF_SENDRCV_14443A_FLG_APPEND_CRC | ISO14443A_FRAME_LSB_BIT8;  //标准帧 + crc
            
        state = CR95HF_SendReceive(&g_sCr95HfFrame, CR95HF_POLLING_DELAY);
        if(state == CR95HF_ERR_SUCCESS)
        {
            if(g_sCr95HfFrame.param.code != CR95HF_SENDRCV_RSPCODE_DAT)
            {
                state = g_sCr95HfFrame.param.code;
            }
        }
    }
    
    return state;
}

