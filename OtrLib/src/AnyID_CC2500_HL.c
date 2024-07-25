#include "AnyID_CC2500_HL.h"

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MCD_) | (defined(_ANYID_STM32_MPD_)))

    const PORT_INF CC2500_SPI_PORT = {GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15};
    const PORT_INF CC2500_GDO0_PORT = {GPIOB, GPIO_Pin_11};
    const PORT_INF CC2500_CS_PORT = {GPIOB, GPIO_Pin_12};

    #define CC2500_SPI SPI2

#elif (defined(_ANYID_STM32_MTD_))

    const PORT_INF CC2500_SPI_PORT = {GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15};
    const PORT_INF CC2500_GDO0_PORT = {GPIOB, GPIO_Pin_11};
    const PORT_INF CC2500_CS_PORT = {GPIOB, GPIO_Pin_12};

    #define CC2500_SPI SPI2

#endif

/******************************************************************************
** 函数名：CC2500_Delay
** 说  明：延时
**
** 参  数：void
** 返  回：void
******************************************************************************/
void CC2500_Delay(void)
{
#if (defined(_ANYID_STM32_))
    u32 i = 0xfff;
#elif (defined(_ANYID_PIC18_))
    u32 i = 0xff;
#endif

    while(i--);
}


void CC2500_InitSPILow(void)
{
#ifdef _ANYID_STM32_
    SPI_InitTypeDef   SPI_InitStructure;

    SPI_Cmd(CC2500_SPI, DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //6M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(CC2500_SPI, &SPI_InitStructure);

    SPI_Cmd(CC2500_SPI, ENABLE);
#endif
}

void CC2500_InitSPIHigh(void)
{
#ifdef _ANYID_STM32_
    SPI_InitTypeDef   SPI_InitStructure;

    SPI_Cmd(CC2500_SPI, DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; //6M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(CC2500_SPI, &SPI_InitStructure);

    SPI_Cmd(CC2500_SPI, ENABLE);
#endif
}


void CC2500_InitInterface(void)
{
#if (defined(_ANYID_STM32_))

    GPIO_InitTypeDef GPIO_InitStructure;

    //GDO0
    GPIO_InitStructure.GPIO_Pin = CC2500_GDO0_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(CC2500_GDO0_PORT.Port, &GPIO_InitStructure);

    //cs
    GPIO_InitStructure.GPIO_Pin = CC2500_CS_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CC2500_CS_PORT.Port, &GPIO_InitStructure);

    //spi
    GPIO_InitStructure.GPIO_Pin = CC2500_SPI_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(CC2500_SPI_PORT.Port, &GPIO_InitStructure);
      
#elif (defined(_ANYID_PIC18_))

    //               C3_CLK  C4_SDI  C5_SDO
    //设置SPI功能口:   输出    输入   输出
    LATC |= 0x38;

    TRISCbits.TRISC3 = 0;       //clk
    TRISCbits.TRISC4 = 1;       //sdi
    TRISCbits.TRISC5 = 0;       //sdo
      
    //Set the SPI module
    SSPSTAT = 0x40;
    SSPCON1 = 0x20;
          
    //CS输出
    CC2500_CS_TRIS = 0;
      
    //GDO0输入
    CC2500_GDO0_TRIS = 1;

    #ifdef CC2500_PA_2591
        CC2500_PA_Disable();
        CC2500_LNA_Disable();
        CC2500_PA_TRIS = 0;
        CC2500_LNA_TRIS = 0;
    #endif
    
#endif
}


void CC2500_LowPower(LPMode lp)
{
#ifdef _ANYID_STM32_
    GPIO_InitTypeDef GPIO_InitStructure;
#endif

    if(lp == LPM_SLEEP)
    {

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MCD_) | (defined(_ANYID_STM32_MPD_)))
        GPIO_InitStructure.GPIO_Pin = CC2500_SPI_PORT.Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
        GPIO_Init(CC2500_SPI_PORT.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = CC2500_GDO0_PORT.Pin;
        GPIO_Init(CC2500_GDO0_PORT.Port, &GPIO_InitStructure);
#endif
    }
    else
    {
        CC2500_InitInterface();
    }
}


void CC2500_IntConfig(FunctionalState state)
{
#if (defined(_ANYID_STM32_))

    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    GPIO_EXTILineConfig(CC2500_EXIT_PORT, CC2500_EXIT_PIN);

    EXTI_InitStructure.EXTI_Line = CC2500_EXIT_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = state;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CC2500_IRQ_CHANNEL;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;
    NVIC_Init(&NVIC_InitStructure);

#elif (defined(_ANYID_PIC18_BWCOM_))
    //Enable Int0
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 0;
    INTCONbits.INT0IF = 0;

#elif (defined(_ANYID_PIC18_WVDU_))
    //Enable Int2
    INTCON3bits.INT2IE = 1;
    INTCON3bits.INT2IP = 1;
    INTCON2bits.INTEDG2 = 0;
    INTCON3bits.INT2IF = 1;
#endif
}

u8 CC2500_WriteByte(u8 byte)
{
#if (defined(_ANYID_STM32_))
    if(SPI_GetFlagStatus(CC2500_SPI, SPI_FLAG_RXNE) == SET)
    {
        SPI_ReceiveData(CC2500_SPI);
    }

    while(SPI_GetFlagStatus(CC2500_SPI, SPI_FLAG_TXE) == RESET);
    SPI_SendData(CC2500_SPI, byte);
    while(SPI_GetFlagStatus(CC2500_SPI, SPI_FLAG_RXNE) == RESET);

    return SPI_ReceiveData(CC2500_SPI);
#elif (defined(_ANYID_PIC18_))

    PIR1bits.SSPIF = 0;
    do
    {
        SSPCON1bits.WCOL = 0;
        SSPBUF = byte;
    } while( SSPCON1bits.WCOL );

    while( PIR1bits.SSPIF == 0 );

    return SSPBUF;
#endif
}

/******************************************************************************
** 函数名：CC2500_SoftReset
** 说  明：软复位CC2500
**
** 参  数：void
** 返  回：void
******************************************************************************/
void CC2500_SoftReset(void)
{
#if (defined(_ANYID_STM32_))

    CC2500_InitSPIHigh();
    CC2500_CS_High();
    CC2500_Delay();
    CC2500_CS_Low();
    CC2500_Delay();
    CC2500_CS_Low();
    CC2500_Delay();
    CC2500_CS_Low();
    CC2500_InitSPILow();
    
#elif (defined(_ANYID_PIC18_))

    CC2500_CS_High();
  	CC2500_Delay();
    
  	CC2500_CS_Low();
  	CC2500_Delay();
  	CC2500_Delay();
    
    CC2500_CS_High();
    CC2500_Delay();
  	CC2500_Delay();
    CC2500_Delay();
  	CC2500_Delay();
    
  	CC2500_CS_Low();
#endif

}


