#ifndef _ANYID_TYPE_
#define _ANYID_TYPE_

#include "string.h"


//µÍ¹¦ºÄµÈ¼¶£º»½ÐÑ£¬ÐÝÃß£¬¹Ø»ú
typedef enum{LPM_SLEEP = 0x01, LPM_WAKEUP = 0x02, LPM_OFF = 0x04}LPMode;

//LEDµÆ×´Ì¬
typedef enum{LED_OFF = 0, LED_ON = 1}LEDState;

//µç³ØµçÑ¹×´Ì¬
typedef enum{BVS_LV = 0, BVS_NORMAL = 1, BVS_KEEP = 2}BatVolState;

//ÇÐ»»×´Ì¬£ºÎ¬³Ö£¬ÇÐ»»
typedef enum{SWS_KEEP = 0, SWS_SWITCH = 1}SwitchState;

//Êý¾ÝÀàÐÍ
typedef enum{DAT_FMT_8 = 0x01, DAT_FMT_16 = 0x02, DAT_FMT_32 = 0x04, DAT_FMT_64 = 0x08}DAT_FMT_TYPE;

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef null
#define null ((void *)0)
#endif

#ifndef EXTERN
#define EXTERN extern
#endif

typedef signed long                         s32;
typedef signed short                        s16;
typedef signed char                         s8;

typedef signed long  const                 sc32;			
typedef signed short const                 sc16;				
typedef signed char  const                 sc8;					

typedef volatile signed long               vs32;
typedef volatile signed short              vs16;
typedef volatile signed char               vs8;

typedef volatile signed long  const       vsc32;		
typedef volatile signed short const       vsc16;		
typedef volatile signed char  const       vsc8;		

typedef unsigned long                       u32;
typedef unsigned short                      u16;
typedef unsigned char                       u8;

typedef unsigned long  const               uc32;				
typedef unsigned short const               uc16;				
typedef unsigned char  const               uc8;				

typedef volatile unsigned long             vu32;
typedef volatile unsigned short            vu16;
typedef volatile unsigned char             vu8;

typedef volatile unsigned long  const      vuc32;	
typedef volatile unsigned short const      vuc16;	
typedef volatile unsigned char  const      vuc8;	

typedef unsigned char                        BYTE;
typedef unsigned char                        byte;

typedef unsigned short                       word;
typedef unsigned short                       WORD;

typedef unsigned long                        DWORD;

typedef unsigned long long                  u64;


enum {BIT_RESET, BIT_SET};
typedef enum{FALSE = 0, TRUE = !FALSE}BOOL;
//typedef enum{false = 0, true = !false}bool;
#ifndef _ANYID_STM32L4_
//typedef enum{RESET = 0, SET = 1}BitState, FlagStatus, ITStatus, BitAction, BitStatus;
typedef enum{RESET = 0, SET = 1}BitState, FlagStatus, ITStatus;
typedef enum{DISABLE = 0, ENABLE = 1}EnState, FunctionalState;

typedef enum{ERROR = 0, SUCCESS = 1}ErrorState, ResultState, ErrorStatus;
#endif


#define U8_MAX     ((u8)255)
#define S8_MAX     ((s8)127)
#define S8_MIN     ((s8)-128)
#define U16_MAX    ((u16)65535u)
#define S16_MAX    ((s16)32767)
#define S16_MIN    ((s16)-32768)
#define U32_MAX    ((u32)4294967295uL)
#define S32_MAX    ((s32)2147483647)
#define S32_MIN    ((s32)2147483648uL)

#endif

