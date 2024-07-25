#include "AnyID_AS1115.h"


/******************************************************************************
** 函数名：AS1115_Init
** 说  明：初始化AS1115，设定译码模式，以及是否使用默认地址
**
** 参  数：输入：decodeMode：译码模式
** 返  回：void
******************************************************************************/
void AS1115_Init(u8 decodeMode)
{
    //10进制译码,全部译码
    AS1115_WriteByte(AS1115_FACTORY_ADDR, AS1115_ADDR_DECODE_MOD, AS1115_DECODE_BHEX(decodeMode));
    //最亮
    AS1115_WriteByte(AS1115_FACTORY_ADDR, AS1115_ADDR_GLOBAL_INTENSITY, AS1115_ITST_DUTYCYCLE_02);//AS1115_ITST_DUTYCYCLE_0F
    //全部扫描
    AS1115_WriteByte(AS1115_FACTORY_ADDR, AS1115_ADDR_SCAN_LIMIT, AS1115_SCAN_DISPLAY_D07);
     //默认控制:内部时钟、控制寄存器不复位、10进制译码、不闪烁
    AS1115_WriteByte(AS1115_FACTORY_ADDR, AS1115_ADDR_FEATURE, AS1115_SYS_CTRL_DEFAULT);

    //Normal状态
    AS1115_WriteByte(AS1115_FACTORY_ADDR, AS1115_ADDR_SHUT_DOWN, AS1115_NORMAL_UNCHANGE_FEATURE);
//MTD中使用默认地址就可以了，不需要使用用户地址模式
#ifndef _ANYID_STM32_MTD_
    //获取用户自定义地址，只有在Normal状态才可以使用SEFL_ADDRESS指令
    AS1115_WriteByte(AS1115_FACTORY_ADDR, AS1115_ADDR_SELF_ADRESS, AS1115_ADDR_MODE_USER);
#endif
}

/******************************************************************************
** 函数名：AS1115_WriteByte
** 说  明：向AS1115内部寄存器写入数据
**
** 参  数：输入：addr：AS1115设备地址(I2C地址)
                 command：AS1115命令
                 data：写入的数据(字节)
** 返  回：写命令是否成功
******************************************************************************/
BOOL AS1115_WriteByte(u8 addr, u8 command, u8 data)
{
    BOOL b = FALSE;

    if(!I2C_Start())return FALSE;

    I2C_SendByte(addr & I2C_WRITE_ADDR);//设置高起始地址+器件地址
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}

    I2C_SendByte(command);   //设置低起始地址
    b = I2C_WaitAck();

    I2C_SendByte(data);
    b = I2C_WaitAck();

    I2C_Stop();

    return b;
}

