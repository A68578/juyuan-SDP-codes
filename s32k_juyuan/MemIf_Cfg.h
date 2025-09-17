#pragma once

#include "common.h"

#define IMemIf_FLAGS_NUMBER					3
#define IMemIf_FLAGS_APPLICATION_ID			0x00
#define IMemIf_FLAGS_REPROGRAM_ID			0x01
#define IMemIf_FLAGS_RESET_ID				0x02
#define IFLAGS_appvalid						0x10000000UL
#define IFLAGS_reprogram				    0x10002000UL
#define IFLAGS_reset					    0x10004000UL

#define IMemIf_APPLICATION_START_ADDR        0x00500000UL
#define IMemIf_APPLICATION_LIMIT_ADDR        0x007D4000UL

#define IMemIf_RAM0_START_ADDR               0x20428000UL
#define IMemIf_RAM0_LIMIT_ADDR               0x20450000UL

#define IMemIf_FLASHDRIVER_SIZE             0x2000U
#define Mem_NULL_PTR                       (MemIfFuncPtrType)0

typedef void (*MemIf_Callback)(void);



typedef struct
{
    uint32 IMemIfConfig_Addr;
    uint32 IMemIfConfig_Len;
    uint32 IMemIfConfig_Flag;
}IMemIfConfigListType;



typedef struct
{
    uint32 MemIf_Addr;
    uint32 MemIf_Len;
    uint32* MemIf_Buf;
    MemIf_Callback MemIfWatchDog;
    MemIf_Callback MemIfNRC78;
}MemIf_DataPtr;

typedef Std_ReturnType(*MemIfFuncPtrType)(MemIf_DataPtr* ptr);


typedef struct
{
    MemIfFuncPtrType MemIfWriteFuncPtr;
    MemIfFuncPtrType  MemIfReadFuncPtr;
    MemIfFuncPtrType MemIfEraseFuncPtr;
}IMemIfFuncPtrListType;


extern IMemIfConfigListType IMemIfConfigList[3];
extern IMemIfFuncPtrListType IMemIfFuncPtrList[3];