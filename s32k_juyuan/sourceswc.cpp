#include "sourceswc.h"


void sourcetask(void)
{
	Std_ReturnType ReqResult = E_NOT_OK;
	/*send to driver then confirm to dcm.*/
	Can_Init();
	CanIf_Init(0);
	CanTp_Init(0);
	Dcm_Init();
	
	/*single frame send 10 service*/
	ReqResult = virtual_send_SF();

	if (ReqResult == E_OK)
	{
		CanTp_MainFunction();
	}
	/*if no error can driver callback.*/
	//fakeDriver_Simulate();

	/*single frame receive.*/
	//fakeDriverRxSF_Simulate();

}