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
	PduInfoType	SF_PduInfo;
	SF_PduInfo.SduLength = sizeof(SingleDcm10Frame);
	SF_PduInfo.SduDataPtr = SingleDcm10Frame;


	transreqresult = CanTp_Transmit(0, &SF_PduInfo);

	return transreqresult;

}




