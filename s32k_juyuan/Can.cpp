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

static Can_ReturnType Can_Check(unsigned char* canbuffer)
{
	Can_ReturnType check_result = CAN_NOT_OK;
	unsigned char* checkbuffer = 0;
	if ((*(canbuffer) & 0xF0) == 0)
	{
		checkbuffer = SingleFrame;
	}
	else if ((*(canbuffer) & 0xF0) == 1)
	{
		checkbuffer = FirstFrame;
	}
	else if ((*(canbuffer) & 0xF0) == 2)
	{
		checkbuffer = ConsecutiveFrame1;
	}
	else if ((*(canbuffer) & 0xF0) == 3)
	{
		checkbuffer = ControlFollowFrameCTS;
	}

	/*SF*/
	for (unsigned int i = 0; i < 8; i++)
	{
		if (*checkbuffer == *canbuffer)
		{
			checkbuffer++;
			canbuffer++;
			check_result = CAN_OK;
		}
		else
		{
			check_result = CAN_NOT_OK;
			break;
		}
	}
	return check_result;
}


Can_ReturnType Can_Write(Can_PduType* CanData)
{
	Can_ReturnType can_result = CAN_NOT_OK;
	unsigned int len = CanData->length;
	
	for (unsigned int i = 0;i<len;i++)
	{
		CanBuffer[i] = *(CanData->sdu++);
	}
	if (CAN_OK == Can_Check(CanBuffer))
	{
		can_result = CAN_OK;
	}
	else
	{
		can_result = CAN_NOT_OK;
	}
	return can_result;
}

void Can_Rx_Callback(PduInfoType* buffer)
{
	CanIf_RxIndication(buffer);

}


void fakeDriverRxSF_Simulate(void)
{
	memset(CanBuffer, 0, sizeof(CanBuffer));
	PduInfoType* sourcepdu = (PduInfoType*)malloc(sizeof(sourcepdu));
	if (sourcepdu)
	{
		sourcepdu->SduDataPtr = SingleFrame;
		sourcepdu->SduLength = 5;
	}

	Can_Rx_Callback(sourcepdu);

}