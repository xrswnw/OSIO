#ifndef _ANYID_CP_BIKE_
#define _ANYID_CP_BIKE_
#include "AnyID_CP.h"

//用结构体作为通信协议格式时，必须要考虑系统结构体的对齐模式和系统的"对齐访问"和"非对齐访问"的区别



//主从设备设定，一个设备既可以是从设备也可以是主设备
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CP_BIKE_MASTER          0x01
#define CP_BIKE_SLAVER          0x01
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//系统命令
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//查询自行车标识（UID）	0X01
//设置自行车借\还状态   0X02
//读取自行车运行信息    0X03
//设置自行车交易信息    0X11
//读取自行车交易信息    0X12
//请求自行车系统更新    0XE1
//执行自行车系统更新    0XE2
//停止自行车系统更新    0XE3
#define CP_BIKE_CMD_GET_ID                  0x01
#define CP_BIKE_CMD_SET_STAT                0x02
#define CP_BIKE_CMD_GET_RUN_INF             0x03


#define CP_BIKE_CMD_SET_TRADE_INF           0x11
#define CP_BIKE_CMD_GET_TRADE_INF           0x12

#define CP_BIKE_CMD_REQUEST_UPDATE          0xE1
#define CP_BIKE_CMD_UPDATE                  0xE2
#define CP_BIKE_CMD_STOP_UPDATE             0xE3


//内部命令码
#define CP_BIKE_CMD_SET_STAT_AND_TRDINF     0x21
#define CP_BIKE_CMD_GET_INF                 0x22
#define CP_BIKE_CMD_SET_ID                  0x23
#define CP_BIKE_CMD_SET_RUN_INF             0x24
#define CP_BIKE_CMD_SET_WPW                 0x25    //设置无线发射功率



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//获取BikeID和Bike故障信息
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//正确获取BikeID的应答帧数据域结构体
#define CP_BIKE_ERRCODE_LEN                                     1
#define CP_BIKE_ID_LEN                                          12
//Bike故障码
#define CP_BIKE_ERRCODE_CHARGE                                  0x80    //充电故障
#define CP_BIKE_ERRCODE_BLDC                                    0x40    //电机故障
#define CP_BIKE_ERRCODE_HALL                                    0x20    //Hall故障
#define CP_BIKE_ERRCODE_MP3                                     0x10    //MP3故障
#define CP_BIKE_ERRCODE_FLASH                                   0x08    //Flash故障
typedef struct bikeIDinf{
    u8 errCode;
    u8 id[CP_BIKE_ID_LEN];
}CP_BikeIDInf, CP_BikeGIDRSP;


//获取BikeID的请求帧，只有主设备才有
#if CP_BIKE_MASTER
    #define CP_BikeGetIDReqFrame(pFrame)                        CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_GET_ID, \
                                                                             CP_FRAME_STATE_REQUEST, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)
#endif


//响应帧，只有从设备才有
#if CP_BIKE_SLAVER
    u8 CP_BikeGetIDRspFrame(u8 * pFrame, u8 state, u8 bikeErrCode, u8 *pBikeID);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//设置BikeID的请求帧
 typedef struct bikeIDinf CP_BikeSIDREQ;
#if CP_BIKE_MASTER
     u8 CP_BikeSetIDReqFrame(u8 * pFrame, u8 bikeErrCode, u8 *pBikeID);                                                                        
#endif


//响应帧
#if CP_BIKE_SLAVER
    u8 CP_BikeSetIDRspFrame(u8 * pFrame, u8 state, u8 bikeState);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










///设置Bike借/还状态
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CP_BIKE_TRADE_TIME_LEN                                  6
typedef union tradeTime{
    u8 tradeTime[CP_BIKE_TRADE_TIME_LEN];
    struct{
        u8 year;
        u8 month;
        u8 day;
        u8 hour;
        u8 minute;
        u8 second;
    }sTradeTime;
} CP_BIKETradeTime;
#define CP_BIKE_BSTAT_RETURN                                    0x00        //还车
#define CP_BIKE_BSTAT_CRUISE                                    0x01        //巡航
#define CP_BIKE_BSTAT_BOOST                                     0x02        //助力
#define CP_BIKE_BSTAT_POS                                       (CP_FRAME_DSTART_POS)
#define CP_BIKE_BSTAT_LEN                                       1
#define CP_BIKE_TIME_POS                                        (CP_BIKE_BSTAT_POS + CP_BIKE_BSTAT_LEN)
#define CP_BIKE_TIME_LEN                                        sizeof(CP_BIKETradeTime)
#if CP_BIKE_MASTER
    u8 CP_BikeSetStateReqFrame(u8 *pFrame, u8 state, u8 *pTimes);
#endif

#if CP_BIKE_SLAVER
    //设置Bike状态的应答帧
    #define CP_BikeSetStateRspFrame(pFrame, state)              CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_SET_STAT, \
                                                                             state, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////















//获取Bike行驶信息
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//正确获取Bike行驶信息的应答帧数据域结构体
typedef struct bikeRunInf{
    u32 mileage;
    u32 totalMileage;
    u32 reduceC;
    u32 totalReduceC;
}CP_BikeRunInf, CP_BikeGRunInfRSP;

#if CP_BIKE_MASTER
    //读取Bike行驶信息请求帧
    #define CP_BikeGetRunInfReqFrame(pFrame)                    CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_GET_RUN_INF, \
                                                                             CP_FRAME_STATE_REQUEST, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)
#endif


#if CP_BIKE_SLAVER
    u8 CP_BikeGetRunInfRspFrame(u8 * pFrame, u8 state, u32 mileage, u32 totalMileage, u32 reduceC, u32 totalReduceC);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//设置Bike行驶信息
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct bikeRunInf CP_BikeSRunInfREQ;
#if CP_BIKE_MASTER
    //设置Bike行驶信息请求帧
    u8 CP_BikeSetRunInfReqFrame(u8 * pFrame, u32 mileage, u32 totalMileage, u32 reduceC, u32 totalReduceC);
#endif


#if CP_BIKE_SLAVER
    #define CP_BikeSetRunInfRspFrame(pFrame)                    CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_SET_RUN_INF, \
                                                                             CP_FRAME_STATE_RESPONE_OK, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)

#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////













//设置Bike交易信息
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//设置Bike交易信息的请求帧数据域结构体
#define CP_BIKE_USER_ID_LEN                                     4
#define CP_BIKE_POLE_ID_LEN                                     4
typedef struct bikeTradeInf{
    CP_BIKETradeTime uTradeTime;
    u8 userID[CP_BIKE_USER_ID_LEN];
    u8 poleID[CP_BIKE_POLE_ID_LEN];
}CP_BikeTradeInf, CP_BikeSTradeInfREQ;

#if CP_BIKE_MASTER
    //设置Bike交易信息请求帧
    u8 CP_BikeSetTradeInfReqFrame(u8 *pFrame, u8 *pTradeTime, u8 *pUserID, u8 *pPoleID);
#endif

#if CP_BIKE_SLAVER
    //设置Bike交易信息的应答帧
    #define CP_BikeSetTradeInfRspFrame(pFrame, state)           CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_SET_TRADE_INF, \
                                                                             state, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////













//获取Bike交易信息
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//获取Bike交易信息的响应帧数据域结构体
typedef struct bikeTradeInf CP_BikeGTradeInfRSP;
#if CP_BIKE_MASTER
    //获取Bike交易信息请求帧
    #define CP_BikeGetTradeInfReqFrame(pFrame)                  CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_GET_TRADE_INF, \
                                                                             CP_FRAME_STATE_REQUEST, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)

#endif


#if CP_BIKE_SLAVER
    //获取Bike交易信息的应答帧
    u8 CP_BikeGetTradeInfRspFrame(u8 *pFrame, u8 state, u8 *pTradeTime, u8 *pUserID, u8 *pPoleID);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//设置Bike状态和交易信息
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct bikeStatAndTradeInf{
    u8 state;
    struct bikeTradeInf tradeInf;
}CP_BikeSStatAndTradeInfREQ;

#if CP_BIKE_MASTER
    //设置Bike交易信息请求帧
    u8 CP_BikeSetStatTrdInfReqFrame(u8 *pFrame, u8 *pTradeTime, u8 *pUserID, u8 *pPoleID, u8 state);
#endif

#if CP_BIKE_SLAVER
    //设置Bike交易信息的应答帧
    #define CP_BikeSetStatTrdInfRspFrame(pFrame, state)         CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_SET_STAT_AND_TRDINF, \
                                                                             state, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





/*


//获取Bike交易信息和状态
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//获取Bike交易信息和状态的响应帧数据域结构体
typedef struct bikeStatAndTradeInf CP_BikeGStatAndTradeInfRSP;
#if CP_BIKE_MASTER
    //获取Bike交易信息和状态请求帧
    #define CP_BikeGetStatTrdInfReqFrame(pFrame)                CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_GET_STAT_AND_TRDINF, \
                                                                             CP_FRAME_STATE_REQUEST, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)

#endif


#if CP_BIKE_SLAVER
    //获取Bike交易信息和状态的应答帧
    u8 CP_BikeGetStatTrdInfRspFrame(u8 *pFrame, u8 state, u8 *pTradeTime, u8 *pUserID, u8 bikeState);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

*/

//获取Bike信息
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//获取Bike信息
typedef struct bikeInf{
    u8 state;
    struct bikeIDinf bikeID;
    struct bikeTradeInf tradeInf;
    struct bikeRunInf runInf;
}CP_BikeInf, CP_BikeGetInfREQ;
#define CP_BIKE_INF_STAT_POS                                    CP_FRAME_DSTART_POS
#define CP_BIKE_INF_STAT_LEN                                    1
#define CP_BIKE_INF_ID_POS                                      (CP_BIKE_INF_STAT_POS + CP_BIKE_INF_STAT_LEN) 
#define CP_BIKE_INF_TRADE_POS                                   (CP_BIKE_INF_ID_POS + sizeof(CP_BikeIDInf)) 
#define CP_BIKE_INF_RUN_POS                                     (CP_BIKE_INF_TRADE_POS + sizeof(CP_BikeTradeInf)) 



#if CP_BIKE_MASTER
    //获取Bike信息
    #define CP_BikeGetInfReqFrame(pFrame)                       CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_GET_INF, \
                                                                             CP_FRAME_STATE_REQUEST, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)
#endif


#if CP_BIKE_SLAVER
    //获取Bike交易信息和状态的应答帧
    u8 CP_BikeGetInfRspFrame(u8 *pFrame, u8 state, u8 bikeState, CP_BikeIDInf *pSIDInf, CP_BikeTradeInf *pSTradeInf, CP_BikeRunInf *pSRunInf);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//设置无线发射功率
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CP_BIKE_SET_WPW_LV_POS                                      CP_FRAME_DSTART_POS
#define CP_BIKE_SET_WPW_LV_MAX                                      17
#if CP_BIKE_MASTER
    u8 CP_BikeSetWPwLvReqFrame(u8 *pFrame, u8 lv);
#endif

#if CP_BIKE_SLAVER
    //设置无线发送功率的应答帧
    #define CP_BikeSetWPwLvRspFrame(pFrame)              CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                     CP_FRAME_BROADCAST_ADDR, \
                                                                     CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER, \
                                                                     CP_BIKE_CMD_SET_WPW, \
                                                                     CP_FRAME_STATE_RESPONE_OK, \
                                                                     pFrame, \
                                                                     CP_FRAME_NO_DATA)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//请求更新Bike文件请求帧数据结构体
//文件类型
#define CP_BIKE_UPDATEINF_TYPE_BP                               0x01        //Bike停车点坐标
#define CP_BIKE_UPDATEINF_TYPE_MP                               0x02        //MP3播放点坐标
#define CP_BIKE_UPDATEINF_TYPE_BIN                              0x03        //系统程序Bin文件
#define CP_BIKE_UPDATEINF_TYPE_BPN                              0x04        //Bike停车点名称
#define CP_BIKE_UPDATEINF_TYPE_MP3                              0x05        //MP3文件


#define CP_BIKE_UPDATEINF_BP_NUM_LEN                            4           //Bike停车点坐标
#define CP_BIKE_UPDATEINF_BPF_SIZE_LEN                          4
#define CP_BIKE_UPDATEINF_MP_NUM_LEN                            4           //MP3播放点坐标
#define CP_BIKE_UPDATEINF_MPF_SIZE_LEN                          4
#define CP_BIKE_UPDATEINF_BIN_VERSION_LEN                       4           //系统程序Bin文件
#define CP_BIKE_UPDATEINF_BIN_SIZE_LEN                          4
#define CP_BIKE_UPDATEINF_BPN_NUM_LEN                           4           //Bike停车点名称
#define CP_BIKE_UPDATEINF_BPNF_SIZE_LEN                         4
#define CP_BIKE_UPDATEINF_MP3_IDX_LEN                           4           //MP3文件
#define CP_BIKE_UPDATEINF_MP3F_SIZE_LEN                         4


typedef struct bikeReqUpdateInf{
    u8 updateType;
    union {
        struct{
            u8 num[CP_BIKE_UPDATEINF_BP_NUM_LEN];
            u8 size[CP_BIKE_UPDATEINF_BPF_SIZE_LEN];
        }sBikePoint;
        struct{
            u8 num[CP_BIKE_UPDATEINF_MP_NUM_LEN];
            u8 size[CP_BIKE_UPDATEINF_MPF_SIZE_LEN];
        }sMp3Point;
        struct{
            u8 version[CP_BIKE_UPDATEINF_BIN_VERSION_LEN];
            u8 size[CP_BIKE_UPDATEINF_BIN_SIZE_LEN];
        }sBikeBin;
        struct{
            u8 num[CP_BIKE_UPDATEINF_BPN_NUM_LEN];
            u8 size[CP_BIKE_UPDATEINF_BPNF_SIZE_LEN];
        }sBikePointName;
        struct{
            u8 index[CP_BIKE_UPDATEINF_MP3_IDX_LEN];
            u8 size[CP_BIKE_UPDATEINF_MP3F_SIZE_LEN];
        }sMp3;
    }uUpdateFileInf;
} CP_BikeReqUpdateInf;


//请求更新Bike文件的失败响应帧
#define CP_BIKE_REQUPDATE_ERR_NONEED                            0x01        //已经是最新，不必更新
#define CP_BIKE_REQUPDATE_ERR_ERSFLASH                          0x02        //擦除FLASH错误
#define CP_BIKE_REQUPDATE_ERR_FSIZE                             0x03        //文件太大


#if CP_BIKE_MASTER
    //请求更新Bike文件请求帧
    u8 CP_BikeReqUpdateReqFrame(u8 *pFrame, u8 fileType, u32 filePar, u32 fileSize);
#endif

#if CP_BIKE_SLAVER
    //请求更新Bike文件响应帧
    #define CP_BikeReqUpdateRspFrame(pFrame, state)             CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_BROADCAST_ADDR, \
                                                                             CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER, \
                                                                             CP_BIKE_CMD_REQUEST_UPDATE, \
                                                                             state, \
                                                                             pFrame, \
                                                                             CP_FRAME_NO_DATA)
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////












////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//更新Bike文件请求帧数据结构体
#define CP_BIKE_UPDATE_INDEX_LEN                            4
#define CP_BIKE_UPDATE_FILE_LEN                             32
typedef struct bikeUpdate{
    u8 index[CP_BIKE_UPDATE_INDEX_LEN];
    u8 file[CP_BIKE_UPDATE_FILE_LEN];
} CP_BikeUpdateREQ;

#if CP_BIKE_MASTER
    //更新Bike文件请求帧
    u8 CP_BikeUpdateReqFrame(u8 *pFrame, u8 isNext, u32 fileIndex, u8 *pFile);
#endif

#if CP_BIKE_SLAVER
    //更新Bike文件应答帧
    u8 CP_BikeUpdateRspFrame(u8 *pFrame, u8 state, u32 fileIndex);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








//停止更新系统
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CP_BIKE_MASTER
    //停止更新Bike文件请求帧
    #define CP_BikeStpUpdateReqFrame(pFrame)             CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                      CP_FRAME_BROADCAST_ADDR, \
                                                                      CP_FRAME_CTRL_REQUEST | CP_FRAME_CTRL_OVER, \
                                                                      CP_BIKE_CMD_STOP_UPDATE, \
                                                                      CP_FRAME_STATE_REQUEST, \
                                                                      pFrame, \
                                                                      CP_FRAME_NO_DATA)
#endif

#if CP_BIKE_SLAVER
    #define CP_BikeStpUpdateRspFrame(pFrame, state)      CP_GetFrame(CP_FRAME_BROADCAST_ADDR, \
                                                                      CP_FRAME_BROADCAST_ADDR, \
                                                                      CP_FRAME_CTRL_RESPONSE | CP_FRAME_CTRL_OVER, \
                                                                      CP_BIKE_CMD_STOP_UPDATE, \
                                                                      state, \
                                                                      pFrame, \
                                                                      CP_FRAME_NO_DATA)

#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif
