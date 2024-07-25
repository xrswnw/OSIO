#include "AnyID_HMC1052_HL.h"


void HMC1052_InitInterface(void)
{
#if (defined(_ANYID_PIC18_))
    HMC1052_Pulse_Low();
    HMC1052_Power_Off();
    HMC1052_X_AD_Disable();
    HMC1052_Y_AD_Disable();
    
    HMC1052_PLUSE_TRIS = 0;
    HMC1052_EN_TRIS = 0;
    HMC1052_X_ADEN_TRIS = 0;
    HMC1052_Y_ADEN_TRIS = 0;

    HMC1052_X = 0;              //配置AD输入通道
    HMC1052_Y = 0;
    HMC1052_X_TRIS = 1;
    HMC1052_Y_TRIS = 1;
    HMC1052_X_AD_SEL = 1;         //设置输入通道为模拟输入
    HMC1052_Y_AD_SEL = 1;

    ADCON0 = 0x00;              //default select AD0 channel
    ADCON1 = 0x30;              //external VSS, Vref
    ADCON2 = 0xAE;              //right aligned, 12 sample TAD,500k convert clock

    
    HMC1052_AD_Disable();        //disable AD module
#endif
}


#define HMC1052_AD_MASK     0x03FF
u16 HMC1052_GetADValue(u8 adChannel)
{
    u16 adValue = 0;
#if (defined(_ANYID_PIC18_))
    ADCON0bits.CHS0 = (adChannel >> 0) & 0x01;     //select channel
    ADCON0bits.CHS1 = (adChannel >> 1) & 0x01;
    ADCON0bits.CHS2 = (adChannel >> 2) & 0x01;
    ADCON0bits.CHS3 = (adChannel >> 3) & 0x01;     

    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);


    adValue = ADRESH;
    adValue <<= 8;
    adValue += ADRESL;
#endif
    return (adValue & HMC1052_AD_MASK);
}

//0.1% duty cycle, or less, 2us Pulse
void HMC1052_SetPulse(void)
{
#if (defined(_ANYID_PIC18_WVDU_))       //注意:这个和系统时钟相关
    HMC1052_Pulse_High();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop(); 
    Nop();
    Nop();
    Nop();
    Nop();
    Nop(); 
    Nop();
    Nop();
    Nop();
    Nop();
    Nop(); 
    HMC1052_Pulse_Low();
#endif
}


