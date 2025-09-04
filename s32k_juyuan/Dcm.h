#pragma once
#include "ComStackTypes.h"
#include "common.h"
extern void DCM_10DelayCancel(void);
extern void Dcm_Init(void);
extern void Dcm_TpRxIndication(PduIdType id, Std_ReturnType result);