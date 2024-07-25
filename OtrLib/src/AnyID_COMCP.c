#include "AnyID_COMCP.h"

u8 g_nCOMCPState = COMCP_RXSTAT_IDLE;
u8 g_nCOMCPFrameStart = 0xff;


/******************************************************************************
** 函数名：COMCP_GetCheckBCC
** 说  明: 计算BCC校验值
**
** 参  数：输入：pBuffer：缓冲区的首地址
                 len：缓冲区长度
** 返  回：BCC校验值
******************************************************************************/
u8 COMCP_GetCheckBCC(u8 *pBuffer, u8 len)
{
    u8 i = 0;
    u8 bcc = 0;

    for(i = 0; i < len; i++)
    {
        bcc ^= pBuffer[i];
    }
    bcc = ~bcc;
    return bcc;
}

/******************************************************************************
** 函数名：COMCP_CheckFrame
** 说  明: 检测数据帧是否是正确的帧
**
** 参  数：输入：pFrame：数据帧
** 返  回：TRUE/FALSE
******************************************************************************/
BOOL COMCP_CheckFrame(u8 *pFrame)
{
    BOOL b = FALSE;

    u8 frameLen = 0;
    u8 trail = 0;
    u8 checkBcc = 0;

    frameLen = pFrame[COMCP_FRAME_LEN_POS];
    trail = pFrame[COMCP_FRAME_LEN_POS + frameLen];
    checkBcc = pFrame[COMCP_FRAME_LEN_POS + frameLen - 1];

    //检测帧尾标志是否正确
    if(trail == COMCP_FRAME_TRAIL)
    {
        //检测BCC校验是否正确
        if(checkBcc == COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1))
        {
            b = TRUE;
        }
    }

    return b;
}


/******************************************************************************
** 函数名：COMCP_ErrorRspFrame
** 说  明: 封装错误响应帧
**
** 参  数：输出：pFrame：帧缓冲区首地址
           输入：cmd：请求帧的命令码
                 errorCode：错误码，见阅读器手册
** 返  回：整个数据帧长度（从帧头到帧尾）
******************************************************************************/
u8 COMCP_ErrorRspFrame(u8 *pFrame, u8 cmd, u8 errorCode)
{
    pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
    pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

    pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_FFLEN;
    pFrame[COMCP_RFRAME_CMD_POS] = cmd;

    pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
    pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_FAIL_FLAG;
    pFrame[COMCP_RFRAME_ERRC_POS] = errorCode;

    pFrame[COMCP_RFRAME_FBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_FFLEN - 1);
    pFrame[COMCP_RFRAME_FTRL_POS] = COMCP_FRAME_TRAIL;

    return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
}


u8 COMCP_OkRspNoParFrame(u8 *pFrame, u8 cmd)
{
    pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
    pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

    pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_OFLEN;
    pFrame[COMCP_RFRAME_CMD_POS] = cmd;

    pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
    pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

    pFrame[COMCP_RFRAME_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_OFLEN - 1);
    pFrame[COMCP_RFRAME_OTRL_POS] = COMCP_FRAME_TRAIL;

    return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
}

u8 COMCP_NoParRequestFrame(u8 *pFrame, u8 cmd)
{
    u8 frameLen = 0;

    frameLen = COMCP_FRAME_NOPAR_RQST_LEN;


    pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
    pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

    pFrame[COMCP_FRAME_LEN_POS] = frameLen;
    pFrame[COMCP_FRAME_CMD_POS] = cmd;

    pFrame[COMCP_FRAME_NOPAR_RQST_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
    pFrame[COMCP_FRAME_NOPAR_RQST_TRL_POS] = COMCP_FRAME_TRAIL;

    return frameLen + 3;
}



///////////////////////////////////////////////////////////////////////////////
#if (defined(_ANYID_STM32_MMD_))


#if 0
    u8 COMCP_WriteLICFrame(u8 *pFrame, u8 *pLicence)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_WLIC_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_WLIC;

        Sys_MemCpy(pLicence, pFrame + COMCP_FRAME_WLIC_INF_POS, COMCP_FRAME_WLIC_INF_LEN);

        pFrame[COMCP_FRAME_WLIC_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_WLIC_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    u8 COMCP_WriteLICIDFrame(u8 *pFrame, u8 licID, u8 *pLicence)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_WLICID_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_WLICID;

        Sys_MemCpy(pLicence, pFrame + COMCP_FRAME_WLICID_INF_POS, COMCP_FRAME_WLICID_INF_LEN);
        pFrame[COMCP_FRAME_WLICID_ID_POS] = licID;

        pFrame[COMCP_FRAME_WLICID_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_WLICID_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    u8 COMCP_FormatLICFrame(u8 *pFrame, u8 addr)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_FMT_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_FMT;

        pFrame[COMCP_FRAME_FMT_ADDR_POS] = addr;

        pFrame[COMCP_FRAME_FMT_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_FMT_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    u8 COMCP_ReadLICFrame(u8 *pFrame, u8 *pCPUID)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_RLIC_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_RLIC;

        Sys_MemCpy(pCPUID, pFrame + COMCP_FRAME_RLIC_CPUID_POS, COMCP_FRAME_RLIC_CPUID_LEN);

        pFrame[COMCP_FRAME_RLIC_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_RLIC_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    u8 COMCP_ReadLICIDFrame(u8 *pFrame, u8 licID)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_RLICID_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_RLICID;

        pFrame[COMCP_FRAME_RLICID_LICID_POS] = licID;

        pFrame[COMCP_FRAME_RLICID_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_RLICID_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }
#endif

    u8 COMCP_WriteLICRspFrame(u8 *pFrame)
    {
        u8 frameLen = 0;

        frameLen = COMCP_RFRAME_WLIC_OLEN;

        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_WLIC;
        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_WLIC_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_RFRAME_WLIC_OTRL_POS] = COMCP_FRAME_TRAIL;

        return frameLen + 3;
    }

    u8 COMCP_WriteLICIDRspFrame(u8 *pFrame)
    {
        u8 frameLen = 0;

        frameLen = COMCP_RFRAME_WLICID_OLEN;


        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_WLICID;
        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_WLICID_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_RFRAME_WLICID_OTRL_POS] = COMCP_FRAME_TRAIL;

        return frameLen + 3;
    }


    u8 COMCP_FormatLICRspFrame(u8 *pFrame)
    {
        u8 frameLen = 0;

        frameLen = COMCP_RFRAME_FMT_OLEN;

        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_FMT;
        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_FMT_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_RFRAME_FMT_OTRL_POS] = COMCP_FRAME_TRAIL;
        return frameLen + 3;
    }



    u8 COMCP_ReadLICRspFrame(u8 *pFrame, u8 *pLicence)
    {
        u8 frameLen = 0;
        frameLen = COMCP_RFRAME_RLIC_OLEN;


        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RLIC;
        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        Sys_MemCpy(pLicence, pFrame + COMCP_RFRAME_RLIC_OINF_POS, COMCP_RFRAME_RLIC_OINF_LEN);

        pFrame[COMCP_RFRAME_RLIC_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_RFRAME_RLIC_OTRL_POS] = COMCP_FRAME_TRAIL;

        return frameLen + 3;
    }

    u8 COMCP_ReadLICIDRspFrame(u8 *pFrame, u8 *pLicence)
    {
        u8 frameLen = 0;

        frameLen = COMCP_RFRAME_RLICID_OLEN;


        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RLICID;
        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;


        Sys_MemCpy(pLicence, pFrame + COMCP_RFRAME_RLICID_OINF_POS, COMCP_RFRAME_RLICID_OINF_LEN);

        pFrame[COMCP_RFRAME_RLICID_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_RFRAME_RLICID_OTRL_POS] = COMCP_FRAME_TRAIL;

        return frameLen + 3;
    }
#elif (defined(_ANYID_STM32_MPD_))

    u8 COMCP_FormatMIDInfRspFrame(u8 *pFrame)
    {
         u8 frameLen = 0;

        frameLen = COMCP_RFRAME_FMT_OLEN;

        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_FMT;
        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_FMT_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_RFRAME_FMT_OTRL_POS] = COMCP_FRAME_TRAIL;

        return frameLen + 3;
    }
    u8 COMCP_ReadMIDInfRspFrame(u8 *pFrame, u8 *pInf)
    {
        u8 frameLen = 0;

        frameLen = COMCP_RFRAME_READ_OLEN;


        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_READ;
        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;


        Sys_MemCpy(pInf, pFrame + COMCP_RFRAME_READ_OINF_POS, COMCP_RFRAME_READ_OINF_LEN);

        pFrame[COMCP_RFRAME_READ_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_RFRAME_READ_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

#elif (defined(_ANYID_PIC18_HSR100_))
#if 0
    /******************************************************************************
    ** 函数名：COMCP_ResetReaderFrame
    ** 说  明: 封装“复位阅读”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_ResetReaderFrame(u8 *pFrame)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_RST_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_RST;

        pFrame[COMCP_FRAME_RST_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_RST_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_ControlAntFrame
    ** 说  明: 封装“控制天线”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：antState：天线状态：COMCP_FRAME_CANT_STAT_ON         0x01
                                         COMCP_FRAME_CANT_STAT_OFF        0x00
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_ControlAntFrame(u8 *pFrame, u8 antState)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_CANT_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_CANT;

        pFrame[COMCP_FRAME_CANT_STAT_POS] = antState;

        pFrame[COMCP_FRAME_CANT_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_CANT_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_GetUIDFrame
    ** 说  明: 封装“获取UID”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：rsqtMode：请求模式：COMCP_FRAME_RSQT_MODE_IDLE       0x26
                                         COMCP_FRAME_RSQT_MODE_ALL        0x52
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_GetUIDFrame(u8 *pFrame, u8 rsqtMode)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_GUID_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_GUID;

        pFrame[COMCP_FRAME_GUID_MODE_POS] = rsqtMode;

        pFrame[COMCP_FRAME_GUID_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_GUID_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AutoGetUIDFrame
    ** 说  明: 封装“自动获取UID”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AutoGetUIDFrame(u8 *pFrame)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_AGUID_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_AGUID;

        pFrame[COMCP_FRAME_AGUID_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_AGUID_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_HaltFrame
    ** 说  明: 封装“挂起标签”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_HaltFrame(u8 *pFrame)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_HALT_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_HALT;

        pFrame[COMCP_FRAME_HALT_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_HALT_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthWFrame
    ** 说  明: 封装“授权写数据”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：keyType：密钥类型：COMCP_FRAME_KEYT_A       0x60
                                        COMCP_FRAME_KEYT_B       0x61
                     pPwd：6字节密钥首地址
                     addr：块地址
                     pData：16字节数据首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthWFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr, u8 *pData)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_AUTHW_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHW;

        pFrame[COMCP_FRAME_AUTHW_KEYT_POS] = keyType;
        COMCP_MemCpy(pPwd, pFrame + COMCP_FRAME_AUTHW_PWD_POS, COMCP_FRAME_AUTHW_PWD_LEN);
        pFrame[COMCP_FRAME_AUTHW_ADDR_POS] = addr;
        COMCP_MemCpy(pData, pFrame + COMCP_FRAME_AUTHW_DATA_POS, COMCP_FRAME_AUTHW_DATA_LEN);

        pFrame[COMCP_FRAME_AUTHW_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_AUTHW_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthRFrame
    ** 说  明: 封装“授权读数据”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：keyType：密钥类型：COMCP_FRAME_KEYT_A       0x60
                                        COMCP_FRAME_KEYT_B       0x61
                     pPwd：6字节密钥首地址
                     addr：块地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthRFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_AUTHR_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHR;

        pFrame[COMCP_FRAME_AUTHR_KEYT_POS] = keyType;
        COMCP_MemCpy(pPwd, pFrame + COMCP_FRAME_AUTHR_PWD_POS, COMCP_FRAME_AUTHR_PWD_LEN);
        pFrame[COMCP_FRAME_AUTHR_ADDR_POS] = addr;

        pFrame[COMCP_FRAME_AUTHR_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_AUTHR_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthWValueFrame
    ** 说  明: 封装“授权写值”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：keyType：密钥类型：COMCP_FRAME_KEYT_A       0x60
                                        COMCP_FRAME_KEYT_B       0x61
                     pPwd：6字节密钥首地址
                     addr：块地址
                     pValue：4字节数据块（高字节数据保存在高地址）
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthWValueFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr, u8 *pValue)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_AUTHWV_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHWV;

        pFrame[COMCP_FRAME_AUTHWV_KEYT_POS] = keyType;
        COMCP_MemCpy(pPwd, pFrame + COMCP_FRAME_AUTHWV_PWD_POS, COMCP_FRAME_AUTHWV_PWD_LEN);
        pFrame[COMCP_FRAME_AUTHWV_ADDR_POS] = addr;
        COMCP_MemCpy(pValue, pFrame + COMCP_FRAME_AUTHWV_VALUE_POS, COMCP_FRAME_AUTHWV_VALUE_LEN);

        pFrame[COMCP_FRAME_AUTHWV_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_AUTHWV_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthRValueFrame
    ** 说  明: 封装“授权读值”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：keyType：密钥类型：COMCP_FRAME_KEYT_A       0x60
                                        COMCP_FRAME_KEYT_B       0x61
                     pPwd：6字节密钥首地址
                     addr：块地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthRValueFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 addr)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_AUTHRV_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHRV;

        pFrame[COMCP_FRAME_AUTHRV_KEYT_POS] = keyType;
        COMCP_MemCpy(pPwd, pFrame + COMCP_FRAME_AUTHRV_PWD_POS, COMCP_FRAME_AUTHRV_PWD_LEN);
        pFrame[COMCP_FRAME_AUTHRV_ADDR_POS] = addr;

        pFrame[COMCP_FRAME_AUTHRV_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_AUTHRV_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthValueFrame
    ** 说  明: 封装“授权值”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：keyType：密钥类型：COMCP_FRAME_KEYT_A       0x60
                                        COMCP_FRAME_KEYT_B       0x61
                     pPwd：6字节密钥首地址
                     addr：Value操作的地址
                     valueMode：Value操作模式：COMCP_FRAME_VALUE_MODE_DEC      0xC0
                                               COMCP_FRAME_VALUE_MODE_INC      0xC1
                                               COMCP_FRAME_VALUE_MODE_RSTOR    0xC2
                                               COMCP_FRAME_VALUE_MODE_TRANS    0xB0
                     transAddr：Value操作的传送地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthValueFrame(u8 *pFrame, u8 keyType, u8 *pPwd, u8 valueMode, u8 addr, u8 *pValue, u8 transAddr)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_AUTHV_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHV;

        pFrame[COMCP_FRAME_AUTHV_KEYT_POS] = keyType;
        COMCP_MemCpy(pPwd, pFrame + COMCP_FRAME_AUTHV_PWD_POS, COMCP_FRAME_AUTHV_PWD_LEN);
        pFrame[COMCP_FRAME_AUTHV_MODE_POS] = valueMode;
        pFrame[COMCP_FRAME_AUTHV_ADDR_POS] = addr;
        COMCP_MemCpy(pValue, pFrame + COMCP_FRAME_AUTHV_VALUE_POS, COMCP_FRAME_AUTHV_VALUE_LEN);
        pFrame[COMCP_FRAME_AUTHV_TADDR_POS] = transAddr;

        pFrame[COMCP_FRAME_AUTHV_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_AUTHV_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_WFrame
    ** 说  明: 封装“写数据”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：块地址
                     pData：16字节数据
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_WFrame(u8 *pFrame, u8 addr, u8 *pData)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_W_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_W;

        pFrame[COMCP_FRAME_W_ADDR_POS] = addr;
        COMCP_MemCpy(pData, pFrame + COMCP_FRAME_W_DATA_POS, COMCP_FRAME_W_DATA_LEN);

        pFrame[COMCP_FRAME_W_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_W_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_RFrame
    ** 说  明: 封装“读数据”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：块地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_RFrame(u8 *pFrame, u8 addr)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_R_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_R;

        pFrame[COMCP_FRAME_R_ADDR_POS] = addr;

        pFrame[COMCP_FRAME_R_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_R_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_WValueFrame
    ** 说  明: 封装“写值”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：块地址
                     pValue：4字节的Value首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_WValueFrame(u8 *pFrame, u8 addr, u8 *pValue)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_WV_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_WV;

        pFrame[COMCP_FRAME_WV_ADDR_POS] = addr;
        COMCP_MemCpy(pValue, pFrame + COMCP_FRAME_WV_VALUE_POS, COMCP_FRAME_WV_VALUE_LEN);

        pFrame[COMCP_FRAME_WV_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_WV_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_RValueFrame
    ** 说  明: 封装“读值”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：块地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_RValueFrame(u8 *pFrame, u8 addr)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_RV_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_RV;

        pFrame[COMCP_FRAME_RV_ADDR_POS] = addr;

        pFrame[COMCP_FRAME_RV_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_RV_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    /******************************************************************************
    ** 函数名：COMCP_AuthValueFrame
    ** 说  明: 封装“授权值”命令帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：Value操作的地址
                     valueMode：Value操作模式：COMCP_FRAME_VALUE_MODE_DEC      0xC0
                                               COMCP_FRAME_VALUE_MODE_INC      0xC1
                                               COMCP_FRAME_VALUE_MODE_RSTOR    0xC2
                                               COMCP_FRAME_VALUE_MODE_TRANS    0xB0
                     transAddr：Value操作的传送地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_ValueFrame(u8 *pFrame, u8 valueMode, u8 addr, u8 *pValue, u8 transAddr)
    {
        u8 frameLen = 0;
        u8 i = 0;

        frameLen = COMCP_FRAME_V_FLEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHV;

        pFrame[COMCP_FRAME_V_MODE_POS] = valueMode;
        pFrame[COMCP_FRAME_V_ADDR_POS] = addr;
        COMCP_MemCpy(pValue, pFrame + COMCP_FRAME_V_VALUE_POS, COMCP_FRAME_V_VALUE_LEN);
        pFrame[COMCP_FRAME_V_TADDR_POS] = transAddr;

        pFrame[COMCP_FRAME_V_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_V_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }
#endif
    /******************************************************************************
    ** 函数名：COMCP_ResetReaderORspFrame
    ** 说  明: 封装“复位阅读器”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_ResetReaderORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_RST_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RST;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_RST_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_RST_OFLEN - 1);
        pFrame[COMCP_RFRAME_RST_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_ControlAntORspFrame
    ** 说  明: 封装“控制天线”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_ControlAntORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_CANT_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_CANT;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_CANT_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_CANT_OFLEN - 1);
        pFrame[COMCP_RFRAME_CANT_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_GetUIDORspFrame
    ** 说  明: 封装“获取UID”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：cardTypeH：标签类型高字节
                     cardTypeL：标签类型低字节
                     uidLen：UID码长度：4/7/10字节
                     pUID：UID数据首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_GetUIDORspFrame(u8 *pFrame, u8 cardTypeH, u8 cardTypeL, u8 uidLen, u8 *pUID)
    {
        u8 frameLen = 0;

        frameLen = COMCP_RFRAME_GUID_OFLEN + uidLen + COMCP_RFRAME_GUID_OCTYPE_LEN + COMCP_RFRAME_GUID_IDLEN_LEN;
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_GUID;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_GUID_OCTYPE_POS] = cardTypeH;
        pFrame[COMCP_RFRAME_GUID_OCTYPE_POS + 1] = cardTypeL;
        pFrame[COMCP_RFRAME_GUID_IDLEN_POS] = uidLen;
        Sys_MemCpy(pUID, pFrame + COMCP_RFRAME_GUID_OUID_POS, uidLen);

        pFrame[frameLen + 3 - 2] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[frameLen + 3 - 1] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AGetUIDORspFrame
    ** 说  明: 封装“自动获取UID”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：cardTypeH：标签类型高字节
                     cardTypeL：标签类型低字节
                     uidLen：UID码长度：4/7/10字节
                     pUID：UID数据首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AGetUIDORspFrame(u8 *pFrame, u8 cardTypeH, u8 cardTypeL, u8 uidLen, u8 *pUID)
    {
        u8 frameLen = 0;

        frameLen = COMCP_RFRAME_AGUID_OFLEN + uidLen + COMCP_RFRAME_AGUID_OCTYPE_LEN + COMCP_RFRAME_AGUID_IDLEN_LEN;
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = frameLen;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_AGUID;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_AGUID_OCTYPE_POS] = cardTypeH;
        pFrame[COMCP_RFRAME_AGUID_OCTYPE_POS + 1] = cardTypeL;
        pFrame[COMCP_RFRAME_AGUID_IDLEN_POS] = uidLen;
        Sys_MemCpy(pUID, pFrame + COMCP_RFRAME_AGUID_OUID_POS, uidLen);

        pFrame[frameLen + 3 - 2] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[frameLen + 3 - 1] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_HaltORspFrame
    ** 说  明: 封装“HALT”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_HaltORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_HALT_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_HALT;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_HALT_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_HALT_OFLEN - 1);
        pFrame[COMCP_RFRAME_HALT_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthWORspFrame
    ** 说  明: 封装“授权写数据”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthWORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_AUTHW_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHW;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_AUTHW_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_AUTHW_OFLEN - 1);
        pFrame[COMCP_RFRAME_AUTHW_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthRORspFrame
    ** 说  明: 封装“授权读数据”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：数据块地址
                     pData：16字节数据首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthRORspFrame(u8 *pFrame, u8 addr, u8 *pData)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_AUTHR_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHR;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_AUTHR_OADDR_POS] = addr;
        Sys_MemCpy(pData, pFrame + COMCP_RFRAME_AUTHR_ODATA_POS, COMCP_RFRAME_AUTHR_ODATA_LEN);


        pFrame[COMCP_RFRAME_AUTHR_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_AUTHR_OFLEN - 1);
        pFrame[COMCP_RFRAME_AUTHR_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthWVORspFrame
    ** 说  明: 封装“授权写值”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthWVORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_AUTHWV_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHWV;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_AUTHWV_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_AUTHWV_OFLEN - 1);
        pFrame[COMCP_RFRAME_AUTHWV_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthRVORspFrame
    ** 说  明: 封装“授权读值”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：数据块地址
                     pValue：4字节值首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthRVORspFrame(u8 *pFrame, u8 addr, u8 *pValue)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_AUTHRV_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHRV;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_AUTHRV_OADDR_POS] = addr;
        Sys_MemCpy(pValue, pFrame + COMCP_RFRAME_AUTHRV_OVALUE_POS, COMCP_RFRAME_AUTHRV_OVALUE_LEN);


        pFrame[COMCP_RFRAME_AUTHRV_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_AUTHRV_OFLEN - 1);
        pFrame[COMCP_RFRAME_AUTHRV_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_AuthVORspFrame
    ** 说  明: 封装“授权值操作”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_AuthVORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_AUTHV_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_AUTHV;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_AUTHV_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_AUTHV_OFLEN - 1);
        pFrame[COMCP_RFRAME_AUTHV_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_WORspFrame
    ** 说  明: 封装“写数据操作”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_WORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_W_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_W;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_W_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_W_OFLEN - 1);
        pFrame[COMCP_RFRAME_W_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_RORspFrame
    ** 说  明: 封装“读数据”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：数据块地址
                     pData：16字节数据首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_RORspFrame(u8 *pFrame, u8 addr, u8 *pData)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_R_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_R;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_R_OADDR_POS] = addr;
        Sys_MemCpy(pData, pFrame + COMCP_RFRAME_R_ODATA_POS, COMCP_RFRAME_R_ODATA_LEN);


        pFrame[COMCP_RFRAME_R_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_R_OFLEN - 1);
        pFrame[COMCP_RFRAME_R_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_WVORspFrame
    ** 说  明: 封装“写值”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_WVORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_WV_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_WV;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_WV_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_WV_OFLEN - 1);
        pFrame[COMCP_RFRAME_WV_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }
    /******************************************************************************
    ** 函数名：COMCP_RVORspFrame
    ** 说  明: 封装“读值”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
               输入：addr：数据块地址
                     pValue：4字节值首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_RVORspFrame(u8 *pFrame, u8 addr, u8 *pValue)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_RV_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RV;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_RV_OADDR_POS] = addr;
        Sys_MemCpy(pValue, pFrame + COMCP_RFRAME_RV_OVALUE_POS, COMCP_RFRAME_RV_OVALUE_LEN);


        pFrame[COMCP_RFRAME_RV_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_RV_OFLEN - 1);
        pFrame[COMCP_RFRAME_RV_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    /******************************************************************************
    ** 函数名：COMCP_VORspFrame
    ** 说  明: 封装“值操作”请求帧的成功响应帧
    **
    ** 参  数：输出：pFrame：帧缓冲区首地址
    ** 返  回：整个数据帧长度（从帧头到帧尾）
    ******************************************************************************/
    u8 COMCP_VORspFrame(u8 *pFrame)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_V_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_V;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_V_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_V_OFLEN - 1);
        pFrame[COMCP_RFRAME_V_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

#elif (defined(_ANYID_STM32_BCTRL_) | defined(_ANYID_PIC18_BWCOM_))

//#if defined(_ANYID_STM32_BCTRL_)
    u8 COMCP_UpdateFRspFrame(u8 *pFrame, u8 errorCode, u32 index)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_UPDATE_FFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_UPDATE;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_FAIL_FLAG;
        pFrame[COMCP_RFRAME_ERRC_POS] = errorCode;

        Sys_GetFrameFromData(pFrame + COMCP_RFRAME_UPDATE_FINDEX_POS, index, COMCP_RFRAME_UPDATE_FINDEX_LEN);

        pFrame[COMCP_RFRAME_UPDATE_FBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], pFrame[COMCP_RFRAME_LEN_POS] - 1);
        pFrame[COMCP_RFRAME_UPDATE_FTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    u8 COMCP_ReadBikeLogRspFrame(u8 *pFrame, u16 mileage, u16 calorie)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_RBLOG_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RBLOG;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        Sys_GetFrameFromData(pFrame + COMCP_RFRAME_RBLOG_MILEAGE_POS, mileage, COMCP_RFRAME_RBLOG_MILEAGE_LEN);
        Sys_GetFrameFromData(pFrame + COMCP_RFRAME_RBLOG_CALORIE_POS, calorie, COMCP_RFRAME_RBLOG_CALORIE_LEN);


        pFrame[COMCP_RFRAME_RBLOG_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_RBLOG_OFLEN - 1);
        pFrame[COMCP_RFRAME_RBLOG_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    u8 COMCP_RequestUpdateRspFrame(u8 *pFrame, u8 state)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_RQTUPDATE_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RQTUPDATE;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_RQTUPDATE_OSTATE_POS] = state;


        pFrame[COMCP_RFRAME_RQTUPDATE_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_RQTUPDATE_OFLEN - 1);
        pFrame[COMCP_RFRAME_RQTUPDATE_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);

    }



//#endif

//#if defined(_ANYID_PIC18_BWCOM_)
    u8 COMCP_SetBikeStateFrame(u8 *pFrame, u8 state)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_SBSTAT_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_SBSTAT;

        pFrame[COMCP_FRAME_SBSTAT_STAT_POS] = state;

        pFrame[COMCP_FRAME_SBSTAT_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_SBSTAT_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    u8 COMCP_UpdateFrame(u8 *pFrame, u32 index, u8 *pPrograme)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_UPDATE_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_UPDATE;

        Sys_GetFrameFromData(pFrame + COMCP_FRAME_UPDATE_INDEX_POS, index, COMCP_FRAME_UPDATE_INDEX_LEN);
        Sys_MemCpy(pPrograme, pFrame + COMCP_FRAME_UPDATE_DATA_POS, COMCP_FRAME_UPDATE_DATA_LEN);

        pFrame[COMCP_FRAME_UPDATE_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_UPDATE_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    u8 COMCP_RequestUpdateFrame(u8 *pFrame, u8 type, u32 num, u32 size)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_RQTUPDATE_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_RQTUPDATE;

        pFrame[COMCP_FRAME_RQTUPDATE_TYPE_POS] = type;
        Sys_GetFrameFromData(pFrame + COMCP_FRAME_RQTUPDATE_NUM_POS, num, COMCP_FRAME_RQTUPDATE_NUM_LEN);
        Sys_GetFrameFromData(pFrame + COMCP_FRAME_RQTUPDATE_SIZE_POS, size, COMCP_FRAME_RQTUPDATE_SIZE_LEN);

        pFrame[COMCP_FRAME_RQTUPDATE_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_RQTUPDATE_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    u8 COMCP_GetBikeStateRspFrame(u8 *pFrame, u8 lendState)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_GETSTAT_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_GETSTAT;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_GETSTAT_OLNDSTAT_POS] = lendState;

        pFrame[COMCP_RFRAME_GETSTAT_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_GETSTAT_OFLEN - 1);
        pFrame[COMCP_RFRAME_GETSTAT_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }


    u8 COMCP_GetUserInfRspFrame(u8 *pFrame, u32 mileage, u32 time, u8 *pUserID)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_GETUSER_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_GETUSER;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        Sys_GetFrameFromData(pFrame + COMCP_RFRAME_GETUSER_OMILEAGE_POS, mileage, COMCP_RFRAME_GETUSER_OMILEAGE_LEN);
        Sys_GetFrameFromData(pFrame + COMCP_RFRAME_GETUSER_OTIME_POS, time, COMCP_RFRAME_GETUSER_OTIME_LEN);
        Sys_MemCpy(pUserID, pFrame + COMCP_RFRAME_GETUSER_OID_POS, COMCP_RFRAME_GETUSER_OID_LEN);

        pFrame[COMCP_RFRAME_GETUSER_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], COMCP_RFRAME_GETUSER_OFLEN - 1);
        pFrame[COMCP_RFRAME_GETUSER_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }


    u8 COMCP_SetUserInfFrame(u8 *pFrame, u32 time, u8 *pUserID)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_SETUSER_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_SETUSER;

        Sys_GetFrameFromData(pFrame + COMCP_FRAME_SETUSER_TIME_POS, time, COMCP_FRAME_SETUSER_TIME_LEN);
        Sys_MemCpy(pUserID, pFrame + COMCP_FRAME_SETUSER_ID_POS, COMCP_FRAME_SETUSER_ID_LEN);

        pFrame[COMCP_FRAME_SETUSER_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_SETUSER_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    u8 COMCP_TickRspFrame(u8 *pFrame, u8 *pBikeID)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_TICK_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_TICK;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        Sys_MemCpy(pBikeID, pFrame + COMCP_RFRAME_TICK_OBIKEID_POS, COMCP_RFRAME_TICK_OBIKEID_LEN);

        pFrame[COMCP_RFRAME_TICK_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], pFrame[COMCP_RFRAME_LEN_POS] - 1);
        pFrame[COMCP_RFRAME_TICK_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    u8 COMCP_ReadFlashFrame(u8 *pFrame, u32 address, u8 size)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_RFLASH_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_RFLASH;

        Sys_GetFrameFromData(pFrame + COMCP_FRAME_RFLASH_ADDR_POS, address, COMCP_FRAME_RFLASH_ADDR_LEN);
        pFrame[COMCP_FRAME_RFLASH_SIZE_POS] = size;

        pFrame[COMCP_FRAME_RFLASH_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_RFLASH_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    u8 COMCP_ReadFlashRspFrame(u8 *pFrame, u8 *pData, u8 size)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_RFLASH_OFLEN + size;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RFLASH;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        Sys_MemCpy(pData, pFrame + COMCP_RFRAME_RFLASH_ODATA_POS, size);

        pFrame[COMCP_RFRAME_RFLASH_ODATA_POS + size] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], pFrame[COMCP_RFRAME_LEN_POS] - 1);
        pFrame[COMCP_RFRAME_RFLASH_ODATA_POS + size + 1] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }


    u8 COMCP_WriteFlashFrame(u8 *pFrame, u32 address, u8 size, u8 *pData)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_WFLASH_LEN + size;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_WFLASH;

        Sys_GetFrameFromData(pFrame + COMCP_FRAME_WFLASH_ADDR_POS, address, COMCP_FRAME_WFLASH_ADDR_LEN);
        pFrame[COMCP_FRAME_WFLASH_SIZE_POS] = size;
        Sys_MemCpy(pData, pFrame + COMCP_FRAME_WFLASH_DATA_POS, size);

        pFrame[COMCP_FRAME_WFLASH_DATA_POS + size] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_WFLASH_DATA_POS + size + 1] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    u8 COMCP_EraseFlashFrame(u8 *pFrame, u32 address)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_EFLASH_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_EFLASH;

        Sys_GetFrameFromData(pFrame + COMCP_FRAME_EFLASH_ADDR_POS, address, COMCP_FRAME_EFLASH_ADDR_LEN);

        pFrame[COMCP_FRAME_EFLASH_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_EFLASH_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    u8 COMCP_ReadEEPROMFrame(u8 *pFrame, u32 address, u8 size)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_REEPROM_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_REEPROM;

        Sys_GetFrameFromData(pFrame + COMCP_FRAME_REEPROM_ADDR_POS, address, COMCP_FRAME_REEPROM_ADDR_LEN);
        pFrame[COMCP_FRAME_REEPROM_SIZE_POS] = size;

        pFrame[COMCP_FRAME_REEPROM_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_REEPROM_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }

    u8 COMCP_ReadEEPROMRspFrame(u8 *pFrame, u8 *pData, u8 size)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_REEPROM_OFLEN + size;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_REEPROM;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        Sys_MemCpy(pData, pFrame + COMCP_RFRAME_REEPROM_ODATA_POS, size);

        pFrame[COMCP_RFRAME_REEPROM_ODATA_POS + size] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], pFrame[COMCP_RFRAME_LEN_POS] - 1);
        pFrame[COMCP_RFRAME_REEPROM_ODATA_POS + size + 1] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    u8 COMCP_WriteEEPROMFrame(u8 *pFrame, u32 address, u8 size, u8 *pData)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_WFLASH_LEN + size;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_WEEPROM;

        Sys_GetFrameFromData(pFrame + COMCP_FRAME_WEEPROM_ADDR_POS, address, COMCP_FRAME_WEEPROM_ADDR_LEN);
        pFrame[COMCP_FRAME_WEEPROM_SIZE_POS] = size;
        Sys_MemCpy(pData, pFrame + COMCP_FRAME_WEEPROM_DATA_POS, size);

        pFrame[COMCP_FRAME_WEEPROM_DATA_POS + size] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_WEEPROM_DATA_POS + size + 1] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }


    u8 COMCP_ReadBikeIDRspFrame(u8 *pFrame, u8 *pBikeID)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_RBIKEID_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_RBIKEID;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        Sys_MemCpy(pBikeID, pFrame + COMCP_RFRAME_RBIKEID_OBIKEID_POS, COMCP_RFRAME_RBIKEID_OBIKEID_LEN);

        pFrame[COMCP_RFRAME_RBIKEID_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], pFrame[COMCP_RFRAME_LEN_POS] - 1);
        pFrame[COMCP_RFRAME_RBIKEID_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }

    u8 COMCP_FormatUpdateBlockFrame(u8 *pFrame, u8 blockNum)
    {
        u8 frameLen = 0;

        frameLen = COMCP_FRAME_FORMAT_LEN;

        pFrame[COMCP_FRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_FRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_FRAME_LEN_POS] = frameLen;
        pFrame[COMCP_FRAME_CMD_POS] = COMCP_FRAME_CMD_FORMAT;

        pFrame[COMCP_FRAME_FORMAT_NUM_POS] = blockNum;

        pFrame[COMCP_FRAME_FORMAT_BCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], frameLen - 1);
        pFrame[COMCP_FRAME_FORMAT_TRL_POS] = COMCP_FRAME_TRAIL;

        return (frameLen + 3);
    }
    u8 COMCP_FormatUpdateBlockRspFrame(u8 *pFrame, u8 formatState)
    {
        pFrame[COMCP_RFRAME_HEAD1_POS] = COMCP_FRAME_HEAD1;
        pFrame[COMCP_RFRAME_HEAD2_POS] = COMCP_FRAME_HEAD2;

        pFrame[COMCP_RFRAME_LEN_POS] = COMCP_RFRAME_FORMAT_OFLEN;
        pFrame[COMCP_RFRAME_CMD_POS] = COMCP_FRAME_CMD_FORMAT;

        pFrame[COMCP_RFRAME_RSPF_POS] = COMCP_RFRAME_RSP_FLAG;
        pFrame[COMCP_RFRAME_OKF_POS] = COMCP_RFRAME_OK_FLAG;

        pFrame[COMCP_RFRAME_FORMAT_OSTATE_POS] = formatState;

        pFrame[COMCP_RFRAME_FORMAT_OBCC_POS] = COMCP_GetCheckBCC(&pFrame[COMCP_FRAME_LEN_POS], pFrame[COMCP_RFRAME_LEN_POS] - 1);
        pFrame[COMCP_RFRAME_FORMAT_OTRL_POS] = COMCP_FRAME_TRAIL;

        return (pFrame[COMCP_RFRAME_LEN_POS] + 3);
    }
//#endif


#endif


