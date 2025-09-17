#pragma once
#include "common.h"
#include "MemIf_Cfg.h"
extern void MemIf_SwtichToEaType(void);
extern Std_ReturnType MemIf_Read(uint32 addr, uint32 len, uint32* buf);
extern Std_ReturnType MemIf_Erase(uint32 addr, uint32 len, uint32* buf);
extern Std_ReturnType Ea_Write(MemIf_DataPtr* ptr);
extern Std_ReturnType Ea_Read(MemIf_DataPtr* ptr);
extern Std_ReturnType Fee_Write(MemIf_DataPtr* ptr);
extern Std_ReturnType Fee_Read(MemIf_DataPtr* ptr);
extern Std_ReturnType Fee_Erase(MemIf_DataPtr* ptr);
extern Std_ReturnType MemIf_RamWrite(MemIf_DataPtr* ptr);
extern Std_ReturnType MemIf_RamRead(MemIf_DataPtr* ptr);
extern Std_ReturnType Ea_Erase(MemIf_DataPtr* ptr);
