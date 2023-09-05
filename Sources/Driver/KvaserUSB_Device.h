/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Interfaces
 *
 *  Copyright (c) 2020-2023 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *  All rights reserved.
 *
 *  This file is part of MacCAN-KvaserCAN.
 *
 *  MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v3.0 (or any later version). You can
 *  choose between one of them if you use MacCAN-KvaserCAN in whole or in part.
 *
 *  BSD 2-Clause Simplified License:
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  MacCAN-KvaserCAN IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF MacCAN-KvaserCAN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  GNU General Public License v3.0 or later:
 *  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MacCAN-KvaserCAN.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef KVASERUSB_DEVICE_H_INCLUDED
#define KVASERUSB_DEVICE_H_INCLUDED

#include "KvaserUSB_Common.h"

#include "MacCAN_IOUsbKit.h"
#include "MacCAN_MsgQueue.h"
#include "MacCAN_MsgPipe.h"

typedef enum kavser_driver_type_t_ {    /* driver type: */
    USB_LEAF_DRIVER,                    /* - driver for Leaf devices */
    USB_MHYDRA_DRIVER,                  /* - driver for Mhydra devices */
    USB_UNKNOWN_DRIVER                  /* - unknown/unsupported (last entry) */
} KvaserUSB_DriverType_t;

typedef struct kvaser_card_info_t_  {   /* card info: */
    uint8_t  channelCount;              /* - channel count */
    uint32_t serialNumber;              /* - serial no. */
    uint32_t clockResolution;           /* - clock resolution */
    uint32_t mfgDate;                   /* - manufacturing date */
    uint8_t  EAN[8];                    /* - EAN code (LSB first) */
    uint8_t  hwRevision;                /* - hardware revision */
    uint8_t  usbHsMode;                 /* - USB HS mode */
    uint8_t  hwType;                    /* - hardware type */
    uint8_t  canTimeStampRef;           /* - CAN time-stamp reference */
} KvaserUSB_CardInfo_t;

typedef struct kvaser_interface_info_t_ { /* channel info: */
    uint32_t channelCapabilities;       /* - channel capabilities */
    uint8_t  canChipType;               /* - CAN chip type */
    uint8_t  canChipSubType;            /* - CAN chip sub-type */
} KvaserUSB_InterfaceInfo_t;            // note: not implemented by any device

typedef struct kvaser_software_info_t_ {  /* software info/details: */
    uint32_t swOptions;                 /* - software options */
    uint32_t firmwareVersion;           /* - firmware version */
    uint16_t maxOutstandingTx;          /* - max. outstanding Tx */
    uint32_t swName;                    /* - software name (hydra only) */
    uint8_t EAN[8];                     /* - EAN code (LSB first, hydra only) */
    uint32_t maxBitrate;                /* - max. bit-rate (hydra only, otherwise 1Mbit/s)*/
} KvaserUSB_SoftwareInfo_t;

typedef struct kvaser_capabilities_t_ { /* channel capabilities: */
    uint16_t hasTimeQuanta : 1;         /* - CAP_SUB_CMD_HAS_BUSPARAMS_TQ (mhydra) */
    uint16_t hasIoApi : 1;              /* - CAP_SUB_CMD_HAS_IO_API (mhydra) */
    uint16_t hasKdi : 1;                /* - CAP_SUB_CMD_HAS_KDI (mhydra) */
    uint16_t kdiInfo : 1;               /* - CAP_SUB_CMD_KDI_INFO (mhydra) */
    uint16_t linHybrid : 1;             /* - CAP_SUB_CMD_LIN_HYBRID (mhydra) */
    uint16_t hasScript : 1;             /* - CAP_SUB_CMD_HAS_SCRIPT */
    uint16_t hasRemote : 1;             /* - CAP_SUB_CMD_HAS_REMOTE */
    uint16_t hasLogger : 1;             /* - CAP_SUB_CMD_HAS_LOGGER */
    uint16_t syncTxFlush : 1;           /* - CAP_SUB_CMD_SYNC_TX_FLUSH */
    uint16_t singleShot : 1;            /* - CAP_SUB_CMD_SINGLE_SHOT */
    uint16_t errorCount : 1;            /* - CAP_SUB_CMD_ERRCOUNT_READ */
    uint16_t busStats : 1;              /* - CAP_SUB_CMD_BUS_STATS */
    uint16_t errorGen : 1;              /* - CAP_SUB_CMD_ERRFRAME (error frame generation!) */
    uint16_t silentMode : 1;            /* - CAP_SUB_CMD_SILENT_MODE */
    uint16_t dummy : 2;
} KvaserUSB_Capabilities_t;

typedef struct kvaser_transceiver_info_t_ {  /* transceiver info: */
    uint32_t transceiverCapabilities;   /* - capabilities */
    uint8_t  transceiverStatus;         /* - status */
    uint8_t  transceiverType;           /* - type */
} KvaserUSB_TransceiverInfo_t;

typedef struct kvaser_device_info_t_ {  /* device info: */
    KvaserUSB_CardInfo_t card;          /* - card info */
    KvaserUSB_SoftwareInfo_t software;  /* - software info */
    KvaserUSB_Capabilities_t capabilities;   /* - channel capabilities */
    KvaserUSB_TransceiverInfo_t transceiver; /* - transceiver info */
} KvaserUSB_DeviceInfo_t;

typedef struct kvaser_bus_params_t_ {   /* bus parameter: */
    uint32_t bitRate;                   /* - bit rate (in [Hz]) */
    uint8_t  tseg1;                     /* - time segment 1 */
    uint8_t  tseg2;                     /* - time segment 2 */
    uint8_t  sjw;                       /* - synchronization jump width */
    uint8_t  noSamp;                    /* - number of sample points */
} KvaserUSB_BusParams_t;

typedef struct kvaser_bus_params_fd_t_ {/* bus parameter for CAN FD: */
    KvaserUSB_BusParams_t nominal;      /* - nominal bit-rate settings */
    KvaserUSB_BusParams_t data;         /* - data phase bit-rate settings */
    bool canFd;                         /* - flag: open as CAN FD */
} KvaserUSB_BusParamsFd_t;

typedef struct kvaser_bus_params_tq_t_ {/* bus parameter (time quanta): */
    struct tq_arbitration_t_ {          /* - arbitration phase: */
        uint16_t prop;                  /*   - TQ for propagation segment */
        uint16_t phase1;                /*   - TQ for phase 1 segment */
        uint16_t phase2;                /*   - TQ for phase 2 segment */
        uint16_t sjw;                   /*   - synchronization jump width */
        uint16_t brp;                   /*   - bit-rate prescaler */
    } arbitration;
    struct tq_data_phase_t_ {           /* - data phase (CAN FD): */
        uint16_t prop;                  /*   - TQ for propagation segment */
        uint16_t phase1;                /*   - TQ for phase 1 segment */
        uint16_t phase2;                /*   - TQ for phase 2 segment */
        uint16_t sjw;                   /*   - synchronization jump width */
        uint16_t brp;                   /*   - bit-rate prescaler */
    } data;
    bool canFd;                         /* - flag: open as CAN FD */
} KvaserUSB_BusParamsTq_t;

typedef uint8_t KvaserUSB_OpMode_t;     /* operation mode (CAN API V1 compatible) */

typedef uint8_t KvaserUSB_DriverMode_t; /* driver mode (OFF, NORMAL, SILENT) */

typedef uint8_t KvaserUSB_BusStatus_t;  /* bus status (Kvaser bus status)! */

typedef uint16_t KvaserUSB_BusLoad_t;   /* bus load (0..10000 = 0.00..100.00) */

typedef struct kvaser_chip_state_event_t_ {  /* event - chip state: */
    uint8_t  channel;                   /* - channel no. (from header) */
    uint16_t time[3];                   /* - 48-bit timer value */
    uint8_t  txErrorCounter;            /* - tx error counter */
    uint8_t  rxErrorCounter;            /* - rx error counter */
    uint8_t  busStatus;                 /* - bus status */
    uint8_t  _reserved1;                /* - (not used) */
    uint16_t _reserved2;                /* - (not used) */
} KvaserUSB_ChipStateEvent_t;

typedef struct kvaser_error_event_t_ {  /* event - ç: */
    uint8_t  errorCode;                 /* - error code (from header) */
    uint16_t time[3];                   /* - 48-bit timer value */
    uint16_t _reserved;                 /* - (not used) */
    uint16_t addInfo1;                  /* - additional information 1 */
    uint16_t addInfo2;                  /* - additional information 2 */
} KvaserUSB_ErrorEvent_t;

typedef struct kvaser_can_error_event_t_ {  /* event - CAN error event: */
    uint8_t  flags;                     /* - flags (from header) */
    uint16_t time[3];                   /* - 48-bit timer value */
    uint8_t  channel;                   /* - CAN channel */
    uint8_t  _reserved;                 /* - (not used) */
    uint8_t  txErrorCounter;            /* - tx error counter */
    uint8_t  rxErrorCounter;            /* - rx error counter */
    uint8_t  busStatus;                 /* - bus status */
    uint8_t  errorFactor;               /* - error factor */
} KvaserUSB_CanErrorEvent_t;

typedef struct kvaser_event_data_t_  {  /* event data: */
    KvaserUSB_ChipStateEvent_t chipState;/*- chip state */
    KvaserUSB_ErrorEvent_t errorEvent;  /* - error event */
    KvaserUSB_CanErrorEvent_t canError; /* - CAN error event */
} KvaserUSB_EventData_t;

typedef struct kvaser_endpoints_t_ {    /* USB endpoints: */
    uint8_t numEndpoints;               /* - number of endpoints */
    struct {                            /* - endpoints (pipes): */
        uint8_t pipeRef;                /*   - endpoint number */
        uint32_t packetSize;            /*   - endpoint size */
    } bulkIn, bulkOut;  // TODO: [KVASER_MAX_CAN_CHANNELS]
} KvaserUSB_Endpoints_t;

typedef uint8_t KvaserUSB_Frequency_t;  /* CAN clock / timer (in [MHz]) */

typedef uint64_t KvaserUSB_CpuTicks_t;  /* 48-bit timer value (ticks) */

typedef struct kavser_hydra_buffer_t_ { /* USB Hydra retention buffer (Leaf Pro): */
    uint32_t length;                    /* - number of bytes in the retention buffer */
    uint8_t buffer[KVASER_HYDRA_RETENTION_SIZE];
} KvaserUSB_HydraBuffer_t;

typedef struct kvaser_recv_context_t_ { /* USB read pipe context: */
    CANPIP_MsgPipe_t msgPipe;           /* - message pipe for data exchange */
    CANQUE_MsgQueue_t msgQueue;         /* - message queue for received CAN frames */
    KvaserUSB_OpMode_t opMode;          /* - demanded CAN operation mode */
    KvaserUSB_EventData_t evData;       /* - asynchronous event data */
    KvaserUSB_Timestamp_t timeRef;      /* - time reference (UTC+0) */
    KvaserUSB_Frequency_t canClock;     /* - CAN clock in [MHz] */
    KvaserUSB_Frequency_t timerFreq;    /* - CAN timer in [MHz] */
    KvaserUSB_HydraBuffer_t hydraBuf;   /* - retention buffer */
    struct tx_acknowledge_tag {         /* - Tx acknowledge: */
        uint8_t maxMsg;                 /*   - max. outstanding Tx messages */
        uint8_t cntMsg;                 /*   - number of sent Tx messages */
        uint8_t transId;                /*   - transaction ID (0..maxMsg-1) */
        bool noAck;                     /*   - flag to skip CMD_TX_ACKNOWLEDGE */
    } txAck;
    uint64_t msgCounter;                /* - number of received CAN frames */
    uint64_t stsCounter;                /* - number of received error frames */
    uint64_t errCounter;                /* - number of received error events */
    // TODO: do we need a mutex?
} KvaserUSB__AsyncContext_t, KvaserUSB_RecvData_t;
typedef CANUSB_AsyncPipe_t KvaserUSB_RecvPipe_t;

typedef struct kvaser_send_context_t_ { /* USB write pipe context: */
#if (0)
    CANQUE_MsgQueue_t msgQueue;         /* - message queue for CAN frames to be sent */
    bool isBusy;                        /* - to indicate a transmission in progress */
#endif
    uint64_t msgCounter;                /* - number of written CAN frames */
    uint64_t errCounter;                /* - number of write pipe errors */
    // TODO: do we need a mutex?
} KvaserUSB_SendContext_t, KvaserUSB_SendData_t;
// TODO: typedef CANUSB_AsyncPipe_t KvaserUSB_SendPipe_t;

typedef uint8_t KvaserUSB_CanChannel_t; /* CAN channel on a device (range 0,..n) */

typedef int32_t KvaserUSB_CanClock_t;

typedef struct kavser_hydra_channel_t_ {/* Hydra device data (Leaf Pro): */
    uint8_t channel2he;                 /* - to map a channel no. to HE (6-bit) */
    uint8_t he2channel;                 /* - to map 6-bit HE to a channel number */
    uint8_t sysdbg_he;                  /* - to map What? to HE (6-bit) */
} KvaserUSB_HydraData_t;

typedef struct kvaser_device_t_ {       /* KvaserCAN device: */
    uint16_t productId;                 /* - USB product id. */
    uint16_t releaseNo;                 /* - USB release no. */
    CANUSB_Handle_t handle;             /* - USB hanlde */
    KvaserUSB_Endpoints_t endpoints;    /* - USB endpoints */
    KvaserUSB_RecvPipe_t recvPipe;      /* - USB reception pipe */
    KvaserUSB_RecvData_t recvData;      /* - pipe w/ CAN message queue */
    KvaserUSB_SendData_t sendData;      /* - only some statistical counters */
    KvaserUSB_CanChannel_t numChannels; /* - number of CAN channels */
    KvaserUSB_CanChannel_t channelNo;   /* - active CAN channel on device */
    KvaserUSB_OpMode_t opCapability;    /* - CAN operation mode capabilities */
    KvaserUSB_DeviceInfo_t deviceInfo;  /* - device information (hw, sw, etc.) */
    KvaserUSB_DriverType_t driverType;  /* - driver type (Leaf or Mhydra device) */
    KvaserUSB_HydraData_t hydraData;    /* - Hydra device data (e.g. Leaf Pro HS v2) */
    char name[KVASER_MAX_STRING_LENGTH+1];   /* - device name (zero-terminated string) */
    char vendor[KVASER_MAX_STRING_LENGTH+1]; /* - vendor name (zero-terminated string) */
    char website[KVASER_MAX_STRING_LENGTH+1];/* - vendor website (zero-terminated string) */
    bool configured;                    /* - to indicate the structure's validity */
} KvaserUSB_Device_t;

#ifdef __cplusplus
extern "C" {
#endif

extern CANUSB_Return_t KvaserUSB_ProbeUsbDevice(CANUSB_Index_t channel, uint16_t *productId);
extern CANUSB_Return_t KvaserUSB_OpenUsbDevice(CANUSB_Index_t channel, KvaserUSB_Device_t *device);
extern CANUSB_Return_t KvaserUSB_CloseUsbDevice(KvaserUSB_Device_t *device);

extern CANUSB_Return_t KvaserUSB_StartReception(KvaserUSB_Device_t *device, CANUSB_AsyncPipeCbk_t callback);
extern CANUSB_Return_t KvaserUSB_AbortReception(KvaserUSB_Device_t *device);

extern CANUSB_Return_t KvaserUSB_SendRequest(KvaserUSB_Device_t *device, const uint8_t *buffer, uint32_t nbyte);
extern CANUSB_Return_t KvaserUSB_ReadResponse(KvaserUSB_Device_t *device, uint8_t *buffer, uint32_t nbyte,
                                                                          uint8_t cmdCode, /*uint8_t transId,*/ uint16_t timeout);

extern void KvaserUSB_TimestampFromTicks(KvaserUSB_Timestamp_t *timeStamp, KvaserUSB_CpuTicks_t cpuTicks, KvaserUSB_Frequency_t cpuFreq);
extern uint64_t KvaserUSB_NanosecondsFromTicks(KvaserUSB_CpuTicks_t cpuTicks, KvaserUSB_Frequency_t cpuFreq);

#ifdef __cplusplus
}
#endif
#endif /* KVASERUSB_DEVICE_H_INCLUDED */
