#pragma once
#include "common.h"
#include "ComStack_Types.h"
#include "CanTp.h"
#include "sourceswc.h"
#include "Dcm.h"

extern void SendReprogramRespond(void);
extern void SetReprogramState(void);
extern void SendResponseMCUReset(void);
extern Std_ReturnType virtual_send_SF(void);
extern Std_ReturnType virtual_send_22ReqResponseFF(void);