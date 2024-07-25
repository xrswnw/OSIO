#ifndef _ANYID_SECURT_
#define _ANYID_SECURT_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"
#include "AnyID_Des.h"

#define CRYPT_INDEX_CRC             0
#define CRYPT_INDEX_SAMID           1
#define CRYPT_INDEX_RSTESAM         2
#define CRYPT_INDEX_SLEMF           4
#define CRYPT_INDEX_EXCHK           6
#define CRYPT_INDEX_SELBIN          7
#define CRYPT_INDEX_READBIN         8
#define CRYPT_INDEX_BINCHK          9
#define CRYPT_INDEX_INCHK           10

#define CRYPT_OP_MAX_NUM            20

typedef struct cryptOperation{
    u8 operation[CRYPT_OP_MAX_NUM];
    u8 opNum;
    u8 opIndex;
    int rlt;
}CRYPT_OPERATION;
extern CRYPT_OPERATION g_sCryptOperation;

#define CRYPT_KEY_LEN         16
#define CRYPT_TXT_LEN         64
extern u8 g_aCryptCpuId[CRYPT_KEY_LEN];
extern u8 g_aCryptEsamId[CRYPT_KEY_LEN];
extern u8 g_aCryptSamId[CRYPT_KEY_LEN];
extern const u8 g_aCryptText[CRYPT_TXT_LEN];

void Crypt_InitOperate(void);
void Crypt_InternalCheck(void);

#define Crypt_Init(k)                  do{Des_InitializeKey((k), 0); Des_InitializeKey((k) + 8, 1);}while(0)
#define Crypt_Encrypt16(src, dst, len) do{\
                                            u8 temp[CRYPT_KEY_LEN] = {0};\
                                            Des_EncryptAnyLength((src), (len), dst, 0); \
                                            Des_DecryptAnyLength(dst, (len), temp, 1); \
                                            Des_EncryptAnyLength(temp, (len), dst, 0); \
                                        }while(0)
#define Crypt_Decrypt64(src, dst, len)  do{\
                                            u8 temp[CRYPT_TXT_LEN] = {0};\
                                            Des_DecryptAnyLength((src), (len), dst, 0); \
                                            Des_EncryptAnyLength(dst, (len), temp, 1); \
                                            Des_DecryptAnyLength(temp, (len), dst, 0); \
                                        }while(0)
//#define Crypt_IsOk(src, len)               a_memcmp((src), g_aCryptText, (len))
#define Crypt_IsOk(src)               a_memcmp((src), g_aCryptText, CRYPT_TXT_LEN)
#endif