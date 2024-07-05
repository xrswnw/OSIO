#include "AnyID_SM5003_Reader.h"

const u8 READER_VERSION[READER_VERSION_SIZE]@0x08005000 = "SM5003 23042800 220800";

void Reader_Delayms(u32 n)
{
    n *= 0x3800;
    n++;
    while(n--);
}

void Reader_Init(void)
{
    
}