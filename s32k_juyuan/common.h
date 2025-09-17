#pragma once
#include "stdio.h"
#define DEBUG 1
#if DEBUG
#include <Windows.h>
#pragma comment(lib, "kernel32.lib")
#endif

#define NULL_PTR 0

typedef enum
{
	E_OK,
	E_NOT_OK
}Std_ReturnType;


typedef enum
{
	False,
	True
}BoolType;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef int uint32;