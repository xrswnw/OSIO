#ifndef _ANYID_SM5003_AD_HL_
#define _ANYID_SM5003_AD_HL_

#include "AnyID_SM5003_Config.h"

#define AD_CHANNEL_NUM              6

#define AD_TEMP1_CHANNEL            ADC_Channel_0
#define AD_TEMP2_CHANNEL            ADC_Channel_1
#define AD_TEMP3_CHANNEL            ADC_Channel_2
#define AD_TEMP4_CHANNEL            ADC_Channel_3
#define AD_TEMP5_CHANNEL            ADC_Channel_4
#define AD_TEMP6_CHANNEL            ADC_Channel_5

#define AD_TIME                     ADC_SampleTime_239Cycles5
#define AD_PORT                     ADC1
#define AD_VALUE_MASK               0xFFF

#define AD_DMA_CHANNEL              DMA1_Channel1

void AD_InitInterface(void);
void AD_Init(void);
void AD_DmaInit(void);
//void AD_SelectGroup(u8 num);
#define AD_Start()                  (AD_PORT)->CR2 |= CR2_EXTTRIG_SWSTART_Set

extern u16 g_aAdBuffer[AD_CHANNEL_NUM];
#define AD_GetTemperValue(x)        (g_aAdBuffer[x] & AD_VALUE_MASK)
#endif

