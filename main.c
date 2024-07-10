#include "AnyID_SM5003_SysCfg.h"

int main(void)
{
    Sys_Init();

    //xTaskCreate(Device_LedTask, DEVICE_TASK_LED, configMINIMAL_STACK_SIZE, NULL, 0, NULL);
      	xTaskCreate(Device_TaskCreat, "Device_TaskCreat", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();              //µ÷¶ÈÆ÷ÆôÓÃ
    
    return 0;
}
