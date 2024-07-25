#ifndef _ANYID_VS10XX_
#define _ANYID_VS10XX_

#if defined(_ANYID_BCTRL_)
    #include "AnyID_BCTRL_VS10XX_HL.h"
#endif

#define VS10XX_WRITE_COMMAND 0x02
#define VS10XX_READ_COMMAND 0x03

extern u8 g_nMp3ErrorTime;
extern u8 g_nMp3VolVal;

#define VS10XX_SPI_MODE	0x0
#define VS10XX_SPI_STATUS	0x1
#define VS10XX_SPI_BASS	0x2
#define VS10XX_SPI_CLOCKF	0x3
#define VS10XX_SPI_DECODE_TIME	0x4
#define VS10XX_SPI_AUDATA	0x5
#define VS10XX_SPI_WRAM	0x6
#define VS10XX_SPI_WRAMADDR	0x7
#define VS10XX_SPI_HDAT0	0x8
#define VS10XX_SPI_HDAT1	0x9
#define VS10XX_SPI_AIADDR	0xa
#define VS10XX_SPI_VOL		0xb
#define VS10XX_SPI_AICTRL0	0xc
#define VS10XX_SPI_AICTRL1	0xd
#define VS10XX_SPI_AICTRL2	0xe
#define VS10XX_SPI_AICTRL3	0xf

#define VS10XX_SM_DIFF		0x01
#define VS10XX_SM_JUMP		0x02
#define VS10XX_SM_RESET	0x04
#define VS10XX_SM_OUTOFWAV	0x08
#define VS10XX_SM_PDOWN	0x10
#define VS10XX_SM_TESTS	0x20
#define VS10XX_SM_STREAM	0x40
#define VS10XX_SM_PLUSV	0x80
#define VS10XX_SM_DACT		0x100
#define VS10XX_SM_SDIORD	0x200
#define VS10XX_SM_SDISHARE	0x400
#define VS10XX_SM_SDINEW	0x800
#define VS10XX_SM_ADPCM        0x1000
#define VS10XX_SM_ADPCM_HP     0x2000

#define VS10XX_MINSENDTOMP3  32
#define VS10XX_VOL_DEFAULT   16
#define VS10XX_VOL_MIN           64
#define VS10XX_VOL_MAX           0
#define VS10XX_VOL_STEP          16

#define VS10XX_ERROR_TIME		200 //µ¥Î»£º10ms

u16 VS10XX_ReadRegister(u8 addressbyte);

void VS10XX_SoftReset(void);
void VS10XX_Reset(void);
void VS10XX_VsSineTest(void);
void VS10XX_SendZeros(void);
void VS10XX_Delay(u16 mS);

void VS10XX_WriteRegister(u8 addressbyte,u8 highbyte,u8 lowbyte);
void VS10XX_Send32ByteData(u8 *pBuffer);
void VS10XX_SetVolumn(u8 vol);
#endif
