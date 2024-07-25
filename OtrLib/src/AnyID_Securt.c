#include "AnyID_Securt.h"

#pragma location = "CCMRAM"
CRYPT_OPERATION g_sCryptOperation = {0};


#pragma location = "CCMRAM"
u8 g_aCryptCpuId[CRYPT_KEY_LEN] = {0};

#pragma location = "CCMRAM"
u8 g_aCryptEsamId[CRYPT_KEY_LEN] = {0};


#pragma location = "CCMRAM"
u8 g_aCryptSamId[CRYPT_KEY_LEN] = {0};

const u8 g_aCryptText[CRYPT_TXT_LEN] = {
'q' - 'a', 'i' - 'a', 'a' - 'a', 'n' - 'a',
0,
'j' - 'a', 'i' - 'a', 'a' - 'a',
0,
'y' - 'a', 'u' - 'a',
0,
1,
0,
0, 1,
0,
2, 0, 1, 2, 1, 0, 0, 4,
0,
3,4,1,7,2,3,2,0,1,2,1,0,0,4,3,1,1,7,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void Crypt_InitOperate(void)
{
    u8 pos = 0;
    //g_sCryptOperation.operation[pos++] = CRYPT_INDEX_CRC;不做CRC校验，否则每次更新程序都要修改秘钥
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_SAMID;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_RSTESAM;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_SLEMF;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_EXCHK;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_SELBIN;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_READBIN;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_INCHK;

    g_sCryptOperation.opIndex = 0;
    g_sCryptOperation.opNum = pos;

}
void Crypt_InternalCheck(void)
{
    u8 pos = 0;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_SLEMF;
    g_sCryptOperation.operation[pos++] = CRYPT_INDEX_INCHK;

    g_sCryptOperation.opIndex = 0;
    g_sCryptOperation.opNum = pos;
}


