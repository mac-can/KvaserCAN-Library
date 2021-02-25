/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
 *
 *  Copyright (C) 2020-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *
 *  This file is part of MacCAN-KvaserCAN.
 *
 *  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MacCAN-KvaserCAN.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef KVASERUSB_COMMON_H_INCLUDED
#define KVASERUSB_COMMON_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#define KVASER_NAME  "Kvaser AB, Sweden"
#define KVASER_WEBSITE  "https://www.kvaser.com"

#define KVASER_VENDOR_ID  0xBFDU
#define KVASER_LEAF_PRO_ID  0x107U
#define KVASER_LEAF_LIGHT_ID  0x120U

#define KVASER_MAX_CAN_CHANNELS  1U
#define KVASER_MAX_NAME_LENGTH  32U

#define KVASER_MIN_COMMAND_LENGTH  4U
#define KVASER_MAX_COMMAND_LENGTH  32U
#define KVASER_USB_COMMAND_TIMEOUT 800U
#define KVASER_USB_REQUEST_DELAY   100U

#define KVASER_HYDRA_COMMAND_LENGTH  32U
#define KVASER_HYDRA_EXT_COMMAND_LENGTH  96U
#define KVASER_HYDRA_MIN_EXT_CMD_LENGTH  KVASER_HYDRA_COMMAND_LENGTH
#define KVASER_HYDRA_MAX_EXT_CMD_LENGTH  KVASER_HYDRA_EXT_COMMAND_LENGTH
#define KVASER_HYDRA_USB_COMMAND_TIMEOUT 5000U

#define KVASER_RECEIVE_QUEUE_SIZE  65536U


/* ---  general CAN data types and defines  ---
 */
#if (OPTION_CANAPI_DRIVER != 0)
    #include "CANAPI_Types.h"
    /* typedefs from CAN API V3 */
    typedef can_message_t KvaserUSB_CanMessage_t;
    typedef can_timestamp_t KvaserUSB_Timestamp_t;
#else
    /* typedefs from <3rdParty.h> */
    // TODO: insert coin here
#endif

/* ---  general CAN command codes  ---
 */
#define CMD_RX_STD_MESSAGE                  0x0CU
#define CMD_TX_STD_MESSAGE                  0x0DU
#define CMD_RX_EXT_MESSAGE                  0x0EU
#define CMD_TX_EXT_MESSAGE                  0x0FU
#define CMD_SET_BUSPARAMS_REQ               0x10U
#define CMD_GET_BUSPARAMS_REQ               0x11U
#define CMD_GET_BUSPARAMS_RESP              0x12U
#define CMD_GET_CHIP_STATE_REQ              0x13U
#define CMD_CHIP_STATE_EVENT                0x14U
#define CMD_SET_DRIVERMODE_REQ              0x15U
#define CMD_GET_DRIVERMODE_REQ              0x16U
#define CMD_GET_DRIVERMODE_RESP             0x17U
#define CMD_RESET_CHIP_REQ                  0x18U
#define CMD_RESET_CARD_REQ                  0x19U
#define CMD_START_CHIP_REQ                  0x1AU
#define CMD_START_CHIP_RESP                 0x1BU
#define CMD_STOP_CHIP_REQ                   0x1CU
#define CMD_STOP_CHIP_RESP                  0x1DU
#define CMD_READ_CLOCK_REQ                  0x1EU
#define CMD_READ_CLOCK_RESP                 0x1FU
#define CMD_GET_CARD_INFO_2                 0x20U

#define CMD_GET_CARD_INFO_REQ               0x22U
#define CMD_GET_CARD_INFO_RESP              0x23U
#define CMD_GET_INTERFACE_INFO_REQ          0x24U
#define CMD_GET_INTERFACE_INFO_RESP         0x25U
#define CMD_GET_SOFTWARE_INFO_REQ           0x26U
#define CMD_GET_SOFTWARE_INFO_RESP          0x27U
#define CMD_GET_BUSLOAD_REQ                 0x28U
#define CMD_GET_BUSLOAD_RESP                0x29U
#define CMD_RESET_STATISTICS                0x2AU
#define CMD_CHECK_LICENSE_REQ               0x2BU
#define CMD_CHECK_LICENSE_RESP              0x2CU
#define CMD_ERROR_EVENT                     0x2DU

#define CMD_FLUSH_QUEUE                     0x30U
#define CMD_RESET_ERROR_COUNTER             0x31U
#define CMD_TX_ACKNOWLEDGE                  0x32U
#define CMD_CAN_ERROR_EVENT                 0x33U

#define CMD_MEMO_GET_DATA                   0x34U
#define CMD_MEMO_PUT_DATA                   0x35U
#define CMD_MEMO_PUT_DATA_START             0x36U
#define CMD_MEMO_ASYNCOP_START              0x37U
#define CMD_MEMO_ASYNCOP_GET_DATA           0x38U
#define CMD_MEMO_ASYNCOP_CANCEL             0x39U
#define CMD_MEMO_ASYNCOP_FINISHED           0x3AU
#define CMD_DISK_FULL_INFO                  0x3BU
#define CMD_TX_REQUEST                      0x3CU
#define CMD_SET_HEARTBEAT_RATE_REQ          0x3DU
#define CMD_HEARTBEAT_RESP                  0x3EU
#define CMD_SET_AUTO_TX_BUFFER              0x3FU
#define CMD_GET_EXTENDED_INFO               0x40U
#define CMD_TCP_KEEPALIVE                   0x41U
#define CMD_TX_INTERVAL_REQ                 0x42U
#define CMD_TX_INTERVAL_RESP                0x43U
#define CMD_FILO_FLUSH_QUEUE_RESP           0x44U

#define CMD_AUTO_TX_BUFFER_REQ              0x48U
#define CMD_AUTO_TX_BUFFER_RESP             0x49U
#define CMD_SET_TRANSCEIVER_MODE_REQ        0x4AU
#define CMD_TREF_SOFNR                      0x4BU
#define CMD_SOFTSYNC_ONOFF                  0x4CU
#define CMD_USB_THROTTLE                    0x4DU
#define CMD_SOUND                           0x4EU
#define CMD_LOG_TRIG_STARTUP                0x4FU
#define CMD_SELF_TEST_REQ                   0x50U
#define CMD_SELF_TEST_RESP                  0x51U
#define CMD_USB_THROTTLE_SCALED             0x52U

#define CMD_SET_IO_PORTS_REQ                0x56U
#define CMD_GET_IO_PORTS_REQ                0x57U
#define CMD_GET_IO_PORTS_RESP               0x58U

#define CMD_GET_CAPABILITIES_REQ            0x5FU
#define CMD_GET_CAPABILITIES_RESP           0x60U
#define CMD_GET_TRANSCEIVER_INFO_REQ        0x61U
#define CMD_GET_TRANSCEIVER_INFO_RESP       0x62U
#define CMD_MEMO_CONFIG_MODE                0x63U

#define CMD_LED_ACTION_REQ                  0x65U
#define CMD_LED_ACTION_RESP                 0x66U
#define CMD_INTERNAL_DUMMY                  0x67U
#define CMD_READ_USER_PARAMETER             0x68U
#define CMD_MEMO_CPLD_PRG                   0x69U
#define CMD_LOG_MESSAGE                     0x6AU
#define CMD_LOG_TRIG                        0x6BU
#define CMD_LOG_RTC_TIME                    0x6CU

/* ---  additional Hydra command codes  ---
 */
#define CMD_TX_CAN_MESSAGE                  0x21U
#define CMD_FLUSH_QUEUE_RESP                0x42U
#define CMD_SET_BUSPARAMS_FD_REQ            0x45U
#define CMD_SET_BUSPARAMS_FD_RESP           0x46U
#define CMD_SET_BUSPARAMS_RESP              0x55U
#define CMD_MAP_CHANNEL_REQ                 0xC8U
#define CMD_MAP_CHANNEL_RESP                0xC9U
#define CMD_GET_SOFTWARE_DETAILS_REQ        0xCAU
#define CMD_GET_SOFTWARE_DETAILS_RESP       0xCBU
#define CMD_TX_CAN_MESSAGE_FD               0xE0U
#define CMD_TX_ACKNOWLEDGE_FD               0xE1U
#define CMD_RX_MESSAGE_FD                   0xE2U
#define CMD_AUTOTX_MESSAGE_FD               0xE3U
#define CMD_EXTENDED                        0xFFU

/* ---  additional Hydra extended command codes  ---
 */
#define CMD_EXT_TX_MSG_FD   CMD_TX_CAN_MESSAGE_FD
#define CMD_EXT_TX_ACK_FD   CMD_TX_ACKNOWLEDGE_FD
#define CMD_EXT_RX_MSG_FD       CMD_RX_MESSAGE_FD
#define CMD_EXT_AUTOTX_FD   CMD_AUTOTX_MESSAGE_FD


/* ---  CAN message flags  ---
 */
#define MSGFLAG_ERROR_FRAME                 0x01U
#define MSGFLAG_OVERRUN                     0x02U
#define MSGFLAG_NERR                        0x04U
#define MSGFLAG_WAKEUP                      0x08U
#define MSGFLAG_REMOTE_FRAME                0x10U
#define MSGFLAG_EXTENDED_ID                 0x20U
#define MSGFLAG_TX                          0x40U
#define MSGFLAG_TXRQ                        0x80U

#define MSGFLAG_SSM_NACK                0x001000U
#define MSGFLAG_ABL                     0x002000U
#define MSGFLAG_FDF                     0x010000U
#define MSGFLAG_BRS                     0x020000U
#define MSGFLAG_ESI                     0x040000U

#define MSGFLAG_EXT           MSGFLAG_EXTENDED_ID
#define MSGFLAG_RTR          MSGFLAG_REMOTE_FRAME
#define MSGFLAG_STS           MSGFLAG_ERROR_FRAME

/* ---  Chip status flags  ---
 */
#define BUSSTAT_BUSOFF                      0x01U
#define BUSSTAT_ERROR_PASSIVE               0x02U
#define BUSSTAT_ERROR_WARNING               0x04U
#define BUSSTAT_ERROR_ACTIVE                0x08U
#define BUSSTAT_BUSOFF_RECOVERY             0x10U
#define BUSSTAT_IGNORING_ERRORS             0x20U

#define BUSSTAT_FLAG_ERR_ACTIVE             0x00U
#define BUSSTAT_FLAG_RESET                  0x01U
#define BUSSTAT_FLAG_BUS_ERROR              0x10U
#define BUSSTAT_FLAG_ERR_PASSIVE            0x20U
#define BUSSTAT_FLAG_BUSOFF                 0x40U

/* ---  Driver modes  ---
 */
#define DRIVERMODE_NORMAL                   0x01U
#define DRIVERMODE_SILENT                   0x02U
#define DRIVERMODE_SELFRECEPTION            0x03U
#define DRIVERMODE_OFF                      0x04U

/* ---  Transceiver (logical) types  ---
 */
#define FILO_TRANSCEIVER_TYPE_UNKNOWN          0U
#define FILO_TRANSCEIVER_TYPE_251              1U
#define FILO_TRANSCEIVER_TYPE_252              2U
#define FILO_TRANSCEIVER_TYPE_SWC              6U
#define FILO_TRANSCEIVER_TYPE_1054_OPTO       11U
#define FILO_TRANSCEIVER_TYPE_SWC_OPTO        12U
#define FILO_TRANSCEIVER_TYPE_1050            14U
#define FILO_TRANSCEIVER_TYPE_1050_OPTO       15U
#define FILO_TRANSCEIVER_TYPE_LIN             19U
#define FILO_TRANSCEIVER_TYPE_KLINE           10U

/* ---  Transceiver line modes  ---
 */
#define FILO_TRANSCEIVER_LINEMODE_NA           0U
#define FILO_TRANSCEIVER_LINEMODE_SWC_SLEEP    4U
#define FILO_TRANSCEIVER_LINEMODE_SWC_NORMAL   5U
#define FILO_TRANSCEIVER_LINEMODE_SWC_FAST     6U
#define FILO_TRANSCEIVER_LINEMODE_SWC_WAKEUP   7U
#define FILO_TRANSCEIVER_LINEMODE_SLEEP        8U
#define FILO_TRANSCEIVER_LINEMODE_NORMAL       9U
#define FILO_TRANSCEIVER_LINEMODE_STDBY       10U
/* ---  Transceiver resnet modes  --- */
#define FILO_TRANSCEIVER_RESNET_NA             0U

/* ---  Transceiver (logical) types  ---
 */
#define HYDRA_TRANSCEIVER_TYPE_UNKNOWN         0U
#define HYDRA_TRANSCEIVER_TYPE_251             1U
#define HYDRA_TRANSCEIVER_TYPE_252             2U
#define HYDRA_TRANSCEIVER_TYPE_SWC             6U
#define HYDRA_TRANSCEIVER_TYPE_1054_OPTO      11U
#define HYDRA_TRANSCEIVER_TYPE_SWC_OPTO       12U
#define HYDRA_TRANSCEIVER_TYPE_1050           14U
#define HYDRA_TRANSCEIVER_TYPE_1050_OPTO      15U
#define HYDRA_TRANSCEIVER_TYPE_LIN            19U

/* ---  Transceiver line modes  ---
 */
#define HYDRA_TRANSCEIVER_LINEMODE_NA          0U
#define HYDRA_TRANSCEIVER_LINEMODE_SWC_SLEEP   4U
#define HYDRA_TRANSCEIVER_LINEMODE_SWC_NORMAL  5U
#define HYDRA_TRANSCEIVER_LINEMODE_SWC_FAST    6U
#define HYDRA_TRANSCEIVER_LINEMODE_SWC_WAKEUP  7U
#define HYDRA_TRANSCEIVER_LINEMODE_SLEEP       8U
#define HYDRA_TRANSCEIVER_LINEMODE_NORMAL      9U
#define HYDRA_TRANSCEIVER_LINEMODE_STDBY      10U
/* ---  Transceiver resnet modes (not supported)  --- */
#define HYDRA_TRANSCEIVER_RESNET_NA            0U

/* ---  Error codes  ---
 */
#define FIRMWARE_ERR_OK                        0U
#define FIRMWARE_ERR_CAN                       1U
#define FIRMWARE_ERR_NVRAM_ERROR               2U
#define FIRMWARE_ERR_NOPRIV                    3U
#define FIRMWARE_ERR_ILLEGAL_ADDRESS           4U
#define FIRMWARE_ERR_UNKNOWN_CMD               5U
#define FIRMWARE_ERR_FATAL                     6U
#define FIRMWARE_ERR_CHECKSUM_ERROR            7U
#define FIRMWARE_ERR_QUEUE_LEVEL               8U
#define FIRMWARE_ERR_PARAMETER                 9U
/* Error codes for EXTINFO status */
#define LEAF_EXTINFO_STAT_OK                   0U
#define LEAF_EXTINFO_STAT_FAILURE              1U
#define LEAF_EXTINFO_NOT_IMPLEMENTED          15U

/* --- Command options
 */
/*  for CMD_READ_CLOCK_REQ: */
#define READ_CLOCK_NOW                      0x01U

/*  for CMD_GET_SOFTWARE_OPTIONS: */
#define SWOPTION_CONFIG_MODE                0x01L
#define SWOPTION_AUTO_TX_BUFFER             0x02L
#define SWOPTION_BETA                       0x04L
#define SWOPTION_RC                         0x08L
#define SWOPTION_BAD_MOOD                   0x10L
#define SWOPTION_CPU_FQ_MASK                0x60L
#define SWOPTION_16_MHZ_CLK                 0x00L
#define SWOPTION_32_MHZ_CLK                 0x20L
#define SWOPTION_24_MHZ_CLK                 0x40L
#define SWOPTION_XX_MHZ_CLK                 0x60L
#define SWOPTION_TIMEOFFSET_VALID           0x80L
#define SWOPTION_CAP_REQ                  0x1000L
// Hydra devices:
#define SWOPTION_80_MHZ_CLK                 0x20L
#define SWOPTION_DELAY_MSGS                0x100L
#define SWOPTION_USE_HYDRA_EXT             0x200L
#define SWOPTION_CANFD_CAP                 0x400L
#define SWOPTION_NONISO_CAP                0x800L

/*  for CMD_SET_AUTO_TX_REQ and _RESP: */
#define AUTOTXBUFFER_CMD_GET_INFO              1U
#define AUTOTXBUFFER_CMD_CLEAR_ALL             2U
#define AUTOTXBUFFER_CMD_ACTIVATE              3U
#define AUTOTXBUFFER_CMD_DEACTIVATE            4U
#define AUTOTXBUFFER_CMD_SET_INTERVAL          5U
#define AUTOTXBUFFER_CMD_GENERATE_BURST        6U
#define AUTOTXBUFFER_CMD_SET_MSG_COUNT         7U
// CMD_SET_AUTO_TX_RESP bit values for automatic tx buffer capabilities
#define AUTOTXBUFFER_CAP_TIMED_TX           0x01U
#define AUTOTXBUFFER_CAP_AUTO_RESP_DATA     0x02U
#define AUTOTXBUFFER_CAP_AUTO_RESP_RTR      0x04U
// Use these message flags with cmdSetAutoTxBuffer.flags
#define AUTOTXBUFFER_MSG_REMOTE_FRAME       0x10U
#define AUTOTXBUFFER_MSG_EXT                0x80U

/*  for CMD_SOFTSYNC_ONOFF: */
#define SOFTSYNC_OFF                           0U
#define SOFTSYNC_ON                            1U
#define SOFTSYNC_NOT_STARTED                   2U

/*  for CMD_GET_CARD_INFO_RESP: */
#define CAN_TIME_STAMP_REF_ACK                 0U
#define CAN_TIME_STAMP_REF_SOF                 1U
#define CAN_TIME_STAMP_REF_INTERRUPT           2U
#define CAN_TIME_STAMP_REF_CANTIMER            3U


/* ---  little endian (x86_64 architecture)  ---
 */
#define BUF2UINT8(buf)   (uint8_t)(buf)
#define BUF2UINT16(buf)  (uint16_t)*((uint16_t*)&(buf))
#define BUF2UINT32(buf)  (uint32_t)*((uint32_t*)&(buf))
#define BUF2UINT64(buf)  (uint64_t)*((uint64_t*)&(buf))

#define UINT8BYTE(var)   (uint8_t)(var)
#define UINT16LSB(var)   (uint8_t)*((uint8_t*)&(var))
#define UINT16MSB(var)   (uint8_t)*(((uint8_t*)&(var))+1)
#define UINT32LOLO(var)  (uint8_t)*((uint8_t*)&(var))
#define UINT32LOHI(var)  (uint8_t)*(((uint8_t*)&(var))+1)
#define UINT32HILO(var)  (uint8_t)*(((uint8_t*)&(var))+2)
#define UINT32HIHI(var)  (uint8_t)*(((uint8_t*)&(var))+3)

#endif /* KVASERUSB_COMMON_H_INCLUDED */
