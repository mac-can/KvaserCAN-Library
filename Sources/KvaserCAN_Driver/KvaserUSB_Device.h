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
#ifndef KVASERUSB_DEVICE_H_INCLUDED
#define KVASERUSB_DEVICE_H_INCLUDED

#include "KvaserUSB_Common.h"

#include "MacCAN_IOUsbKit.h"
#include "MacCAN_MsgQueue.h"
#include "MacCAN_MsgPipe.h"

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
} KvaserUSB_InterfaceInfo_t;

typedef struct kvaser_software_info_t_ {  /* software info: */
    uint32_t swOptions;                 /* - software options */
    uint32_t firmwareVersion;           /* - firmware version */
    uint16_t maxOutstandingTx;          /* - max. outstanding Tx */
    uint32_t swName;                    /* - software name (hydra only) */
    uint8_t EAN[8];                     /* - EAN code (LSB first, hydra only) */
    uint32_t maxBitrate;                /* - max. bit-rate (hydra only, otherwise 1Mbit/s)*/
} KvaserUSB_SoftwareInfo_t;

typedef struct kvaser_device_info_t_ {  /* device info: */
    KvaserUSB_CardInfo_t card;          /* - card info */
//  KvaserUSB_InterfaceInfo_t channel;  /* - channel info */
    KvaserUSB_SoftwareInfo_t software;  /* - software info */
} KvaserUSB_DeviceInfo_t;

typedef struct kvaser_bus_params_t_ {   /* bus parameter: */
    uint32_t bitRate;                   /* - bit rate (in [Hz]) */
    uint8_t  tseg1;                     /* - time segment 1 */
    uint8_t  tseg2;                     /* - time segment 1 */
    uint8_t  sjw;                       /* - synchronization jump width */
    uint8_t  noSamp;                    /* - number of sample points */
} KvaserUSB_BusParams_t;

typedef uint8_t KvaserUSB_OpMode_t;     /* operation mode (CAN API V1 compatible) */

typedef uint8_t KvaserUSB_DriverMode_t; /* driver mode (OFF, NORMAL, SILENT) */

typedef uint8_t KvaserUSB_BusStatus_t;  /* bus status (Kvaser bus status)! */

typedef uint16_t KvaserUSB_BusLoad_t;   /* bus load (0..10000 = 0.00..100.00) */

typedef struct kvaser_chip_state_event_t_ {  /* event - chip state: */
    uint8_t  channel;                        /* - channel no. (from header) */
    uint16_t time[3];
    uint8_t  txErrorCounter;
    uint8_t  rxErrorCounter;
    uint8_t  busStatus;
    uint8_t  _reserved1;
    uint16_t _reserved2;
} KvaserUSB_ChipStateEvent_t;

typedef struct kvaser_error_event_t_ {  /* event - error event: */
    uint8_t  errorCode;                 /* - error code (from header) */
    uint16_t time[3];
    uint16_t _reserved;
    uint16_t addInfo1;
    uint16_t addInfo2;
} KvaserUSB_ErrorEvent_t;

typedef struct kvaser_can_error_event_t_ {  /* event - CAN error event: */
    uint8_t  flags;                         /* - flags (from header) */
    uint16_t time[3];
    uint8_t  channel;
    uint8_t  _reserved;
    uint8_t  txErrorCounter;
    uint8_t  rxErrorCounter;
    uint8_t  busStatus;
    uint8_t  errorFactor;
} KvaserUSB_CanErrorEvent_t;

typedef struct kvaser_event_data_t_  {  /* event data: */
    KvaserUSB_ChipStateEvent_t chipState;
    KvaserUSB_ErrorEvent_t errorEvent;
    KvaserUSB_CanErrorEvent_t canError;
} KvaserUSB_EventData_t;

typedef struct kvaser_endpoints_t_ {    /* USB endpoints: */
    uint8_t numEndpoints;               /* - number of endpoints */
    struct {                            /* - endpoints (pipes): */
        uint8_t pipeRef;                /*   - endpoint number */
        uint32_t packetSize;            /*   - endpoint size */
    } bulkIn, bulkOut;  // TODO: [KVASER_MAX_CAN_CHANNELS]
} KvaserUSB_Endpoints_t;

typedef CANUSB_AsyncPipe_t KvaserUSB_RecvPipe_t;

typedef uint8_t KvaserUSB_CpuClock_t;

typedef uint64_t KvaserUSB_CpuTicks_t;

typedef struct kvaser_recv_data_t_ {    /* USB pipe context: */
    CANPIP_MsgPipe_t msgPipe;           /* - message pipe for data exchange */
    CANQUE_MsgQueue_t msgQueue;         /* - message queue for received CAN frames */
    KvaserUSB_OpMode_t opMode;          /* - demanded CAN operation mode */
    KvaserUSB_EventData_t evData;       /* - asynchronous event data */
    KvaserUSB_Timestamp_t timeRef;      /* - time reference (UTC+0) */
    KvaserUSB_CpuClock_t cpuFreq;       /* - CPU frequency in [MHz] */
    struct tx_acknowledge_tag {         /* - Tx acknowledge: */
        uint8_t maxMsg;                 /*   - max. outstanding Tx messages */
        uint8_t cntMsg;                 /*   - number of sent Tx messages */
        uint8_t transId;                /*   - transaction ID (0..maxMsg-1) */
        bool noAck;                     /*   - flag to skip CMD_TX_ACKNOWLEDGE */
    } txAck;
    // TODO: do we need a mutex?
} KvaserUSB_RecvData_t;

typedef uint8_t KvaserUSB_CanChannel_t; /* CAN channel on a device (range 0,..n) */

typedef struct kvaser_device_t_ {       /* KvaserCAN device: */
    uint16_t productId;                 /* - USB product id. */
    uint16_t releaseNo;                 /* - USB release no. */
    CANUSB_Handle_t handle;             /* - USB hanlde */
    KvaserUSB_Endpoints_t endpoints;    /* - USB endpoints */
    KvaserUSB_RecvPipe_t recvPipe;      /* - USB reception pipe */
    KvaserUSB_RecvData_t recvData;      /* - pipe w/ CAN message queue */
    KvaserUSB_CanChannel_t numChannels; /* - number of CAN channels */
    KvaserUSB_CanChannel_t channelNo;   /* - active CAN channel on device */
    KvaserUSB_OpMode_t opCapability;    /* - CAN operation mode capability */
    KvaserUSB_DeviceInfo_t deviceInfo;  /* - device information (hw, sw, etc.) */
    char name[KVASER_MAX_NAME_LENGTH+1];  /* - device name (zero-terminated string) */
    char vendor[KVASER_MAX_NAME_LENGTH+1];/* - vendor name (zero-terminated string) */
    char website[KVASER_MAX_NAME_LENGTH+1];/*- vendor website (zero-terminated string) */
    bool configured;                    /* - to indicate the structure's validity */
} KvaserUSB_Device_t;

#ifdef __cplusplus
extern "C" {
#endif

extern CANUSB_Return_t KvaserUSB_ProbeUsbDevice(CANUSB_Index_t channel, uint16_t *productId);
extern CANUSB_Return_t KvaserUSB_OpenUsbDevice(CANUSB_Index_t channel, KvaserUSB_Device_t *device);
extern CANUSB_Return_t KvaserUSB_CloseUsbDevice(KvaserUSB_Device_t *device);

extern CANUSB_Return_t KvaserUSB_StartReception(KvaserUSB_Device_t *device, CANUSB_Callback_t callback);
extern CANUSB_Return_t KvaserUSB_AbortReception(KvaserUSB_Device_t *device);

extern CANUSB_Return_t KvaserUSB_SendRequest(KvaserUSB_Device_t *device, const uint8_t *buffer, uint32_t nbyte);
extern CANUSB_Return_t KvaserUSB_ReadResponse(KvaserUSB_Device_t *device, uint8_t *buffer, uint32_t nbyte,
                                                                          uint8_t cmdCode, /*uint8_t transId,*/ uint16_t timeout);

extern void KvaserUSB_TimestampFromTicks(KvaserUSB_Timestamp_t *timeStamp, KvaserUSB_CpuTicks_t cpuTicks, KvaserUSB_CpuClock_t cpuFreq);
extern uint64_t KvaserUSB_NanosecondsFromTicks(KvaserUSB_CpuTicks_t cpuTicks, KvaserUSB_CpuClock_t cpuFreq);

#ifdef __cplusplus
}
#endif
#endif /* KVASERUSB_DEVICE_H_INCLUDED */
