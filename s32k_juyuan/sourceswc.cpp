#include "sourceswc.h"


void sourcetask(void)
{
	Std_ReturnType ReqResult = E_NOT_OK;
	/*send to driver then confirm to dcm.*/
	Can_Init();
	CanIf_Init(0);
	CanTp_Init(0);
	Dcm_Init();
	
	
#if 0
	/*single frame receive 10 01.*/
	fakeDriverRxSF_Simulate();
	
	/*dcm deal with 10 SF session.*/
	
	ReqResult = virtual_send_SF();

	
	if (ReqResult == E_OK)
	{
		CanTp_MainFunction();
	}
	
	/*if no error can driver callback.*/
	fakeDriverTxConfirmation_Simulate();
#endif

	/*22 session Request VIN.*/
	fakeDriverRx22ReqSession_Simulate();
	
	/*dcm respond 22 session and read VIN.*/
	ReqResult = virtual_send_22ReqResponseFF();

	if (ReqResult == E_OK)
	{
		CanTp_MainFunction();
	}

	/*if no error can driver callback,confirm that ff is sent.*/
	fakeDriverTxConfirmation_Simulate();
	
	/*the next step,reset the flag.*/
	DcmResetConfirmFlag();

	/*if ff is sent,send fc to control the frames.*/
	fakeDriverRxFlowControlFrame_Simulate();

	/*if the fc confirms that it is CTS,then transmit.*/





#if 0
	/*22 session Read VIN.*/
	fakeDriverRx22ReadSession_Simulate();
#endif

}