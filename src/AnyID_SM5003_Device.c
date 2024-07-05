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