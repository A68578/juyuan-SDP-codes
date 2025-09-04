/******************************************************************************************************************************************
								 *Include
*******************************************************************************************************************************************/
#include "CanTp.h"
/******************************************************************************************************************************************
								 *Macro
*******************************************************************************************************************************************/
#define CANIF_TO_CANTP_PDUID(x)  (x)

/********************************************************************************************************************************************
								 *Variables static
*********************************************************************************************************************************************/
/* [SWS_CanTp_00168] The CanTp module shall be in the CANTP_OFF state after power up. */

/* Static Global Variable to hold CanTp Internal State {CANTP_OFF,CANTP_ON}*/
unsigned char CanTpInternalState = (unsigned char)CANTP_OFF;

static void CanTp_Start_RX(CanTp_Change_InfoType* rxRunBuffer)
{
	/*[SWS_CanTp_00030] The function CanTp_Init shall initialize all global variables of the module and sets
	 * all transport protocol connections in a sub-state of CANTP_ON, in which neither segmented transmission
	 * nor segmented reception are in progress (Rx thread in state CANTP_RX_WAIT and Tx thread in state CANTP_TX_WAIT)*/
	 /* Initialize all Tx runtime variable that will be used during runTime */

	 /* Initialize all Tx runtime variable that will be used during runTime */
	rxRunBuffer->state = CANTPIDLE;
	rxRunBuffer->transferTotal = 0;
	rxRunBuffer->Buffersize = 0;
	rxRunBuffer->availableDataSize = 0;
	rxRunBuffer->transferCount = 0;
	rxRunBuffer->framehandledCounter = 0;
	rxRunBuffer->nextFlowCounter = 0;
	rxRunBuffer->upperTransData.SduDataPtr = NULL_PTR;
	rxRunBuffer->CanIfTransData.SduDataPtr = NULL_PTR;
}


static void CanTp_Start_TX(CanTp_Change_InfoType* txRunBuffer)
{
	/*[SWS_CanTp_00030] The function CanTp_Init shall initialize all global variables of the module and sets
	 * all transport protocol connections in a sub-state of CANTP_ON, in which neither segmented transmission
	 * nor segmented reception are in progress (Rx thread in state CANTP_RX_WAIT and Tx thread in state CANTP_TX_WAIT). (SRS_Can_01075)*/


	 /* Initialize all Tx runtime variable that will be used during runTime */


	txRunBuffer->state = CANTPIDLE;
	txRunBuffer->transferCount = 0;
	txRunBuffer->framehandledCounter = 0;
	txRunBuffer->transferTotal = 0;
	txRunBuffer->Buffersize = 0;
	txRunBuffer->availableDataSize = 0;
	txRunBuffer->nextFlowCounter = 0;
	txRunBuffer->upperTransData.SduDataPtr = txRunBuffer->upperLayerbuffer;
	txRunBuffer->upperTransData.SduLength = 0;
	txRunBuffer->CanIfTransData.SduDataPtr = txRunBuffer->CanIfbuffer;
	txRunBuffer->CanIfTransData.SduLength = 0;
}

void CanTp_Shutdown(void)
{
	/* [SWS_CanTp_00202] The function CanTp_Shutdown shall close all pending transport protocol connections,
	* free all resources and set the CanTp module into the CANTP_OFF state. */

	/*[SWS_CanTp_00010] The function CanTp_Shutdown shall stop the CanTp module properly.(SRS_BSW_00336)*/

	/* Putting CanTp Module in the OFF state to stop module safely */
	CanTpInternalState = (unsigned char)CANTP_OFF;
}

static void CanTp_HandleAddressMode(CanTp_Change_InfoType* CanTpRuntimeBuffer)
{
	unsigned char ISOTP_MAX_PAYLOAD_SF, ISOTP_MAX_PAYLOAD_FF;
	ISOTP_MAX_PAYLOAD_SF = 7;
	ISOTP_MAX_PAYLOAD_FF = 6;
	/*consecutive frame max payload.*/
	CanTpRuntimeBuffer->MaxCFPayload = 7 - CanTpRuntimeBuffer->ISOTP_NPCI_Offset;
	/*single frame max payload.*/
	CanTpRuntimeBuffer->MaxSFPayload = ISOTP_MAX_PAYLOAD_SF - CanTpRuntimeBuffer->ISOTP_NPCI_Offset;
}




static void CanTp_SelectAddressMode(CanTp_Change_InfoType* CanTpRuntimeBuffer)
{
	switch (CanTpRuntimeBuffer->addressformat)
	{
	case CANTP_STANDARD:
	case CANTP_NORMALFIXED:
		CanTpRuntimeBuffer->ISOTP_NPCI_Offset = 0x00u;
		break;

	case CANTP_EXTENDED:
	case CANTP_MIXED:
	case CANTP_MIXED29BIT:
		CanTpRuntimeBuffer->ISOTP_NPCI_Offset = 0x01u;
		break;

	default:
		/* Do Nothing */
		break;
	}
}

/*
1.�������ȷʵ�ʺ���ص�N-PDU������CanTp_Transmit����Ӧ����SF N-PDU��
2.������ݲ��ʺ���ص�N-PDU������CanTp_Transmit����Ӧ������֡����Ự��
3.N-PDU�����Tx����Ӧ��EcuC��PduLength���ò����еó����˲����൱��ISO 15765-2��TX_DL��
4.�������������ʱ��CanTpģ��Ӧͨ������PduR_CanTpTxȷ�ϻص���֪ͨ�ϲ㡣
5.�����������������N_As��ʱ��N_Bs��ʱ�ȣ���CanTpģ��Ӧ��ֹ�������󣬲�ʹ��E_NOT_OK���ֵ����PduR_CanTpTxConfirmation�ص�������
6.���CanTp_Transmit���񱻵����Ի�ȡ��ǰ���е�CAN�����Ự��ʹ�õ�N-SDU��ʶ������CanTp_Ttransmit����Ӧ�ܾ�����,
��ˣ�Ϊ�˱�֤���ݵ�һ���ԣ��ϲ㣨����DCM��PduRouter��AUTOSAR COM�����������˴洢����ֱ������ȷ��֪ͨ��
7.���ϲ�Ϊ����Ԫ���ݵ�N-SDU���ô˺���ʱ��ֻ��ʹ��PduInfoPtr��ָʾ�ṹ�����ݳ�����Ϣ����ָ���ֵ��ʾ�������N-SDU����Ч�غɳ��ȡ�
���ڴ���Ԫ���ݵ�N-SDU�����˳�����Ϣ�⣬ͨ��PduInfoPtr�ṩ��Ԫ����Ҳ������Ҫ��Ϊ�˻�ȡʵ�ʵ�Tx���ݣ�CAN�����Ӧ����PduR_CanTpCopyTxData����
8.��������˿��������⣬��ú����������PduInfoPtrΪ��ָ�룬��CanTp_TransmitӦ����CANTP_E_PARAM_POINTER����
9.��������˿��������⣬����CanTp_TransmitӦ��麯������TxPduId����Ч�ԡ������ֵ��Ч����CanTp_Transmit����Ӧ������������CANTP_E_INVALID_TX_ID��
*/

Std_ReturnType CanTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{
	Std_ReturnType TransmitResult = E_NOT_OK;
	unsigned int CanIfTransOffset = 0;
	unsigned int CanIfTrans_MultiFrame_Partial_Length = 0;
	if (CanTpInternalState == CANTP_OFF)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_UNINIT);
	}
	else if (PduInfoPtr == NULL_PTR)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_PARAM_POINTER);
	}
	else if (TxPduId >= NUMBER_OF_TXNPDU)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_INVALID_TX_ID);
	}
	else if (PduInfoPtr->SduLength > 4096u)
	{
		TransmitResult = E_NOT_OK;
	}
	/*start a new transmission*/
	CanTp_Start_TX(&CanTp_Change_TxData[TxPduId]);
	/* [SWS_CanTp_00123] If the configured transmit connection channel is in use (state CANTP_TX_PROCESSING),
						 the CanTp module shall reject new transmission requests linked to this channel.
						 To reject a transmission, CanTp returns E_NOT_OK when the
						 upper layer asks for a transmission with the CanTp_Transmit() function */

	if (CANTP_ON == CanTpInternalState)
	{
		/*if it is processing,result is fail.*/
		if (CANTP_TX_PROCESSING == CanTp_Change_TxData[TxPduId].state)
		{
			TransmitResult = E_NOT_OK;
		}
		else if(CANTPIDLE == CanTp_Change_TxData[TxPduId].substate)
		{
			CanTp_Change_TxData[TxPduId].transferTotal = PduInfoPtr->SduLength;
			CanTp_Change_TxData[TxPduId].addressformat = CanTpTxNSdu[TxPduId].CanTpTxAddressingFormat;
			CanTp_Change_TxData[TxPduId].state = CANTP_TX_PROCESSING;
			/*select the N-PDU PCI address type.*/
			CanTp_SelectAddressMode(&CanTp_Change_TxData[TxPduId]);
			/*handle the payload of data.*/
			CanTp_HandleAddressMode(&CanTp_Change_TxData[TxPduId]);
			/*single frame.*/
			if (CanTp_Change_TxData[TxPduId].transferTotal <= CanTp_Change_TxData[TxPduId].MaxSFPayload)
			{
				CanIfTransOffset = CanTp_Change_TxData[TxPduId].CanIfTransData.SduLength;
				
				CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr = CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr + CanIfTransOffset;
				
				*(CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr) = ISOTP_NPCI_SF;
				
				CanIfTransOffset = CanIfTransOffset + 1;
				
				CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr = CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr + CanIfTransOffset;
				
				*(CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr) = CanTp_Change_TxData[TxPduId].transferTotal;
				
				CanIfTransOffset = CanIfTransOffset + 1;
				
				CanTp_Change_TxData[TxPduId].upperTransData.SduLength = (PduLengthType)CanTp_Change_TxData[TxPduId].transferTotal;
			}
			/*multi frame:First frame.*/
			else 
			{
				CanIfTransOffset = CanTp_Change_TxData[TxPduId].CanIfTransData.SduLength;
				
				CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr = CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr + CanIfTransOffset;
				
				CanIfTrans_MultiFrame_Partial_Length = (CanTp_Change_TxData[TxPduId].transferTotal) & 0xF00;
				
				*(CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr) = ISOTP_NPCI_FF| CanIfTrans_MultiFrame_Partial_Length;
				
				CanIfTrans_MultiFrame_Partial_Length = (CanTp_Change_TxData[TxPduId].transferTotal) & 0xFF;

				CanIfTransOffset = CanIfTransOffset + 1;

				*(CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr) = CanIfTrans_MultiFrame_Partial_Length;

				CanIfTransOffset = CanIfTransOffset + 1;

				CanTp_Change_TxData[TxPduId].upperTransData.SduLength = (PduLengthType)CanTp_Change_TxData[TxPduId].transferTotal;
			}

		}
		CanTp_Change_TxData[TxPduId].substate = TX_WAIT_TRANSMIT;
		TransmitResult = E_OK;
	}
	else
	{
		TransmitResult = E_NOT_OK;
	}
	return TransmitResult;
}

void CanTp_Init(const CanTp_ConfigType* CfgPtr)
{
	PduIdInfoType rxChannelCount = 0;
	PduIdInfoType txChannelCount = 0;

	/* [SWS_CanTp_00030] The function CanTp_Init shall initialize all global variables of the module and sets
	* all transport protocol connections in a sub-state of CANTP_ON, in which neither segmented transmission
	* nor segmented reception are in progress (Rx thread in state CANTP_RX_WAIT and Tx thread in state CANTP_TX_WAIT). (SRS_Can_01075)*/
	/*change to initial table*/
	for (rxChannelCount = 0; rxChannelCount < NUMBER_OF_RXNPDU; rxChannelCount++)
	{
		CanTp_Start_RX(&CanTp_Change_RxData[rxChannelCount]);
	}
	for (txChannelCount = 0; txChannelCount < NUMBER_OF_TXNPDU; txChannelCount++)
	{
		CanTp_Start_TX(&CanTp_Change_TxData[txChannelCount]);
	}
	/* [ SWS_CanTp_00170] The CanTp module shall change to the internal state CANTP_ON
	* when the CanTp has been successfully initialized with CanTp_Init(). (SRS_Can_01075)*/

	/* Putting CanTp Module in the ON state to let other functions work */
	CanTpInternalState = (unsigned char)CANTP_ON;
}


static void CanTp_RxIndication_HandledFrameType(unsigned char frametype, PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
	switch (frametype)
	{
		case ISOTP_NPCI_SF:/*Single Frame*/
			if (CANTP_TX_PROCESSING == CanTp_Change_TxData[CanTpRxNSdu[RxPduId].CanTpTxFcNPduConfirmationPduId].state)
			{
				return;
			}
			if ((PduInfoPtr->SduLength < ISOTP_MAX_FRAME_CAN_BYTES) && (CANTP_ON == CanTpRxNSdu[RxPduId].CanTpRxPaddingActivation))
			{
				Dcm_TpRxIndication(RxPduId, E_NOT_OK);
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
			}
			else if (((*(PduInfoPtr->SduDataPtr)) & 0xF0)!= ISOTP_NPCI_SF)
			{
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_OPER_NOT_SUPPORTED);
			}
			else 
			{
				/*receive single frame.*/
				//CanTp_ReceiveSingleFrame(&CanTpRxNSdu[RxPduId], &CanTp_Change_TxData[RxPduId], PduInfoPtr);
			}
			break;

		case ISOTP_NPCI_FF: /* First Frame */
			/*[SWS_CanTp_00093]  If a multiple segmented session occurs (on both receiver and sender side) with a handle whose communication type is functional,
							   the CanTp module shall reject the request and report the runtime error code CANTP_E_INVALID_TATYPE to the Default Error Tracer. */

			if (CANTP_FUNCTIONAL == CanTpRxNSdu[RxPduId].CanTpRxTaType)
			{
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_INVALID_TATYPE);
			}
			if ((((*(PduInfoPtr->SduDataPtr)) & 0xF0) != ISOTP_NPCI_FF))
			{
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_OPER_NOT_SUPPORTED);
			}
			else
			{
				/*receive first frame.*/
				//CanTp_ReceiveFirstFrame(&CanTpRxNSdu[RxPduId], &CanTp_Change_TxData[RxPduId], PduInfoPtr);
			}
			break;

		case ISOTP_NPCI_CF: /* Consecutive Frame*/
			/*[SWS_CanTp_00345]  If frames with a payload <= 8 (either CAN 2.0 frames or small CAN FD frames) are used for a Rx N-SDU and CanTpRxPaddingActivation is equal to CANTP_ON,
								 then CanTp receives by means of CanTp_RxIndication() call an SF Rx N-PDU belonging to that N-SDU, with a length smaller than eight bytes (i.e. PduInfoPtr.SduLength < 8),
								 CanTp shall reject the reception.The runtime error code CANTP_E_PADDING shall be reported to the Default Error Tracer. */
			/* [SWS_CanTp_00346]  If frames with a payload <= 8 (either CAN 2.0 frames or small CAN FD frames) are used for a Rx N-SDU and CanTpRxPaddingActivation is equal to CANTP_ON,
								  and CanTp receives by means of CanTp_RxIndication() call a last CF Rx N-PDU belonging to that NSDU, with a length smaller than eight bytes(i.e. PduInfoPtr. SduLength != 8),
								  CanTp shall abort the ongoing reception by calling PduR_CanTpRxIndication() with the result E_NOT_OK. The runtime error code CANTP_E_PADDING shall be reported to the Default Error Tracer*/
			if (CANTP_TX_PROCESSING == CanTp_Change_TxData[CanTpRxNSdu[RxPduId].CanTpTxFcNPduConfirmationPduId].state)
			{
				return;
			}
			if ((PduInfoPtr->SduLength < ISOTP_MAX_FRAME_CAN_BYTES) && (CANTP_ON == CanTpRxNSdu[RxPduId].CanTpRxPaddingActivation))
			{
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
			}

			break;
	}
}




/*
1.LSduRģ���ڳɹ�����һ��Rx CAN L-PDU��Ӧ���ô˺�����
2.���ݽ���CAN����Э�飨CanTp��ͨ��Э�����ݵ�Ԫ��PDU���ṹPduInfoType���и��ơ�����������£�L-PDU����������ȫ�ֵģ���˷ֲ�����Ӧ��CAN������С�
3.��ע�⣬�ڶ�̬����N-PDU������£�PduInfoPtr������Ԫ���ݡ�
[SWS_CanTp_00235] CanTp_RxIndication����Ӧ���ж��������пɵ��ã������Դ�CAN�����ж��е��ã���
[SWS_CanTp_00322] ��������˿��������⣬��ô������PduInfoPtrΪ��ָ��ʱ������CanTp_RxIndicationӦ����CANTP_E_PARAM_POINTER����
[SWS_CanTp_00359] ��������˿��������⣬��CanTp_RxIndication����Ӧ��麯������RxPduId����Ч�ԡ������ֵ��Ч����CanTp_RxIndication����Ӧ������������CANTP_E_INVALID_RX_ID
*/
void CanTp_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
	unsigned char CanTpPduPCI_Offset;
	unsigned char CanTp_FrameType;
	unsigned char* CanTp_AddressOffset = 0;

	if (CanTpInternalState == CANTP_OFF)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_UNINIT);
	}
	else if (PduInfoPtr == NULL_PTR)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_PARAM_POINTER);
	}
	else if (RxPduId >= NUMBER_OF_RXNPDU)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_INVALID_RX_ID);
	}
	/*canif to cantp*/
	RxPduId = CANIF_TO_CANTP_PDUID(RxPduId);

	CanTp_Change_RxData[RxPduId].addressformat = CanTpRxNSdu[RxPduId].CanTpRxAddressingFormat;

	/*select the N-PDU PCI address type.*/
	CanTp_SelectAddressMode(&CanTp_Change_RxData[RxPduId]);
	/*handle the payload of data.*/
	CanTp_HandleAddressMode(&CanTp_Change_RxData[RxPduId]);

	/*cantp_pci_offset.*/
	CanTpPduPCI_Offset = CanTp_Change_RxData[RxPduId].ISOTP_NPCI_Offset;

	/* Get The Frame Type */
	//CanTp_FrameType = PduInfoPtr->SduDataPtr[CanTpPduPCI_Offset] & ISO15765_NPCI_MASK;
	CanTp_AddressOffset = (PduInfoPtr->SduDataPtr + CanTpPduPCI_Offset);

	CanTp_FrameType = *CanTp_AddressOffset & ISOTP_NPCI_MASK;

	CanTp_RxIndication_HandledFrameType(CanTp_FrameType, RxPduId, PduInfoPtr);

}
