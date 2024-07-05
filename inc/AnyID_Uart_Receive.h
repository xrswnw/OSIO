#ifndef _ANYID_UART_RECEIVE_
#define _ANYID_UART_RECEIVE_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"

#define UART_COM_LED_TIM                6

#define UART_FRAME_CMD_HARD_INFO        0x00
#define UART_FRAME_CMD_IDLE             0x00
#define UART_FRAME_CMD_RST              0x04
#define UART_FRAME_CMD_VER              0xF7
#define UART_FRAME_CMD_GETCPUID         0xF8


#define UART_FRAME_CMD_SET_PARAMS       0x45
#define UART_FRAME_CMD_GET_PARAMS       0x46
#define UART_FRAME_CMD_SET_OUTINFO      0x47
#define UART_FRAME_CMD_GET_DEVINFO      0x48
#define UART_FRAME_CMD_SET_MOT          0x49


#define UART_FRAME_POS_HEAD1            0
#define UART_FRAME_POS_HEAD2            1
#define UART_FRAME_POS_LEN              2
#define UART_FRAME_POS_SRCADDR          3
#define UART_FRAME_POS_DESTADDR         5
#define UART_FRAME_POS_CMD              7
#define UART_FRAME_POS_RFU              8
#define UART_FRAME_POS_PAR              9

#define UART_FRAME_BROADCAST_ADDR       0xFFFF
#define UART_FRAME_MIN_LEN              11

#define UART_FRAME_FLAG_HEAD1           0x7E
#define UART_FRAME_FLAG_HEAD2           0x55

#define UART_FRAME_RESPONSE_FLAG        0x1F
#define UART_FRAME_PARAM_RFU            0x00

#define UART_FRAME_FLAG_OK              0x00
#define UART_FRAME_FLAG_FAIL            0x01
#define UART_FRAME_RSP_NOERR            0x00
#define UART_FRAME_RSP_TAGERR           0x01
#define UART_FRAME_RSP_CRCERR           0x02
#define UART_FRAME_RSP_NORSP            0x03
#define UART_FRAME_RSP_PARERR           0x04

#define UART_DATA_MAX_LEN               512
#define UART_BUFFER_MAX_LEN             (UART_DATA_MAX_LEN + 32) 

#define UART_RX_TICK_TO                 3
#define UART_STAT_RX_END                0x02
#define UART_STAT_RX_OVR                0x01
#define UART_STAT_RX_ING                0x80
#define UART_STAT_RX_IDLE               0x00
typedef struct uartRcvFrame{
    u8 state;
    u16 len;
    u8 buffer[UART_BUFFER_MAX_LEN];
}UART_RCVFRAME;
extern UART_RCVFRAME g_sUartTempRcvFrame;

#define UART_STAT_TX_IDLE               0
#define UART_STAT_TX_BUSY               1
#define UART_TX_TIMEOUT                 100   //200//
typedef struct uartTxFrame{
    u32 tick;
    u8 state;
    u16 len;
    u8 buffer[UART_BUFFER_MAX_LEN];
}UART_TXFRAME;

#define Uart_IsRcvFrame(rcvFrame)			((rcvFrame).state == UART_STAT_RX_END || (rcvFrame).state == UART_STAT_RX_OVR)
#define Uart_ResetFrame(rcvFrame)           ((rcvFrame).state = UART_STAT_RX_IDLE, (rcvFrame).len = 0)

#define UART_USR_ADDR_BROADCAST                     0xFFFF
BOOL Uart_UsrCheckFrame(UART_RCVFRAME *pRcvFrame, u16 *pStartPos, u16 addr);
u16 Uart_UsrResponseFrame(u8 *pParam, u8 len, u8 cmd, u16 addr, u8 *pBuffer);
u16 Uart_UsrRspErrorFrame(u8 errCode, u8 cmd, u16 addr, u8 *pBuffer);
u16 Uart_UsrRspLongFrame(u16 *pParams, u16 regNum, u8 errCode, u8 cmd, u16 addr, u8 *pBuffer);

#endif

