#include "AnyID_SM5003_R485.h"

UART_RCVFRAME g_sR485RcvFrame = {0};
UART_TXFRAME g_sR485TxFrame = {0};

void R485_Init(void)
{
    R485_InitInterface(R485_BAUDRARE);
    R485_ConfigInt(ENABLE);
    R485_InitTxDma(g_sR485TxFrame.buffer, UART_BUFFER_MAX_LEN);
    R485_InitRxDma(g_sR485RcvFrame.buffer, UART_BUFFER_MAX_LEN);
    
    R485_EnableRxDma();
}

void R485_Stop(void)
{
    (R485_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    (R485_TXDMA_CH)->CCR &= CCR_ENABLE_Reset;
    R485_ConfigInt(DISABLE);
    
    USART_ITConfig(R485_PORT, USART_IT_IDLE, DISABLE);
    USART_DMACmd(R485_PORT, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(R485_PORT, USART_DMAReq_Rx, DISABLE);
    USART_Cmd(R485_PORT, DISABLE);
    USART_DeInit(R485_PORT);
}