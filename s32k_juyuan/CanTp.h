#pragma once
#include "common.h"
#include "CanTp_Cfg.h"
#include "CanTp_Types.h"
#include "CanTp_Det.h"
#include "Dcm.h"



typedef struct {
    int dummy; //never used
} CanTp_ConfigType;

typedef uint32 PduIdInfoType;

#define CanTp_NULL_PTR (CanTp_ConfigType*)0

extern void CanTp_Init(const CanTp_ConfigType* CfgPtr);
extern Std_ReturnType CanTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
extern void CanTp_MainFunction(void);
extern BufReq_ReturnType CanTp_SendNextTxFrame(const CanTpTxNSduType* txNSdu, CanTp_Change_InfoType* txRuntime);
extern void CanTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);
extern void CanTp_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);