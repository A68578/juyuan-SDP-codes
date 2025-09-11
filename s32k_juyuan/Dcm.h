#pragma once
#include "ComStack_Types.h"
#include "common.h"
extern void DCM_10DelayCancel(void);
extern void Dcm_Init(void);
extern void Dcm_TpRxIndication(PduIdType id, Std_ReturnType result);
extern void Dcm_TpTxConfirmation(PduIdType txPduID, Std_ReturnType result);
extern BufReq_ReturnType Dcm_CanTpStartOfReception(PduIdType rxPduID, const PduInfoType* PduInfo,
	PduLengthType TpSduLength, PduLengthType* bufferSizePtr);
extern BufReq_ReturnType Dcm_CopyRxData(PduIdType rxPduID, const PduInfoType* PduInfo,
	unsigned short* bufferSizePtr);