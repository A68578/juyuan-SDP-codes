#include "Dcm.h"
void DCM_10DelayCancel(void)
{
	printf("Task Dcm 10 Delay Cancel\n");
}
void Dcm_Init(void)
{
	printf("Task Dcm Init\n");
}


void Dcm_TpRxIndication(PduIdType id, Std_ReturnType result)
{
	printf("Task Dcm TpRxIndication\n");
}

void Dcm_TpTxConfirmation(PduIdType txPduID, Std_ReturnType result)
{
	printf("Task Dcm TpTxConfirmation\n");
}

BufReq_ReturnType Dcm_CanTpStartOfReception(PduIdType rxPduID, const PduInfoType* PduInfo,
	PduLengthType TpSduLength, PduLengthType* bufferSizePtr)
{
	printf("Task Dcm_CanTpStartOfReception\n");
	Req_Resulturn BUFREQ_OK;
}

BufReq_ReturnType Dcm_CopyRxData(PduIdType rxPduID, const PduInfoType* PduInfo,
	unsigned short* bufferSizePtr)
{
	printf("Task  Dcm_CopyRxData\n");
	Req_Resulturn BUFREQ_OK;
}