#ifndef _ANYID_HMC1052_HL_
#define _ANYID_HMC1052_HL_

#include "AnyID_Config.h"

#if (defined(_ANYID_PIC18_WVDU_))

    #define HMC1052_PLUSE_TRIS              TRISCbits.TRISC0
    #define HMC1052_EN_TRIS                 TRISAbits.TRISA7
    #define HMC1052_X_TRIS                  TRISAbits.TRISA0
    #define HMC1052_Y_TRIS                  TRISAbits.TRISA1
    #define HMC1052_X_ADEN_TRIS             TRISCbits.TRISC1
    #define HMC1052_Y_ADEN_TRIS             TRISAbits.TRISA6

    #define HMC1052_X_AD_SEL                ANSELbits.ANS0
    #define HMC1052_Y_AD_SEL                ANSELbits.ANS1

    #define HMC1052_AD_Enable()             (ADCON0bits.ADON = 1)     
    #define HMC1052_AD_Disable()            (ADCON0bits.ADON = 0) 

    #define HMC1052_PLUSE                   LATCbits.LATC0
    #define HMC1052_EN                      LATAbits.LATA7
    #define HMC1052_X                       LATAbits.LATA0
    #define HMC1052_Y                       LATAbits.LATA1
    #define HMC1052_X_ADEN                  LATCbits.LATC1
    #define HMC1052_Y_ADEN                  LATAbits.LATA6

    #define HMC1052_Pulse_High()            (HMC1052_PLUSE = 1)
    #define HMC1052_Pulse_Low()             (HMC1052_PLUSE = 0)

    #define HMC1052_Power_On()              (HMC1052_EN = 0)
    #define HMC1052_Power_Off()             (HMC1052_EN = 1)

    #define HMC1052_X_AD_Enable()           (HMC1052_X_ADEN = 1)
    #define HMC1052_X_AD_Disable()          (HMC1052_X_ADEN = 0)

    #define HMC1052_Y_AD_Enable()           (HMC1052_Y_ADEN = 1)
    #define HMC1052_Y_AD_Disable()          (HMC1052_Y_ADEN = 0)

    #define HMC1052_CHANNEL_X               0x01
    #define HMC1052_CHANNEL_Y               0x00
#endif

void HMC1052_InitInterface(void);
u16 HMC1052_GetADValue(u8 adChannel);
void HMC1052_SetPulse(void);

#define HMC1052_EnableAD(a)  (ADCON0bits.ADON = a)


#endif
