#pragma once
#include "common.h"
#define _CRT_SECURE_NO_WARNINGS

#define ExternVariable 1
#define NULL_PTR (void*)0
#define OFF_BOARD 1
#define ON_BOARD 0

/******************************************************************************************************************************************
								 *Macro
*******************************************************************************************************************************************/
#define VALID_REQUEST_FLAG             	0x00U
#define INVALID_APP_FLAG               	0x01U
#define VALID_APP_FLAG                 	0x02U
#define VALID_RESET_FLAG               	0x03U
#define INVALID_REQUEST_FLAG            0x04U
#define INVALID_RESET_FLAG              0x05U


/*state context*/
typedef struct
{
	unsigned int stateCount;       /**< Number of elements in stateDefinitions (set by component(s) using this module) */
	unsigned int currentstate;     /**< Current state */
	unsigned int nextstate;        /**< Next event */
	unsigned int index;            /*find the corresponding task to handle*/
	unsigned int success_flag;     /*if function is successfully handled,skip the check*/
}fblManagerContext;

/*handler of event*/
typedef StdReturnType(*EventHandler)(fblManagerContext*);


#if OFF_BOARD
/*backup data*/
typedef struct
{
	fblManagerContext *fblmanagerContextBackup;
}fblManagerBackup;
#endif

/*trigger of event*/
typedef enum
{
	FBL_CheckReprog,
	FBL_CheckReset,
	FBL_Mcal_Init,
	FBL_Diagnose_Init,
	FBL_MainFunction,
	FBL_CheckApp,		
	FBL_GotoApp,
	FBL_NULL
}TriggerState;

/*flag of trigger event*/
typedef enum
{
	FBL_CheckReprog_Flag = 1,
	FBL_CheckReset_Flag,
	FBL_Mcal_Init_Flag,
	FBL_DCM_Init_Flag,
	FBL_SelectTargetGo_Flag,
	FBL_MainFunction_Flag,
	FBL_CheckApp_Flag,
	FBL_GotoApp_Flag
}TriggerState_Flag;

typedef struct
{
	TriggerState triggerindex;
	EventHandler handle;
}fblManagerEvent;

typedef struct
{
	TriggerState selectfunc_true;
	TriggerState selectfunc_false;
}FuncRouterSelectType;

typedef struct
{
	EventHandler originfunc;
	FuncRouterSelectType FuncRouterSelect;
}FuncRouterType;