#ifndef _ANYID_DES_
#define _ANYID_DES_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"

#define Des_Memcpy(dst, src, n) a_memcpy((src), (dst), (n))

#define DES_3_KEY_LEN           16
#define DES_KEY_LEN             8

void Des_InitializeKey(u8 *srcBytes, u8 keyN);
void Des_CreateSubKey(u8 *sz_56key, u8 keyN);
void Des_EncryptData(u8 *pSrcBytes, u8 *pDstBytes, u8 keyN);
void Des_DecryptData(u8 *pSrcBytes, u8 *pDstBytes, u8 keyN);
void Des_FunctionF(u8 *sz_Li, u8 *sz_Ri, u8 iKey, u8 keyN);
void Des_InitialPermuteData(u8 *src, u8 *dst);
void Des_ExpansionR(u8 *src, u8 *dst);
void Des_XOR(u8 *szParam1, u8 *szParam2, u8 uiParamLength, u8 *szReturnValueBuffer);
void Des_CompressFuncS(u8 *src48, u8 *dst32);
void Des_PermutationP(u8 *src, u8 *dst);
void Des_Bytes2Bits(u8 *srcBytes, u8 *dstBits, u8 sizeBits);
void Des_Bits2Bytes(u8 *dstBytes, u8 *srcBits, u8 sizeBits);
void Des_Int2Bits(u8 src, u8 *dstBits);
u32 Des_EncryptAnyLength(u8 *pSrcBytes, u32 srcLength, u8 *pCiphertext, u8 keyN);
u32 Des_DecryptAnyLength(u8 *pSrcBytes, u32 srcLength, u8 *pPlaintext, u8 keyN);

#endif
