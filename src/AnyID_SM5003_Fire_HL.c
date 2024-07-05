#include "AnyID_SM5003_Fire_HL.h"

//const PORT_INF EG_CTLx_COM = {GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10};
//const u16 g_aMapTal[FIRE_EG_NUM] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_5, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10};
FIRE_EGINFO g_sFireEgInfo = {0};

const PORT_INF EG_CTLx_COM = {GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10};
const u16 g_aMapTal[FIRE_EG_NUM] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_5, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10};

void Fire_Interface(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = EG_CTLx_COM.Pin;
    GPIO_Init(EG_CTLx_COM.Port, &GPIO_InitStructure);

}

const PORT_INF PWR_PORT_COM = {GPIOB, GPIO_Pin_5};


/*
PWM频率：	Freq = CK_PSC / (PSC + 1) / (ARR + 1)
PWM占空比：	Duty = CCR / (ARR + 1)
PWM分辨率：	Reso = 1 / (ARR + 1)

pwm1 : cnt <= ccr ,1; cnt >= ccr , 0
*/
void Fire_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;              //定义GPIO结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;    //定义TIMx定时器结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;            //定义定时器脉宽调制结构体

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);     //TIM3部分重映射 TIM3_CH2->PB5

    GPIO_InitStructure.GPIO_Pin = PWR_PORT_COM.Pin;         //TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;       //配置输出速率
    GPIO_Init(PWR_PORT_COM.Port, &GPIO_InitStructure);      //初始化GPIOB

    TIM_TimeBaseStructure.TIM_Period = 100 - 1;             //设置自动重装载寄存器周期的值 arr=value-1
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;         //设置预分频值 psc=value-1
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;            //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                  //初始化TIMx时间基数
     
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   //使能比较输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性:TIM输出比较极性高
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);                         //根据T指定的参数初始化外设TIM3 OC2
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);                //使能TIM3在CCR2上的预装载寄存器
    
    TIM_Cmd(TIM3, ENABLE);                                                 
}


void PWM_SetCompare1(u16 Compare)
{
    TIM3->CCR2 = Compare;
}
void Fire_CtlEg(u8 ctlBits)
{
	
    u8 i;
    u8 bits = 0;
    u8 flag = 0;
    u16 enablePin = 0;
    u16 disablePin = 0;
        
    bits = ctlBits & FIRE_EG_MASK;
    for(i = 0; i < FIRE_EG_NUM; i++)
    {
        flag = 1 << i;
        if(bits & flag)
        {
            enablePin |= g_aMapTal[i];
        }
        else
        {
            disablePin |= g_aMapTal[i];
        }
    }
    GPIO_SetBits(EG_CTLx_COM.Port, enablePin);
    GPIO_ResetBits(EG_CTLx_COM.Port, disablePin);
}


	
void Fire_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}
