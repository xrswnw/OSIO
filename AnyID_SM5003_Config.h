#ifndef _ANYID_M345_CONFIG_
#define _ANYID_M345_CONFIG_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"
#include <stdio.h>
//设定工作时钟
#define SYS_CLOCK_FREQ              72000000

//STM32平台相关
#if (defined(_ANYID_STM32_) || defined(_ANYID_GD32_))
    #include "stm32f10x.h"

    //端口定义
    typedef struct Port_Inf
    {
        GPIO_TypeDef* Port;
        u16 Pin;
    } PORT_INF;
#endif
#define SYS_ENABLE_WDT                  0
#define STM32_CPUID_LEN                 12


#ifdef USE_FREE_RTOS
    #include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
#endif

//优先级分组是2:2
#define INT_PRIORITY_GROUP              2
#define INT_PRIORITY_STICK              0  //STICK的优先级最低
#define INT_PRIORITY_UART_RX            14  //串口接收
#define INT_PRIORITY_UART_DMA           8  //串口发送DMA
#define INT_PRIORITY_PCD_IO             4   //15693的发送定时器优先级最高

#define Sys_EnableInt()                 __enable_irq()
#define Sys_DisableInt()                __disable_irq()

#define Sys_GetCpuId(id)                memcpy((id), (u8 *)(0x1ffff7e8), STM32_CPUID_LEN)
#define Sys_SoftReset()                 (*((u32 *)0xE000ED0C) = 0x05fa0004)
#endif
