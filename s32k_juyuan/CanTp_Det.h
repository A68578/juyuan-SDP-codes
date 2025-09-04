#pragma once
#include "Det.h"
#define    CANTP_MODULE_ID                                                    (35u) /* CanTp Module Id 35*/ 
#define    CANTP_INSTANCE_ID                                                  (0x00u)  /* CanTp Instance Id*/
/* Service ID for API's */
#define    CANTP_INIT_SERVICE_ID                                              (0x01u)
#define    CANTP_TRANSMIT_SERVICE_ID                                          (0x49u)
#define    CANTP_MAINFUNCTION_SERVICE_ID                                      (0x06u)
#define    CANTP_RXINDICATION_SERVICE_ID                                      (0x42u)
#define    CANTP_TXCONFIRMATION_SERVICE_ID                                    (0x40u)


//Development Errors
#define    CANTP_E_PARAM_CONFIG                                               (0x01u)
#define    CANTP_E_PARAM_ID                                                   (0x02u)
#define    CANTP_E_PARAM_POINTER                                              (0x03u)
#define    CANTP_E_INIT_FAILED                                                (0x04u)
#define    CANTP_E_UNINIT                                                     (0x20u)
#define    CANTP_E_INVALID_TX_ID                                              (0x30u)
#define    CANTP_E_INVALID_RX_ID                                              (0x40u)