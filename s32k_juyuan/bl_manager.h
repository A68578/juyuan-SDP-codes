#pragma once
#pragma once
#include "common.h"
#include "MemIf_Cfg.h"
#include "MemIf.h"
#include "Dcm.h"
#include "Dcm_Appl.h"
#include "MCU_Clock.h"

#define NULL_PTR (void*)0


/******************************************************************************************************************************************
								 *Macro
*******************************************************************************************************************************************/
#define VALID_REQUEST_FLAG             	0x00U
#define INVALID_APP_FLAG               	0x01U
#define VALID_APP_FLAG                 	0x02U
#define VALID_RESET_FLAG               	0x03U
#define INVALID_REQUEST_FLAG            0x04U
#define INVALID_RESET_FLAG              0x05U


/*blmanager return type*/
typedef enum
{
	Boot_Check_Success,
	Boot_Check_Fail
}bl_manager_ReturnType;

/*state context*/
typedef struct
{
	unsigned int stateCount;       /**< Number of elements in stateDefinitions (set by component(s) using this module) */
	unsigned int currentstate;     /**< Current state */
	unsigned int success_flag;     /*if function is successfully handled,skip the check*/
}fblManagerContext;

/*handler of event*/
typedef bl_manager_ReturnType(*EventHandler)(fblManagerContext*);


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

void fblmain(void);
