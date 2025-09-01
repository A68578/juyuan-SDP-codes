#include "MemIf_Cfg.h"
#include "MemIf.h"

IMemIfConfigListType IMemIfConfigList[] =
{
	/* data address*/         /* data size */   /* data*/
	{IFLAGS_appvalid,			1,				0xAA},/*app valid flag*/
	{IFLAGS_reprogram,			1,				0xA5},/*reprogram flag*/
	{IFLAGS_reset,				1,				0x55},/*reset flag*/
};


IMemIfFuncPtrListType IMemIfFuncPtrList[3] =
{
	{ Ea_Write,Ea_Read,Ea_Erase},
	{ Fee_Write,Fee_Read,Fee_Erase},
	{ MemIf_RamWrite,MemIf_RamRead,Mem_NULL_PTR}
};

