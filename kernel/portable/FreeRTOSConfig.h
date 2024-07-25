/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
#ifdef __ICCARM__
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif /* __ICCARM__ */

/* The following definition allows the startup files that ship with the IDE
to be used without modification when the chip used includes the PMU CM001
errata. */
#define WORKAROUND_PMU_CM001					1


#define configUSE_PREEMPTION					1									//1:抢占式调度；0:协同式调度
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1                                   //优先级调度算法。1:硬件算法调度，速率快，最大支持优先级31；0:软件算法调度，优先级不限，速率相对慢
#define configUSE_TIME_SLICING					1									//同优先级轮流执行
#define configIDLE_SHOULD_YIELD					1									//空闲任务是否让步(每执行一次空闲任务，查看一次是否有高优先级任务)


#define configSUPPORT_DYNAMIC_ALLOCATION		1									//使用动态任务创建方式（堆栈由系统分配）
#define configSUPPORT_STATIC_ALLOCATION			0									//不使用静态任务创建方式（堆栈自身分配）、使能空闲任务、定时器(可选)需存在

#define configUSE_IDLE_HOOK						0									//空闲任务狗子
#define configUSE_TICK_HOOK						0                                   //滴答定时器狗子
#define configCPU_CLOCK_HZ						( SystemCoreClock )					//对应MCU主频
#define configTICK_RATE_HZ						( ( TickType_t ) 1000 )				//系统运行时基，可相应修改，调整运行速度（时间片）
#define configMAX_PRIORITIES					( 31 )                              //32位优先级
#define configMINIMAL_STACK_SIZE				( ( unsigned short ) 128 )			//空闲任务堆栈大小
#define configTOTAL_HEAP_SIZE					( ( size_t ) ( 10 * 1024 ) )		//堆空间
#define configMAX_TASK_NAME_LEN					( 20 )								//任务描述名长度
#define configUSE_TRACE_FACILITY				1                                   //可视化函数跟踪,获取任务状态信息
#define configUSE_16_BIT_TICKS					0                                   //滴答定时器范围。1 ：u16; 0 : u32
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS             //滴答“s”执行的次数。32位
#define configUSE_TICKLESS_IDLE                 0                                   //低功耗模式
#define configUSE_MUTEXES						1
#define configQUEUE_REGISTRY_SIZE				8
//#define configCHECK_FOR_STACK_OVERFLOW			2
#define configUSE_RECURSIVE_MUTEXES				1
#define configUSE_MALLOC_FAILED_HOOK			0                                   //malloc函数回调。1:启用，需编写回调函数；0:不启用   void vApplicationMallocFailedHook( void );
#define configUSE_APPLICATION_TASK_TAG			0
#define configUSE_COUNTING_SEMAPHORES			1
#define configGENERATE_RUN_TIME_STATS			1                               //统计任务运行时间

#if configGENERATE_RUN_TIME_STATS
    //#include "../periph/inc/periph.h"
    extern uint32_t FreeRTOSRunTimeTicks;
    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()    ConfigureTimeForRunTimeStats()                        //初始化计时任务的时基定时器
    #define portGET_RUN_TIME_COUNTER_VALUE()            FreeRTOSRunTimeTicks	//获取时间统计时间值
#endif


#define configUSE_QUEUE_SETS                            1
#define configUSE_STATS_FORMATTING_FUNCTIONS             1                              //统计任务运行时间  vTaskList()


/* Software timer definitions. */
#define configUSE_TIMERS				1											//静态任务创建需要定时器任务
#define configTIMER_TASK_PRIORITY		( 2 )
#define configTIMER_QUEUE_LENGTH		5
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0                                   //定时器首次任务时回调。1:启用，需编写回调函数；0:不启用 
/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1                                //任务优先级设置
#define INCLUDE_uxTaskPriorityGet		1                               //获取任务优先级是能
#define INCLUDE_vTaskDelete				1											//任务删除允许 
#define INCLUDE_uxTaskGetStackHighWaterMark		1									//允许获取任务堆栈历史剩余最小值
#define INCLUDE_vTaskCleanUpResources	1
#define INCLUDE_vTaskSuspend			1											//允许挂起
#define INCLUDE_xTaskResumeFromISR		1											//允许中断中调用挂起解卦
#define INCLUDE_vTaskDelayUntil			1											//允许任务绝对时间堵塞
#define INCLUDE_vTaskDelay				1											//允许任务堵塞
#define INCLUDE_xTaskAbortDelay         1                                           //允许任务强制离开阻塞态

#define INCLUDE_xTaskGetHandle          1       //获取任务句柄
/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 63 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			15					//freertos管理的最低中断优先级						及中断服务函数抢占优先级须在最高和最低之间

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5					//freertos管理的最高中断优先级

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS();for( ;; ); }

//#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); printf( "Assert failed! File: %s, Line: %d\n", __FILE__, __LINE__ ); for( ;; ); }
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names.  WORKAROUND_PMU_CM001 is defined at the top of this file. */
#define vPortSVCHandler SVC_Handler
#define xPortSysTickHandler SysTick_Handler
#define xPortPendSVHandler PendSV_Handler



#endif /* FREERTOS_CONFIG_H */

