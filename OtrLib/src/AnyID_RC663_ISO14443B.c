#include "AnyID_RC663_ISO14443B.h"


const u8 g_sRc663I14443B106[RC663_PROTOCOL_CONFIG_SIZE] =
{
    0x8F,
    0xCC,
    0x01,
    0x06,
    0x7B,
    0x7B,
    0x08,
    0x00,
    0x00,
    0x0F,
    0x05,
    0x34,
    0x3F,
    0x12,
    0x0A
};

u8 ISO14443B_Init(void)
{
    u8 i = 0;
    RC663_SelectProtocol(RC663_RXTX_I14443B_106, RC663_RXTX_I14443B_106);
    for(i = 0; i < RC663_PROTOCOL_CONFIG_SIZE; i++)
    {
        RC663_WriteReg(g_sRc663RegAdd[i], g_sRc663I14443B106[i]);
    }   
    RC663_Config(TRUE, TRUE, TRUE, 0, 0);
    RC663_ClearRegBit(RC663_REG_FRAMECON, RC663_BIT_RXPARITYEN | RC663_BIT_TXPARITYEN);

    return TRUE;
}


u8 ISO14443B_Dtu(void)
{
    u8 state = RC663_STAT_OK;
    
    g_sRc663Frame.cmd = RC663_CMD_TRANSCEIVE;

    state = RC663_Command();
    
    return state;
}

u8 ISO14443B_GetUid(u8 *pUid)
{
    u8 state = RC663_STAT_OK;
    u8 pos = 0;
	
    //读取ST标签
    pos = 0;
    g_sRc663Frame.frame[pos++] = 0x06; g_sRc663Frame.frame[pos++] = 0x00; 
    g_sRc663Frame.txLen = pos;
    g_sRc663Frame.timeout = ISO14443B_OP_TIME;
    state = ISO14443B_Dtu();
    if(state == RC663_STAT_OK)
    {
        u8 rnd = 0;
        
        rnd = g_sRc663Frame.frame[0];
        pos = 0;
        g_sRc663Frame.frame[pos++] = 0x0E; g_sRc663Frame.frame[pos++] = rnd; 
        g_sRc663Frame.txLen = pos;
        g_sRc663Frame.timeout = ISO14443B_OP_TIME;
        state = ISO14443B_Dtu();
        if(state == RC663_STAT_OK && g_sRc663Frame.frame[0] == rnd)
        {
            pos = 0;
            g_sRc663Frame.frame[pos++] = 0x0B;
            g_sRc663Frame.txLen = pos;
            g_sRc663Frame.timeout = ISO14443B_OP_TIME;
            state = ISO14443B_Dtu();
            if(state == RC663_STAT_OK && g_sRc663Frame.rxLen == ISO14443B_UID_LEN)
            {
                memcpy(pUid, g_sRc663Frame.frame, ISO14443B_UID_LEN);
            }
            else 
            {
                state = RC663_STAT_OK;
            }
        }
    }
    //正常模式TypeB
    else 
    {
        pos = 0;
        g_sRc663Frame.frame[pos++] = 0x05; g_sRc663Frame.frame[pos++] = 0x00; g_sRc663Frame.frame[pos++] = 0x00;
        g_sRc663Frame.txLen = pos;
        g_sRc663Frame.timeout = ISO14443B_OP_TIME;
        state = ISO14443B_Dtu();
    }
    
    return state;
} 




