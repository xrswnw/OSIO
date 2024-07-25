#include "AnyID_CR95HF_Felica.h"


u8 Felica_Init(void)
{
    /*u8 i = 0;
    RC663_SelectProtocol(RC663_RXTX_FELICA_212, RC663_RXTX_FELICA_212);
    for(i = 0; i < RC663_PROTOCOL_CONFIG_SIZE; i++)
    {
        RC663_WriteReg(g_sRc663RegAdd[i], g_sRc663Felica[i]);
    }   
    RC663_Config(TRUE, TRUE, TRUE, 0, 0);
    RC663_ClearRegBit(RC663_REG_FRAMECON, RC663_BIT_TXPARITYEN | RC663_BIT_RXPARITYEN);
    RC663_WriteReg(RC663_REG_RXWAIT, RC663_BIT_RXWAITDBFREQ | (0x03 << 1));
    */    
    return TRUE;
}

u8 Felica_Polling(u8 *pUid, u16 systemCode, u8 slot)
{
    /*u8 state = RC663_STAT_OK;
    u8 pos = 0;
 
    g_sRc663Frame.frame[pos++] = 0x00;
    g_sRc663Frame.frame[pos++] = FELICA_CMD_REQC;
    g_sRc663Frame.frame[pos++] = (systemCode >> 0) & 0xFF;
    g_sRc663Frame.frame[pos++] = (systemCode >> 8) & 0xFF;
    g_sRc663Frame.frame[pos++] = FELICA_POLLING_RFU;
    g_sRc663Frame.frame[pos++] = slot;
    
    g_sRc663Frame.frame[0] = pos;
    g_sRc663Frame.txLen = pos;
    
    g_sRc663Frame.timeout = FELICA_POLLING_TIME;
    g_sRc663Frame.cmd = RC663_CMD_TRANSCEIVE;

    state = RC663_Command();
    if(state == RC663_STAT_OK)
    {
        memcpy(pUid, g_sRc663Frame.frame + 2, g_sRc663Frame.rxLen - 2);
    }
    
    return state;*/
}


u8 Felica_Dtu(void)
{
    /*u8 state = RC663_STAT_OK;
    
    g_sRc663Frame.cmd = RC663_CMD_TRANSCEIVE;

    state = RC663_Command();
    
    return state;*/
}

