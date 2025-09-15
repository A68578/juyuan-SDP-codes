#pragma once
#include "CanIf_Cfg.h"
#include "Can.h"


typedef struct {
    int dummy; //never used
} CanIf_ConfigType;



extern void CanIf_Init(const CanIf_ConfigType* ConfigPtr);
extern Std_ReturnType CanIf_Transmit(const PduInfoType* PduInfoPtr);
extern void CanIf_TxConfirmation(PduIdType CanTxPduId);
extern void CanIf_RxIndication(const PduInfoType* PduInfoPtr);