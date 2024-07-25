#ifndef _ANYID_CR95HF_FELICA_
#define _ANYID_CR95HF_FELICA_

#include "AnyID_CR95HF.h"

u8 Felica_Init(void);
u8 Felica_Polling(u8 *pUid, u16 systemCode, u8 slot);
u8 Felica_Dtu(void);

#define FELICA_CMD_REQC             0x00U   //RequestC.
#define FELICA_CMD_REQ_RESP         0x04U   //Request Response.
#define FELICA_CMD_REQ_SERVICE      0x02U   //Request Service.
#define FELICA_CMD_READ             0x06U   //Read.
#define FELICA_CMD_WRITE            0x08U   //Write.
#define FELICA_CMD_AUTH1            0x10U   //Authentication1.
#define FELICA_CMD_AUTH2            0x12U   //Authentication2.
#define FELICA_CMD_READ_SECF        0x14U   //Read from secure file.
#define FELICA_CMD_WRITE_SECF       0x16U   //Write to secure file.
#define FELICA_RSP_REQC             0x01U   //Response to RequestC.

#define FELICA_NUMSLOTS_1           0x00    //< Value for \c bNumSlots = 1. 
#define FELICA_NUMSLOTS_2           0x01    //< Value for \c bNumSlots = 2. 
#define FELICA_NUMSLOTS_4           0x03    //< Value for \c bNumSlots = 4. 
#define FELICA_NUMSLOTS_8           0x07    //< Value for \c bNumSlots = 8. 
#define FELICA_NUMSLOTS_16          0x0F    //< Value for \c bNumSlots = 16. 


#define FELICA_POLLING_RFU          0x00

#define FELICA_POLLING_TIME         24170
#define FELICA_SLOT_TIME            12080

#define FELICA_UID_LEN              16
#define FELICA_BLOCK_LEN            16

#define FELICA_SYSTEM_CODE          0xFFFF
#endif

