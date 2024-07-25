#include "AnyID_CET632_ISO14443A.h"
#include "AnyID_Des.h"

ISO14443A_4 g_sISO1444A4 = {0};
u8 ISO14443A_Init(void)
{
    u8 reg = 0;
    
    //发送控制
    //内部编码，100%调制，TX1正常输出，TX2反向输出，TX1和TX2都使能
    reg = CET632_BIT_MODSRC_IN | CET632_BIT_100ASK | CET632_BIT_TX2INV | CET632_BIT_TX1RFEN | CET632_BIT_TX2RFEN;
    CET632_WriteReg(CET632_REG_TXCTRL, reg);
    //高电平是RF满额输出
    CET632_WriteReg(CET632_REG_TXCW, 0x3F);
    //插入校验位，实行奇校验
    reg = CET632_BIT_PARITYEN | CET632_BIT_PARITYODD;
	CET632_WriteReg(CET632_REG_CRCCTRL, reg);
    //ISO14443A的CRC校验初始值0x6363
	CET632_WriteReg(CET632_REG_CRCPRESETLSB, 0x63);
	CET632_WriteReg(CET632_REG_CRCPRESETMSB, 0x63);
    //清空授权标志
	CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //发送106kbps 米勒编码
    reg = CET632_BIT_A_106K | CET632_BIT_MILLER;
    CET632_WriteReg(CET632_REG_CODERCTRL, reg);
    //Tmod = 2(modWidth + 1)/fc
    //ISO14443A调制宽度是2.36us，sof调制宽度也是2.36us
    CET632_WriteReg(CET632_REG_MODWIDTH, 0x0F);
    CET632_WriteReg(CET632_REG_MODWSOF, 0x0F);

    

    //接收控制
    //接收速率106kbps
    reg = CET632_ReadReg(CET632_REG_RXCTRL1);
    reg &= 0x07;    //保留Gain和lp_off的控制;
    reg |= (CET632_BIT_SUB8PLS | CET632_BIT_SUB16CLK);  
    CET632_WriteReg(CET632_REG_RXCTRL1, reg);
    //接收冲突位设置成0，冲突后的数据全0，rxFrame是ISO14443A，接收解码曼彻斯特编码(ISO14443A)
    reg = CET632_BIT_COLL_BIT1 | CET632_BIT_COLL_ZERO | CET632_BIT_RXFRAM_A | CET632_BIT_RXCODE_MCST;
    CET632_WriteReg(CET632_REG_DECODECTRL, reg);
    //定义TXEOF和RXSOF之间的相位关系
    CET632_WriteReg(CET632_REG_BITPHASE, 0x3D);
    //接收门限
	CET632_WriteReg(CET632_REG_RXTHLD, 0x8C);
    //AGC使能
	CET632_WriteReg(CET632_REG_BPSKCTRL, CET632_BIT_AGCEN);
    //接收延时，当发送完EOF后等待一定时间再开启接收，这样有助于抗干扰
	CET632_WriteReg(CET632_REG_RXWAIT, 0x06); 

    //Timer
	//这个定时器的功能有限，一般自己使用定时器
	CET632_WriteReg(CET632_REG_TIMERCLK, 0x1F);
	CET632_WriteReg(CET632_REG_TIMERRELOAD, 0xFF);

    //清空测试寄存器
    CET632_WriteReg(CET632_REG_TESTANT, 0x00);
	CET632_WriteReg(CET632_REG_ALGADJ2, 0x00);

    //fifo 32	
	CET632_WriteReg(CET632_REG_FIFOLV, CET632_FIFO_LEVEL);
   
    return TRUE;
}

u8 ISO14443A_CheckBcc(u8 *pUID)
{
    u8 bcc = 0;

    bcc = pUID[0] ^ pUID[1] ^ pUID[2] ^ pUID[3] ^ pUID[4];
    if(bcc == 0)
    {
        return CET632_ERR_SUCCESS;
    }
    else
    {
        return CET632_ERR_BCC;
    }
}


u8 ISO14443A_RequestTag(u8 mode, u16 *pTagType)
{
    u8 error = CET632_ERR_SUCCESS;
    u8 txLen = 0, txLastBits = 0;
    u8 rxAlignBit = 0, rxLen = 0;    
	
    g_sCET632Frame.txFrame[txLen++] = mode;
    txLastBits = ISO14443A_FRAME_LSB_BIT7;

    //清空授权标志
    CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //禁止发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);
    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, (rxAlignBit << 4) | txLastBits); 

    a_memset(&g_sISO1444A4, 0, sizeof(g_sISO1444A4));
    
    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_INVENTORY_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 err = 0;
        err = CET632_ReadReg(CET632_REG_ERR);
        if(err & (~CET632_BIT_KEYERR))  //这里不需要处理keyerr
        {
            error = CET632_CheckErr(err);
        }
        else
        {
            *pTagType = g_sCET632Frame.rxFrame[0] | (g_sCET632Frame.rxFrame[1] << 8);
        }
    }
    return error;
}


u8 ISO14443A_AnticollisionTag(u8 lv, u8 *pUID)
{
    u8 error = CET632_ERR_SUCCESS;
    u8 nvb = 0;
    u8 loop = ISO14443A_MAX_ANTCOLL_LOOP;
    u8 nBits = 0;       //上次的防冲突后，最后一个不完整的字节中有多少位是有效的
    u8 nBytes = 0;      //上次的防冲突有后，共有多少个完整的字节
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, txLastBits = 0;
    u8 rxAlignBit = 0, rxLen = 0; 
    u8 collBitIndex = 0;

    //清空授权标志
    //CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //禁止发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);
    while(loop--)
    {
        //数据帧打包，主要是计算NVB
        //如果上次防冲突后，有不完整的字节，该字节也需要拷贝到这次的请求帧中
        //NVB
        nvb = ISO14443A_ANTICOLLISION_NVB_START + ((nBytes << 4) & 0xF0) + (nBits & 0x0F);
        if(nBits > 0)
        {
            nBytes += 1;
        }
        //设置发送buffer
        //select帧长度 + 1字节的发送控制字
        //select帧 = select命令 + nvb + uid + 控制字节
        txLen = 0;
        g_sCET632Frame.txFrame[txLen++] = lv;       //Select命令
        g_sCET632Frame.txFrame[txLen++] = nvb;
        a_memcpy(pUID, g_sCET632Frame.txFrame + txLen, nBytes);
        txLen += nBytes;
        txLastBits = nBits;         //最后一个字节发送bit数目
        rxAlignBit = nBits;

        //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
        CET632_WriteReg(CET632_REG_BITFRAME, (rxAlignBit << 4) | txLastBits);
        error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_INVENTORY_TIME);
        if(error == CET632_ERR_SUCCESS)
        {
            u8 reg = 0;
            //将上次接收未完成的字节补充完整
            //上次接收到的不完整的字节是字节的低位
            //本次接收到的是上次接收的不完整字节的高位
            if(nBits > 0)
            {
                u8 lastRcvByte = 0;
                u8 firstRcvByte = 0;
                lastRcvByte = pUID[nBytes - 1];
                firstRcvByte = g_sCET632Frame.rxFrame[0];
                //保存UID
                pUID[nBytes - 1] = firstRcvByte | lastRcvByte;
                a_memcpy(g_sCET632Frame.rxFrame + 1, pUID + nBytes, rxLen - 1);
            }
            else
            {
                //如果是完整的帧，就直接复制
                a_memcpy(g_sCET632Frame.rxFrame, pUID + nBytes, rxLen);
            }

            reg = CET632_ReadReg(CET632_REG_ERR);
            if(reg & CET632_BIT_COLLERR)
            {
                u8 collPos = 0;

                collPos = CET632_ReadReg(CET632_REG_COLL);
                collBitIndex += collPos;
                //The valid bits shall be part of the UID CLn
                //that was received before a collision occurred followed by a (0)b or (1)b, decided by the PCD.
                //A typical implementation adds a (1)b.
                //既然发生冲突，那么两个冲突的位中肯定有一位是1
                //将冲突位设置成1，并作为有效位放置在接收buf中
                //CET632默认将冲突位设置成1
                nBits = collBitIndex & 0x07;
                nBytes = collBitIndex >> 3;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    if(loop == 0)
    {
        error = CET632_ERR_COLL;
    }

    return error;
}

u8 ISO14443A_SelectTag(u8 lv, u8 *pUID, u8 *pSak)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, txLastBits = 0;
    u8 rxAlignBit = 0, rxLen = 0; 
    
    g_sCET632Frame.txFrame[txLen++] = lv;                   //select0/1/2
    g_sCET632Frame.txFrame[txLen++] = 0x70;                 //nvb 7个字节
    a_memcpy(pUID, g_sCET632Frame.txFrame + txLen, 5);      //UID + BCC
    txLen += 5;
    
    //清空授权标志
    //CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, (rxAlignBit << 4) | txLastBits);
    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_INVENTORY_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 err = 0;
        err = CET632_ReadReg(CET632_REG_ERR);
        if(err & (~CET632_BIT_KEYERR))      //这里不需要处理keyerr
        {
            error = CET632_CheckErr(err);
        }
        else
        {
            *pSak = g_sCET632Frame.rxFrame[0];
        }
    }

    return error;
}

u8 ISO14443A_GetUid(ISO14443A_UID *pUid, u8 mode)
{
    u8 error = 0;

    error = ISO14443A_RequestTag(mode, &(pUid->type));
    if(error == CET632_ERR_SUCCESS)
    {
        u8 uid[ISO14443A_MAX_UID_LEN] = {0};
        error = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT0, uid);
        if(error == CET632_ERR_SUCCESS)
        {
            error = ISO14443A_SelectTag(ISO14443A_CMD_SELECT0, uid, &(pUid->sak));
            if(error == CET632_ERR_SUCCESS)
            {
                pUid->len = ISO14443A_SIGNAL_UID_LEN;
                if(uid[0] == ISO14443A_UID_CT)
                {
                    a_memcpy(uid + 1, pUid->uid, ISO14443A_SIGNAL_UID_LEN - 1);
                    error = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT1, uid);
                    if(error == CET632_ERR_SUCCESS)
                    {
                        error = ISO14443A_SelectTag(ISO14443A_CMD_SELECT1, uid, &(pUid->sak));
                        if(error == CET632_ERR_SUCCESS)
                        {
                            pUid->len = ISO14443A_DOUBLE_UID_LEN;
                            if(uid[0] == ISO14443A_UID_CT)
                            {
                                a_memcpy(uid + 1, pUid->uid + 3, ISO14443A_SIGNAL_UID_LEN - 1);

                                error = ISO14443A_AnticollisionTag(ISO14443A_CMD_SELECT2, uid);
                                if(error == CET632_ERR_SUCCESS)
                                {
                                    error = ISO14443A_SelectTag(ISO14443A_CMD_SELECT2, uid, &(pUid->sak));
                                    if(error == CET632_ERR_SUCCESS)
                                    {
                                        pUid->len = ISO14443A_TRIPLE_UID_LEN;
                                        a_memcpy(uid, pUid->uid + 6, ISO14443A_SIGNAL_UID_LEN);
                                    }
                                }
                            }
                            else
                            {
                                a_memcpy(uid, pUid->uid + 3, ISO14443A_SIGNAL_UID_LEN);
                            }
                        }
                    }
                }
                else
                {
                    a_memcpy(uid, pUid->uid, ISO14443A_SIGNAL_UID_LEN);
                }
            }
        }
    }


    return error;
}

u8 ISO14443A_Halt(void)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, txLastBits = 0;
    u8 rxAlignBit = 0; 
    
    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_HALTA;                   //select0/1/2
    g_sCET632Frame.txFrame[txLen++] = 0x00;                 //nvb 7个字节
    
    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);
    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, (rxAlignBit << 4) | txLastBits);
    CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, 0, 0, ISO14443A_INVENTORY_TIME);

    return error;
}

u8 ISO14443A_AuthM1(u8 *pUID, u8 authMode, u8 *pKey, u8 blockAddr)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 i = 0;
    u8 keyMsb = 0, keyLsb = 0;

    //清空授权标志
    //CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);
    
    for(i = 0; i < ISO14443A_M1_KEY_LEN; i++)
	{
		keyMsb = (pKey[i] >> 4) & 0x0F;
		keyLsb = (pKey[i] >> 0) & 0x0F;
		g_sCET632Frame.txFrame[2 * i] =  (((~keyMsb) << 4) & 0xF0) | keyMsb;
		g_sCET632Frame.txFrame[(2 * i) + 1] = (((~keyLsb) << 4) & 0xF0 ) | keyLsb ;		 
	}
    
    error = CET632_Command(CET632_CMD_LOADKEY, g_sCET632Frame.txFrame, ISO14443A_M1_KEY_LEN << 1, 0, 0, ISO14443A_NORMAL_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 txLen = 0, rxLen = 0;
        g_sCET632Frame.txFrame[txLen++] = authMode;
        g_sCET632Frame.txFrame[txLen++] = blockAddr;
        a_memcpy(pUID, g_sCET632Frame.txFrame + txLen, ISO14443A_SIGNAL_UID_LEN);
        txLen += ISO14443A_SIGNAL_UID_LEN;
        
        error = CET632_Command(CET632_CMD_AUTH, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_AUTH_TIME);
        if(error == CET632_ERR_SUCCESS)
        {
            u8 err = 0;
            err = CET632_ReadReg(CET632_REG_CTRL);
            if((err & CET632_BIT_CRYPTO1ON) == 0)
            {
                error = CET632_ERR_AUTH;
            }
        }
    }
    
    return error;
}

u8 ISO14443A_ReadMifareBlock(u8 blockAddr, u8 *pBlock)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, rxLen = 0;

    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_READ;
    g_sCET632Frame.txFrame[txLen++] = blockAddr;

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, pBlock, &rxLen, ISO14443A_READ_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        if(rxLen == 1)
        {
            error = CET632_ERR_NAK;
        }
        else
        {
            u8 err = 0;
            err = CET632_ReadReg(CET632_REG_ERR);
            error = CET632_CheckErr(err);
        }    
    }
    
    return error;
}

u8 ISO14443A_WriteMifareBlock16(u8 blockAddr, u8 *pBlock)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, rxLen = 0;

    //使能发送和禁止接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCTXEN);
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_WRITE16;
    g_sCET632Frame.txFrame[txLen++] = blockAddr;

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);
    
    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_NORMAL_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 rxLastBit = 0;
        rxLastBit = CET632_ReadReg(CET632_REG_SECONDSTAT) & 0x07;
        if(rxLen == 1 && rxLastBit == ISO14443A_ACK_LEN)
        {
            if(g_sCET632Frame.rxFrame[0] == ISO14443A_ACK_OK)
            {
                rxLen = 0;
                error = CET632_Command(CET632_CMD_TRANCEIVE, pBlock, ISO14443A_M1BLOCK_LEN, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_WRITE_TIME);
                if(error == CET632_ERR_SUCCESS)
                {
                    u8 rxLastBit = 0;
                    rxLastBit = CET632_ReadReg(CET632_REG_SECONDSTAT) & 0x07;
                    if(rxLen == 1 && rxLastBit == ISO14443A_ACK_LEN)
                    {
                        if(g_sCET632Frame.rxFrame[0] != ISO14443A_ACK_OK)
                        {
                            error = CET632_ERR_NAK;
                        }
                    }
                    else
                    {
                        error = CET632_ERR_NAK;
                    }
                }
            }
            else
            {
                error = CET632_ERR_NAK;
            }
        }
        else
        {
            error = CET632_ERR_NAK;
        }
    }
    
    return error;
}



u8 ISO14443A_WriteMifareBlock4(u8 blockAddr, u8 *pBlock)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, rxLen = 0;

    //清空授权标志
    CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //使能发送和禁止接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCTXEN);
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_WRITE4;
    g_sCET632Frame.txFrame[txLen++] = blockAddr;
    a_memcpy(pBlock, g_sCET632Frame.txFrame + txLen, ISO14443A_M0BLOCK_LEN);
    txLen += ISO14443A_M0BLOCK_LEN;

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_WRITE_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 rxLastBit = 0;
        rxLastBit = CET632_ReadReg(CET632_REG_SECONDSTAT) & 0x07;
        if(rxLen == 1 && rxLastBit == ISO14443A_ACK_LEN)
        {
            if(g_sCET632Frame.rxFrame[0] != ISO14443A_ACK_OK)
            {
                error = CET632_ERR_NAK;
            }
        }
        else
        {
            error = CET632_ERR_NAK;
        }
    }
    
    return error;
}


u8 ISO14443A_Value(u8 opMode, u8 *pDataBuffer, u8 transAddr, u8 valueAddr)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, rxLen = 0;

    //使能发送和禁止接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCTXEN);
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN);

    g_sCET632Frame.txFrame[txLen++] = opMode;
    g_sCET632Frame.txFrame[txLen++] = valueAddr;

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);
    
    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_NORMAL_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 rxLastBit = 0;
        rxLastBit = CET632_ReadReg(CET632_REG_SECONDSTAT) & 0x07;
        if(rxLen == 1 && rxLastBit == ISO14443A_ACK_LEN)
        {
            if(g_sCET632Frame.rxFrame[0] == ISO14443A_ACK_OK)
            {
                rxLen = 0;
                //写入value后，是没有返回值的，否则就表示系统出错
                error = CET632_Command(CET632_CMD_TRANCEIVE, pDataBuffer, ISO14443A_M1VALUE_LEN, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_NORMAL_TIME);
                if(error == CET632_ERR_SUCCESS)
                {
                    error = CET632_ERR_NAK;
                }
                else
                {
                    txLen = 0;
                    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_TRANSFER;
                    g_sCET632Frame.txFrame[txLen++] = transAddr;
                    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_WRITE_TIME);
                    if(error == CET632_ERR_SUCCESS)
                    {
                        u8 rxLastBit = 0;
                        rxLastBit = CET632_ReadReg(CET632_REG_SECONDSTAT) & 0x07;
                        if(rxLen == 1 && rxLastBit == ISO14443A_ACK_LEN)
                        {
                            if(g_sCET632Frame.rxFrame[0] != ISO14443A_ACK_OK)
                            {
                                error = CET632_ERR_NAK;
                            }
                        }
                        else
                        {
                            error = CET632_ERR_NAK;
                        }
                    }
                }
            }
            else
            {
                error = CET632_ERR_NAK;
            }
        }
        else
        {
            error = CET632_ERR_NAK;
        }
    }
    
    return error;
}

const u16 fsdi[16] = {16, 24, 32, 40, 48, 64, 96, 128, 256, 257, 257, 257, 257, 257, 257, 257};
u8 ISO14443A_Rats(u8 *pAts, u8 *pLen)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0;

    //清空授权标志
    CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCTXEN);
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_RATS;
    g_sCET632Frame.txFrame[txLen++] = ISO14443A_FSDI_FSD | ISO14443A_CID;  //最多接收64字节

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, pAts, pLen, ISO14443A_WRITE_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 reg = 0;
        reg = CET632_ReadReg(CET632_REG_ERR) & (~CET632_BIT_KEYERR);
        if(reg)
        {
            error = CET632_CheckErr(reg);
        }
        else
        {
            reg = pAts[1];
            g_sISO1444A4.fsc = pAts[1] & 0x0F;
            g_sISO1444A4.fsc = fsdi[g_sISO1444A4.fsc];
            if(reg & 0x20)      //TB是否存在
            {
                if(reg & 0x10)  //TA是否存在
                {
                    g_sISO1444A4.fwt = (pAts[3] >> 4) & 0x0F;
                }
                else
                {
                    g_sISO1444A4.fwt = (pAts[2] >> 4) & 0x0F;
                }
            }
            else
            {

                g_sISO1444A4.fwt = ISO14443A4_FWI_DFT;
            }
            //fwt = (256*16/fc) * 2^fwi = 2^(fwi + 12)/13.56 us
            g_sISO1444A4.fwt = (1 << (g_sISO1444A4.fwt + 12)) / 14;    //单位us
            g_sISO1444A4.fwt += 1000;  //多加1ms
        }
    }
    
    return error;
}

u8 ISO14443A_TransAPDU(u8 *pApdu, u8 len, u8 *pAsk, u8 *pAskLen)
{
    u8 error = CET632_ERR_SUCCESS;
    u8 pcb = 0;

    //清空授权标志
    CET632_ClearRegBit(CET632_REG_CTRL, CET632_BIT_CRYPTO1ON);
    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCTXEN);
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN);

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);
    
    //因为有些处理需要比较长的时间，所以PICC有时会发送SBLOCK请求延时
    //如果响应帧是SBLOCK中的WTX，那么将WTX请求直接发送到PICC，一直到接收真正的响应帧
    do{
        *pAskLen = 0;
        error = CET632_Command(CET632_CMD_TRANCEIVE, pApdu, len, pAsk, pAskLen, g_sISO1444A4.fwt);
        if(error == CET632_ERR_SUCCESS)
        {
            u8 reg = 0;
            reg = CET632_ReadReg(CET632_REG_ERR) & (~CET632_BIT_KEYERR);
            if(reg)
            {
                error = CET632_CheckErr(reg);
            }
            else
            {
                pcb = pAsk[0] & (~ISO14443A_IPCB_NUMMASK);
                if((pcb == ISO14443A_PCB_IBLOCK) || (pcb == ISO14443A_PCB_RBLOCK))
                {
                    if((pAsk[0] & ISO14443A_IPCB_NUMMASK) == (g_sISO1444A4.pcb & ISO14443A_IPCB_NUMMASK))
                    {
                        g_sISO1444A4.pcb = (~g_sISO1444A4.pcb) & ISO14443A_IPCB_NUMMASK;
                    }
                }
                else if(pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX))
                {
                    len = *pAskLen;
                    a_memcpy(pAsk, pApdu, len);
                }
                else
                {
                    error = CET632_ERR_OTHER;
                }
            }
        }
    }while((error == CET632_ERR_SUCCESS) && (pcb == (ISO14443A_PCB_SBLOCK | ISO14443A_SPCB_WTX)));
    
    return error;
}

u8 ISO14443A_DirectCommand(u8 *pFrame, u8 len, u8 *pAsk, u8 *pAskLen)
{
    u8 error = CET632_ERR_SUCCESS;

    //禁止发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCTXEN);
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN);

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, pFrame, len, pAsk, pAskLen, ISO14443A_WRITE_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 reg = 0;
        reg = CET632_ReadReg(CET632_REG_ERR) & (~CET632_BIT_KEYERR);
        if(reg)
        {
            error = CET632_CheckErr(reg);
        }
    }
    
    return error;
}

u8 ISO14443A_Dtu(u8 *pTxFrame, u8 txLen, u8 txBit, u8 *pRxBit, u32 timeout)
{
    u8 error = CET632_ERR_SUCCESS;
    u8 rxAlignBit = 0;

    rxAlignBit = (*pRxBit) & 0x07;
    //禁止发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCTXEN);
    CET632_ClearRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN);

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, (rxAlignBit << 4) | txBit); 
    g_sCET632Frame.rxLen = 0;
    error = CET632_Command(CET632_CMD_TRANCEIVE, pTxFrame, txLen, g_sCET632Frame.rxFrame, &(g_sCET632Frame.rxLen), timeout);
    if(error == CET632_ERR_SUCCESS)
    {
        u8 reg = 0;
        reg = CET632_ReadReg(CET632_REG_ERR) & (~CET632_BIT_KEYERR);
        if(reg)
        {
            error = CET632_CheckErr(reg);
        }
        else
        {
            
        }
    }
    *pRxBit = CET632_ReadReg(CET632_REG_SECONDSTAT) & 0x07;
    
    return error;
}


u8 ISO14443A_AuthM0(u8 *pKey, u8 *pPck)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, rxLen = 0;

    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_PWD;
    a_memcpy(pKey, g_sCET632Frame.txFrame + txLen, ISO14443A_M0KEY_LEN);
    txLen += ISO14443A_M0KEY_LEN;

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, pPck, &rxLen, ISO14443A_READ_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        if(rxLen == 1)
        {
            error = CET632_ERR_NAK;
        }
        else
        {
            u8 err = 0;
            err = CET632_ReadReg(CET632_REG_ERR);
            error = CET632_CheckErr(err);
        }    
    }
    
    return error;
}

u8 ISO14443A_ReadM0Sig(u8 *pSig)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, rxLen = 0;

    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_SIG;
    g_sCET632Frame.txFrame[txLen++] = 0x00;

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, pSig, &rxLen, ISO14443A_READ_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        if(rxLen == 1)
        {
            error = CET632_ERR_NAK;
        }
        else
        {
            u8 err = 0;
            err = CET632_ReadReg(CET632_REG_ERR);
            error = CET632_CheckErr(err);
        }    
    }
    
    return error;
}

u8 ISO14443A_ReadM0Cnt(u8 *pCnt)
{
    u8 error = CET632_ERR_SUCCESS;
    //u8 buffer[ISO14443A_MAX_FRAME_LEN] = {0};
    u8 txLen = 0, rxLen = 0;

    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_CNT;
    g_sCET632Frame.txFrame[txLen++] = 0x02;

    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, pCnt, &rxLen, ISO14443A_READ_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        if(rxLen == 1)
        {
            error = CET632_ERR_NAK;
        }
        else
        {
            u8 err = 0;
            err = CET632_ReadReg(CET632_REG_ERR);
            error = CET632_CheckErr(err);
        }    
    }
    
    return error;
}


//value !value value addr !addr addr ! addr
//value是有符号的4字节，低字节在低地址
void ISO14443A_FormatValue(u8 backUpAddr, int value, u8 *pBlockBuffer)
{
    u32 tempValue = 0;
    u8 tempAddr = 0;

    tempValue = ~value;
    tempAddr = ~backUpAddr;
    
    pBlockBuffer[0] = (value >> 0) & 0xFF;
    pBlockBuffer[1] = (value >> 8) & 0xFF;
    pBlockBuffer[2] = (value >> 16) & 0xFF;
    pBlockBuffer[3] = (value >> 24) & 0xFF;

    pBlockBuffer[4] = (tempValue >> 0) & 0xFF;
    pBlockBuffer[5] = (tempValue >> 8) & 0xFF;
    pBlockBuffer[6] = (tempValue >> 16) & 0xFF;
    pBlockBuffer[7] = (tempValue >> 24) & 0xFF;

    pBlockBuffer[8] = (value >> 0) & 0xFF;
    pBlockBuffer[9] = (value >> 8) & 0xFF;
    pBlockBuffer[10] = (value >> 16) & 0xFF;
    pBlockBuffer[11] = (value >> 24) & 0xFF;

    pBlockBuffer[12] = backUpAddr;
    pBlockBuffer[13] = tempAddr;
    pBlockBuffer[14] = backUpAddr;
    pBlockBuffer[15] = tempAddr;
}

int ISO14443A_CheckValueFormat(u8 *pBlockBuffer)
{
    u32 value1 = 0, value2 = 0, value3 = 0;
    u8 addr1 = 0, addr2 = 0, addr3 = 0, addr4 = 0;
    int rlt = CET632_ERR_SUCCESS;

    value1 |= (pBlockBuffer[0] << 0);
    value1 |= (pBlockBuffer[1] << 8);
    value1 |= (pBlockBuffer[2] << 16);
    value1 |= (pBlockBuffer[3] << 24);

    value2 |= (pBlockBuffer[4] << 0);
    value2 |= (pBlockBuffer[5] << 8);
    value2 |= (pBlockBuffer[6] << 16);
    value2 |= (pBlockBuffer[7] << 24);
    value2 = ~value2;

    value3 |= (pBlockBuffer[8] << 0);
    value3 |= (pBlockBuffer[9] << 8);
    value3 |= (pBlockBuffer[10] << 16);
    value3 |= (pBlockBuffer[11] << 24);
    
    addr1 = pBlockBuffer[12];
    addr2 = pBlockBuffer[13];
    addr2 = ~addr2;
    addr3 = pBlockBuffer[14];
    addr4 = pBlockBuffer[15];
    addr4 = ~addr4;

    if((value1 != value3) || (value2 != value3) || 
       (addr1 != addr2) || (addr2 != addr3) || (addr3 != addr4))
    {
        rlt = CET632_ERR_OTHER;
    }

    return rlt;
        
}


u8 ISO14443A_AuthUltralightC(u8 *pKey)
{
    u8 error = CET632_ERR_SUCCESS;
    /*u8 iv[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 rnd[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 rnda[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    u8 rndb[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u8 t = 0;
    u8 txLen = 0, rxLen = 0;

    memcpy(rnd, rnda, 8);
    
    Des_InitializeKey(pKey, 0);
    Des_InitializeKey(pKey + 8, 1);

    g_sCET632Frame.txFrame[txLen++] = ISO14443A_CMD_AUTHM0U2S1;
    g_sCET632Frame.txFrame[txLen++] = 0x00;
    

    //使能发送和接收CRC，ISO14443A初始化已经设置了奇偶校验
    CET632_SetRegBit(CET632_REG_CRCCTRL, CET632_BIT_CRCRXEN | CET632_BIT_CRCTXEN);
    //设置最后一个字节的bit数和接收的第一个bit放置在第一个字节的起始位置
    CET632_WriteReg(CET632_REG_BITFRAME, 0);

    error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_NORMAL_TIME);
    if(error == CET632_ERR_SUCCESS)
    {
        g_sCET632Frame.txFrame[0] = ISO14443A_CMD_AUTHM0U2S2;
        //解密rndb
        Des_DecryptAnyLength(g_sCET632Frame.rxFrame + 1, 8, rndb, 0); 
        Des_EncryptAnyLength(rndb, 8, temp, 1); 
        Des_DecryptAnyLength(temp, 8, rndb, 0); 
        //CBC模式
        a_xor(rndb, iv, 8);
        memcpy(iv, g_sCET632Frame.rxFrame + 1, 8);

        //rndb循环左移8bit得到rndb1
        t = rndb[0];
        memcpy(rndb, rndb + 1, 7);
        rndb[7] = t;  

        //CBC模式
        a_xor(rnda, iv, 8);
        //计算ek(rnda)
        Des_EncryptAnyLength(rnda, 8, g_sCET632Frame.txFrame + 1, 0); 
        Des_DecryptAnyLength(g_sCET632Frame.txFrame + 1, 8, temp, 1); 
        Des_EncryptAnyLength(temp, 8, g_sCET632Frame.txFrame + 1, 0); 
        memcpy(iv, g_sCET632Frame.txFrame + 1, 8);

        //CBC模式
        a_xor(rndb, iv, 8);
        //计算ek(rndb1)
        Des_EncryptAnyLength(rndb, 8, g_sCET632Frame.txFrame + 9, 0); 
        Des_DecryptAnyLength(g_sCET632Frame.txFrame + 9, 8, temp, 1); 
        Des_EncryptAnyLength(temp, 8, g_sCET632Frame.txFrame + 9, 0); 
        memcpy(iv, g_sCET632Frame.txFrame + 9, 8);

        txLen = 1 + 8 + 8;
        error = CET632_Command(CET632_CMD_TRANCEIVE, g_sCET632Frame.txFrame, txLen, g_sCET632Frame.rxFrame, &rxLen, ISO14443A_NORMAL_TIME);
        if(error == CET632_ERR_SUCCESS)
        {
            //解密rnda
            Des_DecryptAnyLength(g_sCET632Frame.rxFrame + 1, 8, rnda, 0); 
            Des_EncryptAnyLength(rnda, 8, temp, 1); 
            Des_DecryptAnyLength(temp, 8, rnda, 0); 
            //CBC模式
            a_xor(rnda, iv, 8);

            //rndb循环右移8bit得到rnda
            t = rnd[0];
            memcpy(rnd, rnd + 1, 7);
            rnd[7] = t;   
            if(memcmp(rnda, rnd, 8) != 0)
            {
                error = CET632_ERR_AUTHM0U2;
            }
        }
    }*/

    return error;
}
