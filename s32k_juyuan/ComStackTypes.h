#pragma once
/********************************************************************************************************************************************
                                 *Type definition
********************************************************************************************************************************************/
typedef unsigned short PduLengthType;
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



typedef enum
{
    BUFREQ_OK, /*Buffer request accomplished successful. This 
                 status shall have the value 0.*/
    BUFREQ_E_NOT_OK, /*Buffer request not successful. Buffer cannot
                       be accessed. This status shall have the value 1*/
    BUFREQ_E_BUSY, /*Temporarily no buffer available. It¡¯s up the
                     requester to retry request for a certain time.
                     This status shall have the value 2.*/
    BUFREQ_E_OVFL /*No Buffer of the required length can be provided. 
                  This status shall have the value 3.*/
}BufReq_ReturnType;
