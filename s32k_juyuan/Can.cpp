#include "Can.h"
void Can_Init(void)
{
	printf("task Can Init\n");
	for (int i = 0;i<sizeof(CanBuffer);i++)
	{
		CanBuffer[i] = 0;
	}
}

void Can_Rx_Callback(unsigned char Hrh,Can_IdType CanId,unsigned char CanDataLegth,const unsigned char* CanSduPtr)
{
	//CanIf_TxConfirmation();
}


void fakeDriver_Simulate(void)
{
	//Can_Rx_Callback();
}

void Can_Write(Can_PduType* CanData)
{
	//CanBuffer[] = CanData->sdu;
}