#include "Can.h"
void Can_Init(void)
{
	printf("task Can Init\n");
	for (int i = 0;i<sizeof(CanBuffer);i++)
	{
		CanBuffer[i] = 0;
	}
}


void Can_Tx_Callback(void)
{
	CanIf_TxConfirmation(0);
}



void fakeDriverTxConfirmation_Simulate(void)
{
	Can_Tx_Callback();
}



Can_ReturnType Can_Write(Can_PduType* CanData)
{
	uint32 len = CanData->length;
	
	for (uint32 i = 0;i<len;i++)
	{
		CanBuffer[i] = *(CanData->sdu++);
	}
	return CAN_OK;
}

void Can_Rx_Callback(PduInfoType* buffer)
{
	CanIf_RxIndication(buffer);

}


void fakeDriverRxSF_Simulate(void)
{
	PduInfoType* sourcepdu = (PduInfoType*)malloc(sizeof(sourcepdu));
	uint8* sdutempptr = 0;
	sourcepdu->SduDataPtr = (uint8*)malloc(sizeof(char));
	sdutempptr = sourcepdu->SduDataPtr;
	if (sdutempptr)
	{
		*sdutempptr = ISOTP_NPCI_SF | sizeof(SingleDcm10Frame);
		sdutempptr++;
		memcpy(sdutempptr,SingleDcm10Frame,sizeof(SingleDcm10Frame));
	}
	sourcepdu->SduLength = sizeof(SingleDcm10Frame);

	Can_Rx_Callback(sourcepdu);

}


void fakeDriverRx22ReqSession_Simulate(void)
{
	PduInfoType* sourcepdu = (PduInfoType*)malloc(sizeof(sourcepdu));
	uint8* sdutempptr = 0;
	sourcepdu->SduDataPtr = (uint8*)malloc(sizeof(char));
	sdutempptr = sourcepdu->SduDataPtr;
	if (sdutempptr)
	{
		*sdutempptr = ISOTP_NPCI_SF | sizeof(SingleDcm22ReqFrame);
		sdutempptr++;
		memcpy(sdutempptr, SingleDcm22ReqFrame, sizeof(SingleDcm22ReqFrame));
	}
	sourcepdu->SduLength = sizeof(SingleDcm22ReqFrame);

	Can_Rx_Callback(sourcepdu);
}


void fakeDriverRxFlowControlFrame_Simulate(void)
{
	PduInfoType* sourcepdu = (PduInfoType*)malloc(sizeof(sourcepdu));
	uint8* sdutempptr = 0;
	sourcepdu->SduDataPtr = (uint8*)malloc(sizeof(char));
	sdutempptr = sourcepdu->SduDataPtr;
	if (sdutempptr)
	{
		memcpy(sdutempptr, FlowControlFrame, sizeof(FlowControlFrame));
	}

	sourcepdu->SduLength = sizeof(FlowControlFrame);

	Can_Rx_Callback(sourcepdu);


}






void fakeDriverRx22ReadSession_Simulate(void)
{

}