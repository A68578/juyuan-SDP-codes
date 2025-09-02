#pragma once

/******************************************************************************************************************************************
                                 *Macro
*******************************************************************************************************************************************/
/* [SWS_CanTp_00027] The CanTp module shall have two internal states, CANTP_OFF and CANTP_ON. */
#define    CANTP_OFF                                                        (0u)
#define    CANTP_ON                                                         (1u)


typedef unsigned int CanTp_TransferMode;
#define    CANTP_RX_WAIT                                                   ((CanTp_TransferMode)0x00u)
#define    CANTP_RX_PROCESSING                                             ((CanTp_TransferMode)0x01u)
#define    CANTP_TX_WAIT                                                   ((CanTp_TransferMode)0x02u)
#define    CANTP_TX_PROCESSING                                             ((CanTp_TransferMode)0x03u)

/* implement of 15765-2 */
typedef unsigned char ISOTPTransType;
#define    ISOTP_NPCI_MASK                                              ((ISOTPTransType)0xF0u)
#define    ISOTP_NPCI_SF                                                ((ISOTPTransType)0x00u)  /* Single Frame */
#define    ISOTP_NPCI_FF                                                ((ISOTPTransType)0x10u)  /* First Frame */
#define    ISOTP_NPCI_CF                                                ((ISOTPTransType)0x20u)  /* Consecutive Frame */
#define    ISOTP_NPCI_FC                                                ((ISOTPTransType)0x30u)  /* Flow Control */

#define    ISOTP_FC_FS_MASK                                             ((ISOTPTransType)0x0Fu)  /* FlowControl status mask */
#define    ISOTP_FLOW_CONTROL_STATUS_CTS                                ((ISOTPTransType)0x00u)  /* FC Clear/Continue To Send Status */
#define    ISOTP_FLOW_CONTROL_STATUS_WAIT                               ((ISOTPTransType)0x01u)  /* FC Waiting Status */
#define    ISOTP_FLOW_CONTROL_STATUS_OVFLW                              ((ISOTPTransType)0x02u)  /* FC OverFlows Status */

typedef unsigned char TransferStateTypes;
#define    UNINITIALIZED                                                 ((TransferStateTypes)0x00u)
#define    CANTPIDLE                                                     ((TransferStateTypes)0x01u)
#define    SF_OR_FF_RECEIVED_WAITING_PDUR_BUFFER                         ((TransferStateTypes)0x02u)
#define    RX_WAIT_CONSECUTIVE_FRAME                                     ((TransferStateTypes)0x03u)
#define    RX_WAIT_SDU_BUFFER                                            ((TransferStateTypes)0x04u)
#define    RX_WAIT_TX_CONFIRMATION                                       ((TransferStateTypes)0x05u)
#define    TX_WAIT_STMIN                                                 ((TransferStateTypes)0x06u)
#define    TX_WAIT_TRANSMIT                                              ((TransferStateTypes)0x07u)
#define    TX_WAIT_FLOW_CONTROL                                          ((TransferStateTypes)0x08u)
#define    TX_WAIT_TX_CONFIRMATION                                       ((TransferStateTypes)0x09u)

/********************************************************************************************************************************************
                                 *Type definition
********************************************************************************************************************************************/
typedef unsigned int PduLengthType;
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
























