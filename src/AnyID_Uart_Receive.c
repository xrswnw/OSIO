#include "AnyID_Uart_Receive.h"

UART_RCVFRAME g_sUartTempRcvFrame = {0};
u16 Uart_UsrResponseFrame(u8 *pParam, u8 len, u8 cmd, u16 addr, u8 *pBuffer)
{
    u8 pos = 0;
    u16 crc = 0;

    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    memcpy(pBuffer + pos, pParam, len);
    pos += len;

    pBuffer[pos++] = UART_FRAME_FLAG_OK;
    pBuffer[pos++] = UART_FRAME_RSP_NOERR;

    pBuffer[2] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

u16 Uart_UsrRspErrorFrame(u8 errCode, u8 cmd, u16 addr, u8 *pBuffer)
{
    u8 pos = 0;
    u16 crc = 0;

    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    pBuffer[pos++] = UART_FRAME_FLAG_FAIL;
    pBuffer[pos++] = errCode;

    pBuffer[2] = pos - 3 + 2; //减去帧头7E 55 LEN 的三个字节，加上CRC的两个字节

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

u16 Uart_UsrRspLongFrame(u16 *pParams, u16 regNum, u8 errCode, u8 cmd, u16 addr, u8 *pBuffer)
{
    u16 pos = 0;
    u16 crc = 0;
    u16 i = 0;
    
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD1;     // frame head first byte
    pBuffer[pos++] = UART_FRAME_FLAG_HEAD2;     // frame haed second byte
    pBuffer[pos++] = 0x00;                      // length
    pBuffer[pos++] = (addr >> 0) & 0xFF; 
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;                       // cmd
    pBuffer[pos++] = UART_FRAME_PARAM_RFU;      // RFU

    pBuffer[pos++] = 0x00; 
    pBuffer[pos++] = 0x00; 
    for(i = 0; i < regNum; i++)
    {
        pBuffer[pos++] = (pParams[i] >> 8) & 0xFF; 
        pBuffer[pos++] = (pParams[i] >> 0) & 0xFF; 
    }
    if(errCode == UART_FRAME_RSP_NOERR)
    {
        pBuffer[pos++] = UART_FRAME_FLAG_OK;
        pBuffer[pos++] = errCode;
    }
    else
    {
        pBuffer[pos++] = UART_FRAME_FLAG_FAIL;
        pBuffer[pos++] = errCode;
    }

    pBuffer[UART_FRAME_POS_PAR + 1 + 0] = ((pos - 12) >> 0) & 0xFF;
    pBuffer[UART_FRAME_POS_PAR + 1 + 1] = ((pos - 12) >> 8) & 0xFF;

    crc = a_GetCrc(pBuffer + UART_FRAME_POS_LEN, pos - 2); //从LEN开始计算crc
    
    pBuffer[pos++] = (crc >> 0) & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xFF;

    return pos;
}

BOOL Uart_UsrCheckFrame(UART_RCVFRAME *pRcvFrame, u16 *pStartPos, u16 addr)
{
    BOOL b = FALSE;
    u16 pos = 0;

    if(pRcvFrame->len >= UART_FRAME_MIN_LEN)
    {
        while(pos + UART_FRAME_MIN_LEN <= pRcvFrame->len)
        {
            if(pRcvFrame->buffer[pos + 0] == UART_FRAME_FLAG_HEAD1 && pRcvFrame->buffer[pos + 1] == UART_FRAME_FLAG_HEAD2)
            {
                u16 frameLen = 0;
                u16 crc1 = 0, crc2 = 0;
                if(pRcvFrame->buffer[pos + UART_FRAME_POS_LEN])
                {
                    frameLen = pRcvFrame->buffer[pos + 2] + 3;
                }
                else
                {
                    frameLen = pRcvFrame->buffer[pos + UART_FRAME_POS_PAR + 1];
                    frameLen <<= 8;
                    frameLen |= pRcvFrame->buffer[pos + UART_FRAME_POS_PAR + 0];

                    frameLen += UART_FRAME_MIN_LEN + 2;
                }
                if(frameLen >= UART_FRAME_MIN_LEN)
                {
                	u16 destAddr = 0;
                        
                	destAddr = *((u16 *)(pRcvFrame->buffer + pos + UART_FRAME_POS_DESTADDR));
					if(destAddr == UART_USR_ADDR_BROADCAST || destAddr == addr)
					{
						crc1 = a_GetCrc(pRcvFrame->buffer + pos + 2, frameLen - 4);
						crc2 = pRcvFrame->buffer[pos + frameLen - 1];
						crc2 <<= 8;
						crc2 |= pRcvFrame->buffer[pos + frameLen - 2];
						if(crc1 == crc2)
						{
							*pStartPos = pos;
                            b = TRUE;
                            break;
						}
					}
				}
            }
            pos++;
        }
    }

    return b;
}