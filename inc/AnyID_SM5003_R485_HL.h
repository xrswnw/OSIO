
#ifndef _ANYID_SM5003_R485_HL_
#define _ANYID_SM5003_R485_HL_

#include "AnyID_SM5003_Config.h"

#define R485_PORT                           USART1
#define R485_BAUDRARE                       2000000//115200
#define R485_IRQHandler                     USART1_IRQHandler
#define R485_ChkTxOver()                    while(((R485_PORT)->SR & USART_FLAG_TC) == (u16)RESET)


extern const PORT_INF R485_PORT_CTRL;
#define R485_EnableRx()                     (R485_PORT_CTRL.Port->BRR  = R485_PORT_CTRL.Pin)
#define R485_EnableTx()                     (R485_PORT_CTRL.Port->BSRR = R485_PORT_CTRL.Pin)

#define R485_ReadByte()                     ((u16)(R485_PORT->DR & (u16)0x01FF))

                                                
#define R485_SR_IDLE                        0x0010  
#define R485_DMA                            DMA1

#define R485_TXDMA_CH                       DMA1_Channel4
#define R485_TXDMA_INT                      DMA1_Channel4_IRQn
#define R485_TxDMAIRQHandler                DMA1_Channel4_IRQHandler
#define R485_TXDMA_TC_FLAG                  DMA1_FLAG_TC4

#define R485_RXDMA_CH                       DMA1_Channel5
#define R485_RXDMA_INT                      DMA1_Channel5_IRQn
#define R485_RxDMAIRQHandler                DMA1_Channel5_IRQHandler
#define R485_RXDMA_TC_FLAG                  DMA1_FLAG_TC5
#define R485_EnableRxDma()                  do{\
                                                (R485_DMA)->IFCR = R485_RXDMA_TC_FLAG; \
                                                (R485_RXDMA_CH)->CNDTR = UART_BUFFER_MAX_LEN; \
                                                (R485_RXDMA_CH)->CCR |= CCR_ENABLE_Set; \
                                            }while(0)
                                            
#define R485_GetRxLen()                     (UART_BUFFER_MAX_LEN - (R485_RXDMA_CH)->CNDTR)  

#define R485_DisableRxDma()                 do{\
                                                (R485_DMA)->IFCR = R485_RXDMA_TC_FLAG;\
                                                (R485_RXDMA_CH)->CCR &= CCR_ENABLE_Reset;\
                                            }while(0)
                                            

void R485_InitInterface(u32 baudrate);

void R485_ConfigInt(FunctionalState state);

void R485_InitTxDma(u8 *pTxBuffer, u32 len);
void R485_InitRxDma(u8 *pRxBuffer, u32 len);
void R485_EnableTxDma(u8 *p, u16 len);                                                                                          
void R485_DisableTxDma(void);

void R485_Delayms(u32 n);
void R485_WriteByte(u8 ch);
void R485_WriteBuffer(u8 *pBuffer, u32 len);

void R485_WriteStr(char *pBuffer);
#endif
