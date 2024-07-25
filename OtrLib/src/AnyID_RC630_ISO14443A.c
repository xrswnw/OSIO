#include "AnyID_RC630_ISO14443A.h"
#include "AnyID_Des.h"

ISO14443A_4 g_sISO1444A4 = {0};

const u8 g_sRc630I14443A106[RC630_PROTOCOL_CONFIG_SIZE] =
{
    0x8E,
    0x15,
    0x11,
    0x06,
    0x18,
    0x18,
    0x0F,
    0x27,
    0x00,
    0xC0,
    0xCF,
    0x04,
    0x3F,
    0x12,
    0x0A
};
u8 ISO14443A_Init(void)
{
   u8 i = 0;
    RC630_SelectProtocol(RC630_RXTX_I14443A_106, RC630_RXTX_I14443A_106);
    for(i = 0; i < RC630_PROTOCOL_CONFIG_SIZE; i++)
    {
        RC630_WriteReg(g_sRc630RegAdd[i], g_sRc630I14443A106[i]);
    }

    return TRUE;
}

u8 ISO14443A_CheckBcc(u8 *pUID)
{
    u8 bcc = 0;

    bcc = pUID[0] ^ pUID[1] ^ pUID[2] ^ pUID[3] ^ pUID[4];
    if(bcc == 0)
    {
        return RC630_STAT_OK;
    }
    else
    {
        return RC630_STAT_BCC_ERROR;
    }
}

u8 ISO14443A_RequestTag(u8 mode, u16 *pTagType)
{
    u8 state = RC630_STAT_OK;

    g_sRc630Frame.frame[0] = mode;

    g_sRc630Frame.txLen = 1;

    g_sRc630Frame.timeout = ISO14443A_INVENTORY_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(TRUE, FALSE, FALSE, 7, 0);
    state = RC630_Command();
    if(state == RC630_STAT_OK)
    {
        *pTagType = g_sRc630Frame.frame[1];
        *pTagType <<= 8;
        *pTagType |= g_sRc630Frame.frame[0];
    }

    return state;
}


u8 ISO14443A_AnticollisionTag(u8 lv, u8 *pUID)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = lv;
    g_sRc630Frame.frame[pos++] = 0x20;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, FALSE, FALSE, 0, 0);
    state = RC630_Command();
    if(state == RC630_STAT_OK)
    {
        state = ISO14443A_CheckBcc(g_sRc630Frame.frame);
        if(state == RC630_STAT_OK)
        {
            memcpy(pUID, g_sRc630Frame.frame, 5);
        }
    }

    return state;
}


u8 ISO14443A_SelectTag(u8 lv, u8 *pUID, u8 *pSak)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = lv;
    g_sRc630Frame.frame[pos++] = 0x70;
    memcpy(g_sRc630Frame.frame + pos, pUID, 5);
    pos += 5;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, TRUE, 0, 0);
    state = RC630_Command();
    if(state == RC630_STAT_OK)
    {
        *pSak = g_sRc630Frame.frame[0];
    }

    return state;
}

u8 ISO14443A_GetUid(ISO14443A_UID *pUid, u8 mode)
{
    u8 state = RC630_STAT_OK;
    state = ISO14443A_RequestTag(mode, &(pUid->type));
    if(state == RC630_STAT_OK && (pUid->type != ISO14443A_TAGTYPE_TOPAZ) && (pUid->type != ISO14443A_TAGTYPE_FM11RF005))
    {
        u8 uid[ISO14443A_MAX_UID_LEN] = {0};
        state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT0, uid);
        if(state == RC630_STAT_OK)
        {
            state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT0, uid, &(pUid->sak));
            if(state == RC630_STAT_OK)
            {
                pUid->len = ISO14443A_SIGNAL_UID_LEN;
                if(uid[0] == ISO14443A_UID_CT)
                {
                    memcpy(pUid->uid, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);
                    state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT1, uid);
                    if(state == RC630_STAT_OK)
                    {
                        state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT1, uid, &(pUid->sak));
                        if(state == RC630_STAT_OK)
                        {
                            pUid->len = ISO14443A_DOUBLE_UID_LEN;
                            if(uid[0] == ISO14443A_UID_CT)
                            {
                                memcpy(pUid->uid + 3, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);

                                state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT2, uid);
                                if(state == RC630_STAT_OK)
                                {
                                    state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT2, uid, &(pUid->sak));
                                    if(state == RC630_STAT_OK)
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


    return state;
}

u8 ISO14443A_Halt(void)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = ISO14443A_CMD_HALTA;
    g_sRc630Frame.frame[pos++] = 0x00;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, TRUE, 0, 0);
    RC630_Command();

    return state;
}



u8 ISO14443A_AuthM1(u8 *pUID, u8 authMode, u8 *pKey, u8 blockAddr)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    memcpy(g_sRc630Frame.frame + pos, pKey, ISO14443A_M1_KEY_LEN);
    pos += ISO14443A_M1_KEY_LEN;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_AUTH_TIME;
    g_sRc630Frame.cmd = RC630_CMD_LOADKEY;

    RC630_Config(FALSE, FALSE, FALSE, 0, 0);
    state = RC630_Command();
    //if(state == RC630_STAT_OK)
    {
        pos = 0;

        g_sRc630Frame.frame[pos++] = authMode;
        g_sRc630Frame.frame[pos++] = blockAddr;
        memcpy(g_sRc630Frame.frame + pos, pUID, ISO14443A_SIGNAL_UID_LEN);
        pos += ISO14443A_SIGNAL_UID_LEN;

        g_sRc630Frame.txLen = pos;

        g_sRc630Frame.timeout = ISO14443A_AUTH_TIME;
        g_sRc630Frame.cmd = RC630_CMD_MFAUTHENT;

        RC630_Config(FALSE, FALSE, FALSE, 0, 0);
        state = RC630_Command();

        if((RC630_ReadReg(RC630_REG_STATUS) & RC630_BIT_CRYPTO1ON) == 0)//
        {
            state = RC630_STAT_AUTH_ERROR;
        }
        else
        {
            state = RC630_STAT_OK;
        }
    }

    return state;
}

u8 ISO14443A_ReadMifareBlock(u8 blockAddr, u8 *pBlock)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = ISO14443A_CMD_READ;
    g_sRc630Frame.frame[pos++] = blockAddr;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_READ_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, TRUE, 0, 0);
    state = RC630_Command();

    if(state == RC630_STAT_OK)//
    {
        memcpy(pBlock, g_sRc630Frame.frame, ISO14443A_M1BLOCK_LEN);
    }

    return state;
}


u8 ISO14443A_WriteMifareBlock16(u8 blockAddr, u8 *pBlock)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = ISO14443A_CMD_WRITE16;
    g_sRc630Frame.frame[pos++] = blockAddr;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_WRITE_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, FALSE, 0, 0);
    state = RC630_Command();

    if(state == RC630_STAT_OK)//
    {
        if(g_sRc630Frame.rxLen == 1 && g_sRc630Frame.frame[0] == ISO14443A_ACK_OK)
        {
            memcpy(g_sRc630Frame.frame, pBlock, ISO14443A_M1BLOCK_LEN);

            g_sRc630Frame.txLen = ISO14443A_M1BLOCK_LEN;

            g_sRc630Frame.timeout = ISO14443A_WRITE_TIME;
            g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

            state = RC630_Command();
            if(state == RC630_STAT_OK)//
            {
                if((g_sRc630Frame.rxLen != 1) || (g_sRc630Frame.frame[0] != ISO14443A_ACK_OK))
                {
                    state = RC630_STAT_NAK;
                }
            }
        }

    }

    return state;
}



u8 ISO14443A_WriteMifareBlock4(u8 blockAddr, u8 *pBlock)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = ISO14443A_CMD_WRITE4;
    g_sRc630Frame.frame[pos++] = blockAddr;
    memcpy(g_sRc630Frame.frame + pos, pBlock, ISO14443A_M0BLOCK_LEN);
    pos += ISO14443A_M0BLOCK_LEN;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_WRITE_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, FALSE, 0, 0);
    state = RC630_Command();

    if(state == RC630_STAT_OK)//
    {
        if(g_sRc630Frame.rxLen == 1 && g_sRc630Frame.frame[0] == ISO14443A_ACK_OK)
        {
            if(state == RC630_STAT_OK)//
            {
                if((g_sRc630Frame.rxLen != 1) || (g_sRc630Frame.frame[0] != ISO14443A_ACK_OK))
                {
                    state = RC630_STAT_NAK;
                }
            }
        }

    }

    return state;
}


u8 ISO14443A_Value(u8 opMode, u8 *pDataBuffer, u8 transAddr, u8 valueAddr)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = opMode;
    g_sRc630Frame.frame[pos++] = valueAddr;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_VALUE_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, FALSE, 0, 0);
    state = RC630_Command();

    if(state == RC630_STAT_OK)//
    {
        if(g_sRc630Frame.rxLen == 1 && g_sRc630Frame.frame[0] == ISO14443A_ACK_OK)
        {
            memcpy(g_sRc630Frame.frame, pDataBuffer, ISO14443A_M1VALUE_LEN);

            g_sRc630Frame.txLen = ISO14443A_M1VALUE_LEN;

            g_sRc630Frame.timeout = ISO14443A_VALUE_TIME;
            g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

            state = RC630_Command();
            if(state == RC630_STAT_OK)//
            {
                state = RC630_STAT_NAK;
            }
            else
            {
                pos = 0;
                g_sRc630Frame.frame[pos++] = ISO14443A_CMD_TRANSFER;
                g_sRc630Frame.frame[pos++] = transAddr;

                g_sRc630Frame.txLen = pos;

                g_sRc630Frame.timeout = ISO14443A_VALUE_TIME;
                g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

                RC630_Config(FALSE, TRUE, FALSE, 0, 0);
                state = RC630_Command();
                if(state == RC630_STAT_OK)//
                {
                    if((g_sRc630Frame.rxLen != 1) || (g_sRc630Frame.frame[0] != ISO14443A_ACK_OK))
                    {
                        state = RC630_STAT_NAK;
                    }
                }
            }
        }

    }

    return state;
}

const u16 fsdi[16] = {16, 24, 32, 40, 48, 64, 96, 128, 256, 257, 257, 257, 257, 257, 257, 257};
u8 ISO14443A_Rats(u8 *pAts, u8 *pLen)
{
    u8 state = RC630_STAT_OK;
    u8 pos = 0;

    g_sRc630Frame.frame[pos++] = ISO14443A_CMD_RATS;
    g_sRc630Frame.frame[pos++] = ISO14443A_FSDI_FSD | ISO14443A_CID;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_WRITE_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, TRUE, 0, 0);
    state = RC630_Command();

    if(state == RC630_STAT_OK)//
    {
        u8 temp = 0;

        *pLen = g_sRc630Frame.rxLen;
        memcpy(pAts, g_sRc630Frame.frame, g_sRc630Frame.rxLen);

        temp = pAts[1];
        g_sISO1444A4.fsc = pAts[1] & 0x0F;
        g_sISO1444A4.fsc = fsdi[g_sISO1444A4.fsc];
        if(temp & 0x20)      //TB是否存在
        {
            if(temp & 0x10)  //TA是否存在
            {
                g_sISO1444A4.fwt = (pAts[3] >> 4) & 0x0F;
            }
            else
            {
                g_sISO1444A4.fwt = (pAts[2] >> 4) & 0x0F;
            }
        }
        else
        {

            g_sISO1444A4.fwt = ISO14443A4_FWI_DFT;
        }
        //fwt = (256*16/fc) * 2^fwi = 2^(fwi + 12)/13.56 us
        g_sISO1444A4.fwt = (1 << (g_sISO1444A4.fwt + 12)) / 14;    //单位us
        g_sISO1444A4.fwt += 1000;  //多加1ms
    }

    return state;
}

u8 ISO14443A_TransAPDU(u8 *pApdu, u8 len, u8 *pAsk, u8 *pAskLen)
{
    u8 state = RC630_STAT_OK;
    u8 pcb = 0;

    memcpy(g_sRc630Frame.frame, pApdu, len);
    g_sRc630Frame.txLen = len;

    g_sRc630Frame.timeout = g_sISO1444A4.fwt; //fwt单位是us
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, TRUE, 0, 0);
    //因为有些处理需要比较长的时间，所以PICC有时会发送SBLOCK请求延时
    //如果响应帧是SBLOCK中的WTX，那么将WTX请求直接发送到PICC，一直到接收真正的响应帧
    do{
        *pAskLen = 0;

        state = RC630_Command();
        if(state == RC630_STAT_OK)
        {
            *pAskLen = g_sRc630Frame.rxLen;
            memcpy(pAsk, g_sRc630Frame.frame, g_sRc630Frame.rxLen);

            pcb = pAsk[0] & (~ISO14443A_IPCB_NUMMASK);
            if((pcb == ISO14443A_PCB_IBLOCK) || (pcb == ISO14443A_PCB_RBLOCK))
            {
                if((pAsk[0] & ISO14443A_IPCB_NUMMASK) == (g_sISO1444A4.pcb & ISO14443A_IPCB_NUMMASK))
                {
                    g_sISO1444A4.pcb = (~g_sISO1444A4.pcb) & ISO14443A_IPCB_NUMMASK;
                }
            }
            else if(pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX))
            {
                memcpy(g_sRc630Frame.frame, pAsk, *pAskLen);
                g_sRc630Frame.rxLen = *pAskLen;
            }
            else
            {
                state = RC630_STAT_ERROR;
            }
        }
    }while((state == RC630_STAT_OK) && (pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX)));

    return state;
}

u8 ISO14443A_DirectCommand(u8 *pFrame, u8 len, u8 *pAsk, u8 *pAskLen)
{
    u8 state = RC630_STAT_OK;

    memcpy(g_sRc630Frame.frame, pFrame, len);
    g_sRc630Frame.txLen = len;

    g_sRc630Frame.timeout = ISO14443A_WRITE_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, FALSE, FALSE, 0, 0);

    state = RC630_Command();
    if(state == RC630_STAT_OK)
    {
        *pAskLen = g_sRc630Frame.rxLen;
        memcpy(pAsk, g_sRc630Frame.frame, g_sRc630Frame.rxLen);
    }

    return state;
}

u8 ISO14443A_Dtu(u8 *pTxFrame, u8 txLen, u8 txBit, u8 *pRxBit, u32 timeout)
{
    u8 state = RC630_STAT_OK;

    memcpy(g_sRc630Frame.frame, pTxFrame, txLen);
    g_sRc630Frame.txLen = txLen;

    g_sRc630Frame.timeout = timeout;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, FALSE, FALSE, txBit, (*pRxBit) & 0x07);

    state = RC630_Command();
    if(state == RC630_STAT_OK)
    {
        *pRxBit = RC630_ReadReg(RC630_REG_RXBITCTRL) & 0x07;
    }

    return state;
}


u8 ISO14443A_AuthUltralightC(u8 *pKey)
{
    u8 state = RC630_STAT_OK;
    u8 iv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 rnd[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 rnda[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 rndb[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 t = 0;
    u8 txLen = 0;

    memcpy(rnd, rnda, 8);

    Des_InitializeKey(pKey, 0);
    Des_InitializeKey(pKey + 8, 1);

    g_sRc630Frame.frame[txLen++] = ISO14443A_CMD_AUTHM0U2S1;
    g_sRc630Frame.frame[txLen++] = 0x00;
    g_sRc630Frame.txLen = txLen;

    g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, TRUE, TRUE, 0, 0);

    state = RC630_Command();
    if(state == RC630_STAT_OK)
    {
        g_sRc630Frame.frame[0] = ISO14443A_CMD_AUTHM0U2S2;
        //解密rndb
        Des_DecryptAnyLength(g_sRc630Frame.frame + 1, 8, rndb, 0);
        Des_EncryptAnyLength(rndb, 8, temp, 1);
        Des_DecryptAnyLength(temp, 8, rndb, 0);
        //CBC模式
        a_xor(rndb, iv, 8);
        memcpy(iv, g_sRc630Frame.frame + 1, 8);

        //rndb循环左移8bit得到rndb1
        t = rndb[0];
        memcpy(rndb, rndb + 1, 7);
        rndb[7] = t;

        //CBC模式
        a_xor(rnda, iv, 8);
        //计算ek(rnda)
        Des_EncryptAnyLength(rnda, 8, g_sRc630Frame.frame + 1, 0);
        Des_DecryptAnyLength(g_sRc630Frame.frame + 1, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sRc630Frame.frame + 1, 0);
        memcpy(iv, g_sRc630Frame.frame + 1, 8);

        //CBC模式
        a_xor(rndb, iv, 8);
        //计算ek(rndb1)
        Des_EncryptAnyLength(rndb, 8, g_sRc630Frame.frame + 9, 0);
        Des_DecryptAnyLength(g_sRc630Frame.frame + 9, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sRc630Frame.frame + 9, 0);
        memcpy(iv, g_sRc630Frame.frame + 9, 8);

        txLen = 1 + 8 + 8;
        g_sRc630Frame.txLen = txLen;

        state = RC630_Command();
        if(state == RC630_STAT_OK)
        {
            //解密rnda
            Des_DecryptAnyLength(g_sRc630Frame.frame + 1, 8, rnda, 0);
            Des_EncryptAnyLength(rnda, 8, temp, 1);
            Des_DecryptAnyLength(temp, 8, rnda, 0);
            //CBC模式
            a_xor(rnda, iv, 8);

            //rndb循环右移8bit得到rnda
            t = rnd[0];
            memcpy(rnd, rnd + 1, 7);
            rnd[7] = t;
            if(memcmp(rnda, rnd, 8) != 0)
            {
                state = RC630_STAT_AUTH_ERROR;
            }
        }
    }

    return state;
}

//value !value value addr !addr addr ! addr
//value是有符号的4字节，低字节在低地址
void ISO14443A_FormatValue(u8 backUpAddr, int value, u8 *pBlockBuffer)
{
    u32 tempValue = 0;
    u8 tempAddr = 0;

    tempValue = ~value;
    tempAddr = ~backUpAddr;

    pBlockBuffer[0] = (value >> 0) & 0xFF;
    pBlockBuffer[1] = (value >> 8) & 0xFF;
    pBlockBuffer[2] = (value >> 16) & 0xFF;
    pBlockBuffer[3] = (value >> 24) & 0xFF;

    pBlockBuffer[4] = (tempValue >> 0) & 0xFF;
    pBlockBuffer[5] = (tempValue >> 8) & 0xFF;
    pBlockBuffer[6] = (tempValue >> 16) & 0xFF;
    pBlockBuffer[7] = (tempValue >> 24) & 0xFF;

    pBlockBuffer[8] = (value >> 0) & 0xFF;
    pBlockBuffer[9] = (value >> 8) & 0xFF;
    pBlockBuffer[10] = (value >> 16) & 0xFF;
    pBlockBuffer[11] = (value >> 24) & 0xFF;

    pBlockBuffer[12] = backUpAddr;
    pBlockBuffer[13] = tempAddr;
    pBlockBuffer[14] = backUpAddr;
    pBlockBuffer[15] = tempAddr;
}

int ISO14443A_CheckValueFormat(u8 *pBlockBuffer)
{
    u32 value1 = 0, value2 = 0, value3 = 0;
    u8 addr1 = 0, addr2 = 0, addr3 = 0, addr4 = 0;
    int rlt = RC630_STAT_OK;

    value1 |= (pBlockBuffer[0] << 0);
    value1 |= (pBlockBuffer[1] << 8);
    value1 |= (pBlockBuffer[2] << 16);
    value1 |= (pBlockBuffer[3] << 24);

    value2 |= (pBlockBuffer[4] << 0);
    value2 |= (pBlockBuffer[5] << 8);
    value2 |= (pBlockBuffer[6] << 16);
    value2 |= (pBlockBuffer[7] << 24);
    value2 = ~value2;

    value3 |= (pBlockBuffer[8] << 0);
    value3 |= (pBlockBuffer[9] << 8);
    value3 |= (pBlockBuffer[10] << 16);
    value3 |= (pBlockBuffer[11] << 24);

    addr1 = pBlockBuffer[12];
    addr2 = pBlockBuffer[13];
    addr2 = ~addr2;
    addr3 = pBlockBuffer[14];
    addr4 = pBlockBuffer[15];
    addr4 = ~addr4;

    if((value1 != value3) || (value2 != value3) ||
       (addr1 != addr2) || (addr2 != addr3) || (addr3 != addr4))
    {
        rlt = RC630_STAT_ERROR;
    }

    return rlt;

}


#define POLYNOMIAL                      0x8408   //x^16 + x^12 + x^5 + 1
#define PRESET_VALUE                    0xFFFF
u16 ISO14443A_TopazCalCrc(u8 *pFrame, u8 len)
{
    u16 crc = 0;
    u8 i = 0, j = 0;
    crc = PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pFrame[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ POLYNOMIAL;
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

BOOL ISO14443A_TopazCheckCrc(u8 *pFrame, u8 len)
{
    u16 crc = 0;
    BOOL b = FALSE;
    if(len > 2)
    {
        crc = ISO14443A_TopazCalCrc(pFrame, len - 2);
        if(pFrame[len - 2] == ((crc >> 0) & 0xFF) && pFrame[len - 1] == ((crc >> 8) & 0xFF))
        {
            b = TRUE;
        }
    }

    return b;
}


u8 ISO14443A_TopazGetUid(ISO14443A_UID *pUid, u8 mode)
{
    u16 crc = 0;
    u8 pos = 0;
    u8 state = RC630_STAT_OK;

    g_sRc630Frame.frame[pos++] = ISO14443A_CMD_TPZRID;
    g_sRc630Frame.frame[pos++] = 0x00;
    g_sRc630Frame.frame[pos++] = 0x00;
    g_sRc630Frame.frame[pos++] = 0x00;
    g_sRc630Frame.frame[pos++] = 0x00;
    g_sRc630Frame.frame[pos++] = 0x00;
    g_sRc630Frame.frame[pos++] = 0x00;
    crc = ISO14443A_TopazCalCrc(g_sRc630Frame.frame, pos);
    g_sRc630Frame.frame[pos++] = (crc >> 0) & 0xFF;
    g_sRc630Frame.frame[pos++] = (crc >> 8) & 0xFF;

    g_sRc630Frame.txLen = pos;

    g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
    g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

    RC630_Config(FALSE, FALSE, FALSE, 0, 0);
    state = RC630_TopazCommand();
    if(state == RC630_STAT_OK)
    {
        if(ISO14443A_TopazCheckCrc(g_sRc630Frame.frame, g_sRc630Frame.rxLen))
        {
            u8 i = 0;
            memcpy(pUid->uid, g_sRc630Frame.frame + 2, 4);
            pUid->len = 4;
            pUid->sak = g_sRc630Frame.frame[0]; //hr0
            for(i = 0; i < 3; i++)
            {
                pos = 0;
                g_sRc630Frame.frame[pos++] = ISO14443A_CMD_TPZREAD;
                g_sRc630Frame.frame[pos++] = 4 + i;
                g_sRc630Frame.frame[pos++] = 0x00;
                g_sRc630Frame.frame[pos++] = pUid->uid[0];
                g_sRc630Frame.frame[pos++] = pUid->uid[1];
                g_sRc630Frame.frame[pos++] = pUid->uid[2];
                g_sRc630Frame.frame[pos++] = pUid->uid[3];
                crc = ISO14443A_TopazCalCrc(g_sRc630Frame.frame, pos);
                g_sRc630Frame.frame[pos++] = (crc >> 0) & 0xFF;
                g_sRc630Frame.frame[pos++] = (crc >> 8) & 0xFF;

                g_sRc630Frame.txLen = pos;

                g_sRc630Frame.timeout = ISO14443A_WRITE_TIME;
                g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

                RC630_Config(FALSE, FALSE, FALSE, 0, 0);
                state = RC630_TopazCommand();
                if(state == RC630_STAT_OK)
                {
                    if(ISO14443A_TopazCheckCrc(g_sRc630Frame.frame, g_sRc630Frame.rxLen))
                    {
                        pUid->uid[pUid->len++] = g_sRc630Frame.frame[1]; //addr data
                    }
                    else
                    {
                        state = RC630_STAT_CRC_ERROR;
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
            state = RC630_STAT_CRC_ERROR;
        }
    }
    return state;
}

u8 ISO14443A_TopazReadBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock)
{
    u8 state = RC630_STAT_OK;
    u16 crc = 0;
    u8 pos = 0;
    if(pUid->sak == ISO14443A_TAGTYPE_TOPAZ96)
    {
        u8 i = 0;
        for(i = 0; i < ISO14443A_TOPAZ_BLOCK_LEN; i++)
        {
            pos = 0;
            g_sRc630Frame.frame[pos++] = ISO14443A_CMD_TPZREAD;
            g_sRc630Frame.frame[pos++] = ((addr & 0x0F) << 3) | i;
            g_sRc630Frame.frame[pos++] = 0x00;
            g_sRc630Frame.frame[pos++] = pUid->uid[0];
            g_sRc630Frame.frame[pos++] = pUid->uid[1];
            g_sRc630Frame.frame[pos++] = pUid->uid[2];
            g_sRc630Frame.frame[pos++] = pUid->uid[3];
            crc = ISO14443A_TopazCalCrc(g_sRc630Frame.frame, pos);
            g_sRc630Frame.frame[pos++] = (crc >> 0) & 0xFF;
            g_sRc630Frame.frame[pos++] = (crc >> 8) & 0xFF;

            g_sRc630Frame.txLen = pos;

            g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
            g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

            RC630_Config(FALSE, FALSE, FALSE, 0, 0);
            state = RC630_TopazCommand();
            if(state == RC630_STAT_OK)
            {
                if(ISO14443A_TopazCheckCrc(g_sRc630Frame.frame, g_sRc630Frame.rxLen))
                {
                    pBlock[i] = g_sRc630Frame.frame[1];
                }
                else
                {
                    state = RC630_STAT_CRC_ERROR;
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
        g_sRc630Frame.frame[pos++] = ISO14443A_CMD_TPZREADBLK;
        g_sRc630Frame.frame[pos++] = addr & 0x7F;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = 0x00;
        g_sRc630Frame.frame[pos++] = pUid->uid[0];
        g_sRc630Frame.frame[pos++] = pUid->uid[1];
        g_sRc630Frame.frame[pos++] = pUid->uid[2];
        g_sRc630Frame.frame[pos++] = pUid->uid[3];
        crc = ISO14443A_TopazCalCrc(g_sRc630Frame.frame, pos);
        g_sRc630Frame.frame[pos++] = (crc >> 0) & 0xFF;
        g_sRc630Frame.frame[pos++] = (crc >> 8) & 0xFF;

        g_sRc630Frame.txLen = pos;

        g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
        g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

        RC630_Config(FALSE, FALSE, FALSE, 0, 0);
        state = RC630_TopazCommand();
        if(state == RC630_STAT_OK)
        {
            if(ISO14443A_TopazCheckCrc(g_sRc630Frame.frame, g_sRc630Frame.rxLen))
            {
                memcpy(pBlock, g_sRc630Frame.frame + 1, ISO14443A_TOPAZ_BLOCK_LEN);
            }
            else
            {
                state = RC630_STAT_CRC_ERROR;
            }
        }
    }

    return state;
}
u8 ISO14443A_TopazWriteBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock)
{
    u8 state = RC630_STAT_OK;
    u16 crc = 0;
    u8 pos = 0;
    if(pUid->sak == ISO14443A_TAGTYPE_TOPAZ96)
    {
        u8 i = 0;
        for(i = 0; i < ISO14443A_TOPAZ_BLOCK_LEN; i++)
        {
            pos = 0;
            g_sRc630Frame.frame[pos++] = ISO14443A_CMD_TPZWE;
            g_sRc630Frame.frame[pos++] = ((addr & 0x0F) << 3) | i;
            g_sRc630Frame.frame[pos++] = pBlock[i];
            g_sRc630Frame.frame[pos++] = pUid->uid[0];
            g_sRc630Frame.frame[pos++] = pUid->uid[1];
            g_sRc630Frame.frame[pos++] = pUid->uid[2];
            g_sRc630Frame.frame[pos++] = pUid->uid[3];
            crc = ISO14443A_TopazCalCrc(g_sRc630Frame.frame, pos);
            g_sRc630Frame.frame[pos++] = (crc >> 0) & 0xFF;
            g_sRc630Frame.frame[pos++] = (crc >> 8) & 0xFF;

            g_sRc630Frame.txLen = pos;

            g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
            g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

            RC630_Config(FALSE, FALSE, FALSE, 0, 0);
            state = RC630_TopazCommand();
            if(state == RC630_STAT_OK)
            {
                if(!ISO14443A_TopazCheckCrc(g_sRc630Frame.frame, g_sRc630Frame.rxLen))
                {
                    state = RC630_STAT_CRC_ERROR;
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
        g_sRc630Frame.frame[pos++] = ISO14443A_CMD_TPZWEBLK;
        g_sRc630Frame.frame[pos++] = addr & 0x7F;
        g_sRc630Frame.frame[pos++] = pBlock[0];
        g_sRc630Frame.frame[pos++] = pBlock[1];
        g_sRc630Frame.frame[pos++] = pBlock[2];
        g_sRc630Frame.frame[pos++] = pBlock[3];
        g_sRc630Frame.frame[pos++] = pBlock[4];
        g_sRc630Frame.frame[pos++] = pBlock[5];
        g_sRc630Frame.frame[pos++] = pBlock[6];
        g_sRc630Frame.frame[pos++] = pBlock[7];
        g_sRc630Frame.frame[pos++] = pUid->uid[0];
        g_sRc630Frame.frame[pos++] = pUid->uid[1];
        g_sRc630Frame.frame[pos++] = pUid->uid[2];
        g_sRc630Frame.frame[pos++] = pUid->uid[3];
        crc = ISO14443A_TopazCalCrc(g_sRc630Frame.frame, pos);
        g_sRc630Frame.frame[pos++] = (crc >> 0) & 0xFF;
        g_sRc630Frame.frame[pos++] = (crc >> 8) & 0xFF;

        g_sRc630Frame.txLen = pos;

        g_sRc630Frame.timeout = ISO14443A_NORMAL_TIME;
        g_sRc630Frame.cmd = RC630_CMD_TRANSCEIVE;

        RC630_Config(FALSE, FALSE, FALSE, 0, 0);
        state = RC630_TopazCommand();
        if(state == RC630_STAT_OK)
        {
            if(ISO14443A_TopazCheckCrc(g_sRc630Frame.frame, g_sRc630Frame.rxLen))
            {
                memcpy(pBlock, g_sRc630Frame.frame + 1, ISO14443A_TOPAZ_BLOCK_LEN);
            }
            else
            {
                state = RC630_STAT_CRC_ERROR;
            }
        }
    }

    return state;
}
