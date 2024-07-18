#include "AnyID_SM5003_Device.h"

const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE]@0x08005000 = "SM500303 24062000 G32F103";
const u8 DEVICE_HARD_TYPE[DEVICE_VERSION_SIZE]@0x08005080 = "SM5003-GD32103";

DEVICE_PARAMS g_sDeviceParams = {0};


WINAVG_INFO g_aDeviceTemprInfo[AD_CHECK_NUM];
DEVICE_STATINFO g_sDeviceStatInfo = {0};

void Device_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}




void Device_OutCtrlInit(void)
{
    Fire_Interface();
    Water_Interface(); 
	

}

BOOL Device_WriteDeviceParamenter(void)
{
    BOOL b = FALSE;
    
    g_sDeviceParams.crc = 0;
    g_sDeviceParams.crc = a_GetCrc((u8 *)(&g_sDeviceParams), sizeof(DEVICE_PARAMS) - 2);
    
    b = FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    b = FRam_WriteBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    
    return b;
}

void Device_ReadDeviceParamenter(void)
{ 
    BOOL b = FALSE, bBackup = FALSE;
    b = FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    if(b)
    {
        u16 crc1 = 0, crc2 = 0;
        
        crc1 = a_GetCrc((u8 *)(&g_sDeviceParams), sizeof(DEVICE_PARAMS) - 2);
        crc2 = g_sDeviceParams.crc;
        
        if(crc1 != crc2)
        {
            b = FALSE;
        }
    }
    
    bBackup = FRam_ReadBuffer(FRAME_INFO_BACKUP_ADDR, sizeof(DEVICE_PARAMS), (u8 *)(&g_sDeviceParams));
    if(bBackup)
    {
        u16 crc1 = 0, crc2 = 0;
        
        crc1 = a_GetCrc((u8 *)(&g_sDeviceParams), sizeof(DEVICE_PARAMS) - 2);
        crc2 = g_sDeviceParams.crc;
        
        if(crc1 != crc2)
        {
            bBackup = FALSE;
        }
    }
    
    if(b == FALSE && bBackup == FALSE)
    {
        memset(&g_sDeviceParams, 0, sizeof(DEVICE_PARAMS));

        g_sDeviceParams.deviceAlarmTempr = DEVICE_ALARM_TEMPR;
        Device_WriteDeviceParamenter();
    }
    
    Fram_ReadBootParamenter();
    if((g_sFramBootParamenter.appState != FRAM_BOOT_APP_OK) ||
       (g_sFramBootParamenter.addr != g_sDeviceParams.addr))
    {
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
        g_sFramBootParamenter.addr = g_sDeviceParams.addr;
        Fram_WriteBootParamenter();
    }
}



void Device_AD_GetTempr(void)
{
    u8 i;

    for(i = 0; i < AD_CHECK_NUM; i++)
    {
        Win_CalAvg(&g_aDeviceTemprInfo[i], AD_GetTemperValue(i));
        AD_GetTmpr(&g_aDeviceTemprInfo[i]);
    }
}



u8 Device_ProcessUsrFrame(u8 *pFrame)
{
    u8 cmd = 0;
    u16 destAddr = 0;
    u16 paramsLen = 0;
	u8 *pFrames = NULL;
	
    destAddr = *((u16 *)(pFrame + UART_FRAME_POS_DESTADDR));
    if((destAddr != UART_FRAME_BROADCAST_ADDR) && (destAddr != g_sDeviceParams.addr))
    {
        return 0;
    }
    
    cmd = *(pFrame + UART_FRAME_POS_CMD);
	pFrames = pFrame + UART_FRAME_POS_PAR;
    paramsLen = pFrame[UART_FRAME_POS_LEN] + 3 - UART_FRAME_MIN_LEN;


    g_sR485TxFrame.len = 0;
    switch(cmd)
    {
        case UART_FRAME_CMD_RST:
            g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
            break;
        case UART_FRAME_CMD_VER:
            g_sR485TxFrame.len = Uart_UsrResponseFrame((u8 *)DEVICE_VERSION, DEVICE_VERSION_SIZE, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
            break;
        case UART_FRAME_CMD_SET_PARAMS:
            if(paramsLen == 1)
            {
                if(*pFrame >= DEVICE_ALARM_TEMPR)
                {
                    g_sDeviceParams.deviceAlarmTempr = pFrames[0];
                    Device_WriteDeviceParamenter();
                    g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
                }
            }
            break;
        case UART_FRAME_CMD_GET_PARAMS:
            g_sR485TxFrame.len = Uart_UsrResponseFrame((u8 *)(&g_sDeviceParams.deviceAlarmTempr), 1, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
            break;
		case UART_FRAME_CMD_SET_MOT:
             if(paramsLen == 2)
            {
				if(*(pFrames + 0) == 0x00)
				{
					if(*(pFrames + 1) == FIRE_SWITCH_OPEN)
					{
						Water_PumpOpen();
						g_sDeviceStatInfo.state |= DEVICE_STAT_WATER_MOT_OPEN;
						g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
					}
					else if(*(pFrames + 1) == FIRE_SWITCH_CLOSE)
					{
						Water_PumpClose();
						g_sDeviceStatInfo.state &= ~DEVICE_STAT_WATER_MOT_OPEN;
						g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
					}
				}
            }
            break;
        case UART_FRAME_CMD_SET_OUTINFO:			//
            if(paramsLen == 2)
            {
				if(*(pFrames + 0) < FIRE_EG_NUM)
				{
					if(*(pFrames + 1) == FIRE_SWITCH_OPEN || *(pFrames + 1) == FIRE_SWITCH_CLOSE)
					{
						Fire_EgCtr(*(pFrames + 0), *(pFrames + 1));
						g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
					}
				}
            }
            break;
        case UART_FRAME_CMD_GET_DEVINFO:
            if(paramsLen == 2)    //主控状态位一并下发，包含烟感、继电器状态
            {
				u32 stateInfo = 0;

				g_sDeviceStatInfo.mainState = *(pFrames + 1);
				if(*(pFrames + 0) == 0x01)
				{
					Device_UpdateAuth(DEVICE_AUTHRITY_BACKGROUND);
				}
				else
				{
					Device_UpdateAuth(DEVICE_AUTHRITY_GATEWAY);
				}
   
				stateInfo = (g_sWaterIngo.state << 0 | g_sFireEgInfo.state << 8 | g_sDeviceStatInfo.temprFlag << 16  | g_sDeviceStatInfo.state << 24);           //上报设备当前状态和电磁阀开启状态
				g_sR485TxFrame.len = Uart_UsrResponseFrame((u8 *)&stateInfo, 4, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
            }
            break;
        case 0x02:
                    
            PWM_SetCompare1(*(pFrames));
            g_sR485TxFrame.len = Uart_UsrResponseFrame(NULL, 0, cmd, g_sDeviceParams.addr, g_sR485TxFrame.buffer);
            break;
        default:
            break;
    }
	
	if(g_sR485TxFrame.len)
	{
		g_sDeviceStatInfo.onlineTick = g_nSysTick;
	}
	   
    return g_sR485TxFrame.len;
}



u8 Device_CheckTempr(u8 alarmTempr)
{
    u8 index;
	BOOL bOk = TRUE;
    //检查各个仓的温度，并将超阈值仓的电磁阀控制位置1
    for(index = 0; index < AD_CHECK_NUM; index++)
    {
        if(AD_CheckAlarm(&g_aDeviceTemprInfo[index], DEVICE_ALARM_LIMIT_TIME, alarmTempr))
        {
			g_sDeviceStatInfo.temprFlag |= (1 << index);
			g_sDeviceStatInfo.state |= DEVICE_STAT_TEMPR_ALARM;
			bOk = FALSE;
        }
		else
		{
			g_sDeviceStatInfo.temprFlag &= ~(1 << index);
		}
    }
	
			
	if(bOk && (g_sDeviceStatInfo.state & DEVICE_STAT_TEMPR_ALARM))
	{
		g_sDeviceStatInfo.state &= ~DEVICE_STAT_TEMPR_ALARM;
	}
    return index;
}


void Device_PeriphHandle()
{
	u8 index = 0;
	
    if(g_sDeviceStatInfo.authrity ^ DEVICE_AUTHRITY_WATER)
    {
        if(g_sDeviceStatInfo.onlineTick + DEVICE_OUTFIRE_REACTION_TIME <= g_nSysTick)
        {
            g_sDeviceStatInfo.onlineTick = g_nSysTick;
            Device_UpdateAuth(DEVICE_AUTHRITY_WATER);
        }
    }
	
	
	if(g_sDeviceStatInfo.authrity == DEVICE_AUTHRITY_BACKGROUND)
	{
	
	
	}
	else if(g_sDeviceStatInfo.authrity == DEVICE_AUTHRITY_GATEWAY)  
	{
	
	
	}
	else if(g_sDeviceStatInfo.authrity == DEVICE_AUTHRITY_WATER)
	{   //喷淋权限
        if(g_sWaterIngo.state & WATER_PERIPH_PROTECT_TOUCH)		
        {
           if(!a_CheckStateBit(g_sDeviceStatInfo.op, DEVICE_OP_PREPARING)
              && !a_CheckStateBit(g_sDeviceStatInfo.op, DEVICE_OP_OUTFIRE))
           {
                a_SetState(g_sDeviceStatInfo.op, DEVICE_OP_PREPARING);
                g_sDeviceStatInfo.startTick = g_nSysTick;
           }
        }
        else
        {
           if(a_CheckStateBit(g_sDeviceStatInfo.op, DEVICE_OP_PREPARING))
           {
                a_SetState(g_sDeviceStatInfo.op, DEVICE_OP_IDLE);
                g_sDeviceStatInfo.startTick = g_nSysTick;
           }
        } 
		
		if(a_CheckStateBit(g_sDeviceStatInfo.op, DEVICE_OP_PREPARING))
		{
			if(g_sDeviceStatInfo.startTick + DEVICE_OUTFIRE_REACTION_TIME < g_nSysTick)
			{
				 a_SetState(g_sDeviceStatInfo.op, DEVICE_OP_START);
			}
		}
		
		if(a_CheckStateBit(g_sDeviceStatInfo.op, DEVICE_OP_START))
		{
			a_SetState(g_sDeviceStatInfo.op, DEVICE_OP_OUTFIRE);   
			Water_PumpOpen();
			g_sDeviceStatInfo.state |= DEVICE_STAT_WATER_MOT_OPEN;
			for(index = 0; index < FIRE_EG_NUM; index ++)
			{
				Fire_EgCtr(index, FIRE_SWITCH_OPEN);
				Device_Delayms(100);
			}
		}
        
	}
	//--
}





void Device_PeriphTest()
{
	int index = 0;
	
	for(index = 0; index < FIRE_EG_NUM; index ++)
	{
		Fire_EgCtr(index, FIRE_SWITCH_OPEN);
		Device_Delayms(500);
		#if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    	#endif
	}
	
	for(index = FIRE_EG_NUM - 1; index >= 0; index --)
	{
		Fire_EgCtr(index, FIRE_SWITCH_CLOSE);
		Device_Delayms(500);
		#if SYS_ENABLE_WDT
        WDG_FeedIWDog();
        #endif
	}
	Water_PumpOpen();
	Device_Delayms(500);
	#if SYS_ENABLE_WDT
	Device_Delayms(500);
	#endif
	Water_PumpClose();
	
}






//---------------------------------------------------------------------------------------------------------------------------------RTTASK
const PORT_INF DEVICE_RUNLED_COM = {GPIOB, GPIO_Pin_11};
const PORT_INF DEVICE_ALARMLED_COM = {GPIOB, GPIO_Pin_12};
const PORT_INF DEVICE_RUNLED1_COM = {GPIOB, GPIO_Pin_5};
const PORT_INF DEVICE_RUNLED2_COM = {GPIOB, GPIO_Pin_9};



const PORT_INF DEVICE_KEY_COM = {GPIOA, GPIO_Pin_6};

#define DEVICE_IO_CTRMODE_PWM                   0
#define DEVICE_IO_EXTI_ENABLE                   1
void Device_CtrlIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    
    GPIO_InitStructure.GPIO_Pin = DEVICE_RUNLED_COM.Pin;       
    GPIO_Init(DEVICE_RUNLED_COM.Port, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = DEVICE_ALARMLED_COM.Pin;       
    GPIO_Init(DEVICE_ALARMLED_COM.Port, &GPIO_InitStructure);

    
#if DEVICE_IO_EXTI_ENABLE
    GPIO_InitStructure.GPIO_Pin = DEVICE_KEY_COM.Pin;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_AF_PP;         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;       
    GPIO_Init(DEVICE_KEY_COM.Port, &GPIO_InitStructure);  
    
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);
    EXTI_InitTypeDef Exti_InitStructure;
    
    Exti_InitStructure.EXTI_Line = EXTI_Line6;
    Exti_InitStructure.EXTI_LineCmd = ENABLE;
    Exti_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    Exti_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    
    EXTI_Init(&Exti_InitStructure);
    
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;               //注意优先级管理
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
    
#endif
    
#if DEVICE_IO_CTRMODE_PWM

    /* 
    GPIO_InitStructure.GPIO_Pin = DEVICE_RUNLED_COM.Pin;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;       
    GPIO_Init(DEVICE_RUNLED_COM.Port, &GPIO_InitStructure);  


    TIM_TimeBaseStructure.TIM_Period = 100 - 1;             
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;         
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                
     
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //使能比较输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性:TIM输出比较极性高
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);                         //根据T指定的参数初始化外设TIM3 OC2
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);                //使能TIM3在CCR2上的预装载寄存器
    
    TIM_Cmd(TIM2, ENABLE);  
    */
    //----
   /*     

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;    //定义TIMx定时器结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;            //定义定时器脉宽调制结构体

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;    
    TIM_OCInitTypeDef TIM_OCInitStructure;           

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);    

    GPIO_InitStructure.GPIO_Pin = DEVICE_RUNLED1_COM.Pin;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;      
    GPIO_Init(DEVICE_RUNLED1_COM.Port, &GPIO_InitStructure);      

    TIM_TimeBaseStructure.TIM_Period = 100 - 1;            
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;         
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;           
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                  
     
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);                         
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);              
    
    TIM_Cmd(TIM3, ENABLE);   

    
    GPIO_InitStructure.GPIO_Pin = DEVICE_RUNLED2_COM.Pin;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;      
    GPIO_Init(DEVICE_RUNLED2_COM.Port, &GPIO_InitStructure);      

    TIM_TimeBaseStructure.TIM_Period = 100 - 1;            
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;         
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;           
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                  
     
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);                         
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);              
    TIM_ARRPreloadConfig(TIM4, ENABLE);  
    TIM_Cmd(TIM4, ENABLE);   */
#else
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = DEVICE_RUNLED_COM.Pin;
    GPIO_Init(DEVICE_RUNLED_COM.Port, &GPIO_InitStructure); 
    
    GPIO_InitStructure.GPIO_Pin = DEVICE_ALARMLED_COM.Pin;
    GPIO_Init(DEVICE_ALARMLED_COM.Port, &GPIO_InitStructure);
#endif
}

#define Device_SetPwm(v)        do{TIM2->CCR4 = v;}while(0)

#define Device_SetLed1Pwm(v)        do{TIM3->CCR2 = v;}while(0)
#define Device_SetLed2Pwm(v)        do{TIM4->CCR4 = v;}while(0)
#define Device_SetLed3Pwm(v)        do{t->CCR4 = v;}while(0)
void Device_LedTask()
{  
#if DEVICE_IO_CTRMODE_PWM
    const TickType_t delayTime = pdMS_TO_TICKS( 10UL );
    static u8 pwmTime = 0, pwmLed2Time = 100,pwmState = 0;
    TickType_t currentTime;
        
    currentTime = xTaskGetTickCount();
    for( ; ; )
    {
        #if DEVICE_IO_CTRMODE_PWM
                if(pwmState & 0x01)
                {
                    Device_SetPwm((pwmTime -= 1));
                    //Device_SetLed1Pwm((pwmTime -= 1));
                    //Device_SetLed2Pwm((pwmLed2Time += 1));
                    if(pwmTime <= 0)
                    {
                        pwmState = 0x00;
                        vTaskDelayUntil(&currentTime, pdMS_TO_TICKS( 5UL ));
                    }
                }
                else
                {
                    Device_SetPwm((pwmTime += 1));
                    //Device_SetLed1Pwm((pwmTime += 1));
                    //Device_SetLed2Pwm((pwmLed2Time -= 1));
                    if(pwmTime >= 100)
                    {
                        pwmState = 0x01;
                        vTaskDelayUntil(&currentTime, pdMS_TO_TICKS( 5UL ));
                    }
                }
                vTaskDelayUntil(&currentTime, delayTime);
        #else
                Device_RunLedOn();
                vTaskDelayUntil(&currentTime, delayTime);
                Device_AlarmLedOn();
                vTaskDelayUntil(&currentTime, delayTime);
                Device_RunLedOff();
                vTaskDelayUntil(&currentTime, delayTime);
                Device_AlarmLedOff();
                vTaskDelayUntil(&currentTime, delayTime);
        #endif
    }
#endif
}


/*
size_t __write(int handle, const unsigned char *buf, size_t bufSize) {
		int sendsize;

    sendsize = uartSend((uint8_t *)buf,bufSize);

    return sendsize ;
}
*/
int fputc(int ch, FILE *f)                  //重定向，使用printf，需添加宏
{
    
    R485_WriteBuffer((u8 *)&ch, 1);
    
    return ch;
}

void Device_Peintf(u8 *pBuffer)
{
    portENTER_CRITICAL();                                   //进入临界区、等待数据发送完成，否则高优先级可能会抢占
    printf((char const *)pBuffer);
    portEXIT_CRITICAL();                                    //在同串口情况下抢占导致乱码
}
//demo


void Device_Task1();
void Device_Task2();
void Device_Task3();

#if configSUPPORT_DYNAMIC_ALLOCATION
TaskHandle_t Device_Task1Handle;
TaskHandle_t Device_Task2Handle;
TaskHandle_t Device_Task3Handle;


#if configSUPPORT_STATIC_ALLOCATION
StackType_t Device_StaicTask1[configMINIMAL_STACK_SIZE];
StackType_t Device_StaicTask2[configMINIMAL_STACK_SIZE];
StackType_t Device_StaicTask3[configMINIMAL_STACK_SIZE];
#endif

StaticTask_t Device_TcbTask1;
StaticTask_t Device_TcbTask2;
StaticTask_t Device_TcbTask3;
#endif

List_t g_sList = {0};
ListItem_t g_sListItem1 = {0};
ListItem_t g_sListItem2 = {0};
ListItem_t g_sListItem3 = {0};
ListItem_t g_sListItem4 = {0};

#define Device_PrintfListInfo(s,p)  do{printf(s);printf("Addr : 0x%p\r\n", &g_sList);}while(0)


void Device_PrintfDemoInfo()
{
    printf("g_sList : 0x%p\r\n", &g_sList);
    printf("g_sList.pxIndex : 0x%p\r\n", g_sList.pxIndex);
    printf("g_sList.xListEnd : 0x%p\r\n", &g_sList.xListEnd);
    printf("g_sList.uxNumberOfItems : %d\r\n", g_sList.uxNumberOfItems);
    printf("\r\n");
    
    printf("g_sListItem1 : 0x%p\r\n", &g_sListItem1);
    printf("g_sListItem1.pvContainer : 0x%p\r\n", g_sListItem1.pvContainer);
    printf("g_sListItem1.pxPrevious : 0x%p\r\n", g_sListItem1.pxPrevious);
    printf("g_sListItem1.pxNext : 0x%p\r\n", g_sListItem1.pxNext);
    printf("g_sListItem1.xItemValue : %d\r\n", g_sListItem1.xItemValue);
    printf("\r\n");
    
    
    printf("g_sListItem2 : 0x%p\r\n", &g_sListItem2);
    printf("g_sListItem2.pvContainer : 0x%p\r\n", g_sListItem2.pvContainer);
    printf("g_sListItem2.pxPrevious : 0x%p\r\n", g_sListItem2.pxPrevious);
    printf("g_sListItem2.pxNext : 0x%p\r\n", g_sListItem2.pxNext);
    printf("g_sListItem2.xItemValue : %d\r\n", g_sListItem2.xItemValue);
    printf("\r\n");
    
    printf("g_sListItem3 : 0x%p\r\n", &g_sListItem3);
    printf("g_sListItem3.pvContainer : 0x%p\r\n", g_sListItem3.pvContainer);
    printf("g_sListItem3.pxPrevious : 0x%p\r\n", g_sListItem3.pxPrevious);
    printf("g_sListItem3.pxNext : 0x%p\r\n", g_sListItem3.pxNext);
    printf("g_sListItem3.xItemValue : %d\r\n", g_sListItem3.xItemValue);
    printf("\r\n");
    
    printf("g_sListItem4 : 0x%p\r\n", &g_sListItem4);
    printf("g_sListItem4.pvContainer : 0x%p\r\n", g_sListItem4.pvContainer);
    printf("g_sListItem4.pxPrevious : 0x%p\r\n", g_sListItem4.pxPrevious);
    printf("g_sListItem4.pxNext : 0x%p\r\n", g_sListItem4.pxNext);
    printf("g_sListItem4.xItemValue : %d\r\n", g_sListItem4.xItemValue);
    printf("\r\n");



}
void Device_ListTest()
{

    vListInitialise(&g_sList);

    vListInitialiseItem(&g_sListItem1);
    vListInitialiseItem(&g_sListItem2);
    vListInitialiseItem(&g_sListItem3);
    vListInitialiseItem(&g_sListItem4);

    g_sListItem1.xItemValue = 12;
    g_sListItem2.xItemValue = 8;
    g_sListItem3.xItemValue = 11;
    g_sListItem4.xItemValue = 10;
    //打印刚初始化后的列表信息
    printf("/********************Init****************/\r\n");
    Device_PrintfDemoInfo();
    
    printf("InsertEnd  ListItem1\r\n");
    vListInsertEnd(&g_sList, &g_sListItem1);
    Device_PrintfDemoInfo();
    printf("InsertEnd  ListItem2\r\n");
    vListInsertEnd(&g_sList, &g_sListItem2);
    Device_PrintfDemoInfo();
    printf("InsertEnd  ListItem3\r\n");
    vListInsertEnd(&g_sList, &g_sListItem3);
    Device_PrintfDemoInfo();
    printf("InsertEnd  ListItem4\r\n");
    vListInsertEnd(&g_sList, &g_sListItem4);
    Device_PrintfDemoInfo();
    
    printf("Remove  ListItem1\r\n");
    uxListRemove(&g_sListItem1);
    Device_PrintfDemoInfo();
    printf("Remove  ListItem2\r\n");
    uxListRemove(&g_sListItem2);
    Device_PrintfDemoInfo();
    printf("Remove  ListItem3\r\n");
    uxListRemove(&g_sListItem3);
    Device_PrintfDemoInfo();
    printf("Remove  ListItem4\r\n");
    uxListRemove(&g_sListItem4);
    Device_PrintfDemoInfo();
    
    
    printf("Insert  ListItem1\r\n");
    vListInsert(&g_sList, &g_sListItem1);
    Device_PrintfDemoInfo();
    printf("Insert  ListItem2\r\n");
    vListInsert(&g_sList, &g_sListItem2);
    Device_PrintfDemoInfo();
    printf("Insert  ListItem3\r\n");
    vListInsert(&g_sList, &g_sListItem3);
    Device_PrintfDemoInfo();
    printf("Insert  ListItem4\r\n");
    vListInsert(&g_sList, &g_sListItem4);
    Device_PrintfDemoInfo();
}

void Device_TaskCreat() 
{
	//portENTER_CRITICAL();				//进入临界区，关闭中断，停止调度器
    Device_ListTest();
#if configSUPPORT_DYNAMIC_ALLOCATION
	xTaskCreate((TaskFunction_t) Device_Task1,                                   //函数地址
                (const char * const) "Device_Task1",                             //函数名
                (const configSTACK_DEPTH_TYPE) configMINIMAL_STACK_SIZE,        //堆栈长度 
                (void * const) NULL,                                            //携带参数
                (UBaseType_t) 2,                                                //优先级
                (TaskHandle_t *) &Device_Task1Handle);		                    //句柄                           //Device_Task1抢占Device_TaskCreat(),堵塞后再次进入
	xTaskCreate((TaskFunction_t) Device_Task2, 
                (const char * const) "Device_Task2", 
                (const configSTACK_DEPTH_TYPE) configMINIMAL_STACK_SIZE, 
                (void * const) NULL, 
                (UBaseType_t) 3, 
                (TaskHandle_t *) &Device_Task2Handle);    	//Device_Task2抢占Device_TaskCreat(),堵塞后再次进入
	xTaskCreate((TaskFunction_t) Device_Task3, 
                (const char * const) "Device_Task3", 
                (const configSTACK_DEPTH_TYPE) configMINIMAL_STACK_SIZE, 
                (void * const) NULL, 
                (UBaseType_t) 4, 
                (TaskHandle_t *) &Device_Task3Handle);		//Device_Task3抢占Device_TaskCreat(),堵塞后再次进入
#endif	
	
#if configSUPPORT_STATIC_ALLOCATION
	Device_Task1Handle = xTaskCreateStatic((TaskFunction_t) Device_Task1,				//函数地址
						  (const char * const) "Device_Task1",							//函数名
						  (const uint32_t) configMINIMAL_STACK_SIZE,					//堆栈大小
						  (void * const) NULL,											//携带参数
						  (UBaseType_t) 2,												//优先级
						  (StackType_t * const) Device_StaicTask1,						//堆栈地址
						  (StaticTask_t * const) &Device_TcbTask1);						//数据块地址
	Device_Task2Handle = xTaskCreateStatic((TaskFunction_t) Device_Task2,
						  (const char * const) "Device_Task2",
						  (const uint32_t) configMINIMAL_STACK_SIZE,
						  (void * const) NULL,
						  (UBaseType_t) 3,
						  (StackType_t * const) Device_StaicTask2,
						  (StaticTask_t * const) &Device_TcbTask2);
	Device_Task3Handle = xTaskCreateStatic((TaskFunction_t) Device_Task3,
						  (const char * const) "Device_Task3",
						  (const uint32_t) configMINIMAL_STACK_SIZE,
						  (void * const) NULL,
						  (UBaseType_t) 4,
						  (StackType_t * const) Device_StaicTask3,
						  (StaticTask_t * const) &Device_TcbTask3);
#endif
	vTaskDelete(NULL);
	//portEXIT_CRITICAL();				//退出临界区
}

void Device_Task1()
{
  	u8 state = 0;
    TickType_t currentTime = 0;
    const TickType_t delayTime = pdMS_TO_TICKS( 500UL );
	while(1)
	{
	  	if(state == 0)
		{
		  	state = 1;
			Device_RunLedOn();
		}
		else
		{
			state = 0;
			Device_RunLedOff();
		}
       
        Device_Peintf("Task1 Runing\r\n"); 
        //vTaskSuspend(Device_Task2Handle);
        vTaskDelay(delayTime);
		vTaskDelayUntil(&currentTime, delayTime);
	}
}

void Device_Task2()
{
    u8 state = 0;
    TickType_t currentTime = 0;
    const TickType_t delayTime = pdMS_TO_TICKS( 800UL );
	while(1)
	{
	  	if(state == 0)
		{
		  	state = 1;
			Device_AlarmLedOn();
		}
		else
		{
			state = 0;
			Device_AlarmLedOff();
		}
        
        Device_Peintf("Task2 Runing\r\n"); 
        //vTaskSuspend(Device_Task1Handle);
        vTaskDelay(delayTime);
		//vTaskDelayUntil(&currentTime, delayTime);
	}
}
u8 g_nKeyValue = 0;
void Device_Task3()
{
    TickType_t currentTime = 0;
    u8 state = 0;
    const TickType_t delayTime = pdMS_TO_TICKS( 1000UL );
	while(1)
	{
		if(g_nKeyValue == 1)
		{
            if(state == 0)
            {
                state = 1;
                vTaskSuspend(Device_Task2Handle);       //获取任务状态，不能一直挂起
                Device_Peintf("ISR OP, Suspend Task2\r\n");
            }
		}
		else if(g_nKeyValue == 2)
		{
			vTaskResume(Device_Task1Handle);	
		}
        else
        {
            if(state == 1)
            {
                state = 0;
            }
        }
        Device_Peintf("Task3 Runing\r\n"); 
        vTaskDelay(delayTime);
        //vTaskDelayUntil(&currentTime, delayTime);
	}
}
//建立IO中断，在中断中解除挂起xTaskResumeFromISR

//putchar
//空闲任务内存分配

#if configSUPPORT_STATIC_ALLOCATION
StaticTask_t Device_TcbIdleTask; 
StackType_t Device_StackIdleTask[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory(StaticTask_t ** ppxIdleTaskTCBBuffer,
								   StackType_t ** ppxIdleTaskStackBuffer,
								   uint32_t * pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &Device_TcbIdleTask; 					//	空闲任务数据块
	*ppxIdleTaskStackBuffer = Device_StackIdleTask;					//空闲任务堆栈地址
	* pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;				//空闲任务堆栈大小
}

StaticTask_t Device_TcbTimeTask; 
StackType_t Device_StackTimeTask[configMINIMAL_STACK_SIZE];
void vApplicationGetTimerTaskMemory(StaticTask_t ** ppxTimerTaskTCBBuffer,
								    StackType_t ** ppxTimerTaskStackBuffer,
								    uint32_t * pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &Device_TcbTimeTask;
	*ppxTimerTaskStackBuffer = Device_StackTimeTask;
	*pulTimerTaskStackSize = configMINIMAL_STACK_SIZE;
}
#endif
//--