#include "Dcm.h"
void DCM_10DelayCancel(void)
{
	printf("Task Dcm 10 Delay Cancel\n");
}
void Dcm_Init(void)
{
	printf("Task Dcm Init\n");
}


void Dcm_TpRxIndication(PduIdType id, Std_ReturnType result)
{
	printf("Task Dcm TpRxIndication\n");
}