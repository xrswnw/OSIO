#ifndef _ANYID_SM5003_SYSCFG_
#define _ANYID_SM5003_SYSCFG_

#include "AnyID_SM5003_Config.h"
#include "AnyID_SM5003_SysTick_HL.h"
#include "AnyID_SM5003_Device.h"


extern u32 g_nSysState;
#define SYS_STAT_IDLE           0x00000001
#define SYS_STAT_RUNLED         0x00000002
#define SYS_STAT_RESET          0x00000004
#define SYS_STAT_ALARMLED       0x00000008
#define SYS_STAT_ALARMDLY       0x00000010    
#define SYS_STAT_R485TX         0x00000020
#define SYS_STAT_SUBINFO        0x00000040
#define SYS_STAT_UARTERR        0x00000080
#define SYS_STAT_AD             0x00000100
#define SYS_STAT_IN_SENSOR      0x00000200
#define SYS_STAT_CTLOUTPUT      0x00000400
#define SYS_STAT_ADDRECORD      0x00000800
#define SYS_STAT_RECORDINFO     0x00001000


#define LED_STAT_FIRE_ALARM          0x01
#define LED_STAT_WATER_ALARM         0x02
#define LED_ALARM_HZ_500MS           50
#define LED_ALARM_HZ_1S              100
#define LED_ALARM_HZ_2S              200
typedef struct SysLedInfo{
    u32 interval;
    u32 runLedTimes;
    u32 alarmLedTimes;
    u8 state;
}SYS_LEDINFO;
extern SYS_LEDINFO g_sSysLedInfo;




#define SYS_ADDR_BASE_ADDR				20

extern const PORT_INF SYS_RUNLED_COM;
#define Sys_RunLedOff()             (SYS_RUNLED_COM.Port->BSRR = SYS_RUNLED_COM.Pin)          //high
#define Sys_RunLedOn()              (SYS_RUNLED_COM.Port->BRR = SYS_RUNLED_COM.Pin)           //low

extern const PORT_INF SYS_ALARMLED_COM;
#define Sys_AlarmLedOff()             (SYS_ALARMLED_COM.Port->BSRR = SYS_ALARMLED_COM.Pin)        //high
#define Sys_AlarmLedOn()              (SYS_ALARMLED_COM.Port->BRR = SYS_ALARMLED_COM.Pin)         //low

extern const PORT_INF SYS_PROTECT_COM;
#define Check_Protect_State()        (SYS_PROTECT_COM.Port->IDR & SYS_PROTECT_COM.Pin)


u16 Sys_GetDeviceAddr();

void Sys_Delayms(u32 n);                                 
void Sys_Delay5ms(u32 n);
void Sys_CfgClock(void);
void Sys_CfgPeriphClk(FunctionalState state);
void Sys_CfgNVIC(void);

void Sys_CtrlIOInit(void);
void Sys_Init(void);
void Sys_LedTask(void);
void Sys_R485Task(void);
void Sys_ADTask(void);
void Sys_CheckSensorTask(void);
void Sys_RecordTask(void);
void Sys_OutCtrlTask(void);

#endif
