#pragma once
#include "common.h"
#include "ComStack_Types.h"
#include "Dcm.h"
#include "Dcm_Appl.h"
#include "CanIf.h"
#include "CanTp.h"
#include "Can.h"
extern void sourcetask(void);


extern uint8 SingleDcm10Frame[2];
extern uint8 SingleDcm22ReqFrame[3];
extern uint8 MultiDcm22ReadFrame[20];
extern uint8 FlowControlFrame[3];







extern uint8 SingleDcm2EFrame[8];







/*---------------------------------------------------------------------------------------------------------------------------*/
extern uint8 FirstFrame[8];
extern uint8 ConsecutiveFrame1[8];
extern uint8 ConsecutiveFrame2[8];
extern uint8 ConsecutiveFrame3[8];
extern uint8 ConsecutiveFrame4[8];
extern uint8 ControlFollowFrameCTS[8];
extern uint8 ControlFollowFrameWT[8];
extern uint8 ControlFollowFrameOVF[8];