#pragma once
#include "ComStackTypes.h"
/******************************************************************************************************************************************
                                 *Macro
*******************************************************************************************************************************************/
/* [SWS_CanTp_00027] The CanTp module shall have two internal states, CANTP_OFF and CANTP_ON.(CanTp mode) */
#define    CANTP_OFF                                                        (0u)
#define    CANTP_ON                                                         (1u)


typedef    unsigned char                                                CanTp_TransferStateType;
#define    CANTP_RX_WAIT                                                ((CanTp_TransferStateType)0x00u)
#define    CANTP_RX_PROCESSING                                          ((CanTp_TransferStateType)0x01u)
#define    CANTP_TX_WAIT                                                ((CanTp_TransferStateType)0x02u)
#define    CANTP_TX_PROCESSING                                          ((CanTp_TransferStateType)0x03u)

/* implement of 15765-2 */
typedef unsigned char                                                   ISOTPTransType;
#define    ISOTP_NPCI_MASK                                              ((ISOTPTransType)0xF0u)
#define    ISOTP_NPCI_SF                                                ((ISOTPTransType)0x00u)  /* Single Frame */
#define    ISOTP_NPCI_FF                                                ((ISOTPTransType)0x10u)  /* First Frame */
#define    ISOTP_NPCI_CF                                                ((ISOTPTransType)0x20u)  /* Consecutive Frame */
#define    ISOTP_NPCI_FC                                                ((ISOTPTransType)0x30u)  /* Flow Control */

#define    ISOTP_FC_FS_MASK                                             ((ISOTPTransType)0x0Fu)  /* FlowControl status mask */
#define    ISOTP_FLOW_CONTROL_STATUS_CTS                                ((ISOTPTransType)0x00u)  /* FC Clear/Continue To Send Status */
#define    ISOTP_FLOW_CONTROL_STATUS_WAIT                               ((ISOTPTransType)0x01u)  /* FC Waiting Status */
#define    ISOTP_FLOW_CONTROL_STATUS_OVFLW                              ((ISOTPTransType)0x02u)  /* FC OverFlows Status */


#define    ISOTP_MAX_FRAME_CAN_BYTES                                    ((unsigned char)0x08u)
#define    ISOTP_MAX_FRAME_CANFD_BYTES                                  ((unsigned char)CANIF_TX_DL) //0x0C 0x10 0x14 0x18 0x20 0x30 0x40

#define    ISOTP_MAX_PAYLOAD_CAN_SF                                     (ISOTP_MAX_FRAME_CAN_BYTES - 1u)
#define    ISOTP_MAX_PAYLOAD_CANFD_SF                                   (ISOTP_MAX_FRAME_CANFD_BYTES - 2u)
#define    ISOTP_MAX_PAYLOAD_CAN_FF                                     (ISOTP_MAX_FRAME_CAN_BYTES - 2u)
#define    ISOTP_MAX_PAYLOAD_CANFD_FF                                   (ISOTP_MAX_FRAME_CANFD_BYTES - 6u)

#define    ISOTP_SF_DL_MASK                                             ((unsigned char)0x0Fu)  /* Single frame data length mask */

#define    ISOTP_FF_DL_MASK                                             ((unsigned char)0x0Fu)  /* First frame First 4 bits data length mask */


typedef unsigned char                                                    CanTp_TransferSubStateType;
#define    UNINITIALIZED                                                 ((CanTp_TransferSubStateType)0x00u)
#define    CANTPIDLE                                                     ((CanTp_TransferSubStateType)0x01u)
#define    SF_OR_FF_RECEIVED_WAITING_UPPERLAYER_BUFFER                   ((CanTp_TransferSubStateType)0x02u)
#define    RX_WAIT_CONSECUTIVE_FRAME                                     ((CanTp_TransferSubStateType)0x03u)
#define    RX_WAIT_SDU_BUFFER                                            ((CanTp_TransferSubStateType)0x04u)
#define    RX_WAIT_TX_CONFIRMATION                                       ((CanTp_TransferSubStateType)0x05u)
#define    TX_WAIT_STMIN                                                 ((CanTp_TransferSubStateType)0x06u)
#define    TX_WAIT_TRANSMIT                                              ((CanTp_TransferSubStateType)0x07u)
#define    TX_WAIT_FLOW_CONTROL                                          ((CanTp_TransferSubStateType)0x08u)
#define    TX_WAIT_TX_CONFIRMATION                                       ((CanTp_TransferSubStateType)0x09u)

typedef    unsigned char                                                 AddressingFormatType;
#define    CANTP_EXTENDED                                                ((AddressingFormatType)0x00u)
#define    CANTP_MIXED                                                   ((AddressingFormatType)0x01u)
#define    CANTP_MIXED29BIT                                              ((AddressingFormatType)0x02u)
#define    CANTP_NORMALFIXED                                             ((AddressingFormatType)0x03u)
#define    CANTP_STANDARD                                                ((AddressingFormatType)0x04u)

typedef    unsigned char                                                  TaType;
#define    CANTP_FUNCTIONAL                                               ((TaType)0x00u)
#define    CANTP_PHYSICAL                                                 ((TaType)0x01u)

typedef    unsigned char                                                  FrameType;
#define    INVALID_FRAME                                                  ((FrameType)0x00u)
#define    SINGLE_FRAME                                                   ((FrameType)0x01u)
#define    FIRST_FRAME                                                    ((FrameType)0x02u)
#define    CONSECUTIVE_FRAME                                              ((FrameType)0x03u)
#define    FLOW_CONTROL_CTS_FRAME                                         ((FrameType)0x04u)
#define    FLOW_CONTROL_WAIT_FRAME                                        ((FrameType)0x05u)
#define    FLOW_CONTROL_OVERFLOW_FRAME                                    ((FrameType)0x06u)











#define TIMER_DECREMENT(timer) \
        if (timer > 0) { \
            timer = timer - 1; \
        } \

#define COUNT_DECREMENT(count) TIMER_DECREMENT(count)























