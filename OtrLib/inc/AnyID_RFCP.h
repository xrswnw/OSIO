#ifndef _ANYID_RFCP_
#define _ANYID_RFCP_

#include "AnyID_Config.h"

#define RFCP_RX_BUF_LEN              32
#define RFCP_TX_BUF_LEN              32

#define RFCP_FRAME_LEN_POS           0   //帧长度位置
#define RFCP_FRAME_ADDR_POS          1   //帧地址
#define RFCP_FRAME_CMD_POS           2   //帧命令
#define RFCP_FRAME_RSP_POS           3
#define RFCP_FRAME_CTRL_POS          3
#define RFCP_FRAME_PAR_POS           4

#define RFCP_FRAME_DFT_ADDR          0x00    //默认地址

#define RFCP_FRAME_RSP_FLAG          0x80    //响应帧标识

#define RFCP_FRAME_RQST_RSP          0x01    //请求接收机对该帧给予响应
#define RFCP_FRAME_NO_RSP            0x00    //接收机不需要对该帧进行响应

//CRC校验值
#define RFCP_FRAME_CRC_MASK          0x80
#define RFCP_FRAME_CRC_LEN           2


u8 RFCP_RequestFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 reqFlag, u8 *pPar, u8 parLen);
u8 RFCP_ReponseFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 *pPar, u8 parLen);

#if (defined(_ANYID_STM32_MTD_) | defined(_ANYID_STM32_MID_) | defined(_ANYID_STM32_MPD_) | defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MCD_))

    //命令
    #define RFCP_FRAME_CMD_SIGHTID       0x01    //景点标识
    #define RFCP_FRAME_CMD_ALARM         0x02    //报警点标识
    #define RFCP_FRAME_CMD_OFF           0x03    //关机
    #define RFCP_FRAME_CMD_LIC           0x04    //发放授权
    #define RFCP_FRAME_CMD_RQTLIC        0x05    //请求授权
    #define RFCP_FRAME_CMD_TESTLED       0x06    //测试显示板LED
    #define RFCP_FRAME_CMD_GET_VISION    0x07    //获取软件版本号
    #define RFCP_FRAME_CMD_GET_CPUID     0x08    //获取CPUID



    //帧长度
    #define RFCP_FRAME_LEN_SIGHTID       0x08    //景点标识
    #define RFCP_FRAME_LEN_ALARM         0x03    //报警点标识
    #define RFCP_FRAME_LEN_OFF           0x03    //关机

    #define RFCP_FRAME_LEN_LIC           0x04    //发放授权
    #define RFCP_RFRAME_LEN_LIC          0x11    //发放授权

    #define RFCP_FRAME_LEN_RQTLIC        0x13    //请求授权
    #define RFCP_RFRAME_LEN_RQTLIC       0x13    //请求授权

    #define RFCP_FRAME_LEN_TESTLED       0x03    //测试显示板LED

    #define RFCP_FRAME_LEN_GET_VISION    0x03    //获取软件版本号
    #define RFCP_RFRAME_LEN_GET_VISION   0x07    //获取软件版本号

    #define RFCP_FRAME_LEN_GET_CPUID     0x03    //获取CPUID
    #define RFCP_RFRAME_LEN_GET_CPUID    0x0F    //获取CPUID



    //景点标识帧格式
    #define RFCP_FRAME_SIGHTID_ID_POS    RFCP_FRAME_PAR_POS    	//ID位置
    #define RFCP_FRAME_SIGHTID_ID_LEN    0x02    					//ID长度
    #define RFCP_FRAME_SIGHTID_VOL_POS   (RFCP_FRAME_SIGHTID_ID_POS + RFCP_FRAME_SIGHTID_ID_LEN)
    #define RFCP_FRAME_SIGHTID_VOL_LEN   0x02
    #define RFCP_FRAME_SIGHTID_POW_POS   (RFCP_FRAME_SIGHTID_VOL_POS + RFCP_FRAME_SIGHTID_VOL_LEN)
    #define RFCP_FRAME_SIGHTID_POW_LEN   0x01


    //授权帧格式
    #define RFCP_FRAME_LIC_LG_POS        RFCP_FRAME_PAR_POS
    #define RFCP_FRAME_LIC_LG_LEN        1
    #define RFCP_FRAME_LIC_LG_MAX        0x06
    #define RFCP_FRAME_LIC_LG_DFT        0
    //授权帧响应帧格式
    #define RFCP_RFRAME_LIC_CPUID_POS    RFCP_FRAME_PAR_POS
    #define RFCP_RFRAME_LIC_CPUID_LEN    12
    #define RFCP_RFRAME_LIC_MTDVOL_POS   (RFCP_RFRAME_LIC_CPUID_POS + RFCP_RFRAME_LIC_CPUID_LEN)
    #define RFCP_RFRAME_LIC_MTDVOL_LEN   2
    #define RFCP_RFRAME_LIC_MTDVOL_MASK  0x1FFF


    //请求授权帧格式
    #define RFCP_FRAME_RQTLIC_CPUID_POS  RFCP_FRAME_PAR_POS
    #define RFCP_FRAME_RQTLIC_CPUID_LEN  12
    #define RFCP_FRAME_RQTLIC_LICU_POS   (RFCP_FRAME_RQTLIC_CPUID_POS + RFCP_FRAME_RQTLIC_CPUID_LEN)
    #define RFCP_FRAME_RQTLIC_LICU_LEN   2
    #define RFCP_FRAME_RQTLIC_PWD_POS    (RFCP_FRAME_RQTLIC_LICU_POS + RFCP_FRAME_RQTLIC_LICU_LEN)
    #define RFCP_FRAME_RQTLIC_PWD_LEN    2
    //请求授权帧响应帧格式
    #define RFCP_RFRAME_RQTLIC_CPUID_POS RFCP_FRAME_PAR_POS
    #define RFCP_RFRAME_RQTLIC_CPUID_LEN 12
    #define RFCP_RFRAME_RQTLIC_LIC_POS   (RFCP_RFRAME_RQTLIC_CPUID_POS + RFCP_RFRAME_RQTLIC_CPUID_LEN)
    #define RFCP_RFRAME_RQTLIC_LIC_LEN   2
    #define RFCP_RFRAME_RQTLIC_PWD_POS   (RFCP_RFRAME_RQTLIC_LIC_POS + RFCP_RFRAME_RQTLIC_LIC_LEN)
    #define RFCP_RFRAME_RQTLIC_PWD_LEN   2


    //获取版本号帧响应帧格式
    #define RFCP_RFRAME_VISION_TYPE_POS  RFCP_FRAME_PAR_POS
    #define RFCP_RFRAME_VISION_TYPE_LEN  1
    #define RFCP_RFRAME_VISION_VXX_POS   (RFCP_RFRAME_VISION_TYPE_POS + RFCP_RFRAME_VISION_TYPE_LEN)
    #define RFCP_RFRAME_VISION_VXX_LEN   1
    #define RFCP_RFRAME_VISION_VYY_POS   (RFCP_RFRAME_VISION_VXX_POS + RFCP_RFRAME_VISION_VXX_LEN)
    #define RFCP_RFRAME_VISION_VYY_LEN   1
    #define RFCP_RFRAME_VISION_VZZ_POS   (RFCP_RFRAME_VISION_VYY_POS + RFCP_RFRAME_VISION_VYY_LEN)
    #define RFCP_RFRAME_VISION_VZZ_LEN   1

    //获取CPUID帧响应帧格式
    #define RFCP_RFRAME_GETCPUID_POS     RFCP_FRAME_PAR_POS
    #define RFCP_RFRAME_GETCPUID_LEN     12





    u8 RFCP_SightIDFrame(u8 *pFrame, u16 sightID, u16 voltage, u8 power);

    u8 RFCP_AlarmFrame(u8 *pFrame);

    u8 RFCP_OFFFrame(u8 *pFrame);

    u8 RFCP_LicenceFrame(u8 *pFrame, u8 lg, u8 isRsq);
    u8 RFCP_LicenceRspFrame(u8 *pFrame, u8 *pCPUID, u16 voltage);

    u8 RFCP_RequestLicenceFrame(u8 *pFrame, u8 *pCPUID, u16 licence, u16 pwd);
    u8 RFCP_RequestLicenceRspFrame(u8 *pFrame, u8 *pCPUID, u16 licence, u16 pwd);

    u8 RFCP_TestLEDFrame(u8 *pFrame);

    u8 RFCP_GetVisionFrame(u8 *pFrame);
    u8 RFCP_VisionRsqFrame(u8 *pFrame, u8 visionType, u8 xx, u8 yy, u8 zz);

    u8 RFCP_GetCPUIDFrame(u8 *pFrame);
    u8 RFCP_CPUIDRsqFrame(u8 *pFrame, u8 *pCPUUID);
    BOOL C(u8 *pFrame, u8 len);

/*#elif (defined(_ANYID_PIC18_BWCOM_))

BOOL RFCP_CheckRxFrame(u8 *pFrame, u8 len);

u8 RFCP_MasteFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 reqFlag, u8 *pPar, u8 parLen, u32 index);
u8 RFCP_SlaveFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 *pPar, u8 parLen, u32 index);
u32 RFCP_GetFrameIndex(u8 *pFrame, u8 len);
*/
#endif



#endif
