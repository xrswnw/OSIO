#include "AnyID_ST25R391x_ISO14443A.h"
#include "AnyID_Des.h"

ISO14443A_4 g_sISO1444A4 = {0};

u8 ISO14443A_Init(void)
{
    u8 err = ST25R3916_STAT_OK;
    
    //读写器模式-14443a-OOK-nfc ar 关闭
    St25r3916_WriteReg(ST25R3916_REG_MODE, ST25R3916_REG_MODE_om_iso14443a);;
    //tx-106 rx-106
    St25r3916_WriteReg(ST25R3916_REG_BIT_RATE, ST25R3916_REG_BIT_RATE_txrate_106 | ST25R3916_REG_BIT_RATE_rxrate_106);
    //辅助信息
    St25r3916_WriteReg(ST25R3916_REG_AUX, 0x00);
    // 禁用EMD抑制
    St25r3916_ClearRegBits(ST25R3916_REG_EMD_SUP_CONF, ST25R3916_REG_EMD_SUP_CONF_emd_emv); //emvco标准
    //config rx
    //AM通道-600K low pass-
    /*St25r3916_WriteReg(ST25R3916_REG_RX_CONF1, 0x13);  
    //AM/PM demoudle peak detector
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF2, 0x2D);
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF3, 0x00);
    St25r3916_WriteReg(ST25R3916_REG_RX_CONF4, 0x00);
    St25r3916_WriteReg(ST25R3916_REG_P2P_RX_CONF, 0x0C);
    St25r3916_WriteReg(ST25R3916_REG_CORR_CONF1, 0x13);
    St25r3916_WriteReg(ST25R3916_REG_CORR_CONF2, 0x01);

    St25r3916_WriteReg(ST25R3916_REG_AUX_MOD, 0x10);
    St25r3916_WriteReg(ST25R3916_REG_RES_AM_MOD, 0x80);
    //mask定时器，默认8*4.72----不起作用
    St25r3916_WriteReg(ST25R3916_REG_MASK_RX_TIMER, 0x08);  */
    
    return err;
}

#define ISO14443A_PRESET_VALUE      0x6363
#define ISO14443A_POLYNOMIAL        0x8408   //x^16 + x^12 + x^5 + 1
u16 ISO14443A_CalCrc(u16 *pFrame, u8 len)
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

u16 ISO14443A_AddOddBit(u8 dat)
{
    u8 odd = 0;
    u16 temp = 0;
    u8 i = 0;

    temp = dat & 0xFF;
    for(i = 0; i < 8; i++)
    {
        odd ^= (temp & 0x01);
        temp >>= 1;
    }
    temp = dat & 0xFF;
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

u8 ISO14443A_CheckBcc(u8 *pUID)
{
    u8 bcc = 0;

    bcc = pUID[0] ^ pUID[1] ^ pUID[2] ^ pUID[3] ^ pUID[4];
    if(bcc == 0)
    {
        return ST25R3916_STAT_OK;
    }
    else
    {
        return ST25R3916_STAT_BCC_ERROR;
    }
}

BOOL ISO14443A_CheckCrc(u16 *pFrame, u8 len)
{
    u16 crc1 = 0, crc2 = 0;
    BOOL b = FALSE;
    if(len > 2)
    {
        crc1 = ISO14443A_CalCrc(pFrame, len - 2);
        crc2 = pFrame[len - 2] & 0xFF;
        crc2 += ((pFrame[len - 1] << 8) & 0xFF00);

        if(crc1 == crc2)
        {
            b = TRUE;
        }
    }

    return b;
}


#define ST25R3916_RX_FRAME_TIMEOUT      1000        //1000*16us = 16ms
#define ST25R3916_TX_FRAME_TICK         100         //100 * 16us = 1.6ms

u8 ISO14443A_Cmd(u8 cmd, ST25R3916_FRAME *pFrame)
{
    u8 state = ST25R3916_ERR_NOMSG;
    u32 irqReg = 0, irqFlag = 0;
    u32 tick = 0;
    
    St25r3916_WriteCmd(ST25R3916_CMD_CLEAR_FIFO);
    irqReg = ST25R3916_IRQ_MASK_NONE; 
    St25r3916_WriteRegs(ST25R3916_REG_IRQ_MASK_MAIN, (u8 *)&irqReg, ST25R3916_INT_REGS_LEN);    //使能/禁止中断
    St25r3916_ReadRegs(ST25R3916_REG_IRQ_MAIN, (u8 *)&irqReg, ST25R3916_INT_REGS_LEN);          //读取寄存器，清空所有寄存器中断标志
    
    St25r3916_FlushFifo();                              //清空fifo
    St25r3916_WriteFifo(pFrame->frame, pFrame->txLen);  //写入fifo
    if(pFrame->txLastBitsNum)
    {
        pFrame->txLen--;
    }
    St25r3916_WriteReg(ST25R3916_REG_NUM_TX_BYTES1, (pFrame->txLen & 0xE0) >> 5);
    St25r3916_WriteReg(ST25R3916_REG_NUM_TX_BYTES2, ((pFrame->txLen & 0x1F) << 3) | (pFrame->txLastBitsNum & 0x07));
    
    St25r3916_WriteCmd(cmd);
    
    tick = (pFrame->txLen + 1) * ST25R3916_TX_FRAME_TICK;
    
    while(tick) //发送
    {
        St25r3916_Delay16us(1);
        //这里耗时基本是10us
        St25r3916_ReadRegs(ST25R3916_REG_IRQ_MAIN, (u8 *)&irqReg, ST25R3916_INT_REGS_LEN);
        irqFlag |= irqReg;
        if(irqFlag & ST25R3916_IRQ_MASK_TXE)
        {
            irqFlag &= (~ST25R3916_IRQ_MASK_TXE);
            St25r3916_FlushFifo();                          //清空fifo
            break;
        }
        tick--;
    }
    
    //如果发送完成，进入接收
    pFrame->rxLen = 0;
    if(tick > 0)
    {
        tick = pFrame->norspTime >> 4;  //注意这里要除16，协议都是用us作为单位
        while(tick)
        {
            St25r3916_Delay16us(1);
            //这里耗时基本是10us
            St25r3916_ReadRegs(ST25R3916_REG_IRQ_MAIN, (u8 *)&irqReg, ST25R3916_INT_REGS_LEN);
            irqFlag |= irqReg;
            if(irqFlag & ST25R3916_IRQ_MASK_RXE)          //接收完成
            {
                break;
            }
            tick--;
        }
        
        if(tick > 0)
        {
            u16 rxCodeLen = 0;
            rxCodeLen = St25r3916_ReadReg(ST25R3916_REG_FIFO_STATUS2);
            rxCodeLen &= 0xC0;
            rxCodeLen <<= 2;
            rxCodeLen |= St25r3916_ReadReg(ST25R3916_REG_FIFO_STATUS1);
            rxCodeLen &= 0x3FF;
            
            pFrame->rxLen = rxCodeLen;
            St25r3916_ReadFifo(pFrame->frame, rxCodeLen);
            state = ST25R3916_STAT_OK;
        }
    }
    
    return state;
}

u8 ISO14443A_RequestTag(u8 mode, u16 *pTagType)
{
    u8 state = ST25R3916_STAT_OK;
    
    memset(&g_sSt25r3916Frame, 0, sizeof(ST25R3916_FRAME));
    memset(&g_sISO1444A4, 0, sizeof(ISO14443A_4));
    
    g_sSt25r3916Frame.frame[0] = mode;
    g_sSt25r3916Frame.txLen = 1;
    g_sSt25r3916Frame.txLastBitsNum = 7;
    
    g_sSt25r3916Frame.norspTime = ISO14443A_INVENTORY_TIME_MIN;
    
    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);                                  //不需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位    
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位
    
    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
        *pTagType = g_sSt25r3916Frame.frame[1];
        *pTagType <<= 8;
        *pTagType |= g_sSt25r3916Frame.frame[0];
    }
    return state;
}

u8 ISO14443A_AnticollisionTag(u8 lv, u8 *pUID)
{
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = lv;
    g_sSt25r3916Frame.frame[pos++] = 0x20;
    g_sSt25r3916Frame.txLen = pos;
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
    
    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位
    
    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
        state = ISO14443A_CheckBcc(g_sSt25r3916Frame.frame);
        if(state == ST25R3916_STAT_OK)
        {
            memcpy(pUID, g_sSt25r3916Frame.frame, 5);
        }
    }
    return state;
}

u8 ISO14443A_SelectTag(u8 lv, u8 *pUID, u8 *pSak)
{
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = lv;
    g_sSt25r3916Frame.frame[pos++] = 0x70;
    memcpy(g_sSt25r3916Frame.frame + pos, pUID, 5);
    pos += 5;
        
    g_sSt25r3916Frame.txLen = pos;
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
       *pSak = g_sSt25r3916Frame.frame[0];
    }

    return state;
}

u8 ISO14443A_GetUid(ISO14443A_UID *pUid, u8 mode)
{
    u8 state = ST25R3916_STAT_OK;
    state = ISO14443A_RequestTag(mode, &(pUid->type));
    if(state == ST25R3916_STAT_OK && (pUid->type != ISO14443A_TAGTYPE_TOPAZ) && (pUid->type != ISO14443A_TAGTYPE_FM11RF005))
    {
        u8 uid[ISO14443A_MAX_UID_LEN] = {0};
        state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT0, uid);
        if(state == ST25R3916_STAT_OK)
        {
            state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT0, uid, &(pUid->sak));
            if(state == ST25R3916_STAT_OK)
            {
                pUid->len = ISO14443A_SIGNAL_UID_LEN;
                if(uid[0] == ISO14443A_UID_CT)
                {
                    memcpy(pUid->uid, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);
                    state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT1, uid);
                    if(state == ST25R3916_STAT_OK)
                    {
                        state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT1, uid, &(pUid->sak));
                        if(state == ST25R3916_STAT_OK)
                        {
                            pUid->len = ISO14443A_DOUBLE_UID_LEN;
                            if(uid[0] == ISO14443A_UID_CT)
                            {
                                memcpy(pUid->uid + 3, uid + 1, ISO14443A_SIGNAL_UID_LEN - 1);

                                state = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT2, uid);
                                if(state == ST25R3916_STAT_OK)
                                {
                                    state = ISO14443A_SelectTag(ISO14443A_CMD_SELECT2, uid, &(pUid->sak));
                                    if(state == ST25R3916_STAT_OK)
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
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_HALTA;
    g_sSt25r3916Frame.frame[pos++] = 0x00;
    g_sSt25r3916Frame.txLen = pos;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
    
    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位
    
    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);

    return state;
}


u8 ISO14443A_AuthM1(u8 *pUID, u8 authMode, u8 *pKey, u8 blockAddr)
{
    u8 state = ST25R3916_STAT_OK;
//    u8 txLen = 0;
//    u32 iv = 0;
//    u16 crc = 0;
//    int i = 0;
//    u16 frame[18] = {0};
//
//
//    //三重认证第一步：请求认证，获取随机数
//    txLen = 0;
//    frame[txLen++] = authMode;
//    frame[txLen++] = blockAddr;
//    crc = ISO14443A_CalCrc(frame, txLen);
//    frame[txLen++] = (crc >> 0) & 0xFF;
//    frame[txLen++] = (crc >> 8) & 0xFF;
//
//    for(i = 0; i < txLen; i++)
//    {
//        frame[i] = ISO14443A_AddOddBit(frame[i]);
//    }
//    if(g_sISO14443AMifareAuth.crypto1On == 1)
//    {
//        Mifare_Cipher(frame, txLen, 0);
//    }
//
//    for(i = 0; i < txLen; i++)
//    {
//        g_sSt25r3916Frame.frame[(i << 1) + 0] = (frame[i] >> 0) & 0xFF;
//        g_sSt25r3916Frame.frame[(i << 1) + 1] = (frame[i] >> 1) & 0x80;
//    }
//    txLen = txLen << 1;
//    
//    g_sSt25r3916Frame.txLen = txLen;
//    g_sSt25r3916Frame.txLastBitsNum = 0;
//
//    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME * 10;
//    
//    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);                                  //不需要接收CRC
//    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
//    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位
//    
//    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//    if(state == ST25R3916_STAT_OK)
//    {
//        if(g_sSt25r3916Frame.rxLen)
//        {
//            g_sISO14443AMifareAuth.lfsr = *((u64 *)pKey);
//            g_sISO14443AMifareAuth.uid = ARRAY_TO_UINT32(pUID);
//            g_sISO14443AMifareAuth.nr = 0xFFFFFFFF;
//
//            for(i = 0; i < 4; i++)
//            {
//                frame[i] = g_sSt25r3916Frame.frame[i * 2 + 1] & 0x80;
//                frame[i] <<= 1;
//                frame[i] |= (g_sSt25r3916Frame.frame[i * 2 + 0] & 0xFF);
//            }
//            //获取随机数
//            g_sISO14443AMifareAuth.nt = ARRAY_TO_UINT32(frame);
//            if(g_sISO14443AMifareAuth.crypto1On == 1)
//            {
//                //如果是嵌套认证，需要解密一次
//                iv = g_sISO14443AMifareAuth.uid ^ g_sISO14443AMifareAuth.nt;
//                g_sISO14443AMifareAuth.nt = g_sISO14443AMifareAuth.nt ^ MIFARE_update_word(iv, 1);
//            }
//        }
//        else
//        {
//            state = ST25R3916_STAT_TRANS_ERROR;
//        }
//    }
//
//    if(g_sISO14443AMifareAuth.crypto1On == 0)
//    {
//        //初始化编码器和ks1
//        iv = g_sISO14443AMifareAuth.uid ^ g_sISO14443AMifareAuth.nt;
//        MIFARE_update_word(iv, 0);
//    }
//    //nr xor ks1，并生成ks2
//    UINT32_TO_ARRAY_WITH_PARITY(g_sISO14443AMifareAuth.nr, frame);
//    for(i = 3; i >= 0; i--)
//    {                           /* Same as in MIFARE_update_word, but with added parity */
//        frame[3 - i] = frame[3 - i] ^ MIFARE_update_byte((g_sISO14443AMifareAuth.nr >> (i * 8)) & 0xff, 0);
//        frame[3 - i] ^= mf20(g_sISO14443AMifareAuth.lfsr) << 8;
//    }
//
//    //suc2(nt) xor ks2，并生成ks3，以后的编码和解码都用ks3完成
//    g_sISO14443AMifareAuth.ar = prng_next(64);
//    UINT32_TO_ARRAY_WITH_PARITY(g_sISO14443AMifareAuth.ar, frame + 4);
//    for(i = 0; i < 4; i++)
//    {
//        frame[i + 4] = frame[i + 4] ^ MIFARE_update_byte(0, 0);
//        frame[i + 4] = frame[i + 4] ^ (mf20(g_sISO14443AMifareAuth.lfsr) << 8);
//    }
//
//    txLen = 0;
//    for(i = 0; i < 8; i++)
//    {
//        g_sSt25r3916Frame.frame[txLen++] = frame[i] & 0xFF;
//        g_sSt25r3916Frame.frame[txLen++] = (frame[i] >> 1) & 0x80;
//    }
//
//    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME * 10;
//    g_sSt25r3916Frame.txLen = txLen;
//    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//    if(state == ST25R3916_STAT_OK)
//    {
//        if(g_sSt25r3916Frame.rxLen)
//        {
//            u32 ta1 = 0, ta2 = 0;
//            for(i = 0; i < 4; i++)
//            {
//                frame[i] = g_sSt25r3916Frame.frame[i * 2 + 1] & 0x80;
//                frame[i] <<= 1;
//                frame[i] |= (g_sSt25r3916Frame.frame[i * 2 + 0] & 0xFF);
//            }
//            ta1 = ARRAY_TO_UINT32(frame);
//            ta2 = prng_next(96) ^ MIFARE_update_word(0, 0);
//            if(ta1 == ta2)
//            {
//                g_sISO14443AMifareAuth.crypto1On = 1;
//            }
//            else
//            {
//                g_sISO14443AMifareAuth.crypto1On = 0;
//            }
//        }
//        else
//        {
//            state = ST25R3916_STAT_TRANS_ERROR;
//        }
//    }
//
    return state;
}

u8 ISO14443A_ReadMifareBlock(u8 blockAddr, u8 *pBlock)
{
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_READ;
    g_sSt25r3916Frame.frame[pos++] = blockAddr;
    g_sSt25r3916Frame.txLen = pos;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
       memcpy(pBlock, g_sSt25r3916Frame.frame, ISO14443A_M1BLOCK_LEN);
    }

    return state;
}


u8 ISO14443A_WriteMifareBlock16(u8 blockAddr, u8 *pBlock)
{
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_WRITE16;
    g_sSt25r3916Frame.frame[pos++] = blockAddr;
    g_sSt25r3916Frame.txLen = pos;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_WRITE_TIME;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
       if(g_sSt25r3916Frame.rxLen == 1 && g_sSt25r3916Frame.frame[0] == ISO14443A_ACK_OK)
       {
           memcpy(g_sSt25r3916Frame.frame, pBlock, ISO14443A_M1BLOCK_LEN);    
           g_sSt25r3916Frame.txLen = ISO14443A_M1BLOCK_LEN;
            
           g_sSt25r3916Frame.norspTime = ISO14443A_WRITE_TIME;

           state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
           if(state == ST25R3916_STAT_OK)//
           {
               if((g_sSt25r3916Frame.rxLen != 1) || (g_sSt25r3916Frame.frame[0] != ISO14443A_ACK_OK))
               {
                   state = ST25R3916_STAT_NAK;
               }
           }
       }
   }

   return state;
}


u8 ISO14443A_WriteMifareBlock4(u8 blockAddr, u8 *pBlock)
{
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_WRITE4;
    g_sSt25r3916Frame.frame[pos++] = blockAddr;
    memcpy(g_sSt25r3916Frame.frame + pos, pBlock, ISO14443A_M0BLOCK_LEN);
    pos += ISO14443A_M0BLOCK_LEN;
    g_sSt25r3916Frame.txLen = pos;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_WRITE_TIME * 2;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)//
    {
        if(g_sSt25r3916Frame.rxLen == 1 && g_sSt25r3916Frame.frame[0] == ISO14443A_ACK_OK)
        {
            if(state == ST25R3916_STAT_OK)//
            {
                if((g_sSt25r3916Frame.rxLen != 1) || (g_sSt25r3916Frame.frame[0] != ISO14443A_ACK_OK))
                {
                    state = ST25R3916_STAT_NAK;
                }
            }
        }
        else
        {
            state = ST25R3916_STAT_NAK;
        }

    }

    return state;
}


u8 ISO14443A_Value(u8 opMode, u8 *pDataBuffer, u8 transAddr, u8 valueAddr)
{
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = opMode;
    g_sSt25r3916Frame.frame[pos++] = valueAddr;
    g_sSt25r3916Frame.txLen = pos;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_VALUE_TIME;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);                           //不需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)//
    {
        if(g_sSt25r3916Frame.rxLen == 1 && g_sSt25r3916Frame.frame[0] == ISO14443A_ACK_OK)
        {
            memcpy(g_sSt25r3916Frame.frame, pDataBuffer, ISO14443A_M1VALUE_LEN);
            g_sSt25r3916Frame.txLen = ISO14443A_M1VALUE_LEN;

            g_sSt25r3916Frame.norspTime = ISO14443A_VALUE_TIME;

            state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
            if(state == ST25R3916_STAT_OK)//
            {
                state = ST25R3916_STAT_NAK;
            }
            else
            {
                pos = 0;
                g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_TRANSFER;
                g_sSt25r3916Frame.frame[pos++] = transAddr;

                g_sSt25r3916Frame.txLen = pos;

                g_sSt25r3916Frame.norspTime = ISO14443A_VALUE_TIME;

                state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
                if(state == ST25R3916_STAT_OK)//
                {
                    if((g_sSt25r3916Frame.rxLen != 1) || (g_sSt25r3916Frame.frame[0] != ISO14443A_ACK_OK))
                    {
                        state = ST25R3916_STAT_NAK;
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
    u8 state = ST25R3916_STAT_OK;
    u8 pos = 0;

    g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_RATS;
    g_sSt25r3916Frame.frame[pos++] = ISO14443A_FSDI_FSD | ISO14443A_CID;
    g_sSt25r3916Frame.txLen = pos;

    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_WRITE_TIME;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)//
    {
        u8 temp = 0;

        *pLen = g_sSt25r3916Frame.rxLen;
        memcpy(pAts, g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen);

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
    u8 state = ST25R3916_STAT_OK;
    u8 pcb = 0;

    memcpy(g_sSt25r3916Frame.frame, pApdu, len);
    g_sSt25r3916Frame.txLen = len;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = g_sISO1444A4.fwt; //fwt单位是us

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    //因为有些处理需要比较长的时间，所以PICC有时会发送SBLOCK请求延时
    //如果响应帧是SBLOCK中的WTX，那么将WTX请求直接发送到PICC，一直到接收真正的响应帧
    do{
        *pAskLen = 0;

        state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
        if(state == ST25R3916_STAT_OK)
        {
            *pAskLen = g_sSt25r3916Frame.rxLen;
            memcpy(pAsk, g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen);

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
                memcpy(g_sSt25r3916Frame.frame, pAsk, *pAskLen);
                g_sSt25r3916Frame.rxLen = *pAskLen;
            }
            else
            {
                state = ST25R3916_STAT_ERROR;
            }
        }
    }while((state == ST25R3916_STAT_OK) && (pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX)));

    return state;
}

u8 ISO14443A_DirectCommand(u8 *pFrame, u8 len, u8 *pAsk, u8 *pAskLen)
{
    u8 state = ST25R3916_STAT_OK;

    memcpy(g_sSt25r3916Frame.frame, pFrame, len);
    g_sSt25r3916Frame.txLen = len;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_WRITE_TIME;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);                            //不需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
        *pAskLen = g_sSt25r3916Frame.rxLen;
        memcpy(pAsk, g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen);
    }

    return state;
}

u8 ISO14443A_Dtu(u8 *pTxFrame, u8 txLen, u8 txBit, u8 *pRxBit, u32 timeout)
{
    u8 state = ST25R3916_STAT_OK;

    memcpy(g_sSt25r3916Frame.frame, pTxFrame, txLen);
    g_sSt25r3916Frame.txLen = txLen;

    g_sSt25r3916Frame.txLastBitsNum = txBit;

    g_sSt25r3916Frame.norspTime = timeout;

    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);                            //不需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
        *pRxBit = g_sSt25r3916Frame.rxLen;
    }

    return state;
}


u8 ISO14443A_AuthUltralightC(u8 *pKey)
{
    u8 state = ST25R3916_STAT_OK;
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

    g_sSt25r3916Frame.frame[txLen++] = ISO14443A_CMD_AUTHM0U2S1;
    g_sSt25r3916Frame.frame[txLen++] = 0x00;
    g_sSt25r3916Frame.txLen = txLen;
    
    g_sSt25r3916Frame.txLastBitsNum = 0;

    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
    
    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, 0);                                  //需要接收CRC
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par_off);    //发送校验位
    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par_off);    //接收校验位

    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
    if(state == ST25R3916_STAT_OK)
    {
        g_sSt25r3916Frame.frame[0] = ISO14443A_CMD_AUTHM0U2S2;
        //解密rndb
        Des_DecryptAnyLength(g_sSt25r3916Frame.frame + 1, 8, rndb, 0);
        Des_EncryptAnyLength(rndb, 8, temp, 1);
        Des_DecryptAnyLength(temp, 8, rndb, 0);
        //CBC模式
        a_xor(rndb, iv, 8);
        memcpy(iv, g_sSt25r3916Frame.frame + 1, 8);

        //rndb循环左移8bit得到rndb1
        t = rndb[0];
        memcpy(rndb, rndb + 1, 7);
        rndb[7] = t;

        //CBC模式
        a_xor(rnda, iv, 8);
        //计算ek(rnda)
        Des_EncryptAnyLength(rnda, 8, g_sSt25r3916Frame.frame + 1, 0);
        Des_DecryptAnyLength(g_sSt25r3916Frame.frame + 1, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sSt25r3916Frame.frame + 1, 0);
        memcpy(iv, g_sSt25r3916Frame.frame + 1, 8);

        //CBC模式
        a_xor(rndb, iv, 8);
        //计算ek(rndb1)
        Des_EncryptAnyLength(rndb, 8, g_sSt25r3916Frame.frame + 9, 0);
        Des_DecryptAnyLength(g_sSt25r3916Frame.frame + 9, 8, temp, 1);
        Des_EncryptAnyLength(temp, 8, g_sSt25r3916Frame.frame + 9, 0);
        memcpy(iv, g_sSt25r3916Frame.frame + 9, 8);

        txLen = 1 + 8 + 8;
        g_sSt25r3916Frame.txLen = txLen;

        state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITH_CRC, &g_sSt25r3916Frame);
        if(state == ST25R3916_STAT_OK)
        {
            //解密rnda
            Des_DecryptAnyLength(g_sSt25r3916Frame.frame + 1, 8, rnda, 0);
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
                state = ST25R3916_STAT_AUTH_ERROR;
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
    int rlt = ST25R3916_STAT_OK;

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
        rlt = ST25R3916_STAT_ERROR;
    }

    return rlt;

}


//#define POLYNOMIAL                      0x8408   //x^16 + x^12 + x^5 + 1
//#define PRESET_VALUE                    0xFFFF
//u16 ISO14443A_TopazCalCrc(u8 *pFrame, u8 len)
//{
//    u16 crc = 0;
//    u8 i = 0, j = 0;
//    crc = PRESET_VALUE;
//    for(i = 0; i < len; i++)
//    {
//        crc = crc ^ pFrame[i];
//        for(j = 0; j < 8; j++)
//        {
//            if(crc & 0x0001)
//            {
//                crc = (crc >> 1) ^ POLYNOMIAL;
//            }
//            else
//            {
//                crc = (crc >> 1);
//            }
//        }
//    }
//    crc = ~crc;
//    return crc;
//}
//
//BOOL ISO14443A_TopazCheckCrc(u8 *pFrame, u8 len)
//{
//    u16 crc = 0;
//    BOOL b = FALSE;
//    if(len > 2)
//    {
//        crc = ISO14443A_TopazCalCrc(pFrame, len - 2);
//        if(pFrame[len - 2] == ((crc >> 0) & 0xFF) && pFrame[len - 1] == ((crc >> 8) & 0xFF))
//        {
//            b = TRUE;
//        }
//    }
//
//    return b;
//}
//
//
//u8 ISO14443A_TopazGetUid(ISO14443A_UID *pUid, u8 mode)
//{
//    u16 crc = 0;
//    u8 pos = 0;
//    u8 state = ST25R3916_STAT_OK;
//
//    g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_TPZRID;
//    g_sSt25r3916Frame.frame[pos++] = 0x00;
//    g_sSt25r3916Frame.frame[pos++] = 0x00;
//    g_sSt25r3916Frame.frame[pos++] = 0x00;
//    g_sSt25r3916Frame.frame[pos++] = 0x00;
//    g_sSt25r3916Frame.frame[pos++] = 0x00;
//    g_sSt25r3916Frame.frame[pos++] = 0x00;
//    crc = ISO14443A_TopazCalCrc(g_sSt25r3916Frame.frame, pos);
//    g_sSt25r3916Frame.frame[pos++] = (crc >> 0) & 0xFF;
//    g_sSt25r3916Frame.frame[pos++] = (crc >> 8) & 0xFF;
//    
//    g_sSt25r3916Frame.txLen = pos;
//    
//    g_sSt25r3916Frame.txLastBitsNum = 0;
//
//    g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
//
//    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);                            //不需要接收CRC
//    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par);    //不发送校验位
//    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par);    //不接收校验位
//
//    //state = RC663_TopazCommand();
//    state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//    if(state == ST25R3916_STAT_OK)
//    {
//        if(ISO14443A_TopazCheckCrc(g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen))
//        {
//            u8 i = 0;
//            memcpy(pUid->uid, g_sSt25r3916Frame.frame + 2, 4);
//            pUid->len = 4;
//            pUid->sak = g_sSt25r3916Frame.frame[0]; //hr0
//            for(i = 0; i < 3; i++)
//            {
//                pos = 0;
//                g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_TPZREAD;
//                g_sSt25r3916Frame.frame[pos++] = 4 + i;
//                g_sSt25r3916Frame.frame[pos++] = 0x00;
//                g_sSt25r3916Frame.frame[pos++] = pUid->uid[0];
//                g_sSt25r3916Frame.frame[pos++] = pUid->uid[1];
//                g_sSt25r3916Frame.frame[pos++] = pUid->uid[2];
//                g_sSt25r3916Frame.frame[pos++] = pUid->uid[3];
//                crc = ISO14443A_TopazCalCrc(g_sSt25r3916Frame.frame, pos);
//                g_sSt25r3916Frame.frame[pos++] = (crc >> 0) & 0xFF;
//                g_sSt25r3916Frame.frame[pos++] = (crc >> 8) & 0xFF;
//
//                g_sSt25r3916Frame.txLen = pos;
//
//                g_sSt25r3916Frame.norspTime = ISO14443A_WRITE_TIME;
//
//                //state = RC663_TopazCommand();
//                state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//                if(state == ST25R3916_STAT_OK)
//                {
//                    if(ISO14443A_TopazCheckCrc(g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen))
//                    {
//                        pUid->uid[pUid->len++] = g_sSt25r3916Frame.frame[1]; //addr data
//                    }
//                    else
//                    {
//                        state = ST25R3916_STAT_CRC_ERROR;
//                        break;
//                    }
//                }
//                else
//                {
//                    break;
//                }
//            }
//        }
//        else
//        {
//            state = ST25R3916_STAT_CRC_ERROR;
//        }
//    }
//    return state;
//}
//
//u8 ISO14443A_TopazReadBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock)
//{
//    u8 state = ST25R3916_STAT_OK;
//    u16 crc = 0;
//    u8 pos = 0;
//    
//    g_sSt25r3916Frame.txLastBitsNum = 0;
//    
//    St25r3916_SetRegBits(ST25R3916_REG_AUX, ST25R3916_REG_AUX_no_crc_rx, ST25R3916_REG_AUX_no_crc_rx);                            //不需要接收CRC
//    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_tx_par, ST25R3916_REG_ISO14443A_NFC_no_tx_par);    //不发送校验位
//    St25r3916_SetRegBits(ST25R3916_REG_ISO14443A_NFC, ST25R3916_REG_ISO14443A_NFC_no_rx_par, ST25R3916_REG_ISO14443A_NFC_no_rx_par);    //不接收校验位
//    
//    if(pUid->sak == ISO14443A_TAGTYPE_TOPAZ96)
//    {
//        u8 i = 0;
//        for(i = 0; i < ISO14443A_TOPAZ_BLOCK_LEN; i++)
//        {
//            pos = 0;
//            g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_TPZREAD;
//            g_sSt25r3916Frame.frame[pos++] = ((addr & 0x0F) << 3) | i;
//            g_sSt25r3916Frame.frame[pos++] = 0x00;
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[0];
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[1];
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[2];
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[3];
//            crc = ISO14443A_TopazCalCrc(g_sSt25r3916Frame.frame, pos);
//            g_sSt25r3916Frame.frame[pos++] = (crc >> 0) & 0xFF;
//            g_sSt25r3916Frame.frame[pos++] = (crc >> 8) & 0xFF;
//
//            g_sSt25r3916Frame.txLen = pos;
//
//            g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
//
//            //state = RC663_TopazCommand();
//            state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//            if(state == ST25R3916_STAT_OK)
//            {
//                if(ISO14443A_TopazCheckCrc(g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen))
//                {
//                    pBlock[i] = g_sSt25r3916Frame.frame[1];
//                }
//                else
//                {
//                    state = ST25R3916_STAT_CRC_ERROR;
//                    break;
//                }
//            }
//            else
//            {
//                break;
//            }
//        }
//    }
//    else
//    {
//        g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_TPZREADBLK;
//        g_sSt25r3916Frame.frame[pos++] = addr & 0x7F;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = 0x00;
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[0];
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[1];
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[2];
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[3];
//        crc = ISO14443A_TopazCalCrc(g_sSt25r3916Frame.frame, pos);
//        g_sSt25r3916Frame.frame[pos++] = (crc >> 0) & 0xFF;
//        g_sSt25r3916Frame.frame[pos++] = (crc >> 8) & 0xFF;
//
//        g_sSt25r3916Frame.txLen = pos;
//
//        g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
//
//        //state = RC663_TopazCommand();
//        state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//        if(state == ST25R3916_STAT_OK)
//        {
//            if(ISO14443A_TopazCheckCrc(g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen))
//            {
//                memcpy(pBlock, g_sSt25r3916Frame.frame + 1, ISO14443A_TOPAZ_BLOCK_LEN);
//            }
//            else
//            {
//                state = ST25R3916_STAT_CRC_ERROR;
//            }
//        }
//    }
//
//    return state;
//}
//
//u8 ISO14443A_TopazWriteBlock(ISO14443A_UID *pUid, u8 addr, u8 *pBlock)
//{
//    u8 state = ST25R3916_STAT_OK;
//    u16 crc = 0;
//    u8 pos = 0;
//    if(pUid->sak == ISO14443A_TAGTYPE_TOPAZ96)
//    {
//        u8 i = 0;
//        for(i = 0; i < ISO14443A_TOPAZ_BLOCK_LEN; i++)
//        {
//            pos = 0;
//            g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_TPZWE;
//            g_sSt25r3916Frame.frame[pos++] = ((addr & 0x0F) << 3) | i;
//            g_sSt25r3916Frame.frame[pos++] = pBlock[i];
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[0];
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[1];
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[2];
//            g_sSt25r3916Frame.frame[pos++] = pUid->uid[3];
//            crc = ISO14443A_TopazCalCrc(g_sSt25r3916Frame.frame, pos);
//            g_sSt25r3916Frame.frame[pos++] = (crc >> 0) & 0xFF;
//            g_sSt25r3916Frame.frame[pos++] = (crc >> 8) & 0xFF;
//
//            g_sSt25r3916Frame.txLen = pos;
//
//            g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
//
//            //state = RC663_TopazCommand();
//            state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//            if(state == ST25R3916_STAT_OK)
//            {
//                if(!ISO14443A_TopazCheckCrc(g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen))
//                {
//                    state = ST25R3916_STAT_CRC_ERROR;
//                    break;
//                }
//            }
//            else
//            {
//                break;
//            }
//        }
//    }
//    else
//    {
//        g_sSt25r3916Frame.frame[pos++] = ISO14443A_CMD_TPZWEBLK;
//        g_sSt25r3916Frame.frame[pos++] = addr & 0x7F;
//        g_sSt25r3916Frame.frame[pos++] = pBlock[0];
//        g_sSt25r3916Frame.frame[pos++] = pBlock[1];
//        g_sSt25r3916Frame.frame[pos++] = pBlock[2];
//        g_sSt25r3916Frame.frame[pos++] = pBlock[3];
//        g_sSt25r3916Frame.frame[pos++] = pBlock[4];
//        g_sSt25r3916Frame.frame[pos++] = pBlock[5];
//        g_sSt25r3916Frame.frame[pos++] = pBlock[6];
//        g_sSt25r3916Frame.frame[pos++] = pBlock[7];
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[0];
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[1];
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[2];
//        g_sSt25r3916Frame.frame[pos++] = pUid->uid[3];
//        crc = ISO14443A_TopazCalCrc(g_sSt25r3916Frame.frame, pos);
//        g_sSt25r3916Frame.frame[pos++] = (crc >> 0) & 0xFF;
//        g_sSt25r3916Frame.frame[pos++] = (crc >> 8) & 0xFF;
//
//        g_sSt25r3916Frame.txLen = pos;
//
//        g_sSt25r3916Frame.norspTime = ISO14443A_NORMAL_TIME;
//
//        //state = RC663_TopazCommand();
//        state = ISO14443A_Cmd(ST25R3916_CMD_TRANSMIT_WITHOUT_CRC, &g_sSt25r3916Frame);
//        if(state == ST25R3916_STAT_OK)
//        {
//            if(ISO14443A_TopazCheckCrc(g_sSt25r3916Frame.frame, g_sSt25r3916Frame.rxLen))
//            {
//                memcpy(pBlock, g_sSt25r3916Frame.frame + 1, ISO14443A_TOPAZ_BLOCK_LEN);
//            }
//            else
//            {
//                state = ST25R3916_STAT_CRC_ERROR;
//            }
//        }
//    }
//
//    return state;
//}