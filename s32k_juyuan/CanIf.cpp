#include "CanIf.h"


void CanIf_Init(const CanIf_ConfigType* ConfigPtr)
{
	printf("Task CanIf Init\n");
}
Std_ReturnType CanIf_Transmit(PduIdType TxPduId,const PduInfoType* PduInfoPtr)
{
	Std_ReturnType retval = E_NOT_OK;
	Can_PduType CanIfTransData = {0};

	CanIfTransData.swPduHandle = CANIF_CANTXPDUID;
	CanIfTransData.id = CANIF_TXPDU_CANID;
	CanIfTransData.length = (unsigned char)PduInfoPtr->SduLength;
	CanIfTransData.sdu = PduInfoPtr->SduDataPtr;




}

void CanIf_TxConfirmation(PduIdType CanTxPduId)
{

}








void CanIf_RxIndication(
	const Can_HwType* Mailbox,
	const PduInfoType* PduInfoPtr
){ }

