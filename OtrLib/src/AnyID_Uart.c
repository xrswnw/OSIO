#include "AnyID_Uart.h"

void Uart_WriteBuffer(u8 *pBuffer, u32 len)
{
    u32 i = 0;
    for(i = 0; i < len; i++)
    {
        Uart_WriteByte(pBuffer[i]);
    }
}

void Uart_WriteString(u8 *pString)
{
    u8 i = 0;
    while(pString[i] != '\0')
    {
        Uart_WriteByte(pString[i]);
        i++;
    }
}

void Uart_WriteH(u32 data)
{
    u8 i = 0;

    u8 temp = 0;

    Uart_WriteByte('0');
    Uart_WriteByte('x');

    for(i = 0; i < 8; i++)
    {
        temp = (data & 0xf0000000) >> 28;

        if(temp > 9)
        {
            temp = temp - 10 + 'A';
        }
        else
        {
            temp += '0';
        }

        Uart_WriteByte(temp);

        data <<= 4;
    }
}


void Uart_WriteD(s32 data)
{
    u8 decimal[10] = {0};

    u8 i = 0;
    if(data < 0)
    {
        Uart_WriteByte('-');
        data *= -1;
    }
    else
    {
        //Uart_WriteByte('+');
    }
    for(i = 0; i < 10; i++)
    {

        decimal[i] = data % 10;
        data = data / 10;

        if(data == 0)
        {
            i++;
            break;
        }
    }


    for(; i > 0; i--)
    {
        Uart_WriteByte(decimal[i - 1] + '0');
    }
}



void Uart_WriteArray(u8 *pFrame, u8 len)
{
    u8 j = 0;

    u8 temp = 0;
    for(j = 0; j < len; j++)
    {
        temp = (pFrame[j] & 0xF0) >> 4;
        if(temp > 9)
        {
            temp = temp - 10 + 'A';
        }
        else
        {
            temp += '0';
        }
        Uart_WriteByte(temp);

        temp = (pFrame[j] & 0x0F);
        if(temp > 9)
        {
            temp = temp - 10 + 'A';
        }
        else
        {
            temp += '0';
        }
        Uart_WriteByte(temp);
    }
}


