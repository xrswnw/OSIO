#include "AnyID_EM4094.h"

void EM4094_Init(void)
{
    EM4094_WriteData(0x02804033);
}
