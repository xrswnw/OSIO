#ifndef _ANYID_SI2C_Driver_
#define _ANYID_SI2C_Driver_

#include "AnyID_config.h"

#define I2C_WRITE_ADDR      0xFE
#define I2C_READ_ADDR       0x01

BOOL I2C_Start(void);
void I2C_Stop(void);

void I2C_Ack(void);
void I2C_NoAck(void);

BOOL I2C_WaitAck(void);
u8 I2C_ReceiveByte(void);
void I2C_SendByte(u8 SendByte);

void I2C_Init(void);
void I2C_LowPower(LPMode lp);
#endif

