#include "MemIf.h"
void MemIf_SwtichToEaType(void)
{
	//MemIf_SetConfigType(EEPROM_TYPE);
}

Std_ReturnType MemIf_Read(uint32 addr, uint32 len, uint32* buf)
{
	
	return E_OK;
}


Std_ReturnType MemIf_Erase(uint32 addr, uint32 len, uint32* buf)
{
	Std_ReturnType MemIfResult = E_OK;
	
	
	return MemIfResult;
}

Std_ReturnType Ea_Write(MemIf_DataPtr* ptr)
{
	return E_OK;
}
Std_ReturnType Ea_Read(MemIf_DataPtr* ptr)
{
	return E_OK;
}
Std_ReturnType Fee_Write(MemIf_DataPtr* ptr)
{
	return E_OK;
}
Std_ReturnType Fee_Read(MemIf_DataPtr* ptr)
{
	return E_OK;
}
Std_ReturnType Fee_Erase(MemIf_DataPtr* ptr)
{
	return E_OK;
}
Std_ReturnType MemIf_RamWrite(MemIf_DataPtr* ptr)
{
	return E_OK;
}
Std_ReturnType MemIf_RamRead(MemIf_DataPtr* ptr)
{
	return E_OK;
}
Std_ReturnType Ea_Erase(MemIf_DataPtr* ptr)
{
	return E_OK;
}