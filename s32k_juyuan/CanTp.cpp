#include "CanTp.h"
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
	unsigned char ISOTP_CANFD_PAYLOAD_SF,
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
1.如果数据确实适合相关的N-PDU，则函数CanTp_Transmit（）应发送SF N-PDU。
2.如果数据不适合相关的N-PDU，函数CanTp_Transmit（）应启动多帧传输会话。
3.N-PDU的最大Tx长度应从EcuC的PduLength配置参数中得出。此参数相当于ISO 15765-2的TX_DL。
4.当传输请求完成时，CanTp模块应通过调用PduR_CanTpTx确认回调来通知上层。
5.如果发生错误（溢流、N_As超时、N_Bs超时等），CanTp模块应中止传输请求，并使用E_NOT_OK结果值调用PduR_CanTpTxConfirmation回调函数。
6.如果CanTp_Transmit服务被调用以获取当前运行的CAN传输层会话中使用的N-SDU标识符，则CanTp_Ttransmit功能应拒绝请求,
因此，为了保证数据的一致性，上层（例如DCM、PduRouter或AUTOSAR COM）必须锁定此存储区域，直到出现确认通知。
7.当上层为不含元数据的N-SDU调用此函数时，只需使用PduInfoPtr所指示结构的数据长度信息。该指针的值表示待传输的N-SDU的有效载荷长度。
对于带有元数据的N-SDU，除了长度信息外，通过PduInfoPtr提供的元数据也至关重要。为了获取实际的Tx数据，CAN传输层应调用PduR_CanTpCopyTxData服务。
8.如果启用了开发错误检测，则该函数如果参数PduInfoPtr为空指针，则CanTp_Transmit应引发CANTP_E_PARAM_POINTER错误。
9.如果启用了开发错误检测，则函数CanTp_Transmit应检查函数参数TxPduId的有效性。如果其值无效，则CanTp_Transmit函数应引发开发错误CANTP_E_INVALID_TX_ID。
*/

Std_ReturnType CanTp_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{
	Std_ReturnType TransmitResult = E_NOT_OK;
	unsigned int CanIfTransOffset = 0;
	if (CanTpInternalState == CANTP_OFF)
	{
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_UNINIT);
	}
	else if (PduInfoPtr == NULL_PTR)
	{
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_PARAM_POINTER);
	}
	else if (TxPduId >= NUMBER_OF_TXNPDU)
	{
		Det_ReportError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_INVALID_TX_ID);
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
		else if(CANTPIDLE == CanTp_Change_TxData[TxPduId].state)
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
				CanTp_Change_TxData[TxPduId].CanIfTransData.SduLength = CanTp_Change_TxData[TxPduId].transferTotal;
				CanIfTransOffset = CanIfTransOffset + 1;
				
				
				CanTp_Change_TxData[TxPduId].upperTransData.SduLength = (PduLengthType)CanTp_Change_TxData[TxPduId].transferTotal;
			}
			/*multi frame*/
			else 
			{
				;
			}

		}
	}
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
