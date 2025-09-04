#pragma once
#include "common.h"
#include "CanTp_Cfg.h"
#include "CanTp_Types.h"
#include "CanTp_Det.h"
#include "Dcm.h"



typedef struct {
    int dummy; //never used
} CanTp_ConfigType;

typedef unsigned int PduIdInfoType;

#define CanTp_NULL_PTR (CanTp_ConfigType*)0

extern void CanTp_Init(const CanTp_ConfigType* CfgPtr);