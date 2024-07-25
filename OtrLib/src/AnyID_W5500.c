#include "AnyID_W5500.h"

u16 g_nW5500LocalPort = 0;

void W5500_WriteReg(u32 addr, u8 data)
{
    W5500_CsLow(); 

    W5500_WriteByte((addr >> 16) & 0xFF);	
    W5500_WriteByte((addr >> 8) & 0xFF);	
    W5500_WriteByte(((addr >> 0) & 0xF8) | W5500_FLAG_WRITE);	 
    W5500_WriteByte(data);   

    W5500_CsHigh();                            
}

u8 W5500_ReadReg(u32 addr)
{
    u8 data = 0;

    W5500_CsLow();     

    W5500_WriteByte((addr >> 16) & 0xFF);	
    W5500_WriteByte((addr >> 8) & 0xFF);	
    W5500_WriteByte((addr >> 0) & 0xF8);
    data = W5500_WriteByte(0x00);    

    W5500_CsHigh();      

    return data;    
}

u16 W5500_WriteBuffer(u32 addr, u8 *pBuffer, u16 len)
{
    u16 i = 0;

    if(len > 0)
    {
        W5500_CsLow();   

        W5500_WriteByte((addr >> 16) & 0xFF);	
        W5500_WriteByte((addr >> 8) & 0xFF);	
        W5500_WriteByte(((addr >> 0) & 0xF8) | W5500_FLAG_WRITE);	 

        for(i = 0; i < len; i++)
        {
            W5500_WriteByte(pBuffer[i]);
        }

        W5500_CsHigh();  
    }

    return len;  
}

u16 W5500_ReadBuffer(u32 addr, u8* pBuffer, u16 len)
{
    u16 i = 0;

    if(len > 0)
    {
        W5500_CsLow();

        W5500_WriteByte((addr >> 16) & 0xFF);	
        W5500_WriteByte((addr >> 8) & 0xFF);	
        W5500_WriteByte((addr >> 0) & 0xF8);  

        for(i = 0; i < len; i++)                   
        {
            pBuffer[i] = W5500_WriteByte(0x00);
        }

        W5500_CsHigh(); 
    }

    return len;
}

u16 W5500_WriteData(u32 addr, u32 data, u8 len)
{
    u16 i = 0;

    if(len > 0)
    {
        u8 buf[4] = {0};
        for(i = 0; i < len; i++)
        {
            buf[i] = (data >> ((len - i - 1) << 3)) & 0xFF;
        }
        
        W5500_CsLow();   

        W5500_WriteByte((addr >> 16) & 0xFF);	
        W5500_WriteByte((addr >> 8) & 0xFF);	
        W5500_WriteByte(((addr >> 0) & 0xF8) | W5500_FLAG_WRITE);	 

        for(i = 0; i < len; i++)
        {
            W5500_WriteByte(buf[i]);
        }

        W5500_CsHigh();  
    }

    return len;  
}

u32 W5500_ReadData(u32 addr, u8 len)
{
    u16 i = 0;
    u32 data = 0;
    
    if(len > 0)
    {
        u8 buf[4] = {0};
        
        W5500_CsLow();

        W5500_WriteByte((addr >> 16) & 0xFF);	
        W5500_WriteByte((addr >> 8) & 0xFF);	
        W5500_WriteByte((addr >> 0) & 0xF8);  

        for(i = 0; i < len; i++)                   
        {
            buf[i] = W5500_WriteByte(0x00);
        }

        W5500_CsHigh(); 

        for(i = 0; i < len; i++)
        {
            data |= (buf[i] << ((len - i - 1) << 3));
        }
    }

    return data;
}


//This fuction is to get socket TX free buf size
//This gives free buffer size of transmit buffer. This is the data size that user can transmit.
//User shuold check this value first and control the size of transmitting data
u16 W5500_SocketGetFreeTxBufSize(u8 s)
{
    u16 val = 0, val1 = 0;
    if(s < W5500_MAX_SOCKET_NUM)
    {
        do
        {
            val1 = W5500_ReadData(W5500_SktTxfmsReg(s), 2);
            if(val1)
            {
                val = W5500_ReadData(W5500_SktTxfmsReg(s), 2);
            }
        }while(val != val1); 
    }

    
    return val;
}

//This fuction is to give size of received data in receive buffer.
u16 W5500_SocketGetRxBufSize(u8 s)
{
    u16 val = 0,val1 = 0;
    if(s < W5500_MAX_SOCKET_NUM)
    {
        do
        {
            val1 = W5500_ReadData(W5500_SktRxsReg(s), 2);
            if(val1)
            {
                val = W5500_ReadData(W5500_SktRxsReg(s), 2);
            }
        }while(val != val1);
    }
    
    return val;
}

//This function is being called by send() and sendto() function also.
//This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
//register. User should read upper byte first and lower byte later to get proper value.
void W5500_SocketWriteBuffer(u8 s, u8 *pBuffer, u16 len)
{
    u16 p = 0;
    u32 addr = 0;

    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(len)
        {
            p = W5500_ReadData(W5500_SktTxmwpReg(s), 2);
            
            addr = (u32)(p << 8) + (s << 5) + 0x10;
            W5500_WriteBuffer(addr, pBuffer, len);

            p += len;
            W5500_WriteData(W5500_SktTxmwpReg(s), p, 2);
        }
    }
}

//This function is being called by recv() also.
//This function read the Rx read pointer register
//and after copy the data from receive buffer update the Rx write pointer register.
//User should read upper byte first and lower byte later to get proper value.
void W5500_SocketReadBuffer(u8 s, u8 *pBuffer, u16 len)
{

    u16 p = 0;
    u32 addr = 0;
    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(len)
        {
            p = W5500_ReadData(W5500_SktRxmrpReg(s), 2);
            
            addr = (u32)(p << 8) + (s << 5) + 0x10;
            W5500_ReadBuffer(addr, pBuffer, len);

            p += len;
            W5500_WriteData(W5500_SktRxmrpReg(s), p, 2);
        }
    }
}


void W5500_SocketMallocBuffer(void)
{
    u8 i = 0;
    //8个通道，每个通道收发都是2k的buffer
    for (i = 0 ;i < W5500_MAX_SOCKET_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
    {
        W5500_WriteReg(W5500_SktTxmsReg(i), 2);
        W5500_WriteReg(W5500_SktRxmsReg(i), 2);
    }
}

//This Socket function initialize the channel in perticular mode, 
//and set the port and wait for W5200 done it.
u8 W5500_SocketOpen(u8 s, u8 protocol, u16 port, u8 flag)
{
    u8 ret = 0;
    
    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(((protocol & 0x0F) == W5500_SKT_MR_TCP)    ||
           ((protocol & 0x0F) == W5500_SKT_MR_UDP)    ||
           ((protocol & 0x0F) == W5500_SKT_MR_IPRAW)  ||
           ((protocol & 0x0F) == W5500_SKT_MR_MACRAW) ||
           ((protocol & 0x0F) == W5500_SKT_MR_PPPOE))
        {
            W5500_SocketClose(s);
            W5500_WriteReg(W5500_SktModeReg(s), protocol | flag);
            if(port == 0) 
            {
                g_nW5500LocalPort++;
                port = g_nW5500LocalPort;
            } 
            W5500_WriteData(W5500_SktSrcPortReg(s), port, 2);
            
            W5500_SocketSetCommand(s, W5500_SKT_CR_OPEN);   //Run sockinit W5500_SKT_CR，注意:这里最好有一个超时时间的设置
            while(W5500_SocketGetCommand(s));
            
            ret = 1;
        }    
    }

    
    return ret;
}

//This function close the socket and parameter is "s" which represent the socket number
void W5500_SocketClose(u8 s)
{
    if(s < W5500_MAX_SOCKET_NUM)
    {
        W5500_SocketSetCommand(s, W5500_SKT_CR_CLOSE);

        while(W5500_SocketGetCommand(s));
        W5500_SocketClearIntFlag(s, 0xFF); //清空所有中断标志       
    }

}


//This function established  the connection for the channel in passive (server) mode. 
//This function waits for the request from the peer.
u8 W5500_SocketListen(u8 s)
{
    u8 ret = 0;
    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(W5500_SocketGetStatus(s) == W5500_SKT_SR_INIT)
        {
            W5500_SocketSetCommand(s, W5500_SKT_CR_LISTEN);
            while(W5500_SocketGetCommand(s));
            ret = 1;
        }
    }

    return ret;
}

//This function established  the connection for the channel in Active (client) mode.
//This function waits for the untill the connection is established.
u8 W5500_SocketConnect(u8 s, u8 *addr, u16 port)
{
    u8 ret;
    u32 ipAddr = 0;

    if(s < W5500_MAX_SOCKET_NUM)
    {
        ipAddr = *((u32 *)addr);
        if((ipAddr != 0xFFFFFFFF) && (ipAddr != 0x00000000) && (port != 0x0000))
        {
            u8 status = 0;
            
            ret = 1;
            
            W5500_WriteBuffer(W5500_SktDstIpReg(s), addr, 4);            
            W5500_WriteData(W5500_SktDstPortReg(s), port, 2);
            
            W5500_SocketSetCommand(s, W5500_SKT_CR_CONNECT);
            while(W5500_SocketGetCommand(s));

            while(status != W5500_SKT_SR_SYNSENT)
            {
                status = W5500_SocketGetStatus(s);
                if(W5500_SocketGetStatus(s) == W5500_SKT_SR_ESTABLISHED)
                {
                    break;
                }
                if(W5500_SocketGetIntFlag(s) & W5500_SKT_IR_TIMEOUT)
                {
                    W5500_SocketClearIntFlag(s, W5500_SKT_IR_TIMEOUT);
                    ret = 0;
                    break;
                }
            }
        }
    }

    return ret;
}

//This function used for disconnect the socket s.
void W5500_SocketDisconnect(u8 s)
{
    if(s < W5500_MAX_SOCKET_NUM)
    {
        W5500_SocketSetCommand(s, W5500_SKT_CR_DISCON);
        while(W5500_SocketGetCommand(s));
    }
}

//This function used to send the data in TCP mode
u16 W5500_SocketTransmit(u8 s, u8 *pBuf, u16 len)
{
    u8 status = 0;
    u16 freesize = 0;
    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(len > W5500_SOCKET_BUF_SIZE) 
        {
            len = W5500_SOCKET_BUF_SIZE;            /*Check size not to exceed MAX size*/
        }
    
        do
        {
            freesize = W5500_SocketGetFreeTxBufSize(s);
            status = W5500_SocketGetStatus(s);
            if((status != W5500_SKT_SR_ESTABLISHED) && (status != W5500_SKT_SR_CLOSE_WAIT))
            {
                len = 0;
                break;
            }
        }while(freesize < len);  
    
        W5500_SocketWriteBuffer(s, pBuf, len);                      
        W5500_SocketSetCommand(s, W5500_SKT_CR_SEND);
    
        while(W5500_SocketGetCommand(s));
    
        while((W5500_SocketGetIntFlag(s) & W5500_SKT_IR_SEND_OK) != W5500_SKT_IR_SEND_OK)
        {
            status = W5500_SocketGetStatus(s);
            if((status != W5500_SKT_SR_ESTABLISHED) && (status != W5500_SKT_SR_CLOSE_WAIT) )
            {
                W5500_SocketClose(s);
                return 0;
            }
        }
        
    #ifdef __DEF_IINCHIP_INT__
        putISR(s, getISR(s) & (~W5500_SKT_IR_SEND_OK));
    #else
        W5500_SocketClearIntFlag(s, W5500_SKT_IR_SEND_OK);
    #endif
    }
    else
    {
        len = 0;
    }
    
    return len;
}

//This function is an application I/F function which is used to receive the data in TCP mode.
//It continues to wait for data as much as the application wants to receive.
u16 W5500_SocketReceive(u8 s, u8 *pBuf, u16 len)
{
    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(len > 0)
        {
            W5500_SocketReadBuffer(s, pBuf, len);
            
            W5500_SocketSetCommand(s, W5500_SKT_CR_RECV);
            while(W5500_SocketGetCommand(s));
        }
    }
    else
    {
        len = 0;
    }
    
    return len;
}

//This function is an application I/F function which is used to send the data for other then TCP mode.
//Unlike TCP transmission, The peer's destination address and the port is needed.
u16 W5500_SocketTransmitTo(u8 s, u8 *pBuf, u16 len, u8 *addr, u16 port)
{
    u8 status = 0;
    u32 ipAddr = 0;
    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(len > W5500_SOCKET_BUF_SIZE) 
        {
            len = W5500_SOCKET_BUF_SIZE;            /*Check size not to exceed MAX size*/
        }
    
        ipAddr = *((u32 *)addr);
        if((ipAddr != 0x00000000) && (port != 0x0000))
        {
            W5500_WriteBuffer(W5500_SktDstIpReg(s), addr, 4);            
            W5500_WriteData(W5500_SktDstPortReg(s), port, 2);
    
            W5500_SocketWriteBuffer(s, pBuf, len);                      
            W5500_SocketSetCommand(s, W5500_SKT_CR_SEND);
            while(W5500_SocketGetCommand(s));
        
            while((W5500_SocketGetIntFlag(s) & W5500_SKT_IR_SEND_OK) != W5500_SKT_IR_SEND_OK)
            {
                status = W5500_SocketGetStatus(s);
                if((status != W5500_SKT_SR_ESTABLISHED) && (status != W5500_SKT_SR_CLOSE_WAIT) )
                {
                    W5500_SocketClose(s);
                    len = 0;
                }
            }
            
        #ifdef __DEF_IINCHIP_INT__
            putISR(s, getISR(s) & (~W5500_SKT_IR_SEND_OK));
        #else
            W5500_SocketClearIntFlag(s, W5500_SKT_IR_SEND_OK);
        #endif
        }
        else
        {
            len = 0;
        }
    }
    else
    {
        len = 0;
    }
    
    return len;
}

//This function is an application I/F function which is used to receive the data in other then
//TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well.
u16 W5500_SocketReceiveFrome(u8 s, u8 *pBuf, u16 len, u8 *addr, u16 *port)
{
    u8 head[8] = {0};
    u16 rcvLen = 0;
    u16 ptr = 0;
    u32 memoryAddr = 0;
    if(s < W5500_MAX_SOCKET_NUM)
    {
        if(len > 0)
        {
            ptr = W5500_ReadData(W5500_SktRxmrpReg(s), 2);
            memoryAddr = (u32)(ptr << 8) + (s << 5) + 0x18;

            switch(W5500_ReadReg(W5500_SktModeReg(s)) & 0x07)
            {
                case W5500_SKT_MR_UDP:
                    W5500_ReadBuffer(memoryAddr, head, 0x08);        
                    ptr += 8;
                    
                    memcpy(addr, head, W5500_IP_LEN);
                    *port = head[4];
                    *port = (*port << 8) + head[5];
                    
                    rcvLen = head[6];
                    rcvLen = (rcvLen << 8) + head[7];

                    memoryAddr = (u32)(ptr << 8) + (s << 5) + 0x18;
                    W5500_ReadBuffer(memoryAddr, pBuf, rcvLen);                
                    ptr += rcvLen;

                    W5500_WriteData(W5500_SktRxmrpReg(s), ptr, 2);
                    
                    break;
                case W5500_SKT_MR_IPRAW:
                    W5500_ReadBuffer(memoryAddr, head, 0x06);        
                    ptr += 6;
                    
                    memcpy(addr, head, W5500_IP_LEN);
                    rcvLen = head[4];
                    rcvLen = (rcvLen << 8) + head[5];

                    memoryAddr = (u32)(ptr << 8) + (s << 5) + 0x18;
                    W5500_ReadBuffer(memoryAddr, pBuf, rcvLen);                
                    ptr += rcvLen;

                    W5500_WriteData(W5500_SktRxmrpReg(s), ptr, 2);
                    
                    break;
                case W5500_SKT_MR_MACRAW:
                    W5500_ReadBuffer(memoryAddr, head, 0x02);        
                    ptr += 2;
                    
                    rcvLen = head[0];
                    rcvLen = (rcvLen << 8) + head[1];
                    if(rcvLen > 1514)
                    {
                        while(1);
                    }

                    memoryAddr = (u32)(ptr << 8) + (s << 5) + 0x18;
                    W5500_ReadBuffer(memoryAddr, pBuf, rcvLen);                
                    ptr += rcvLen;

                    W5500_WriteData(W5500_SktRxmrpReg(s), ptr, 2);
                    
                    break;
                default:
                    break;
            }
            W5500_SocketSetCommand(s, W5500_SKT_CR_RECV);
            while(W5500_SocketGetCommand(s));
        }
    }
    return rcvLen;
}










