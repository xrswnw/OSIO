#ifndef _ANYID_CONFIG_
#define _ANYID_CONFIG_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"

//设定工作时钟
#if (defined(_ANYID_PIC18_HSR100_) | \
     defined(_ANYID_PIC18_BWCOM_)  | \
     defined(_ANYID_PIC18_WVDU_))
    #define SYS_CLOCK_FREQ              32000000
#elif (defined(_ANYID_STM32_MMD_) | \
      defined(_ANYID_STM32_MCD_) | \
      defined(_ANYID_STM32_MTD_) | \
      defined(_ANYID_STM32_MPD_) | \
      defined(_ANYID_STM32_BCTRL_) | \
      defined(_ANYID_STM32_BBOOT_))
    #define SYS_CLOCK_FREQ              72000000
#elif (defined(_ANYID_STM32_BLDC_))
    #define SYS_CLOCK_FREQ              24000000
#endif



//STM32平台相关
#if (defined(_ANYID_STM32_))
    #include "stm32f10x_conf.h"

    //端口定义
    typedef struct Port_Inf
    {
        GPIO_TypeDef* Port;
        u16 Pin;
    } PORT_INF;
#endif


//PIC18平台相关
#if (defined(_ANYID_PIC18_))
    #include <p18cxxx.h>
#endif



#endif

