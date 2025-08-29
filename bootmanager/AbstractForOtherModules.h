#pragma once
#include "common.h"
/******************************************************************************************************************************************
                                 *Macro
*******************************************************************************************************************************************/
/*abstract for MemIf*/
#define IMemIf_FLAGS_APPLICATION_ID			0x00U
#define IMemIf_FLAGS_REPROGRAM_ID			0x01U
#define IMemIf_FLAGS_RESET_ID				0x02U

#define IFLAGS_appvalid						0x01U					
#define IFLAGS_reprogram					0x02U		
#define IFLAGS_reset						0x03U

typedef struct
{
    unsigned int IMemIfConfig_Addr;   /*for MemIfconfig address.*/
    unsigned int IMemIfConfig_Len;    /*for MemIfconfig datalength.*/
    unsigned int IMemIfConfig_Flag;   /*for MemIfconfig eventflag.*/
}IMemIf_ConfigType;


extern IMemIf_ConfigType IMemIfConfigList[3];



/*initializes the CanTp module */
void CanTp_Init(void);
/*initializes the Dcm module */
void Dcm_Init(void);
/* initializes the clock module */
void MCU_ClkSet(void);
/* initializes the Can Port module */
void Port_Init(void);
/* initializes the Can module */
void Can_Init(void);
/* initializes the Timer module */
void TIM_Init(void);
/* initializes the Ea module */
void Ea_Init(void);
/* initializes the Watch Dog module */
void WDog_Init(void);
/*MCU interrupt initialization*/
void MCU_ISR_Init(void);
/*Main Function of CanTp*/
void CanTp_MainFunction(void);
/*Main Function of Dcm*/
void Dcm_MainFunction(void);
/*Select EA as the memory type*/
void MemIf_SwtichToEaType(void);
/*Memory read Interface*/
StdReturnType MemIf_Read(unsigned int addr, unsigned int len, unsigned int* buf, unsigned int readflagId);
/*Call the erase function to erase the flag information in memory*/
StdReturnType MemIf_Erase(unsigned int addr, unsigned int len, unsigned int* buf);
/*for dcm delay cancel task*/
void DCM_10DelayCancel(void);
/*for response of reprogram task*/
void SendReprogramRespond(void);
/*for state of reprogram*/
void SetReprogramState(void);
/* start watchdog */
void WdgM_Start(void);
/* Can sends and receives packets periodically */
void Can_MainFunction_Write(void);
void Can_MainFunction_Read(void);
/* Reset MCU */
void SendResponseMCUReset(void);