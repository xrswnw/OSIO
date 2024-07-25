#include "AnyID_RC522_HL.h"

/******************************************************************************
** 函数名：RC522_InitInterface
** 说  明: 初始化RC522操作接口
**
** 参  数：void
** 返  回：void
******************************************************************************/
void RC522_InitInterface(void)
{
#ifdef _ANYID_PIC18_
    //C3_CLK, C4_SDI, C5_SDO
    //设置SPI功能口的输出/输入
    LATC |= 0x38;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 0;

    //Set the SPI module
    SSPSTAT = 0x40;
    SSPCON1 = 0x20;
    
    //CS输出
    RC522_CS_TRIS = 0;

    //RST输出
    RC522_RST_TRIS = 0;
#endif
}


/******************************************************************************
** 函数名：RC522_WriteByte
** 说  明: 写入RC522一个字节
**
** 参  数：写入SPI的值
** 返  回：SPI读取的值
******************************************************************************/
u8 RC522_WriteByte(u8 byte)
{
#ifdef _ANYID_PIC18_
    do
    {
        SSPCON1bits.WCOL = 0;
        SSPBUF = byte;
    } while(SSPCON1bits.WCOL);


    return SSPBUF;
#endif
}


