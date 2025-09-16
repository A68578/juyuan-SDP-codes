#include "Dcm_Appl.h"
void SendReprogramRespond(void)
{
	printf("task send reprogram response\n");
}

void SetReprogramState(void)
{
	printf("task send program state\n");
}

void SendResponseMCUReset(void)
{
	printf("task send response to mcu reset\n");
}

Std_ReturnType virtual_send_SF(void)
{
	Std_ReturnType transreqresult = E_NOT_OK;
	PduInfoType* SF_PduInfo = new PduInfoType;
	SF_PduInfo->SduDataPtr = new unsigned char;

	SF_PduInfo->SduDataPtr[0] = 0x50;
	SF_PduInfo->SduDataPtr[1] = 0x01;
	
	SF_PduInfo->SduLength = 2;

	memcpy(DcmRspSFBuffer, SF_PduInfo->SduDataPtr, SF_PduInfo->SduLength);

	transreqresult = CanTp_Transmit(0, SF_PduInfo);

	return transreqresult;

}




