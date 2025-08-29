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

#include "bootmanager.h"
#include "AbstractForOtherModules.h"

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

#if OFF_BOARD
fblManagerBackup g_fblmanager_backup;
#endif

/*static functions to debug*/
/**
 * @brief 调试错误并终止程序。
 *
 * 该函数用于在调试过程中打印错误信息，并通过调用 exit(1) 强制终止程序。
 * 通常用于处理不可恢复的错误或调试时的断言失败。
 *
 * @note 此函数为静态函数，仅在当前文件内可见。
 */
static void DET_debug(void)
{
	printf("error debug\n");
	exit(1);
}

/**
 * @brief 打印应用程序的调试信息。
 *
 * 该函数用于在调试模式下输出应用程序的调试信息，通常用于开发阶段的日志记录。
 * 调用此函数会在标准输出中打印 "app debug" 字符串。
 */
static void APP_debug(void)
{
	printf("app debug\n");
}

/**
 * @brief 检查目标标志位的状态。
 *
 * 该函数用于验证指定的标志位是否与预期值匹配。首先根据输入的标志位ID获取配置信息，
 * 然后切换到EA内存类型，读取目标标志位的值并与预期值进行比较。
 *
 * @param readflagId 标志位的ID，有效范围为0到2。
 * @return StdReturnType 返回检查结果：
 *   - Boot_Check_SUCCESS: 标志位值与预期值匹配。
 *   - Boot_Check_FAIL: 标志位值与预期值不匹配或读取失败。
 */
StdReturnType FBL_CheckTargetFlag(unsigned int readflagId)
{
	unsigned int checkFlagData = 0;
	unsigned int checkFlagDataTemp = 0;
	unsigned int checkFlagSize = 0;
	unsigned int checkFlagAddr = 0;
	StdReturnType result = Boot_Check_FAIL;

	if (readflagId < 3)
	{
		/* Extract the target Flag information */
		checkFlagSize = IMemIfConfigList[readflagId].IMemIfConfig_Len;
		checkFlagData = IMemIfConfigList[readflagId].IMemIfConfig_Flag;
		checkFlagAddr = IMemIfConfigList[readflagId].IMemIfConfig_Addr;
	}
	/* Select EA as the memory type */
	MemIf_SwtichToEaType();
	result = MemIf_Read(checkFlagAddr, checkFlagSize, &checkFlagDataTemp,readflagId);
	/* Check the read result of the target flag */
	if ((result == Boot_Check_SUCCESS) && (checkFlagData == checkFlagDataTemp))
	{
		result = Boot_Check_SUCCESS;
	}
	else
	{
		result = Boot_Check_FAIL;
	}

	return result;
}

/**
 * @brief 清除指定的标志位。
 *
 * 该函数用于清除内存中指定的标志位。通过给定的标志ID，函数会从配置列表中提取对应的地址、长度和标志值，
 * 并调用内存接口的擦除函数来清除标志位。
 *
 * @param readflagId 标志ID，用于指定需要清除的标志位。有效范围为0到2。
 * @return StdReturnType 返回操作结果：
 *   - Boot_Check_FAIL: 操作失败（标志ID无效或擦除失败）。
 *   - 其他值: 操作成功（具体返回值由MemIf_Erase函数决定）。
 *
 * @note 函数内部会切换到EA内存类型，并调用MemIf_Erase函数执行实际的擦除操作。
 */
StdReturnType FBL_ClearFlag(unsigned int readflagId)
{

	StdReturnType result = Boot_Check_FAIL;
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
		result = MemIf_Erase(addr, len, &data);
	}

	return result;
}

/**
 * @brief 检查并处理重新编程事件。
 *
 * 该函数用于检查是否需要重新编程，并根据检查结果执行相应的操作。
 * 如果检查通过，会清除标志位并发送重新编程响应；否则，标记为失败。
 *
 * @param fblmanagercontext 指向 FBL 管理器上下文的指针，用于存储操作状态。
 * @return StdReturnType 返回操作结果：
 *   - Boot_Check_SUCCESS: 操作成功。
 *   - Boot_Check_FAIL: 操作失败。
 *
 * @note 如果传入的上下文指针为 NULL，会触发调试输出。
 */
StdReturnType FBL_CheckReprog_Event(fblManagerContext *fblmanagercontext)
{
	printf("check reprog\n");
	StdReturnType checkresult = Boot_Check_FAIL;
	unsigned int flag = 0xFF;
	StdReturnType ClearFlag_result = Boot_Check_FAIL;
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			checkresult = FBL_CheckTargetFlag(IMemIf_FLAGS_REPROGRAM_ID);
			if (checkresult == Boot_Check_SUCCESS)
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
				if (ClearFlag_result == Boot_Check_SUCCESS)
				{
					/* Send a Response */
					DCM_10DelayCancel();
					SendReprogramRespond();
					SetReprogramState();
					result = Boot_Check_SUCCESS;
					fblmanagercontext->success_flag = Boot_Check_SUCCESS;
				}
				else
				{
					fblmanagercontext->success_flag = Boot_Check_FAIL;
				}
			}
			else
			{
				fblmanagercontext->success_flag = Boot_Check_FAIL;
				result = Boot_Check_FAIL;
			}
		}
	}
	return result;
}

/**
 * @brief 检查并处理复位事件。
 *
 * 该函数用于检查复位标志位，并根据标志位的状态决定是否执行MCU复位操作。
 * 如果复位标志位有效，则清除该标志位并触发MCU复位；否则，函数返回失败状态。
 *
 * @param fblmanagercontext 指向FBL管理器上下文的指针。如果为NULL，会触发调试输出。
 * @return StdReturnType 返回操作结果：
 *   - Boot_Check_SUCCESS: 复位操作成功执行。
 *   - Boot_Check_FAIL: 复位操作未执行或执行失败。
 */
StdReturnType FBL_CheckReset_Event(fblManagerContext *fblmanagercontext)
{
	printf("check reset\n");
	StdReturnType result = Boot_Check_FAIL;
	StdReturnType checkresult = Boot_Check_FAIL;
	StdReturnType ClearFlag_result = Boot_Check_FAIL;
	unsigned int flag = 0xFF;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		checkresult = FBL_CheckTargetFlag(IMemIf_FLAGS_RESET_ID);
		if (checkresult == Boot_Check_SUCCESS)
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
			if (ClearFlag_result == Boot_Check_SUCCESS)
			{
				/* Reset MCU */
				SendResponseMCUReset();
				result = Boot_Check_SUCCESS;
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

/**
 * @brief 初始化 MCAL（Microcontroller Abstraction Layer）相关模块的事件处理函数。
 *
 * 该函数负责初始化 MCAL 中的多个模块，包括时钟、端口、CAN、定时器、EA、看门狗和中断等。
 * 如果传入的上下文对象已标记为成功，则直接返回成功状态；否则执行初始化流程。
 *
 * @param fblmanagercontext 指向 FBL 管理器上下文的指针，用于传递状态和控制信息。
 *                         如果为 NULL，会触发调试输出。
 *
 * @return StdReturnType 返回初始化结果：
 *                       - Boot_Check_SUCCESS: 初始化成功。
 *                       - Boot_Check_FAIL: 初始化失败或参数无效。
 *
 * @note 如果上下文对象的 success_flag 已标记为成功，则跳过初始化流程。
 *       初始化过程中会依次调用各模块的初始化函数。
 */
StdReturnType FBL_Mcal_Init_Event(fblManagerContext *fblmanagercontext)
{
	printf("mcal init\n");
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
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
			WDog_Init();
			/*MCU interrupt initialization*/
			MCU_ISR_Init();
			fblmanagercontext->success_flag = Boot_Check_SUCCESS;
			result = Boot_Check_SUCCESS;
		}
	}
	return result;
}
/**
 * @brief 初始化诊断事件。
 *
 * 该函数用于初始化诊断事件，检查传入的 `fblmanagercontext` 是否有效，并根据其状态执行相应的操作。
 * 如果 `fblmanagercontext` 为 NULL，则调用调试函数 `DET_debug`。
 * 如果 `fblmanagercontext->success_flag` 为 `Boot_Check_SUCCESS`，则直接返回成功状态。
 * 否则，初始化 CAN 传输层 (`CanTp_Init`) 和诊断通信管理器 (`Dcm_Init`)，并将 `success_flag` 设置为成功状态。
 *
 * @param fblmanagercontext 指向 `fblManagerContext` 结构体的指针，包含启动检查的状态标志。
 * @return StdReturnType 返回操作结果：
 *   - `Boot_Check_SUCCESS`：操作成功。
 *   - `Boot_Check_FAIL`：操作失败（仅在 `fblmanagercontext` 为 NULL 时返回）。
 */
StdReturnType FBL_Diagnose_Init_Event(fblManagerContext *fblmanagercontext)
{
	printf("diagnose init\n");
	StdReturnType result = Boot_Check_FAIL;
	unsigned int readflag = 0;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			CanTp_Init();
			Dcm_Init();
			fblmanagercontext->success_flag = Boot_Check_SUCCESS;
			result = Boot_Check_SUCCESS;
		}
	}
	return result;
}

/**
 * @brief FBL主函数的事件处理函数。
 *
 * 该函数负责处理FBL（Flash Bootloader）的主事件循环，包括启动看门狗、周期性地执行CAN通信的发送和接收，
 * 以及调度其他模块的主函数运行。
 *
 * @param fblmanagercontext 指向FBL管理器上下文的指针。如果为NULL，会触发调试错误。
 *
 * @return StdReturnType 返回操作状态，成功时返回Boot_Check_SUCCESS。
 *
 * @note 函数内部包含一个无限循环，用于周期性地执行以下操作：
 *       1. 调用Can_MainFunction_Write和Can_MainFunction_Read处理CAN通信。
 *       2. 调用CanTp_MainFunction和Dcm_MainFunction调度其他模块的运行。
 *
 * @warning 如果传入的上下文指针为NULL，会触发DET_debug()调试错误。
 */
StdReturnType FBL_MainFunction_Event(fblManagerContext *fblmanagercontext)
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
		WdgM_Start();
		while (1)
		{
			/* Can sends and receives packets periodically */
			Can_MainFunction_Write();
			Can_MainFunction_Read();
			/* Modules main run-time scheduling */
			CanTp_MainFunction();
			Dcm_MainFunction();
		}
		
	}
	return Boot_Check_SUCCESS;
}
/**
 * @brief 检查应用程序的有效性并更新状态。
 *
 * 该函数用于验证应用程序的有效性，并根据检查结果更新传入的上下文结构体中的成功标志。
 * 如果上下文结构体为空，会触发调试输出。
 *
 * @param fblmanagercontext 指向FBL管理器上下文的指针，用于存储检查结果。
 * @return StdReturnType 返回检查结果：
 *   - Boot_Check_SUCCESS: 应用程序有效。
 *   - Boot_Check_FAIL: 应用程序无效或检查失败。
 */
StdReturnType FBL_CheckApp_Event(fblManagerContext *fblmanagercontext)
{
	printf("check app\n");
	StdReturnType checkresult = Boot_Check_FAIL;
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			result = FBL_CheckTargetFlag(IMemIf_FLAGS_APPLICATION_ID);
			if (result == Boot_Check_SUCCESS)
			{
				app_check_flag = VALID_APP_FLAG;
				fblmanagercontext->success_flag = Boot_Check_SUCCESS;
			}
			else
			{
				app_check_flag = INVALID_APP_FLAG;
				fblmanagercontext->success_flag = Boot_Check_FAIL;
			}
		}
	}
	return result;
}
/**
 * @brief 处理跳转到应用程序的事件。
 *
 * 该函数用于在引导管理器中处理跳转到应用程序的请求。它会检查应用程序标志是否有效，
 * 如果有效则跳转到应用程序入口点，否则返回失败状态。
 *
 * @param fblmanagercontext 指向引导管理器上下文的指针。如果为NULL，会触发调试输出。
 * @return StdReturnType 返回操作结果：
 *                      - Boot_Check_SUCCESS: 成功跳转到应用程序。
 *                      - Boot_Check_FAIL: 跳转失败或参数无效。
 */
StdReturnType FBL_GotoApp_Event(fblManagerContext *fblmanagercontext)
{
	printf("go to app\n");
	StdReturnType result = Boot_Check_FAIL;
	if (fblmanagercontext == NULL_PTR)
	{
		DET_debug();
	}
	else
	{
		if (fblmanagercontext->success_flag == Boot_Check_SUCCESS)
		{
			result = Boot_Check_SUCCESS;
		}
		else
		{
			if (app_check_flag == VALID_APP_FLAG)
			{
				APPENTRY(APP_debug);
				result = Boot_Check_SUCCESS;
				fblmanagercontext->success_flag = Boot_Check_SUCCESS;
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
	{FBL_Diagnose_Init,FBL_Diagnose_Init_Event},
	{FBL_MainFunction,FBL_MainFunction_Event},
	{FBL_CheckApp,FBL_CheckApp_Event},
	{FBL_GotoApp,FBL_GotoApp_Event}
};
static FuncRouterType FuncRouter[] =
{
	{FBL_CheckReprog_Event,{FBL_Mcal_Init,FBL_CheckApp}},
	{FBL_CheckApp_Event,{FBL_GotoApp,FBL_Mcal_Init}},
	{FBL_Mcal_Init_Event,{FBL_Diagnose_Init,FBL_MainFunction}},
	{FBL_Diagnose_Init_Event,{FBL_MainFunction,FBL_MainFunction}},
	{FBL_MainFunction_Event,{FBL_MainFunction,FBL_MainFunction}},
	{FBL_GotoApp_Event,{FBL_MainFunction,FBL_MainFunction}}
};


static EventHandler FuncConfig[1] = {NULL};

#if 0 //OFF_BOARD
/*back up data*/
/**
 * @brief 备份 fblManagerContext 结构体数据。
 *
 * 该函数用于将传入的 fblManagerContext 结构体数据备份到全局变量 g_fblmanager_backup 中。
 * 注意：函数内部使用了动态内存分配，调用者需确保传入的指针有效。
 *
 * @param fblmanagercontext 指向需要备份的 fblManagerContext 结构体的指针。
 */
static void fbl_manager_backup(fblManagerContext *fblmanagercontext)
{
	g_fblmanager_backup.fblmanagerContextBackup = (fblManagerContext *)malloc(sizeof(fblManagerContext));
	g_fblmanager_backup.fblmanagerContextBackup = fblmanagercontext;
}
/*free data*/
/**
 * 释放 fblManagerContext 结构体的内存。
 * 该函数用于清理由 fblManagerContext 占用的内存资源。
 *
 * @param fblmanagercontext 指向需要释放的 fblManagerContext 结构体的指针。
 */
static void fbl_free_data(fblManagerContext *fblmanagercontext)
{
	free(fblmanagercontext);
}
/*get data from back up*/
/**
 * @brief 获取备份的FBL管理器上下文。
 *
 * 此函数用于返回全局备份的FBL管理器上下文指针。
 * 通常用于恢复或检查备份的FBL管理器状态。
 *
 * @return fblManagerContext* 指向备份的FBL管理器上下文的指针。
 */
static fblManagerContext *get_fbl_manager_backup(void)
{
	return g_fblmanager_backup.fblmanagerContextBackup;
}
#endif

/**
 * @brief 处理FBL任务的状态转换。
 *
 * 根据当前状态调用对应的处理函数，执行状态转换逻辑。
 *
 * @param fblmanagercontext 指向FBL管理器上下文的指针，包含当前状态和其他相关信息。
 * @return StdReturnType 返回执行结果，成功或失败。
 */
static StdReturnType FBltaskhandler(fblManagerContext *fblmanagercontext)
{
	StdReturnType result = Boot_Check_FAIL;
	result = fblchange_channel_select[fblmanagercontext->currentstate].handle(fblmanagercontext);
	return result;
}

/**
 * @brief 处理特殊任务的函数。
 *
 * 该函数根据当前状态（FBL_CheckApp）执行特定的任务处理逻辑。
 * 如果当前状态为 FBL_CheckApp，则将其切换为 FBL_CheckReset 并调用任务处理器。
 * 如果任务处理器返回失败，则恢复状态为 FBL_CheckApp 并再次调用任务处理器。
 * 如果当前状态不是 FBL_CheckApp，则直接返回成功状态。
 *
 * @param fblmanagercontext 指向 FBL 管理器上下文的指针，包含当前状态等信息。
 * @return StdReturnType 返回任务处理的结果（Boot_Check_SUCCESS 或 Boot_Check_FAIL）。
 */
static StdReturnType forspecial_task(fblManagerContext *fblmanagercontext)
{
	StdReturnType result = Boot_Check_FAIL;

	if (fblmanagercontext->currentstate == FBL_CheckApp)
	{
		fblmanagercontext->currentstate = FBL_CheckReset;
		result = FBltaskhandler(fblmanagercontext);
		if (result == Boot_Check_FAIL)
		{
			fblmanagercontext->currentstate = FBL_CheckApp;
			result = FBltaskhandler(fblmanagercontext);
		}
	}
	else
	{
		result = Boot_Check_SUCCESS;
	}
	return result;
}

/*for task cycle.*/
/**
 * @brief 执行FBL（Flash Boot Loader）主任务。
 *
 * 该函数负责处理FBL的主要逻辑，包括：
 * 1. 调用初始配置函数（FuncConfig[0]）进行初始化。
 * 2. 遍历功能路由表（FuncRouter），根据当前状态和结果选择执行不同的功能分支。
 * 3. 根据执行结果更新FBL管理器的状态（currentstate）和成功标志（success_flag）。
 * 4. 调用FBltaskhandler处理具体任务，并根据其返回值决定是否终止循环。
 * 5. 在循环结束后，更新FuncConfig[0]以匹配当前状态。
 *
 * @param fblmanager 指向FBL管理器上下文的指针，包含当前状态和配置信息。
 * @note 如果定义了OFF_BOARD宏，函数会调用fbl_manager_backup进行数据备份（当前被注释掉）。
 */
void fblmaintask(fblManagerContext *fblmanager)
{
	StdReturnType result = Boot_Check_FAIL;
	result = FuncConfig[0](fblmanager);
	for (int i = 0; i < SIZEARRAY(FuncRouter); i++)
	{
		result = forspecial_task(fblmanager);
		if (FuncConfig[0] == FuncRouter[i].originfunc)
		{
			if (result == Boot_Check_SUCCESS)
			{
				fblmanager->currentstate = FuncRouter[i].FuncRouterSelect.selectfunc_true;
				fblmanager->success_flag = Boot_Check_FAIL;
				result = FBltaskhandler(fblmanager);
				if (result == Boot_Check_SUCCESS)
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
				fblmanager->success_flag = Boot_Check_FAIL;
				result = FBltaskhandler(fblmanager);
				if (result == Boot_Check_SUCCESS)
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
#if 0 //OFF_BOARD
	/*back up data*/
	fbl_manager_backup(fblmanager);
#endif
}

/*initial all the state-machine task*/
/**
 * @brief 初始化FBL管理器的任务。
 *
 * 该函数用于初始化FBL管理器的任务，包括设置状态计数器和成功标志。
 * 用户需要输入初始任务编号，函数会检查输入的有效性，并根据输入选择对应的处理函数。
 * 如果输入无效，会提示错误并要求重新输入。
 *
 * @note 输入的任务编号必须在有效范围内（0到6），否则会提示错误。
 * @warning 使用了goto语句实现循环输入，需注意代码的可读性和维护性。
 */
void initfblmanager_task(void)
{
	g_fblchange.stateCount = SIZEARRAY(fblchange_channel_select);
	g_fblchange.success_flag = Boot_Check_FAIL;
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

#if 0 //OFF_BOARD
	g_fblmanager_backup.fblmanagerContextBackup = { 0 };
#endif
}

/**
 * @brief 主函数，负责初始化并运行FBL（Flash Boot Loader）管理器任务。
 *
 * 该函数的主要流程如下：
 * 1. 初始化FBL管理器任务（initfblmanager_task）。
 * 2. 进入主循环，持续运行FBL管理器任务（fblmaintask），直到当前状态变为FBL_NULL。
 *
 * 注意：
 * - 主循环通过检查全局变量g_fblchange.currentstate的状态来决定是否继续运行。
 * - 代码中包含了一段被注释掉的逻辑（OFF_BOARD），用于在任务结束时释放备份数据。
 */
int main()
{
#if 1
	initfblmanager_task();
	/*when it is not null state,it will continue.*/
	while (g_fblchange.currentstate != FBL_NULL)
	{
		fblmaintask(&g_fblchange);
	}
#if 0//OFF_BOARD
	/*finish the task,free the data back up.*/
	if (g_fblchange.currentstate == FBL_NULL)
	{
		fbl_free_data(g_fblmanager_backup.fblmanagerContextBackup);
	}
#endif
#endif
}