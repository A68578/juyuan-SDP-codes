
#include "CanTp_Cfg.h"
CanTp_Change_InfoType CanTp_Change_RxData[] =
{
	{},
	{}
};

CanTp_Change_InfoType CanTp_Change_TxData[] =
{
	{}
};

/********************************************************************************************************************************************
								 *Constants Prototype
*********************************************************************************************************************************************/
const CanTpRxNSduType CanTpRxNSdu[NUMBER_OF_RXNPDU] =
{
	/*CanTpBs*/		/*CanTpNar*/		/*CanTpNbr*/		/*CanTpNcr*/		/*CanTpRxAddressingFormat*/		/*CanTpRxNSduId*/		/*CanTpRxPaddingActivation*/		/*CanTpRxTaType*/		/*CanTpRxWftMax*/		/*CanTpSTmin*/		/*CanTpRxNPduId*/		/*CanTpTxFcNPduConfirmationPduId*/
	//{0,				1.00,				1.00,				1.00,				CANTP_STANDARD,					0,						CANTP_OFF,							CANTP_PHYSICAL,			0,						0,					0,						CANTP_RESP_LOWLAYER_TXPDUID							}, //PDUR_CANTPRXSDUID_0x746
	//{0,				1.00,				1.00,				1.00,				CANTP_STANDARD,					1,						CANTP_OFF,							CANTP_FUNCTIONAL,		0,						0,					1,						CANTP_RESP_LOWLAYER_TXPDUID								}  //PDUR_CANTPRXSDUID_0x7DF
};

const CanTpTxNSduType CanTpTxNSdu[NUMBER_OF_TXNPDU] =
{
	/*CanTpNas*/		/*CanTpNbs*/		/*CanTpNcs*/		/*CanTpTc*/		/*CanTpTxAddressingFormat*/		/*CanTpTxNSduId*/		/*CanTpTxPaddingActivation*/		/*CanTpTxTaType*/		/*CanTpTxNPduConfirmationPduId*/		/*CanTpRxFcNPduId*/
	//{0.07,			 	1.0,				0.07,				TRUE,			CANTP_STANDARD,					0,						CANTP_OFF,							CANTP_PHYSICAL,			CANTP_RESP_LOWLAYER_TXPDUID,										0					}                                          //PDUR_CANTPTXSDUID_0x74E

};