#include "CanIf.h"


void CanIf_Init(const CanIf_ConfigType* ConfigPtr)
{
	printf("Task CanIf Init\n");
}
Std_ReturnType CanIf_Transmit(const PduInfoType* PduInfoPtr)
{
	Std_ReturnType retval = E_NOT_OK;
	Can_PduType CanIfTransData = {0};

	CanIfTransData.swPduHandle = CANIF_CANTXPDUID;
	CanIfTransData.id = CANIF_TXPDU_CANID;
	CanIfTransData.length = (unsigned char)PduInfoPtr->SduLength;
	CanIfTransData.sdu = PduInfoPtr->SduDataPtr;

	/* call can write to transmit can msg */
	if (CAN_OK == Can_Write(&CanIfTransData))
	{
		retval = E_OK;
	}

	return retval;
}

void CanIf_TxConfirmation(PduIdType CanTxPduId)
{
	if (CanTxPduId > CanIf_TxPdu_MAX_NUM)
	{
		printf("error channel\n");
		return;
	}
	else
	{
		CanIf_TxPduCfg[CanTxPduId].UserTxConfirmationName(CanTxPduId, E_OK);
	}
}








void CanIf_RxIndication(const PduInfoType* PduInfoPtr)
{

}

