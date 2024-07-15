/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "AnyID_SM5003_SysCfg.h"
#include "AnyID_SM5003_Reader.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*
void SVC_Handler(void)
{
}
*/
/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*
void PendSV_Handler(void)
{
}
*/
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*
void SysTick_Handler(void)
{
    g_nSysTick++;
    if(g_nSysTick % 5 == 0)
	{
		a_SetStateBit(g_nSysState, SYS_STAT_RUNLED);
	}
	
	if(g_nSysTick % 21 == 0)
	{
		a_SetStateBit(g_nSysState,  SYS_STAT_ADDRECORD | SYS_STAT_AD | SYS_STAT_IN_SENSOR | SYS_STAT_CTLOUTPUT);
	}
}
*/

u16 g_nCountSensor_Count;
void EXTI9_5_IRQHandler()
{
    BaseType_t xYieldRequired = pdFALSE;
    if (EXTI_GetITStatus(EXTI_Line6) == SET)
	{
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
        {
            g_nCountSensor_Count ++;
            printf("ISR OP, Resume Task2\r\n");
            xYieldRequired = xTaskResumeFromISR(Device_Task2Handle);
            if(xYieldRequired == pdTRUE)
            {
                portYIELD_FROM_ISR( xYieldRequired );           //上下文切换，如果任务正在A在运行时进入中断，此时解除高优先级任务B，此时ISR结束后立即运行任务B，无需等待
            }
            g_nKeyValue = 0;
        }
        else
        {
            g_nKeyValue = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
}

void R485_RxDMAIRQHandler(void)
{
    R485_DisableRxDma(); //接收缓冲区满，一般不会出现这样的情况，如果有，就可能系统有故障
    g_sR485RcvFrame.len = R485_GetRxLen(); 
    g_sR485RcvFrame.state = UART_STAT_RX_OVR;    //接收数据完成
}

void R485_TxDMAIRQHandler(void)
{
    R485_DisableTxDma();                                             //DMA完成后，最后一个字节可能没有发送出去，需要在主循环中做判断处理
    R485_EnableRxDma();                                              //使能接收
    g_sR485TxFrame.state = UART_STAT_TX_IDLE;
}

u16 g_nR485Sr = 0;
u16 g_nR485Dr = 0;
void R485_IRQHandler(void)
{
    if(R485_PORT->SR & R485_SR_IDLE)
    {    
        R485_DisableRxDma();
        g_sR485RcvFrame.len = R485_GetRxLen(); 
        g_sR485RcvFrame.state = UART_STAT_RX_END;    //接收数据完成
    }

    g_nR485Sr = R485_PORT->SR;  //通过读取SR和DR清空中断标志
    g_nR485Dr = R485_PORT->DR;    
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
