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


static void CanTp_ReceiveSingleFrame(const CanTpRxNSduType* rxNSdu, CanTp_Change_InfoType* rxRuntime, const PduInfoType* CanTpPduData)
{
	unsigned char rxSDU_Offset = rxRuntime->ISOTP_NPCI_Offset;
	PduInfoType rxPduData;  /* Pdur Receive Data */
	BufReq_ReturnType BufferRequest_Result = BUFREQ_E_NOT_OK;
	BufReq_ReturnType CopyData_Result = BUFREQ_E_NOT_OK;

	/* Idle = CANTP_ON.CANTP_RX_WAIT and CANTP_ON.CANTP_TX_WAIT */
   /* CanTp status = Idle so   Process the SF N-PDU as the start of a new reception */
   /* [SWS_CanTp_00124] When an SF or FF N-PDU without MetaData is received, and
					   the corresponding connection channel is currently receiving the same connection
					   (state CANTP_RX_PROCESSING, same N_AI), the CanTp module shall abort the
					   reception in progress and shall process the received frame as the start of a new reception */

	if (CANTP_RX_PROCESSING == rxRuntime->state)
	{
		/* Report an PDUR with E_NOT_OK to Abort*/
		Dcm_TpRxIndication(rxNSdu->CanTpRxNPduId, E_NOT_OK);


		/* Start new Reception */
		CanTp_Start_RX(rxRuntime);
	}
	else
	{
		/* Do Nothing */
	}

	/* Total length of the N-SDU Data to be received. */
	rxRuntime->transferTotal = *(CanTpPduData->SduDataPtr + rxSDU_Offset) & ISOTP_SF_DL_MASK;


	/* Pdu Recieved Data */
	rxPduData.SduDataPtr = CanTpPduData->SduDataPtr + rxSDU_Offset;
	rxPduData.SduLength = rxRuntime->transferTotal;

	/* Processing Mode*/
	rxRuntime->state = CANTP_RX_PROCESSING;

	/* [SWS_CanTp_00079] When receiving an SF or an FF N-PDU, the CanTp module
			 shall notify the upper layer (PDU Router) about this reception using the
			 PduR_CanTpStartOfReception function */

	BufferRequest_Result = Dcm_CanTpStartOfReception(rxNSdu->CanTpRxNSduId, &rxPduData, rxRuntime->transferTotal, &rxRuntime->Buffersize);

	switch (BufferRequest_Result)
	{
		/* [SWS_CanTp_00339] After the reception of a First Frame or Single Frame, if the
							 function PduR_CanTpStartOfReception() returns BUFREQ_OK with a smaller
							 available buffer size than needed for the already received data, the CanTp module
							 shall abort the reception of the N-SDU and call PduR_CanTpRxIndication() with
							 the result E_NOT_OK */
	case BUFREQ_OK:
		if (rxRuntime->Buffersize < rxRuntime->transferTotal)
		{
			Dcm_TpRxIndication(rxNSdu->CanTpRxNPduId, E_NOT_OK);
		}
		else
		{
			/* Id           ->  id of the received I-PDU.
			 * info         -> Provides the source buffer (SduDataPtr) and the number of
							   bytes to be copied
			 *bufferSizePtr -> Available receive buffer after data has been copied.
			 */
			CopyData_Result = Dcm_CopyRxData(rxNSdu->CanTpRxNSduId, &rxPduData, &rxRuntime->Buffersize);

			if (CopyData_Result == BUFREQ_OK)
			{
				Dcm_TpRxIndication(rxNSdu->CanTpRxNPduId, E_OK);
			}
			else if (CopyData_Result == BUFREQ_E_NOT_OK)
			{
				Dcm_TpRxIndication(rxNSdu->CanTpRxNPduId, E_NOT_OK);
			}
			/* End of the reception*/
			rxRuntime->substate = CANTPIDLE;
			rxRuntime->state = CANTP_RX_WAIT;
			break;
		}
		/*[SWS_CanTp_00081] After the reception of a First Frame or Single Frame, if the function
							   PduR_CanTpStartOfReception()returns BUFREQ_E_NOT_OK to the CanTp module,
							   the CanTp module shall abort the reception of this N-SDU. No Flow
							   Control will be sent and PduR_CanTpRxIndication() will not be called in this case. */


							   /* [SWS_CanTp_00353] After the reception of a Single Frame, if the function
												   PduR_CanTpStartOfReception()returns BUFREQ_E_OVFL to the CanTp module,
												   the CanTp module shall abort the N-SDU reception.*/
	case BUFREQ_E_NOT_OK:
		/* abort the reception*/
	case BUFREQ_E_OVFL:
		/* abort the reception*/
		rxRuntime->substate = CANTPIDLE;
		rxRuntime->state = CANTP_RX_WAIT;
		break;

	default:
		/* Do Nothing */
		break;
	}
}

static void CanTp_ReceiveFirstFrame(const CanTpRxNSduType* rxNSdu, CanTp_Change_InfoType* rxRuntime, const PduInfoType* CanTpPduData)
{
	unsigned char rxSDU_Offset = rxRuntime->ISOTP_NPCI_Offset;
	unsigned char MaxPayload;
	BufReq_ReturnType BufferRequest_Result = BUFREQ_E_NOT_OK;
	BufReq_ReturnType CopyData_Result = BUFREQ_E_NOT_OK;
	PduLengthType BuffersizeTemp;

	CanTp_SelectAddressMode(rxRuntime);

	MaxPayload = rxRuntime->MaxCFPayload;

	/* Status -> Segmented Receive in progress
			 Terminate the current reception,report an indication, with parameter Result set to
			 E_NOT_OK, to the upper layer, and process the FF N-PDU as the start of a new reception
   Status -> IDle
			Process the SF N-PDU as the start of a new reception */
	if (CANTP_RX_PROCESSING == rxRuntime->state)
	{
		/* Report an PDUR with E_NOT_OK to Abort*/
		Dcm_TpRxIndication(rxNSdu->CanTpRxNPduId, E_NOT_OK);

		/* Start new Reception */
		CanTp_Start_RX(rxRuntime);
	}
	else
	{
		/* Do Nothing */
	}

	/* Get total Length FF_DL */
	rxRuntime->transferTotal = ((CanTpPduData->SduDataPtr[rxSDU_Offset++] & ISOTP_FF_DL_MASK) << 8);
	rxRuntime->transferTotal |= CanTpPduData->SduDataPtr[rxSDU_Offset++];




#if CANTP_EXTENDED
	if (CanTpPduData->SduLength >= ISOTP_MAX_FRAME_CAN_BYTES)
	{
		if (0 == rxRuntime->transferTotal)
		{
			for (unsigned char i = 0; i < 4u; i++) {
				rxRuntime->transferTotal |= CanTpPduData->SduDataPtr[rxSDU_Offset] << (24 - 8 * i);
				rxSDU_Offset++;
			}
		}
	}
#endif

	/* Pdu Recieved Data */
	rxRuntimeParam->pdurBuffer.SduDataPtr = &CanTpPduData->SduDataPtr[indexCount];



	rxRuntimeParam->pdurBuffer.SduLength = CanTpPduData->SduLength - (PduLengthType)indexCount;


	/* Processing Mode*/
	rxRuntimeParam->mode = CANTP_RX_PROCESSING;
	BufferRequest_Status = Dcm_StartOfReception(rxNSduConfig->CanTpRxNSduId, &rxRuntimeParam->pdurBuffer, (PduLengthType)rxRuntimeParam->transferTotal, &rxRuntimeParam->Buffersize);
	BuffersizeTemp = rxRuntimeParam->Buffersize;

	switch (BufferRequest_Status)
	{
	case BUFREQ_OK:


		/* [SWS_CanTp_00339] After the reception of a First Frame or Single Frame, if the
							 function PduR_CanTpStartOfReception() returns BUFREQ_OK with a smaller
							 available buffer size than needed for the already received data, the CanTp module
							 shall abort the reception of the N-SDU and call PduR_CanTpRxIndication() with
							 the result E_NOT_OK */
		if (rxRuntimeParam->Buffersize < rxRuntimeParam->pdurBuffer.SduLength)
		{
			/* call PduR_CanTpRxIndication() with the result E_NOT_OK */
			Dcm_TpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);
			/* abort the reception*/
			rxRuntimeParam->state = CANTPIDLE;
			rxRuntimeParam->mode = CANTP_RX_WAIT;


		}
		else
		{
			/*[SWS_CanTp_00224] When the Rx buffer is large enough for the next block
							   (directly after the First Frame or the last Consecutive Frame of a block, or after
							   repeated calls to PduR_CanTpCopyRxData() according to SWS_CanTp_00222),
							   the CanTp module shall send a Flow Control N-PDU with ClearToSend status
							   (FC(CTS)) and shall then expect the reception of Consecutive Frame N-PDUs.*/


							   /* Id           ->  dentification of the received I-PDU.
								* info         -> Provides the source buffer (SduDataPtr) and the number of
												  bytes to be copied
								*bufferSizePtr -> Available receive buffer after data has been copied.
								*/
			CopyData_Status = Dcm_CopyRxData(rxNSduConfig->CanTpRxNSduId, &rxRuntimeParam->pdurBuffer, &rxRuntimeParam->Buffersize);

			switch (CopyData_Status)
			{
			case BUFREQ_OK: /* Data copied successfully */

				/* increase size of successfully transfered bytes */
				rxRuntimeParam->transferCount += rxRuntimeParam->pdurBuffer.SduLength;


				/* [SWS_CanTp_00082] After the reception of a First Frame, if the function
													 PduR_CanTpStartOfReception() returns BUFREQ_OK with a smaller available buffer
													 size than needed for the next block, the CanTp module shall start the timer N_Br.*/
				if (rxRuntimeParam->Buffersize < ((rxRuntimeParam->transferTotal - rxRuntimeParam->transferCount) & MaxPayload))
				{
					/* [SWS_CanTp_00222] While the timer N_Br is active, the CanTp module shall call
										 the service PduR_CanTpCopyRxData() with a data length 0 (zero) and NULL_PTR
										 as data buffer during each processing of the MainFunction */
										 /* Set N_BR timer */
					rxRuntimeParam->stateTimeoutCount = KILOSCALE(rxNSduConfig->CanTpNbr) / MAIN_FUNCTION_PERIOD_MILLISECONDS;

					/* main function will check this state to handle if the buffer size
					  become available or not and if the timer is expired send wait flow control */
					rxRuntimeParam->state = RX_WAIT_SDU_BUFFER;

					/* Receiver in progressing mode */
				}
				else /* size of available buffer is sufficient for next block */
				{
					rxRuntimeParam->Buffersize = BuffersizeTemp;

					/* Change State To wait Consecutive Frame */
					rxRuntimeParam->state = RX_WAIT_CONSECUTIVE_FRAME;

					/* Processing Received mode */

					/* Count on First CF = 1 */
					rxRuntimeParam->framesHandledCount = 1;

					/* send a Flow Control N-PDU with ContinueToSend status  */
					CanTp_SendFlowControlFrame(rxNSduConfig, rxRuntimeParam, FLOW_CONTROL_CTS_FRAME);
				}
				break;

			case BUFREQ_E_NOT_OK:
				Dcm_TpRxIndication(rxNSduConfig->CanTpRxNPduId, E_NOT_OK);
				break;

			default:
				/* Nothing */
				break;
			}
		}
		break;


		/*[SWS_CanTp_00081] After the reception of a First Frame or Single Frame, if the function
							PduR_CanTpStartOfReception()returns BUFREQ_E_NOT_OK to the CanTp module,
							the CanTp module shall abort the reception of this N-SDU. No Flow
							Control will be sent and PduR_CanTpRxIndication() will not be called in this case. */
	case BUFREQ_E_NOT_OK:
		/* abort the reception*/
		rxRuntimeParam->state = CANTPIDLE;
		rxRuntimeParam->mode = CANTP_RX_WAIT;
		break;


		/*[SWS_CanTp_00318]  After the reception of a First Frame, if the function
							 PduR_CanTpStartOfReception()returns BUFREQ_E_OVFL to the CanTp module,
							 the CanTp module shall send a Flow Control N-PDU with overflow status
							 (FC(OVFLW)) and abort the N-SDU reception.*/
							 /* No buffer of the required length can be
								  provided; reception is aborted. bufferSizePtr */
								  /* [SWS_CanTp_00353] After the reception of a Single Frame, if the function
														  PduR_CanTpStartOfReception()returns BUFREQ_E_OVFL to the CanTp module,
														  the CanTp module shall abort the N-SDU reception.*/
	case BUFREQ_E_OVFL:
		/* send a Flow Control N-PDU with overflow status */
		CanTp_SendFlowControlFrame(rxNSduConfig, rxRuntimeParam, FLOW_CONTROL_OVERFLOW_FRAME);


		/* abort the reception*/
		rxRuntimeParam->state = CANTPIDLE;
		rxRuntimeParam->mode = CANTP_RX_WAIT;
		break;
	default:
		break;
	}

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
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
			}
			else if (((*(PduInfoPtr->SduDataPtr)) & 0xF0)!= ISOTP_NPCI_SF)
			{
				Dcm_TpRxIndication(RxPduId, E_NOT_OK);
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_OPER_NOT_SUPPORTED);
			}
			else 
			{
				/*receive single frame.*/
				CanTp_ReceiveSingleFrame(&CanTpRxNSdu[RxPduId], &CanTp_Change_TxData[RxPduId], PduInfoPtr);
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
				CanTp_ReceiveFirstFrame(&CanTpRxNSdu[RxPduId], &CanTp_Change_TxData[RxPduId], PduInfoPtr);
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
			/* [SWS_CanTp_00093]  If a multiple segmented session occurs (on both receiver and sender side) with a handle whose communication type is functional,
								 the CanTp module shall reject the request and report the runtime error code CANTP_E_INVALID_TATYPE to the Default Error Tracer. */
			else if (CANTP_FUNCTIONAL == CanTpRxNSdu[RxPduId].CanTpRxTaType)
			{
				Dcm_TpRxIndication(RxPduId, E_NOT_OK);
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID,CANTP_RXINDICATION_SERVICE_ID, CANTP_E_INVALID_TATYPE);
			}
			else
			{
				//CanTp_ReceiveConsecutiveFrame(&CanTpRxNSdu[RxPduId], &CanTp_Change_TxData[RxPduId], PduInfoPtr);
			}
			break;

		case ISOTP_NPCI_FC: /* Flow Control*/
			/* [SWS_CanTp_00349]  If CanTpTxPaddingActivation is equal to CANTP_ON for a Tx N-SDU,and if a FC N-PDU is received for that Tx N-SDU on a ongoing transmission,by means of CanTp_RxIndication() call,
								  and the length of this FC is smaller than eight bytes (i.e. PduInfoPtr.SduLength <8) the CanTp module shall abort the transmission session calling PduR_CanTpTxConfirmation() with the result E_NOT_OK.
								  The runtime error code CANTP_E_PADDING shall be reported to the Default Error Tracer. */
			if ((PduInfoPtr->SduLength < ISOTP_MAX_FRAME_CAN_BYTES) && (CANTP_ON == CanTpRxNSdu[RxPduId].CanTpRxPaddingActivation))
			{
				Dcm_TpTxConfirmation(CanTpTxNSdu[RxPduId].CanTpRxFcNPduId, E_NOT_OK);

				CanTp_Start_TX(&CanTp_Change_TxData[0]);
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID,CANTP_RXINDICATION_SERVICE_ID, CANTP_E_PADDING);
			}
			/* [SWS_CanTp_00093]  If a multiple segmented session occurs (on both receiver and sender side) with a handle whose communication type is functional,
								  the CanTp module shall reject the request and report the runtime error code CANTP_E_INVALID_TATYPE to the Default Error Tracer. */
			else if (CANTP_FUNCTIONAL == CanTpRxNSdu[RxPduId].CanTpRxTaType)
			{
				Dcm_TpRxIndication(RxPduId, E_NOT_OK);
				Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_RXINDICATION_SERVICE_ID, CANTP_E_INVALID_TATYPE);
			}
			else
			{
				/* Get the Frame Status for Flow Control */
				//CanTp_ReceiveFlowControlFrame(&CanTpTxNSdu[RxPduId], &CanTp_Change_TxData[RxPduId], PduInfoPtr);
			}
			break;

		default:
			break;
	}
}




/*
1.LSduR模块在成功接收一个Rx CAN L-PDU后，应调用此函数。
2.数据将由CAN传输协议（CanTp）通过协议数据单元（PDU）结构PduInfoType进行复制。在这种情况下，L-PDU缓冲区不是全局的，因此分布在相应的CAN传输层中。
3.请注意，在动态接收N-PDU的情况下，PduInfoPtr还包含元数据。
[SWS_CanTp_00235] CanTp_RxIndication函数应在中断上下文中可调用（即可以从CAN接收中断中调用）。
[SWS_CanTp_00322] 如果启用了开发错误检测，那么当参数PduInfoPtr为空指针时，函数CanTp_RxIndication应引发CANTP_E_PARAM_POINTER错误
[SWS_CanTp_00359] 如果启用了开发错误检测，则CanTp_RxIndication函数应检查函数参数RxPduId的有效性。如果其值无效，则CanTp_RxIndication函数应引发开发错误CANTP_E_INVALID_RX_ID
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
	CanTp_AddressOffset = (PduInfoPtr->SduDataPtr + CanTpPduPCI_Offset);

	CanTp_FrameType = *CanTp_AddressOffset & ISOTP_NPCI_MASK;

	CanTp_RxIndication_HandledFrameType(CanTp_FrameType, RxPduId, PduInfoPtr);

}

/*
在通过CAN网络传输与TP相关的CAN帧（SF、FF、CF、FC）后，LSduR模块应调用CanTp_TxConfirmation函数。
[SWS_CanTp_00236] CanTp_TxConfirmation函数应在中断上下文中可调用（即可以从CAN发送中断中调用）
[SWS_CanTp_00360] 如果启用了开发错误检测，则CanTp_TxConfirmation函数应检查函数参数TxPduId的有效性。
如果其值无效，则CanTp_TxConfirmation函数应引发开发错误CANTP_E_INVALID_TX_ID
*/

void CanTp_TxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
	FrameType TxPduIDtype;
	unsigned char ConfirmFlag = 0;
	//[SWS_CanTp_00031] If development error detection for the CanTp module is enabled 
	// the CanTp module shall raise an error (CANTP_E_UNINIT)
	//when the PDU Router or CAN Interface tries to use any function (except CanTp_GetVersionInfo) 
	// before the function CanTp_Init has been called. (SRS_Can_01076)
	if (CANTP_OFF == CanTpInternalState)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TRANSMIT_SERVICE_ID, CANTP_E_UNINIT);
	}
	else if (TxPduId >= NUMBER_OF_TXNPDU)
	{
		Det_ReportRuntimeError(CANTP_MODULE_ID, CANTP_INSTANCE_ID, CANTP_TXCONFIRMATION_SERVICE_ID, CANTP_E_INVALID_TX_ID);
	}
	if (TxPduId < NUMBER_OF_TXNPDU)
	{
		if (E_NOT_OK == result)
		{
			//[SWS_CanTp_00355] CanTp shall abort the corrensponding session, when CanTp_TxConfirmation() is called with the result E_NOT_OK.
			CanTp_Change_TxData[TxPduId].state = CANTP_TX_WAIT;
			CanTp_Change_TxData[TxPduId].substate = CANTPIDLE;
			ConfirmFlag = 1;
		}
		else
		{
			TxPduIDtype = *(CanTp_Change_TxData[TxPduId].CanIfTransData.SduDataPtr) & ISOTP_NPCI_MASK;

			switch (TxPduIDtype)
			{
			case ISOTP_NPCI_SF:
				CanTp_Change_TxData[TxPduId].state = CANTP_TX_WAIT;
				CanTp_Change_TxData[TxPduId].substate = CANTPIDLE;
				ConfirmFlag = 1;
				break;

				// According to ISO15765-2,if a First Frame was sent,N_Bs Timer will start,then the Flow Control Frame waits to Send.
			case ISOTP_NPCI_FF:
				CanTp_Change_TxData[TxPduId].stateTimeOutCounter = (unsigned int)((CanTpTxNSdu[TxPduId].CanTpNbs) * 1000) / MAIN_FUNCTION_PERIOD_MILLISECONDS;
				CanTp_Change_TxData[TxPduId].substate = TX_WAIT_FLOW_CONTROL;
				break;

			case ISOTP_NPCI_CF:
				//CanTp_HandleNextTxFrame(&CanTpTxNSdu[TxPduId], &CanTp_Change_TxData[TxPduId]);
				break;

			default:
				/* Do Nothing */
				break;
			}
		}
		if (ConfirmFlag)
		{
			/*[SWS_CanTp_00204] The CanTp module shall notify the upper layer by calling the
		   PduR_CanTpTxConfirmation callback when the transmit request has been completed */
			Dcm_TpTxConfirmation(TxPduId, result);
		}
	}
	else
	{
		/* Do Nothing */
	}
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	








