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
	unsigned int frameCounter;
	unsigned int stateCounter;
    unsigned int STmin;
    unsigned int BS;
    
   TransferStateTypes state;
    unsigned int transferTotal;
    unsigned int transferCount;
    PduLengthType availableDataSize;
    PduInfoType   pdurTransData;       // The PDUR make an instance of this.
    CanTp_CanIfPduInfoType CanIfTransData;
    CanTp_TransferMode CanTpTransmode;
    PduLengthType Buffersize;
    unsigned char addressformat;
    unsigned char MaxSFPayload;   //byte;
    unsigned char MaxFFPayload;    //byte;
    unsigned char MaxCFPayload;    //byte;
    unsigned char ISOTP_NPCI_Offset;  
    unsigned char* CanIfbuffer;
    unsigned char* PduRbuffer;
}CanTp_Change_InfoType;