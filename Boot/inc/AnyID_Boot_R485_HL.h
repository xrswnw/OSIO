#ifndef _ANYID_BOOT_R485_HL_
#define _ANYID_BOOT_R485_HL_

#include "AnyID_Boot_Config.h"

#define R485_PORT                       USART1
#define R485_IRQHandler                 USART1_IRQHandler
#define R485_BAUDRARE                   115200
#define R485_ChkTxOver()                while(((R485_PORT)->SR & USART_FLAG_TC) == (u16)RESET)

extern const PORT_INF R485_PORT_CTRL;
#define R485_EnableRx()         (R485_PORT_CTRL.Port->BRR = R485_PORT_CTRL.Pin)
#define R485_EnableTx()         (R485_PORT_CTRL.Port->BSRR = R485_PORT_CTRL.Pin)

#define R485_ReadByte()                 ((u16)(R485_PORT->DR & (u16)0x01FF))

void R485_InitInterface(u32 baudrate);

void R485_ConfigInt(void);
void R485_EnableInt(FunctionalState rxState, FunctionalState txState);

void R485_WriteByte(u8 ch);
void R485_WriteBuffer(u8 *pBuffer, u32 len);
#endif
