#include "AbstractForOtherModules.h"
IMemIf_ConfigType IMemIfConfigList[3] =
{
	{IFLAGS_appvalid,1,0x00},
	{IFLAGS_reprogram,1,0x02},
	{IFLAGS_reset,1,0x03}
};

/*outside Modules API*/
/**
 * @brief 初始化CanTp模块。
 *
 * 该函数用于初始化CanTp（CAN传输协议）模块，通常在系统启动时调用。
 * 当前实现仅打印初始化日志，后续可根据需求扩展功能。
 */
void CanTp_Init(void)
{
	printf("task cantp init\n");
}
void Dcm_Init(void) 
{
	printf("task dcm init\n");
}
/* initializes the clock module */
void MCU_ClkSet(void)
{
	printf("task mcu clkset\n");
}
/* initializes the Can Port module */
void Port_Init(void)
{
	printf("task can port init\n");
}
/* initializes the Can module */
void Can_Init(void)
{
	printf("task can init\n");
}
/* initializes the Timer module */
void TIM_Init(void)
{
	printf("task cantp init\n");
}
/* initializes the Ea module */
void Ea_Init(void)
{
	printf("task ea init\n");
}
/* initializes the Watch Dog module */
void WDog_Init(void)
{
	printf("task watch dog init\n");
}
/*MCU interrupt initialization*/
void MCU_ISR_Init(void)
{
	printf("task mcu interrupt init\n");
}
/*Main Function of CanTp*/
void CanTp_MainFunction(void)
{
	printf("task cantp mainfunction\n");
}
/*Main Function of Dcm*/
void Dcm_MainFunction(void)
{
	printf("task dcm mainfuction\n");
}
/*Select EA as the memory type*/
void MemIf_SwtichToEaType(void)
{
	printf("task MemIfSwitchToEaType\n");
}

/*Memory read Interface*/
StdReturnType MemIf_Read(unsigned int addr, unsigned int len, unsigned int* buf,unsigned int readflagId)
{
	*buf = IMemIfConfigList[readflagId].IMemIfConfig_Flag;
	return Boot_Check_SUCCESS;
}

/*Call the erase function to erase the flag information in memory*/
StdReturnType MemIf_Erase(unsigned int addr, unsigned int len, unsigned int* buf)
{
	return Boot_Check_SUCCESS;
}

/*for dcm delay cancel task*/
void DCM_10DelayCancel(void)
{
	printf("task dcm delay cancel\n");
}
/*for response of reprogram task*/
void SendReprogramRespond(void)
{
	printf("task response of reprogram\n");
}
/*for state of reprogram*/
void SetReprogramState(void)
{
	printf("task of state of reprogram\n");
}
/* start watchdog */
void WdgM_Start(void)
{
	printf("task of watch dog\n");
}

/* Can sends and receives packets periodically */
void Can_MainFunction_Write(void)
{
	printf("task of can write\n");
}
void Can_MainFunction_Read(void)
{
	printf("task of can read\n");
}
/* Reset MCU */
void SendResponseMCUReset(void)
{
	printf("task of send response mcu\n");
}