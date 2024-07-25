#include "AnyID_FM17xx_ISO14443A.h"
#include "AnyID_Des.h"

ISO14443A_4 g_sISO1444A4 = {0};

u8 ISO14443A_Init(void)
{
    //关闭天线
    FM17xx_CloseAntenna();
    //因为CRC-A的校验码就是6363，所以这里必须设置成6363，否则以后的CRC校验总是错误
    FM17xx_WriteReg(FM17XX_REG_MODE, 0x3D);                   //CRC 6363
    //不使用定时器，使用自己查询模式
    FM17xx_WriteReg(FM17XX_REG_TMODE, 0x00);
    FM17xx_WriteReg(FM17XX_REG_TRELOADLO, 0xFF);
    FM17xx_WriteReg(FM17XX_REG_TRELOADHI, 0XFF);

    FM17xx_WriteReg(FM17XX_REG_WATERLEVEL, FM17XX_FIFO_LEVEL);


    //系统置于空闲模式
    FM17xx_WriteReg(FM17XX_REG_COMMAND, FM17XX_CMD_IDLE);
    return FM17XX_STAT_OK;
}

u8 ISO14443A_CheckBcc(u8 *pUID)
{
    u8 bcc = 0;

    bcc = pUID[0] ^ pUID[1] ^ pUID[2] ^ pUID[3] ^ pUID[4];
    if(bcc == 0)
    {
        return FM17XX_STAT_OK;
    }
    else
    {
        return FM17XX_STAT_BCC_ERROR;
    }
}

u8 ISO14443A_RequestTag(u8 mode, u16 *pTagType)
{
    u8 state = FM17XX_STAT_OK;

    g_sFm17xxFrame.frame[0] = mode;

    g_sFm17xxFrame.txLen = 1;

    g_sFm17xxFrame.timeout = ISO14443A_INVENTORY_TIME_MIN;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(TRUE, FALSE, FALSE, 7, 0);
    state = FM17xx_Command();
    if(state == FM17XX_STAT_OK)
    {
        *pTagType = g_sFm17xxFrame.frame[1];
        *pTagType <<= 8;
        *pTagType |= g_sFm17xxFrame.frame[0];
    }

    return state;
}


u8 ISO14443A_AnticollisionTag(u8 lv, u8 *pUID)
{
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = lv;
    g_sFm17xxFrame.frame[pos++] = 0x20;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_NORMAL_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, FALSE, FALSE, 0, 0);
    state = FM17xx_Command();
    if(state == FM17XX_STAT_OK)
    {
        state = ISO14443A_CheckBcc(g_sFm17xxFrame.frame);
        if(state == FM17XX_STAT_OK)
        {
            memcpy(pUID, g_sFm17xxFrame.frame, 5);
        }
    }
    return state;
}


u8 ISO14443A_SelectTag(u8 lv, u8 *pUID, u8 *pSak)
{
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = lv;
    g_sFm17xxFrame.frame[pos++] = 0x70;
    memcpy(g_sFm17xxFrame.frame + pos, pUID, 5);
    pos += 5;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_NORMAL_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, TRUE, 0, 0);
    state = FM17xx_Command();
    if(state == FM17XX_STAT_OK)
    {
        *pSak = g_sFm17xxFrame.frame[0];
    }

    return state;
}

u8 ISO14443A_GetUid(ISO14443A_UID *pUid, u8 mode)
{
    u8 state = FM17XX_STAT_OK;
    state = ISO14443A_RequestTag(mode, &(pUid->type));
    if(state == FM17XX_STAT_OK && (pUid->type != ISO14443A_TAGTYPE_TOPAZ) && (pUid->type != ISO14443A_TAGTYPE_FM11RF005))
    {
        u8 uid[ISO14443A_MAX_UID_LEN] = {0};
        state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT0, uid);
        if(state == FM17XX_STAT_OK)
        {
            state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT0, uid, &(pUid->sak));
            if(state == FM17XX_STAT_OK)
            {
                pUid->len = ISO14443A_SIGNAL_UID_LEN;
                if(uid[0] == ISO14443A_UID_CT)
                {
                    memcpy(pUid->uid, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);
                    state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT1, uid);
                    if(state == FM17XX_STAT_OK)
                    {
                        state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT1, uid, &(pUid->sak));
                        if(state == FM17XX_STAT_OK)
                        {
                            pUid->len = ISO14443A_DOUBLE_UID_LEN;
                            if(uid[0] == ISO14443A_UID_CT)
                            {
                                memcpy(pUid->uid + 3, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);

                                state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT2, uid);
                                if(state == FM17XX_STAT_OK)
                                {
                                    state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT2, uid, &(pUid->sak));
                                    if(state == FM17XX_STAT_OK)
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
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = ISO14443A_CMD_HALTA;
    g_sFm17xxFrame.frame[pos++] = 0x00;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_NORMAL_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, TRUE, 0, 0);
    FM17xx_Command();

    return state;
}

u8 ISO14443A_AuthM1(u8 *pUID, u8 authMode, u8 *pKey, u8 blockAddr)
{
    u8 state = FM17XX_STAT_OK;
    u8 reg = 0;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = authMode;
    g_sFm17xxFrame.frame[pos++] = blockAddr;
    memcpy(g_sFm17xxFrame.frame + pos, pKey, ISO14443A_M1_KEY_LEN);
    pos += ISO14443A_M1_KEY_LEN;
    memcpy(g_sFm17xxFrame.frame + pos, pUID, ISO14443A_SIGNAL_UID_LEN);
    pos += ISO14443A_SIGNAL_UID_LEN;
    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_AUTH_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_AUTHENT;

    FM17xx_Config(FALSE, FALSE, FALSE, 0, 0);

    FM17xx_Command();

    reg = FM17xx_ReadReg(FM17XX_REG_STATUS2);
    if((reg & FM17XX_BIT_CRYPTO1ON) == 0)//
    {
        state = FM17XX_STAT_AUTH_ERROR;
    }

    return state;
}

u8 ISO14443A_ReadMifareBlock(u8 blockAddr, u8 *pBlock)
{
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = ISO14443A_CMD_READ;
    g_sFm17xxFrame.frame[pos++] = blockAddr;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_READ_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, TRUE, 0, 0);
    state = FM17xx_Command();

    if(state == FM17XX_STAT_OK)//
    {
        memcpy(pBlock, g_sFm17xxFrame.frame, ISO14443A_M1BLOCK_LEN);
    }

    return state;
}


u8 ISO14443A_WriteMifareBlock16(u8 blockAddr, u8 *pBlock)
{
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = ISO14443A_CMD_WRITE16;
    g_sFm17xxFrame.frame[pos++] = blockAddr;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_WRITE_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, FALSE, 0, 0);
    state = FM17xx_Command();

    if(state == FM17XX_STAT_OK)//
    {
        if(g_sFm17xxFrame.rxLen == 1 && g_sFm17xxFrame.frame[0] == ISO14443A_ACK_OK)
        {
            memcpy(g_sFm17xxFrame.frame, pBlock, ISO14443A_M1BLOCK_LEN);

            g_sFm17xxFrame.txLen = ISO14443A_M1BLOCK_LEN;

            g_sFm17xxFrame.timeout = ISO14443A_WRITE_TIME;
            g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

            state = FM17xx_Command();
            if(state == FM17XX_STAT_OK)//
            {
                if((g_sFm17xxFrame.rxLen != 1) || (g_sFm17xxFrame.frame[0] != ISO14443A_ACK_OK))
                {
                    state = FM17XX_STAT_NAK;
                }
            }
        }

    }

    return state;
}



u8 ISO14443A_WriteMifareBlock4(u8 blockAddr, u8 *pBlock)
{
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = ISO14443A_CMD_WRITE4;
    g_sFm17xxFrame.frame[pos++] = blockAddr;
    memcpy(g_sFm17xxFrame.frame + pos, pBlock, ISO14443A_M0BLOCK_LEN);
    pos += ISO14443A_M0BLOCK_LEN;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_WRITE_TIME * 2;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, FALSE, 0, 0);
    state = FM17xx_Command();

    if(state == FM17XX_STAT_OK)//
    {
        if(g_sFm17xxFrame.rxLen == 1 && g_sFm17xxFrame.frame[0] == ISO14443A_ACK_OK)
        {
            if(state == FM17XX_STAT_OK)//
            {
                if((g_sFm17xxFrame.rxLen != 1) || (g_sFm17xxFrame.frame[0] != ISO14443A_ACK_OK))
                {
                    state = FM17XX_STAT_NAK;
                }
            }
        }
        else
        {
            state = FM17XX_STAT_NAK;
        }

    }

    return state;
}


u8 ISO14443A_Value(u8 opMode, u8 *pDataBuffer, u8 transAddr, u8 valueAddr)
{
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = opMode;
    g_sFm17xxFrame.frame[pos++] = valueAddr;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_VALUE_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, FALSE, 0, 0);
    state = FM17xx_Command();

    if(state == FM17XX_STAT_OK)//
    {
        if(g_sFm17xxFrame.rxLen == 1 && g_sFm17xxFrame.frame[0] == ISO14443A_ACK_OK)
        {
            memcpy(g_sFm17xxFrame.frame, pDataBuffer, ISO14443A_M1VALUE_LEN);

            g_sFm17xxFrame.txLen = ISO14443A_M1VALUE_LEN;

            g_sFm17xxFrame.timeout = ISO14443A_VALUE_TIME;
            g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

            state = FM17xx_Command();
            if(state == FM17XX_STAT_OK)//
            {
                state = FM17XX_STAT_NAK;
            }
            else
            {
                pos = 0;
                g_sFm17xxFrame.frame[pos++] = ISO14443A_CMD_TRANSFER;
                g_sFm17xxFrame.frame[pos++] = transAddr;

                g_sFm17xxFrame.txLen = pos;

                g_sFm17xxFrame.timeout = ISO14443A_VALUE_TIME;
                g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

                FM17xx_Config(FALSE, TRUE, FALSE, 0, 0);
                state = FM17xx_Command();
                if(state == FM17XX_STAT_OK)//
                {
                    if((g_sFm17xxFrame.rxLen != 1) || (g_sFm17xxFrame.frame[0] != ISO14443A_ACK_OK))
                    {
                        state = FM17XX_STAT_NAK;
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
    u8 state = FM17XX_STAT_OK;
    u8 pos = 0;

    g_sFm17xxFrame.frame[pos++] = ISO14443A_CMD_RATS;
    g_sFm17xxFrame.frame[pos++] = ISO14443A_FSDI_FSD | ISO14443A_CID;

    g_sFm17xxFrame.txLen = pos;

    g_sFm17xxFrame.timeout = ISO14443A_WRITE_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, TRUE, 0, 0);
    state = FM17xx_Command();

    if(state == FM17XX_STAT_OK)//
    {
        u8 temp = 0;

        *pLen = g_sFm17xxFrame.rxLen;
        memcpy(pAts, g_sFm17xxFrame.frame, g_sFm17xxFrame.rxLen);

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
    u8 state = FM17XX_STAT_OK;
    u8 pcb = 0;

    memcpy(g_sFm17xxFrame.frame, pApdu, len);
    g_sFm17xxFrame.txLen = len;

    g_sFm17xxFrame.timeout = g_sISO1444A4.fwt; //fwt单位是us
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, TRUE, 0, 0);
    //因为有些处理需要比较长的时间，所以PICC有时会发送SBLOCK请求延时
    //如果响应帧是SBLOCK中的WTX，那么将WTX请求直接发送到PICC，一直到接收真正的响应帧
    do{
        *pAskLen = 0;

        state = FM17xx_Command();
        if(state == FM17XX_STAT_OK)
        {
            *pAskLen = g_sFm17xxFrame.rxLen;
            memcpy(pAsk, g_sFm17xxFrame.frame, g_sFm17xxFrame.rxLen);

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
                memcpy(g_sFm17xxFrame.frame, pAsk, *pAskLen);
                g_sFm17xxFrame.rxLen = *pAskLen;
            }
            else
            {
                state = FM17XX_STAT_ERROR;
            }
        }
    }while((state == FM17XX_STAT_OK) && (pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX)));

    return state;
}

u8 ISO14443A_DirectCommand(u8 *pFrame, u8 len, u8 *pAsk, u8 *pAskLen)
{
    u8 state = FM17XX_STAT_OK;

    memcpy(g_sFm17xxFrame.frame, pFrame, len);
    g_sFm17xxFrame.txLen = len;

    g_sFm17xxFrame.timeout = ISO14443A_WRITE_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, FALSE, FALSE, 0, 0);

    state = FM17xx_Command();
    if(state == FM17XX_STAT_OK)
    {
        *pAskLen = g_sFm17xxFrame.rxLen;
        memcpy(pAsk, g_sFm17xxFrame.frame, g_sFm17xxFrame.rxLen);
    }

    return state;
}

u8 ISO14443A_Dtu(u8 *pTxFrame, u8 txLen, u8 txBit, u8 *pRxBit, u32 timeout)
{
    u8 state = FM17XX_STAT_OK;

    memcpy(g_sFm17xxFrame.frame, pTxFrame, txLen);
    g_sFm17xxFrame.txLen = txLen;

    g_sFm17xxFrame.timeout = timeout;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, FALSE, FALSE, txBit, (*pRxBit) & 0x07);

    state = FM17xx_Command();
    if(state == FM17XX_STAT_OK)
    {
        *pRxBit = FM17xx_ReadReg(FM17XX_REG_BITFRAMING) & 0x07;
    }

    return state;
}


u8 ISO14443A_AuthUltralightC(u8 *pKey)
{
    u8 state = FM17XX_STAT_OK;
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

    g_sFm17xxFrame.frame[txLen++] = ISO14443A_CMD_AUTHM0U2S1;
    g_sFm17xxFrame.frame[txLen++] = 0x00;
    g_sFm17xxFrame.txLen = txLen;

    g_sFm17xxFrame.timeout = ISO14443A_NORMAL_TIME;
    g_sFm17xxFrame.cmd = FM17XX_CMD_TRANSCEIVE;

    FM17xx_Config(FALSE, TRUE, TRUE, 0, 0);

    state = FM17xx_Command();
    if(state == FM17XX_STAT_OK)
    {
        g_sFm17xxFrame.frame[0] = ISO14443A_CMD_AUTHM0U2S2;
        //解密rndb
        Des_DecryptAnyLength(g_sFm17xxFrame.frame + 1, 8, rndb, 0);
        Des_EncryptAnyLength(rndb, 8, temp, 1);
        Des_DecryptAnyLength(temp, 8, rndb, 0);
        //CBC模式
        a_xor(rndb, iv, 8);
        memcpy(iv, g_sFm17xxFrame.frame + 1, 8);

        //rndb循环左移8bit得到rndb1
        t = rndb[0];
        memcpy(rndb, rndb + 1, 7);
        rndb[7] = t;

        //CBC模式
        a_xor(rnda, iv, 8);
        //计算ek(rnda)
        Des_EncryptAnyLength(rnda, 8, g_sFm17xxFrame.frame + 1, 0);
        Des_DecryptAnyLength(g_sFm17xxFrame.frame + 1, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sFm17xxFrame.frame + 1, 0);
        memcpy(iv, g_sFm17xxFrame.frame + 1, 8);

        //CBC模式
        a_xor(rndb, iv, 8);
        //计算ek(rndb1)
        Des_EncryptAnyLength(rndb, 8, g_sFm17xxFrame.frame + 9, 0);
        Des_DecryptAnyLength(g_sFm17xxFrame.frame + 9, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sFm17xxFrame.frame + 9, 0);
        memcpy(iv, g_sFm17xxFrame.frame + 9, 8);

        txLen = 1 + 8 + 8;
        g_sFm17xxFrame.txLen = txLen;

        state = FM17xx_Command();
        if(state == FM17XX_STAT_OK)
        {
            //解密rnda
            Des_DecryptAnyLength(g_sFm17xxFrame.frame + 1, 8, rnda, 0);
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
                state = FM17XX_STAT_AUTH_ERROR;
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
    int rlt = FM17XX_STAT_OK;

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
        rlt = FM17XX_STAT_ERROR;
    }

    return rlt;

}
