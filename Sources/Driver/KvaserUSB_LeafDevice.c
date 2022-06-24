/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
 *
 *  Copyright (c) 2020-2022 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
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
#include "KvaserUSB_LeafDevice.h"
#include "KvaserCAN_Devices.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <assert.h>

#include "MacCAN_Debug.h"

#ifndef OPTION_PRINT_DEVICE_INFO
#define OPTION_PRINT_DEVICE_INFO  0  /* note: set to non-zero value to print device information */
#endif
#ifndef OPTION_PRINT_BUS_PARAMS
#define OPTION_PRINT_BUS_PARAMS  0  /* note: set to non-zero value to print bus params */
#endif
#define LEN_RX_STD_MESSAGE             24U
#define LEN_TX_STD_MESSAGE             20U
#define LEN_RX_EXT_MESSAGE             24U
#define LEN_TX_EXT_MESSAGE             20U
#define LEN_SET_BUSPARAMS_REQ          12U
#define LEN_GET_BUSPARAMS_REQ           4U
#define LEN_GET_BUSPARAMS_RESP         12U
#define LEN_GET_CHIP_STATE_REQ          4U
#define LEN_CHIP_STATE_EVENT           16U
#define LEN_SET_DRIVERMODE_REQ          8U
#define LEN_GET_DRIVERMODE_REQ          4U
#define LEN_GET_DRIVERMODE_RESP         8U
#define LEN_RESET_CHIP_REQ              4U
#define LEN_RESET_CARD_REQ              4U
#define LEN_START_CHIP_REQ              4U
#define LEN_START_CHIP_RESP             4U
#define LEN_STOP_CHIP_REQ               4U
#define LEN_STOP_CHIP_RESP              4U
#define LEN_READ_CLOCK_REQ              4U
#define LEN_READ_CLOCK_RESP             8U
#define LEN_GET_CARD_INFO_2            32U
#define LEN_GET_CARD_INFO_REQ           4U
#define LEN_GET_CARD_INFO_RESP         32U
#define LEN_GET_INTERFACE_INFO_REQ      4U
#define LEN_GET_INTERFACE_INFO_RESP    12U
#define LEN_GET_SOFTWARE_INFO_REQ       4U
#define LEN_GET_SOFTWARE_INFO_RESP     32U
#define LEN_GET_BUSLOAD_REQ             4U
#define LEN_GET_BUSLOAD_RESP           16U
#define LEN_RESET_STATISTICS            4U
#define LEN_ERROR_EVENT                16U
#define LEN_FLUSH_QUEUE                 8U
#define LEN_FILO_FLUSH_QUEUE_RESP       8U
#define LEN_RESET_ERROR_COUNTER         4U
#define LEN_TX_ACKNOWLEDGE             12U
#define LEN_CAN_ERROR_EVENT            16U
#define LEN_LOG_MESSAGE                24U
#define LEN_GET_CAPABILITIES_REQ        8U
#define LEN_GET_CAPABILITIES_RESP      16U
#define LEN_GET_TRANSCEIVER_INFO_REQ    4U
#define LEN_GET_TRANSCEIVER_INFO_RESP  12U

#define MIN(x,y)  (((x) < (y)) ? (x) : (y))

static void ReceptionCallback(void *refCon, UInt8 *buffer, UInt32 size);
static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_Frequency_t frequency);
static bool DecodeMessage(KvaserUSB_CanMessage_t *message, uint8_t *buffer, uint32_t nbyte, KvaserUSB_Frequency_t frequency);

static uint32_t FillSetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, const KvaserUSB_BusParams_t *params);
static uint32_t FillGetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillSetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, const KvaserUSB_DriverMode_t mode);
static uint32_t FillGetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillGetChipStateReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillStartChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillStopChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillResetChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillResetCardReq(uint8_t *buffer, uint32_t maxbyte);
static uint32_t FillTxCanMessageReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, uint8_t transId, const KvaserUSB_CanMessage_t *message);
static uint32_t FillFlushQueueReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, uint8_t flags);
static uint32_t FillResetErrorCounterReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillResetStatisticsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillReadClockReq(uint8_t *buffer, uint32_t maxbyte, uint8_t flags);
static uint32_t FillGetBusLoadReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillGetCardInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t dataLevel);
static uint32_t FillGetSoftwareInfoReq(uint8_t *buffer, uint32_t maxbyte);
static uint32_t FillGetInterfaceInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillGetCapabilitiesReq(uint8_t *buffer, uint32_t maxbyte, uint16_t subCmd, uint16_t subData);
static uint32_t FillGetTransceiverInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
#if (OPTION_PRINT_DEVICE_INFO != 0)
 static void PrintDeviceInfo(const KvaserUSB_DeviceInfo_t *deviceInfo);
#endif

bool Leaf_ConfigureChannel(KvaserUSB_Device_t *device) {
    /* sanity check */
    if (!device)
        return false;
    if (device->configured)
        return false;
    if (device->handle == CANUSB_INVALID_HANDLE)
        return false;
    if (device->driverType != USB_LEAF_DRIVER)
        return false;
    if (device->numChannels != LEAF_NUM_CHANNELS)  // TODO: multi-channel devices
        return false;
    if (device->endpoints.numEndpoints != LEAF_NUM_ENDPOINTS)
        return false;

    /* set CAN channel properties and defaults */
    device->channelNo = 0U;  /* note: only one CAN channel */
    device->recvData.canClock = KvaserDEV_GetCanClockInMHz(device->productId);
    device->recvData.timerFreq = KvaserDEV_GetTimerFreqInMHz(device->productId);
    device->recvData.txAck.maxMsg = LEAF_MAX_OUTSTANDING_TX;

    /* set CAN channel operation capabilities from device spec. */
    device->opCapability = 0x00U;  /* note: CAN FD not supported by Leaf devices */
    device->opCapability |= KvaserDEV_IsErrorFrameSupported(device->productId) ? CANMODE_ERR : 0x00U;
    device->opCapability |= KvaserDEV_IsSilentModeSupported(device->productId) ? CANMODE_MON : 0x00U;
    device->opCapability |= CANMODE_NXTD;
    device->opCapability |= CANMODE_NRTR;

    /* Gotcha! */
    device->configured = true;

    return device->configured;
}

CANUSB_Return_t Leaf_InitializeChannel(KvaserUSB_Device_t *device, const KvaserUSB_OpMode_t opMode) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    MACCAN_DEBUG_DRIVER("    Initializing %s driver...\n", device->name);
    /* start the reception loop */
    retVal = KvaserUSB_StartReception(device, ReceptionCallback);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): reception loop could not be started (%i)\n", device->name, device->handle, retVal);
        goto end_init;
    }
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = Leaf_StopChip(device, 0U);  /* 0 = don't wait for response */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* set driver mode OFF */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set driver mode NORMAL\n", device->name, device->handle);
    retVal = Leaf_SetDriverMode(device, DRIVERMODE_NORMAL);  /* note: OFF doesn't work */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): driver mode could not be set (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* trigger chip state event */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): trigger chip state event\n", device->name, device->handle);
    retVal = Leaf_RequestChipState(device, 0U);  /* 0 = no delay */
    if (retVal != CANUSB_SUCCESS) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip state event could not be triggered (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* get device information: card, software, interface, transceiver */
    retVal = Leaf_GetCardInfo(device, &device->deviceInfo.card);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): card information could not be read (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    retVal = Leaf_GetSoftwareInfo(device, &device->deviceInfo.software);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): firmware information could not be read (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
#if (0)
    retVal = Leaf_GetInterfaceInfo(device, &device->deviceInfo.channel);  // FIXME: returns (-50)
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): channel information could not be read (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
#endif
    retVal = Leaf_GetTransceiverInfo(device, &device->deviceInfo.transceiver);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): transceiver information could not be read (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* get device capabilities (if supported) */
    if (device->deviceInfo.software.swOptions & SWOPTION_CAP_REQ) {
        retVal = Leaf_GetCapabilities(device, &device->deviceInfo.capabilities);
        if (retVal < 0) {
            MACCAN_DEBUG_ERROR("+++ %s (device #%u): channel capabilities could not be read (%i)\n", device->name, device->handle, retVal);
            goto err_init;
        }
    }
#if (OPTION_PRINT_DEVICE_INFO != 0)
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): properties and capabilities\n", device->name, device->handle);
    PrintDeviceInfo(&device->deviceInfo);  /* note: only for debugging purposes */
#endif
    /* get CPU clock frequency (in [MHz]) from software options */
    switch (device->deviceInfo.software.swOptions & SWOPTION_CPU_FQ_MASK) {
        case SWOPTION_16_MHZ_CLK: device->recvData.canClock = 16U; break;
        case SWOPTION_32_MHZ_CLK: device->recvData.canClock = 32U; break;
        case SWOPTION_24_MHZ_CLK: device->recvData.canClock = 24U; break;
        default:
            device->recvData.canClock = KvaserDEV_GetCanClockInMHz(device->productId);
            break;
    }
    device->recvData.timerFreq = device->recvData.canClock;

    /* get reference time (amount of time in seconds and nanoseconds since the Epoch) */
    (void)clock_gettime(CLOCK_REALTIME, &device->recvData.timeRef);  // TODO: not used yet
#if (OPTION_PRINT_DEVICE_INFO != 0)
    MACCAN_DEBUG_DRIVER("    - clocks:\n");
    MACCAN_DEBUG_DRIVER("      - CAN clock: %u MHz\n", device->recvData.canClock);
    MACCAN_DEBUG_DRIVER("      - CAN timer: %u MHz\n", device->recvData.timerFreq);
    /* get device clock (don't care about the result) */
    uint64_t nsec = 0U;
    retVal = Leaf_ReadClock(device, &nsec);  // FIXME: returns (-50)
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): device clock could not be read (%i)\n", device->name, device->handle, retVal);
    }
    MACCAN_DEBUG_DRIVER("      - Clock: %u.%04u sec\n", (nsec / 1000000000), ((nsec % 1000000000) / 1000000));
    MACCAN_DEBUG_DRIVER("      - Time: %u.%04u sec\n", device->recvData.timeRef.tv_sec, device->recvData.timeRef.tv_nsec / 1000000);
#endif
    /* get max. outstanding transmit messages */
    if ((0U < device->deviceInfo.software.maxOutstandingTx) &&
        (device->deviceInfo.software.maxOutstandingTx < MIN(LEAF_MAX_OUTSTANDING_TX, 255U)))
        device->recvData.txAck.maxMsg = (uint8_t)device->deviceInfo.software.maxOutstandingTx;
    else
        device->recvData.txAck.maxMsg = (uint8_t)MIN(LEAF_MAX_OUTSTANDING_TX, 255U);
    /* update capabilities from software options (in case of wrong configuration) */
    device->opCapability &= ~(CANMODE_FDOE | CANMODE_BRSE | CANMODE_NISO);  /* note: CAN FD not supported by Leaf devices */
    /* check requested operation mode */
    if ((opMode & ~device->opCapability)) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): unsupported operation mode (%02x)\n", device->name, device->handle, (opMode & ~device->opCapability));
        retVal = CANUSB_ERROR_ILLPARA;  /* note: cf. CAN API V3 function 'can_test' */
        goto err_init;
    }
    /* store demanded CAN operation mode*/
    device->recvData.opMode = opMode;
    retVal = CANUSB_SUCCESS;
end_init:
    return retVal;
err_init:
    (void)KvaserUSB_AbortReception(device);
    return retVal;
}

CANUSB_Return_t Leaf_TeardownChannel(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    MACCAN_DEBUG_DRIVER("    Teardown %s driver...\n", device->name);
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = Leaf_StopChip(device, 0U);  /* 0 = don't wait for response */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
        //goto end_exit;
    }
    /* set driver mode OFF */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set driver mode NORMAL\n", device->name, device->handle);
    retVal = Leaf_SetDriverMode(device, DRIVERMODE_NORMAL);  /* note: OFF doesn't work */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): driver mode could not be set (%i)\n", device->name, device->handle, retVal);
        //goto end_exit;
    }
//end_exit:
    /* stop the reception loop */
    retVal = KvaserUSB_AbortReception(device);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): reception loop could not be aborted (%i)\n", device->name, device->handle, retVal);
        return retVal;
    }
    return retVal;
}

CANUSB_Return_t Leaf_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_SET_BUSPARAMS_REQ w/o response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillSetBusParamsReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo, params);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
#if (OPTION_PRINT_BUS_PARAMS != 0)
    if (retVal == CANUSB_SUCCESS) {
        MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set bus params - freq=%u tseg1=%u tseg2=%u sjw=%u sam=%u\n", device->name, device->handle,
                            params->bitRate, params->tseg1, params->tseg2, params->sjw, params->noSamp);
    }
#endif
    return retVal;
}

CANUSB_Return_t Leaf_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_BUSPARAMS_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetBusParamsReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_GET_BUSPARAMS_RESP;
        resp = CMD_GET_BUSPARAMS_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..7: bitRate
             * - byte 8: tseg1
             * - byte 9: tseg2
             * - byte 10: sjw
             * - byte 11: noSamp
             */
            params->bitRate = BUF2UINT32(buffer[4]);
            params->tseg1 = BUF2UINT8(buffer[8]);
            params->tseg2 = BUF2UINT8(buffer[9]);
            params->sjw = BUF2UINT8(buffer[10]);
            params->noSamp = BUF2UINT8(buffer[11]);
#if (OPTION_PRINT_BUS_PARAMS != 0)
            MACCAN_DEBUG_DRIVER(">>> %s (device #%u): get bus params - freq=%u tseg1=%u tseg2=%u sjw=%u sam=%u\n", device->name, device->handle,
                                params->bitRate, params->tseg1, params->tseg2, params->sjw, params->noSamp);
#endif
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_SetDriverMode(KvaserUSB_Device_t *device, const KvaserUSB_DriverMode_t mode) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_SET_DRIVERMODE_REQ w/o response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillSetDriverModeReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo, mode);
    retVal = KvaserUSB_SendRequest(device, buffer, size);

    return retVal;
}

CANUSB_Return_t Leaf_GetDriverMode(KvaserUSB_Device_t *device, KvaserUSB_DriverMode_t *mode) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !mode)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_DRIVERMODE_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetDriverModeReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_GET_DRIVERMODE_RESP;
        resp = CMD_GET_DRIVERMODE_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4: drivermode
             * - byte 8..11: (not used)
             */
            *mode = BUF2UINT32(buffer[4]);
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_StartChip(KvaserUSB_Device_t *device, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_START_CHIP_REQ and wait for response (optional) */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillStartChipReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (timeout > 0U)) {
        size = LEN_START_CHIP_RESP;
        resp = CMD_START_CHIP_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, timeout);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             */
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_StopChip(KvaserUSB_Device_t *device, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_STOP_CHIP_REQ and wait for response (optional) */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillStopChipReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (timeout > 0U)) {
        size = LEN_STOP_CHIP_RESP;
        resp = CMD_STOP_CHIP_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, timeout);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             */
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_ResetChip(KvaserUSB_Device_t *device, uint16_t delay)  {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_CHIP_REQ w/o response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillResetChipReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Leaf_ResetCard(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_CARD_REQ w/o response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillResetCardReq(buffer, KVASER_MAX_COMMAND_LENGTH);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Leaf_RequestChipState(KvaserUSB_Device_t *device, uint16_t delay)  {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_CHIP_STATE_REQ w/o response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetChipStateReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Leaf_SendMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !message)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* suppress certain CAN messages depending on the operation mode */
    if (message->xtd && (device->recvData.opMode & CANMODE_NXTD))
        return CANUSB_ERROR_ILLPARA;
    if (message->rtr && (device->recvData.opMode & CANMODE_NRTR))
        return CANUSB_ERROR_ILLPARA;
    if (message->sts)  /* note: error frames cannot be sent */
        return CANUSB_ERROR_ILLPARA;

    /* check for pending transmit messages */
    if (device->recvData.txAck.cntMsg < device->recvData.txAck.maxMsg)
        device->recvData.txAck.transId = (device->recvData.txAck.transId + 1U) % device->recvData.txAck.maxMsg;
    else
        return CANUSB_ERROR_FULL;

    /* channel no. and transaction id */
    uint8_t channel = device->channelNo;
    uint8_t transId = device->recvData.txAck.transId;

    /* send request CMD_TX_{STD|EXT}_MESSAGE and wait for ackknowledge (optional) */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillTxCanMessageReq(buffer, KVASER_MAX_COMMAND_LENGTH, channel, transId, message);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_TX_ACKNOWLEDGE;
        resp = CMD_TX_ACKNOWLEDGE;
        if (timeout > 0U) {
            /* expect a response (timed or blocking write) */
            device->recvData.txAck.noAck = false;
            retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, timeout);
            /* note: the transaction id. is ckecked by the callback routine
             */
            if (retVal == CANUSB_SUCCESS) {
                /* command response:
                 * - byte 0..3: (header) [note: channel in byte 2]
                 * - byte 4..9: time (48-bit)
                 * - byte 10: flags
                 * - byte 11: time offset
                 */
                // TODO: what to do with them?
            }
        } else {
            /* skip the response in the callback routine */
            device->recvData.txAck.noAck = true;
        }
        /* one more transmit message pending*/
        device->recvData.txAck.cntMsg++;
    }
    return retVal;
}

CANUSB_Return_t Leaf_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device || !message)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* read one CAN message from message queue, if any */
    retVal = CANQUE_Dequeue(device->recvData.msgQueue, (void*)message, timeout);

    return retVal;
}

CANUSB_Return_t Leaf_FlushQueue(KvaserUSB_Device_t *device/*, uint8_t flags*/) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;
    uint8_t flags = 0x00U;  // TODO: symbolic constants

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_FLUSH_QUEUE and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillFlushQueueReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo, flags);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_FILO_FLUSH_QUEUE_RESP;
        resp = CMD_FILO_FLUSH_QUEUE_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..7: flags
             * - byte 8..11: (not used)
             */
            /* flags = BUF2UINT32(buffer[4]); */
            // TODO: what to do with them
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_ResetErrorCounter(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_ERROR_COUNTER w/o response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillResetErrorCounterReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Leaf_ResetStatistics(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_STATISTICS w/o response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillResetStatisticsReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Leaf_ReadClock(KvaserUSB_Device_t *device, uint64_t *nsec) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !nsec)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_READ_CLOCK_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillReadClockReq(buffer, KVASER_MAX_COMMAND_LENGTH, READ_CLOCK_NOW);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_READ_CLOCK_RESP;
        resp = CMD_READ_CLOCK_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..9: time (48-bit)
             * - byte 10+11: (not used)
             */
            uint64_t ticks = 0ULL;
            ticks |= (uint64_t)BUF2UINT16(buffer[4]) << 0;
            ticks |= (uint64_t)BUF2UINT16(buffer[6]) << 16;
            ticks |= (uint64_t)BUF2UINT16(buffer[8]) << 32;
            /* note: when software options not read before, assume clock running at 24MHz
             */
            if (device->recvData.timerFreq == 0U)
                device->recvData.timerFreq = KvaserDEV_GetTimerFreqInMHz(device->productId);
            *nsec = KvaserUSB_NanosecondsFromTicks(ticks, device->recvData.timerFreq);
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !load)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_BUSLOAD_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetBusLoadReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_GET_BUSLOAD_RESP;
        resp = CMD_GET_BUSLOAD_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..9: time-stamp (absolute time)
             * - byte 10+11: sampling interval (in [usec])
             * - byte 12+13: number of samples where tx or rx was active
             * - byte 14+15: milliseconds since last response
             */
            uint64_t interval = (uint64_t)BUF2UINT16(buffer[10]);
            uint64_t samples = (uint64_t)BUF2UINT16(buffer[12]);
            uint64_t delta_t = (uint64_t)BUF2UINT16(buffer[14]);
            if (delta_t != 0U) {
                uint64_t zwerg = (samples * interval * 10ULL) / delta_t;
                /* 0..10000 is equivalent 0.00%..100.00% */
                if (zwerg > 10000ULL)
                    zwerg = 10000ULL;
                *load = (KvaserUSB_BusLoad_t)zwerg;
            }
            else
                *load = (KvaserUSB_BusLoad_t)0;
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_GetCardInfo(KvaserUSB_Device_t *device, KvaserUSB_CardInfo_t *info/*, uint8_t dataLevel*/) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_CARD_INFO_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetCardInfoReq(buffer, KVASER_MAX_COMMAND_LENGTH, 0U/*dataLevel*/);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        /* note: we first receive CARD_INFO_V2 response, but we ignore it
         */
        size = LEN_GET_CARD_INFO_RESP;
        resp = CMD_GET_CARD_INFO_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command length (header)
             * - byte 1: command code (header)
             * - byte 2: transaction id. (header)
             * - byte 3: channel count (user data)
             * - byte 4..7: serial no.
             * - byte 8..11: (not used)
             * - byte 12-15: clock resolution
             * - byte 16..19: manufacturing date
             * - byte 20..27: EAN code (LSB first)
             * - byte 28: hardware revision
             * - byte 29: USB HS mode
             * - byte 30: hardware type
             * - byte 31: CAN time-stamp reference
             */
            info->channelCount = BUF2UINT8(buffer[3]);
            info->serialNumber = BUF2UINT32(buffer[4]);
            info->clockResolution = BUF2UINT32(buffer[12]);
            info->mfgDate = BUF2UINT32(buffer[16]);
            info->EAN[0] = BUF2UINT8(buffer[20]);
            info->EAN[1] = BUF2UINT8(buffer[21]);
            info->EAN[2] = BUF2UINT8(buffer[22]);
            info->EAN[3] = BUF2UINT8(buffer[23]);
            info->EAN[4] = BUF2UINT8(buffer[24]);
            info->EAN[5] = BUF2UINT8(buffer[25]);
            info->EAN[6] = BUF2UINT8(buffer[26]);
            info->EAN[7] = BUF2UINT8(buffer[27]);
            info->hwRevision = BUF2UINT8(buffer[28]);
            info->usbHsMode = BUF2UINT8(buffer[29]);
            info->hwType = BUF2UINT8(buffer[30]);
            info->canTimeStampRef = BUF2UINT8(buffer[31]);
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_GetSoftwareInfo(KvaserUSB_Device_t *device, KvaserUSB_SoftwareInfo_t *info) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_SOFTWARE_INFO_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetSoftwareInfoReq(buffer, KVASER_MAX_COMMAND_LENGTH);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_GET_SOFTWARE_INFO_RESP;
        resp = CMD_GET_SOFTWARE_INFO_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..7: swOptions
             * - byte 8..11: firmwareVersion
             * - byte 12..13: maxOutstandingTx
             * - byte 14..31: (not used)
             */
            info->swOptions = BUF2UINT32(buffer[4]);
            info->firmwareVersion = BUF2UINT32(buffer[8]);
            info->maxOutstandingTx = BUF2UINT16(buffer[12]);
            info->maxBitrate = 1000000U;  // CAN 2.0 max. 1Mbit/s
       }
    }
    return retVal;
}

CANUSB_Return_t Leaf_GetInterfaceInfo(KvaserUSB_Device_t *device, KvaserUSB_InterfaceInfo_t *info) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* note: this command seem not to work on the Leaf Light v2 device!
     */
    /* send request CMD_GET_INTERFACE_INFO_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetInterfaceInfoReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_GET_INTERFACE_INFO_RESP;
        resp = CMD_GET_INTERFACE_INFO_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..7: channelCapabilities
             * - byte 8: canChipType
             * - byte 9: canChipSubType
             * - byte 10+11: (not used)
             */
            info->channelCapabilities = BUF2UINT32(buffer[4]);
            info->canChipType = BUF2UINT8(buffer[8]);
            info->canChipSubType = BUF2UINT8(buffer[9]);
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_GetCapabilities(KvaserUSB_Device_t *device, KvaserUSB_Capabilities_t *capabilities) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !capabilities)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* set default values */
    capabilities->dummy = 0;
    capabilities->silentMode = KvaserDEV_IsSilentModeSupported(device->productId);
    capabilities->errorFrame = KvaserDEV_IsErrorFrameSupported(device->productId);
    capabilities->busStats = 0;
    capabilities->errorCount = 0;
    capabilities->singleShot = 0;
    capabilities->syncTxFlush = 0;
    capabilities->hasLogger = 0;
    capabilities->hasRemote = 0;
    capabilities->hasScript = 0;

    /* sub-commands */
    uint16_t subCmds[9] = {
        CAP_SUB_CMD_SILENT_MODE,
        CAP_SUB_CMD_ERRFRAME,
        CAP_SUB_CMD_BUS_STATS,
        CAP_SUB_CMD_ERRCOUNT_READ,
        CAP_SUB_CMD_SINGLE_SHOT,
        CAP_SUB_CMD_SYNC_TX_FLUSH,
        CAP_SUB_CMD_HAS_LOGGER,
        CAP_SUB_CMD_HAS_REMOTE,
        CAP_SUB_CMD_HAS_SCRIPT
    };
    /* send request CMD_GET_CAPABILITIES_REQ w/ sub-command and wait for response */
    for (int i = 0; i < 9; i++) {
        bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
        size = FillGetCapabilitiesReq(buffer, KVASER_MAX_COMMAND_LENGTH, subCmds[i], 0x00);  // TODO: subData?
        retVal = KvaserUSB_SendRequest(device, buffer, size);
        if (retVal == CANUSB_SUCCESS) {
            size = LEN_GET_CAPABILITIES_RESP;
            resp = CMD_GET_CAPABILITIES_RESP;
            retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
            if (retVal == CANUSB_SUCCESS) {
                /* command response:
                 * - byte 0..3: (header)
                 * - byte 4..5: sub-command
                 * - byte 6..7: status (0=OK, 1=NOT_IMPLEMENTED, 2=UNAVAILABLE)
                 * - byte 8..11: mask
                 * - byte 12..15: value
                 */
                uint16_t status = BUF2UINT16(buffer[6]);
                uint32_t mask = BUF2UINT32(buffer[8]);
                uint32_t value = BUF2UINT32(buffer[12]);
                if (status == 0) {
                    switch (subCmds[i]) {
                        // TODO: clarify what´s the meaning of 'mask' and 'value'
                        case CAP_SUB_CMD_SILENT_MODE: capabilities->silentMode = (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_ERRFRAME: capabilities->errorFrame = (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_BUS_STATS: capabilities->busStats = (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_ERRCOUNT_READ: capabilities->errorCount = (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_SINGLE_SHOT: capabilities->singleShot = (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_SYNC_TX_FLUSH: capabilities->syncTxFlush= (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_LOGGER: capabilities->hasLogger = (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_REMOTE: capabilities->hasRemote = (value & mask) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_SCRIPT: capabilities->hasScript = (value & mask) ? 1 : 0; break;
                        default: /* nothing to do here */ break;
                    }
                }
            }
        }
    }
    return retVal;
}

CANUSB_Return_t Leaf_GetTransceiverInfo(KvaserUSB_Device_t *device, KvaserUSB_TransceiverInfo_t *info) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_TRANSCEIVER_INFO_REQ and wait for response */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillGetTransceiverInfoReq(buffer, KVASER_MAX_COMMAND_LENGTH, device->channelNo);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = LEN_GET_TRANSCEIVER_INFO_RESP;
        resp = CMD_GET_TRANSCEIVER_INFO_RESP;
        retVal = KvaserUSB_ReadResponse(device, buffer, size, resp, KVASER_USB_COMMAND_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..7: transceiver capabilities
             * - byte 8: transceiver status
             * - byte 9: transceiver type
             * - byte 10+11: (not used)
             */
            info->transceiverCapabilities = BUF2UINT32(buffer[4]);
            info->transceiverStatus = BUF2UINT8(buffer[8]);
            info->transceiverType = BUF2UINT8(buffer[9]);
        }
    }
    return retVal;
}

static void ReceptionCallback(void *refCon, UInt8 *buffer, UInt32 size) {
    KvaserUSB_RecvData_t *context = (KvaserUSB_RecvData_t*)refCon;
    KvaserUSB_CanMessage_t message;
    UInt32 index = 0U;
    UInt32 nbyte;

    assert(refCon);
    assert(buffer);

    /* Kvaser USB command:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3...
     */
    if (size >= KVASER_MIN_COMMAND_LENGTH) {
        /* the "command/message pump" */
        while (index < size) {
            /* get the command length */
            nbyte = (UInt32)buffer[index];
            MACCAN_LOG_WRITE(&buffer[index], nbyte, index ? "+" : "<");
            if ((index + nbyte) > size) {
                MACCAN_LOG_PRINTF("! URB error: expected=%lu vs. received=%lu\n", (index + nbyte), size);
                break;
            }
            /* interpret the command code */
            switch (buffer[index+1]) {
                case CMD_RX_STD_MESSAGE:
                case CMD_RX_EXT_MESSAGE:
                    /* standard or extended CAN message: will not be handled */
                    // TODO: clarify if and what we will receive here
                    break;
                case CMD_CHIP_STATE_EVENT:
                case CMD_ERROR_EVENT:
                case CMD_CAN_ERROR_EVENT:
                    /* event message: update event status */
                    (void)UpdateEventData(&context->evData, &buffer[index], nbyte, context->timerFreq);
                    break;
                case CMD_GET_BUSPARAMS_RESP:
                case CMD_GET_DRIVERMODE_RESP:
                case CMD_START_CHIP_RESP:
                case CMD_STOP_CHIP_RESP:
                case CMD_READ_CLOCK_RESP:
                case CMD_GET_CARD_INFO_RESP:
                case CMD_GET_INTERFACE_INFO_RESP:
                case CMD_GET_SOFTWARE_INFO_RESP:
                case CMD_GET_BUSLOAD_RESP:
                case CMD_FILO_FLUSH_QUEUE_RESP:
                case CMD_GET_CAPABILITIES_RESP:
                case CMD_GET_TRANSCEIVER_INFO_RESP:
                    /* command response: write packet into the pipe */
                    (void)CANPIP_Write(context->msgPipe, &buffer[index], nbyte);
                    break;
                case CMD_LOG_MESSAGE:
                    /* logged CAN message: decode and enqueue */
                    if (DecodeMessage(&message, &buffer[index], nbyte, context->timerFreq)) {
                        /* suppress certain CAN messages depending on the operation mode */
                        if (message.xtd && (context->opMode & CANMODE_NXTD))
                            break;
                        if (message.rtr && (context->opMode & CANMODE_NRTR))
                            break;
                        if (message.sts && !(context->opMode & CANMODE_ERR))
                            break;
                        (void)CANQUE_Enqueue(context->msgQueue, (void*)&message);
                    } else {
                        /* there are flags that do not belong to a received CAN message */
                        (void)UpdateEventData(&context->evData, &buffer[index], nbyte, context->timerFreq);
                    }
                    break;
                case CMD_TX_ACKNOWLEDGE:
                    /* transmit message ackowledgement: write packet into the pipe only when requested */
                    if (!context->txAck.noAck && (buffer[index+3] == context->txAck.transId))
                        (void)CANPIP_Write(context->msgPipe, &buffer[index], nbyte);
                    if (context->txAck.cntMsg > 0)
                        context->txAck.cntMsg--;
                    break;
                default:
                    /* ignore the rest */
                    break;
            }
            /* next command */
            index += nbyte;
        }
    } else {
        /* something went wrong on the USB line */
        MACCAN_LOG_WRITE(buffer, size, "?");
    }
}

static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_Frequency_t frequency) {
    bool result = false;

    if (!event || !buffer)
        return false;
    if (nbyte < KVASER_MIN_COMMAND_LENGTH)
        return false;

    (void)frequency;  // currently not used

    switch (buffer[1]) {
        case CMD_ERROR_EVENT:
            /* command response:
             * - byte 0..3: (header)
             * - byte 3: error code
             * - byte 4..9: time
             * - byte 10..11: (not used)
             * - byte 12..13: additional information 1
             * - byte 14..15: additional information 2
             */
            if (buffer[0] == LEN_ERROR_EVENT) {
                event->errorEvent.errorCode = BUF2UINT8(buffer[3]);
                event->errorEvent.time[0] = BUF2UINT16(buffer[4]);
                event->errorEvent.time[1] = BUF2UINT16(buffer[6]);
                event->errorEvent.time[2] = BUF2UINT16(buffer[8]);
                event->errorEvent._reserved = BUF2UINT16(buffer[10]);
                event->errorEvent.addInfo1 = BUF2UINT16(buffer[12]);
                event->errorEvent.addInfo2 = BUF2UINT16(buffer[14]);
                MACCAN_LOG_PRINTF("! error code: %02X\n", event->errorEvent.errorCode);
                result = true;
            }
            break;
        case CMD_CAN_ERROR_EVENT:
            /* command response:
             * - byte 0..3: (header)
             * - byte 3: flags
             * - byte 4..9: time
             * - byte 10: channel;
             * - byte 11: (not used)
             * - byte 12: tx error counter
             * - byte 13: rx error counter
             * - byte 14: bus status
             * - byte 15: error factor
             */
            if (buffer[0] == LEN_CAN_ERROR_EVENT) {
                event->canError.flags = BUF2UINT8(buffer[3]);
                event->canError.time[0] = BUF2UINT16(buffer[4]);
                event->canError.time[1] = BUF2UINT16(buffer[6]);
                event->canError.time[2] = BUF2UINT16(buffer[8]);
                event->canError.channel = BUF2UINT8(buffer[10]);
                event->canError._reserved = BUF2UINT8(buffer[11]);
                event->canError.txErrorCounter = BUF2UINT8(buffer[12]);
                event->canError.rxErrorCounter = BUF2UINT8(buffer[13]);
                event->canError.busStatus = BUF2UINT8(buffer[14]);
                event->canError.errorFactor = BUF2UINT8(buffer[15]);
                MACCAN_LOG_PRINTF("! CAN error: %02X (flags=%02X)\n", event->canError.busStatus, event->canError.flags);
                result = true;
            }
            break;
        case CMD_CHIP_STATE_EVENT:
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..9: time
             * - byte 10: tx error counter
             * - byte 11: rx error counter
             * - byte 12: bus status
             * - byte 13..15: (not used)
             */
            if (buffer[0] == LEN_CHIP_STATE_EVENT) {
                event->chipState.time[0] = BUF2UINT16(buffer[4]);
                event->chipState.time[1] = BUF2UINT16(buffer[6]);
                event->chipState.time[2] = BUF2UINT16(buffer[8]);
                event->chipState.txErrorCounter = BUF2UINT8(buffer[10]);
                event->chipState.rxErrorCounter = BUF2UINT8(buffer[11]);
                event->chipState.busStatus = BUF2UINT8(buffer[12]);
                event->chipState._reserved1 = BUF2UINT8(buffer[13]);
                event->chipState._reserved2 = BUF2UINT16(buffer[14]);
                MACCAN_LOG_PRINTF("! chip state: %02X\n", event->chipState.busStatus);
                result = true;
            }
            break;
        case CMD_LOG_MESSAGE:
            /* logged message:
             * - byte 0..3: (header)
             * - byte 3: flags
             * - byte 4..24: (irrelevant)
             */
            if (buffer[0] == LEN_LOG_MESSAGE) {
                MACCAN_LOG_PRINTF("! Logged flags:");
                if ((buffer[3] & MSGFLAG_OVERRUN))
                    MACCAN_LOG_PRINTF(" MSGFLAG_OVERRUN");
                if ((buffer[3] & MSGFLAG_NERR))
                    MACCAN_LOG_PRINTF(" MSGFLAG_NERR");
                if ((buffer[3] & MSGFLAG_WAKEUP))
                    MACCAN_LOG_PRINTF(" MSGFLAG_WAKEUP");
                if ((buffer[3] & MSGFLAG_TX))
                    MACCAN_LOG_PRINTF(" MSGFLAG_TX");
                if ((buffer[3] & MSGFLAG_TXRQ))
                    MACCAN_LOG_PRINTF(" MSGFLAG_TXRQ");
                MACCAN_LOG_PRINTF("\n");
            }
            break;
        default:
            /* ignore the rest */
            break;
    }
    return result;
}

static bool DecodeMessage(KvaserUSB_CanMessage_t *message, uint8_t *buffer, uint32_t nbyte, KvaserUSB_Frequency_t frequency) {
    uint64_t ticks = 0ULL;
    bool result = false;

    if (!message || !buffer)
        return false;
    if (nbyte < KVASER_MIN_COMMAND_LENGTH)
        return false;
    if (buffer[0] != LEN_LOG_MESSAGE)
        return false;
    if (buffer[1] != CMD_LOG_MESSAGE)
        return false;

    /* logged message:
     * - byte 0..3: (header)
     * - byte 3: flags
     * - byte 4..9: time
     * - byte 10: dlc
     * - byte 11: timeOffset
     * - byte 12..15: ident
     * - byte 16..24: data[8]
     */
    bzero(message, sizeof(KvaserUSB_CanMessage_t));
    /* message: id, dlc, data and flags */
    message->id = BUF2UINT32(buffer[12]) & CAN_MAX_XTD_ID;
    message->dlc = MIN(BUF2UINT8(buffer[10]), CAN_MAX_LEN);
    memcpy(message->data, &buffer[16], CAN_MAX_LEN);
    message->xtd = (BUF2UINT32(buffer[12]) & 0x80000000U) ? 1 : 0;
    message->rtr = (BUF2UINT8(buffer[3]) & MSGFLAG_REMOTE_FRAME) ? 1 : 0;
    message->sts = (BUF2UINT8(buffer[3]) & MSGFLAG_ERROR_FRAME) ? 1 : 0;
    /* time-stamp from 48-bit timer value */
    ticks |= (uint64_t)BUF2UINT16(buffer[4]) << 0;
    ticks |= (uint64_t)BUF2UINT16(buffer[6]) << 16;
    ticks |= (uint64_t)BUF2UINT16(buffer[8]) << 32;
    KvaserUSB_TimestampFromTicks(&message->timestamp, ticks, frequency);
    /* note: we only enqueue ordinary CAN messages and error frames.
     *       The flags MSGFLAG_OVERRUN, MSGFLAG_NERR, MSGFLAG_WAKEUP,
     *       MSGFLAG_TX and MSGFLAG_TXRQ are handled by UpdateEventData
     */
    if (!(BUF2UINT8(buffer[3]) & ~(MSGFLAG_REMOTE_FRAME | MSGFLAG_ERROR_FRAME)))
        result = true;
    return result;
}

static uint32_t FillSetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, const KvaserUSB_BusParams_t *params) {
    assert(buffer);
    assert(params);
    assert(maxbyte >= LEN_SET_BUSPARAMS_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     * - byte 4..7: bitRate
     * - byte 8: tseg1
     * - byte 9: tseg2
     * - byte 10: sjw
     * - byte 11: noSamp
     */
    buffer[0] = LEN_SET_BUSPARAMS_REQ;
    buffer[1] = CMD_SET_BUSPARAMS_REQ;
    buffer[2] = CMD_SET_BUSPARAMS_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* arguments */
    buffer[4] = UINT8BYTE(params->bitRate);
    buffer[5] = UINT8BYTE(params->bitRate >> 8);
    buffer[6] = UINT8BYTE(params->bitRate >> 16);
    buffer[7] = UINT8BYTE(params->bitRate >> 24);
    buffer[8] = UINT8BYTE(params->tseg1);
    buffer[9] = UINT8BYTE(params->tseg2);
    buffer[10] = UINT8BYTE(params->sjw);
    buffer[11] = UINT8BYTE(params->noSamp);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_BUSPARAMS_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_GET_BUSPARAMS_REQ;
    buffer[1] = CMD_GET_BUSPARAMS_REQ;
    buffer[2] = CMD_GET_BUSPARAMS_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];

}

static uint32_t FillSetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, const KvaserUSB_DriverMode_t mode) {
    assert(buffer);
    assert(maxbyte >= LEN_SET_DRIVERMODE_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     * - byte 4: mode
     * - byte 5..7: (not used)
     */
    buffer[0] = LEN_SET_DRIVERMODE_REQ;
    buffer[1] = CMD_SET_DRIVERMODE_REQ;
    buffer[2] = CMD_SET_DRIVERMODE_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* arguments */
    buffer[4] = UINT8BYTE(mode);
    buffer[5] = UINT8BYTE(0x00);
    buffer[6] = UINT8BYTE(0x00);
    buffer[7] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_DRIVERMODE_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_GET_DRIVERMODE_REQ;
    buffer[1] = CMD_GET_DRIVERMODE_REQ;
    buffer[2] = CMD_GET_DRIVERMODE_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetChipStateReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_CHIP_STATE_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_GET_CHIP_STATE_REQ;
    buffer[1] = CMD_GET_CHIP_STATE_REQ;
    buffer[2] = CMD_GET_CHIP_STATE_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}


static uint32_t FillStartChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_START_CHIP_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_START_CHIP_REQ;
    buffer[1] = CMD_START_CHIP_REQ;
    buffer[2] = CMD_START_CHIP_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillStopChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_STOP_CHIP_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_STOP_CHIP_REQ;
    buffer[1] = CMD_STOP_CHIP_REQ;
    buffer[2] = CMD_STOP_CHIP_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillResetChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_RESET_CHIP_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_RESET_CHIP_REQ;
    buffer[1] = CMD_RESET_CHIP_REQ;
    buffer[2] = CMD_RESET_CHIP_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillResetCardReq(uint8_t *buffer, uint32_t maxbyte) {
    assert(buffer);
    assert(maxbyte >= LEN_RESET_CARD_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: (not used)
     */
    buffer[0] = LEN_RESET_CARD_REQ;
    buffer[1] = CMD_RESET_CARD_REQ;
    buffer[2] = CMD_RESET_CARD_REQ;
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillTxCanMessageReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, uint8_t transId, const KvaserUSB_CanMessage_t *message) {
    assert(buffer);
    assert(message);
    assert(maxbyte >= LEN_TX_STD_MESSAGE);
    assert(maxbyte >= LEN_TX_EXT_MESSAGE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: channel (!)
     * - byte 3: transaction id.
     * - byte 4..17: rawMessage
     * - byte 18: (not used)
     * - byte 19: flags
     */
    buffer[0] = LEN_TX_STD_MESSAGE; // note: LEN_TX_EXT_MESSAGE == LEN_TX_STD_MESSAGE
    buffer[1] = message->xtd ? CMD_TX_EXT_MESSAGE : CMD_TX_STD_MESSAGE;
    buffer[2] = UINT8BYTE(channel);
    buffer[3] = UINT8BYTE(transId);
    /* arguments */
    if (message->xtd) {
        // Extended CAN
        buffer[4] = UINT8BYTE((message->id >> 24) & 0x1F);
        buffer[5] = UINT8BYTE((message->id >> 18) & 0x3F);
        buffer[6] = UINT8BYTE((message->id >> 14) & 0x0F);
        buffer[7] = UINT8BYTE((message->id >>  6) & 0xFF);
        buffer[8] = UINT8BYTE((message->id      ) & 0x3F);
    }
    else {
        // Standard CAN
        buffer[4] = UINT8BYTE((message->id >>  6) & 0x1F);
        buffer[5] = UINT8BYTE((message->id      ) & 0x3F);
    }
    buffer[9] = (uint8_t)MIN(message->dlc, CAN_MAX_DLC);
    memcpy(&buffer[10], message->data, CAN_MAX_LEN);
    uint8_t flags = MSGFLAG_TX;
    flags |= message->rtr ? MSGFLAG_REMOTE_FRAME : 0x00U;
    buffer[18] = UINT8BYTE(0x00);
    buffer[19] = UINT8BYTE(flags);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillFlushQueueReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel, uint8_t flags) {
    assert(buffer);
    assert(maxbyte >= LEN_FLUSH_QUEUE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     * - byte 4: flags
     * - byte 5..7: (not used)
     */
    buffer[0] = LEN_FLUSH_QUEUE;
    buffer[1] = CMD_FLUSH_QUEUE;
    buffer[2] = CMD_FLUSH_QUEUE;
    buffer[3] = UINT8BYTE(channel);
    /* arguments */
    buffer[4] = UINT8BYTE(flags);
    buffer[5] = UINT8BYTE(0x00);
    buffer[6] = UINT8BYTE(0x00);
    buffer[7] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillResetErrorCounterReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_RESET_ERROR_COUNTER);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_RESET_ERROR_COUNTER;
    buffer[1] = CMD_RESET_ERROR_COUNTER;
    buffer[2] = CMD_RESET_ERROR_COUNTER;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillResetStatisticsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_RESET_STATISTICS);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_RESET_STATISTICS;
    buffer[1] = CMD_RESET_STATISTICS;
    buffer[2] = CMD_RESET_STATISTICS;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillReadClockReq(uint8_t *buffer, uint32_t maxbyte, uint8_t flags) {
    assert(buffer);
    assert(maxbyte >= LEN_READ_CLOCK_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: flags
     */
    buffer[0] = LEN_READ_CLOCK_REQ;
    buffer[1] = CMD_READ_CLOCK_REQ;
    buffer[2] = CMD_READ_CLOCK_REQ;
    buffer[3] = UINT8BYTE(flags);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetBusLoadReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_BUSLOAD_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_GET_BUSLOAD_REQ;
    buffer[1] = CMD_GET_BUSLOAD_REQ;
    buffer[2] = CMD_GET_BUSLOAD_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetCardInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t dataLevel) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_CARD_INFO_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: data level (?)
     */
    buffer[0] = LEN_GET_CARD_INFO_REQ;
    buffer[1] = CMD_GET_CARD_INFO_REQ;
    buffer[2] = CMD_GET_CARD_INFO_REQ;
    buffer[3] = UINT8BYTE(dataLevel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetSoftwareInfoReq(uint8_t *buffer, uint32_t maxbyte) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_SOFTWARE_INFO_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: (not used)
     */
    buffer[0] = LEN_GET_SOFTWARE_INFO_REQ;
    buffer[1] = CMD_GET_SOFTWARE_INFO_REQ;
    buffer[2] = CMD_GET_SOFTWARE_INFO_REQ;
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetInterfaceInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_INTERFACE_INFO_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_GET_INTERFACE_INFO_REQ;
    buffer[1] = CMD_GET_INTERFACE_INFO_REQ;
    buffer[2] = CMD_GET_INTERFACE_INFO_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetCapabilitiesReq(uint8_t *buffer, uint32_t maxbyte, uint16_t subCmd, uint16_t subData) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_CAPABILITIES_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2..3: (not used)
     * - byte 4..5: sub-command
     * - byte 6..7: sub-data (not used)
     *        or
     * - byte 6..7: sub-data = channel
     */
    buffer[0] = LEN_GET_CAPABILITIES_REQ;
    buffer[1] = CMD_GET_CAPABILITIES_REQ;
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments (depend on sub-command) */
    buffer[4] = UINT16LSB(subCmd);
    buffer[5] = UINT16MSB(subCmd);
    buffer[6] = UINT16LSB(subData);
    buffer[7] = UINT16MSB(subData);
    /* return request length */
    return (uint32_t)buffer[0];
}

static uint32_t FillGetTransceiverInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= LEN_GET_TRANSCEIVER_INFO_REQ);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: channel
     */
    buffer[0] = LEN_GET_TRANSCEIVER_INFO_REQ;
    buffer[1] = CMD_GET_TRANSCEIVER_INFO_REQ;
    buffer[2] = CMD_GET_TRANSCEIVER_INFO_REQ;
    buffer[3] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)buffer[0];
}

#if (OPTION_PRINT_DEVICE_INFO != 0)
 static void PrintDeviceInfo(const KvaserUSB_DeviceInfo_t *deviceInfo) {
    assert(deviceInfo);
    MACCAN_DEBUG_DRIVER("    - card info:\n");
    MACCAN_DEBUG_DRIVER("      - channel count: %d\n", deviceInfo->card.channelCount);
    MACCAN_DEBUG_DRIVER("      - serial no.: %d\n", deviceInfo->card.serialNumber);
    MACCAN_DEBUG_DRIVER("      - clock resolution: %d\n", deviceInfo->card.clockResolution);
    time_t mfgDate = (time_t)deviceInfo->card.mfgDate;
    MACCAN_DEBUG_DRIVER("      - manufacturing date: %s", ctime(&mfgDate));
    MACCAN_DEBUG_DRIVER("      - EAN code (LSB first): %x%x%x%x%x-%x%x%x%x%x-%x%x%x%x%x-%x\n",
                        (deviceInfo->card.EAN[7] >> 4), (deviceInfo->card.EAN[7] & 0xf),
                        (deviceInfo->card.EAN[6] >> 4), (deviceInfo->card.EAN[6] & 0xf),
                        (deviceInfo->card.EAN[5] >> 4), (deviceInfo->card.EAN[5] & 0xf),
                        (deviceInfo->card.EAN[4] >> 4), (deviceInfo->card.EAN[4] & 0xf),
                        (deviceInfo->card.EAN[3] >> 4), (deviceInfo->card.EAN[3] & 0xf),
                        (deviceInfo->card.EAN[2] >> 4), (deviceInfo->card.EAN[2] & 0xf),
                        (deviceInfo->card.EAN[1] >> 4), (deviceInfo->card.EAN[1] & 0xf),
                        (deviceInfo->card.EAN[0] >> 4), (deviceInfo->card.EAN[0] & 0xf));
    MACCAN_DEBUG_DRIVER("      - hardware revision: %d\n", deviceInfo->card.hwRevision);
    MACCAN_DEBUG_DRIVER("      - USB HS mode: %d\n", deviceInfo->card.usbHsMode);
    MACCAN_DEBUG_DRIVER("      - hardware type: %d\n", deviceInfo->card.hwType);
    MACCAN_DEBUG_DRIVER("      - CAN time-stamp reference: %d\n", deviceInfo->card.canTimeStampRef);
#if (0)
    MACCAN_DEBUG_DRIVER("    - channel info:\n");  // TODO: activate when fixed
    MACCAN_DEBUG_DRIVER("      - channel capabilities: 0x%x\n", deviceInfo->channel.channelCapabilities);
    MACCAN_DEBUG_DRIVER("      - CAN chip type: %d\n", deviceInfo->channel.canChipType);
    MACCAN_DEBUG_DRIVER("      - CAN chip sub-type: %d\n", deviceInfo->channel.canChipSubType);
#endif
    MACCAN_DEBUG_DRIVER("    - software info/details:\n");
    MACCAN_DEBUG_DRIVER("      - software options: 0x%x\n", deviceInfo->software.swOptions);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_CONFIG_MODE: %d\n", (deviceInfo->software.swOptions & SWOPTION_CONFIG_MODE) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_AUTO_TX_BUFFER: %d\n", (deviceInfo->software.swOptions & SWOPTION_AUTO_TX_BUFFER) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_BETA: %d\n", (deviceInfo->software.swOptions & SWOPTION_BETA) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_RC: %d\n", (deviceInfo->software.swOptions & SWOPTION_RC) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_BAD_MOOD: %d\n", (deviceInfo->software.swOptions & SWOPTION_BAD_MOOD) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_XX_MHZ_CLK: %s\n",
                        ((deviceInfo->software.swOptions & SWOPTION_CPU_FQ_MASK) == SWOPTION_16_MHZ_CLK) ? "16" :
                        ((deviceInfo->software.swOptions & SWOPTION_CPU_FQ_MASK) == SWOPTION_32_MHZ_CLK) ? "32" :
                        ((deviceInfo->software.swOptions & SWOPTION_CPU_FQ_MASK) == SWOPTION_24_MHZ_CLK) ? "24" : "XX");
    MACCAN_DEBUG_DRIVER("        - SWOPTION_TIMEOFFSET_VALID: %d\n", (deviceInfo->software.swOptions & SWOPTION_TIMEOFFSET_VALID) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_CAP_REQ: %d\n", (deviceInfo->software.swOptions & SWOPTION_CAP_REQ) ? 1 : 0);
    uint8_t major = (uint8_t)(deviceInfo->software.firmwareVersion >> 24);
    uint8_t minor = (uint8_t)(deviceInfo->software.firmwareVersion >> 16);
    uint16_t build = (uint16_t)(deviceInfo->software.firmwareVersion >> 0);
    MACCAN_DEBUG_DRIVER("      - firmware version: %u.%u (build %u)\n", major, minor, build);
    MACCAN_DEBUG_DRIVER("      - max. outstanding Tx: %d\n", deviceInfo->software.maxOutstandingTx);
    MACCAN_DEBUG_DRIVER("      - max. bit-rate (hydra only, otherwise 1Mbit/s): %d\n", deviceInfo->software.maxBitrate);
    MACCAN_DEBUG_DRIVER("    - transceiver info:\n");
    MACCAN_DEBUG_DRIVER("      - transceiver capabilities: 0x%x\n", deviceInfo->transceiver.transceiverCapabilities);
    MACCAN_DEBUG_DRIVER("      - transceiver status: %d\n", deviceInfo->transceiver.transceiverStatus);
    MACCAN_DEBUG_DRIVER("      - transceiver type: %d\n", deviceInfo->transceiver.transceiverType);
    MACCAN_DEBUG_DRIVER("    - capabilities (if SWOPTION_CAP_REQ):\n");
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_SILENT_MODE: %d\n", deviceInfo->capabilities.silentMode);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_ERRFRAME: %d\n", deviceInfo->capabilities.errorFrame);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_BUS_STATS: %d\n", deviceInfo->capabilities.busStats);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_ERRCOUNT_READ: %d\n", deviceInfo->capabilities.errorCount);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_SINGLE_SHOT: %d\n", deviceInfo->capabilities.singleShot);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_SYNC_TX_FLUSH: %d\n", deviceInfo->capabilities.syncTxFlush);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_LOGGER: %d\n", deviceInfo->capabilities.hasLogger);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_REMOTE: %d\n", deviceInfo->capabilities.hasRemote);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_SCRIPT: %d\n", deviceInfo->capabilities.hasScript);
 }
#endif
