#ifndef _ANYID_COMCP_
#define _ANYID_COMCP_

#include "AnyID_Config.h"
///////////////////////////////////////////////////////////////////////////////
extern u8 g_nCOMCPFrameStart;                       //帧在Buffer中的起始位置
#define COMCP_SetFrameStart(len)            (g_nCOMCPFrameStart = len)
#define COMCP_GetFrameStart()               g_nCOMCPFrameStart

extern u32 g_nCOMCPRxOverTime;
#define COMCP_RX_OVERTIME             20
#define COMCP_IsRxOverTime()          (g_nCOMCPRxOverTime > COMCP_RX_OVERTIME)

///////////////////////////////////////////////////////////////////////////////
extern u8 g_nCOMCPState;                                //接收状态
#define COMCP_RXSTAT_IDLE                     0x80      //空闲状态
#define COMCP_RXSTAT_HEAD1                    0x01      //接收到帧头1:0x7E
#define COMCP_RXSTAT_HEAD2                    0x02      //接收到帧头2:0x55
#define COMCP_RXSTAT_LEN                      0x04      //接收到帧长度
#define COMCP_RXSTAT_TRAIL                    0x08      //接收到帧尾:0xAA
#define COMCP_RXSTAT_START                    0x10



/*
#define COMCP_SetState(state)               (g_nCOMCPState = state)
#define COMCP_CheckState(state)             ((g_nCOMCPState & state) == state)
*/

///////////////////////////////////////////////////////////////////////////////
#define COMCP_FRAME_HEAD1                   0x7E    //帧头1
#define COMCP_FRAME_HEAD2                   0x55    //帧头2
#define COMCP_FRAME_TRAIL                   0xAA    //帧尾
#define COMCP_FRAME_HEAD_LEN                2       //帧头长度:2字节


////////////////////////////////////////////////////////////////////////////////////
//请求帧基本格式
//帧头、帧长度、帧命令和帧参数在数据帧中的位置
#define COMCP_FRAME_HEAD1_POS               0
#define COMCP_FRAME_HEAD2_POS               1
#define COMCP_FRAME_LEN_POS                 2
#define COMCP_FRAME_CMD_POS                 3
#define COMCP_FRAME_PAR_POS                 4

////////////////////////////////////////////////////////////////////////////////////
//响应帧基本格式
#define COMCP_RFRAME_HEAD1_POS              0
#define COMCP_RFRAME_HEAD2_POS              1
#define COMCP_RFRAME_LEN_POS                2
#define COMCP_RFRAME_CMD_POS                3
#define COMCP_RFRAME_RSPF_POS               4       //响应标识
#define COMCP_RFRAME_OKF_POS                5       //是否成功标识: 00 失败， 01 成功
#define COMCP_RFRAME_ERRC_POS               6       //错误码
#define COMCP_RFRAME_PAR_POS                6       //响应帧成功参数数据起始位置
#define COMCP_RFRAME_FPAR_POS               7       //响应帧错误参数数据起始位置


#define COMCP_RFRAME_OK_FLAG                0x01    //成功响应帧标识
#define COMCP_RFRAME_FAIL_FLAG              0x00    //错误响应帧标识


////////////////////////////////////////////////////////////////////////////////////
//错误响应帧格式:所有的错误响应帧的格式都是一样的
#define COMCP_RFRAME_FFLEN                  6       //帧长度
#define COMCP_RFRAME_RSP_FLAG               0x1F    //响应帧标志
#define COMCP_RFRAME_FBCC_POS               (COMCP_RFRAME_ERRC_POS + 1)
#define COMCP_RFRAME_FTRL_POS               (COMCP_RFRAME_FBCC_POS + 1)

//无参数的正确响应帧格式:所有的无参数的正确响应帧的格式都是一样的
#define COMCP_RFRAME_OFLEN                  5       //帧长度
#define COMCP_RFRAME_RSP_FLAG               0x1F    //响应帧标志
#define COMCP_RFRAME_OBCC_POS               (COMCP_RFRAME_OKF_POS + 1)
#define COMCP_RFRAME_OTRL_POS               (COMCP_RFRAME_OBCC_POS + 1)

//无参数的请求帧
#define COMCP_FRAME_NOPAR_RQST_LEN          3
#define COMCP_FRAME_NOPAR_RQST_BCC_POS      (COMCP_FRAME_PAR_POS)
#define COMCP_FRAME_NOPAR_RQST_TRL_POS      (COMCP_FRAME_NOPAR_RQST_BCC_POS + 1)



u8 COMCP_GetCheckBCC(u8 *pBuffer, u8 len);
BOOL COMCP_CheckFrame(u8 *pFrame);
u8 COMCP_ErrorRspFrame(u8 *pFrame, u8 cmd, u8 errorCode);
u8 COMCP_OkRspNoParFrame(u8 *pFrame, u8 cmd);
u8 COMCP_NoParRequestFrame(u8 *pFrame, u8 cmd);


///////////////////////////////////////////////////////////////////////////////
#if (defined(_ANYID_STM32_MMD_))

    //PC到MMD写授权信息，MMD到PC的响应授权帧格式
    #define COMCP_FRAME_LICINF_NUMBER           100
    #define COMCP_FRAME_LICINF_LEN              22


///////////////////////////////////////////////////////////////////////////////////
    //授权信息格式
    #define COMCP_LIC_CPUID_POS                 0
    #define COMCP_LIC_CPUID_LEN                 12
    //该次MMD配置该MTD的授权次数
    #define COMCP_LIC_CUR_LIC_POS               (COMCP_LIC_CPUID_POS + COMCP_LIC_CPUID_LEN)
    #define COMCP_LIC_CUR_LIC_LEN               2
    //该MMD总共给MTD配置授权的次数
    #define COMCP_LIC_TOTAL_LIC_POS             (COMCP_LIC_CUR_LIC_POS + COMCP_LIC_CUR_LIC_LEN)
    #define COMCP_LIC_TOTAL_LIC_LEN             2
    //该MTD配置了N台导游机
    #define COMCP_LIC_CUR_CFGT_POS              (COMCP_LIC_TOTAL_LIC_POS + COMCP_LIC_TOTAL_LIC_LEN)
    #define COMCP_LIC_CUR_CFGT_LEN              2
    //该MTD总共配置了N台导游机
    #define COMCP_LIC_TOTAL_CFGT_POS            (COMCP_LIC_CUR_CFGT_POS + COMCP_LIC_CUR_CFGT_LEN)
    #define COMCP_LIC_TOTAL_CFGT_LEN            2
    //该授权信息存储的序号
    #define COMCP_LIC_ID_POS                    (COMCP_LIC_TOTAL_CFGT_POS + COMCP_LIC_TOTAL_CFGT_LEN)
    #define COMCP_LIC_ID_LEN                    1
    //该授权信息存储空间状态信息
    #define COMCP_LIC_STAT_POS                  (COMCP_LIC_ID_POS + COMCP_LIC_ID_LEN)
    #define COMCP_LIC_STAT_LEN                  1

    #define COMCP_LIC_STAT_IDLE                 0x00
    #define COMCP_LIC_STAT_USED                 0x01


///////////////////////////////////////////////////////////////////////////////////
    //读MMD的授权信息请求帧
    #define COMCP_FRAME_CMD_RLIC                0x01
    #define COMCP_FRAME_RLIC_LEN                0x0F
    #define COMCP_FRAME_RLIC_CPUID_POS          COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_RLIC_CPUID_LEN          12
    #define COMCP_FRAME_RLIC_BCC_POS           (COMCP_FRAME_RLIC_CPUID_POS + COMCP_FRAME_RLIC_CPUID_LEN)
    #define COMCP_FRAME_RLIC_BCC_LEN           1
    #define COMCP_FRAME_RLIC_TRL_POS            (COMCP_FRAME_RLIC_BCC_POS + COMCP_FRAME_RLIC_BCC_LEN)


    //读MMD的授权信息响应帧
    #define COMCP_RFRAME_RLIC_OLEN              0x1B        //操作OK时长度
    #define COMCP_RFRAME_RLIC_OINF_POS          COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_RLIC_OINF_LEN          COMCP_FRAME_LICINF_LEN
    #define COMCP_RFRAME_RLIC_OBCC_POS         (COMCP_RFRAME_RLIC_OINF_POS + COMCP_RFRAME_RLIC_OINF_LEN)
    #define COMCP_RFRAME_RLIC_OBCC_LEN         1
    #define COMCP_RFRAME_RLIC_OTRL_POS          (COMCP_RFRAME_RLIC_OBCC_POS + COMCP_RFRAME_RLIC_OBCC_LEN)


///////////////////////////////////////////////////////////////////////////////////
    //写MMD的授权信息请求帧
    #define COMCP_FRAME_CMD_WLIC                0x02
    #define COMCP_FRAME_WLIC_LEN                0x19
    #define COMCP_FRAME_WLIC_INF_POS            COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_WLIC_INF_LEN            COMCP_FRAME_LICINF_LEN
    #define COMCP_FRAME_WLIC_BCC_POS            (COMCP_FRAME_WLIC_INF_POS + COMCP_FRAME_WLIC_INF_LEN)
    #define COMCP_FRAME_WLIC_BCC_LEN            1
    #define COMCP_FRAME_WLIC_TRL_POS            (COMCP_FRAME_WLIC_BCC_POS + COMCP_FRAME_WLIC_BCC_LEN)


    //写MMD的授权信息响应帧
    #define COMCP_RFRAME_WLIC_OLEN              0x05        //操作OK时长度
    #define COMCP_RFRAME_WLIC_OPAR_POS          COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_WLIC_OPAR_LEN          0
    #define COMCP_RFRAME_WLIC_OBCC_POS          (COMCP_RFRAME_WLIC_OPAR_POS + COMCP_RFRAME_WLIC_OPAR_LEN)
    #define COMCP_RFRAME_WLIC_OBCC_LEN          1
    #define COMCP_RFRAME_WLIC_OTRL_POS          (COMCP_RFRAME_WLIC_OBCC_POS + COMCP_RFRAME_WLIC_OBCC_LEN)



///////////////////////////////////////////////////////////////////////////////
    //格式化管理器请求帧
    #define COMCP_FRAME_CMD_FMT                 0x03

    #define COMCP_FRAME_FMT_LEN                 0x04
    #define COMCP_FRAME_FMT_ADDR_POS            COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_FMT_ADDR_LEN            1
    #define COMCP_FRAME_FMT_BCC_POS             (COMCP_FRAME_FMT_ADDR_POS + COMCP_FRAME_FMT_ADDR_LEN)
    #define COMCP_FRAME_FMT_BCC_LEN             1
    #define COMCP_FRAME_FMT_TRL_POS             (COMCP_FRAME_FMT_BCC_POS + COMCP_FRAME_FMT_BCC_LEN)
    #define COMCP_FRAME_FMT_ALL                 0x00

    //格式化管理器响应帧
    #define COMCP_RFRAME_FMT_OLEN               0x05        //操作OK时长度
    #define COMCP_RFRAME_FMT_OPAR_POS           COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_FMT_OPAR_LEN           0
    #define COMCP_RFRAME_FMT_OBCC_POS           (COMCP_RFRAME_FMT_OPAR_POS + COMCP_RFRAME_FMT_OPAR_LEN)
    #define COMCP_RFRAME_FMT_OBCC_LEN           1
    #define COMCP_RFRAME_FMT_OTRL_POS           (COMCP_RFRAME_FMT_OBCC_POS + COMCP_RFRAME_FMT_OBCC_LEN)



///////////////////////////////////////////////////////////////////////////////
    //读取授权信息序号对应的授权信息请求帧
    #define COMCP_FRAME_CMD_RLICID              0x04

    #define COMCP_FRAME_RLICID_LEN              0x04
    #define COMCP_FRAME_RLICID_LICID_POS        COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_RLICID_LICID_LEN        1
    #define COMCP_FRAME_RLICID_BCC_POS          (COMCP_FRAME_RLICID_LICID_POS + COMCP_FRAME_RLICID_LICID_LEN)
    #define COMCP_FRAME_RLICID_BCC_LEN          1
    #define COMCP_FRAME_RLICID_TRL_POS          (COMCP_FRAME_RLICID_BCC_POS + COMCP_FRAME_RLICID_BCC_LEN)


    //读取授权信息序号对应的授权信响应帧
    #define COMCP_RFRAME_RLICID_OLEN            0x1B        //操作OK时长度
    #define COMCP_RFRAME_RLICID_OINF_POS        COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_RLICID_OINF_LEN        COMCP_FRAME_LICINF_LEN
    #define COMCP_RFRAME_RLICID_OBCC_POS        (COMCP_RFRAME_RLICID_OINF_POS + COMCP_RFRAME_RLICID_OINF_LEN)
    #define COMCP_RFRAME_RLICID_OBCC_LEN        1
    #define COMCP_RFRAME_RLICID_OTRL_POS        (COMCP_RFRAME_RLICID_OBCC_POS + COMCP_RFRAME_RLICID_OBCC_LEN)

///////////////////////////////////////////////////////////////////////////////////
    //写授权信息序号对应的授权信息请求帧
    #define COMCP_FRAME_CMD_WLICID              0x05

    #define COMCP_FRAME_WLICID_LEN              0x1A
    #define COMCP_FRAME_WLICID_INF_POS          COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_WLICID_INF_LEN          COMCP_FRAME_LICINF_LEN
    #define COMCP_FRAME_WLICID_ID_POS           (COMCP_FRAME_PAR_POS + COMCP_FRAME_LICINF_LEN)
    #define COMCP_FRAME_WLICID_ID_LEN           1
    #define COMCP_FRAME_WLICID_BCC_POS          (COMCP_FRAME_WLICID_ID_POS + COMCP_FRAME_WLICID_ID_LEN)
    #define COMCP_FRAME_WLICID_BCC_LEN          1
    #define COMCP_FRAME_WLICID_TRL_POS          (COMCP_FRAME_WLICID_BCC_POS + COMCP_FRAME_WLICID_BCC_LEN)


    //写授权信息序号对应的授权信息响应帧
    #define COMCP_RFRAME_WLICID_OLEN            0x05        //操作OK时长度
    #define COMCP_RFRAME_WLICID_OPAR_POS        COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_WLICID_OPAR_LEN        0
    #define COMCP_RFRAME_WLICID_OBCC_POS        (COMCP_RFRAME_WLIC_OPAR_POS + COMCP_RFRAME_WLIC_OPAR_LEN)
    #define COMCP_RFRAME_WLICID_OBCC_LEN        1
    #define COMCP_RFRAME_WLICID_OTRL_POS        (COMCP_RFRAME_WLIC_OBCC_POS + COMCP_RFRAME_WLIC_OBCC_LEN)


///////////////////////////////////////////////////////////////////////////////////
/*
    u8 COMCP_WriteLICFrame(u8 *pFrame, u8 *pLicence);
    u8 COMCP_WriteLICIDFrame(u8 *pFrame, u8 licID, u8 *pLicence);
    u8 COMCP_FormatLICFrame(u8 *pFrame, u8 addr);
    u8 COMCP_ReadLICFrame(u8 *pFrame, u8 *pCPUID);
    u8 COMCP_ReadLICIDFrame(u8 *pFrame, u8 licID);
*/
    u8 COMCP_WriteLICRspFrame(u8 *pFrame);
    u8 COMCP_WriteLICIDRspFrame(u8 *pFrame);
    u8 COMCP_FormatLICRspFrame(u8 *pFrame);
    u8 COMCP_ReadLICIDRspFrame(u8 *pFrame, u8 *pLicence);
    u8 COMCP_ReadLICRspFrame(u8 *pFrame, u8 *pLicence);
    ///////////////////////////////////////////////////////////////////////////////
#elif defined(_ANYID_STM32_MPD_)

    //最多支持的MID个数
    #define COMCP_MIDINF_NUM                256
    #define COMCP_MIDINF_LEN                3



    //MID信息的存储格式
    #define COMCP_MIDINF_VOL_POS            0
    #define COMCP_MIDINF_VOL_LEN            2
    #define COMCP_MIDINF_POW_POS            (COMCP_MIDINF_VOL_POS + COMCP_MIDINF_VOL_LEN)
    #define COMCP_MIDINF_POW_LEN            1

    ///////////////////////////////////////////////////////////////////////////////////
    //读MID的信息请求帧
    #define COMCP_FRAME_CMD_READ                0x06
    #define COMCP_FRAME_READ_LEN                0x05
    #define COMCP_FRAME_READ_ID_POS             COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_READ_ID_LEN             2
    #define COMCP_FRAME_READ_BCC_POS            (COMCP_FRAME_READ_ID_POS + COMCP_FRAME_READ_ID_LEN)
    #define COMCP_FRAME_READ_BCC_LEN            1
    #define COMCP_FRAME_READ_TRL_POS            (COMCP_FRAME_READ_BCC_POS + COMCP_FRAME_READ_BCC_LEN)


    //读MID的信息响应帧
    #define COMCP_RFRAME_READ_OLEN              0x08        //操作OK时长度
    #define COMCP_RFRAME_READ_OINF_POS          COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_READ_OINF_LEN          COMCP_MIDINF_LEN
    #define COMCP_RFRAME_READ_OBCC_POS          (COMCP_RFRAME_READ_OINF_POS + COMCP_RFRAME_READ_OINF_LEN)
    #define COMCP_RFRAME_READ_OBCC_LEN          1
    #define COMCP_RFRAME_READ_OTRL_POS          (COMCP_RFRAME_READ_OBCC_POS + COMCP_RFRAME_READ_OBCC_LEN)


    ///////////////////////////////////////////////////////////////////////////////
    //格式化巡检机请求帧
    #define COMCP_FRAME_CMD_FMT                 0x07

    #define COMCP_FRAME_FMT_LEN                 0x05
    #define COMCP_FRAME_FMT_ADDR_POS            COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_FMT_ADDR_LEN            2
    #define COMCP_FRAME_FMT_BCC_POS             (COMCP_FRAME_FMT_ADDR_POS + COMCP_FRAME_FMT_ADDR_LEN)
    #define COMCP_FRAME_FMT_BCC_LEN             1
    #define COMCP_FRAME_FMT_TRL_POS             (COMCP_FRAME_FMT_BCC_POS + COMCP_FRAME_FMT_BCC_LEN)
    #define COMCP_FRAME_FMT_ALL                 0xFFFF

    //格式化巡检机响应帧
    #define COMCP_RFRAME_FMT_OLEN               0x05        //操作OK时长度
    #define COMCP_RFRAME_FMT_OPAR_POS           COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_FMT_OPAR_LEN           0
    #define COMCP_RFRAME_FMT_OBCC_POS           (COMCP_RFRAME_FMT_OPAR_POS + COMCP_RFRAME_FMT_OPAR_LEN)
    #define COMCP_RFRAME_FMT_OBCC_LEN           1
    #define COMCP_RFRAME_FMT_OTRL_POS           (COMCP_RFRAME_FMT_OBCC_POS + COMCP_RFRAME_FMT_OBCC_LEN)

    u8 COMCP_FormatMIDInfRspFrame(u8 *pFrame);
    u8 COMCP_ReadMIDInfRspFrame(u8 *pFrame, u8 *pInf);

#elif (defined(_ANYID_PIC18_HSR100_))

    /////////////////////////////////////////////////////////////////////////////////////
    //Mifare 卡操作参数
    #define COMCP_FRAME_PWD_LEN                 6       //密钥长度
    #define COMCP_FRAME_KEYT_A                  0x60    //KEYA
    #define COMCP_FRAME_KEYT_B                  0x61    //KEYB
    #define COMCP_FRAME_RSQT_MODE_IDLE          0x26    //请求模式:IDLE
    #define COMCP_FRAME_RSQT_MODE_ALL           0x52    //请求模式:ALL
    //电子钱包操作模式
    #define COMCP_FRAME_VALUE_MODE_DEC          0xC0    //减值
    #define COMCP_FRAME_VALUE_MODE_INC          0xC1    //增值
    #define COMCP_FRAME_VALUE_MODE_RSTOR        0xC2    //备份
    #define COMCP_FRAME_VALUE_MODE_TRANS        0xB0    //转存



    ///////////////////////////////////////////////////////////////////////////////
    //复位读写器命令
    #define COMCP_FRAME_RST_FLEN                3
    #define COMCP_FRAME_CMD_RST                 0x52
    #define COMCP_FRAME_RST_BCC_POS             COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_RST_TRL_POS             (COMCP_FRAME_RST_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_RST_OFLEN              5
    #define COMCP_RFRAME_RST_OBCC_POS           COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_RST_OTRL_POS           (COMCP_RFRAME_RST_OBCC_POS + 1)



    ///////////////////////////////////////////////////////////////////////////////
    //控制天线命令
    #define COMCP_FRAME_CANT_FLEN               4
    #define COMCP_FRAME_CMD_CANT                0x53
    #define COMCP_FRAME_CANT_STAT_POS           COMCP_FRAME_PAR_POS     //设置天线状态:开/关
    #define COMCP_FRAME_CANT_STAT_LEN           1
    #define COMCP_FRAME_CANT_BCC_POS            (COMCP_FRAME_CANT_STAT_POS + COMCP_FRAME_CANT_STAT_LEN)
    #define COMCP_FRAME_CANT_TRL_POS            (COMCP_FRAME_CANT_BCC_POS + 1)

    #define COMCP_FRAME_CANT_STAT_ON            0x01                    //开天线
    #define COMCP_FRAME_CANT_STAT_OFF           0x00                    //关天线

    //响应帧:成功
    #define COMCP_RFRAME_CANT_OFLEN             5
    #define COMCP_RFRAME_CANT_OBCC_POS          COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_CANT_OTRL_POS          (COMCP_RFRAME_CANT_OBCC_POS + 1)



    ///////////////////////////////////////////////////////////////////////////////
    //获取UID: 得到卡的ID，并将卡置于Active状态
    #define COMCP_FRAME_GUID_FLEN               4
    #define COMCP_FRAME_CMD_GUID                0x54
    #define COMCP_FRAME_GUID_MODE_POS           COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_GUID_MODE_LEN           1
    #define COMCP_FRAME_GUID_BCC_POS            (COMCP_FRAME_GUID_MODE_POS + COMCP_FRAME_GUID_MODE_LEN)
    #define COMCP_FRAME_GUID_TRL_POS            (COMCP_FRAME_GUID_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_GUID_OFLEN             5                      //注意:这个是没有计算参数的长度的，因为这里长度是可变的
    #define COMCP_RFRAME_GUID_OCTYPE_POS        COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_GUID_OCTYPE_LEN        2
    #define COMCP_RFRAME_GUID_IDLEN_POS         (COMCP_RFRAME_GUID_OCTYPE_POS + COMCP_RFRAME_GUID_OCTYPE_LEN)
    #define COMCP_RFRAME_GUID_IDLEN_LEN         1
    #define COMCP_RFRAME_GUID_OUID_POS          (COMCP_RFRAME_GUID_IDLEN_POS + COMCP_RFRAME_GUID_IDLEN_LEN)




    ///////////////////////////////////////////////////////////////////////////////
    //处理模式:读写器接收到该帧后，以ALL的模式循环获取卡的UID，并将卡设置成Active状态，
    //如果50ms内，没有接收到上位机的其他命令时，将卡设置成HALT状态
    //如此反复循环
    //自动获取UID
    #define COMCP_FRAME_AGUID_FLEN               3
    #define COMCP_FRAME_CMD_AGUID                0x55
    #define COMCP_FRAME_AGUID_BCC_POS            COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_AGUID_TRL_POS            (COMCP_FRAME_AGUID_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_AGUID_OFLEN             5                  //注意:这个是没有计算参数的长度的，因为这里长度是可变的
    #define COMCP_RFRAME_AGUID_OCTYPE_POS        COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_AGUID_OCTYPE_LEN        2
    #define COMCP_RFRAME_AGUID_IDLEN_POS         (COMCP_RFRAME_AGUID_OCTYPE_POS + COMCP_RFRAME_AGUID_OCTYPE_LEN)
    #define COMCP_RFRAME_AGUID_IDLEN_LEN         1
    #define COMCP_RFRAME_AGUID_OUID_POS          (COMCP_RFRAME_AGUID_IDLEN_POS + COMCP_RFRAME_AGUID_IDLEN_LEN)



    ///////////////////////////////////////////////////////////////////////////////
    //halt命令
    #define COMCP_FRAME_HALT_FLEN                3
    #define COMCP_FRAME_CMD_HALT                 0x56
    #define COMCP_FRAME_HALT_BCC_POS             COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_HALT_TRL_POS             (COMCP_FRAME_HALT_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_HALT_OFLEN              5
    #define COMCP_RFRAME_HALT_OBCC_POS           COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_HALT_OTRL_POS           (COMCP_RFRAME_HALT_OBCC_POS + 1)

    /*
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //change pwd
    #define COMCP_FRAME_CHPWD_FLEN              15
    #define COMCP_FRAME_CMD_CHPWD               0x57

    #define COMCP_FRAME_CHPWD_ADDR_POS          COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_CHPWD_ADDR_LEN          1
    #define COMCP_FRAME_CHPWD_OPWD_POS          (COMCP_FRAME_CHPWD_ADDR_POS + COMCP_FRAME_CHPWD_ADDR_LEN)
    #define COMCP_FRAME_CHPWD_OPWD_LEN           COMCP_FRAME_PWD_LEN
    #define COMCP_FRAME_CHPWD_NPWD_POS          (COMCP_FRAME_CHPWD_OPWD_POS + COMCP_FRAME_CHPWD_OPWD_LEN)
    #define COMCP_FRAME_CHPWD_NPWD_LEN           COMCP_FRAME_PWD_LEN
    #define COMCP_FRAME_CHPWD_BCC_POS           (COMCP_FRAME_CHPWD_NPWD_POS + COMCP_FRAME_CHPWD_NPWD_LEN)
    #define COMCP_FRAME_CHPWD_TRL_POS           (COMCP_FRAME_CHPWD_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_CHPWD_OFLEN                5
    #define COMCP_RFRAME_CHPWD_OBCC_POS             COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_CHPWD_OTRL_POS             (COMCP_RFRAME_CHPWD_OBCC_POS + 1)
    */

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //授权写:通过密钥验证相应的数据区，然后将数据写入数据块
    #define COMCP_FRAME_AUTHW_FLEN               27
    #define COMCP_FRAME_CMD_AUTHW                0x11
    #define COMCP_FRAME_AUTHW_KEYT_POS           COMCP_FRAME_PAR_POS                                         //授权模式
    #define COMCP_FRAME_AUTHW_KEYT_LEN           1
    #define COMCP_FRAME_AUTHW_PWD_POS            (COMCP_FRAME_AUTHW_KEYT_POS + COMCP_FRAME_AUTHW_KEYT_LEN)   //6字节密钥
    #define COMCP_FRAME_AUTHW_PWD_LEN            COMCP_FRAME_PWD_LEN
    #define COMCP_FRAME_AUTHW_ADDR_POS           (COMCP_FRAME_AUTHW_PWD_POS + COMCP_FRAME_AUTHW_PWD_LEN)     //数据块地址
    #define COMCP_FRAME_AUTHW_ADDR_LEN           1
    #define COMCP_FRAME_AUTHW_DATA_POS           (COMCP_FRAME_AUTHW_ADDR_POS + COMCP_FRAME_AUTHW_ADDR_LEN)   //16字节数据
    #define COMCP_FRAME_AUTHW_DATA_LEN           16
    #define COMCP_FRAME_AUTHW_BCC_POS            (COMCP_FRAME_AUTHW_DATA_POS + COMCP_FRAME_AUTHW_DATA_LEN)
    #define COMCP_FRAME_AUTHW_TRL_POS            (COMCP_FRAME_AUTHW_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_AUTHW_OFLEN             5
    #define COMCP_RFRAME_AUTHW_OBCC_POS          COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_AUTHW_OTRL_POS          (COMCP_RFRAME_AUTHW_OBCC_POS + 1)


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //授权读:通过密钥验证相应的数据区，然后读取数据
    #define COMCP_FRAME_AUTHR_FLEN               11
    #define COMCP_FRAME_CMD_AUTHR                0x12
    #define COMCP_FRAME_AUTHR_KEYT_POS           COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_AUTHR_KEYT_LEN           1
    #define COMCP_FRAME_AUTHR_PWD_POS            (COMCP_FRAME_AUTHR_KEYT_POS + COMCP_FRAME_AUTHR_KEYT_LEN)
    #define COMCP_FRAME_AUTHR_PWD_LEN            COMCP_FRAME_PWD_LEN
    #define COMCP_FRAME_AUTHR_ADDR_POS           (COMCP_FRAME_AUTHR_PWD_POS + COMCP_FRAME_AUTHR_PWD_LEN)
    #define COMCP_FRAME_AUTHR_ADDR_LEN           1
    #define COMCP_FRAME_AUTHR_BCC_POS            (COMCP_FRAME_AUTHR_ADDR_POS + COMCP_FRAME_AUTHR_ADDR_LEN)
    #define COMCP_FRAME_AUTHR_TRL_POS            (COMCP_FRAME_AUTHR_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_AUTHR_OFLEN             22
    #define COMCP_RFRAME_AUTHR_OADDR_POS         COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_AUTHR_OADDR_LEN         1
    #define COMCP_RFRAME_AUTHR_ODATA_POS         (COMCP_RFRAME_AUTHR_OADDR_POS + COMCP_RFRAME_AUTHR_OADDR_LEN)
    #define COMCP_RFRAME_AUTHR_ODATA_LEN         16
    #define COMCP_RFRAME_AUTHR_OBCC_POS          (COMCP_RFRAME_AUTHR_ODATA_POS + COMCP_RFRAME_AUTHR_ODATA_LEN)
    #define COMCP_RFRAME_AUTHR_OTRL_POS          (COMCP_RFRAME_AUTHR_OBCC_POS + 1)


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //授权写值:通过密钥验证相应的数据区，然后将值和地址按照值格式组成16字节数据块，并写入指令的块地址
    #define COMCP_FRAME_AUTHWV_FLEN              15
    #define COMCP_FRAME_CMD_AUTHWV               0x13
    #define COMCP_FRAME_AUTHWV_KEYT_POS          COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_AUTHWV_KEYT_LEN          1
    #define COMCP_FRAME_AUTHWV_PWD_POS           (COMCP_FRAME_AUTHWV_KEYT_POS + COMCP_FRAME_AUTHWV_KEYT_LEN)
    #define COMCP_FRAME_AUTHWV_PWD_LEN           COMCP_FRAME_PWD_LEN
    #define COMCP_FRAME_AUTHWV_ADDR_POS          (COMCP_FRAME_AUTHWV_PWD_POS + COMCP_FRAME_AUTHWV_PWD_LEN)
    #define COMCP_FRAME_AUTHWV_ADDR_LEN          1
    #define COMCP_FRAME_AUTHWV_VALUE_POS         (COMCP_FRAME_AUTHWV_ADDR_POS + COMCP_FRAME_AUTHWV_ADDR_LEN)
    #define COMCP_FRAME_AUTHWV_VALUE_LEN         4
    #define COMCP_FRAME_AUTHWV_BCC_POS           (COMCP_FRAME_AUTHWV_VALUE_POS + COMCP_FRAME_AUTHWV_VALUE_LEN)
    #define COMCP_FRAME_AUTHWV_TRL_POS           (COMCP_FRAME_AUTHWV_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_AUTHWV_OFLEN            5
    #define COMCP_RFRAME_AUTHWV_OBCC_POS         COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_AUTHWV_OTRL_POS         (COMCP_RFRAME_AUTHWV_OBCC_POS + 1)



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //授权值操作:通过密钥验证相应的数据区，然后根据值操作的模式和值，完成对数据区的值操作
    #define COMCP_FRAME_AUTHV_FLEN               17
    #define COMCP_FRAME_CMD_AUTHV                0x14
    #define COMCP_FRAME_AUTHV_KEYT_POS           COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_AUTHV_KEYT_LEN           1
    #define COMCP_FRAME_AUTHV_PWD_POS            (COMCP_FRAME_AUTHV_KEYT_POS + COMCP_FRAME_AUTHV_KEYT_LEN)
    #define COMCP_FRAME_AUTHV_PWD_LEN            COMCP_FRAME_PWD_LEN
    #define COMCP_FRAME_AUTHV_MODE_POS           (COMCP_FRAME_AUTHV_PWD_POS + COMCP_FRAME_AUTHV_PWD_LEN)    //操作模式
    #define COMCP_FRAME_AUTHV_MODE_LEN           1
    #define COMCP_FRAME_AUTHV_ADDR_POS           (COMCP_FRAME_AUTHV_MODE_POS + COMCP_FRAME_AUTHV_MODE_LEN)  //操作地址
    #define COMCP_FRAME_AUTHV_ADDR_LEN           1
    #define COMCP_FRAME_AUTHV_VALUE_POS          (COMCP_FRAME_AUTHV_ADDR_POS + COMCP_FRAME_AUTHV_ADDR_LEN)  //值
    #define COMCP_FRAME_AUTHV_VALUE_LEN          4
    #define COMCP_FRAME_AUTHV_TADDR_POS          (COMCP_FRAME_AUTHV_VALUE_POS + COMCP_FRAME_AUTHV_VALUE_LEN)//传送地址
    #define COMCP_FRAME_AUTHV_TADDR_LEN          1
    #define COMCP_FRAME_AUTHV_BCC_POS            (COMCP_FRAME_AUTHV_TADDR_POS + COMCP_FRAME_AUTHV_TADDR_LEN)
    #define COMCP_FRAME_AUTHV_TRL_POS            (COMCP_FRAME_AUTHV_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_AUTHV_OFLEN             5
    #define COMCP_RFRAME_AUTHV_OBCC_POS          COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_AUTHV_OTRL_POS          (COMCP_RFRAME_AUTHV_OBCC_POS + 1)



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //授权读值:通过密钥验证相应的数据区，然后将指令地址的值读出
    #define COMCP_FRAME_AUTHRV_FLEN              11
    #define COMCP_FRAME_CMD_AUTHRV               0x15
    #define COMCP_FRAME_AUTHRV_KEYT_POS          COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_AUTHRV_KEYT_LEN          1
    #define COMCP_FRAME_AUTHRV_PWD_POS           (COMCP_FRAME_AUTHRV_KEYT_POS + COMCP_FRAME_AUTHRV_KEYT_LEN)
    #define COMCP_FRAME_AUTHRV_PWD_LEN           COMCP_FRAME_PWD_LEN
    #define COMCP_FRAME_AUTHRV_ADDR_POS          (COMCP_FRAME_AUTHRV_PWD_POS + COMCP_FRAME_AUTHRV_PWD_LEN)
    #define COMCP_FRAME_AUTHRV_ADDR_LEN          1
    #define COMCP_FRAME_AUTHRV_BCC_POS           (COMCP_FRAME_AUTHRV_ADDR_POS + COMCP_FRAME_AUTHRV_ADDR_LEN)
    #define COMCP_FRAME_AUTHRV_TRL_POS           (COMCP_FRAME_AUTHRV_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_AUTHRV_OFLEN            10
    #define COMCP_RFRAME_AUTHRV_OADDR_POS        COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_AUTHRV_OADDR_LEN        1
    #define COMCP_RFRAME_AUTHRV_OVALUE_POS       (COMCP_RFRAME_AUTHRV_OADDR_POS + COMCP_RFRAME_AUTHRV_OADDR_LEN)
    #define COMCP_RFRAME_AUTHRV_OVALUE_LEN       4
    #define COMCP_RFRAME_AUTHRV_OBCC_POS         (COMCP_RFRAME_AUTHRV_OVALUE_POS + COMCP_RFRAME_AUTHRV_OVALUE_LEN)
    #define COMCP_RFRAME_AUTHRV_OTRL_POS         (COMCP_RFRAME_AUTHRV_OBCC_POS + 1)



    //以下的命令和上面介绍的对数据区的操作命令类似，只是不需要授权
    //因为一个扇区只要进行一次授权验证就可以了，
    //所以用户对某一个扇区中的某一块完成授权验证以后，对扇区的块操作就不要需要再次密码验证了
    //除非用户修改了密码
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //write
    #define COMCP_FRAME_W_FLEN                   20
    #define COMCP_FRAME_CMD_W                    0x16
    #define COMCP_FRAME_W_ADDR_POS               COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_W_ADDR_LEN               1
    #define COMCP_FRAME_W_DATA_POS               (COMCP_FRAME_W_ADDR_POS + COMCP_FRAME_W_ADDR_LEN)
    #define COMCP_FRAME_W_DATA_LEN               16
    #define COMCP_FRAME_W_BCC_POS                (COMCP_FRAME_W_DATA_POS + COMCP_FRAME_W_DATA_LEN)
    #define COMCP_FRAME_W_TRL_POS                (COMCP_FRAME_W_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_W_OFLEN                 5
    #define COMCP_RFRAME_W_OBCC_POS              COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_W_OTRL_POS              (COMCP_RFRAME_W_OBCC_POS + 1)


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //read
    #define COMCP_FRAME_R_FLEN                   4
    #define COMCP_FRAME_CMD_R                    0x17
    #define COMCP_FRAME_R_ADDR_POS               COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_R_ADDR_LEN               1
    #define COMCP_FRAME_R_BCC_POS                (COMCP_FRAME_R_ADDR_POS + COMCP_FRAME_R_ADDR_LEN)
    #define COMCP_FRAME_R_TRL_POS                (COMCP_FRAME_R_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_R_OFLEN                 22
    #define COMCP_RFRAME_R_OADDR_POS             COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_R_OADDR_LEN             1
    #define COMCP_RFRAME_R_ODATA_POS             (COMCP_RFRAME_R_OADDR_POS + COMCP_RFRAME_R_OADDR_LEN)
    #define COMCP_RFRAME_R_ODATA_LEN             16
    #define COMCP_RFRAME_R_OBCC_POS              (COMCP_RFRAME_R_ODATA_POS + COMCP_RFRAME_R_ODATA_LEN)
    #define COMCP_RFRAME_R_OTRL_POS              (COMCP_RFRAME_R_OBCC_POS + 1)


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //write value
    #define COMCP_FRAME_WV_FLEN                  8
    #define COMCP_FRAME_CMD_WV                   0x18
    #define COMCP_FRAME_WV_ADDR_POS              COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_WV_ADDR_LEN              1
    #define COMCP_FRAME_WV_VALUE_POS             (COMCP_FRAME_WV_ADDR_POS + COMCP_FRAME_WV_ADDR_LEN)
    #define COMCP_FRAME_WV_VALUE_LEN             4
    #define COMCP_FRAME_WV_BCC_POS               (COMCP_FRAME_WV_VALUE_POS + COMCP_FRAME_WV_VALUE_LEN)
    #define COMCP_FRAME_WV_TRL_POS               (COMCP_FRAME_WV_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_WV_OFLEN                5
    #define COMCP_RFRAME_WV_OBCC_POS             COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_WV_OTRL_POS             (COMCP_RFRAME_WV_OBCC_POS + 1)


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //value
    #define COMCP_FRAME_V_FLEN                   10
    #define COMCP_FRAME_CMD_V                    0x19
    #define COMCP_FRAME_V_MODE_POS               COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_V_MODE_LEN               1
    #define COMCP_FRAME_V_ADDR_POS               (COMCP_FRAME_V_MODE_POS + COMCP_FRAME_V_MODE_LEN)
    #define COMCP_FRAME_V_ADDR_LEN               1
    #define COMCP_FRAME_V_VALUE_POS              (COMCP_FRAME_V_ADDR_POS + COMCP_FRAME_V_ADDR_LEN)
    #define COMCP_FRAME_V_VALUE_LEN              4
    #define COMCP_FRAME_V_TADDR_POS              (COMCP_FRAME_V_VALUE_POS + COMCP_FRAME_V_VALUE_LEN)
    #define COMCP_FRAME_V_TADDR_LEN              1
    #define COMCP_FRAME_V_BCC_POS                (COMCP_FRAME_V_TADDR_POS + COMCP_FRAME_V_TADDR_LEN)
    #define COMCP_FRAME_V_TRL_POS                (COMCP_FRAME_V_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_V_OFLEN                 5
    #define COMCP_RFRAME_V_OBCC_POS              COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_V_OTRL_POS              (COMCP_RFRAME_V_OBCC_POS + 1)


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //read value
    #define COMCP_FRAME_RV_FLEN                  4
    #define COMCP_FRAME_CMD_RV                   0x1A
    #define COMCP_FRAME_RV_ADDR_POS              COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_RV_ADDR_LEN              1
    #define COMCP_FRAME_RV_BCC_POS               (COMCP_FRAME_RV_ADDR_POS + COMCP_FRAME_RV_ADDR_LEN)
    #define COMCP_FRAME_RV_TRL_POS               (COMCP_FRAME_RV_BCC_POS + 1)
    //响应帧:成功
    #define COMCP_RFRAME_RV_OFLEN                10
    #define COMCP_RFRAME_RV_OADDR_POS            COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_RV_OADDR_LEN            1
    #define COMCP_RFRAME_RV_OVALUE_POS           (COMCP_RFRAME_RV_OADDR_POS + COMCP_RFRAME_RV_OADDR_LEN)
    #define COMCP_RFRAME_RV_OVALUE_LEN           4
    #define COMCP_RFRAME_RV_OBCC_POS             (COMCP_RFRAME_RV_OVALUE_POS + COMCP_RFRAME_RV_OVALUE_LEN)
    #define COMCP_RFRAME_RV_OTRL_POS             (COMCP_RFRAME_RV_OBCC_POS + 1)



    //以下提供数据帧打包函数，用户不需要详细的了解数据帧的结构，就可以获得正确的数据帧
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //请求帧打包格式
/*
    u8 COMCP_ResetReaderFrame(u8 *pFrame);
    u8 COMCP_ControlAntFrame(u8 *pFrame, u8 antState);
    u8 COMCP_GetUIDFrame(u8 *pFrame, u8 rsqtMode);
    u8 COMCP_AutoGetUIDFrame(u8 *pFrame);
    u8 COMCP_HaltFrame(u8 *pFrame);
    u8 COMCP_AuthWFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr, u8 *pData);
    u8 COMCP_AuthRFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr);
    u8 COMCP_AuthWValueFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr, u8 *pValue);
    u8 COMCP_AuthRValueFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr);
    u8 COMCP_AuthValueFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 valueMode, u8 addr, u8 *pValue, u8 transAddr);
    u8 COMCP_WFrame(u8 *pFrame, u8 addr, u8 *pData);
    u8 COMCP_RFrame(u8 *pFrame, u8 addr);
    u8 COMCP_WValueFrame(u8 *pFrame, u8 addr, u8 *pValue);
    u8 COMCP_RValueFrame(u8 *pFrame, u8 addr);
    u8 COMCP_ValueFrame(u8 *pFrame, u8 valueMode, u8 addr, u8 *pValue, u8 transAddr);
*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //响应帧打包格式(用户不需要，可以用来参考)
    u8 COMCP_ResetReaderORspFrame(u8 *pFrame);
    u8 COMCP_ControlAntORspFrame(u8 *pFrame);
    u8 COMCP_GetUIDORspFrame(u8 *pFrame, u8 cardTypeH, u8 cardTypeL, u8 uidLen, u8 *pUID);
    u8 COMCP_AGetUIDORspFrame(u8 *pFrame, u8 cardTypeH, u8 cardTypeL, u8 uidLen, u8 *pUID);
    u8 COMCP_HaltORspFrame(u8 *pFrame);
    u8 COMCP_AuthWORspFrame(u8 *pFrame);
    u8 COMCP_AuthRORspFrame(u8 *pFrame, u8 addr, u8 *pData);
    u8 COMCP_AuthWVORspFrame(u8 *pFrame);
    u8 COMCP_AuthRVORspFrame(u8 *pFrame, u8 addr, u8 *pValue);
    u8 COMCP_AuthVORspFrame(u8 *pFrame);
    u8 COMCP_WORspFrame(u8 *pFrame);
    u8 COMCP_RORspFrame(u8 *pFrame, u8 addr, u8 *pData);
    u8 COMCP_WVORspFrame(u8 *pFrame);
    u8 COMCP_RVORspFrame(u8 *pFrame, u8 addr, u8 *pValue);
    u8 COMCP_VORspFrame(u8 *pFrame);

#elif (defined(_ANYID_STM32_BCTRL_) | defined(_ANYID_PIC18_BWCOM_))

    //设置Bike借还状态（0x01）
    #define COMCP_FRAME_CMD_SBSTAT                   0x01
    #define COMCP_FRAME_SBSTAT_LEN                   4
    #define COMCP_FRAME_SBSTAT_STAT_POS              COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_SBSTAT_STAT_LEN              1
    #define COMCP_FRAME_SBSTAT_BCC_POS               (COMCP_FRAME_SBSTAT_STAT_POS + COMCP_FRAME_SBSTAT_STAT_LEN)
    #define COMCP_FRAME_SBSTAT_BCC_LEN               1
    #define COMCP_FRAME_SBSTAT_TRL_POS               (COMCP_FRAME_SBSTAT_BCC_POS + COMCP_FRAME_SBSTAT_BCC_LEN)
    //借出/归还状态
    #define COMCP_FRAME_SBSTAT_LEND                  0x01
    #define COMCP_FRAME_SBSTAT_RETURN                0x00


    //读取该次租赁用户行驶记录（0x02）
    #define COMCP_FRAME_CMD_RBLOG                    0x02
    #define COMCP_FRAME_RBLOG_LEN                    3
    #define COMCP_FRAME_RBLOG_BCC_POS                (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_RBLOG_BCC_LEN                1
    #define COMCP_FRAME_RBLOG_TRL_POS                (COMCP_FRAME_RBLOG_BCC_POS + COMCP_FRAME_RBLOG_BCC_LEN)
    //读取该次租赁用户行驶记录的响应帧
    #define COMCP_RFRAME_RBLOG_OFLEN                 13
    #define COMCP_RFRAME_RBLOG_MILEAGE_POS           (COMCP_RFRAME_PAR_POS)
    #define COMCP_RFRAME_RBLOG_MILEAGE_LEN           4
    #define COMCP_RFRAME_RBLOG_CALORIE_POS           (COMCP_RFRAME_RBLOG_MILEAGE_POS + COMCP_RFRAME_RBLOG_MILEAGE_LEN)
    #define COMCP_RFRAME_RBLOG_CALORIE_LEN           4
    #define COMCP_RFRAME_RBLOG_OBCC_POS              (COMCP_RFRAME_RBLOG_CALORIE_POS + COMCP_RFRAME_RBLOG_CALORIE_LEN)
    #define COMCP_RFRAME_RBLOG_OTRL_POS              (COMCP_RFRAME_RBLOG_OBCC_POS + 1)

    //更新程序（0x03）
    #define COMCP_FRAME_CMD_UPDATE                   0x03
    #define COMCP_FRAME_UPDATE_LEN                   0x27
    #define COMCP_FRAME_UPDATE_INDEX_POS             COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_UPDATE_INDEX_LEN             4
    #define COMCP_FRAME_UPDATE_DATA_POS              (COMCP_FRAME_UPDATE_INDEX_POS + COMCP_FRAME_UPDATE_INDEX_LEN)
    #define COMCP_FRAME_UPDATE_DATA_LEN              32
    #define COMCP_FRAME_UPDATE_BCC_POS               (COMCP_FRAME_UPDATE_DATA_POS + COMCP_FRAME_UPDATE_DATA_LEN)
    #define COMCP_FRAME_UPDATE_BCC_LEN               1
    #define COMCP_FRAME_UPDATE_TRL_POS               (COMCP_FRAME_UPDATE_BCC_POS + COMCP_FRAME_UPDATE_BCC_LEN)
    //更新程序错误响应帧
    #define COMCP_RFRAME_UPDATE_FFLEN                10
    #define COMCP_RFRAME_UPDATE_FINDEX_POS           (COMCP_RFRAME_FPAR_POS)
    #define COMCP_RFRAME_UPDATE_FINDEX_LEN           4
    #define COMCP_RFRAME_UPDATE_FBCC_POS             (COMCP_RFRAME_UPDATE_FINDEX_POS + COMCP_RFRAME_UPDATE_FINDEX_LEN)
    #define COMCP_RFRAME_UPDATE_FTRL_POS             (COMCP_RFRAME_UPDATE_FBCC_POS + 1)
    //系统不处于IDLE状态，不能更新
    #define COMCP_FRAME_UPDATE_EBUSY                 0xFFFFFFFF //系统忙


    //请求更新程序（0x04）
    #define COMCP_FRAME_CMD_RQTUPDATE                0x04
    #define COMCP_FRAME_RQTUPDATE_LEN                12
    #define COMCP_FRAME_RQTUPDATE_TYPE_POS           COMCP_FRAME_PAR_POS
    #define COMCP_FRAME_RQTUPDATE_TYPE_LEN           1
    #define COMCP_FRAME_RQTUPDATE_NUM_POS            (COMCP_FRAME_RQTUPDATE_TYPE_POS + COMCP_FRAME_RQTUPDATE_TYPE_LEN)
    #define COMCP_FRAME_RQTUPDATE_NUM_LEN            4
    #define COMCP_FRAME_RQTUPDATE_SIZE_POS           (COMCP_FRAME_RQTUPDATE_NUM_POS + COMCP_FRAME_RQTUPDATE_NUM_LEN)
    #define COMCP_FRAME_RQTUPDATE_SIZE_LEN           4
    #define COMCP_FRAME_RQTUPDATE_BCC_POS            (COMCP_FRAME_RQTUPDATE_SIZE_POS + COMCP_FRAME_RQTUPDATE_SIZE_LEN)
    #define COMCP_FRAME_RQTUPDATE_BCC_LEN            1
    #define COMCP_FRAME_RQTUPDATE_TRL_POS            (COMCP_FRAME_RQTUPDATE_BCC_POS + COMCP_FRAME_RQTUPDATE_BCC_LEN)
    //请求更新程序响应帧
    #define COMCP_RFRAME_RQTUPDATE_OFLEN             6
    #define COMCP_RFRAME_RQTUPDATE_OSTATE_POS        COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_RQTUPDATE_OSTATE_LEN        1
    #define COMCP_RFRAME_RQTUPDATE_OBCC_POS          (COMCP_RFRAME_RQTUPDATE_OSTATE_POS + COMCP_RFRAME_RQTUPDATE_OSTATE_LEN)
    #define COMCP_RFRAME_RQTUPDATE_OTRL_POS          (COMCP_RFRAME_RQTUPDATE_OBCC_POS + 1)
    //请求更新程序状态
    #define COMCP_FRAME_RQTUPDATE_STATE_YES          0x00   //可以被更新
    #define COMCP_FRAME_RQTUPDATE_STATE_ENALW        0x01   //更新内容未被更新到应用区，不允许再次更新
    #define COMCP_FRAME_RQTUPDATE_STATE_ENFMT        0x02   //未被格式化
    #define COMCP_FRAME_RQTUPDATE_STATE_EBUSY        0x03   //系统忙
    #define COMCP_FRAME_RQTUPDATE_STATE_EPAR         0x04   //参数错误
    //请求更新类型
    #define COMCP_FRAME_RQTUPDATE_TYPE_BP            0x01
    #define COMCP_FRAME_RQTUPDATE_TYPE_MP3P          0x02
    #define COMCP_FRAME_RQTUPDATE_TYPE_PROGRAME      0x03
    #define COMCP_FRAME_RQTUPDATE_TYPE_BPNAME        0x04
    #define COMCP_FRAME_RQTUPDATE_TYPE_MP3           0x05
    #define COMCP_FRAME_RQTUPDATE_TYPE_FORCE         0x00   //强制更新
    


    //格式化更新区（0x05）
    #define COMCP_FRAME_CMD_FORMAT                   0x05
    #define COMCP_FRAME_FORMAT_LEN                   0x04
    #define COMCP_FRAME_FORMAT_NUM_POS               (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_FORMAT_NUM_LEN               1
    #define COMCP_FRAME_FORMAT_BCC_POS               (COMCP_FRAME_FORMAT_NUM_POS + COMCP_FRAME_FORMAT_NUM_LEN)
    #define COMCP_FRAME_FORMAT_BCC_LEN               1
    #define COMCP_FRAME_FORMAT_TRL_POS               (COMCP_FRAME_FORMAT_BCC_POS + COMCP_FRAME_FORMAT_BCC_LEN)
    #define COMCP_FRAME_FORMAT_ALL                   0x00
    #define COMCP_FRAME_FORMAT_FORCE                 0xFF
    //#define COMCP_FRAME_FORMAT_QUERY                 0xFE   //查询更新状态
    #define COMCP_FRAME_FORMAT_BLOCK_MAX             0x10   //最大块数目
    //格式化更新区响应帧
    #define COMCP_RFRAME_FORMAT_OFLEN                6
    #define COMCP_RFRAME_FORMAT_OSTATE_POS           COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_FORMAT_OSTATE_LEN           1
    #define COMCP_RFRAME_FORMAT_OBCC_POS             (COMCP_RFRAME_FORMAT_OSTATE_POS + COMCP_RFRAME_FORMAT_OSTATE_LEN)
    #define COMCP_RFRAME_FORMAT_OTRL_POS             (COMCP_RFRAME_FORMAT_OBCC_POS + 1)
    //格式化状态
    #define COMCP_FRAME_FORMAT_STATE_OVER            0xFF   //格式化完成
    #define COMCP_FRAME_FORMAT_STATE_START           0xFE   //开始格式化
    #define COMCP_FRAME_FORMAT_STATE_ENUM            0xFD   //块数目错误
    #define COMCP_FRAME_FORMAT_STATE_ENALW           0xFC   //更新区已有更新文件，不许更新
    #define COMCP_FRAME_FORMAT_STATE_EBUSY           0xFB   //系统忙
    

    /*
    //读取车辆的授权状态（0x05）
    #define COMCP_FRAME_CMD_GETLIC                   0x05
    #define COMCP_FRAME_GETLIC_LEN                   0x03
    #define COMCP_FRAME_GETLIC_BCC_POS               (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_GETLIC_BCC_LEN               1
    #define COMCP_FRAME_GETLIC_TRL_POS               (COMCP_FRAME_GETLIC_BCC_POS + COMCP_FRAME_GETLIC_BCC_LEN)
    //读取车辆的授权状态响应帧
    #define COMCP_RFRAME_GETLIC_OFLEN                6
    #define COMCP_RFRAME_GETLIC_OSTATE_POS           COMCP_RFRAME_PAR_POS
    #define COMCP_RFRAME_GETLIC_OSTATE_LEN           1
    #define COMCP_RFRAME_GETLIC_OBCC_POS             (COMCP_RFRAME_GETLIC_OSTATE_POS + COMCP_RFRAME_GETLIC_OSTATE_LEN)
    #define COMCP_RFRAME_GETLIC_OTRL_POS             (COMCP_RFRAME_GETLIC_OBCC_POS + 1)
    //授权状态
    #define COMCP_FRAME_LIC_STATE_NO                 0x00
    #define COMCP_FRAME_LIC_STATE_YES                0x01
    */

    //获取Bike状态（0x06）
    #define COMCP_FRAME_CMD_GETSTAT                  0x06
    #define COMCP_FRAME_GETSTAT_LEN                  0x03
    #define COMCP_FRAME_GETSTAT_BCC_POS              (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_GETSTAT_BCC_LEN              1
    #define COMCP_FRAME_GETSTAT_TRL_POS              (COMCP_FRAME_GETSTAT_BCC_POS + COMCP_FRAME_GETSTAT_BCC_LEN)
    //获取Bike状态响应帧
    #define COMCP_RFRAME_GETSTAT_OFLEN               6
    #define COMCP_RFRAME_GETSTAT_OLNDSTAT_POS        (COMCP_RFRAME_PAR_POS)
    #define COMCP_RFRAME_GETSTAT_OLNDSTAT_LEN        1
    #define COMCP_RFRAME_GETSTAT_OBCC_POS            (COMCP_RFRAME_GETSTAT_OLNDSTAT_POS + COMCP_RFRAME_GETSTAT_OLNDSTAT_LEN)
    #define COMCP_RFRAME_GETSTAT_OTRL_POS            (COMCP_RFRAME_GETSTAT_OBCC_POS + 1)



    //获取用户信息（0x07）
    #define COMCP_FRAME_CMD_GETUSER                  0x07
    #define COMCP_FRAME_GETUSER_LEN                  0x03
    #define COMCP_FRAME_GETUSER_BCC_POS              (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_GETUSER_BCC_LEN              1
    #define COMCP_FRAME_GETUSER_TRL_POS              (COMCP_FRAME_GETUSER_BCC_POS + COMCP_FRAME_GETUSER_BCC_LEN)
    //获取用户信息响应帧
    #define COMCP_RFRAME_GETUSER_OFLEN               16
    #define COMCP_RFRAME_GETUSER_OMILEAGE_POS        (COMCP_RFRAME_PAR_POS)
    #define COMCP_RFRAME_GETUSER_OMILEAGE_LEN        4
    #define COMCP_RFRAME_GETUSER_OTIME_POS           (COMCP_RFRAME_GETUSER_OMILEAGE_POS + COMCP_RFRAME_GETUSER_OMILEAGE_LEN)
    #define COMCP_RFRAME_GETUSER_OTIME_LEN           3
    #define COMCP_RFRAME_GETUSER_OID_POS             (COMCP_RFRAME_GETUSER_OTIME_POS + COMCP_RFRAME_GETUSER_OTIME_LEN)
    #define COMCP_RFRAME_GETUSER_OID_LEN             4
    #define COMCP_RFRAME_GETUSER_OBCC_POS            (COMCP_RFRAME_GETUSER_OID_POS + COMCP_RFRAME_GETUSER_OID_LEN)
    #define COMCP_RFRAME_GETUSER_OTRL_POS            (COMCP_RFRAME_GETUSER_OBCC_POS + 1)

    //设置用户信息（0x08）
    #define COMCP_FRAME_CMD_SETUSER                  0x08
    #define COMCP_FRAME_SETUSER_LEN                  10
    #define COMCP_FRAME_SETUSER_TIME_POS             (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_SETUSER_TIME_LEN             3
    #define COMCP_FRAME_SETUSER_ID_POS               (COMCP_FRAME_SETUSER_TIME_POS + COMCP_FRAME_SETUSER_TIME_LEN)
    #define COMCP_FRAME_SETUSER_ID_LEN               4
    #define COMCP_FRAME_SETUSER_BCC_POS              (COMCP_FRAME_SETUSER_ID_POS + COMCP_FRAME_SETUSER_ID_LEN)
    #define COMCP_FRAME_SETUSER_BCC_LEN              1
    #define COMCP_FRAME_SETUSER_TRL_POS              (COMCP_FRAME_SETUSER_BCC_POS + COMCP_FRAME_SETUSER_BCC_LEN)

    //设置用户信息（0x09）
    #define COMCP_FRAME_CMD_TICK                     0x09
    //Tick响应帧
    #define COMCP_RFRAME_TICK_OFLEN                  17
    #define COMCP_RFRAME_TICK_OBIKEID_POS            (COMCP_RFRAME_PAR_POS)
    #define COMCP_RFRAME_TICK_OBIKEID_LEN            12
    #define COMCP_RFRAME_TICK_OBCC_POS               (COMCP_RFRAME_TICK_OBIKEID_POS + COMCP_RFRAME_TICK_OBIKEID_LEN)
    #define COMCP_RFRAME_TICK_OTRL_POS               (COMCP_RFRAME_TICK_OBCC_POS + 1)


    //读取FLASH的数据（0x0A）内部测试命令，不可外泄
    #define COMCP_FRAME_CMD_RFLASH                   0x0A
    #define COMCP_FRAME_RFLASH_LEN                   0x08
    #define COMCP_FRAME_RFLASH_ADDR_POS              (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_RFLASH_ADDR_LEN              4
    #define COMCP_FRAME_RFLASH_SIZE_POS              (COMCP_FRAME_RFLASH_ADDR_POS + COMCP_FRAME_RFLASH_ADDR_LEN)
    #define COMCP_FRAME_RFLASH_SIZE_LEN              1
    #define COMCP_FRAME_RFLASH_BCC_POS               (COMCP_FRAME_RFLASH_SIZE_POS + COMCP_FRAME_RFLASH_SIZE_LEN)
    #define COMCP_FRAME_RFLASH_BCC_LEN               1
    #define COMCP_FRAME_RFLASH_TRL_POS               (COMCP_FRAME_RFLASH_BCC_POS + COMCP_FRAME_RFLASH_BCC_LEN)
    //每次读取的最长长度
    #define COMCP_FRAME_RFLASH_MAX_SIZE              32
    //读取FLASH的数据响应帧
    #define COMCP_RFRAME_RFLASH_OFLEN               5
    #define COMCP_RFRAME_RFLASH_ODATA_POS           (COMCP_RFRAME_PAR_POS)


    //停止更新（0x0B）
    #define COMCP_FRAME_CMD_STOPUPDATE              0x0B



    //写FLASH的数据（0x0C）内部测试命令，不可外泄
    #define COMCP_FRAME_CMD_WFLASH                   0x0C
    #define COMCP_FRAME_WFLASH_LEN                   0x08
    #define COMCP_FRAME_WFLASH_ADDR_POS              (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_WFLASH_ADDR_LEN              4
    #define COMCP_FRAME_WFLASH_SIZE_POS              (COMCP_FRAME_WFLASH_ADDR_POS + COMCP_FRAME_WFLASH_ADDR_LEN)
    #define COMCP_FRAME_WFLASH_SIZE_LEN              1
    #define COMCP_FRAME_WFLASH_DATA_POS              (COMCP_FRAME_WFLASH_SIZE_POS + COMCP_FRAME_WFLASH_SIZE_LEN)
    //每次写入的最长长度
    #define COMCP_FRAME_WFLASH_MAX_SIZE              32

    //擦除FLASH的数据（0x0D）内部测试命令，不可外泄
    #define COMCP_FRAME_CMD_EFLASH                   0x0D
    #define COMCP_FRAME_EFLASH_LEN                   0x07
    #define COMCP_FRAME_EFLASH_ADDR_POS              (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_EFLASH_ADDR_LEN              4
    #define COMCP_FRAME_EFLASH_BCC_POS               (COMCP_FRAME_EFLASH_ADDR_POS + COMCP_FRAME_EFLASH_ADDR_LEN)
    #define COMCP_FRAME_EFLASH_BCC_LEN               1
    #define COMCP_FRAME_EFLASH_TRL_POS               (COMCP_FRAME_EFLASH_BCC_POS + COMCP_FRAME_EFLASH_BCC_LEN)



    //读取EEPROM的数据（0x0E）内部测试命令，不可外泄
    #define COMCP_FRAME_CMD_REEPROM                  0x0E
    #define COMCP_FRAME_REEPROM_LEN                  0x08
    #define COMCP_FRAME_REEPROM_ADDR_POS             (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_REEPROM_ADDR_LEN             4
    #define COMCP_FRAME_REEPROM_SIZE_POS             (COMCP_FRAME_REEPROM_ADDR_POS + COMCP_FRAME_REEPROM_ADDR_LEN)
    #define COMCP_FRAME_REEPROM_SIZE_LEN             1
    #define COMCP_FRAME_REEPROM_BCC_POS              (COMCP_FRAME_REEPROM_SIZE_POS + COMCP_FRAME_REEPROM_SIZE_LEN)
    #define COMCP_FRAME_REEPROM_BCC_LEN              1
    #define COMCP_FRAME_REEPROM_TRL_POS              (COMCP_FRAME_REEPROM_BCC_POS + COMCP_FRAME_REEPROM_BCC_LEN)
    //每次读取的最长长度
    #define COMCP_FRAME_REEPROM_MAX_SIZE              32
    //读取EEPROM的数据响应帧
    #define COMCP_RFRAME_REEPROM_OFLEN               5
    #define COMCP_RFRAME_REEPROM_ODATA_POS           (COMCP_RFRAME_PAR_POS)


    //写EEPROM的数据（0x0F）内部测试命令，不可外泄
    #define COMCP_FRAME_CMD_WEEPROM                   0x0F
    #define COMCP_FRAME_WEEPROM_LEN                   0x08
    #define COMCP_FRAME_WEEPROM_ADDR_POS              (COMCP_FRAME_PAR_POS)
    #define COMCP_FRAME_WEEPROM_ADDR_LEN              4
    #define COMCP_FRAME_WEEPROM_SIZE_POS              (COMCP_FRAME_WEEPROM_ADDR_POS + COMCP_FRAME_WEEPROM_ADDR_LEN)
    #define COMCP_FRAME_WEEPROM_SIZE_LEN              1
    #define COMCP_FRAME_WEEPROM_DATA_POS              (COMCP_FRAME_WEEPROM_SIZE_POS + COMCP_FRAME_WEEPROM_SIZE_LEN)
    //每次写入的最长长度
    #define COMCP_FRAME_WEEPROM_MAX_SIZE              32

    //读取BikeID（0x10）内部测试命令，不可外泄
    #define COMCP_FRAME_CMD_RBIKEID                   0x10
    //读取BikeID的响应帧
    #define COMCP_RFRAME_RBIKEID_OFLEN                17
    #define COMCP_RFRAME_RBIKEID_OBIKEID_POS          (COMCP_RFRAME_PAR_POS)
    #define COMCP_RFRAME_RBIKEID_OBIKEID_LEN          12
    #define COMCP_RFRAME_RBIKEID_OBCC_POS             (COMCP_RFRAME_RBIKEID_OBIKEID_POS + COMCP_RFRAME_RBIKEID_OBIKEID_LEN)
    #define COMCP_RFRAME_RBIKEID_OTRL_POS             (COMCP_RFRAME_RBIKEID_OBCC_POS + 1)
    

//#if defined(_ANYID_STM32_BCTRL_)
    u8 COMCP_SetBikeStateFrame(u8 *pFrame, u8 state);
    #define COMCP_SetBikeStateRspFrame(pFrame) COMCP_OkRspNoParFrame(pFrame, COMCP_FRAME_CMD_SBSTAT)

    
    #define COMCP_ReadBikeLogFrame(pFrame)      COMCP_NoParRequestFrame(pFrame, COMCP_FRAME_CMD_RBLOG)
    u8 COMCP_ReadBikeLogRspFrame(u8 *pFrame, u16 mileage, u16 calorie);


    u8 COMCP_UpdateFrame(u8 *pFrame, u32 index, u8 *pPrograme);
    u8 COMCP_UpdateFRspFrame(u8 *pFrame, u8 errorCode, u32 index);
    #define COMCP_UpdateRspFrame(pFrame) COMCP_OkRspNoParFrame(pFrame, COMCP_FRAME_CMD_UPDATE)


    
    u8 COMCP_RequestUpdateFrame(u8 *pFrame, u8 type, u32 version, u32 size);
    u8 COMCP_RequestUpdateRspFrame(u8 *pFrame, u8 state);

    
    #define COMCP_GetBikeStateFrame(pFrame) COMCP_NoParRequestFrame(pFrame, COMCP_FRAME_CMD_GETSTAT)
    u8 COMCP_GetBikeStateRspFrame(u8 *pFrame, u8 lendState);

    
    #define COMCP_StopUpdateFrame(pFrame)    COMCP_NoParRequestFrame(pFrame, COMCP_FRAME_CMD_STOPUPDATE)
    #define COMCP_StopUpdateRspFrame(pFrame) COMCP_OkRspNoParFrame(pFrame, COMCP_FRAME_CMD_STOPUPDATE)



    #define COMCP_GetUserInfFrame(pFrame)       COMCP_NoParRequestFrame(pFrame, COMCP_FRAME_CMD_GETUSER)
    u8 COMCP_GetUserInfRspFrame(u8 *pFrame, u32 mileage, u32 time, u8 *pUserID);


    u8 COMCP_SetUserInfFrame(u8 *pFrame, u32 time, u8 *pUserID);
    #define COMCP_SetUserInfRspFrame(pFrame)    COMCP_OkRspNoParFrame(pFrame, COMCP_FRAME_CMD_SETUSER)


    #define COMCP_TickFrame(pFrame)       COMCP_NoParRequestFrame(pFrame, COMCP_FRAME_CMD_TICK)
    u8 COMCP_TickRspFrame(u8 *pFrame, u8 *pBikeID);

    u8 COMCP_ReadFlashFrame(u8 *pFrame, u32 address, u8 size);
    u8 COMCP_ReadFlashRspFrame(u8 *pFrame, u8 *pData, u8 size);

    u8 COMCP_WriteFlashFrame(u8 *pFrame, u32 address, u8 size, u8 *pData);
    #define COMCP_WriteFlashRspFrame(pFrame) COMCP_OkRspNoParFrame(pFrame, COMCP_FRAME_CMD_WFLASH)  


    u8 COMCP_EraseFlashFrame(u8 *pFrame, u32 address);
    #define COMCP_EraseFlashRspFrame(pFrame) COMCP_OkRspNoParFrame(pFrame, COMCP_FRAME_CMD_EFLASH)


    u8 COMCP_ReadEEPROMFrame(u8 *pFrame, u32 address, u8 size);
    u8 COMCP_ReadEEPROMRspFrame(u8 *pFrame, u8 *pData, u8 size);

    u8 COMCP_WriteEEPROMFrame(u8 *pFrame, u32 address, u8 size, u8 *pData);
    #define COMCP_WriteEEPROMRspFrame(pFrame) COMCP_OkRspNoParFrame(pFrame, COMCP_FRAME_CMD_WEEPROM) 

    #define COMCP_ReadBikeIDFrame(pFrame)    COMCP_NoParRequestFrame(pFrame, COMCP_FRAME_CMD_RBIKEID)
    u8 COMCP_ReadBikeIDRspFrame(u8 *pFrame, u8 *pBikeID);


    u8 COMCP_FormatUpdateBlockFrame(u8 *pFrame, u8 blockNum);
    u8 COMCP_FormatUpdateBlockRspFrame(u8 *pFrame, u8 formatState);
//#endif

//#if defined(_ANYID_PIC18_BWCOM_)
    
    
    
    
    

    

    
    


    
//#endif



#endif



#endif
