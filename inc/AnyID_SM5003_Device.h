#ifndef _ANYID_SM5003_DEVICE_
#define _ANYID_SM5003_DEVICE_

#include "AnyID_SM5003_WDG_HL.h"
#include "AnyID_SM5003_AD.h"
#include "AnyID_SM5003_R485.h"
#include "AnyID_SM5003_FRam.h"
#include "AnyID_SM5003_Fire_HL.h"
#include "AnyID_SM5003_Water_HL.h"
#include "stdio.h"
#include "stdlib.h"
#define DEVICE_VERSION_SIZE					50
extern const u8 DEVICE_VERSION[DEVICE_VERSION_SIZE];

#define DEVICE_ADDR_WATER_MOT				0xFF



#define DEVICE_STAT_WATER_MOT_OPEN			0x04
#define DEVICE_STAT_TEMPR_ALARM 			0x08

#define DEVICE_MAIN_STAT_SMOKE_OPEN			0x04
#define DEVICE_MAIN_STAT_RELAY_CLOSE		0x80

#define DEVICE_MAST_SMOKE_ERR				0x01
#define DEVICE_SMOKE_SAMPLE_TIME			5

#define DEVICE_ALARM_TEMPR                  30
#define DEVICE_ALARM_LIMIT_TIME				50

#define DEVICE_OUTFIRE_REACTION_TIME        10 * 200


#define DEVICE_STAT_COM_ERR                 0x00
#define DEVICE_STAT_COM_OK                  0x01


#define DEVICE_OP_IDLE                      0x00
#define DEVICE_OP_PREPARING                 0x01
#define DEVICE_OP_START                     0x02
#define DEVICE_OP_OUTFIRE                   0x04

#define DEVICE_AUTHRITY_NULL			    0x00			//权限未知
#define DEVICE_AUTHRITY_BACKGROUND			0x01			//后台权限
#define DEVICE_AUTHRITY_GATEWAY			    0x02			//网关权限
#define DEVICE_AUTHRITY_WATER			    0x03			//喷淋权限

typedef struct deviceStatInfo{
	u8 state;
	u8 authrity;							
    u8 op;
	u8 mainState;
	u8 temprFlag;
	u8 flag;
	u32 onlineTick;
    u32 startTick;
	u32 limitTime;
}DEVICE_STATINFO;
extern DEVICE_STATINFO g_sDeviceStatInfo;

void Device_PeriphHandle();


#define Device_UpdateAuth(v)				do{\
												if(v ^ g_sDeviceStatInfo.authrity)\
												{\
													g_sDeviceStatInfo.authrity = v;\
												}\
											}while(0)

#define DEVICE_LOCAL_ADDR               (0x03)
#define DEVICE_DEST_ADDR                (0x02)
typedef struct deviceParams{
    u16 addr;
    u8 deviceAlarmTempr;
    u16 crc;
}DEVICE_PARAMS;
extern DEVICE_PARAMS g_sDeviceParams;

extern WINAVG_INFO g_aDeviceTemprInfo[AD_CHECK_NUM];

typedef struct deviceOutCtrl{
    u8 rfu : 2;
    u8 FireCtrlBits : 6;
    BitState PumpCtrlBit;
}DEVICE_OUTCTRL;

typedef struct deviceInFb{
    BitState protectErrBit;
    BitState waterPositionBit;
}DEVICE_FBINFO;

#define DEVINFO_RECORD_TIME_UNNOR    (100 * 2)           //2秒
#define DEVINFO_RECORD_TIME_NOR      (100 * 120)         //2分钟


#define DEVICE_STAT_TEMPRSAFE     0x01
#define DEVICE_STAT_WATERSAFE     0x02
#define DEVICE_STAT_PROTECTERR    0x04
#define DEVICE_STAT_TEMPRALARM    0x08
#define DEVICE_STAT_WATERALARM    0x10
#define DEVICE_STAT_DANGER        0x20



BOOL Device_WriteDeviceParamenter(void);
void Device_ReadDeviceParamenter(void);
void Device_OutCtrlInit(void);

void Device_AD_GetTempr(void);
u8 Device_ProcessUsrFrame(u8 *pFrame);
void Device_SetPumpCtrlBit(DEVICE_OUTCTRL *pUsrCtrl, DEVICE_OUTCTRL *pOutCtrl);
u8 Device_CheckTempr(u8 alarmTempr);

void Device_SetFireCtrlBits(u32 ctrlBits, DEVICE_OUTCTRL *pUsrCtrl, DEVICE_OUTCTRL *pOutCtrl);
void Device_PeriphTest();















//-----------------------

extern const PORT_INF DEVICE_RUNLED_COM;
#define Device_RunLedOff()             (DEVICE_RUNLED_COM.Port->BSRR = DEVICE_RUNLED_COM.Pin)          //high
#define Device_RunLedOn()              (DEVICE_RUNLED_COM.Port->BRR = DEVICE_RUNLED_COM.Pin)           //low

extern const PORT_INF DEVICE_ALARMLED_COM;
#define Device_AlarmLedOff()             (DEVICE_ALARMLED_COM.Port->BSRR = DEVICE_ALARMLED_COM.Pin)        //high
#define Device_AlarmLedOn()              (DEVICE_ALARMLED_COM.Port->BRR = DEVICE_ALARMLED_COM.Pin)         //low


extern const PORT_INF DEVICE_RUNLED1_COM;
#define Device_RunLed1Off()             (DEVICE_RUNLED1_COM.Port->BSRR = DEVICE_RUNLED1_COM.Pin)        //high
#define Device_RunLed1On()              (DEVICE_RUNLED1_COM.Port->BRR = DEVICE_RUNLED1_COM.Pin)         //low
extern const PORT_INF DEVICE_RUNLED2_COM;
#define Device_RunLed2Off()             (DEVICE_RUNLED2_COM.Port->BSRR = DEVICE_RUNLED2_COM.Pin)        //high
#define Device_RunLed2On()              (DEVICE_RUNLED2_COM.Port->BRR = DEVICE_RUNLED2_COM.Pin)         //low


extern TaskHandle_t Device_Task1Handle;
extern TaskHandle_t Device_Task2Handle;
extern TaskHandle_t Device_Task3Handle;
extern u8 g_nKeyValue;
void Device_CtrlIOInit(void);

#define DEVICE_TASK_LED                 "DeviceTaskLed"
void Device_LedTask();



extern TaskHandle_t Device_QueueTask1Handle;
extern TaskHandle_t Device_QueueTask2Handle;
extern TaskHandle_t Device_QueueTask3Handle;
void Device_QueueTask1();
void Device_QueueTask2();;
void Device_QueueTask3();
 void Device_QueueCreat();
//demo
void Device_TaskCreat();
//
#if configGENERATE_RUN_TIME_STATS
    extern uint32_t FreeRTOSRunTimeTicks ;
    void Tim_Init(u32 period, u32 prescaler);
    void ConfigureTimeForRunTimeStats();
#endif
    
    
    
extern TaskHandle_t Device_SemaphoreTask1Handle;
extern TaskHandle_t Device_SemaphoreTask2Handle;
extern TaskHandle_t Device_SemaphoreTask3Handle;
extern TaskHandle_t Device_SemaphoreTask4Handle;
void Device_SemaphoreTask1();
void Device_SemaphoreTask2();
void Device_SemaphoreTask3();
void Device_SemaphoreTask4();


//优先级翻转


void Device_SemaphoreTaskLow();
void Device_SemaphoreTaskMid();
extern TaskHandle_t Device_SemaphoreTaskHighHandle;
extern TaskHandle_t Device_SemaphoreTaskMidHandle;
extern TaskHandle_t Device_SemaphoreTaskLowHandle;
void Device_SemaphoreTaskHigh();

//队列集测试
extern TaskHandle_t Device_QueueCreatTaskHandle;
extern TaskHandle_t Device_QueueGiveTaskHandle;
extern TaskHandle_t Device_QueueTakeTaskHandle;

void Device_QueueTakeTask();
void Device_QueueGiveTask();
void Device_QueueCreatTask();


//


extern TaskHandle_t Device_TaskNoticeTask1Handle;
extern TaskHandle_t Device_TaskNoticeTask2Handle;
extern TaskHandle_t Device_TaskNoticeTask3Handle;
void Device_TaskNoticeTask1();
void Device_TaskNoticeTask2();
void Device_TaskNoticeTask3();
void Device_Tim1CallBackTask( TimerHandle_t pxTimer );
  void Device_Tim2CallBackTask( TimerHandle_t pxTimer );
#endif