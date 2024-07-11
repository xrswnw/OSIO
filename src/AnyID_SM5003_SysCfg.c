#include "AnyID_SM5003_SysCfg.h"

u32 g_nSysState = 0;
u32 g_nSysRecordTick = 0;
SYS_LEDINFO g_sSysLedInfo = {0};



#if SYS_ENABLE_WDT
    #define SYS_ENABLE_TEST         0
#else
    #define SYS_ENABLE_TEST         1
#endif

void Sys_Delayms(u32 n)
{
    n *= 0x6000;
    n++;
    while(n--);
}

void Sys_CfgClock(void)
{
    ErrorStatus HSEStartUpStatus = ERROR;

    RCC_DeInit();
    //Enable HSE
    RCC_HSEConfig(RCC_HSE_ON);

    //Wait till HSE is ready
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if(HSEStartUpStatus == SUCCESS)
    {
        //HCLK = SYSCLK = 72.0M
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        //PCLK2 = HCLK = 72.0M
        RCC_PCLK2Config(RCC_HCLK_Div1);

        //PCLK1 = HCLK/2 = 33.9M
        RCC_PCLK1Config(RCC_HCLK_Div2);

        //ADCCLK = PCLK2/8
        RCC_ADCCLKConfig(RCC_PCLK2_Div8);

        // Select USBCLK source 72 / 1.5 = 48M
        RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

        //Flash 2 wait state
        FLASH_SetLatency(FLASH_Latency_2);

        //Enable Prefetch Buffer
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        //PLLCLK = 12.00MHz * 6 = 72 MHz
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);

        //Enable PLL
        RCC_PLLCmd(ENABLE);

        //Wait till PLL is ready
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        //Select PLL as system clock source
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        //Wait till PLL is used as system clock source
        while(RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
}

void Sys_CfgPeriphClk(FunctionalState state)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, state);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 |
                           RCC_APB1Periph_TIM4 |
                           RCC_APB1Periph_TIM2, state);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_ADC1 |
                           RCC_APB2Periph_USART1 |
                           RCC_APB2Periph_AFIO, state);
}

void Sys_CfgNVIC(void)
{
    //NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM
    //Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  //VECT_TAB_FLASH
    //Set the Vector Table base location at 0x08000000
#ifdef _ANYID_BOOT_STM32_
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
#else
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
#endif

    //Configure the Priority Group to 2 bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


const PORT_INF SYS_ADDR_COM0 = {GPIOB, GPIO_Pin_13};
const PORT_INF SYS_ADDR_COM1 = {GPIOB, GPIO_Pin_14};
const PORT_INF SYS_ADDR_COM2 = {GPIOB, GPIO_Pin_15};
u16 Sys_GetDeviceAddr()
{
    u16 addr = 0, count = 0, tempAddr = 0;
	while(count < 21)
	{
		if((SYS_ADDR_COM0.Port->IDR & SYS_ADDR_COM0.Pin) == (uint32_t)Bit_RESET)
		{
			tempAddr |= 0x0001;
		}
		
		if((SYS_ADDR_COM1.Port->IDR & SYS_ADDR_COM1.Pin) == (uint32_t)Bit_RESET)
		{
			tempAddr |= 0x0002;
		}
		
		if((SYS_ADDR_COM2.Port->IDR & SYS_ADDR_COM2.Pin) == (uint32_t)Bit_RESET)
		{
			tempAddr |= 0x0004;
		}
		
		if(addr == tempAddr)
		{
			count++;
		}
		else
		{	
			count = 0;
			addr = tempAddr;
		}
	}
	
	return addr;
}

const PORT_INF SYS_RUNLED_COM = {GPIOB, GPIO_Pin_11};
const PORT_INF SYS_ALARMLED_COM = {GPIOB, GPIO_Pin_12};
const PORT_INF SYS_PROTECT_COM = {GPIOA, GPIO_Pin_7};
void Sys_CtrlIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    //RUN LED
    GPIO_InitStructure.GPIO_Pin = SYS_RUNLED_COM.Pin;
    GPIO_Init(SYS_RUNLED_COM.Port, &GPIO_InitStructure); 
    
    //ALARM LED
    GPIO_InitStructure.GPIO_Pin = SYS_ALARMLED_COM.Pin;
    GPIO_Init(SYS_ALARMLED_COM.Port, &GPIO_InitStructure);
    
    //PROTECT COM
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = SYS_PROTECT_COM.Pin;
    GPIO_Init(SYS_PROTECT_COM.Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SYS_ADDR_COM0.Pin | SYS_ADDR_COM1.Pin | SYS_ADDR_COM2.Pin;
	GPIO_Init(SYS_ADDR_COM0.Port, &GPIO_InitStructure);
}


void Sys_Init(void)
{
    Sys_CfgClock();
    Sys_CfgNVIC();
    Sys_CfgPeriphClk(ENABLE);
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );			//���������ȼ�λ��ָ��Ϊ��ռ���ȼ�λ�� �������κ����ȼ�λ��Ϊ�����ȼ�λ
#if SYS_ENABLE_WDT
    WDG_InitIWDG();
#endif
	Sys_Delayms(500);
    Sys_Delayms(500);
    Sys_Delayms(500);
    Sys_Delayms(500);
    Sys_DisableInt();
    
    Device_CtrlIOInit();
    
    Sys_EnableInt();
}


 


/*
void Sys_R485Task(void)
{
    if(USART_GetFlagStatus(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);

        R485_Stop();
        Sys_Delayms(10);
        R485_Init();
    }

    if(Uart_IsRcvFrame(g_sR485RcvFrame) && g_sR485TxFrame.state == UART_STAT_TX_IDLE)
    {
        BOOL bTx = FALSE;
        memcpy(&g_sUartTempRcvFrame, &g_sR485RcvFrame, sizeof(UART_RCVFRAME));
        Uart_ResetFrame(g_sR485RcvFrame);

        if(g_sUartTempRcvFrame.len >= UART_FRAME_MIN_LEN)
        {
            u16 startPos = 0;
            if(Uart_UsrCheckFrame(&g_sUartTempRcvFrame, &startPos, g_sDeviceParams.addr))
            {
                g_sR485TxFrame.len = Device_ProcessUsrFrame(g_sUartTempRcvFrame.buffer + startPos);
                if(g_sR485TxFrame.len)
                {
                    g_sR485TxFrame.state = UART_STAT_TX_BUSY;
                    g_sR485TxFrame.tick = g_nSysTick;
                    R485_EnableTxDma(g_sR485TxFrame.buffer, g_sR485TxFrame.len);
                    if(g_sR485TxFrame.buffer[UART_FRAME_POS_CMD + 1] == UART_FRAME_CMD_RST)
                    {
                        Sys_Delayms(5);
                        Sys_SoftReset();
                    }
                    bTx = TRUE;
                }
            }
        }
        if(bTx == FALSE)
        {
            R485_EnableRxDma();
        }
    }

    if(g_sR485TxFrame.state == UART_STAT_TX_BUSY)
    {
        if(g_sR485TxFrame.tick + UART_TX_TIMEOUT < g_nSysTick)
        {
            g_sR485TxFrame.state = UART_STAT_TX_IDLE;
            R485_DisableTxDma();
            R485_EnableRxDma();
        }
    }
}
*/

//-----------------------
u8 g_nTempBuffer[2];
void Sys_TaskScheduler()
{
   //xTaskCreate(Device_LedTask, DEVICE_TASK_LED, configMINIMAL_STACK_SIZE, NULL, 0, NULL);

    
    
    
    
    
    //vTaskStartScheduler();              //����������
    Device_ProcessUsrFrame(g_nTempBuffer);

}