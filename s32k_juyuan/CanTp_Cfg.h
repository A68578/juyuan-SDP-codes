#pragma once
#include "common.h"
#include "CanTp_Types.h"
#define    NUMBER_OF_TXNPDU                                                (1u)
#define    NUMBER_OF_RXNPDU                                                (1u)

#define    CANTP_EXTEND                                                     0

#define    CANTP_RESP_LOWLAYER_TXPDUID										0
#define    CANTP_REQ_LOWLAYER_RXPDUID										0


#define   CANIF_TX_DL                                                       0x40


#define    CanTpMainFunctionPeriod                                         (0.01f)

#define    MAIN_FUNCTION_PERIOD_MILLISECONDS                              (uint32)(CanTpMainFunctionPeriod * 1000)

/*
Used for the initialization of unused bytes with a certain value
*/
#define    CanTpPaddingByte                                                (0x55u)

#define    CustomInOut(x,condition,y1,y2)                                 (((x) <= (condition))?(y1):(y2))
#if (CANTP_EXTEND == 1)
#define    ISOTP_MAX_PADDING_BYTES(x)                                     CustomInOut((uint8)x,ISOI5765_MAX_FRAME_CAN20_BYTES,ISOI5765_MAX_FRAME_CAN20_BYTES,CustomInOut((uint8)x,0x18u,(uint8)(((uint8)(x - 1u) & 0xFCu) + 0x04u),CustomInOut(x,0x40u,(uint8)(((uint8)(x - 1u) & 0xF0u) + 0x10u),0u)))
#endif
#define ISOTP_MAX_PADDING_BYTES                                           (0x08u)

typedef struct
{
    uint8* SduDataPtr;
    PduLengthType SduLength;
}CanTp_CanIfPduInfoType;


typedef struct
{
	uint32 nextFlowCounter;             
	uint32 framehandledCounter;
	uint32 stateTimeOutCounter;
    uint32 ISOTP_STmin;
    uint32 ISOTP_BS;
    CanTp_TransferSubStateType substate;
    CanTp_TransferStateType state;
    uint32 transferTotal;
    uint32 transferCount;
    PduLengthType availableDataSize;
    PduInfoType   upperTransData;       // The PDUR make an instance of this.
    CanTp_CanIfPduInfoType CanIfTransData;
    PduLengthType Buffersize;
    uint8 addressformat;
    uint8 MaxSFPayload;   //byte;
    uint8 MaxFFPayload;    //byte;
    uint8 MaxCFPayload;    //byte;
    uint8 ISOTP_NPCI_Offset;  
    uint8* CanIfbuffer;
    uint8* upperLayerbuffer;
}CanTp_Change_InfoType;

typedef struct
{
    uint8 CanTpBs;
    double CanTpNar;
    double CanTpNbr;
    double CanTpNcr;
    uint8 CanTpRxAddressingFormat;
    uint16 CanTpRxNSduId;
    uint8 CanTpRxPaddingActivation;
    uint8 CanTpRxTaType;
    uint16 CanTpRxWftMax; 
    uint8 CanTpSTmin;
    uint16 CanTpRxNPduId;
    uint16 CanTpTxFcNPduConfirmationPduId;
}CanTpRxNSduType;

typedef struct
{
    double CanTpNas; //Value in second of the N_As timeout.N_As is the time for transmission of a CAN frame(any N_PDU) on the part of the sender.
    double CanTpNbs; //Value in seconds of the N_Bs timeout.N_Bs is the time of transmission until reception of the next Flow Control N_PDU.
    double CanTpNcs; //Value in seconds of the performance requirement of (N_Cs + N_As). N_Cs is the time in which CanTp is allowed to request the Tx data of a Consecutive Frame N_PDU.
    BoolType CanTpTc;
    uint8 CanTpTxAddressingFormat;
    uint16 CanTpTxNSduId;
    uint8 CanTpTxPaddingActivation;
    uint8 CanTpTxTaType;
    uint16 CanTpTxNPduConfirmationPduId;
    uint16 CanTpRxFcNPduId;
}CanTpTxNSduType;


extern CanTp_Change_InfoType CanTp_Change_RxData[1];
extern CanTp_Change_InfoType CanTp_Change_TxData[1];
extern const CanTpRxNSduType CanTpRxNSdu[NUMBER_OF_RXNPDU];
extern const CanTpTxNSduType CanTpTxNSdu[NUMBER_OF_TXNPDU];