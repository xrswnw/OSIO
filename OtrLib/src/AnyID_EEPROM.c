#include "AnyID_EEPROM.h"
/********************************************************************************
** 函数名: EEPROM_WriteByte
** 说  明: 写入数据时，一定要要足够的时间让EEPROM内部完成写的操作，否则连续快速的写数据可能不能成功
**
** 输  入:写入地址，需写入的数据
** 输  出:void
** 返  回: 写入成功TRUE,  写入失败FALSE
********************************************************************************/
BOOL EEPROM_WriteByte(u16 addr, u8 data)
{
    BOOL b = TRUE;

    //开始
    if(!I2C_Start())
    {
        b = FALSE;
    }

    //设备地址和数据高地址
    if(b)
    {
        I2C_SendByte((EEPROM_DEV_ADDR & I2C_WRITE_ADDR) | ((addr >> 7) & 0x06));
        if(!I2C_WaitAck())
        {
            b = FALSE;
        }
    }

    //数据低地址
    if(b)
    {
        I2C_SendByte((addr >> 0) & 0xff);
        b = I2C_WaitAck();
    }

    //写入数据
    if(b)
    {
        I2C_SendByte(data);
        b = I2C_WaitAck();
    }

    //停止
    I2C_Stop();

    return b;
}

/********************************************************************************
** 函数名: EEPROM_WritePage
** 说  明: 注意页的大小
**                 如果从页的边界开始写入数据，数据写满一页为止
**                 如果从页的中间开始写入数据局，写到页末时，地址会翻转到页的开始重新写入
** 输  入: 写入地址，需写入的页数据， 页数据长度
** 输  出: void
** 返  回: 写入成功TRUE,  写入失败FALSE
********************************************************************************/
BOOL EEPROM_WritePage(u16 addr, u8 *pBuffer, u8 len)
{
    u8 i = 0;
    BOOL b = TRUE;

    u8 actualLen = EEPROM_PAGE_SIZE;

    if(len < actualLen)
    {
        actualLen = len;
    }

    //开始
    if(!I2C_Start())
    {
        b = FALSE;
    }

    //设备地址和数据高地址
    if(b)
    {
        I2C_SendByte((EEPROM_DEV_ADDR & I2C_WRITE_ADDR) | ((addr >> 7) & 0x06));
        if(!I2C_WaitAck())
        {
            b = FALSE;
        }
    }

    //数据低地址
    if(b)
    {
        I2C_SendByte((addr >> 0) & 0xff);
        b = I2C_WaitAck();
    }

    //写入数据
    for(i = 0; i < actualLen; i++)
    {
        if(b)
        {
            I2C_SendByte(pBuffer[i]);
            b = I2C_WaitAck();
        }
    }

    //停止
    I2C_Stop();

    return b;
}

/********************************************************************************
** 函数名: EEPROM_ReadCurrenAddr
** 说  明: 从当前位置进行读操作
**
** 输  入: void
** 输  出: 读取到的数据
** 返  回: 读取成功TRUE,  读取失败FALSE
********************************************************************************/
BOOL EEPROM_ReadCurrenAddr(u8 *pData)
{
    BOOL b = TRUE;

    //开始
    if(!I2C_Start())
    {
        b = FALSE;
    }

    //设备地址和读出数据数据
    if(b)
    {
        I2C_SendByte(EEPROM_DEV_ADDR | I2C_READ_ADDR);
        if(!I2C_WaitAck())
        {
            b = FALSE;
        }
    }

    //
    if(b)
    {
        *pData = I2C_ReceiveByte();
        b = (BOOL)(!I2C_WaitAck());     //注意:这里是没有ACK的
    }

    //停止
    I2C_Stop();

    return b;
}

/********************************************************************************
** 函数名: EEPROM_ReadRandomAddr
** 说  明: 从寄存器的任意位置进行读操作
**
** 输  入: 读取位置
** 输  出: 读取到的数据
** 返  回: 读取成功TRUE,  读取失败FALSE
********************************************************************************/
BOOL EEPROM_ReadRandomAddr(u16 addr, u8 *pData)
{

    BOOL b = TRUE;

    //开始
    if(!I2C_Start())
    {
        b = FALSE;
    }


    //设备地址和数据高地址
    if(b)
    {
        I2C_SendByte((EEPROM_DEV_ADDR & I2C_WRITE_ADDR) | ((addr >> 7) & 0x06));
        if(!I2C_WaitAck())
        {
            b = FALSE;
        }
    }

    //数据低地址
    if(b)
    {
        I2C_SendByte((addr >> 0) & 0xff);
        b = I2C_WaitAck();
    }


    //开始
    if(!I2C_Start())
    {
        b = FALSE;
    }

    //设备地址
    if(b)
    {
        I2C_SendByte(EEPROM_DEV_ADDR | I2C_READ_ADDR | ((addr >> 7) & 0x06));
        if(!I2C_WaitAck())
        {
            b = FALSE;
        }
    }

    //读出数据数据
    if(b)
    {
        *pData = I2C_ReceiveByte();
        b = (BOOL)(!I2C_WaitAck());     //注意:这里是没有ACK的
    }

    //停止
    I2C_Stop();

    return b;
}

/********************************************************************************
** 函数名: EEPROM_ReadSequential
** 说  明: 连续读取数据
**
** 输  入:  读取位置， 读取长度
** 输  出:  读取到的数据
** 返  回:  读取成功TRUE,  读取失败FALSE
********************************************************************************/
BOOL EEPROM_ReadSequential(u16 addr, u8 *pBuffer, u16 len)
{
    BOOL b = TRUE;

    u16 i = 0;

    //开始
    if(!I2C_Start())
    {
        b = FALSE;
    }


    //设备地址和数据高地址
    if(b)
    {
        I2C_SendByte((EEPROM_DEV_ADDR & I2C_WRITE_ADDR) | ((addr >> 7) & 0x06));
        if(!I2C_WaitAck())
        {
            b = FALSE;
        }
    }

    //数据低地址
    if(b)
    {
        I2C_SendByte((addr >> 0) & 0xff);
        b = I2C_WaitAck();
    }


    //开始
	if(b)
	{
		if(!I2C_Start())
		{
			b = FALSE;
        }
	}

    //设备地址
    if(b)
    {
        I2C_SendByte(EEPROM_DEV_ADDR | I2C_READ_ADDR | ((addr >> 7) & 0x06));
        if(!I2C_WaitAck())
        {
            b = FALSE;
        }
    }

    //读出数据数据，最后一个数据要以NOACK结尾
	len--;		
	if(b)
	{
		for(i = 0; i < len; i++)
		{
			pBuffer[i] = I2C_ReceiveByte();
			I2C_Ack();
		}
		
		pBuffer[i] = I2C_ReceiveByte();
		I2C_NoAck();
	}
	
    I2C_Stop();

    return b;
}



