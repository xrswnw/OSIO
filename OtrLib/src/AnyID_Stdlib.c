#include "AnyID_Stdlib.h"

const u32 g_aCrc32tab[256] = {
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


#define CRC_STM32_INIT_VALUE    0xFFFFFFFF
u32 a_GetCrc32(u8 *pBuffer, u32 len)
{
    u32 i = 0, crc = 0;
    
    crc = CRC_STM32_INIT_VALUE;
    for (i = 0; i < len; i++)
    {
        crc = g_aCrc32tab[(crc ^ pBuffer[i]) & 0xFF] ^ (crc >> 8);
    }

    return crc ^ 0xFFFFFFFF;
}

#define CRC8_POLYNOMIAL                      0x8C   //x8+x5+x4+1
#define CRC8_PRESET_VALUE                    0x00
u8 a_GetCrc8(u8 *pFrame, u32 len)
{
    u8 crc = 0;
    u32 i = 0, j = 0;
    crc = CRC8_PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pFrame[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ CRC8_POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }
    return crc;
}

#define POLYNOMIAL                      0x8408   //x^16 + x^12 + x^5 + 1
#define PRESET_VALUE                    0xFFFF
u16 a_GetCrc(u8 *pFrame, u32 len)
{
    u16 crc = 0;
    u32 i = 0, j = 0;
    crc = PRESET_VALUE;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ pFrame[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }
    crc = ~crc;
    return crc;
}

#define CRC5_PRESET_VALUE                   0x48    //x5+x3+1
#define CRC5_POLYNOMIAL_VALUE               0x48
u8 a_CalCrc5(u8 *pByte, u16 len)
{
    u16 i = 0, j = 0;
    u8 crc = CRC5_PRESET_VALUE;        // Initial value: 0x48 = 0x09<<(8-5)

    for(i = 0; i < len; i++)
    {
        crc ^= pByte[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x80)
            {
                crc = (crc << 1) ^ CRC5_POLYNOMIAL_VALUE;        // 0x48 = 0x09<<(8-5)
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc >> 3;
}

const u8 g_aAsicto[256] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

u16 a_Str2Hex(char *pStr, u8 *pHexBuf)
{
    u16 len = 0;
    u16 i = 0;
    u8 t = 0;
    if(pStr != NULL)
    {
        len = strlen(pStr) & 0xFFFE;    //偶数
        for(i = 0; i < len; i += 2)
        {
            t = g_aAsicto[pStr[i + 0]];
            t <<= 4;
            t |= g_aAsicto[pStr[i + 1]];
            
            *pHexBuf = t;
            pHexBuf++;
        }
        
        len >>= 1;
    }
    
    return len;
}

void a_ArrayCopy(void *pSrc, DAT_FMT_TYPE srcType, void *pDest, DAT_FMT_TYPE destType, u32 n)
{
    u8 srcStep = 0;
    u8 destStep = 0;
    u8 step = 0, m = 0;
    u8 *p1 = NULL, *p2 = NULL;
    u32 i = 0;
    
    p1 = pSrc;
    p2 = pDest;
    
    srcStep = (u8)srcType;
    destStep = (u8)destType;
    
    step = srcStep;         //取最小的步进
    if(srcStep > destStep)
    {
        step = destStep;
    }
    for(i = 0; i < n; i++)
    {
        for(m = 0; m < step; m++)
        {
            p2[m] = p1[m];
        }
        p1 += srcStep;
        p2 += destStep;
    }
}

void a_ArrayToU16s(u16 *pDest, u8 *pSrc, u32 n)
{
    u32 i = 0;
    u32 t = 0;
    for(i = 0; i < n; i++)
    {
        t = pSrc[(i << 1) + 0];
        t <<= 8;
        t |= pSrc[(i << 1) + 1];
        pDest[i] = t;
    }
}


const u8 g_aHextoAsic[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
void a_Hex2Str(char *pBuf, u8 *pHex, u16 len)
{
    u16 i = 0;
    u8 t = 0;
    for(i = 0; i < len; i++)
    {
        t = (pHex[i] >> 4) & 0x0F;
        pBuf[(i << 1) + 0] = g_aHextoAsic[t];

        t = (pHex[i] >> 0) & 0x0F;
        pBuf[(i << 1) + 1] = g_aHextoAsic[t];
    }
}

u32 a_Bcd2Dec(u32 bcd)
{
    u32 d = 0;
    
    d *=10; d += ((bcd >> 28) & 0x0F); 
    d *=10; d += ((bcd >> 24) & 0x0F);
    d *=10; d += ((bcd >> 20) & 0x0F);
    d *=10; d += ((bcd >> 16) & 0x0F);
    d *=10; d += ((bcd >> 12) & 0x0F);
    d *=10; d += ((bcd >> 8) & 0x0F);
    d *=10; d += ((bcd >> 4) & 0x0F);
    d *=10; d += ((bcd >> 0) & 0x0F);
    
    return d;
}

u32 a_Dec2Bcd(u32 d)
{
    u32 bcd = 0;
    u32 t = 0;
    u32 pos = 0;
    while(d > 0)
    {
        t = d % 10;
        bcd |= (t << pos);
        d /= 10;
        pos += 4;
    }
    
    return bcd;
}

//字符串转换成整数
u32 a_atoi(const u8 *str, const u8 len, u8 fmt)
{
    u32 n = 0;
    u8 *p = NULL;
    u8 c = 0;
    u8 i = 0;

    if(str != NULL)
    {
        switch(fmt)
        {
            case STD_LIB_FMT_BIN:
                for(p = (u8 *)str, i = 0; i < len; i++, p++)
                {
                    c = *p;
                    if(a_is_bin(c))
                    {
                        n *= STD_LIB_FMT_BIN;
                        n += c - '0';
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            case STD_LIB_FMT_OCT:
                for(p = (u8 *)str, i = 0; i < len; i++, p++)
                {
                    c = *p;
                    if(a_is_oct(c))
                    {
                        n *= STD_LIB_FMT_OCT;
                        n += c - '0';
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            case STD_LIB_FMT_DEC:
                for(p = (u8 *)str, i = 0; i < len; i++, p++)
                {
                    c = *p;
                    if(a_is_dec(c))
                    {
                        n *= STD_LIB_FMT_DEC;
                        n += c - '0';
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            case STD_LIB_FMT_FLT:
                for(p = (u8 *)str, i = 0; i < len; i++, p++)
                {
                    c = *p;
                    if(a_is_dec(c))
                    {
                        n *= STD_LIB_FMT_DEC;
                        n += c - '0';
                    }
                    else if(c == '.')
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            case STD_LIB_FMT_HEX:
                for(p = (u8 *)str, i = 0; i < len; i++, p++)
                {
                    c = *p;
                    if(a_is_digit(c))
                    {
                        n *= STD_LIB_FMT_HEX;
                        n += c - '0';
                    }
                    else if((c) >= 'a' && (c) <= 'f')
                    {
                        n *= STD_LIB_FMT_HEX;
                        n += c - 'a' + 10;
                    }
                    else if((c) >= 'A' && (c) <= 'F')
                    {
                        n *= STD_LIB_FMT_HEX;
                        n += c - 'A' + 10;
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            default:
                break;

        }
    }
   return n;
}

int a_IndexOf(u8 *pBuffer, u16 start, u16 len, u8 c)
{
    int pos = -1;
    
    while(start < len)
    {
        if(pBuffer[start] == c)
        {
            pos = start;
            break;
        }
        start++;
    }
    
    return pos;
}
u8 a_itoa(u32 data, u8 *pBuffer, u8 floatNum, u8 bufLen)
{
    u8 len = 0;
    u8 reLen = 0;

    if(pBuffer == NULL)
    {
        return 0;
    }

    floatNum = bufLen - floatNum;
    for(len = bufLen; len > 0; len--)
    {
        pBuffer[len - 1] = data % 10 + '0';
        data = data / 10;


        if((len - 1) == floatNum)
        {
			len -= 1;
            pBuffer[len - 1] = '.';

        }

        if(data == 0)
        {
			len--;
            break;
        }
    }

    reLen = bufLen - len;
    for(; len > 0; len--)
    {
        pBuffer[len - 1] = '0';
    }

    return reLen;
}


void *a_memset(void *dst, u8 val, u32 count)
{
    u8 *pchdst = NULL;

    pchdst = (u8 *)dst;

    if(pchdst != NULL)
    {
        while(count--)
        {
            *pchdst++ = val;
        }
        return dst;
    }
    else
    {
        return NULL;
    }
}

void *a_memcpy(const void *src, void *dst, u32 count)
{
    u8 * pchdst = (u8*)dst;
    u8 * pchsrc = (u8*)src;

    if(pchdst != NULL && pchsrc != NULL)
    {
        while(count--)
        {
            *pchdst++ = *pchsrc++;
        }
        return dst;
    }
    else
    {
        return NULL;
    }
}

u32 a_strlen(const u8 * str)
{
    u32 len = 0;
    const u8 *p = str;
    if(p != NULL)
    {
        while(*p++);

        len = (u32)(p - str - 1);
    }

    return len;
}
BOOL a_strcmp(const u8 *str1, const u8 *str2)
{
    BOOL b = FALSE;
    u8 * pchstr1 = (u8*)str1;
    u8 * pchstr2 = (u8*)str2;

    if(pchstr1 != NULL && pchstr2 != NULL)
    {
        while((*pchstr1 != '\0') && (*pchstr2 != '\0') && (*pchstr1++ == *pchstr2++));

        if((*pchstr1 == '\0') || (*pchstr2 == '\0'))
        {
            b = TRUE;
        }
    }


    return b;
}

BOOL a_memcmp(const void *p1, const void *p2, u32 len)
{
    BOOL b = FALSE;
    u8 * pchstr1 = (u8*)p1;
    u8 * pchstr2 = (u8*)p2;

    if(pchstr1 != NULL && pchstr2 != NULL)
    {
        while((len) && (*pchstr1++ == *pchstr2++))
        {
            len--;
        }

        if(len == 0)
        {
            b = TRUE;
        }
    }


    return b;
}



u16 a_lfsr(u16 seed)
{
    u16 lfsrBit = 0;
    u8 bit1Times = 0;

    u8 i = 0;

    //保留异或位置的值
    lfsrBit = seed & SYS_LFSR_MASK;
    //多项式移位
    seed >>= 1;

    //计算种子的异或位中有几个1
    for(i = 0; i < 6; i++)
    {
        if(lfsrBit & 0x01)
        {
            bit1Times++;
        }
        lfsrBit >>= 1;
    }

    //如果有奇数个1，异或的结果就是1，PWD的最高位置1
    if(bit1Times & 0x01)
    {
        seed |= SYS_LFSR_MSB;
    }

	return seed;
}


u32 a_GetDataFromFrame(u8 *pBuffer, u8 len)
{
    u8 i = 0;

    u32 data = 0;

    for(i = 0; i < len; i++)
    {
        data <<= 8;
        data += pBuffer[i];
    }

    return data;
}

void a_GetFrameFromData(u8 *pBuffer, u32 data, u8 len)
{
    u8 i = 0;

    for(i = len; i > 0; i--)
    {
        pBuffer[i - 1] = (u8)(data);
        data >>= 8;
    }
}


static int skip_atoi(const char **s)
{
    int i = 0;
    while(a_is_digit(**s))
    {
        i = i*10 + *((*s)++) - '0';
    }
    return i;
}




#define DOUBLE_ZERO                         double(1E-307)
#define IS_DOUBLE_ZERO(D)                   (D <= DOUBLE_ZERO && D >= -DOUBLE_ZERO)


#define ZEROPAD                             1               // Pad with zero
#define SIGN                                2               // Unsigned/signed long
#define PLUS                                4               // Show plus
#define SPACE                               8               // Space if plus
#define LEFT                                16              // Left justified
#define SPECIAL                             32              // 0x
#define LARGE                               64              // Use 'ABCDEF' instead of 'abcdef'

#define FLT_MAX_10_EXP                      38
#define DBL_MAX_10_EXP                      308
#define LDBL_MAX_10_EXP                     308

#define NOFLOAT                             1

static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";


static char * number(char *str, long num, int base, int size, int precision, int type)
{
    char c = 0, sign = 0, tmp[66] = {0};
    char *dig = digits;
    int i = 0;

    if(type & LARGE)
    {
        dig = upper_digits;
    }
    if(type & LEFT)
    {
        type &= ~ZEROPAD;
    }
    if(base < 2 || base > 36)
    {
        return 0;
    }

    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;

    if(type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
            size--;
        }
        else if (type & PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & SPACE)
        {
            sign = ' ';
            size--;
        }
    }

    if(type & SPECIAL)
    {
        if(16 == base)
        {
            size -= 2;
        }
        else if(8 == base)
        {
            size--;
        }
    }

    i = 0;
    if(0 == num)
    {
        tmp[i++] = '0';
    }
    else
    {
        while (num != 0)
        {
            tmp[i++] = dig[((unsigned long) num) % (unsigned) base];
            num = ((unsigned long) num) / (unsigned) base;
        }
    }

    if(i > precision)
    {
        precision = i;
    }
    size -= precision;

    if(!(type & (ZEROPAD | LEFT)))
    {
        while(size-- > 0)
        {
            *str++ = ' ';
        }
    }

    if(sign)
    {
        *str++ = sign;
    }

    if(type & SPECIAL)
    {
        if(8 == base)
        {
            *str++ = '0';
        }
        else if(16 == base)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    }

    if(!(type & LEFT))
    {
        while(size-- > 0)
        {
            *str++ = c;
        }
    }
    while(i < precision--)
    {
        *str++ = '0';
    }
    while(i-- > 0)
    {
        *str++ = tmp[i];
    }
    while(size-- > 0)
    {
        *str++ = ' ';
    }

    return str;
}


//格式化输出到一个缓冲区
//%[flags][width][.precision][length]specifier
//specifier     Output                                                      Example
//c             Character                                                   a
//d or i        Signed decimal integer                                      392
//e             Scientific notation (mantise/exponent) using e character    3.9265e+2
//E             Scientific notation (mantise/exponent) using E character    3.9265E+2
//f             Decimal floating point                                      392.65
//g             Use the shorter of %e or %f                                 392.65
//G             Use the shorter of %E or %f                                 392.65
//o             Unsigned octal                                              610
//s             String of characters                                        sample
//u             Unsigned decimal integer                                    7235
//x             Unsigned hexadecimal integer                                7fa
//X             Unsigned hexadecimal integer (capital letters)              7FA
//p             Pointer address                                             B800:0000
//n             Nothing printed. The argument must be a pointer to a signed int, where the number of characters written so far is stored.
//%             A % followed by another % character will write % to stdout. %

//flags         description
//-             Left-justify within the given field width; Right justification is the default (see width sub-specifier).
//+             Forces to precede the result with a plus or minus sign (+ or -) even for positive numbers. By default, only negative numbers are preceded with a - sign.
//(space)       If no sign is going to be written, a blank space is inserted before the value.
//#             Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X respectively for values different than zero.
//              Used with e, E and f, it forces the written output to contain a decimal point even if no digits would follow. By default, if no digits follow, no decimal point is written.
//              Used with g or G the result is the same as with e or E but trailing zeros are not removed.
//0             Left-pads the number with zeroes (0) instead of spaces, where padding is specified (see width sub-specifier).

//width         description
//(number)      Minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is not truncated even if the result is larger.
//*             The width is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.


//.precision    description
//.number       For integer specifiers (d, i, o, u, x, X): precision specifies the minimum number of digits to be written. If the value to be written is shorter than this number, the result is padded with leading zeros. The value is not truncated even if the result is longer. A precision of 0 means that no character is written for the value 0.
//              For e, E and f specifiers: this is the number of digits to be printed after the decimal point.
//              For g and G specifiers: This is the maximum number of significant digits to be printed.
//              For s: this is the maximum number of characters to be printed. By default all characters are printed until the ending null character is encountered.
//              For c type: it has no effect.
//              hen no precision is specified, the default is 1. If the period is specified without an explicit value for precision, 0 is assumed.
//.*            The precision is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.


//length        description
//h             The argument is interpreted as a short int or unsigned short int (only applies to integer specifiers: i, d, o, u, x and X).
//l             The argument is interpreted as a long int or unsigned long int for integer specifiers (i, d, o, u, x and X), and as a wide character or wide character string for specifiers c and s.
//L             The argument is interpreted as a long double (only applies to floating point specifiers: e, E, f, g and G).

#if 0
int a_sprintf(char *buf, const char *fmt, va_list args)
{
    char *str = NULL;
    int field_width = 0;                //Width of output field

    for(str = buf; *fmt; fmt++)
    {
        unsigned long num = 0;
        int base = 10;
        int flags = 0;              //Flags to number()    Process flags
        int qualifier = -1;         //'h', 'l', or 'L' for integer fields
        int precision = -1;         //Min. # of digits for integers; max number of chars for from string
        BOOL bFmt = TRUE;

        //检测%
        if(*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        bFmt = TRUE;
        //flags
        while(bFmt)
        {
            fmt++;                  //跳过刚刚检测到的%
            switch(*fmt)
            {
                case '-':
                    flags |= LEFT;
                    break;
                case '+':
                    flags |= PLUS;
                    break;
                case ' ':
                    flags |= SPACE;
                    break;
                case '#':
                    flags |= SPECIAL;
                    break;
                case '0':
                    flags |= ZEROPAD;
                    break;
                default:
                    bFmt = FALSE;
            }
        }

        //width
        field_width = -1;
        if(a_is_digit(*fmt))            //number表示宽度
        {
            field_width = skip_atoi(&fmt);
        }
        else if('*' == *fmt)            //*表示宽度
        {
            fmt++;
            field_width = va_arg(args, int);
            if(field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        //precision
        precision = -1;
        if('.' == *fmt)
        {
            ++fmt;                      //跳过.
            if(a_is_digit(*fmt))
            {
                precision = skip_atoi(&fmt);
            }
            else if('*' == *fmt)
            {
                ++fmt;
                precision = va_arg(args, int);
            }
            if(precision < 0)
            {
                precision = 0;
            }
        }

        //length
        qualifier = -1;
        if('h' == *fmt || 'l' == *fmt || 'L' == *fmt)
        {
            qualifier = *fmt;
            fmt++;
        }

        //specifier:base
        base = STD_LIB_FMT_DEC;
        switch(*fmt)
        {
            case 'c':
            {
                if(!(flags & LEFT))
                {
                    while(--field_width > 0)
                    {
                        *str++ = ' ';
                    }
                }
                *str++ = (unsigned char)va_arg(args, int);
                while(--field_width > 0)
                {
                    *str++ = ' ';
                }
                continue;
            }
            case 's':
            {
                int len = 0;
                int i = 0;
                char *s = va_arg(args, char *);
                if(!s)
                {
                    s = "<NULL>";
                }
                len = a_strlen((u8 const *)s);
                //如果字符串长度大于显示长度，则只显示设定的显示长度，多余部分不显示
                if(len > precision)
                {
                    len = precision;
                }
                //空余部分显示空格
                if(!(flags & LEFT))
                {
                    while(len < field_width--)
                    {
                        *str++ = ' ';
                    }
                }

                for(i = 0; i < len; ++i)
                {
                    *str++ = *s++;
                }

                while(len < field_width--)
                {
                    *str++ = ' ';
                }
                continue;
            }
            case 'p':
            {
                if(-1 == field_width)
                {
                    field_width = 2 * sizeof(void *);
                    flags |= ZEROPAD;
                }
                str = number(str, (unsigned long) va_arg(args, void *), 16, field_width, precision, flags);
                continue;
            }
            case 'n':
            {
                if ('l' == qualifier)
                {
                    long *ip = va_arg(args, long *);
                    *ip = (str - buf);
                }
                else
                {
                    int *ip = va_arg(args, int *);
                    *ip = (str - buf);
                }
                continue;
            }
            //case 'A':
            //{
            //    flags |= LARGE;                 //no break
            //}
            //case 'a':
            //{
            //    if ('l' == qualifier)
            //    {
            //        str = eaddr(str, va_arg(args, unsigned char *), field_width, precision, flags);
            //    }
            //    else
            //    {
            //        str = iaddr(str, va_arg(args, unsigned char *), field_width, precision, flags);
            //    }
            //    continue;
            //}
            //Integer number formats - set up the flags and "break"
            case 'o':
            {
                base = 8;
                break;
            }
            case 'X':
            {
                flags |= LARGE; /* no break */
            }
            case 'x':
            {
                base = 16;
                break;
            }
            case 'd':
            case 'i':
            {
                flags |= SIGN; /* no break */
            }
            case 'u':
            {
                break;
            }
#ifndef NOFLOAT
            case 'E':
            case 'G':
            case 'e':
            case 'f':
            case 'g':
            {
                str = flt(str, va_arg(args, double), field_width, precision, *fmt, flags | SIGN);
                continue;
            }
#endif
            default:
            {
                if(*fmt != '%')
                {
                    *str++ = '%';
                }

                if(*fmt)
                {
                    *str++ = *fmt;
                }
                else
                {
                    --fmt;
                }
                continue;
            }
        }  /* end of switch (*fmt) */

        if(qualifier == 'l')
        {
            num = va_arg(args, unsigned long);
        }
        else if(qualifier == 'h')
        {
            if (flags & SIGN)
            {
                num = va_arg(args, short);
            }
            else
            {
                num = va_arg(args, unsigned short);
            }
        }
        else if(flags & SIGN)
        {
            num = va_arg(args, int);
        }
        else
        {
            num = va_arg(args, unsigned long);
        }

        str = number(str, num, base, field_width, precision, flags);
    } //end of for (str = buf; *fmt; fmt++)

    *str = '\0';
    return str - buf;
}


int a_printf(char *buf, const char *fmt, ...)
{
    va_list args;
    int n = 0;

    va_start(args, fmt);
    n = a_sprintf(buf, fmt, args);
    va_end(args);

    return n;
}
#endif
void a_xor(void *p1, const void *p2, u32 len)
{
    u8 * pchstr1 = (u8*)p1;
    u8 * pchstr2 = (u8*)p2;
    u8 i = 0;
    while(len--)
    {
        pchstr1[i] = pchstr1[i] ^ pchstr2[i];
        i++;
    }
}


