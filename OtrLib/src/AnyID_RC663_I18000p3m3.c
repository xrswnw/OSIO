#include "AnyID_RC663_I18000p3m3.h"

EPC_INFO g_sEpcInfo = {0};

const u8 g_aRc663I18000p3m3DSM2[RC663_PROTOCOL_CONFIG_SIZE] =
{
    0x8F,
    0x10,
    0x01,
    0x06,
    0x00,       //txcrc-disable，3M3基于bit的crc，很多指令又不需要crc，而且还分为crc5和crc16，所以全部基于APP来计算crc，更方便一点
    0x00,       //rxcrc-disable，3M3基于bit的crc，很多指令又不需要crc，而且还分为crc5和crc16，所以全部基于APP来计算crc，更方便一点
    0x09,
    0x00,
    0x00,
    0x80,
    0x01,       //tx parity disabel\rx parity disable\no stopSym\RC663_REG_FRAMECON,
    0x04,
    0x36,
    0x12,
    0x0A
};

u8 Epc_Init(void)
{
    u8 i = 0;
    //必须先设置协议，因为设置协议之后，参数会复位
    RC663_SelectProtocol(RC663_TX_I18000P3M3_ASK, RC663_RX_I18000P3M3_QS_M4);   //接收通道使用848K-M4模式，这个参数是固定不变的

    for(i = 0; i < RC663_PROTOCOL_CONFIG_SIZE; i++)
    {
        RC663_WriteReg(g_sRc663RegAdd[i], g_aRc663I18000p3m3DSM2[i]);
    }

    RC663_WriteReg(RC663_REG_TXSYM0L, 0x41);
    RC663_WriteReg(RC663_REG_TXSYM0H, 0x50);
    RC663_WriteReg(RC663_REG_TXSYM1L, 0x41);
    RC663_WriteReg(RC663_REG_TXSYM1H, 0x01);
    RC663_WriteReg(RC663_REG_TXSYM10LEN, 0x8E);
    
    return TRUE;
}

u16 Epc_ForamtBit(u8 *pBits, u16 pos, u32 dat, u8 num)
{
    u8 i = 0;
    for(i = 0; i < num; i++)
    {
        pBits[pos++] = (dat >> (num - 1 - i)) & 0x01;
    }

    return pos;
}

u16 Epc_ForamtBitLeftAlign(u8 *pBits, u16 pos, u8 dat, u8 num)
{
    u8 i = 0;
    for(i = 0; i < num; i++)
    {
        pBits[pos++] = (dat >> (8 - 1 - i)) & 0x01;
    }

    return pos;
}

u32 Epc_FormatByt(u8 *pBits, u8 len)
{
    u8 i = 0;
    u32 n = 0;
    for(i = 0; i < len; i++)
    {
        n <<= 1;
        n |= pBits[i];
    }

    return n;
}

u8 Epc_FormatBuffer(u8 *pBuffer, u8 *pBits, u16 len)
{
    u8 i = 0;
    u8 num = 0;
    num = len >> 3;
    for(i = 0; i < num; i++)
    {
        pBuffer[i] = Epc_FormatByt(pBits + (i << 3), 8);
    }
    if(len & 0x07)
    {
        u8 v = 0;
        
        v = Epc_FormatByt(pBits + (i << 3), len & 0x07);
        v <<= (8 - (len & 0x07));           //数据发送是msbfirst，所以这里要左移
        pBuffer[num++] = v;
    }

    return num;
}

#define EPC_CRC5_PRESET_VALUE               0x48    //x5+x3+1
#define EPC_CRC5_POLYNOMIAL_VALUE           0x48
u8 Epc_CalCrc5(u8 *pBits, u16 bitNum)
{
    u16 i = 0, j = 0, k = 0;
    u8 crc = EPC_CRC5_PRESET_VALUE;        // Initial value: 0x48 = 0x09<<(8-5)

    for(i = 0; i < bitNum;)
    {
        u8 t = 0;
        for(k = 0; k < 8 && i < bitNum; k++, i++)
        {
            t <<= 1;
            t |= pBits[i];
        }
        t <<= (8 - k);

        crc ^= t;
        for(j = 0; j < k; j++)
        {
            if(crc & 0x80)
            {
                crc = (crc << 1) ^ EPC_CRC5_POLYNOMIAL_VALUE;        // 0x48 = 0x09<<(8-5)
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc >> 3;
}

#define EPC_CRC16_PRESET_VALUE              0xFFFF    //x5+x3+1
#define EPC_CRC16_POLYNOMIAL_VALUE          0x1021
u16 Epc_CalCrc16(u8 *pBits, u16 bitNum)
{
    u16 i = 0, j = 0, k = 0;
    u16 crc = EPC_CRC16_PRESET_VALUE;

    for(i = 0; i < bitNum;)
    {
        u16 t = 0;
        for(k = 0; k < 8 && i < bitNum; k++, i++)
        {
            t <<= 1;
            t |= pBits[i];
        }
        t <<= (8 - k);

        t <<= 8;
        crc ^= t;
        for(j = 0; j < k; j++)
        {
            if(crc & 0x8000)
            {
                crc = (crc << 1) ^ EPC_CRC16_POLYNOMIAL_VALUE;        // 0x48 = 0x09<<(8-5)
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return ~crc;
}

BOOL Epc_CheckCrc5(u8 *pBits, u16 len)
{
    BOOL b = FALSE;
    if(len > EPC_CRC5_LEN)
    {
        u8 crc1 = 0, crc2 = 0;
        crc1 = Epc_FormatByt(pBits + len - EPC_CRC5_LEN, EPC_CRC5_LEN);
        crc2 = Epc_CalCrc5(pBits, len - EPC_CRC5_LEN);
        if(crc1 == crc2)
        {
            b = TRUE;
        }
    }

    return b;
}

BOOL Epc_CheckCrc16(u8 *pBits, u16 len)
{
    BOOL b = FALSE;
    if(len > EPC_CRC16_LEN)
    {
        u16 crc1 = 0, crc2 = 0;
        crc1 = Epc_FormatByt(pBits + len - EPC_CRC16_LEN, EPC_CRC16_LEN);
        crc2 = Epc_CalCrc16(pBits, len - EPC_CRC16_LEN);
        if(crc1 == crc2)
        {
            b = TRUE;
        }
    }

    return b;
}

u8 Epc_Command(EPC_FRAME *pEpcFrame)
{
    u8 state = RC663_STAT_OK;
    u16 crc = 0;
    u16 t = 0;
    
    if(pEpcFrame->txCrcMode == EPC_CRC_5)
    {
        crc = Epc_CalCrc5(pEpcFrame->bits, pEpcFrame->txbitsNum);
        pEpcFrame->txbitsNum = Epc_ForamtBit(pEpcFrame->bits, pEpcFrame->txbitsNum, crc, EPC_CRC5_LEN);
    }
    else if(pEpcFrame->txCrcMode == EPC_CRC_16)
    {
        crc = Epc_CalCrc16(pEpcFrame->bits, pEpcFrame->txbitsNum);
        pEpcFrame->txbitsNum = Epc_ForamtBit(pEpcFrame->bits, pEpcFrame->txbitsNum, crc, EPC_CRC16_LEN);
    }
    
    g_sRc663Frame.cmd = RC663_CMD_TRANSCEIVE;
    g_sRc663Frame.timeout = pEpcFrame->waitTime << 1;  //这里实际是超时时间，理论上pEpcFrame->waitTime时间肯定会有结果的
    
    g_sRc663Frame.txLen = Epc_FormatBuffer(g_sRc663Frame.frame, pEpcFrame->bits, pEpcFrame->txbitsNum);
    pEpcFrame->lastBitNum = pEpcFrame->txbitsNum & 0x07;

    t = (pEpcFrame->waitTime >> 2) + 1;        //单位是4.72us，实际是除以4.72，这里就简化除以4，并且额外增加4.72us
    RC663_WriteReg(RC663_REG_T0RELOADHI, (t >> 8) & 0xFF);
    RC663_WriteReg(RC663_REG_T0RELOADLO, (t >> 0) & 0xFF);
    
    if(pEpcFrame->bPreamble)
    {
        RC663_SetRegBits(RC663_REG_FRAMECON, RC663_MASK_STARTSYM, RC663_VALUE_START_SYM0);
    }
    else
    {
        RC663_SetRegBits(RC663_REG_FRAMECON, RC663_MASK_STARTSYM, RC663_VALUE_START_SYM1);
    }

    RC663_Config(FALSE, FALSE, FALSE, pEpcFrame->lastBitNum, 0);
    state = RC663_Command();
    if(state == RC663_STAT_OK)
    {
        u16 rxBitsNum = 0;
        u8 i = 0;
        
        for(i = 0; i < g_sRc663Frame.rxLen; i++)
        {
            rxBitsNum = Epc_ForamtBitLeftAlign(pEpcFrame->bits, rxBitsNum, g_sRc663Frame.frame[i], 8);
        }
        state = EPC_ERR_SUC;
        if(pEpcFrame->rxCrcMode == EPC_CRC_5)
        {
            pEpcFrame->rxbitsNum += EPC_CRC5_LEN;
            if(Epc_CheckCrc5(pEpcFrame->bits, pEpcFrame->rxbitsNum) == FALSE)
            {
                state = EPC_ERR_CRC;
            }
        }
        else if(pEpcFrame->rxCrcMode == EPC_CRC_16)
        {
            if(pEpcFrame->bEpcReply)
            {
                u8 i = 0;
                g_sRc663Frame.frame[g_sRc663Frame.rxLen++] = (pEpcFrame->rn >> 8) & 0xFF;
                g_sRc663Frame.frame[g_sRc663Frame.rxLen++] = (pEpcFrame->rn >> 0) & 0xFF;
                
                pEpcFrame->rxbitsNum = 0;
                for(i = 0; i < g_sRc663Frame.rxLen; i++)
                {
                    pEpcFrame->rxbitsNum = Epc_ForamtBit(pEpcFrame->bits, pEpcFrame->rxbitsNum, g_sRc663Frame.frame[i], 8); 
                }
            }
            else
            {
                pEpcFrame->rxbitsNum += EPC_CRC16_LEN;
            }

            if(Epc_CheckCrc16(pEpcFrame->bits, pEpcFrame->rxbitsNum) == FALSE)
            {
                state = EPC_ERR_CRC;
            }
        }
    }
    else if(state == RC663_STAT_ERROR)
    {
        u8 err = 0;
        err = RC663_ReadReg(RC663_REG_ERROR);
        if(err & (RC663_BIT_INTEGERR | RC663_BIT_COLLDET | RC663_BIT_PROTERR | RC663_BIT_NODATAERR | RC663_BIT_MINFRAMEERR))
        {
            state = EPC_ERR_TAG;
        }
        else
        {
            state = EPC_ERR_NORSP;
        }
    }
    else
    {
        state = EPC_ERR_NORSP;
    }
    return state;
}

u8 Epc_CalPicc(EPC_INFO *pEpcInfo)
{
    u8 i = 0;
    EPC_FRAME *pFrame = NULL;
    u8 state = RC663_STAT_OK;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_CALL, 4);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.target, 3);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.action, 3);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.bank, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.addrLen, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.addr, (pEpcInfo->s.msk.addrLen + 1) << 3);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.len, 8);
    for(i = 0; i < (pEpcInfo->s.msk.len >> 3); i++)
    {
        pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.value[i], 8);
    }
    if(pEpcInfo->s.msk.len & 0x07)
    {
        pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.value[i], pEpcInfo->s.msk.len & 0x07);
    }
    
    pFrame->bits[pFrame->txbitsNum++] = 1;                                          //848kHz
    pFrame->bits[pFrame->txbitsNum++] = 1; pFrame->bits[pFrame->txbitsNum++] = 1;   //Manchester 4
    pFrame->bits[pFrame->txbitsNum++] = 0;                                          //diable pilot tone

    pFrame->txCrcMode = EPC_CRC_16;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = 0;
    pFrame->rxCrcMode = EPC_CRC_16;
    pFrame->bEpcReply = FALSE;               //动态长度
    if(pEpcInfo->s.action == EPC_CAL_ACTION_TID)
    {
        pFrame->rxbitsNum = 1 + 12 * 8;             //flag+tid
    }
    else if(pEpcInfo->s.action == EPC_CAL_ACTION_EPC)
    {
        pFrame->rxbitsNum = 1 + 2 * 8 + 12 * 8;     //flag+pc+epc
    }
    else 
    {
        pFrame->rxbitsNum = 1 + 2 * 8;              //flag+storedcrc
    }
 
    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_SelectPicc(EPC_INFO *pEpcInfo)
{
    u8 i = 0;
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_SELECT, 4);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.target, 3);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.action, 3);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.bank, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.addrLen, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.addr, (pEpcInfo->s.msk.addrLen + 1) << 3);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.len, 8);
    for(i = 0; i < (pEpcInfo->s.msk.len >> 3); i++)
    {
        pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.value[i], 8);
    }
    if(pEpcInfo->s.msk.len & 0x07)
    {
        pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->s.msk.value[i], pEpcInfo->s.msk.len & 0x07);
    }
    pFrame->bits[pFrame->txbitsNum++] = 0;                     //disable truncation

    pFrame->txCrcMode = EPC_CRC_16;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = 0;
    pFrame->rxCrcMode = EPC_CRC_NO;
    pFrame->bEpcReply = FALSE;               //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_BeginRoundPicc(EPC_INFO *pEpcInfo)
{
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;
    
    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_BEGIN, 4);
    pFrame->bits[pFrame->txbitsNum++] = 1;                                          //848kHz
    pFrame->bits[pFrame->txbitsNum++] = 1; pFrame->bits[pFrame->txbitsNum++] = 1;   //Manchester 4
    pFrame->bits[pFrame->txbitsNum++] = 0;                                          //diable pilot tone
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->r.sel, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->r.session, 2);
    pFrame->bits[pFrame->txbitsNum++] = pEpcInfo->r.flag;                           //target A
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->r.q, 4);

    pFrame->txCrcMode = EPC_CRC_5;
    pFrame->bPreamble = TRUE;

    pFrame->rxbitsNum = EPC_RN_LEN;
    pFrame->rxCrcMode = EPC_CRC_5;
    pFrame->bEpcReply = FALSE;                                                      //动态长度
    
    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_ResizeRoundPicc(EPC_INFO *pEpcInfo)
{
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_RESIZE, 4);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->r.session, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->r.upDn, 3);

    pFrame->txCrcMode = EPC_CRC_NO;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = EPC_RN_LEN;
    pFrame->rxCrcMode = EPC_CRC_5;
    pFrame->bEpcReply = FALSE;               //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_NextSlotPicc(EPC_INFO *pEpcInfo)
{
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_NXTSLOT, 4);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->r.session, 2);

    pFrame->txCrcMode = EPC_CRC_NO;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = EPC_RN_LEN;
    pFrame->rxCrcMode = EPC_CRC_5;
    pFrame->bEpcReply = FALSE;               //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_AckPicc(EPC_INFO *pEpcInfo)
{
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_ACK, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->rn, EPC_RN_LEN);

    pFrame->txCrcMode = EPC_CRC_NO;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = 0;
    pFrame->rxCrcMode = EPC_CRC_16;
    pFrame->bEpcReply = TRUE;               //动态长度

    pFrame->rn = pEpcInfo->rn;
    
    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_NakPicc(EPC_INFO *pEpcInfo)
{
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_NAK, 8);

    pFrame->txCrcMode = EPC_CRC_NO;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = 0;
    pFrame->rxCrcMode = EPC_CRC_NO;
    pFrame->bEpcReply = FALSE;               //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_ReqRnPicc(EPC_INFO *pEpcInfo)
{
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_REQRN, 8);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->rn, EPC_RN_LEN);

    pFrame->txCrcMode = EPC_CRC_16;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = EPC_RN_LEN;
    pFrame->rxCrcMode = EPC_CRC_16;
    pFrame->bEpcReply = FALSE;               //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_ReadPicc(EPC_INFO *pEpcInfo)
{
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);
    
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_READ, 8);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->b.index, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_ADDR_LEN_08, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->b.addr, 1 << 3);   //EPC_ADDR_LEN_08
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->b.count, 8);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->rn, EPC_RN_LEN);

    pFrame->txCrcMode = EPC_CRC_16;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = 1 + (pEpcInfo->b.count << 4) + EPC_RN_LEN;
    pFrame->rxCrcMode = EPC_CRC_16;
    pFrame->bEpcReply = FALSE;               //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_WritePicc(EPC_INFO *pEpcInfo)
{
    u16 nWord = 0;
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);
    
    nWord = pEpcInfo->b.buf[(pEpcInfo->nWordIndex << 1) + 0] << 8;
    nWord |= pEpcInfo->b.buf[(pEpcInfo->nWordIndex << 1) + 1];

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_WRITE, 8);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->b.index, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_ADDR_LEN_08, 2);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->b.addr + pEpcInfo->nWordIndex, 1 << 3);   //EPC_ADDR_LEN_08
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, nWord, EPC_BLOCK_LEN);// ^ pEpcInfo->rn
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->rn, EPC_RN_LEN);

    pFrame->txCrcMode = EPC_CRC_16;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = 1 + EPC_RN_LEN;
    pFrame->rxCrcMode = EPC_CRC_16;
    pFrame->bEpcReply = FALSE;               //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout---------------------写的时间可能不同

    state = Epc_Command(pFrame);
    
    return state;
}

u8 Epc_ConfigEas(EPC_INFO *pEpcInfo, BOOL bEnable)
{
    u8 eas = 0x0000;
    EPC_FRAME *pFrame = NULL;
    u8 state = EPC_ERR_SUC;

    pFrame = &pEpcInfo->frame;

    Epc_ResetFrame(pFrame);
    if(bEnable)
    {
        eas |= 0x0001;
    }

    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, EPC_CMD_CFGEAS, 16);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, 0x00, 8);                //rfu
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, eas ^ pEpcInfo->rn, EPC_BLOCK_LEN);
    pFrame->txbitsNum = Epc_ForamtBit(pFrame->bits, pFrame->txbitsNum, pEpcInfo->rn, EPC_RN_LEN);

    pFrame->txCrcMode = EPC_CRC_16;
    pFrame->bPreamble = FALSE;

    pFrame->rxbitsNum = 1 + EPC_BLOCK_LEN;
    pFrame->rxCrcMode = EPC_CRC_16;
    pFrame->bEpcReply = FALSE;                              //动态长度

    pFrame->waitTime = EPC_T1_MAX_US + EPC_T3_MIN_US;                               //timeout---------------------写的时间可能不同

    state = Epc_Command(pFrame);
    
    return state;
}

void Epc_ResetAntiColl(EPC_INFO *pEpcInfo, BOOL bBeginRound)
{
    if(bBeginRound)
    {
        pEpcInfo->anti.qfp = pEpcInfo->r.q * 100;
    }

    pEpcInfo->anti.slot = (1 << pEpcInfo->r.q);
    pEpcInfo->anti.colliCount = 0;
    pEpcInfo->anti.noRspCount = 0;
    pEpcInfo->anti.tagNum = 0;
}

u8 Epc_RoundQfp(u8 qfp)
{
    u8 qn = 0, qf = 0;

    qn = qfp / 10;
    qf = qfp % 10;

    if(qf > 5)
    {
        qn++;
    }
    return qn;
}

void Epc_ResizeQ(EPC_INFO *pEpcInfo)
{
    u8 q = 0;
    q = pEpcInfo->r.q;

    if(pEpcInfo->rlt == EPC_ERR_NORSP)                          //没有标签,max(0, qfp - c)
    {
        pEpcInfo->anti.noRspCount++;
        if(pEpcInfo->anti.qfp > EPC_C1_VALUE)
        {
            pEpcInfo->anti.qfp -= EPC_C1_VALUE;
        }
        else
        {
            pEpcInfo->anti.qfp = 0;
        }
        q = pEpcInfo->anti.qfp / 100;        //ceil计算
        if(pEpcInfo->anti.qfp % 100)
        {
            q++;
        }

        pEpcInfo->anti.c = EPC_C1_VALUE;
    }
    else if(pEpcInfo->collisionCount > 0)   //多个标签,min(15, qfp - c)
    {
        pEpcInfo->anti.colliCount++;
        pEpcInfo->anti.qfp += EPC_C2_VALUE;
        if(pEpcInfo->anti.qfp > EPC_QFP_MAX)
        {
            pEpcInfo->anti.qfp = EPC_QFP_MAX;
        }
        q = pEpcInfo->anti.qfp / 100;        //floor计算

        pEpcInfo->anti.c = EPC_C2_VALUE;
    }

    if(q > pEpcInfo->r.q)
    {
        pEpcInfo->r.upDn = EPC_ROUND_Q_UP;
        pEpcInfo->r.q++;
    }
    else if(q < pEpcInfo->r.q)
    {
        pEpcInfo->r.upDn = EPC_ROUND_Q_DN;
        pEpcInfo->r.q--;
    }
    else
    {
        pEpcInfo->r.upDn = EPC_ROUND_Q_NC;
    }
}
