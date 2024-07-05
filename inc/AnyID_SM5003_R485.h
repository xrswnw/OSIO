#ifndef _ANYID_SM5003_R485_
#define _ANYID_SM5003_R485_

#include "AnyID_SM5003_R485_HL.h"
#include "AnyID_Uart_Receive.h"

extern UART_RCVFRAME g_sR485RcvFrame;
extern UART_TXFRAME g_sR485TxFrame;

void R485_Init(void);
void R485_Stop(void);

#endif
