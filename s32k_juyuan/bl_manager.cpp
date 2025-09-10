/*****************************************************************************************************************************************
 *Copyright(C),unauthorized use, reproduction and dissemination are not allowed
 *FileName:bootmanagerfbl.cpp
 *Date: 2025.xxx
 *Description:This file is the implementation of the bootmanager module
 *****************************************************************************************************************************************/
 /*****************************************************************************************************************************************
  *   Revision History:
  *
  *   Version     Date          Author        Descriptions
  *   ---------   ----------    ------------  ---------------
  *   01.00.00    2025.xxx      /      Original
  *****************************************************************************************************************************************/

#include "bl_manager.h"


#include "Windows.h"


  /*global macros*/
#define SIZEARRAY(list) sizeof(list)/sizeof(list[0])

#define DEBUG 1 
#if DEBUG 
#define APPENTRY(addr)  do { \
        void (*jump_func)(void) = (void (*)(void))addr; \
        jump_func(); \
    } while (0)

#endif


/*global definitions*/
fblManagerContext g_fblchange;
static unsigned int app_check_flag;


static void DET_debug(void)
{
	printf("error debug\n");
	exit(1);
}


static void APP_debug(void)
{
	printf("app debug\n");
}


bl_manager_ReturnType FBL_CheckTargetFlag(unsigned int readflagId)
{
	unsigned int checkFlagData = 0;
	unsigned int checkFlagDataTemp = 0;
	unsigned int checkFlagSize = 0;
	unsigned int checkFlagAddr = 0;
	bl_manager_ReturnType result = Boot_Check_Fail;

	if (readflagId < 3)
	{
		/* Extract the target Flag information */
		checkFlagSize = IMemIfConfigList[readflagId].IMemIfConfig_Len;
		checkFlagData = IMemIfConfigList[readflagId].IMemIfConfig_Flag;
		checkFlagAddr = IMemIfConfigList[readflagId].IMemIfConfig_Addr;
	}
	/* Select EA as the memory type */
	MemIf_SwtichToEaType();
	result = (bl_manager_ReturnType)MemIf_Read(checkFlagAddr, checkFlagSize, &checkFlagDataTemp);
	/* Check the read result of the target flag */
	if ((result == Boot_Check_Success) && (checkFlagData == checkFlagDataTemp))
	{
		result = Boot_Check_Success;
	}
	else
	{
		result = Boot_Check_Fail;
	}

	 return result;
}


bl_manager_ReturnType FBL_ClearFlag(unsigned int readflagId)
{

	bl_manager_ReturnType result = Boot_Check_Fail;
	unsigned int data;
	unsigned int addr;
	unsigned int len;

	/* Check for matching Flag ID */
	if (readflagId < 3)
	{
		/* Extracts the Flag information to be cleared */
		len = IMemIfConfigList[readflagId].IMemIfConfig_Len;
		data = IMemIfConfigList[readflagId].IMemIfConfig_Flag;
		addr = IMemIfConfigList[readflagId].IMemIfConfig_Addr;
		/* Select EA as the memory type */
		MemIf_SwtichToEaType();
		/* Call the erase function to erase the flag information in memory */
		result = (bl_manager_ReturnType)MemIf_Erase(addr, len, &data);
	}

	return result;
}

bl_manager_ReturnType FBL_CheckReprog_Event(fblManagerContext* fblmanagercontext)
{
	printf("check reprog\n");
	bl_manager_ReturnType checkresult = Boot_Check_Fail;
	unsigned int flag = 0xFF;
	bl_manager_ReturnType ClearFlag_result = Boot_Check_Fail;
	bl_manager_ReturnType result = Boot_Check_Fail;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_Success)
		{
			result = Boot_Check_Success;
		}
		else
		{
			checkresult = FBL_CheckTargetFlag(IMemIf_FLAGS_REPROGRAM_ID);
			if (checkresult == Boot_Check_Success)
			{
				flag = VALID_REQUEST_FLAG;
			}
			else
			{
				flag = INVALID_REQUEST_FLAG;
			}
			if (flag == VALID_REQUEST_FLAG)
			{
				ClearFlag_result = FBL_ClearFlag(IMemIf_FLAGS_REPROGRAM_ID);
				if (ClearFlag_result == Boot_Check_Success)
				{
					/* Send a Response */
					DCM_10DelayCancel();
					SendReprogramRespond();
					SetReprogramState();
					result = Boot_Check_Success;
					fblmanagercontext->success_flag = Boot_Check_Success;
				}
				else
				{
					fblmanagercontext->success_flag = Boot_Check_Fail;
				}
			}
			else
			{
				fblmanagercontext->success_flag = Boot_Check_Fail;
				result = Boot_Check_Fail;
			}
		}
	}
	return result;
}


bl_manager_ReturnType FBL_CheckReset_Event(fblManagerContext* fblmanagercontext)
{
	printf("check reset\n");
	bl_manager_ReturnType result = Boot_Check_Fail;
	bl_manager_ReturnType checkresult = Boot_Check_Fail;
	bl_manager_ReturnType ClearFlag_result = Boot_Check_Fail;
	unsigned int flag = 0xFF;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		checkresult = FBL_CheckTargetFlag(IMemIf_FLAGS_RESET_ID);
		if (checkresult == Boot_Check_Success)
		{
			flag = VALID_RESET_FLAG;
		}
		else
		{
			flag = INVALID_RESET_FLAG;
		}
		if (flag == VALID_RESET_FLAG)
		{
			ClearFlag_result = FBL_ClearFlag(IMemIf_FLAGS_RESET_ID);
			if (ClearFlag_result == Boot_Check_Success)
			{
				/* Reset MCU */
				SendResponseMCUReset();
				result = Boot_Check_Success;
				exit(0);
			}
			else
			{
				;
			}
		}
	}
	return result;
}


bl_manager_ReturnType FBL_Mcal_Init_Event(fblManagerContext* fblmanagercontext)
{
	printf("mcal init\n");
	bl_manager_ReturnType result = Boot_Check_Fail;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_Success)
		{
			result = Boot_Check_Success;
		}
		else
		{
			/* initializes the clock module */
			MCU_ClkSet();
			/* initializes the Can Port module */
			Port_Init();
			/* initializes the Can module */
			Can_Init();
			/* initializes the Timer module */
			TIM_Init();
			/* initializes the Ea module */
			Ea_Init();
			/* initializes the Watch Dog module */
			Wdog_Init();
			/*MCU interrupt initialization*/
			MCU_ISR_Init();
			fblmanagercontext->success_flag = Boot_Check_Success;
			result = Boot_Check_Success;
		}
	}
	return result;
}

bl_manager_ReturnType FBL_Diagnostic_Init_Event(fblManagerContext* fblmanagercontext)
{
	printf("diagnose init\n");
	bl_manager_ReturnType result = Boot_Check_Fail;
	unsigned int readflag = 0;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_Success)
		{
			result = Boot_Check_Success;
		}
		else
		{
			CanTp_Init(CanTp_NULL_PTR);
			Dcm_Init();
			fblmanagercontext->success_flag = Boot_Check_Success;
			result = Boot_Check_Success;
		}
	}
	return result;
}


bl_manager_ReturnType FBL_MainFunction_Event(fblManagerContext* fblmanagercontext)
{
	printf("fbl_bsw_mainfunction\n");
	unsigned int readflag = 0;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		/* start watchdog */
		Wdog_Start();
		while (1)
		{
			/* Can sends and receives packets periodically */
			//Can_MainFunction_Write();
			//Can_MainFunction_Read();
			//if (timer == 1)
			//{
				/* Modules main run-time scheduling */
				//CanTp_MainFunction();
				//Dcm_MainFunction();
			//}
		}

	}
	return Boot_Check_Success;
}

bl_manager_ReturnType FBL_CheckApp_Event(fblManagerContext* fblmanagercontext)
{
	printf("check app\n");
	bl_manager_ReturnType checkresult = Boot_Check_Fail;
	bl_manager_ReturnType result = Boot_Check_Fail;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_Success)
		{
			result = Boot_Check_Success;
		}
		else
		{
			result = FBL_CheckTargetFlag(IMemIf_FLAGS_APPLICATION_ID);
			if (result == Boot_Check_Success)
			{
				app_check_flag = VALID_APP_FLAG;
				fblmanagercontext->success_flag = Boot_Check_Success;
			}
			else
			{
				app_check_flag = INVALID_APP_FLAG;
				fblmanagercontext->success_flag = Boot_Check_Fail;
			}
		}
	}
	return result;
}

bl_manager_ReturnType FBL_GotoApp_Event(fblManagerContext* fblmanagercontext)
{
	printf("go to app\n");
	bl_manager_ReturnType result = Boot_Check_Fail;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_Success)
		{
			result = Boot_Check_Success;
		}
		else
		{
			if (app_check_flag == VALID_APP_FLAG)
			{
				APPENTRY(APP_debug);
				result = Boot_Check_Success;
				fblmanagercontext->success_flag = Boot_Check_Success;
			}
		}
	}
	return result;
}

static fblManagerEvent fblchange_channel_select[] =
{
	{FBL_CheckReprog,FBL_CheckReprog_Event},
	{FBL_CheckReset,FBL_CheckReset_Event},
	{FBL_Mcal_Init,FBL_Mcal_Init_Event},
	{FBL_Diagnostic_Init,FBL_Diagnostic_Init_Event},
	{FBL_MainFunction,FBL_MainFunction_Event},
	{FBL_CheckApp,FBL_CheckApp_Event},
	{FBL_GotoApp,FBL_GotoApp_Event}
};
static FuncRouterType FuncRouter[] =
{
	{FBL_CheckReprog_Event,{FBL_Mcal_Init,FBL_CheckApp}},
	{FBL_CheckApp_Event,{FBL_GotoApp,FBL_Mcal_Init}},
	{FBL_Mcal_Init_Event,{FBL_Diagnostic_Init,FBL_NULL}},
	{FBL_Diagnostic_Init_Event,{FBL_MainFunction,FBL_NULL}},
	{FBL_GotoApp_Event,{FBL_END,FBL_NULL}},
	{FBL_MainFunction_Event,{FBL_END,FBL_NULL}},
};


static EventHandler FuncConfig[1] = { NULL };

static bl_manager_ReturnType FBltaskhandler(fblManagerContext* fblmanagercontext)
{
	bl_manager_ReturnType result = Boot_Check_Fail;
	result = fblchange_channel_select[fblmanagercontext->currentstate].handle(fblmanagercontext);
	return result;
}

/*
static bl_manager_ReturnType forspecial_task(fblManagerContext* fblmanagercontext)
{
	bl_manager_ReturnType result = Boot_Check_Fail;

	if (fblmanagercontext->currentstate == FBL_CheckApp)
	{
		fblmanagercontext->currentstate = FBL_CheckReset;
		result = FBltaskhandler(fblmanagercontext);
		if (result == Boot_Check_Fail)
		{
			fblmanagercontext->currentstate = FBL_CheckApp;
			result = FBltaskhandler(fblmanagercontext);
		}
	}
	else
	{
		result = Boot_Check_Success;
	}
	return result;
}
*/
void fblmaintask_cycle(fblManagerContext* fblmanager)
{
	bl_manager_ReturnType result = Boot_Check_Fail;
	result = FuncConfig[0](fblmanager);
	for (int i = 0; i < SIZEARRAY(FuncRouter); i++)
	{
		//result = forspecial_task(fblmanager);
		if (FuncConfig[0] == FuncRouter[i].originfunc)
		{
			if (result == Boot_Check_Success)
			{
				fblmanager->currentstate = FuncRouter[i].FuncRouterSelect.selectfunc_true;
				fblmanager->success_flag = Boot_Check_Fail;
				result = FBltaskhandler(fblmanager);
				if (result == Boot_Check_Success)
				{
					break;
				}
				else
				{
					;
				}

			}
			else
			{
				fblmanager->currentstate = FuncRouter[i].FuncRouterSelect.selectfunc_false;
				fblmanager->success_flag = Boot_Check_Fail;
				result = FBltaskhandler(fblmanager);
				if (result == Boot_Check_Success)
				{
					break;
				}
				else
				{
					;
				}
			}
		}
		else
		{
			;
		}
	}
	for (int j = 0; j < SIZEARRAY(FuncRouter); j++)
	{
		if (fblchange_channel_select[fblmanager->currentstate].handle == FuncRouter[j].originfunc)
		{
			FuncConfig[0] = fblchange_channel_select[fblmanager->currentstate].handle;
		}
	}
}


void initfblmanager_task(void)
{
	g_fblchange.stateCount = SIZEARRAY(fblchange_channel_select);
	g_fblchange.success_flag = Boot_Check_Fail;
loop:
	printf("please input your initial task:");
	scanf("%d", &g_fblchange.currentstate);
	if ((g_fblchange.currentstate < 0) || (g_fblchange.currentstate > 6))
	{
		printf("error input!\n");
		system("cls");
		goto loop;
	}
	else
	{
		FuncConfig[0] = fblchange_channel_select[g_fblchange.currentstate].handle;
	}
}

void fblmain(void)
{
	initfblmanager_task();
	/*when it is not null state,it will continue.*/
	while (g_fblchange.currentstate != FBL_NULL)
	{
		fblmaintask_cycle(&g_fblchange);
	}
}