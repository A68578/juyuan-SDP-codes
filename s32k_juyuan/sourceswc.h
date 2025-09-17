#pragma once
#include "common.h"
#include "ComStack_Types.h"
#include "Dcm.h"
#include "Dcm_Appl.h"
#include "CanIf.h"
#include "CanTp.h"
#include "Can.h"
extern void sourcetask(void);


extern unsigned char SingleDcm10Frame[2];
extern unsigned char SingleDcm22ReqFrame[3];
extern unsigned char MultiDcm22ReadFrame[20];
extern unsigned char FlowControlFrame[3];







extern unsigned char SingleDcm2EFrame[8];







/*---------------------------------------------------------------------------------------------------------------------------*/
extern unsigned char FirstFrame[8];
extern unsigned char ConsecutiveFrame1[8];
extern unsigned char ConsecutiveFrame2[8];
extern unsigned char ConsecutiveFrame3[8];
extern unsigned char ConsecutiveFrame4[8];
extern unsigned char ControlFollowFrameCTS[8];
extern unsigned char ControlFollowFrameWT[8];
extern unsigned char ControlFollowFrameOVF[8];