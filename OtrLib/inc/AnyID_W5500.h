#ifndef _ANYID_W5500_
#define _ANYID_W5500_


#if defined(_ANYID_N232_)
    #include "AnyID_N232_W5500_HL.h"
#endif

#define W5500_REG_MODE              (0x000000)
// Gateway IP Register address
#define W5500_REG_GWADDR            (0x000100)
// Subnet mask Register address
#define W5500_REG_SUBADDR           (0x000500)
// Source MAC Register address
#define W5500_REG_MACADDR           (0x000900)
// Source IP Register address
#define W5500_REG_LIPADDR           (0x000F00)
// set Interrupt low level timer register address
// 控制中断引脚负脉冲宽度
#define W5500_REG_INTLT             (0x001300)
// Interrupt Registe
#define W5500_REG_INT               (0x001500)
// Interrupt mask registe
#define W5500_REG_INTMASK           (0x001600)
// Socket Interrupt Registe
#define W5500_REG_SKTINT            (0x001700) 
// Socket Interrupt Mask Registe
#define W5500_REG_SKTINTMSK         (0x001800)
// Retry time value register ( 1 is 100us )
#define W5500_REG_RT                (0x001900)
// Retry count reigste
#define W5500_REG_RC                (0x001B00)
//PPP LCP Request Timer register  in PPPoE mode
#define W5500_REG_PRT               (0x001C00)
// PPP LCP Magic number register  in PPPoE mode
#define W5500_REG_PMN               (0x001D00)
// PPP Destination MAC Register address
#define W5500_REG_PDMAC             (0x001E00)
//PPP Session Identification Register
#define W5500_REG_PSID0             (0x002400)
// PPP Maximum Segment Size(MSS) registe
#define W5500_REG_PMSS              (0x002600)
// Unreachable IP register address in UDP mode
#define W5500_REG_URCHIP            (0x002800)
// Unreachable Port register address in UDP mode
#define W5500_REG_URCHPORT          (0x002C00)
// PHY Configuration Registe
#define W5500_REG_PHY               (0x002E00)
// chip version register address
#define W5500_REG_VER               (0x003900)   
// socket Mode registe
#define W5500_SktModeReg(ch)        (0x000008 + (ch << 5))
// channel Sn_CR registe
#define W5500_SktCmdReg(ch)         (0x000108 + (ch << 5))
// channel interrupt registe
#define W5500_SktIntReg(ch)         (0x000208 + (ch << 5))
// channel status registe
#define W5500_SktStatReg(ch)        (0x000308 + (ch << 5))
// source port registe
#define W5500_SktSrcPortReg(ch)     (0x000408 + (ch << 5))
// Peer MAC register address
#define W5500_SktDstMacReg(ch)      (0x000608 + (ch << 5))
// Peer IP register address
#define W5500_SktDstIpReg(ch)       (0x000C08 + (ch << 5))
// Peer port register address
#define W5500_SktDstPortReg(ch)     (0x001008 + (ch << 5))
// Maximum Segment Size(Sn_MSSR0) register address
#define W5500_SktMssReg(ch)         (0x001208 + (ch << 5))
// IP Type of Service(TOS) Register
#define W5500_SktTosReg(ch)         (0x001508 + (ch << 5))
// IP Time to live(TTL) Register
#define W5500_SktTtlReg(ch)         (0x001608 + (ch << 5))
// Receive memory size reigste
#define W5500_SktRxmsReg(ch)        (0x001E08 + (ch << 5))
// Transmit memory size reigste
#define W5500_SktTxmsReg(ch)        (0x001F08 + (ch << 5))
// Transmit free memory size registe
#define W5500_SktTxfmsReg(ch)       (0x002008 + (ch << 5))
// Transmit memory read pointer register address
#define W5500_SktTxmrpReg(ch)       (0x002208 + (ch << 5))
// Transmit memory write pointer register address
#define W5500_SktTxmwpReg(ch)       (0x002408 + (ch << 5))
// Received data size registe
#define W5500_SktRxsReg(ch)         (0x002608 + (ch << 5))
// Read point of Receive memory
#define W5500_SktRxmrpReg(ch)       (0x002808 + (ch << 5))
// Write point of Receive memory
#define W5500_SktRxmwpReg(ch)       (0x002A08 + (ch << 5))
// socket interrupt mask registe
#define W5500_SktIntmReg(ch)        (0x002C08 + (ch << 5))
// frag field value in IP header registe
#define W5500_SktIpFlagReg(ch)      (0x002D08 + (ch << 5))
// Keep Timer registe
#define W5500_SktKtvReg(ch)         (0x002F08 + (ch << 5))

//MODE register values
#define W5500_MR_RST                0x80   // reset 
#define W5500_MR_WOL                0x20   // Wake on Lan 
#define W5500_MR_PB                 0x10   // ping block 
#define W5500_MR_PPPOE              0x08   // enable pppoe 
#define W5500_MR_UDP_FARP           0x02   // enbale FORCE ARP 
// IR register values 
#define W5500_IR_CONFLICT           0x80 // check ip confict 
#define W5500_IR_UNREACH            0x40 // get the destination unreachable message in UDP sending 
#define W5500_IR_PPPoE              0x20 // get the PPPoE close message 
#define W5500_IR_MAGIC              0x10 // get the magic packet interrupt 

// Sn_MR values 
#define W5500_SKT_MR_CLOSE          0x00     // unused socket 
#define W5500_SKT_MR_TCP            0x01     // TCP 
#define W5500_SKT_MR_UDP            0x02     // UDP 
#define W5500_SKT_MR_IPRAW          0x03      // IP LAYER RAW SOCK 
#define W5500_SKT_MR_MACRAW         0x04      // MAC LAYER RAW SOCK 
#define W5500_SKT_MR_PPPOE          0x05     // PPPoE 
#define W5500_SKT_MR_UCASTB         0x10     // Unicast Block in UDP Multicating
#define W5500_SKT_MR_ND             0x20     // No Delayed Ack(TCP) flag 
#define W5500_SKT_MR_MC             0x20     // Multicast IGMP (UDP) flag 
#define W5500_SKT_MR_BCASTB         0x40     // Broadcast blcok in UDP Multicating 
#define W5500_SKT_MR_MULTI          0x80     // support UDP Multicating 

 // Sn_MR values on MACRAW MODE 
#define W5500_SKT_MR_MIP6N          0x10     // IPv6 packet Block 
#define W5500_SKT_MR_MMB            0x20     // IPv4 Multicasting Block 
//#define Sn_MR_BCASTB                 0x40     // Broadcast blcok 
#define W5500_SKT_MR_MFEN           0x80     // support MAC filter enable 

// Sn_CR values 
#define W5500_SKT_CR_OPEN           0x01     // initialize or open socket 
#define W5500_SKT_CR_LISTEN         0x02     // wait connection request in tcp mode(Server mode) 
#define W5500_SKT_CR_CONNECT        0x04     // send connection request in tcp mode(Client mode) 
#define W5500_SKT_CR_DISCON         0x08     // send closing reqeuset in tcp mode 
#define W5500_SKT_CR_CLOSE          0x10     // close socket 
#define W5500_SKT_CR_SEND           0x20     // update txbuf pointer, send data 
#define W5500_SKT_CR_SEND_MAC       0x21     // send data with MAC address, so without ARP process 
#define W5500_SKT_CR_SEND_KEEP      0x22     //  send keep alive message 
#define W5500_SKT_CR_RECV           0x40     // update rxbuf pointer, recv data 


#define W5500_SKT_IR_SEND_OK        0x10     // complete sending 
#define W5500_SKT_IR_TIMEOUT        0x08     // assert timeout 
#define W5500_SKT_IR_RECV           0x04     // receiving data 
#define W5500_SKT_IR_DISCON         0x02     // closed socket 
#define W5500_SKT_IR_CON            0x01     // established connection 

// Sn_SR values 
#define W5500_SKT_SR_CLOSED         0x00     // closed 
#define W5500_SKT_SR_INIT           0x13     // init state 
#define W5500_SKT_SR_LISTEN         0x14     // listen state 
#define W5500_SKT_SR_SYNSENT        0x15     // connection state 
#define W5500_SKT_SR_SYNRECV        0x16     // connection state 
#define W5500_SKT_SR_ESTABLISHED    0x17     // success to connect 
#define W5500_SKT_SR_FIN_WAIT       0x18     // closing state 
#define W5500_SKT_SR_CLOSING        0x1A     // closing state 
#define W5500_SKT_SR_TIME_WAIT      0x1B     // closing state 
#define W5500_SKT_SR_CLOSE_WAIT     0x1C     // closing state 
#define W5500_SKT_SR_LAST_ACK       0x1D     // closing state 
#define W5500_SKT_SR_UDP            0x22     // udp socket 
#define W5500_SKT_SR_IPRAW          0x32     // ip raw mode socket 
#define W5500_SKT_SR_MACRAW         0x42     // mac raw mode socket 
#define W5500_SKT_SR_PPPOE          0x5F     // pppoe socket 

// IP PROTOCOL 
#define W5500_IPPROTO_IP            0        // Dummy for IP 
#define W5500_IPPROTO_ICMP          1        // Control message protocol 
#define W5500_IPPROTO_IGMP          2        // Internet group management protocol 
#define W5500_IPPROTO_GGP           3        // Gateway^2 (deprecated) 
#define W5500_IPPROTO_TCP           6        // TCP 
#define W5500_IPPROTO_PUP           12       // PUP 
#define W5500_IPPROTO_UDP           17       // UDP 
#define W5500_IPPROTO_IDP           22       // XNS idp 
#define W5500_IPPROTO_ND            77       // UNOFFICIAL net disk protocol 
#define W5500_IPPROTO_RAW           255      // Raw IP packet 



#define W5500_FLAG_WRITE            0x04
#define W5500_MAX_SOCKET_NUM        8
#define W5500_SOCKET_BUF_SIZE       2048
#define W5500_IP_LEN                4
#define W5500_MAC_LEN               6
void W5500_WriteReg(u32 addr, u8 data);
u8 W5500_ReadReg(u32 addr);
u16 W5500_WriteBuffer(u32 addr, u8 *pBuffer, u16 len);
u16 W5500_ReadBuffer(u32 addr, u8 *pBuffer, u16 len);
u16 W5500_WriteData(u32 addr, u32 data, u8 len);
u32 W5500_ReadData(u32 addr, u8 len);


#define W5500_SetModeReg(v)             W5500_WriteReg(W5500_REG_MODE, (v));
#define W5500_SoftReset()               W5500_WriteReg(W5500_REG_MODE, W5500_MR_RST)        //iinchip_init
#define W5500_SetGatewayAddr(addr)      W5500_WriteBuffer(W5500_REG_GWADDR, (addr), W5500_IP_LEN)
#define W5500_GetGatewayAddr(addr)      W5500_ReadBuffer(W5500_REG_GWADDR, (addr), W5500_IP_LEN)
#define W5500_SetSubnetAddr(addr)       W5500_WriteBuffer(W5500_REG_SUBADDR, (addr), W5500_IP_LEN)
#define W5500_GetSubnetAddr(addr)       W5500_ReadBuffer(W5500_REG_SUBADDR, (addr), W5500_IP_LEN)
#define W5500_SetLocalIpAddr(addr)      W5500_WriteBuffer(W5500_REG_LIPADDR, (addr), W5500_IP_LEN)
#define W5500_GetLocalIpAddr(addr)      W5500_ReadBuffer(W5500_REG_LIPADDR, (addr), W5500_IP_LEN)
#define W5500_SetMacAddr(addr)          W5500_WriteBuffer(W5500_REG_MACADDR, (addr), W5500_MAC_LEN)
#define W5500_GetMacAddr(addr)          W5500_ReadBuffer(W5500_REG_MACADDR, (addr), W5500_MAC_LEN)

#define W5500_SetRetransTime(to)        W5500_WriteData(W5500_REG_RT, (to), 2)
#define W5500_SetRetransNum(n)          W5500_WriteReg(W5500_REG_RC, (n))
#define W5500_GetIntFlag()              W5500_ReadReg(W5500_REG_INT)
#define W5500_ClearIntFlag(m)           W5500_WriteReg(W5500_REG_INT, (~(m)) & (W5500_GetIntFlag()))



void W5500_SocketMallocBuffer(void);                        // setting tx/rx buf size
#define W5500_SocketSetSegSize(s, size) W5500_WriteData(W5500_SktMssReg((s)), (size), 2)
#define W5500_SocketSetIpLiveTime(s, t) W5500_WriteReg(W5500_SktTtlReg((s)) , (t))  
#define W5500_SocketGetIntFlag(s)       W5500_ReadReg(W5500_SktIntReg((s)))  
#define W5500_SocketClearIntFlag(s, v)  W5500_WriteReg(W5500_SktIntReg((s)), (v)) 
#define W5500_SocketGetStatus(s)        W5500_ReadReg(W5500_SktStatReg((s))) 
#define W5500_SocketSetCommand(s, c)    W5500_WriteReg(W5500_SktCmdReg((s)), (c))
#define W5500_SocketGetCommand(s)       W5500_ReadReg(W5500_SktCmdReg((s)))


u16 W5500_SocketGetFreeTxBufSize(u8 s); // get socket TX free buf size
u16 W5500_SocketGetRxBufSize(u8 s); // get socket RX recv buf size
void W5500_SocketWriteBuffer(u8 s, u8 *pBuffer, u16 len);
void W5500_SocketReadBuffer(u8 s, u8 *pBuffer, u16 len);

u8 W5500_SocketOpen(u8 s, u8 protocol, u16 port, u8 flag);
void W5500_SocketClose(u8 s);
u8 W5500_SocketListen(u8 s);
u8 W5500_SocketConnect(u8 s, u8 *addr, u16 port);
void W5500_SocketDisconnect(u8 s);
u16 W5500_SocketTransmit(u8 s, u8 *pBuf, u16 len);
u16 W5500_SocketReceive(u8 s, u8 *pBuf, u16 len);
u16 W5500_SocketTransmitTo(u8 s, u8 *pBuf, u16 len, u8 *addr, u16 port);
u16 W5500_SocketReceiveFrome(u8 s, u8 *pBuf, u16 len, u8 *addr, u16 *port);

#endif

