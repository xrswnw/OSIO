#include "AnyID_SM5003_SysCfg.h"

int main(void)
{
    Sys_Init();
    while(1)
    {
        Sys_LedTask();
        Sys_R485Task();
        Sys_CheckSensorTask();
        Sys_ADTask();
        Sys_OutCtrlTask();
        Sys_RecordTask();
    }
}
