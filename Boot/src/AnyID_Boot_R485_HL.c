#include "AnyID_Boot_R485_HL.h"


#define R485_INT_CHANNEL        USART1_IRQn
const PORT_INF R485_PORT_TX = {GPIOA, GPIO_Pin_9};
const PORT_INF R485_PORT_RX = {GPIOA, GPIO_Pin_10};
const PORT_INF R485_PORT_CTRL = {GPIOA, GPIO_Pin_11};
void R485_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = R485_PORT_CTRL.Pin;
    GPIO_Init(R485_PORT_CTRL.Port, &GPIO_InitStructure);
    R485_EnableRx();

    USART_DeInit(R485_PORT);

    GPIO_InitStructure.GPIO_Pin = R485_PORT_TX.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(R485_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = R485_PORT_RX.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(R485_PORT_RX.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    //Configure USART
    USART_Init(R485_PORT, &USART_InitStructure);
    // Enable the USART
    USART_Cmd(R485_PORT, ENABLE);
}

void R485_ConfigInt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = R485_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void R485_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(R485_PORT, USART_IT_RXNE | USART_IT_TC);

    USART_ITConfig(R485_PORT, USART_IT_RXNE, rxState);
    USART_ITConfig(R485_PORT, USART_IT_TC, txState);
}

void R485_WriteByte(u8 ch)
{
	(R485_PORT)->DR = (ch & (u16)0x01FF);
    while(((R485_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
}

void R485_Delayms(u32 n)             //œµÕ≥—” ±n∫¡√Î
{
    n *= 0x3800;
    n++;
    while(n--);
}

void R485_WriteBuffer(u8 *pBuffer, u32 len)
{
    u32 i = 0;
    R485_EnableTx();
    R485_Delayms(1);
    for(i = 0; i < len; i++)
    {
        R485_WriteByte(pBuffer[i]);
    }
    R485_Delayms(1);
    R485_EnableRx();
}

