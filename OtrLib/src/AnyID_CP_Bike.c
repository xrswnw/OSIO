#include "AnyID_CP_Bike.h"

u8 i = 0;

//CP_BikeGIDRSP g_sGetIDRspFrame = {0, {0}};
#if CP_BIKE_SLAVER
    //获取BikeID和故障信息响应帧
    u8 CP_BikeGetIDRspFrame(u8 * pFrame, u8 state, u8 bikeErrCode, u8 *pBikeID)
    {
        CP_BikeGIDRSP *p = NULL;
        u8 len = 0;

        if(state == CP_FRAME_STATE_RESPONE_OK)
        {
            p = (CP_BikeGIDRSP *)(pFrame + CP_FRAME_DSTART_POS);
            p->errCode = bikeErrCode;
            a_memcpy((const void *)pBikeID, p->id, CP_BIKE_ID_LEN);

            len = sizeof(CP_BikeGIDRSP);
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_ID, state, pFrame, len);
        }
        else
        {
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_ID, state, pFrame, CP_FRAME_NO_DATA);
        }


        return len;
    }
#endif


//设置BikeID的请求帧
#if CP_BIKE_MASTER
     u8 CP_BikeSetIDReqFrame(u8 * pFrame, u8 bikeErrCode, u8 *pBikeID)
    {
        CP_BikeSIDREQ *p = NULL;
        u8 len = 0;

        p = (CP_BikeSIDREQ *)(pFrame + CP_FRAME_DSTART_POS);
        p->errCode = bikeErrCode;
        a_memcpy((const void *)pBikeID, p->id, CP_BIKE_ID_LEN);

        len = sizeof(CP_BikeSIDREQ);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_SET_ID, CP_FRAME_STATE_REQUEST, pFrame, len);

        return len;
    }
#endif

//响应帧
#if CP_BIKE_SLAVER
    u8 CP_BikeSetIDRspFrame(u8 * pFrame, u8 state, u8 bikeState)
    {
        u8 len = 0;

        if(state == CP_FRAME_STATE_RESPONE_OK)
        {
            *(pFrame + CP_BIKE_BSTAT_POS) = bikeState;
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_SET_ID, state, pFrame, CP_BIKE_BSTAT_LEN);
        }
        else
        {
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_SET_ID, state, pFrame, CP_FRAME_NO_DATA);
        }

        return len;
    }
#endif






#if CP_BIKE_MASTER
    //设置Bike借/还状态请求帧
    u8 CP_BikeSetStateReqFrame(u8 *pFrame, u8 state, u8 *pTimes)
    {
        u8 len = 0;

        *(pFrame + CP_BIKE_BSTAT_POS) = state;
        a_memcpy(pTimes, pFrame + CP_BIKE_TIME_POS, CP_BIKE_TIME_LEN);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_SET_STAT, CP_FRAME_STATE_REQUEST, pFrame, CP_BIKE_BSTAT_LEN + CP_BIKE_TIME_LEN);

        return len;
    }
#endif



#if CP_BIKE_SLAVER
    //读取Bike行驶信息应答帧
    u8 CP_BikeGetRunInfRspFrame(u8 * pFrame, u8 state, u32 mileage, u32 totalMileage, u32 reduceC, u32 totalReduceC)
    {
        CP_BikeGRunInfRSP *p = NULL;
        u8 len = 0;

        if(state == CP_FRAME_STATE_RESPONE_OK)
        {
            p = (CP_BikeGRunInfRSP *)(pFrame + CP_FRAME_DSTART_POS);
            p->mileage = mileage;
            p->totalMileage = totalMileage;
            p->reduceC = reduceC;
            p->totalReduceC = totalReduceC;

            len = sizeof(CP_BikeGRunInfRSP);
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_RUN_INF, state, pFrame, len);
        }
        else
        {
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_RUN_INF, state, pFrame, CP_FRAME_NO_DATA);
        }

        return len;
    }
#endif


#if CP_BIKE_MASTER
    //设置Bike行驶信息请求帧
    u8 CP_BikeSetRunInfReqFrame(u8 * pFrame, u32 mileage, u32 totalMileage, u32 reduceC, u32 totalReduceC)
    {
        CP_BikeSRunInfREQ *p = NULL;
        u8 len = 0;

        p = (CP_BikeSRunInfREQ *)(pFrame + CP_FRAME_DSTART_POS);
        p->mileage = mileage;
        p->reduceC = reduceC;
        p->totalMileage = totalMileage;
        p->totalReduceC = totalReduceC;

        len = sizeof(CP_BikeSRunInfREQ);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_SET_RUN_INF, CP_FRAME_STATE_REQUEST, pFrame, len);

        return len;
    }
#endif



#if CP_BIKE_MASTER
    //设置Bike交易信息请求帧
    u8 CP_BikeSetTradeInfReqFrame(u8 *pFrame, u8 *pTradeTime, u8 *pUserID, u8 *pPoleID)
    {
        u8 len = 0;

        CP_BikeSTradeInfREQ *p = NULL;

        p = (CP_BikeSTradeInfREQ *)(pFrame + CP_FRAME_DSTART_POS);
        a_memcpy((const void *)pTradeTime, p->uTradeTime.tradeTime, CP_BIKE_TRADE_TIME_LEN);
        a_memcpy((const void *)pUserID, p->userID, CP_BIKE_USER_ID_LEN);
        a_memcpy((const void *)pPoleID, p->poleID, CP_BIKE_POLE_ID_LEN);

        len = sizeof(CP_BikeSTradeInfREQ);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_SET_TRADE_INF, CP_FRAME_STATE_REQUEST, pFrame, len);

        return len;
    }
#endif


#if CP_BIKE_SLAVER
    //获取Bike交易信息的应答帧
    u8 CP_BikeGetTradeInfRspFrame(u8 *pFrame, u8 state, u8 *pTradeTime, u8 *pUserID, u8 *pPoleID)
    {
        CP_BikeGTradeInfRSP *p = NULL;
        u8 len = 0;

        if(state == CP_FRAME_STATE_RESPONE_OK)
        {
            p = (CP_BikeGTradeInfRSP *)(pFrame + CP_FRAME_DSTART_POS);
            a_memcpy((const void *)pTradeTime, p->uTradeTime.tradeTime, CP_BIKE_TRADE_TIME_LEN);
            a_memcpy((const void *)pUserID, p->userID, CP_BIKE_USER_ID_LEN);
            a_memcpy((const void *)pPoleID, p->poleID, CP_BIKE_POLE_ID_LEN);

            len = sizeof(CP_BikeGTradeInfRSP);
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_TRADE_INF, state, pFrame, len);
        }
        else
        {
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_TRADE_INF, state, pFrame, CP_FRAME_NO_DATA);
        }

        return len;
    }
#endif



#if CP_BIKE_MASTER
    //设置Bike交易信息和状态请求帧
    u8 CP_BikeSetStatTrdInfReqFrame(u8 *pFrame, u8 *pTradeTime, u8 *pUserID, u8 *pPoleID, u8 bikeState)
    {
        u8 len = 0;

        CP_BikeSStatAndTradeInfREQ *p = NULL;

        p = (CP_BikeSStatAndTradeInfREQ *)(pFrame + CP_FRAME_DSTART_POS);
        a_memcpy((const void *)pTradeTime, p->tradeInf.uTradeTime.tradeTime, CP_BIKE_TRADE_TIME_LEN);
        a_memcpy((const void *)pUserID, p->tradeInf.userID, CP_BIKE_USER_ID_LEN);
        a_memcpy((const void *)pPoleID, p->tradeInf.poleID, CP_BIKE_POLE_ID_LEN);
        p->state = bikeState;

        len = sizeof(CP_BikeSStatAndTradeInfREQ);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_SET_STAT_AND_TRDINF, CP_FRAME_STATE_REQUEST, pFrame, len);

        return len;
    }
#endif

/*
#if CP_BIKE_SLAVER
    //获取Bike交易信息和状态的应答帧
    u8 CP_BikeGetStatTrdInfRspFrame(u8 *pFrame, u8 state, u8 *pTradeTime, u8 *pUserID, u8 bikeState)
    {
        CP_BikeGStatAndTradeInfRSP *p = NULL;
        u8 len = 0;

        if(state == CP_FRAME_STATE_RESPONE_OK)
        {
            p = (CP_BikeGStatAndTradeInfRSP *)(pFrame + CP_FRAME_DSTART_POS);
            a_memcpy(pTradeTime, p->tradeInf.uTradeTime.tradeTime, CP_BIKE_TRADE_TIME_LEN);
            a_memcpy(pUserID, p->tradeInf.userID, CP_BIKE_USER_ID_LEN);
            p->state = bikeState;

            len = sizeof(CP_BikeGTradeInfRSP);
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_STAT_AND_TRDINF, state, pFrame, len);
        }
        else
        {
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_STAT_AND_TRDINF, state, pFrame, CP_FRAME_NO_DATA);
        }

        return len;
    }
#endif
*/

#if CP_BIKE_SLAVER
    //获取Bike信息
    u8 CP_BikeGetInfRspFrame(u8 *pFrame, u8 state, u8 bikeState, CP_BikeIDInf *pSIDInf, CP_BikeTradeInf *pSTradeInf, CP_BikeRunInf *pSRunInf)
    {
        u8 len = 0;

        if(state == CP_FRAME_STATE_RESPONE_OK)
        {
            pFrame[CP_BIKE_INF_STAT_POS] = bikeState;
            a_memcpy((const void *)pSIDInf, pFrame + CP_BIKE_INF_ID_POS, sizeof(CP_BikeIDInf));
            a_memcpy((const void *)pSTradeInf, pFrame + CP_BIKE_INF_TRADE_POS, sizeof(CP_BikeTradeInf));
            a_memcpy((const void *)pSRunInf, pFrame + CP_BIKE_INF_RUN_POS, sizeof(CP_BikeRunInf));


            len = CP_BIKE_INF_STAT_LEN + sizeof(CP_BikeIDInf) + sizeof(CP_BikeTradeInf) + sizeof(CP_BikeRunInf);
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_INF, state, pFrame, len);
        }
        else
        {
            len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                              CP_BIKE_CMD_GET_INF, state, pFrame, CP_FRAME_NO_DATA);
        }

        return len;
    }
#endif


#if CP_BIKE_MASTER
    u8 CP_BikeSetWPwLvReqFrame(u8 *pFrame, u8 lv)
    {
        u8 len = 0;

        *(pFrame + CP_BIKE_BSTAT_POS) = lv;
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_SET_WPW, CP_FRAME_STATE_REQUEST, pFrame, 1);

        return len;
    }
#endif



#if CP_BIKE_MASTER
    //请求更新Bike文件请求帧
    u8 CP_BikeReqUpdateReqFrame(u8 *pFrame, u8 fileType, u32 filePar, u32 fileSize)
    {
        u8 len = 0;
        CP_BikeReqUpdateInf *p = NULL;

        p = (CP_BikeReqUpdateInf *)(pFrame + CP_FRAME_DSTART_POS);

        p->updateType = fileType;
        *((u32 *)(p->uUpdateFileInf.sBikePoint.num)) = filePar;
        *((u32 *)(p->uUpdateFileInf.sBikePoint.size)) = fileSize;


        len = sizeof(CP_BikeReqUpdateInf);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_REQUEST_UPDATE, CP_FRAME_STATE_REQUEST, pFrame, len);

        return len;

    }
#endif






#if CP_BIKE_MASTER
    //更新Bike文件请求帧
    u8 CP_BikeUpdateReqFrame(u8 *pFrame, u8 isNext, u32 fileIndex, u8 *pFile)
    {
        u8 len = 0;
        CP_BikeUpdateREQ *p = NULL;

        p = (CP_BikeUpdateREQ *)(pFrame + CP_FRAME_DSTART_POS);

        *((u32 *)(p->index)) = fileIndex;
        a_memcpy((const void *)pFile, p->file, CP_BIKE_UPDATE_FILE_LEN);


        len = sizeof(CP_BikeUpdateREQ);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_REQUEST | isNext,
                          CP_BIKE_CMD_UPDATE, CP_FRAME_STATE_REQUEST, pFrame, len);

        return len;
    }
#endif


#if CP_BIKE_SLAVER
    //更新Bike文件应答帧
    u8 CP_BikeUpdateRspFrame(u8 *pFrame, u8 state, u32 fileIndex)
    {

        u8 len = 0;
        *((u32 *)(pFrame + CP_FRAME_DSTART_POS)) = fileIndex;

        len = sizeof(fileIndex);
        len = CP_GetFrame(CP_FRAME_BROADCAST_ADDR, CP_FRAME_BROADCAST_ADDR, CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER,
                          CP_BIKE_CMD_UPDATE, state, pFrame, len);


        return len;
    }
#endif


