#include "AnyID_SPIFLASH_HL.h"

//#if defined(_ANYID_STM32_BCTRL_)

    const PORT_INF SPIFLASH_CS = {GPIOB, GPIO_Pin_12};
    const PORT_INF SPIFLASH_SPI_PORT = {GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15};
//#endif

void SPIFLASH_InitInterface(void)
{
//#if defined(_ANYID_STM32_)
    SPI_InitTypeDef   SPI_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;


	//这里为什么要初始化CS引脚才可以？？
	//是不是和GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz有关系？
    //cs
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = SPIFLASH_CS.Pin;
    GPIO_Init(SPIFLASH_CS.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIFLASH_SPI_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPIFLASH_SPI_PORT.Port, &GPIO_InitStructure);

    SPI_Cmd(SPIFLASH_SPI, DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; //6M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIFLASH_SPI, &SPI_InitStructure);

    SPI_Cmd(SPIFLASH_SPI, ENABLE);
//#endif
}

u8 SPIFLASH_WriteByte(u8 byte)
{
//#if defined(_ANYID_STM32_)
    if(SPI_I2S_GetFlagStatus(SPIFLASH_SPI, SPI_I2S_FLAG_RXNE) == SET)
    {
        SPI_I2S_ReceiveData(SPIFLASH_SPI);
    }

    while(SPI_I2S_GetFlagStatus(SPIFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPIFLASH_SPI, byte);
    while(SPI_I2S_GetFlagStatus(SPIFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

    return SPI_I2S_ReceiveData(SPIFLASH_SPI);
//#endif
}

