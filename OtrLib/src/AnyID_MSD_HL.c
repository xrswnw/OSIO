#include "AnyID_MSD_HL.h"

#ifdef _ANYID_STM32_MTD_
#define MSD_SPI             SPI1

const PORT_INF MSD_SPI_Port = {GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7};
const PORT_INF MSD_SPI_MISO = {GPIOA, GPIO_Pin_6};
const PORT_INF MSD_CS = {GPIOA, GPIO_Pin_4};
const PORT_INF MSD_Power = {GPIOC, GPIO_Pin_1};

#endif

void MSD_WriteByte(u8 byte)
{
#ifdef _ANYID_STM32_
    if(SPI_GetFlagStatus(MSD_SPI, SPI_FLAG_RXNE) == SET)
    {
        SPI_ReceiveData(MSD_SPI);
    }

    while(SPI_GetFlagStatus(MSD_SPI, SPI_FLAG_TXE) == RESET);
    SPI_SendData(MSD_SPI, byte);
    while(SPI_GetFlagStatus(MSD_SPI, SPI_FLAG_RXNE) == RESET);
    SPI_ReceiveData(MSD_SPI);
#endif
}

u8 MSD_ReadByte(void)
{
    u8 data = 0;

#ifdef _ANYID_STM32_
    if(SPI_GetFlagStatus(MSD_SPI, SPI_FLAG_RXNE) == SET)
    {
        SPI_ReceiveData(MSD_SPI);
    }

    while(SPI_GetFlagStatus(MSD_SPI, SPI_FLAG_TXE) == RESET);
    SPI_SendData(MSD_SPI, DUMMY);

    while(SPI_GetFlagStatus(MSD_SPI, SPI_FLAG_RXNE) == RESET);
    data = SPI_ReceiveData(MSD_SPI);
#endif

    return data;
}

void MSD_InterfaceInit(void)
{
#ifdef _ANYID_STM32_
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = MSD_SPI_Port.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(MSD_SPI_Port.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MSD_SPI_MISO.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(MSD_SPI_MISO.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MSD_CS.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MSD_CS.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MSD_Power.Pin;
    GPIO_Init(MSD_Power.Port, &GPIO_InitStructure);

    SPI_Cmd(MSD_SPI, ENABLE);
#endif
}

void MSD_LowPower(LPMode lp)
{
    if(lp == LPM_SLEEP || lp == LPM_OFF)
    {
#ifdef _ANYID_STM32_
        GPIO_InitTypeDef  GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Pin = MSD_SPI_Port.Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
        GPIO_Init(MSD_SPI_Port.Port, &GPIO_InitStructure);

        if(lp == LPM_SLEEP)
        {
            GPIO_InitStructure.GPIO_Pin = MSD_CS.Pin;
            GPIO_Init(MSD_CS.Port, &GPIO_InitStructure);
        }
#endif
    }
    else
    {
        MSD_InterfaceInit();
    }
}

void MSD_SPILowSpeed(void)
{
#ifdef _ANYID_STM32_
    SPI_InitTypeDef   SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(MSD_SPI, &SPI_InitStructure);
#endif
}

void MSD_SPIHighSpeed(void)
{
#ifdef _ANYID_STM32_
    SPI_InitTypeDef   SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(MSD_SPI, &SPI_InitStructure);
#endif
}
