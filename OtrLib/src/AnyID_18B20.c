#include "AnyID_18B20.h"

BOOL DS18B20_Reset(void)
{
    BOOL b = TRUE;
    u8 delay = 0;

    DS18B20_EnableOut();
    DS18B20_SetLow();
    DS18B20_Delay5us(100);  //min480us

    DS18B20_EnableIn();

    delay = 0;
    do{
        delay++;
        DS18B20_Delay5us(1); //15~60us
    }while(DS18B20_GetDat() && delay < 12);
    if(delay >= 12)
    {
        b = FALSE;
    }

    delay = 0;
    do{
        delay++;
        DS18B20_Delay5us(1); //60~240us
    }while(DS18B20_GetDat() == 0 && delay < 50);
    if(delay >= 50)
    {
        b = FALSE;
    }
    DS18B20_Delay5us(1);
	return b;
}


void DS18B20_WriteByte(u8 t)
{
    u8 i = 0;


    for(i = 0; i < 8; i++)
    {
        DS18B20_EnableOut();
        if(t & 0x01)
        {
            DS18B20_SetLow();
            DS18B20_Delay5us(1);
            DS18B20_EnableIn();
            DS18B20_Delay5us(11);
        }
        else
        {
            DS18B20_SetLow();
            DS18B20_Delay5us(11);
            DS18B20_EnableIn();
            DS18B20_Delay5us(1);
        }
        t >>= 1;
    }
}


u8 DS18B20_ReadByte(void)
{
    u8 t = 0;
    u8 i = 0;


    for(i = 0; i < 8; i++)
    {
        DS18B20_EnableOut();
        DS18B20_SetLow();
        DS18B20_Delay5us(1);
		DS18B20_EnableIn();
        DS18B20_Delay5us(1);
        if(DS18B20_GetDat())
        {
            t |= (1 << i);
        }
        DS18B20_Delay5us(10);
    }

    return t;
}


void DS18B20_Init(u16 alarmT, u8 config)
{
    BOOL b = FALSE;
    b = DS18B20_Reset();
    if(b)
    {
        DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
        DS18B20_WriteByte(DS18B20_CMD_WRITE);
        DS18B20_WriteByte((alarmT >> 8) & 0xFF);
        DS18B20_WriteByte((alarmT >> 0) & 0xFF);
        DS18B20_WriteByte(config & 0xFF);
    }
}

u16 DS18B20_ReadTempr(void)
{
    BOOL b = FALSE;
    u16 t = 0x7FFF;
	b = DS18B20_Reset();
    if(b)
    {
        DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
        DS18B20_WriteByte(DS18B20_CMD_READ);
        t = DS18B20_ReadByte();
        t += (DS18B20_ReadByte() << 8);
    }

    return t;
}

void DS18B20_StartConvert(void)
{
    BOOL b = FALSE;
    b = DS18B20_Reset();
    if(b)
    {
        DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
        DS18B20_WriteByte(DS18B20_CMD_COVN);
    }
}




























