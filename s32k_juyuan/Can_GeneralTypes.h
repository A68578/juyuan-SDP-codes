#pragma once
#include "ComStack_Types.h"
typedef uint32 Can_IdType;
typedef uint16 Can_HwHandleType;

typedef struct
{
    Can_IdType CanId;
    Can_HwHandleType Hoh;
    uint8 ControllerId;
}Can_HwType;


typedef struct
{
    PduIdType swPduHandle;
    uint8 length;
    Can_IdType id;
    uint8* sdu;
}Can_PduType;