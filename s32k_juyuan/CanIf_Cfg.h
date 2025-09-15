#pragma once
#include "common.h"
#include "ComStack_Types.h"
#include "CanTp.h"

#define CANIF_CANTXPDUID            0
#define CANIF_TXPDU_CANID         	0x794U
#define CANIF_RXPDU_CANID_1     	0x714U
#define CANIF_RXPDU_CANID_2     	0x7DFU
#define CanIf_RxPdu_MAX_NUM			2U
#define CanIf_TxPdu_MAX_NUM			1U



typedef struct
{
	unsigned int CanId;
	unsigned int Date_length;
	void (*UserRxIndicationName)(PduIdType, const PduInfoType*);
}CanIf_RxPduCfg_Type;

typedef struct
{
	void (*UserTxConfirmationName)(PduIdType, Std_ReturnType);
}CanIf_TxPduCfg_Type;


extern const CanIf_RxPduCfg_Type CanIf_RxPduCfg[CanIf_RxPdu_MAX_NUM];
extern const CanIf_TxPduCfg_Type CanIf_TxPduCfg[CanIf_TxPdu_MAX_NUM];