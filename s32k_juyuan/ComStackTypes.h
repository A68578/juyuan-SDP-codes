#pragma once
/********************************************************************************************************************************************
                                 *Type definition
********************************************************************************************************************************************/
typedef unsigned int PduLengthType;
typedef unsigned short PduIdType;
typedef struct
{
    unsigned char* SduDataPtr; /*Pointer to the SDU (i.e. payload data) of the PDU. The type of this
                        pointer depends on the memory model being used at compile time*/
    unsigned char* MetaDataPtr; /*Pointer to the meta data (e.g. CAN ID, socket ID, diagnostic addresses)
                          of the PDU, consisting of a sequence of meta data items. The length
                          and type of the meta data items is statically configured for each PDU.
                          Meta data items with more than 8 bits use platform byte order.*/
    PduLengthType SduLength;

}PduInfoType;