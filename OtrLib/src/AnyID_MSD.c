#include "AnyID_MSD.h"

#define SD_TYPE_SDC1  1
#define SD_TYPE_SDC2  2
#define SD_TYPE_SDCH  3
#define SD_TYPE_MMC1  4
#define SD_TYPE_MMC2  5
#define SD_TYPE_MMCH  6
//SD卡能够支持的电压工作范围
//D31           Card power up status bit (busy) 0 or 1
//D[30-24]      Reserved                        0
//D23           3.5 ~ 3.6V                      1
//D22           3.4 ~ 3.5V                      1
//D21           3.3 ~ 3.4V                      1
//D20           3.2 ~ 3.3V                      1
//D19           3.1 ~ 3.2V                      1
//D18           3.0 ~ 3.1V                      1
//D17           2.9 ~ 3.0V                      1
//D16           2.8 ~ 2.9V                      1
//D15           2.7 ~ 2.8V                      1
//D14           2.6 ~ 2.7V                      0
//D13           2.5 ~ 2.6V                      0
//D12           2.4 ~ 2.5V                      0
//D11           2.3 ~ 2.4V                      0
//D10           2.2 ~2.3V                       0
//D9            2.1 ~ 2.2V                      0
//D8            2.0 ~ 2.1V                      0
//D7            1.65 ~ 1.95V                    0
//D[6-0]        Reserved                        0
#define SD_TYPE_VOLH 7

static u8 MSD_Type = 0;

#define SYS48M_DELAY_500MS  0x16B66C

u8 MSD_WaitReady(void)
{
    u8 res;

    u32 delay = SYS48M_DELAY_500MS;    //500ms
    MSD_WriteByte(DUMMY);
    do{
        res = MSD_ReadByte();
        delay--;
    }while((res != DUMMY) && delay);

    return res;
}

u8 MSD_Init(void)
{
    u32 i = 0, delay = 0;
    u8 res, ocr[4], cmd;
    MSD_SPILowSpeed();

    /*MSD_POWER_OFF();
    delay = SYS48M_DELAY_500MS;
    while(delay--);

    MSD_POWER_ON();
    delay = SYS48M_DELAY_500MS;
    while(delay--);*/

    delay = 0xfff;                         //SD进入IDLE状态SysFeedDog()SysFeedDog()
    do{
        MSD_CS_HIGH();
        for(i = 0; i < 15; i++)
            MSD_WriteByte(DUMMY);

        MSD_CS_LOW();
        res = MSD_SendCommand(CMD0, 0);

        delay--;
    }while(res != MSD_IN_IDLE_STATE && delay);

    if(delay == 0)                      //超时
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    //查看SD卡是否支持设定的工作环境0x01 = 工作电压2.7~3.6V
    //0xAA工作模式检测
    //如果CMD8命令响应非法，说明该卡不是SD2.0的卡，否则读取CMD8的返回值（R7）
    //返回结果的值应该等于0x1AA就说明该卡支持该工作环境（工作电压2.7~3.6V）
    res = MSD_SendCommand(CMD8, 0x1AA);

    if(res & MSD_ILLEGAL_COMMAND)               //如果CMD8是非法命令，说明是SDV1.x
    {
        res = MSD_SendCommand(CMD58, 0);        //查看工作电压范围
        if(res == MSD_IN_IDLE_STATE)
        {
            for(i = 0; i < 4; i++)              //读取4字节的数据
                ocr[i] = MSD_ReadByte();
            if((ocr[1] & 0x1F) && (ocr[2] & 0xff) && (ocr[3] & 0xff))
            {
                MSD_Type = SD_TYPE_VOLH;
                MSD_CS_HIGH();
                return MSD_FAIL;
            }
        }
        else
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }

        res = MSD_SendCommand(ACMD41, 0);           //检测是否是SD卡
        if(res & MSD_ILLEGAL_COMMAND)               //ACMD41非法命令，说明是SD卡
        {
            cmd = CMD1;
            MSD_Type = SD_TYPE_MMC1;
        }
        else if(res <= MSD_IN_IDLE_STATE)
        {
            cmd = ACMD41;
            MSD_Type = SD_TYPE_SDC1;
        }

        delay = 0xffff;
        do{
            res = MSD_SendCommand(cmd, 0);
            delay--;
        }while(res != MSD_RESPONSE_NO_ERROR && delay);

        if(delay == 0)                              //超时
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }
    }
    else if(res == MSD_IN_IDLE_STATE)               //如果CMD8是正确命令，说明是SDV2.0（SDHC）
    {
        for(i = 0; i < 4; i++)                      //读取4字节的数据R7,查看卡是否支持该工作条件
            ocr[i] = MSD_ReadByte();
        if(ocr[2] != 0x01 || ocr[3] != 0xAA)
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }


        res = MSD_SendCommand(ACMD41, 1UL << 30);   //ACMD41非法命令，说明是SD卡
        if(res & MSD_ILLEGAL_COMMAND)
        {
            cmd = CMD1;
            MSD_Type = SD_TYPE_MMC2;
        }
        else if(res <= MSD_IN_IDLE_STATE)
        {
            cmd = ACMD41;
            MSD_Type = SD_TYPE_SDC2;
        }

        delay = 0xffff;
        do{
            res = MSD_SendCommand(cmd, 1UL << 30);  //HCS置位，表明系统支持HC卡
            delay--;
        }while(res != MSD_RESPONSE_NO_ERROR && delay);

        if(delay == 0)                              //超时
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }

        res = MSD_SendCommand(CMD58, 0);            //查看是否时高速卡R3
        if(res <= MSD_IN_IDLE_STATE)
        {
            for(i = 0; i < 4; i++)                  //读取4字节的数据
                ocr[i] = MSD_ReadByte();
            if(ocr[0] & 0x40)                       //D[30]：1—高速卡
                MSD_Type++;
        }
        else
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }
    }
    else
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    res = MSD_SendCommand(CMD16, BLOCK_SIZE);
    if(res != MSD_SUCCESS)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    MSD_CS_HIGH();
    MSD_SPIHighSpeed();

    return MSD_SUCCESS;//(MSD_GoIdleState())
}

u8 MSD_Reset(void)
{
    u32 i = 0, delay = 0;
    u8 res, ocr[4], cmd;
    MSD_SPILowSpeed();
    delay = 0xfff;                         //SD进入IDLE状态
    do{
        MSD_CS_HIGH();
        for(i = 0; i < 15; i++)
            MSD_WriteByte(DUMMY);

        MSD_CS_LOW();
        res = MSD_SendCommand(CMD0, 0);

        delay--;
    }while(res != MSD_IN_IDLE_STATE && delay);

    if(delay == 0)                      //超时
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    //查看SD卡是否支持设定的工作环境0x01 = 工作电压2.7~3.6V
    //0xAA工作模式检测
    //如果CMD8命令响应非法，说明该卡不是SD2.0的卡，否则读取CMD8的返回值（R7）
    //返回结果的值应该等于0x1AA就说明该卡支持该工作环境（工作电压2.7~3.6V）
    res = MSD_SendCommand(CMD8, 0x1AA);

    if(res & MSD_ILLEGAL_COMMAND)               //如果CMD8是非法命令，说明是SDV1.x
    {
        res = MSD_SendCommand(CMD58, 0);        //查看工作电压范围
        if(res == MSD_IN_IDLE_STATE)
        {
            for(i = 0; i < 4; i++)              //读取4字节的数据
                ocr[i] = MSD_ReadByte();
            if((ocr[1] & 0x1F) && (ocr[2] & 0xff) && (ocr[3] & 0xff))
            {
                MSD_Type = SD_TYPE_VOLH;
                MSD_CS_HIGH();
                return MSD_FAIL;
            }
        }
        else
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }

        res = MSD_SendCommand(ACMD41, 0);           //检测是否是SD卡
        if(res & MSD_ILLEGAL_COMMAND)               //ACMD41非法命令，说明是SD卡
        {
            cmd = CMD1;
            MSD_Type = SD_TYPE_MMC1;
        }
        else if(res <= MSD_IN_IDLE_STATE)
        {
            cmd = ACMD41;
            MSD_Type = SD_TYPE_SDC1;
        }

        delay = 0xffff;
        do{
            res = MSD_SendCommand(cmd, 0);
            delay--;
        }while(res != MSD_RESPONSE_NO_ERROR && delay);

        if(delay == 0)                              //超时
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }
    }
    else if(res == MSD_IN_IDLE_STATE)               //如果CMD8是正确命令，说明是SDV2.0（SDHC）
    {
        for(i = 0; i < 4; i++)                      //读取4字节的数据R7,查看卡是否支持该工作条件
            ocr[i] = MSD_ReadByte();
        if(ocr[2] != 0x01 || ocr[3] != 0xAA)
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }


        res = MSD_SendCommand(ACMD41, 1UL << 30);   //ACMD41非法命令，说明是SD卡
        if(res & MSD_ILLEGAL_COMMAND)
        {
            cmd = CMD1;
            MSD_Type = SD_TYPE_MMC2;
        }
        else if(res <= MSD_IN_IDLE_STATE)
        {
            cmd = ACMD41;
            MSD_Type = SD_TYPE_SDC2;
        }

        delay = 0xffff;
        do{
            res = MSD_SendCommand(cmd, 1UL << 30);  //HCS置位，表明系统支持HC卡
            delay--;
        }while(res != MSD_RESPONSE_NO_ERROR && delay);

        if(delay == 0)                              //超时
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }

        res = MSD_SendCommand(CMD58, 0);            //查看是否时高速卡R3
        if(res <= MSD_IN_IDLE_STATE)
        {
            for(i = 0; i < 4; i++)                  //读取4字节的数据
                ocr[i] = MSD_ReadByte();
            if(ocr[0] & 0x40)                       //D[30]：1—高速卡
                MSD_Type++;
        }
        else
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }
    }
    else
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    res = MSD_SendCommand(CMD16, BLOCK_SIZE);
    if(res != MSD_SUCCESS)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    MSD_CS_HIGH();
    MSD_SPIHighSpeed();

    return MSD_SUCCESS;//(MSD_GoIdleState())
}

#define ACMD_FLAG   0x80
#define ACMD_MASK   0x80
#define CMD_STOP_BIT 0x01
u8 MSD_SendCommand(u8 cmd, u32 arg)
{
    u8 n, res;

    if(cmd & ACMD_FLAG)
    {
        //cmd &= ACMD_MASK;
        res = MSD_SendCommand(CMD55, 0);
        if(res > MSD_IN_IDLE_STATE)
          return res;
    }

    //保护SD卡，当SD卡处于忙状态时不能够复位，否则SD就极可能被损坏
    if(MSD_WaitReady() != DUMMY)
        return MSD_FAIL;

    MSD_WriteByte(cmd);
    MSD_WriteByte((u8)(arg >> 24));
    MSD_WriteByte((u8)(arg >> 16));
    MSD_WriteByte((u8)(arg >> 8));
    MSD_WriteByte((u8)arg);
    n = 0xff;
    if(cmd == CMD0)
      n = 0x95;                    /* Valid CRC for CMD0(0) */
    else if(cmd == CMD8)
      n = 0x87;                    /* Valid CRC for CMD8(0x1AA) */
    MSD_WriteByte(n | CMD_STOP_BIT);

    n = 10;
    do{
        res = MSD_ReadByte();
        n--;
    }while((res & 0x80) && n);

    return res;
}

u8 MSD_ReadCSDRegister(MSD_CSD* pCsd)
{
    u32 i = 0, delay;
    u8 res;
    u8 CSD_Tab[16];

    MSD_CS_LOW();

    res = MSD_SendCommand(CMD9, 0);
    if(res != MSD_SUCCESS)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    delay = 0xfff;
    do{
        res = MSD_ReadByte();
        delay--;
    }while(res != MSD_START_DATA_SINGLE_BLOCK_READ && delay);

    if(delay == 0)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    for(i = 0; i < MSD_CSD_LENGTH; i++)
        CSD_Tab[i] = MSD_ReadByte();


    MSD_ReadByte();                     //crc
    MSD_ReadByte();

    MSD_CS_HIGH();

    //Byte0
    pCsd->CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
    pCsd->SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
    pCsd->Reserved1 = CSD_Tab[0] & 0x03;

    //Byte1
    pCsd->TAAC = CSD_Tab[1] ;

    //Byte2
    pCsd->NSAC = CSD_Tab[2];

    //Byte3
    pCsd->MaxBusClkFrec = CSD_Tab[3];

    //Byte4
    pCsd->CardComdClasses = CSD_Tab[4] << 4;

    //Byte5
    pCsd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
    pCsd->RdBlockLen = CSD_Tab[5] & 0x0F;

    //Byte6
    pCsd->PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
    pCsd->WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
    pCsd->RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
    pCsd->DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
    pCsd->Reserved2 = 0; /* Reserved */
    pCsd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;

    //Byte7
    pCsd->DeviceSize |= (CSD_Tab[7]) << 2;

    //Byte8
    pCsd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;
    pCsd->MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
    pCsd->MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);

    //Byte9
    pCsd->MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
    pCsd->MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
    pCsd->DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;

    //Byte10
    pCsd->DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;
    pCsd->EraseGrSize = (CSD_Tab[10] & 0x7C) >> 2;
    pCsd->EraseGrMul = (CSD_Tab[10] & 0x03) << 3;

    //Byte11
    pCsd->EraseGrMul |= (CSD_Tab[11] & 0xE0) >> 5;
    pCsd->WrProtectGrSize = (CSD_Tab[11] & 0x1F);

    //Byte12
    pCsd->WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
    pCsd->ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
    pCsd->WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
    pCsd->MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;

    //Byte13
    pCsd->MaxWrBlockLen |= (CSD_Tab[13] & 0xc0) >> 6;
    pCsd->WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
    pCsd->Reserved3 = 0;
    pCsd->ContentProtectAppli = (CSD_Tab[13] & 0x01);

    //Byte14
    pCsd->FileFormatGrouop = (CSD_Tab[14] & 0x80) >> 7;
    pCsd->CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
    pCsd->PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
    pCsd->TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
    pCsd->FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
    pCsd->ECC = (CSD_Tab[14] & 0x03);

    //Byte15
    pCsd->CRC = (CSD_Tab[15] & 0xFE) >> 1;
    pCsd->Reserved4 = 1;

    return MSD_SUCCESS;
}

u8 MSD_ReadBlock(u8 *pBuffer, u32 addr, u32 len)
{
    u32 i = 0, delay = 0;
    u8 res;
//UDebug_PrintfA('h');
    //高速卡使用“块”为地址单位
    if(MSD_Type == SD_TYPE_SDCH || MSD_Type == SD_TYPE_MMCH)
        addr /= 512;

    MSD_CS_LOW();

    res = MSD_SendCommand(CMD17, addr);

    if(res != MSD_SUCCESS)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    delay = 0xfff;
    do{
        res = MSD_ReadByte();
        delay--;
    }while(res != MSD_START_DATA_SINGLE_BLOCK_READ && delay);

    if(delay == 0)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    for(i = 0; i < len; i++)
        pBuffer[i] = MSD_ReadByte();

    MSD_ReadByte();                     //crc
    MSD_ReadByte();
    MSD_CS_HIGH();
    return MSD_SUCCESS;
}

u8 MSD_ReadBlocks(u8 *pBuffer, u32 addr, u32 len)
{
    u32 i = 0, delay = 0;
    u8 res;
    u8 count = len /512, j = 0;

    //高速卡使用“块”为地址单位
    if(MSD_Type == SD_TYPE_SDCH || MSD_Type == SD_TYPE_MMCH)
        addr /= 512;

    MSD_CS_LOW();

    res = MSD_SendCommand(CMD18, addr);

    if(res != MSD_SUCCESS)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    for(j = 0; j < count; j++)
    {
        delay = 0xfff;
        do{
            res = MSD_ReadByte();
            delay--;
        }while(res != MSD_START_DATA_SINGLE_BLOCK_READ && delay);

        if(delay == 0)
        {
            MSD_CS_HIGH();
            return MSD_FAIL;
        }

        for(i = 0; i < 512; i++)
            pBuffer[i + j * 512] = MSD_ReadByte();

        MSD_ReadByte();                     //crc
        MSD_ReadByte();
    }

    res = MSD_SendCommand(CMD12, 0);

    delay = 0xfff;
    do{
        delay--;
        res = MSD_ReadByte();
    }while(delay && res == MSD_BUS_BUSY);

    if(delay == 0)
    {
        MSD_CS_HIGH();
        return MSD_BUSY;
    }

    MSD_CS_HIGH();
    return MSD_SUCCESS;
}

#define MSD_WRITE_RES_MASK      0x1F
u8 MSD_WriteBlock(u8 *pBuffer, u32 addr, u32 len)
{
    u32 i = 0, delay = 0;
    u8 res;

    //高速卡使用“块”为地址单位
    if(MSD_Type == SD_TYPE_SDCH || MSD_Type == SD_TYPE_MMCH)
        addr /= 512;


    MSD_CS_LOW();

    res = MSD_SendCommand(CMD24, addr);

    if(res != MSD_SUCCESS)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    MSD_WriteByte(DUMMY);                     //one or more byte space

    MSD_WriteByte(MSD_START_DATA_SINGLE_BLOCK_WRITE);
    for(i = 0; i < len; i++)
        MSD_WriteByte(pBuffer[i]);
    MSD_ReadByte();                     //crc
    MSD_ReadByte();

    res = MSD_ReadByte() & MSD_WRITE_RES_MASK;  //数据响应帧
    if(res != MSD_DATA_OK)                //错误返回
    {
        res = MSD_SendCommand(CMD12, 0);

        delay = 0xfff;
        do{
            delay--;
            res = MSD_ReadByte();
        }while(delay && res != DUMMY);

        if(delay == 0)
        {
            MSD_CS_HIGH();
            return MSD_BUSY;
        }

        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    delay = 0xfff;
    do{
        delay--;
        res = MSD_ReadByte();
    }while(delay && res != DUMMY);

    MSD_CS_HIGH();
    return MSD_SUCCESS;
}

u8 MSD_WriteBlocks(u8 *pBuffer, u32 addr, u32 len)
{
    u32 i = 0, delay = 0;
    u8 res, count = len / 512, j = 0;

    //高速卡使用“块”为地址单位
    if(MSD_Type == SD_TYPE_SDCH || MSD_Type == SD_TYPE_MMCH)
    {
        addr /= 512;
    }

    MSD_CS_LOW();

    res = MSD_SendCommand(CMD25, addr);
    if(res != MSD_SUCCESS)
    {
        MSD_CS_HIGH();
        return MSD_FAIL;
    }

    for(j = 0; j < count; j++)
    {
        MSD_WriteByte(DUMMY);                     //one or more byte space
        if(MSD_WaitReady() != DUMMY)
        {
            MSD_CS_HIGH();
            return MSD_BUSY;
        }
        MSD_WriteByte(MSD_START_DATA_MULTIPLE_BLOCK_WRITE);
        for(i = 0; i < 512; i++)
            MSD_WriteByte(pBuffer[i + j * 512]);
        //MSD_ReadByte();                     //crc
        //MSD_ReadByte();

        delay = 0xf;
        do{
            delay--;
            res = MSD_ReadByte() & MSD_WRITE_RES_MASK;
        }while(delay && res != MSD_DATA_OK);

        //res = MSD_ReadByte() & MSD_WRITE_RES_MASK;  //数据响应帧
        if(res != MSD_DATA_OK)                //错误返回
        {
            res = MSD_SendCommand(CMD12, 0);
            //MSD_WriteByte(MSD_STOP_DATA_MULTIPLE_BLOCK_WRITE);
            //MSD_WriteByte(DUMMY);

            delay = 0xff;
            do{
                delay--;
                res = MSD_ReadByte();
            }while(delay && res != DUMMY);

            if(delay == 0)
            {
                MSD_CS_HIGH();
                return MSD_BUSY;
            }

            MSD_CS_HIGH();
            return MSD_FAIL;
        }
        delay = 0xffff;
        do{
            delay--;
            res = MSD_ReadByte();
        }while(delay && res != DUMMY);
    }

    MSD_WriteByte(MSD_STOP_DATA_MULTIPLE_BLOCK_WRITE);

    MSD_CS_HIGH();
    return MSD_SUCCESS;
}
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
