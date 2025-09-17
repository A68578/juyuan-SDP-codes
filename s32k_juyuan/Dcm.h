#pragma once
#define DEBUG 1
#include "ComStack_Types.h"
#include "common.h"
#include "CanTp.h"
#include "Dcm_Types.h"
#include "sourceswc.h"



#if DEBUG
#include "string.h" 
#include "stdlib.h"
#endif



extern void DCM_10DelayCancel(void);
extern void Dcm_Init(void);
extern void Dcm_TpRxIndication(PduIdType id, Std_ReturnType result);
extern void Dcm_TpTxConfirmation(PduIdType txPduID, Std_ReturnType result);
extern BufReq_ReturnType Dcm_CanTpStartOfReception(PduIdType rxPduID, const PduInfoType* PduInfo,
	PduLengthType TpSduLength, PduLengthType* bufferSizePtr);
extern BufReq_ReturnType Dcm_CopyRxData(PduIdType rxPduID, const PduInfoType* PduInfo,
	uint16* bufferSizePtr);
BufReq_ReturnType Dcm_CopyTxData(PduIdType txPduID, const PduInfoType* PduInfo, const RetryInfoType* retry,
	PduLengthType* availableDataPtr
);
extern void DcmResetConfirmFlag(void);


extern uint8 DcmlocalSFBuffer[8];
extern uint8 DcmlocalFFBuffer[8];
extern uint8 DcmRspSFBuffer[8];
extern Std_ReturnType DcmLocalConfirm;