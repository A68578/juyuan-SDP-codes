#include "CanIf.h"
void CanIf_Init(const CanIf_ConfigType* ConfigPtr){	printf("Task CanIf Init\n");}Std_ReturnType CanIf_Transmit(
	PduIdType TxPduId,
	const PduInfoType* PduInfoPtr);void CanIf_TxConfirmation(
	PduIdType CanTxPduId
);
void CanIf_RxIndication(	const Can_HwType* Mailbox,	const PduInfoType* PduInfoPtr
);