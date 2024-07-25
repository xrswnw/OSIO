#include "AnyID_Des.h"

u8 g_aDesSubKeys[2][16][48] = {0};

// permuted choice table (PC1)
const u8 g_aDesPc1Table[56] = {
57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};
// permuted choice key (PC2)
const u8 g_aDesPc2Table[48] = {
14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
// number left rotations of pc1
const u8 g_aDesShiftTable[16] = {
1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
// initial permutation (IP)
const u8 g_aDesIpTable[64] = {
58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};
// expansion operation matrix (E)
const u8 g_aDesETable[48] = {
32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};
// The (in)famous S-boxes
const u8 g_aDesSBox[8][4][16] = {
// S1
14,  4,    13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
// S2
15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
// S3
10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
// S4
7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
// S5
2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
// S6
12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
// S7
4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
// S8
13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};
// 32-bit permutation function P used on the output of the S-boxes
const u8 g_aDesPTable[32] = {
16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};
// final permutation IP^-1
const u8 g_aDesIprTable[64] = {
40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
};

void Des_InitializeKey(u8 *srcBytes, u8 keyN)
{
    //convert 8 u8-bytes key to 64 binary-bits
    u8 sz_64key[64] = {0};
    u8 k = 0;

    Des_Bytes2Bits(srcBytes, sz_64key, 64);
    //PC 1
    u8 sz_56key[56] = {0};
    for(k = 0; k < 56; k++)
    {
        sz_56key[k] = sz_64key[g_aDesPc1Table[k]-1];
    }
    Des_CreateSubKey(sz_56key, keyN);
}

void Des_CreateSubKey(u8 *sz_56key, u8 keyN)
{
    u8 i = 0, j = 0;
    u8 szTmpL[28] = {0};
    u8 szTmpR[28] = {0};
    u8 szCi[28] = {0};
    u8 szDi[28] = {0};

    Des_Memcpy(szTmpL, sz_56key, 28);
    Des_Memcpy(szTmpR, sz_56key + 28, 28);

    for(i = 0; i < 16; i++)
    {
        //permuted choice 48 bits key
        u8 szTmp56[56] = {0};

        //shift to left
        //Left 28 bits
        Des_Memcpy(szCi,szTmpL + g_aDesShiftTable[i], 28 - g_aDesShiftTable[i]);
        Des_Memcpy(szCi + 28 - g_aDesShiftTable[i], szTmpL, g_aDesShiftTable[i]);
        //Right 28 bits
        Des_Memcpy(szDi, szTmpR + g_aDesShiftTable[i], 28 - g_aDesShiftTable[i]);
        Des_Memcpy(szDi + 28 - g_aDesShiftTable[i], szTmpR, g_aDesShiftTable[i]);

        Des_Memcpy(szTmp56, szCi, 28);
        Des_Memcpy(szTmp56 + 28, szDi, 28);
        for(j = 0; j < 48; j++)
        {
            g_aDesSubKeys[keyN][i][j] = szTmp56[g_aDesPc2Table[j] - 1];
        }
        //Evaluate new szTmpL and szTmpR
        Des_Memcpy(szTmpL, szCi, 28);
        Des_Memcpy(szTmpR, szDi, 28);
    }
}

void Des_EncryptData(u8 *pSrcBytes, u8 *pDstBytes, u8 keyN)
{
    u8 i = 0, j = 0;
    u8 szSrcBits[64] = {0};
    u8 sz_IP[64] = {0};
    u8 sz_Li[32] = {0};
    u8 sz_Ri[32] = {0};
    u8 sz_Final64[64] = {0};
    u8 szCiphertextRaw[64];

    Des_Bytes2Bits(pSrcBytes, szSrcBits, 64);
    //IP
    Des_InitialPermuteData(szSrcBits, sz_IP);
    Des_Memcpy(sz_Li, sz_IP, 32);
    Des_Memcpy(sz_Ri, sz_IP + 32, 32);

    for(i = 0; i < 16; i++)
    {
        Des_FunctionF(sz_Li, sz_Ri, i, keyN);
    }
    //so D=LR

    Des_Memcpy(sz_Final64, sz_Ri, 32);
    Des_Memcpy(sz_Final64 + 32, sz_Li, 32);

    //~IP
    for(j = 0; j < 64; j++)
    {
        szCiphertextRaw[j] = sz_Final64[g_aDesIprTable[j] - 1];
    }
    Des_Bits2Bytes(pDstBytes, szCiphertextRaw, 64);
}

void Des_DecryptData(u8 *pSrcBytes, u8 *pDstBytes, u8 keyN)
{
    u8 szSrcBits[64] = {0};
    u8 sz_IP[64] = {0};
    u8 sz_Li[32] = {0};
    u8 sz_Ri[32] = {0};
    u8 sz_Final64[64] = {0};
    u8 szPlaintextRaw[64] = {0};
    u8 i = 0, j = 0;

    Des_Bytes2Bits(pSrcBytes, szSrcBits, 64);
    //IP --- return is sz_IP
    Des_InitialPermuteData(szSrcBits, sz_IP);
    //divide the 64 bits data to two parts
    Des_Memcpy(sz_Ri, sz_IP, 32); //exchange L to R
    Des_Memcpy(sz_Li, sz_IP + 32, 32);  //exchange R to L
    //16 rounds F and xor and exchange
    for(i = 0; i < 16; i++)
    {
        Des_FunctionF(sz_Ri, sz_Li, 15 - i, keyN);
    }
    Des_Memcpy(sz_Final64, sz_Li, 32);
    Des_Memcpy(sz_Final64 + 32, sz_Ri, 32);
    // ~IP
    for(j = 0; j < 64; j++)
    {
        szPlaintextRaw[j] = sz_Final64[g_aDesIprTable[j] - 1];
    }
    Des_Bits2Bytes(pDstBytes, szPlaintextRaw, 64);
}

void Des_FunctionF(u8 *sz_Li, u8 *sz_Ri, u8 iKey, u8 keyN)
{
    u8 sz_48R[48] = {0};
    u8 sz_xor48[48] = {0};
    u8 sz_P32[32] = {0};
    u8 sz_Rii[32] = {0};
    u8 sz_Key[48] = {0};
    u8 s_Compress32[32] = {0};

    Des_Memcpy(sz_Key, g_aDesSubKeys[keyN][iKey], 48);
    Des_ExpansionR(sz_Ri, sz_48R);
    Des_XOR(sz_48R, sz_Key, 48, sz_xor48);

    Des_CompressFuncS(sz_xor48, s_Compress32);
    Des_PermutationP(s_Compress32, sz_P32);
    Des_XOR(sz_P32, sz_Li, 32, sz_Rii);
    Des_Memcpy(sz_Li, sz_Ri, 32);
    Des_Memcpy(sz_Ri, sz_Rii, 32);
}

void Des_InitialPermuteData(u8 *src, u8 *dst)
{
    u8 i = 0;
    //IP
    for(i = 0; i < 64; i++)
    {
        dst[i] = src[g_aDesIpTable[i] - 1];
    }
}

void Des_ExpansionR(u8 *src, u8 *dst)
{
    u8 i = 0;
    //IP
    for(i = 0; i < 48; i++)
    {
        dst[i] = src[g_aDesETable[i] - 1];
    }
}

void Des_XOR(u8 *szParam1, u8 *szParam2, u8 uiParamLength, u8 *szReturnValueBuffer)
{
    u8 i = 0;
    for(i = 0; i < uiParamLength; i++)
    {
        szReturnValueBuffer[i] = szParam1[i] ^ szParam2[i];
    }
}

void Des_CompressFuncS(u8 *src48, u8 *dst32)
{
    u8 bTemp[8][6]={0};
    u8 dstBits[4]={0};
    u8 i = 0, j = 0;

    for(i = 0; i < 8; i++)
    {
        Des_Memcpy(bTemp[i], src48 + i * 6, 6);
        int iX = (bTemp[i][0]) * 2 + (bTemp[i][5]);
        int iY = 0;
        for(j = 1; j < 5; j++)
        {
            iY += bTemp[i][j] << (4 - j);
        }
        Des_Int2Bits(g_aDesSBox[i][iX][iY], dstBits);
        Des_Memcpy(dst32 + i * 4, dstBits, 4);
    }

}

void Des_PermutationP(u8 *src, u8 *dst)
{
    u8 i = 0;
    for(i = 0; i < 32; i++)
    {
        dst[i] = src[g_aDesPTable[i]-1];
    }
}

void Des_Bytes2Bits(u8 *srcBytes, u8 *dstBits, u8 sizeBits)
{
    u8 i = 0;
    for(i = 0; i < sizeBits; i++)
    {
        dstBits[i] = ((srcBytes[i >> 3] << (i & 7)) & 0x80) >> 7;
    }
}

void Des_Bits2Bytes(u8 *dstBytes, u8 *srcBits, u8 sizeBits)
{
    u8 i = 0;
    memset(dstBytes, 0, sizeBits >> 3);
    for(i = 0; i < sizeBits; i++)
    {
        dstBytes[i >> 3] |= (srcBits[i] << (7 - (i & 7)));
    }
}

void Des_Int2Bits(u8 src, u8 *dstBits)
{
    u8 i = 0;
    for(i = 0; i < 4; i++)
    {
        dstBits[i] = ((src << i) & 8) >> 3;
    }
}



u32 Des_EncryptAnyLength(u8 *pSrcBytes, u32 srcLength, u8 *pCiphertext, u8 keyN)
{
    u32 iParts = srcLength >> 3;
    u8 iResidue = srcLength % 8;
    u32 i = 0;

    for(i = 0; i < iParts; i++)
    {
        Des_EncryptData(pSrcBytes + (i << 3), pCiphertext + (i << 3), keyN);
    }
    if(iResidue)
    {
        u8 szLast8Bits[8] = {0};
        memcpy(szLast8Bits, pSrcBytes + (i << 3), iResidue);
        Des_EncryptData(szLast8Bits, pCiphertext + (i << 3), keyN);
        i += 1;
    }

    return (i << 3);
}

u32 Des_DecryptAnyLength(u8 *pSrcBytes, u32 srcLength, u8 *pPlaintext, u8 keyN)
{
    u32 iParts = srcLength >> 3;
    u8 iResidue = srcLength % 8;
    u32 i = 0;

    for(i = 0; i < iParts; i++)
    {
        Des_DecryptData(pSrcBytes + (i << 3), pPlaintext + (i << 3), keyN);
    }
    if(iResidue)
    {
        u8 szLast8Bits[8] = {0};
        memcpy(szLast8Bits, pSrcBytes + (i << 3), iResidue);
        Des_DecryptData(szLast8Bits, pPlaintext + (i << 3), keyN);
        i += 1;
    }

    return (i << 3);
}
