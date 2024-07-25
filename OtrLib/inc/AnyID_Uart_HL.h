#ifndef _ANYID_UART_HL_
#define _ANYID_UART_HL_

#include "AnyID_Config.h"

#if (defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MTD_) | defined(_ANYID_STM32_MPD_))

    #define UART_USART USART1

#elif (defined(_ANYID_STM32_BCTRL_) | defined(_ANYID_STM32_BBOOT_))

    #define UART_GPS_PORT           USART1
    #define UART_GPRS_PORT          USART2
    #define UART_WIRELESS_PORT      USART3

    #define UART_GPS_BAUDRARE       4800
    #define UART_GPRS_BAUDRARE      9600
    #define UART_WIRELESS_BAUDRARE  9600

#elif (defined(_ANYID_PIC18_HSR100_) | defined(_ANYID_PIC18_BWCOM_))

    #define UART_BAUND_RATE         115200              //注意:8M的时钟不支持115200
    #define UART_PORT               0

#endif


#if (defined(_ANYID_STM32_))
    #define UART_ReadByte(uart)     USART_ReceiveData(uart)
#elif (defined(_ANYID_PIC18_))
    #define UART_ReadByte(uart)     (RCREG)
#endif


void UART_Init(u32 uart, u32 baudRate);
void UART_WriteByte(u32 uart, u8 ch);
void UART_EnableInt(u32 uart, FunctionalState rxState, FunctionalState txState);
void UART_ConfigInt(u32 uart);

#endif
