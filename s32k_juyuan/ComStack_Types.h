#pragma once
#include "common.h"
/********************************************************************************************************************************************
                                 *Type definition
********************************************************************************************************************************************/
typedef uint16 PduLengthType;
typedef uint16 PduIdType;
typedef struct
{
    uint8* SduDataPtr; /*Pointer to the SDU (i.e. payload data) of the PDU. The type of this
                        pointer depends on the memory model being used at compile time*/
    uint8* MetaDataPtr; /*Pointer to the meta data (e.g. CAN ID, socket ID, diagnostic addresses)
                          of the PDU, consisting of a sequence of meta data items. The length
                          and type of the meta data items is statically configured for each PDU.
                          Meta data items with more than 8 bits use platform byte order.*/
    PduLengthType SduLength;

}PduInfoType;



typedef enum
{
    BUFREQ_OK, /*Buffer request accomplished successful. This 
                 status shall have the value 0.*/
    BUFREQ_E_NOT_OK, /*Buffer request not successful. Buffer cannot
                       be accessed. This status shall have the value 1*/
    BUFREQ_E_BUSY, /*Temporarily no buffer available. It¡¯s up the
                     requester to Req_Resultry request for a certain time.
                     This status shall have the value 2.*/
    BUFREQ_E_OVFL /*No Buffer of the required length can be provided. 
                  This status shall have the value 3.*/
}BufReq_ReturnType;




typedef enum
{
    TP_DATACONF, /*TP_DATACONF indicates that all data, that
                   have been copied so far, are confirmed and
                   can be removed from the TP buffer. Data
                   copied by this API call are excluded and will
                   be confirmed later.*/
    TP_DATARETRY, /*TP_DATARETRY indicates that this API call
                    shall copyalready copied data in order to
                    recover from an error. In this case TxTpData
                    Cnt specifies the offset of the first byte to be
                    copied by the API call*/
    TP_CONFPENDING /*TP_CONFPENDING indicates that the
                     previously copied data must remain in the TP*/
}TpDataStateType;


typedef struct
{
    TpDataStateType TpDataState; /*The enum type to be used to store the state of Tp buffer.*/
    PduLengthType TxTpDataCnt; /*Offset from the current position which identifies the number of bytes to
                                 be Req_Resultransmitted*/
}RetryInfoType;