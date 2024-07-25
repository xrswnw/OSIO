#include "AnyID_RFCP.h"


u8 RFCP_RequestFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 reqFlag, u8 *pPar, u8 parLen)
{
    pFrame[RFCP_FRAME_LEN_POS] = parLen + 3;
    pFrame[RFCP_FRAME_ADDR_POS] = filterAddr;
    pFrame[RFCP_FRAME_CMD_POS] = cmd;
    pFrame[RFCP_FRAME_RSP_POS] = reqFlag;

    Sys_MemCpy(pPar, pFrame + RFCP_FRAME_PAR_POS, parLen);

    return (pFrame[RFCP_FRAME_LEN_POS] + 1);
}

u8 RFCP_ReponseFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 *pPar, u8 parLen)
{
    u8 i = 0;

    pFrame[RFCP_FRAME_LEN_POS] = parLen + 3;
    pFrame[RFCP_FRAME_ADDR_POS] = filterAddr;
    pFrame[RFCP_FRAME_CMD_POS] = cmd;
    pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RSP_FLAG;

    Sys_MemCpy(pPar, pFrame + RFCP_FRAME_PAR_POS, parLen);

    return (pFrame[RFCP_FRAME_LEN_POS] + 1);
}


#if (defined(_ANYID_STM32_MTD_) | defined(_ANYID_STM32_MID_) | defined(_ANYID_STM32_MPD_) | defined(_ANYID_STM32_MMD_) | defined(_ANYID_STM32_MCD_))

    u8 RFCP_SightIDFrame(u8 *pFrame, u16 sightID, u16 voltage, u8 power)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_SIGHTID;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_SIGHTID;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_NO_RSP;

        Sys_GetFrameFromData(&pFrame[RFCP_FRAME_SIGHTID_ID_POS], sightID, RFCP_FRAME_SIGHTID_ID_LEN);
        Sys_GetFrameFromData(&pFrame[RFCP_FRAME_SIGHTID_VOL_POS], voltage, RFCP_FRAME_SIGHTID_VOL_LEN);
        pFrame[RFCP_FRAME_SIGHTID_POW_POS] = power;

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_AlarmFrame(u8 *pFrame)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_ALARM;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_ALARM;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_NO_RSP;

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_OFFFrame(u8 *pFrame)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_OFF;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_OFF;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_NO_RSP;

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_LicenceFrame(u8 *pFrame, u8 lg, u8 isRsq)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_LIC;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_LIC;
        pFrame[RFCP_FRAME_RSP_POS] = isRsq;

        if(lg >= RFCP_FRAME_LIC_LG_MAX)
        {
            lg = RFCP_FRAME_LIC_LG_DFT;
        }
        pFrame[RFCP_FRAME_LIC_LG_POS] = lg;

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_LicenceRspFrame(u8 *pFrame, u8 *pCPUID, u16 voltage)
    {
        u8 i = 0;

        pFrame[RFCP_FRAME_LEN_POS] = RFCP_RFRAME_LEN_LIC;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_LIC;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RSP_FLAG;

        for(i = 0; i < RFCP_RFRAME_LIC_CPUID_LEN; i++)
        {
            pFrame[i + RFCP_RFRAME_LIC_CPUID_POS] = pCPUID[i];
        }

        voltage &= RFCP_RFRAME_LIC_MTDVOL_MASK;
        Sys_GetFrameFromData(&pFrame[RFCP_RFRAME_LIC_MTDVOL_POS], voltage, RFCP_RFRAME_LIC_MTDVOL_LEN);

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_RequestLicenceFrame(u8 *pFrame, u8 *pCPUID, u16 licence, u16 pwd)
    {
        u8 i = 0;

        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_RQTLIC;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_RQTLIC;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RQST_RSP;

        for(i = 0; i < RFCP_FRAME_RQTLIC_CPUID_LEN; i++)
        {
            pFrame[i + RFCP_FRAME_RQTLIC_CPUID_POS] = pCPUID[i];
        }

        Sys_GetFrameFromData(&pFrame[RFCP_FRAME_RQTLIC_LICU_POS], licence, RFCP_FRAME_RQTLIC_LICU_LEN);

        Sys_GetFrameFromData(&pFrame[RFCP_FRAME_RQTLIC_PWD_POS], pwd, RFCP_FRAME_RQTLIC_PWD_LEN);

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_RequestLicenceRspFrame(u8 *pFrame, u8 *pCPUID, u16 licence, u16 pwd)
    {

        u8 i = 0;

        pFrame[RFCP_FRAME_LEN_POS] = RFCP_RFRAME_LEN_RQTLIC;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_RQTLIC;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RSP_FLAG;

        for(i = 0; i < RFCP_RFRAME_RQTLIC_CPUID_LEN; i++)
        {
            pFrame[i + RFCP_RFRAME_RQTLIC_CPUID_POS] = pCPUID[i];
        }

        Sys_GetFrameFromData(&pFrame[RFCP_RFRAME_RQTLIC_LIC_POS], licence, RFCP_RFRAME_RQTLIC_LIC_LEN);

        Sys_GetFrameFromData(&pFrame[RFCP_RFRAME_RQTLIC_PWD_POS], pwd, RFCP_RFRAME_RQTLIC_PWD_LEN);

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }


    u8 RFCP_TestLEDFrame(u8 *pFrame)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_TESTLED;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_TESTLED;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_NO_RSP;//RFCP_FRAME_RQST_RSP

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_GetVisionFrame(u8 *pFrame)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_GET_VISION;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_GET_VISION;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RQST_RSP;

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_VisionRspFrame(u8 *pFrame, u8 visionType, u8 xx, u8 yy, u8 zz)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_GET_VISION;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_GET_VISION;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RSP_FLAG;

        pFrame[RFCP_RFRAME_VISION_TYPE_POS] = visionType;
        pFrame[RFCP_RFRAME_VISION_VXX_POS] = xx;
        pFrame[RFCP_RFRAME_VISION_VYY_POS] = yy;
        pFrame[RFCP_RFRAME_VISION_VZZ_POS] = zz;

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }


    u8 RFCP_GetCPUIDFrame(u8 *pFrame)
    {
        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_GET_CPUID;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_GET_CPUID;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RQST_RSP;

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    u8 RFCP_CPUIDRsqFrame(u8 *pFrame, u8 *pCPUUID)
    {
        u8 i = 0;

        pFrame[RFCP_FRAME_LEN_POS] = RFCP_FRAME_LEN_GET_CPUID;
        pFrame[RFCP_FRAME_ADDR_POS] = RFCP_FRAME_DFT_ADDR;
        pFrame[RFCP_FRAME_CMD_POS] = RFCP_FRAME_CMD_GET_CPUID;
        pFrame[RFCP_FRAME_RSP_POS] = RFCP_FRAME_RSP_FLAG;

        for(i = 0; i < RFCP_RFRAME_GETCPUID_LEN; i++)
        {
            pFrame[RFCP_RFRAME_GETCPUID_POS + i] = pCPUUID[i];
        }

        return (pFrame[RFCP_FRAME_LEN_POS] + 1);
    }

    BOOL RFCP_CheckRxFrame(u8 *pFrame, u8 len)
    {
        BOOL b = FALSE;

        u8 cmd = 0;
        u8 crc = 0;
        u8 frameLen = 0;

        crc = pFrame[len + 2];

        if(crc & RFCP_FRAME_CRC_MASK)
        {
            cmd = pFrame[RFCP_FRAME_CMD_POS];
            switch(cmd)
            {
                case RFCP_FRAME_CMD_SIGHTID:
                    frameLen = RFCP_FRAME_LEN_SIGHTID;
                    break;
                case RFCP_FRAME_CMD_ALARM:
                    frameLen = RFCP_FRAME_LEN_ALARM;
                    break;
                case RFCP_FRAME_CMD_OFF:
                    frameLen = RFCP_FRAME_LEN_OFF;
                    break;
                case RFCP_FRAME_CMD_LIC:
#if defined(_ANYID_STM32_MTD_)
                    frameLen = RFCP_FRAME_LEN_LIC;
#elif defined(_ANYID_STM32_MCD_)
                    frameLen = RFCP_RFRAME_LEN_LIC;
#endif
                    break;
                case RFCP_FRAME_CMD_RQTLIC:
                    frameLen = RFCP_FRAME_LEN_RQTLIC;
                    break;
                case RFCP_FRAME_CMD_TESTLED:
                    frameLen = RFCP_FRAME_LEN_TESTLED;
                    break;
                case RFCP_FRAME_CMD_GET_VISION:
                    frameLen = RFCP_FRAME_LEN_GET_VISION;
                    break;
                case RFCP_FRAME_CMD_GET_CPUID:
                    frameLen = RFCP_FRAME_LEN_GET_CPUID;
                    break;
            }

            if(frameLen == len)
            {
                b = TRUE;
            }
        }
        return b;
    }

/*#elif (defined(_ANYID_PIC18_BWCOM_))

    BOOL RFCP_CheckRxFrame(u8 *pFrame, u8 len)
    {
        u8 crc = 0;
        crc = pFrame[len + 2];

        if(crc & RFCP_FRAME_CRC_MASK)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    u8 RFCP_MasteFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 reqFlag, u8 *pPar, u8 parLen, u32 index)
    {
        u8 len = 0;
        
        len = RFCP_RequestFrame(pFrame, filterAddr, cmd, reqFlag, pPar, parLen);
        Sys_GetFrameFromData(pFrame + len, index, 4);

        pFrame[RFCP_FRAME_LEN_POS] += 4;

        return len + 4;
    }

    u8 RFCP_SlaveFrame(u8 *pFrame, u8 filterAddr, u8 cmd, u8 *pPar, u8 parLen, u32 index)
    {
        u8 len = 0;
        
        len = RFCP_ReponseFrame(pFrame, filterAddr, cmd, pPar, parLen);
        Sys_GetFrameFromData(pFrame + len, index, 4);

        pFrame[RFCP_FRAME_LEN_POS] += 4;

        return len + 4;
    }

    u32 RFCP_GetFrameIndex(u8 *pFrame, u8 len)
    {
        u32 index = 0;

        index = Sys_GetDataFromFrame(pFrame + len - 4, 4);

        return index;
    }
*/

#endif


