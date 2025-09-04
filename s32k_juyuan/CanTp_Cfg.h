#pragma once
#include "common.h"
#include "CanTp_Types.h"
#define    NUMBER_OF_TXNPDU                                                (1u)
#define    NUMBER_OF_RXNPDU                                                (2u)



typedef struct
{
    unsigned char* SduDataPtr;
    PduLengthType SduLength;
}CanTp_CanIfPduInfoType;


typedef struct
{
	unsigned int nextFlowCounter;             
	unsigned int framehandledCounter;
	unsigned int stateTimeOutCounter;
    unsigned int ISOTP_STmin;
    unsigned int ISOTP_BS;
    
   TransferStateTypes state;
    unsigned int transferTotal;
    unsigned int transferCount;
    PduLengthType availableDataSize;
    PduInfoType   upperTransData;       // The PDUR make an instance of this.
    CanTp_CanIfPduInfoType CanIfTransData;
    PduLengthType Buffersize;
    unsigned char addressformat;
    unsigned char MaxSFPayload;   //byte;
    unsigned char MaxFFPayload;    //byte;
    unsigned char MaxCFPayload;    //byte;
    unsigned char ISOTP_NPCI_Offset;  
    unsigned char* CanIfbuffer;
    unsigned char* upperLayerbuffer;
}CanTp_Change_InfoType;

typedef struct
{
    unsigned char CanTpBs;
    double CanTpNar;
    double CanTpNbr;
    double CanTpNcr;
    unsigned char CanTpRxAddressingFormat;
    unsigned short CanTpRxNSduId;
    unsigned char CanTpRxPaddingActivation;
    unsigned char CanTpRxTaType;
    unsigned short CanTpRxWftMax; 
    unsigned char CanTpSTmin;
    unsigned short CanTpRxNPduId;
    unsigned short CanTpTxFcNPduConfirmationPduId;
}CanTpRxNSduType;

typedef struct
{
    double CanTpNas; //Value in second of the N_As timeout.N_As is the time for transmission of a CAN frame(any N_PDU) on the part of the sender.
    double CanTpNbs; //Value in seconds of the N_Bs timeout.N_Bs is the time of transmission until reception of the next Flow Control N_PDU.
    double CanTpNcs; //Value in seconds of the performance requirement of (N_Cs + N_As). N_Cs is the time in which CanTp is allowed to request the Tx data of a Consecutive Frame N_PDU.
    BoolType CanTpTc;
    unsigned char CanTpTxAddressingFormat;
    unsigned short CanTp1TxNSduId;
    unsigned char CanTpTxPaddingActivation;
    unsigned char CanTpTxTaType;
    unsigned short CanTpTxNPduConfirmationPduId;
    unsigned short CanTpRxFcNPduId;
}CanTpTxNSduType;
















extern CanTp_Change_InfoType CanTp_Change_RxData[2];
extern CanTp_Change_InfoType CanTp_Change_TxData[1];
extern const CanTpRxNSduType CanTpRxNSdu[NUMBER_OF_RXNPDU];
extern const CanTpTxNSduType CanTpTxNSdu[NUMBER_OF_TXNPDU];