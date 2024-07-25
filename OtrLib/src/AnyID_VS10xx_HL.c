#include "AnyID_VS10XX_HL.h"

#if defined(_ANYID_STM32_MTD_)

    const PORT_INF VS10XX_DREQ_PORT = {GPIOC, GPIO_Pin_0};
    const PORT_INF VS10XX_XCCS_PORT = {GPIOA, GPIO_Pin_2};
    const PORT_INF VS10XX_RST_PORT = {GPIOA, GPIO_Pin_3};
    const PORT_INF VS10XX_XDCS_PORT = {GPIOA, GPIO_Pin_1};
    const PORT_INF VS10XX_SPI_PORT = {GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15};


#elif defined(_ANYID_STM32_BCTRL_)

    const PORT_INF VS10XX_DREQ_PORT = {GPIOC, GPIO_Pin_7};
    const PORT_INF VS10XX_XCCS_PORT = {GPIOB, GPIO_Pin_12};
    const PORT_INF VS10XX_RST_PORT = {GPIOC, GPIO_Pin_8};
    const PORT_INF VS10XX_XDCS_PORT = {GPIOC, GPIO_Pin_6};
    const PORT_INF VS10XX_SPI_PORT = {GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15};

    const PORT_INF VS10XX_PA_PORT = {GPIOC, GPIO_Pin_9};


#endif


void VS10XX_InitInterface(void)
{
#ifdef _ANYID_STM32_
    GPIO_InitTypeDef  GPIO_InitStructure;

    //只有当MP3_DREQ为高电平时，VS1003可以获得32字节的SDI或一个SCI*
    GPIO_InitStructure.GPIO_Pin = VS10XX_DREQ_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(VS10XX_DREQ_PORT.Port, &GPIO_InitStructure);

    //MP3_XCS:PINA2
    GPIO_InitStructure.GPIO_Pin = VS10XX_XCCS_PORT.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(VS10XX_XCCS_PORT.Port, &GPIO_InitStructure);

    //MP3_RESET:PINA3
    GPIO_InitStructure.GPIO_Pin = VS10XX_RST_PORT.Pin;
    GPIO_Init(VS10XX_RST_PORT.Port, &GPIO_InitStructure);

    //MP3_XDCS:PINA10
    GPIO_InitStructure.GPIO_Pin = VS10XX_XDCS_PORT.Pin;
    GPIO_Init(VS10XX_XDCS_PORT.Port, &GPIO_InitStructure);
#if defined(_ANYID_STM32_BCTRL_)
    //外接PA
    GPIO_InitStructure.GPIO_Pin = VS10XX_PA_PORT.Pin;
    GPIO_Init(VS10XX_PA_PORT.Port, &GPIO_InitStructure);
#endif
    //spi
    GPIO_InitStructure.GPIO_Pin = VS10XX_SPI_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(VS10XX_SPI_PORT.Port, &GPIO_InitStructure);
#endif
}

void VS10XX_InitSPI(u8 speed)
{
#ifdef _ANYID_STM32_
    SPI_InitTypeDef   SPI_InitStructure;

    SPI_Cmd(VS10XX_SPI, DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    if(speed == 1)
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    else if(speed == 2)
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

    SPI_Init(VS10XX_SPI, &SPI_InitStructure);

    SPI_Cmd(VS10XX_SPI, ENABLE);
#endif
}


u8 VS10XX_WriteByte(u8 byte)
{
#ifdef _ANYID_STM32_
    while (SPI_GetFlagStatus(VS10XX_SPI, SPI_FLAG_TXE) == RESET);
    SPI_SendData(VS10XX_SPI, byte);
    while (SPI_GetFlagStatus(VS10XX_SPI, SPI_FLAG_RXNE) == RESET);
    return SPI_ReceiveData(VS10XX_SPI);
#endif
}

