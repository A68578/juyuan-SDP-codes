#pragma once
#include "common.h"
#include "ComStack_Types.h"
#include "Dcm.h"
#include "Dcm_Appl.h"
#include "CanIf.h"
#include "CanTp.h"
#include "Can.h"
extern void sourcetask(void);

extern unsigned char SingleFrame[8];
extern unsigned char FirstFrame[8];
extern unsigned char ConsecutiveFrame1[8];
extern unsigned char ConsecutiveFrame2[8];
extern unsigned char ConsecutiveFrame3[8];
extern unsigned char ConsecutiveFrame4[8];
extern unsigned char ControlFollowFrameCTS[8];
extern unsigned char ControlFollowFrameWT[8];
extern unsigned char ControlFollowFrameOVF[8];