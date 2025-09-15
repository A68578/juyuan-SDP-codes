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

BufReq_ReturnType Dcm_CopyTxData(PduIdType txPduID,const PduInfoType* PduInfo,const RetryInfoType* retry,
	PduLengthType* availableDataPtr
)
{
	printf("Task  Dcm_CopyTxData\n");
	BufReq_ReturnType copy_result = BUFREQ_E_NOT_OK;
	unsigned short BufferTrans = 0;
	unsigned char* sourcebuffer = 0;
	*availableDataPtr = 8;
	if ((*(PduInfo->SduDataPtr) & 0xF0) == 0)
	{
		sourcebuffer = SingleFrame;
	}
	else if ((*(PduInfo->SduDataPtr) & 0xF0) == 1)
	{
		sourcebuffer = FirstFrame;
	}
	else if ((*(PduInfo->SduDataPtr) & 0xF0) == 2)
	{
		sourcebuffer = ConsecutiveFrame1;
	}
	else if ((*(PduInfo->SduDataPtr) & 0xF0) == 3)
	{
		sourcebuffer = ControlFollowFrameCTS;
	}
	if (sourcebuffer)
	{
		if (*availableDataPtr > 0 && PduInfo->SduLength <= *availableDataPtr)
		{
			memcpy(PduInfo->SduDataPtr, sourcebuffer, PduInfo->SduLength);


			*availableDataPtr = *availableDataPtr - PduInfo->SduLength;


			copy_result = BUFREQ_OK;
		}
	}
	else
	{
		copy_result = BUFREQ_E_NOT_OK;
	}

	return copy_result;
}
