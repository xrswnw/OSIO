#include "AnyID_SM5003_Water_HL.h"

const PORT_INF WATER_POSITION_COM = {GPIOA, GPIO_Pin_6};
const PORT_INF WATER_PROTECT_COM = {GPIOA, GPIO_Pin_7};
const PORT_INF WATER_PUMP_CTRL_COM = {GPIOA, GPIO_Pin_8};


WATER_INFO g_sWaterIngo = {0};
void Water_Interface(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    WATER_PUMP_CTRL_COM.Port->BRR = WATER_PUMP_CTRL_COM.Pin;
    GPIO_InitStructure.GPIO_Pin = WATER_PUMP_CTRL_COM.Pin;
    GPIO_Init(WATER_PUMP_CTRL_COM.Port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = WATER_POSITION_COM.Pin;
    GPIO_Init(WATER_POSITION_COM.Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WATER_PROTECT_COM.Pin;
    GPIO_Init(WATER_PROTECT_COM.Port, &GPIO_InitStructure);
}


u8 Water_GetPeriphInfo()
{
	u8 state = 0;
	
	if(Get_WaterPosition())
	{
		state |= WATER_PERIPH_POSITION_TOUCH;
	}
	
	if(Get_WaterProtect())
	{
		state |= WATER_PERIPH_PROTECT_TOUCH;         //温度过高，温度开关闭合
	}

	return state;
}