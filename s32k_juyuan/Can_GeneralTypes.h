#pragma once
#include "ComStack_Types.h"
typedef unsigned int Can_IdType;
typedef unsigned short Can_HwHandleType;

typedef struct
{
    Can_IdType CanId;
    Can_HwHandleType Hoh;
    unsigned char ControllerId;
}Can_HwType;


typedef struct
{
    PduIdType swPduHandle;
    unsigned char length;
    Can_IdType id;
    unsigned char* sdu;
}Can_PduType;