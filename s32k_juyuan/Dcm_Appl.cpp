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
	if (DcmLocalConfirm == E_OK)
	{
		PduInfoType* SF_PduInfo = new PduInfoType;
		SF_PduInfo->SduDataPtr = new uint8;

		SF_PduInfo->SduDataPtr[0] = 0x50;
		SF_PduInfo->SduDataPtr[1] = 0x01;

		SF_PduInfo->SduLength = 2;

		memcpy(DcmRspSFBuffer, SF_PduInfo->SduDataPtr, SF_PduInfo->SduLength);

		transreqresult = CanTp_Transmit(0, SF_PduInfo);
	}
	return transreqresult;
}

Std_ReturnType virtual_send_22ReqResponseFF(void)
{
	Std_ReturnType transreqresult = E_NOT_OK;
	if (DcmLocalConfirm == E_OK)
	{
		PduInfoType* Multi_PduInfo = new PduInfoType;
		Multi_PduInfo->SduDataPtr = new uint8;
		Multi_PduInfo->SduDataPtr[0] = 0x62;
		Multi_PduInfo->SduDataPtr[1] = 0xF1;
		Multi_PduInfo->SduDataPtr[2] = 0x90;
		
		for (uint32 i = 0;i < 6;i++)
		{
			Multi_PduInfo->SduDataPtr[i + 2] = MultiDcm22ReadFrame[i];
		}

		Multi_PduInfo->SduLength = 23;

		memcpy(DcmRspSFBuffer, Multi_PduInfo->SduDataPtr, Multi_PduInfo->SduLength);

		transreqresult = CanTp_Transmit(0, Multi_PduInfo);
	}
	return transreqresult;
}

