#include "Dcm.h"


unsigned char DcmlocalSFBuffer[8];
unsigned char DcmlocalFFBuffer[8];

static void Comm_CopyData(void* des, void* src, unsigned int srcSize)
{
	unsigned char* pd, * ps;
	unsigned int i;

	if ((des != src) && (srcSize != 0))
	{
		pd = (unsigned char*)des;
		ps = (unsigned char*)src;
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
}

void Dcm_TpTxConfirmation(PduIdType txPduID, Std_ReturnType result)
{
	printf("Task Dcm TpTxConfirmation£¬the result is: %d\n", result);
}

BufReq_ReturnType Dcm_CanTpStartOfReception(PduIdType rxPduID, const PduInfoType* PduInfo,
	PduLengthType TpSduLength, PduLengthType* bufferSizePtr)
{
	printf("Task Dcm_CanTpStartOfReception\n");
	return BUFREQ_OK;
}

BufReq_ReturnType Dcm_CopyRxData(PduIdType rxPduID, const PduInfoType* PduInfo,
	unsigned short* bufferSizePtr)
{
	printf("Task  Dcm_CopyRxData\n");
	return BUFREQ_OK;
}

void copydata_to_dcmsf(void)
{
	memcpy(DcmlocalSFBuffer, SingleDcm10Frame, sizeof(SingleDcm10Frame));
}

BufReq_ReturnType Dcm_CopyTxData(PduIdType txPduID,const PduInfoType* PduInfo,const RetryInfoType* retry,
	PduLengthType* availableDataPtr
)
{
	printf("Task  Dcm_CopyTxData\n");
	BufReq_ReturnType copy_result = BUFREQ_E_NOT_OK;

	copydata_to_dcmsf();

	memcpy(PduInfo->SduDataPtr, DcmlocalSFBuffer, PduInfo->SduLength);

	copy_result = BUFREQ_OK;
	
	return copy_result;
}
