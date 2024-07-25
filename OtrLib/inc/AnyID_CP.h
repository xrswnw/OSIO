#ifndef _ANYID_CP_
#define _ANYID_CP_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"


//帧接收缓冲区长度
#if (defined(_ANYID_STM32_) | defined(_ANYID_LM3S_) || defined(_ANYID_GD32_) )
    #define CP_FRAME_BUF_MAX_LEN                250
#elif (defined(_ANYID_PIC18_) || defined(_ANYID_STM8_))
    #define CP_FRAME_BUF_MAX_LEN                101
#endif
extern u8 g_aCPRcvBuffer[CP_FRAME_BUF_MAX_LEN];



extern const u8 CP_Version[];
#define CP_GetVersion(pVersion, len)        (pVersion = CP_Version, len = sizeof(CP_Version))



//存入接收字节的位置
extern u16 g_nCPFrameRcvPos;
#define CP_GetFrameLen()                    (g_nCPFrameRcvPos)
#define CP_SetFrameRcvPos(pos)              (g_nCPFrameRcvPos = pos)

//同一帧的各个字节之间的间隔时间在50~100ms，超过100ms表示一帧数据完成
extern u16 g_nCPFrameIdleTime;
//接收标识
extern u8 g_nCPFrameRcvFlag;
//帧接收标识
#define CP_FRAME_FLAG_IDLE                  0x00
#define CP_FRAME_FLAG_RCV                   0x01
#define CP_FRAME_FLAG_COPY                  0x02


//如果处于接收状态，就增加字节接收间隔时间
//在主循环中检测字节接收间隔时间g_nCPFrameIdleTime，如果g_nCPFrameIdleTime > CP_FRAME_TIME，表示"帧"接收完成
#define CP_IncFrameTime(t)                  do{                                                    \
                                                if(g_nCPFrameRcvFlag == CP_FRAME_FLAG_RCV)          \
                                                {                                                   \
                                                    g_nCPFrameIdleTime += (t);                      \
                                                }                                                   \
                                             }while(0)
#define CP_ClearFrameTime()                 (g_nCPFrameIdleTime = 0)



//接收帧状态
#define CP_SetRcvFlag(flag)                 (g_nCPFrameRcvFlag = flag)
#define CP_GetRcvFlag()                     (g_nCPFrameRcvFlag)

//该帧是否接收完成:CP_FRAME_TIME时间内没有新数据到来，并且以往有数据
#define CP_IsRcvFrame(t)                     ((g_nCPFrameRcvFlag == CP_FRAME_FLAG_RCV) && (g_nCPFrameIdleTime >= (t)))
#define CP_IsIdleState(t)                    (((g_nCPFrameRcvFlag & CP_FRAME_FLAG_RCV) != CP_FRAME_FLAG_RCV) || (g_nCPFrameIdleTime > (t)))


//如果队列满，就不保存数据
//如果数据正被主循环COPY，就不保存数据
//数据进入队列，清接收间隔时间，设置接收标志
#define CP_ReceiveByte(byte)                do{                                                    \
                                                if((g_nCPFrameRcvPos < CP_FRAME_BUF_MAX_LEN) &&     \
                                                   (g_nCPFrameRcvFlag != CP_FRAME_FLAG_COPY))       \
                                                {                                                   \
                                                    g_aCPRcvBuffer[g_nCPFrameRcvPos++] = byte;      \
                                                    g_nCPFrameIdleTime = 0;                         \
                                                    g_nCPFrameRcvFlag = CP_FRAME_FLAG_RCV;          \
                                                }                                                   \
                                             }while(0)


//帧基本格式
//帧头、帧长度、帧命令和帧参数在数据帧中的位置
#define CP_FRAME_HEAD1_POS                  0
#define CP_FRAME_HEAD2_POS                  1
#define CP_FRAME_SCRADDR_POS                2
#define CP_FRAME_DESADDR_POS                4
#define CP_FRAME_CTRL_POS                   6
#define CP_FRAME_CMD_POS                    7
#define CP_FRAME_STATE_POS                  8
#define CP_FRAME_DLEN_POS                   9
#define CP_FRAME_DSTART_POS                 10

#define CP_FRAME_ADDR_LEN                   2
#define CP_FRAME_CRC_LEN                    2

#define CP_FRAME_HEAD1_FLAG                 0x7E    //帧头1
#define CP_FRAME_HEAD2_FLAG                 0x55    //帧头2
#define CP_FRAME_TRAIL_FLAG                 0xAA    //帧尾

#define CP_FRAME_BROADCAST_ADDR             0xFFFF  //广播地址

//控制码
#define CP_FRAME_CTRL_REQUEST               0x80    //请求帧
#define CP_FRAME_CTRL_RESPONSE              0x00    //应答帧
#define CP_FRAME_CTRL_NEXT                  0x20    //还有后续帧
#define CP_FRAME_CTRL_OVER                  0x00    //没有后续帧


//帧状态，该参数暂时只对应答帧有意义
#define CP_FRAME_STATE_RESPONE_OK           0x00
#define CP_FRAME_STATE_RESPONE_FAIL         0x01

#define CP_FRAME_STATE_REQUEST              0x00    //请求帧时，该参数全零，无意义

//
#define CP_FRAME_NO_DATA                    0x00    //无数据域



//帧头结构
typedef struct cp_frame_head{
    union{                  //帧头
        u8 head[2];
        struct{
            u8 head1;
            u8 head2;
        }sHead;
    }uHead;
    union{                  //源地址
        u8 scrAddr[2];
        struct{
            u8 scrAddrLo;
            u8 scrAddrHi;
        }sScrAddr;
    }uScrAddr;
    union{                  //目标地址
        u8 desAddr[2];
        struct{
            u8 desAddrLo;
            u8 desAddrHi;
        }sDesAddr;
    }uDesAddr;
    u8 ctrl;                //控制码
    u8 cmd;                 //命令吗
    u8 state;               //状态码
    u8 dataLen;             //数据长度
} CP_FRAME_HEAD;

//帧尾结构
typedef struct cp_frame_trail{
    union{                  //CRC码
        u8 crc[2];
        struct{
            u8 crcLo;
            u8 crcHi;
        }sCrc;
    }uCrc;
    u8 trail;               //帧尾
} CP_FRAME_TRAIL;

//数据域最大长度
#define CP_FRAME_DATA_MAX_LEN                       200
#define CP_FRAME_HEAD_LEN                           (sizeof(CP_FRAME_HEAD))
#define CP_FRAME_TRAIL_LEN                          (sizeof(CP_FRAME_TRAIL))


#define CP_SetScrAddr(pHead, addr)                  (*((u16 *)((pHead)->uScrAddr.scrAddr)) = (addr))
#define CP_SetDesAddr(pHead, addr)                  (*((u16 *)((pHead)->uDesAddr.desAddr)) = (addr))
#define CP_SetCrc(pTrail, crc)                      (*((u16 *)((pTrail)->uCrc.crc)) = (crc))
#define CP_GetScrAddr(pHead)                        (*((u16 *)((pHead)->uScrAddr.scrAddr)))
#define CP_GetDesAddr(pHead)                        (*((u16 *)((pHead)->uDesAddr.desAddr)))



BOOL CP_CheckCrc(u8 *pFrame, u8 len);
BOOL CP_CheckFrame(u8 *pInFrame, u16 inLen, u8 **pOutFrame, u16 *pOutLen);
void CP_Init(u16 scrAddr, u8 ctrl);
u8 CP_GetFrame(u16 scrAddr, u16 desAddr, u8 ctrl, u8 cmd, u8 state, u8 *pFrame, u8 dataLen);

void CP_Task(void (*process)(u8 *pFrame, u16 len), u8 delayms);

#if 0
	//测试专用
	#define CP_FRAME_CMD_TEST               0xFF
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//通信接口
	#define CP_TEST_ITME_UART0              0x00    //串口
	#define CP_TEST_ITME_UART1              0x01
	#define CP_TEST_ITME_UART2              0x02
	#define CP_TEST_ITME_UART3              0x03
	#define CP_TEST_ITME_UART4              0x04
	#define CP_TEST_ITME_NET0               0x05    //网络
	#define CP_TEST_ITME_NET1               0x06
	#define CP_TEST_ITME_NET2               0x07
	
	//存储器
	#define CP_TEST_ITME_FLASH              0x10    //外部FLASH
	#define CP_TEST_ITME_EEPROM             0x11    //eeprom
	#define CP_TEST_ITME_FRAM               0x12    //铁电
	
	//板级IO
	#define CP_TEST_ITME_LED                0x20    //LED
	#define CP_TEST_ITME_BEEP               0x21    //蜂鸣器
	#define CP_TEST_ITME_RELAY              0x22    //继电器
	
	//外设
	#define CP_TEST_ITME_CC2500             0x30    //CC2500测试
	#define CP_TEST_ITME_MP3                0x31    //Mp3测试
	#define CP_TEST_ITME_HFREADER           0x32    //HFReader测试
	#define CP_TEST_ITME_UHFREADER          0x32    //UHFReader测试
	#define CP_TEST_ITME_GPS                0x33    //GPS测试
	#define CP_TEST_ITME_GPRS               0x34    //GPRS测试
	#define CP_TEST_ITME_BLDC               0x35    //电机测试
	#define CP_TEST_ITME_LCD                0x36    //LCD测试





//请求帧
u8 CP_SysTestReqFrame(u8 *pFrame, u8 testItem);

#define CP_TEST_RESULT_OK               0x00
#define CP_TEST_RESULT_FAIL             0x01
//响应帧
u8 CP_SysTestRspFrame(u8 *pFrame, u8 result);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

//这里是2.4G通信协议
#define CP_RF_FRAME_LEN_POS           0x00
#define CP_RF_FRAME_ADR_POS           0x01
#define CP_RF_FRAME_CMD_POS           0x02
#define CP_RF_FRAME_DAT_POS           0x03

#endif
