#include "AnyID_Uart_HL.h"

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MTD_) | (defined(_ANYID_STM32_MPD_)))

    #define UART_CLK                RCC_APB2Periph_USART1
    #define UART_INT_CHANNEL        USART1_IRQChannel
    #define UART_BAUND_RATE         115200

    const PORT_INF UART_PORT_TX = {GPIOA, GPIO_Pin_9};
    const PORT_INF UART_PORT_RX = {GPIOA, GPIO_Pin_10};


#elif (defined(_ANYID_PIC18_HSR100_) | defined(_ANYID_PIC18_BWCOM_))

    #define UART_SPBRG_VAL(bd)          (((SYS_CLOCK_FREQ / (bd)) / 16) - 1)
    #define UARTIsTxReady()         (TXSTAbits.TRMT)
    #define UARTIsRxReady()         (PIR1bits.RCIF)

#endif


void UART_Init(u32 uart, u32 baudRate)
{
#ifdef _ANYID_STM32_
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    PORT_INF uartTxPort, uartRxPort;

    USART_DeInit((USART_TypeDef *)uart);

    if(((USART_TypeDef *)uart) == USART1)
    {
        uartTxPort.Port = GPIOA;
        uartTxPort.Pin = GPIO_Pin_9;
        uartRxPort.Port = GPIOA;
        uartRxPort.Pin = GPIO_Pin_10;
    }
    else if(((USART_TypeDef *)uart) == USART2)
    {
        uartTxPort.Port = GPIOA;
        uartTxPort.Pin = GPIO_Pin_2;
        uartRxPort.Port = GPIOA;
        uartRxPort.Pin = GPIO_Pin_3;
    }
    else if(((USART_TypeDef *)uart) == USART3)
    {
        uartTxPort.Port = GPIOB;
        uartTxPort.Pin = GPIO_Pin_10;
        uartRxPort.Port = GPIOB;
        uartRxPort.Pin = GPIO_Pin_11;
    }


    GPIO_InitStructure.GPIO_Pin = uartTxPort.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(uartTxPort.Port, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = uartRxPort.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(uartRxPort.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Clock = USART_Clock_Disable;
    USART_InitStructure.USART_CPOL = USART_CPOL_Low;
    USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

    //Configure USART
    USART_Init(((USART_TypeDef *)uart), &USART_InitStructure);
    // Enable the USART
    USART_Cmd(((USART_TypeDef *)uart), ENABLE);

#elif (defined(_ANYID_PIC18_))

    TXSTA = 0x24;
    RCSTA = 0x90;       // 0b10010000;
    SPBRG = UART_SPBRG_VAL(baudRate);

#endif

}


void UART_WriteByte(u32 uart, u8 ch)
{
#ifdef _ANYID_STM32_

    while((((USART_TypeDef *)uart)->SR & USART_FLAG_TC) == (u16)RESET);
	((USART_TypeDef *)uart)->DR = (ch & (u16)0x01FF);

#elif (defined(_ANYID_PIC18_))

    while(!UARTIsTxReady());
    TXREG = ch;

#endif

}


void UART_ConfigInt(u32 uart)
{
#ifdef _ANYID_STM32_

    NVIC_InitTypeDef NVIC_InitStructure;

    if(((USART_TypeDef *)uart) == USART1)
    {
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
    }
    else if(((USART_TypeDef *)uart) == USART2)
    {
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
    }
    if(((USART_TypeDef *)uart) == USART3)
    {
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
    }


    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

#elif (defined(_ANYID_PIC18_))

    //接收和发送都是高优先级中断
    IPR1bits.RCIP = 1;
    IPR1bits.TXIP = 0;


#endif

}

void UART_EnableInt(u32 uart, FunctionalState rxState, FunctionalState txState)
{
#ifdef _ANYID_STM32_

	USART_ClearITPendingBit(((USART_TypeDef *)uart), USART_IT_RXNE | USART_IT_TXE);
	
    USART_ITConfig(((USART_TypeDef *)uart), USART_IT_RXNE, rxState);
    USART_ITConfig(((USART_TypeDef *)uart), USART_IT_TXE, txState);

#elif (defined(_ANYID_PIC18_))

    if(rxState == ENABLE)
    {
        PIE1bits.RCIE = 1;
    }
    else
    {
        PIE1bits.RCIE = 0;
    }

    if(txState == ENABLE)
    {
        PIE1bits.TXIE = 1;
    }
    else
    {
        PIE1bits.TXIE = 0;
    }
#endif

}


