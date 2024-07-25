#include "AnyID_Tag_I18000p3m3.h"

u8 g_aEpcHandle[2] = {0};
const u8 g_aRc663I18000p3m3DSM2[RC663_PROTOCOL_CONFIG_SIZE] =
{
    0x8F,
    0x10,
    0x01,
    0x06,
    0x11,
    0x91,
    0x09,
    0x00,
    0x00,
    0x80,
    0x01,
    0x04,
    0x36,
    0x12,
    0x0A
};

u8 I18000p3m3_Init(void)
{
    u8 i = 0;
    RC663_SelectProtocol(RC663_RXTX_I18000P3M3_DS_M2, RC663_RXTX_I18000P3M3_DS_M2);
    for(i = 0; i < RC663_PROTOCOL_CONFIG_SIZE; i++)
    {
        RC663_WriteReg(g_sRc663RegAdd[i], g_aRc663I18000p3m3DSM2[i]);
    }
    RC663_Config(TRUE, TRUE, TRUE, 0, 0);
    g_aEpcHandle[0] = 0x00;
    g_aEpcHandle[1] = 0x00;

    return TRUE;
}

u8 I18000p3m3_AckReq(void)
{
    u8 state = RC663_STAT_OK;
    u8 reg = 0;
    u32 timeout = 1000;

    g_sRc663Frame.frame[0] = 0x8C;
    g_sRc663Frame.frame[1] = 0x00;
    g_sRc663Frame.frame[2] = 0x00;
    g_sRc663Frame.txLen = 3;

    RC663_WriteReg(RC663_REG_RXCORR, 0x88);
    RC663_WriteReg(RC663_REG_RXCTRL, 0x07);
    RC663_WriteReg(RC663_REG_RXWAIT, 0x3D);
    RC663_WriteReg(RC663_REG_TXWAITLO, 0x80);

    // Set Preamble
    RC663_WriteReg(RC663_REG_FRAMECON, 0x01);
    RC663_WriteReg(RC663_REG_TXDATAMOD, 0x00);
    RC663_WriteReg(RC663_REG_TXSYM0H, 0x68);
    RC663_WriteReg(RC663_REG_TXSYM0L, 0x41);
    RC663_WriteReg(RC663_REG_TXSYM10LEN, 0x8E);
    RC663_WriteReg(RC663_REG_TXSYM10BURSTLEN, 0x00);
    RC663_WriteReg(RC663_REG_TXSYM10BURSTCTRL, 0x00);
    RC663_WriteReg(RC663_REG_TXSYM10MOD, 0x00);
    RC663_WriteReg(RC663_REG_TXSYMFREQ, 0x05);

    RC663_FlushFifo();

    // Set preamble done
    RC663_WriteFIFO(g_sRc663Frame.frame, g_sRc663Frame.txLen);

    RC663_WriteReg(RC663_REG_COMMAND, RC663_CMD_ACKREQ);

    reg = RC663_ReadReg(RC663_REG_STATUS);

    do
    {
        reg = RC663_ReadReg(RC663_REG_STATUS);
        RC663_Delay16us(6);
        timeout--;
    }while((reg != 0x00) && (timeout > 0));

    //Check if an error occured read the error register
    reg = RC663_ReadReg(RC663_REG_ERROR);
    if(reg & (RC663_BIT_NODATAERR | RC663_BIT_FIFOOVL | RC663_BIT_FIFOWRERR | RC663_BIT_EE_ERR))
    {
        state = RC663_STAT_ERROR;
    }
    else
    {
        g_sRc663Frame.rxLen = RC663_ReadReg(RC663_REG_FIFOLENGTH);
        RC663_ReadFIFO(g_sRc663Frame.frame, g_sRc663Frame.rxLen);

        g_aEpcHandle[0] = g_sRc663Frame.frame[g_sRc663Frame.rxLen - 2];
        g_aEpcHandle[1] = g_sRc663Frame.frame[g_sRc663Frame.rxLen - 1];
    }

    return state;
}



u8 I18000p3m3_Read(u8 memBank, u8 *pWordPtr, u8 wordPtrLength, u8 wordCount)
{
    u8 reg = 0;
    u8 state = RC663_STAT_OK;
    u8 pos = 0;
    u8 bitNum = 0;
    u8 i = 0;

    memBank &= I18000P3M3_MEMBANCK_MSK;
    wordPtrLength &= I18000P3M3_MEMBANCK_WPLMSK;

    // prepare command header
    g_sRc663Frame.frame[pos++] = I18000P3M3_CMD_READ;
    g_sRc663Frame.frame[pos] = (memBank << 6) | (wordPtrLength << 4);  // 4 bits left

    // append pointer
    for(i = 0; i < (wordPtrLength + 1); i++)
    {
        g_sRc663Frame.frame[pos++] |= (pWordPtr[i]) >> 4;
        g_sRc663Frame.frame[pos] = (pWordPtr[i]) << 4;
    }
    // 4 bits left

    // append word-count
    g_sRc663Frame.frame[pos++] |= wordCount >> 4;  // 0 bits left
    g_sRc663Frame.frame[pos]  = wordCount << 4;  // 4 bits left

    // append handle
    g_sRc663Frame.frame[pos++] |= g_aEpcHandle[0] >> 4;    // 0 bits left
    g_sRc663Frame.frame[pos] = g_aEpcHandle[0] << 4;    // 4 bits left
    g_sRc663Frame.frame[pos++] |= g_aEpcHandle[1] >> 4;    // 0 bits left
    g_sRc663Frame.frame[pos++] = g_aEpcHandle[1] << 4;    // 4 bits left

    g_sRc663Frame.txLen = pos;

    RC663_SetTxLastBits(4);

    g_sRc663Frame.cmd = RC663_CMD_TRANSCEIVE;
    g_sRc663Frame.timeout = 100000;

    //Set Frame Sync
    RC663_WriteReg(RC663_REG_FRAMECON, 0x02);
    RC663_WriteReg(RC663_REG_TXDATAMOD, 0x00);

    RC663_WriteReg(RC663_REG_TXSYM1H, 0x01);
    RC663_WriteReg(RC663_REG_TXSYM1L, 0xA1);
    RC663_WriteReg(RC663_REG_TXSYM10LEN, 0x8E);
    RC663_WriteReg(RC663_REG_TXSYM10BURSTLEN, 0x00);
    RC663_WriteReg(RC663_REG_TXSYM10BURSTCTRL, 0x00);
    RC663_WriteReg(RC663_REG_TXSYM10MOD, 0x00);
    RC663_WriteReg(RC663_REG_TXSYMFREQ, 0x05);

    RC663_WriteReg(RC663_REG_TXCRCCON, 0x7B);
    RC663_WriteReg(RC663_REG_RXCRCCON, 0x7B);

    state = RC663_Command();
    if(state == RC663_STAT_OK)
    {
        for (i = 0; i < (g_sRc663Frame.rxLen - 1); i++)
        {
            g_sRc663Frame.frame[i] = (g_sRc663Frame.frame[i] << 1) | (g_sRc663Frame.frame[i + 1] >> 7);
        }
        //Status byte is removed
        g_sRc663Frame.rxLen--;
        if((g_sRc663Frame.frame[g_sRc663Frame.rxLen - 2] != g_aEpcHandle[0]) || (g_sRc663Frame.frame[g_sRc663Frame.rxLen - 1] != g_aEpcHandle[1]))
        {
            state = RC663_STAT_HANDLE_ERR;
        }
    }

    return state;
}






