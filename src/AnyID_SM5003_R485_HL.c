#include "AnyID_SM5003_R485_HL.h"

#define R485_INT_CHANNEL        USART1_IRQn
const PORT_INF R485_PORT_TX = {GPIOA, GPIO_Pin_9};
const PORT_INF R485_PORT_RX = {GPIOA, GPIO_Pin_10};
const PORT_INF R485_PORT_CTRL = {GPIOA, GPIO_Pin_11};
void R485_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;        //GPIO_Mode_IN_FLOATING;
    GPIO_Init(R485_PORT_RX.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    // Configure USART
    USART_Init(R485_PORT, &USART_InitStructure);
    // Enable the USART
    USART_Cmd(R485_PORT, ENABLE);
    USART_ITConfig(R485_PORT, USART_IT_IDLE, ENABLE);
    USART_DMACmd(R485_PORT, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(R485_PORT, USART_DMAReq_Rx, ENABLE);
}

void R485_ConfigInt(FunctionalState state)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = R485_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = state;

    NVIC_Init(&NVIC_InitStructure);
}

void R485_InitTxDma(u8 *pTxBuffer, u32 len)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    //DMA1通道4配置
    DMA_DeInit(R485_TXDMA_CH);
    //外设地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(R485_PORT->DR));
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pTxBuffer;
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = len;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //设置DMA的传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(R485_TXDMA_CH, &DMA_InitStructure);

    DMA_ITConfig(R485_TXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
    R485_DisableTxDma();// 关闭DMA
    
    //dma interrupt
    NVIC_InitStructure.NVIC_IRQChannel = R485_TXDMA_INT;                                  //
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void R485_InitRxDma(u8 *pRxBuffer, u32 len)
{
    NVIC_InitTypeDef  NVIC_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};


    DMA_DeInit(R485_RXDMA_CH);
    //外设地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(R485_PORT->DR));
    
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pRxBuffer;
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = len;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //设置DMA的传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    
    DMA_Init(R485_RXDMA_CH, &DMA_InitStructure);
    DMA_ITConfig(R485_RXDMA_CH, DMA_IT_TC, ENABLE);  //open DMA send inttrupt
    R485_DisableRxDma();// 关闭DMA
    
    //dma interrupt
    NVIC_InitStructure.NVIC_IRQChannel = R485_RXDMA_INT;   


    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_DMA >> 2;    //
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_DMA & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void R485_EnableTxDma(u8 *p, u16 len)              
{
    R485_EnableTx();
    (R485_DMA)->IFCR = R485_TXDMA_TC_FLAG; 
    (R485_TXDMA_CH)->CMAR = ((u32)(p)); 
    (R485_TXDMA_CH)->CNDTR = len; 
    (R485_TXDMA_CH)->CCR |= CCR_ENABLE_Set; 
}
                                                                                             
void R485_DisableTxDma(void)          
{
    (R485_DMA)->IFCR = R485_TXDMA_TC_FLAG;
    (R485_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    R485_ChkTxOver();
    R485_EnableRx();
}

void R485_WriteByte(u8 ch)
{
	(R485_PORT)->DR = (ch & (u16)0x01FF);
    while(((R485_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
}

void R485_Delayms(u32 n)             //系统延时n毫秒
{
    n *= 0x6000;
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
    R485_ChkTxOver();
    R485_Delayms(1);
    R485_EnableRx();
}