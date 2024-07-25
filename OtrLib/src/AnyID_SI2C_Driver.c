#include "AnyID_SI2C_Driver.h"

    const PORT_INF I2C_SCL = {GPIOA, GPIO_Pin_11};
    const PORT_INF I2C_SDA = {GPIOA, GPIO_Pin_12};

    #define SCL_H         I2C_SCL.Port->BSRR = I2C_SCL.Pin
    #define SCL_L         I2C_SCL.Port->BRR  = I2C_SCL.Pin

    #define SDA_H         I2C_SDA.Port->BSRR = I2C_SDA.Pin
    #define SDA_L         I2C_SDA.Port->BRR  = I2C_SDA.Pin

    #define SCL_read      I2C_SCL.Port->IDR  & I2C_SCL.Pin
    #define SDA_read      I2C_SDA.Port->IDR  & I2C_SDA.Pin


void I2C_Init(void)
{
#ifdef _ANYID_STM32_
    GPIO_InitTypeDef  GPIO_InitStructure;

    //Configure I2C1 pins: SCL and SDA
    GPIO_InitStructure.GPIO_Pin =  I2C_SCL.Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(I2C_SCL.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  I2C_SDA.Pin;
    GPIO_Init(I2C_SDA.Port, &GPIO_InitStructure);
#endif
}

void I2C_LowPower(LPMode lp)
{
    if(lp == LPM_SLEEP || lp == LPM_OFF)
    {
#ifdef _ANYID_STM32_
        GPIO_InitTypeDef  GPIO_InitStructure;

        //Configure I2C1 pins: SCL and SDA
        GPIO_InitStructure.GPIO_Pin =  I2C_SCL.Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(I2C_SCL.Port, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin =  I2C_SDA.Pin;
        GPIO_Init(I2C_SDA.Port, &GPIO_InitStructure);
#endif
    }
    else
        I2C_Init();
}


void I2C_delay(void)
{
   u32 i = 500;              //这里可以优化速度    ，经测试最低到5还能写入
   while(i--);
}

BOOL I2C_Start(void)
{
    SDA_H;
    SCL_H;
    I2C_delay();
    if(!SDA_read)return FALSE;    //SDA线为低电平则总线忙,退出
    SDA_L;
    I2C_delay();
    if(SDA_read) return FALSE;    //SDA线为高电平则总线出错,退出
    SDA_L;
    I2C_delay();
    return TRUE;
}

void I2C_Stop(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}

void I2C_Ack(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

void I2C_NoAck(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

BOOL I2C_WaitAck(void)      //返回为:=1有ACK,=0无ACK
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    if(SDA_read)
    {
        SCL_L;
        return FALSE;
    }
    SCL_L;
    return TRUE;
}

void I2C_SendByte(u8 SendByte) //数据从高位到低位//
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
        if(SendByte&0x80)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }
        SendByte<<=1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

u8 I2C_ReceiveByte(void)  //数据从高位到低位
{
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;
    while(i--)
    {
        ReceiveByte<<=1;
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if(SDA_read)
        {
            ReceiveByte |= 0x01;
        }
    }
    SCL_L;
    return ReceiveByte;
}
