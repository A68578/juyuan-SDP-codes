#pragma once
#include "CanIf_Cfg.h"
typedef struct {
    int dummy; //never used
} CanIf_ConfigType;

typedef unsigned int Can_IdType;
typedef unsigned short Can_HwHandleType;

typedef struct
{
    Can_IdType CanId;
    Can_HwHandleType Hoh;
    unsigned char ControllerId;
}Can_HwType;


extern void CanIf_Init(const CanIf_ConfigType* ConfigPtr);
extern Std_ReturnType CanIf_Transmit(PduIdType TxPduId,const PduInfoType* PduInfoPtr);
extern void CanIf_TxConfirmation(PduIdType CanTxPduId);
extern void CanIf_RxIndication(const Can_HwType* Mailbox,const PduInfoType* PduInfoPtr);