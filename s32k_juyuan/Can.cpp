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



void fakeDriver_Simulate(void)
{
	Can_Tx_Callback();
}



Can_ReturnType Can_Write(Can_PduType* CanData)
{
	//Can_ReturnType can_result = CAN_NOT_OK;
	unsigned int len = CanData->length;
	
	for (unsigned int i = 0;i<len;i++)
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
	unsigned char* sdutempptr = 0;
	sourcepdu->SduDataPtr = (unsigned char*)malloc(sizeof(char));
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