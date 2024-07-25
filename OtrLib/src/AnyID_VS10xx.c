#include "AnyID_VS10XX.h"


u8 g_nMp3ErrorTime = 0;;
u8 g_nMp3VolVal = VS10XX_VOL_DEFAULT;

u16 VS10XX_ReadRegister(u8 addressbyte)
{
    u16 resultvalue = 0;
    u8 temp1,temp2;

    VS10XX_SelectControl();
    VS10XX_Delay(1);
    VS10XX_WriteByte(VS10XX_READ_COMMAND);                                                //首先发送READ命令
    VS10XX_WriteByte((addressbyte));                                                  //再发送寄存器地址
    temp1 = VS10XX_WriteByte(0xff);                                                     //读取寄存器数据高字节
    resultvalue = (u16)temp1;
    resultvalue = (resultvalue) << 8;
    temp2 = VS10XX_WriteByte(0xff);                                                   //读取寄存器数据低字节
    resultvalue |= (u16)temp2;
    VS10XX_Delay(1);
    VS10XX_DeselectControl();
    return resultvalue;
}

void VS10XX_SoftReset(void)
{
    VS10XX_WriteRegister(VS10XX_SPI_MODE, 0x08, 0x04); //Newmode, Reset, No L1-2

    VS10XX_Delay(1);        //One millisecond delay
    while(!VS10XX_DREQ);   //wait for startup

    //Send null bytes to data shorterface
    VS10XX_SelectData();
    VS10XX_WriteByte(0);
    VS10XX_WriteByte(0);
    VS10XX_WriteByte(0);
    VS10XX_WriteByte(0);
    VS10XX_DeselectData();
}

void VS10XX_Reset(void)
{
    VS10XX_PutInReset();
    VS10XX_Delay(1);

    VS10XX_WriteByte(0xFF);

    VS10XX_DeselectControl();
    VS10XX_DeselectData();
    VS10XX_ReleaseFromReset();

    while(!VS10XX_DREQ);

    VS10XX_SoftReset();

    //采样率：48kHz， 立体声                                                  //设置VS1003系统参数
    VS10XX_WriteRegister (VS10XX_SPI_AUDATA, 0xbb, 0x81);
    //音量
    VS10XX_WriteRegister(VS10XX_SPI_VOL, g_nMp3VolVal, g_nMp3VolVal);
    g_nMp3VolVal = VS10XX_VOL_DEFAULT;
    //Set clock register, CLKI = Fosc * 4 = 12.288M * 4 = 49.1526M
    VS10XX_WriteRegister(VS10XX_SPI_CLOCKF, 0xD0, 0x00);
}

//测试Mp3是否正常，发送正弦信号
void VS10XX_VsSineTest(void)
{
    VS10XX_PutInReset();
    VS10XX_Delay(100);

    VS10XX_WriteByte(0xFF);

    VS10XX_DeselectControl();
    VS10XX_DeselectData();
    VS10XX_ReleaseFromReset();
    VS10XX_Delay(100);

    VS10XX_WriteRegister(VS10XX_SPI_MODE, 0x08, 0x20);
    while (!VS10XX_DREQ);


    //向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
    //其中n = 0x24, 设定vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
    //这里后应该能从耳机听到一个单一频率的声音
    VS10XX_SelectData();
    VS10XX_WriteByte(0x53);
    VS10XX_WriteByte(0xef);
    VS10XX_WriteByte(0x6e);
    VS10XX_WriteByte(0x24);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_DeselectData();
    VS10XX_Delay (500);


    //推出测试
    VS10XX_SelectData();
    VS10XX_WriteByte(0x45);
    VS10XX_WriteByte(0x78);
    VS10XX_WriteByte(0x69);
    VS10XX_WriteByte(0x74);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_DeselectData();
    VS10XX_Delay(500);


    //其中n = 0x44, 设定vs1003所产生的正弦波的频率值，具体计算方法见vs1003的datasheet
    //这里后应该能从耳机听到另一个单一频率的声音
    VS10XX_SelectData();
    VS10XX_WriteByte(0x53);
    VS10XX_WriteByte(0xef);
    VS10XX_WriteByte(0x6e);
    VS10XX_WriteByte(0x44);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_DeselectData();
    VS10XX_Delay (500);

    //退出测试
    VS10XX_SelectData();
    VS10XX_WriteByte(0x45);
    VS10XX_WriteByte(0x78);
    VS10XX_WriteByte(0x69);
    VS10XX_WriteByte(0x74);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_WriteByte(0x00);
    VS10XX_DeselectData();
    VS10XX_Delay(500);
}


//功能：发送1048个0，WMA格式语音的中断时发送
void VS10XX_SendZeros(void)
{
    u16 i = 0;
    VS10XX_WriteByte(0xff);
    VS10XX_SelectData();
    VS10XX_WriteByte(0);
    for(i = 0; i < 1048; i++)
    {
        while(!VS10XX_DREQ);
        VS10XX_WriteByte(0);
    }
    VS10XX_DeselectData();
}

void VS10XX_Delay(u16 mS)
{
    u16 i;
    u32 j;
    for(i = 0; i < mS; i++)
    {
        for(j = 0; j < 0xfff; j++);
    }
}


void VS10XX_WriteRegister(u8 addressbyte, u8 highbyte, u8 lowbyte)
{
    VS10XX_SelectControl();
    while(!VS10XX_DREQ);
    VS10XX_WriteByte(VS10XX_WRITE_COMMAND);                                               //发送Write命令
    VS10XX_WriteByte((addressbyte));                                                  //发送寄存器地址
    VS10XX_WriteByte((highbyte));                                                     //发送数据高字节
    VS10XX_WriteByte((lowbyte));                                                      //发送数据低字节
    VS10XX_Delay(1);
    VS10XX_DeselectControl();
}

void VS10XX_Send32ByteData(u8 *pBuffer)
{
    u8 i = 0;
    for(i = 0; i < VS10XX_MINSENDTOMP3; i++)
    {
        VS10XX_WriteByte(pBuffer[i]);
    }
}

void VS10XX_SetVolumn(u8 vol)
{
    VS10XX_SelectControl();
    while(!VS10XX_DREQ);
    VS10XX_WriteByte(VS10XX_WRITE_COMMAND);
    VS10XX_WriteByte((VS10XX_SPI_VOL));
    VS10XX_WriteByte(vol);
    VS10XX_WriteByte(vol);
    VS10XX_Delay(1);
    VS10XX_DeselectControl();
}

