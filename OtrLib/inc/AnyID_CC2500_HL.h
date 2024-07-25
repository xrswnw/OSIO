#ifndef _ANYID_CC2500_HL_
#define _ANYID_CC2500_HL_

#include "AnyID_config.h"

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MCD_) | (defined(_ANYID_STM32_MPD_)))

    EXTERN const PORT_INF           CC2500_SPI_PORT;
    EXTERN const PORT_INF           CC2500_GDO0_PORT;
    EXTERN const PORT_INF           CC2500_CS_PORT;

    #define CC2500_CS_High()        GPIO_SetBits(CC2500_CS_PORT.Port, CC2500_CS_PORT.Pin)
    #define CC2500_CS_Low()         GPIO_ResetBits(CC2500_CS_PORT.Port, CC2500_CS_PORT.Pin)

    #define CC2500_EXIT_LINE        EXTI_Line11
    #define CC2500_IRQ_CHANNEL      EXTI15_10_IRQChannel
    #define CC2500_EXIT_PORT        GPIO_PortSourceGPIOB
    #define CC2500_EXIT_PIN         GPIO_PinSource11

#elif (defined(_ANYID_STM32_MTD_))

    EXTERN const PORT_INF           CC2500_SPI_PORT;
    EXTERN const PORT_INF           CC2500_GDO0_PORT;
    EXTERN const PORT_INF           CC2500_CS_PORT;

    #define CC2500_CS_High()        GPIO_SetBits(CC2500_CS_PORT.Port, CC2500_CS_PORT.Pin)
    #define CC2500_CS_Low()         GPIO_ResetBits(CC2500_CS_PORT.Port, CC2500_CS_PORT.Pin)

    #define CC2500_EXIT_LINE        EXTI_Line11
    #define CC2500_IRQ_CHANNEL      EXTI15_10_IRQChannel
    #define CC2500_EXIT_PORT        GPIO_PortSourceGPIOB
    #define CC2500_EXIT_PIN         GPIO_PinSource11


#elif (defined(_ANYID_PIC18_BWCOM_))

    #define CC2500_CS_TRIS          TRISAbits.TRISA5
    #define CC2500_CS               LATAbits.LATA5
    #define CC2500_GDO0_TRIS        TRISBbits.TRISB0
    #define CC2500_GDO0             LATBbits.LATB0
    
    #define CC2500_CS_High()        (CC2500_CS = 1)
    #define CC2500_CS_Low()         (CC2500_CS = 0)

#elif (defined(_ANYID_PIC18_WVDU_))
    
    #define CC2500_CS_TRIS          TRISAbits.TRISA5
    #define CC2500_CS               LATAbits.LATA5
    
    #define CC2500_GDO0_TRIS        TRISBbits.TRISB2
    #define CC2500_GDO0             LATBbits.LATB2
    
    #define CC2500_PA_TRIS          TRISBbits.TRISB4
    #define CC2500_PA               LATBbits.LATB4
    
    #define CC2500_LNA_TRIS         TRISBbits.TRISB5
    #define CC2500_LNA              LATBbits.LATB5
        
    #define CC2500_CS_High()        (CC2500_CS = 1)
    #define CC2500_CS_Low()         (CC2500_CS = 0)
    
    #define CC2500_PA_Disable()     (CC2500_PA = 0)     //cc2591
    #define CC2500_PA_Enable()      (CC2500_PA = 1)
    
    #define CC2500_LNA_Disable()    (CC2500_LNA = 0)    //cc2591
    #define CC2500_LNA_Enable()     (CC2500_LNA = 1)

    #define CC2500_PA_2591          1

#endif
void CC2500_Delay(void);
void CC2500_InitSPILow(void);
void CC2500_InitSPIHigh(void);

void CC2500_InitInterface(void);
void CC2500_LowPower(LPMode lp);
void CC2500_SoftReset(void);


void CC2500_IntConfig(FunctionalState state);
u8 CC2500_WriteByte(u8 byte);


#endif
