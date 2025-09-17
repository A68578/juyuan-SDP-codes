#include "Dcm.h"


uint8 DcmlocalSFBuffer[8];
uint8 DcmlocalFFBuffer[8];
uint8 DcmRspSFBuffer[8];
Std_ReturnType DcmLocalConfirm = E_NOT_OK;

static void Comm_CopyData(void* des, void* src, uint32 srcSize)
{
	uint8* pd, * ps;
	uint32 i;

	if ((des != src) && (srcSize != 0))
	{
		pd = (uint8*)des;
		ps = (uint8*)src;
		for (i = 0; i <= srcSize; i++)
		{
			pd[i] = ps[i];
		}
	}
}

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
	DcmLocalConfirm = result;
}

void DcmResetConfirmFlag(void)
{
	Std_ReturnType DcmLocalConfirm = E_NOT_OK;
}

void Dcm_TpTxConfirmation(PduIdType txPduID, Std_ReturnType result)
{
	printf("Task Dcm TpTxConfirmation£¬the result is: %d\n", result);
}

BufReq_ReturnType Dcm_CanTpStartOfReception(PduIdType rxPduID, const PduInfoType* PduInfo,
	PduLengthType TpSduLength, PduLengthType* bufferSizePtr)
{
	printf("Task Dcm_CanTpStartOfReception\n");
	if (!bufferSizePtr)
	{
		return BUFREQ_E_NOT_OK;
	}
	else
	{
		*bufferSizePtr = 8;
	}
	
	
	
	
	return BUFREQ_OK;
}

BufReq_ReturnType Dcm_CopyRxData(PduIdType rxPduID, const PduInfoType* PduInfo,
	uint16* bufferSizePtr)
{
	printf("Task  Dcm_CopyRxData\n");
	
	memcpy(DcmlocalSFBuffer, PduInfo->SduDataPtr, *bufferSizePtr);
	
	return BUFREQ_OK;
}

BufReq_ReturnType Dcm_CopyTxData(PduIdType txPduID,const PduInfoType* PduInfo,const RetryInfoType* retry,
	PduLengthType* availableDataPtr
)
{
	printf("Task  Dcm_CopyTxData\n");
	BufReq_ReturnType copy_result = BUFREQ_E_NOT_OK;

	memcpy(PduInfo->SduDataPtr, DcmRspSFBuffer, PduInfo->SduLength);

	copy_result = BUFREQ_OK;
	
	return copy_result;
}
