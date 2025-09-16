#include "sourceswc.h"


void sourcetask(void)
{
	Std_ReturnType ReqResult = E_NOT_OK;
	/*send to driver then confirm to dcm.*/
	Can_Init();
	CanIf_Init(0);
	CanTp_Init(0);
	Dcm_Init();
	
	
	/*single frame receive 10 01.*/
	fakeDriverRxSF_Simulate();
	
	/*dcm deal with 10 SF session.*/
	
	ReqResult = virtual_send_SF();

	
	if (ReqResult == E_OK)
	{
		CanTp_MainFunction();
	}
	
	/*if no error can driver callback.*/
	fakeDriver_Simulate();

	

}