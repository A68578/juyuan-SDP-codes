#pragma once
#define DEBUG 1
#include "common.h"
#include "Can_GeneralTypes.h"
#include "CanPBCfg.h"
#include "CanIf.h"
#include "sourceswc.h"
#if DEBUG
#include "string.h"
#include "stdlib.h"
#endif

typedef enum
{
	CAN_OK,
	CAN_NOT_OK
}Can_ReturnType;

extern void Can_Init(void);
extern Can_ReturnType Can_Write(Can_PduType* CanData);
extern void fakeDriverTxConfirmation_Simulate(void);

extern void fakeDriverRxSF_Simulate(void);
extern void fakeDriverRx22ReqSession_Simulate(void);
extern void fakeDriverRx22ReadSession_Simulate(void);
extern void fakeDriverRxFlowControlFrame_Simulate(void);















