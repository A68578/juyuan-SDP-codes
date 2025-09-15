#include "CanIf_Cfg.h"
/*
const CanIf_RxPduCfg_Type CanIf_RxPduCfg[CanIf_RxPdu_MAX_NUM] =
{
	{0x714U,		64,		CanTp_RxIndication},
	{0x7DFU,		64,		CanTp_RxIndication}
};
*/
const CanIf_TxPduCfg_Type CanIf_TxPduCfg[CanIf_TxPdu_MAX_NUM] =
{
	{CanTp_TxConfirmation}
};