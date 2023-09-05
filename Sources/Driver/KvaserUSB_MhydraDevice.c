/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Interfaces
 *
 *  Copyright (c) 2021-2023 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
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
#include "KvaserUSB_MhydraDevice.h"
#include "KvaserCAN_Devices.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <assert.h>

#include "MacCAN_Debug.h"
#include <inttypes.h>

#ifndef OPTION_PRINT_DEVICE_INFO
#define OPTION_PRINT_DEVICE_INFO  0  /* note: set to non-zero value to print device information */
#endif
#ifndef OPTION_PRINT_BUS_PARAMS
#define OPTION_PRINT_BUS_PARAMS  0  /* note: set to non-zero value to print bus params */
#endif
#ifndef OPTION_CHECK_BUS_PARAMS
#define OPTION_CHECK_BUS_PARAMS  1  /* note set to zero to disable checking of bus params */
#endif
#define ROUTER_HE  0x00U
#define DYNAMIC_HE  ROUTER_HE
#define ILLEGAL_HE  0x3EU
#define MAX_HE_COUNT  64U

#define HYDRA_MAX_CARD_CHANNELS  5U

#define HYDRA_CMD_SIZE  KVASER_HYDRA_COMMAND_LENGTH
#define HYDRA_CMD_EXT_SIZE  KVASER_HYDRA_EXT_COMMAND_LENGTH
#define HYDRA_CMD_RESP_TIMEOUT  KVASER_HYDRA_USB_COMMAND_TIMEOUT

#define SET_DST(x,dst)  (((x)&0xC0U) | ((dst)&0x3FU))
#define SET_SEQ(x,seq)  (((x)&0xF000U) | ((seq)&0xFFFU))

#define MIN(x,y)  (((x) < (y)) ? (x) : (y))

static void ReceptionCallback(void *refCon, UInt8 *buffer, UInt32 size);
static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_Frequency_t frequency);
static bool DecodeMessage(KvaserUSB_CanMessage_t *message, uint8_t *buffer, uint32_t nbyte, KvaserUSB_Frequency_t frequency);

static CANUSB_Return_t MapChannel(KvaserUSB_Device_t *device);
static CANUSB_Return_t SendRequest(KvaserUSB_Device_t *device, const uint8_t *buffer, uint32_t nbyte);
static CANUSB_Return_t ReadResponse(KvaserUSB_Device_t *device, uint8_t *buffer, uint32_t nbyte, uint8_t cmdCode, /*uint8_t transId,*/ uint16_t timeout);

static uint32_t FillMapChannelReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel);
static uint32_t FillMapChannelSysDbgReq(uint8_t *buffer, uint32_t maxbyte);
static uint32_t FillSetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParams_t *params);
static uint32_t FillSetBusParamsFdReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParamsFd_t *params);
/*static uint32_t FillSetBusParamsTqReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParamsTq_t *params);  // TODO: activate when needed */
static uint32_t FillGetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, bool canFd);
static uint32_t FillGetBusParamsTqReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, bool canFd);
static uint32_t FillSetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_DriverMode_t mode);
static uint32_t FillGetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillGetChipStateReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillStartChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillStopChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillResetChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillResetCardReq(uint8_t *buffer, uint32_t maxbyte);
static uint32_t FillTxCanMessageReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint8_t transId, const KvaserUSB_CanMessage_t *message);
static uint32_t FillFlushQueueReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint8_t flags);
static uint32_t FillResetErrorCounterReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillResetStatisticsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillReadClockReq(uint8_t *buffer, uint32_t maxbyte, uint8_t flags);
static uint32_t FillGetBusLoadReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillGetCardInfoReq(uint8_t *buffer, uint32_t maxbyte, int8_t dataLevel);
static uint32_t FillGetSoftwareDetailsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t hydraExt);
static uint32_t FillGetMaxOutstandingTxReq(uint8_t *buffer, uint32_t maxbyte);
static uint32_t FillGetCapabilitiesReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint16_t subCmd/*, uint32_t extraInfo*/);
static uint32_t FillGetTransceiverInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);

static uint8_t Dlc2Len(uint8_t dlc);
/*static uint8_t Len2Dlc(uint8_t len);  // uncomment when needed */
#if (OPTION_PRINT_DEVICE_INFO != 0)
 static void PrintDeviceInfo(const KvaserUSB_DeviceInfo_t *deviceInfo);
#endif

bool Mhydra_ConfigureChannel(KvaserUSB_Device_t *device) {
    /* sanity check */
    if (!device)
        return false;
    if (device->configured)
        return false;
    if (device->handle == CANUSB_INVALID_HANDLE)
        return false;
    if (device->driverType != USB_MHYDRA_DRIVER)
        return false;
    if (device->numChannels != MHYDRA_NUM_CHANNELS)  // TODO: multi-channel devices
        return false;
    if (device->endpoints.numEndpoints != MHYDRA_NUM_ENDPOINTS)
        return false;

    /* set CAN channel properties and defaults */
    device->channelNo = 0U;  /* note: only one CAN channel */
    device->recvData.canClock = KvaserDEV_GetCanClockInMHz(device->productId);
    device->recvData.timerFreq = KvaserDEV_GetTimerFreqInMHz(device->productId);
    device->recvData.txAck.maxMsg = MHYDRA_MAX_OUTSTANDING_TX;

    /* set CAN channel operation capabilities from device spec. */
    device->opCapability = CANMODE_DEFAULT;
    device->opCapability |= KvaserDEV_IsCanFdSupported(device->productId) ? CANMODE_FDOE : 0x00U;
    device->opCapability |= KvaserDEV_IsCanFdSupported(device->productId) ? CANMODE_BRSE : 0x00U;
    // TODO: device->opCapability |= KvaserDEV_IsNonIsoCanFdSupported(device->productId) ? CANMODE_NISO : 0x00U;
    //device->opCapability |= CANMODE_SHRD;  /{ not possible with IOUsbKit }/
    device->opCapability |= CANMODE_NXTD;    /* suppressing extended frames (software solution) */
    device->opCapability |= CANMODE_NRTR;    /* suppressing remote frames (software solution) */
    device->opCapability |= CANMODE_ERR;     /* status frames are always enabled / error frames only with SJA1000 */
    device->opCapability |= KvaserDEV_IsSilentModeSupported(device->productId) ? CANMODE_MON : 0x00U;

    /* initialize Hydra HE address and channel no. */
    device->hydraData.channel2he = ILLEGAL_HE;
    device->hydraData.he2channel = 0xFFU;

    /* Gotcha! */
    device->configured = true;

    return device->configured;
}

CANUSB_Return_t Mhydra_InitializeChannel(KvaserUSB_Device_t *device, const KvaserUSB_OpMode_t opMode) {
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
    /* map channel (It is not clear to me what the hydra is all about.) */
    retVal = MapChannel(device);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): device could not be mapped (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = Mhydra_StopChip(device, HYDRA_CMD_RESP_TIMEOUT);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* set driver mode NORMAL (mode OFF seems not to be supported) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set driver mode NORMAL\n", device->name, device->handle);
    retVal = Mhydra_SetDriverMode(device, DRIVERMODE_NORMAL);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): driver mode could not be set (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* trigger chip state event */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): trigger chip state event\n", device->name, device->handle);
    retVal = Mhydra_RequestChipState(device, 0U);  /* 0 = no delay */
    if (retVal != CANUSB_SUCCESS) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip state event could not be triggered (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* get device information: card, software, interface, transceiver */
    retVal = Mhydra_GetCardInfo(device, &device->deviceInfo.card);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): card information could not be read (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    retVal = Mhydra_GetSoftwareInfo(device, &device->deviceInfo.software);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): firmware information could not be read (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    retVal = Mhydra_GetTransceiverInfo(device, &device->deviceInfo.transceiver);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): transceiver information could not be read (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* get device capabilities (if supported) */
    if (device->deviceInfo.software.swOptions & SWOPTION_CAP_REQ) {
        retVal = Mhydra_GetCapabilities(device, &device->deviceInfo.capabilities);
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
    switch (device->deviceInfo.software.swOptions & SWOPTION_CAN_CLK_MASK) {
        case SWOPTION_80_MHZ_CAN_CLK: device->recvData.canClock = 80U; break;
        case SWOPTION_24_MHZ_CAN_CLK: device->recvData.canClock = 24U; break;
        default:
            device->recvData.canClock = KvaserDEV_GetCanClockInMHz(device->productId);  // FIXME: Leaf Pro has undefine SWOPTION_CAN_CLK_MASK
            break;
    }
    switch (device->deviceInfo.software.swOptions & SWOPTION_CPU_FQ_MASK) {
        case SWOPTION_80_MHZ_CLK: device->recvData.timerFreq = 80U; break;
        case SWOPTION_24_MHZ_CLK: device->recvData.timerFreq = 24U; break;
        default:
            device->recvData.timerFreq = KvaserDEV_GetTimerFreqInMHz(device->productId);
            break;
    }
    /* get reference time (amount of time in seconds and nanoseconds since the Epoch) */
    (void)clock_gettime(CLOCK_REALTIME, &device->recvData.timeRef);  // TODO: not used yet
#if (OPTION_PRINT_DEVICE_INFO != 0)
    MACCAN_DEBUG_DRIVER("    - clocks:\n");
    MACCAN_DEBUG_DRIVER("      - CAN clock: %u MHz\n", device->recvData.canClock);
    MACCAN_DEBUG_DRIVER("      - CAN timer: %u MHz\n", device->recvData.timerFreq);
    /* get device clock (don't care about the result) */
    uint64_t nsec = 0U;
    retVal = Mhydra_ReadClock(device, &nsec);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): device clock could not be read (%i)\n", device->name, device->handle, retVal);
    }
    MACCAN_DEBUG_DRIVER("      - Clock: %u.%04u sec\n", (nsec / 1000000000), ((nsec % 1000000000) / 1000000));
    MACCAN_DEBUG_DRIVER("      - Time: %u.%04u sec\n", device->recvData.timeRef.tv_sec, device->recvData.timeRef.tv_nsec / 1000000);
#endif
    /* get max. outstanding transmit messages */
    if ((0U < device->deviceInfo.software.maxOutstandingTx) &&
        (device->deviceInfo.software.maxOutstandingTx < MIN(MHYDRA_MAX_OUTSTANDING_TX, 255U)))
        device->recvData.txAck.maxMsg = (uint8_t)device->deviceInfo.software.maxOutstandingTx;
    else
        device->recvData.txAck.maxMsg = (uint8_t)MIN(MHYDRA_MAX_OUTSTANDING_TX, 255U);
    /* update capabilities from software options (in case of wrong configuration) */
    device->opCapability &= ~(CANMODE_FDOE | CANMODE_BRSE | CANMODE_NISO);
    device->opCapability |= (device->deviceInfo.software.swOptions & SWOPTION_CANFD_CAP) ? CANMODE_FDOE : 0x00;
    device->opCapability |= (device->deviceInfo.software.swOptions & SWOPTION_CANFD_CAP) ? CANMODE_BRSE : 0x00;
    // TODO: device->opCapability |= (device->deviceInfo.software.swOptions & SWOPTION_NONISO_CAP) ? CANMODE_NISO : 0x00;
    /* check requested operation mode */
    if ((opMode & ~device->opCapability)) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): unsupported operation mode (%02x)\n", device->name, device->handle, (opMode & ~device->opCapability));
        retVal = CANUSB_ERROR_ILLPARA;  /* [2021-05-30]: cf. CAN API V3 function 'can_test' */
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

CANUSB_Return_t Mhydra_TeardownChannel(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    MACCAN_DEBUG_DRIVER("    Teardown %s driver...\n", device->name);
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = Mhydra_StopChip(device, 0U);  /* 0 = don't wait for response */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
        //goto end_exit;
    }
    // TODO: set driver mode OFF resp. NORMAL (or remove it from 'Leaf_TeardownChannel')
//end_exit:
    /* stop the reception loop */
    retVal = KvaserUSB_AbortReception(device);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): reception loop could not be aborted (%i)\n", device->name, device->handle, retVal);
        return retVal;
    }
    /* now we are off :( */
    MACCAN_DEBUG_DRIVER("    Diagnostic data:\n");
    MACCAN_DEBUG_DRIVER("%8"PRIu64" CAN frame(s) written to endpoint\n", device->sendData.msgCounter);
    MACCAN_DEBUG_DRIVER("%8"PRIu64" error(s) while writing to endpoint\n", device->sendData.errCounter);
    MACCAN_DEBUG_DRIVER("%8"PRIu64" CAN frame(s) received and enqueued\n", device->recvData.msgCounter);
    MACCAN_DEBUG_DRIVER("%8"PRIu64" error frame(s) received and encoded\n", device->recvData.stsCounter);
    MACCAN_DEBUG_DRIVER("%8"PRIu64" error event(s) received and recorded\n", device->recvData.errCounter);
    MACCAN_DEBUG_DRIVER("%10.1f%% highest level of the receive queue\n", ((float)CANQUE_QueueHigh(device->recvData.msgQueue) * 100.0) \
                                                                       /  (float)CANQUE_QueueSize(device->recvData.msgQueue));
    MACCAN_DEBUG_DRIVER("%8"PRIu64" overrun event(s) of the receive queue\n", CANQUE_OverflowCounter(device->recvData.msgQueue));
    return retVal;
}

CANUSB_Return_t Mhydra_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

#if (OPTION_PRINT_BUS_PARAMS != 0)
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set bus params - bitRate=%u tseg1=%u tseg2=%u sjw=%u noSamp=%u\n",
                        device->name, device->handle,
                        params->bitRate, params->tseg1, params->tseg2, params->sjw, params->noSamp);
#endif
    /* check bus params (noSamp issue) */
    if (params->noSamp != 1)
        return CANUSB_ERROR_ILLPARA;

    /* send request CMD_SET_BUSPARAMS_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillSetBusParamsReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, params);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_SET_BUSPARAMS_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: driver mode
             * - byte 5: (reserved)
             * - byte 6..31: (not used)
             */
            // TODO: handle driver mode
        }
    }
#if (OPTION_CHECK_BUS_PARAMS != 0)
    /* read back bus params and compare with written bus params */
    if (retVal == CANUSB_SUCCESS) {
        KvaserUSB_BusParams_t actual;
        retVal = Mhydra_GetBusParams(device, &actual);
        if (retVal == CANUSB_SUCCESS) {
            if ((actual.bitRate != params->bitRate) ||
                (actual.tseg1 != params->tseg1) ||
                (actual.tseg2 != params->tseg2) ||
                (actual.sjw != params->sjw ) ||
                (actual.noSamp != params->noSamp))
                retVal = CANUSB_ERROR_ILLPARA;
        }
    }
#endif
    return retVal;
}

CANUSB_Return_t Mhydra_SetBusParamsFd(KvaserUSB_Device_t *device, const KvaserUSB_BusParamsFd_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

#if (OPTION_PRINT_BUS_PARAMS != 0)
    if (params->canFd)
        MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set bus params CAN FD - bitRate=%u tseg1=%u tseg2=%u sjw=%u noSamp=%u"
                                                                      " : bitRate=%u tseg1=%u tseg2=%u sjw=%u noSamp=%u\n",
                            device->name, device->handle,
                            params->nominal.bitRate, params->nominal.tseg1, params->nominal.tseg2, params->nominal.sjw, params->nominal.noSamp,
                            params->data.bitRate, params->data.tseg1, params->data.tseg2, params->data.sjw, params->data.noSamp);
    else
        MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set bus params CAN FD - bitRate=%u tseg1=%u tseg2=%u sjw=%u noSamp=%u\n",
                            device->name, device->handle,
                            params->nominal.bitRate, params->nominal.tseg1, params->nominal.tseg2, params->nominal.sjw, params->nominal.noSamp);
#endif
    /* check bus params (noSamp issue) */
    if (params->nominal.noSamp != 1)
        return CANUSB_ERROR_ILLPARA;
    if (params->canFd && (params->data.noSamp != 1))
        return CANUSB_ERROR_ILLPARA;

    /* send request CMD_SET_BUSPARAMS_FD_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillSetBusParamsFdReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, params);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_SET_BUSPARAMS_FD_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: driver mode
             * - byte 5: (reserved)
             * - byte 6..31: (not used)
             */
            // TODO: handle driver mode
        }
    }
#if (OPTION_CHECK_BUS_PARAMS != 0)
    /* read back bus params and compare with written bus params */
    if (retVal == CANUSB_SUCCESS) {
        KvaserUSB_BusParamsFd_t actual;
        retVal = Mhydra_GetBusParamsFd(device, &actual);
        if (retVal == CANUSB_SUCCESS) {
            if ((actual.nominal.bitRate != params->nominal.bitRate) ||
                (actual.nominal.tseg1 != params->nominal.tseg1) ||
                (actual.nominal.tseg2 != params->nominal.tseg2) ||
                (actual.nominal.sjw != params->nominal.sjw ) ||
                (actual.nominal.noSamp != params->nominal.noSamp))
                retVal = CANUSB_ERROR_ILLPARA;
            if (params->canFd && ((actual.data.bitRate != params->data.bitRate) ||
                                  (actual.data.tseg1 != params->data.tseg1) ||
                                  (actual.data.tseg2 != params->data.tseg2) ||
                                  (actual.data.sjw != params->data.sjw ) ||
                                  (actual.data.noSamp != params->data.noSamp)))
                retVal = CANUSB_ERROR_ILLPARA;

        }
    }
#endif
    return retVal;
}

#ifdef BUS_PARAMS_TQ  // TODO: activate when required
CANUSB_Return_t Mhydra_SetBusParamsTq(KvaserUSB_Device_t *device, const KvaserUSB_BusParamsTq_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_SET_BUSPARAMS_TQ_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillSetBusParamsTqReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, params);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_SET_BUSPARAMS_TQ_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: status
             * - byte 5: (reserved)
             * - byte 6..31: (not used)
             */
            // TODO: handle status
#if (OPTION_PRINT_BUS_PARAMS != 0)
            if (params->canFd)
                MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set bus params TQ - prop=%u phase1=%u phase2=%u sjw=%u brp=%u"
                                                                          " : prop=%u phase1=%u phase2=%u sjw=%u brp=%u\n",
                                    device->name, device->handle,
                                    params->arbitration.prop, params->arbitration.phase1, params->arbitration.phase2, params->arbitration.sjw, params->arbitration.brp,
                                    params->data.prop, params->data.phase1, params->data.phase2, params->data.sjw, params->data.brp);
            else
                MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set bus params TQ - prop=%u phase1=%u phase2=%u sjw=%u brp=%u\n",
                                    device->name, device->handle,
                                    params->arbitration.prop, params->arbitration.phase1, params->arbitration.phase2, params->arbitration.sjw, params->arbitration.brp);
#endif
        }
    }
    return retVal;
}
#endif

CANUSB_Return_t Mhydra_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_BUSPARAMS_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetBusParamsReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, false);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_BUSPARAMS_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..7: bitRate
             * - byte 8: tseg1
             * - byte 9: tseg2
             * - byte 10: sjw
             * - byte 11: noSamp
             * - byte 12..31: (not used)
             */
            params->bitRate = BUF2UINT32(buffer[4]);
            params->tseg1 = BUF2UINT8(buffer[8]);
            params->tseg2 = BUF2UINT8(buffer[9]);
            params->sjw = BUF2UINT8(buffer[10]);
            params->noSamp = BUF2UINT8(buffer[11]);
#if (OPTION_PRINT_BUS_PARAMS != 0)
            MACCAN_DEBUG_DRIVER(">>> %s (device #%u): get bus params - bitRate=%u tseg1=%u tseg2=%u sjw=%u noSamp=%u\n", device->name, device->handle,
                                params->bitRate, params->tseg1, params->tseg2, params->sjw, params->noSamp);
#endif
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_GetBusParamsFd(KvaserUSB_Device_t *device, KvaserUSB_BusParamsFd_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_BUSPARAMS_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetBusParamsReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, false);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_BUSPARAMS_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..7: bitRate
             * - byte 8: tseg1
             * - byte 9: tseg2
             * - byte 10: sjw
             * - byte 11: noSamp
             * - byte 12..31: (not used)
             */
            params->nominal.bitRate = BUF2UINT32(buffer[4]);
            params->nominal.tseg1 = BUF2UINT8(buffer[8]);
            params->nominal.tseg2 = BUF2UINT8(buffer[9]);
            params->nominal.sjw = BUF2UINT8(buffer[10]);
            params->nominal.noSamp = BUF2UINT8(buffer[11]);

            /* send request CMD_GET_BUSPARAMS_REQ (CAN FD) and wait for response */
            bzero(buffer, HYDRA_CMD_SIZE);
            size = FillGetBusParamsReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, true);
            retVal = SendRequest(device, buffer, size);
            if (retVal == CANUSB_SUCCESS) {
                size = HYDRA_CMD_SIZE;
                resp = CMD_GET_BUSPARAMS_RESP;
                retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
                if (retVal == CANUSB_SUCCESS) {
                    /* command response:
                     * - byte 0: command code
                     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
                     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
                     * - byte 4..7: bitRate
                     * - byte 8: tseg1
                     * - byte 9: tseg2
                     * - byte 10: sjw
                     * - byte 11: noSamp
                     * - byte 12..31: (not used)
                     */
                    params->data.bitRate = BUF2UINT32(buffer[4]);
                    params->data.tseg1 = BUF2UINT8(buffer[8]);
                    params->data.tseg2 = BUF2UINT8(buffer[9]);
                    params->data.sjw = BUF2UINT8(buffer[10]);
                    params->data.noSamp = BUF2UINT8(buffer[11]);
#if (OPTION_PRINT_BUS_PARAMS != 0)
                    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): get bus params CAN FD - bitRate=%u tseg1=%u tseg2=%u sjw=%u noSamp=%u"
                                                                                  " : bitRate=%u tseg1=%u tseg2=%u sjw=%u noSamp=%u\n",
                                        device->name, device->handle,
                                        params->nominal.bitRate, params->nominal.tseg1, params->nominal.tseg2, params->nominal.sjw, params->nominal.noSamp,
                                        params->data.bitRate, params->data.tseg1, params->data.tseg2, params->data.sjw, params->data.noSamp);
#endif
                }
            }
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_GetBusParamsTq(KvaserUSB_Device_t *device, KvaserUSB_BusParamsTq_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_BUSPARAMS_TQ_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetBusParamsTqReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, true);  // FIXME: canFd ? true : false
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_BUSPARAMS_TQ_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..5: propagation segment (arbitration phase)
             * - byte 6..7: phase 1 segment (arbitration phase)
             * - byte 8..9: phase 2 segment (arbitration phase)
             * - byte 10..11: synchronous jump width (arbitration phase)
             * - byte 12..13: bit-rate prescaler (arbitration phase)
             * - byte 14..15: propagation segment (CAN FD data phase)
             * - byte 16..17: phase 1 segment (CAN FD data phase)
             * - byte 18..19: phase 2 segment (CAN FD data phase)
             * - byte 20..21: synchronous jump width (CAN FD data phase)
             * - byte 22..23: bit-rate prescaler (CAN FD data phase)
             * - byte 24: open as CAN FD
             * - byte 25: status
             * - byte 26..31: (not used)
             */
            params->arbitration.prop = BUF2UINT16(buffer[4]);
            params->arbitration.phase1 = BUF2UINT16(buffer[6]);
            params->arbitration.phase2 = BUF2UINT16(buffer[8]);
            params->arbitration.sjw = BUF2UINT16(buffer[10]);
            params->arbitration.brp = BUF2UINT16(buffer[12]);
            params->data.prop = BUF2UINT16(buffer[14]);
            params->data.phase1 = BUF2UINT16(buffer[16]);
            params->data.phase2 = BUF2UINT16(buffer[18]);
            params->data.sjw = BUF2UINT16(buffer[20]);
            params->data.brp = BUF2UINT16(buffer[22]);
            params->canFd = buffer[24] ? true : false;
            // TODO: handle status
#if (OPTION_PRINT_BUS_PARAMS != 0)
            if (params->canFd)
                MACCAN_DEBUG_DRIVER(">>> %s (device #%u): get bus params TQ - prop=%u phase1=%u phase2=%u sjw=%u brp=%u"
                                                                          " : prop=%u phase1=%u phase2=%u sjw=%u brp=%u\n",
                                    device->name, device->handle,
                                    params->arbitration.prop, params->arbitration.phase1, params->arbitration.phase2, params->arbitration.sjw, params->arbitration.brp,
                                    params->data.prop, params->data.phase1, params->data.phase2, params->data.sjw, params->data.brp);
            else
                MACCAN_DEBUG_DRIVER(">>> %s (device #%u): get bus params TQ - prop=%u phase1=%u phase2=%u sjw=%u brp=%u\n",
                                    device->name, device->handle,
                                    params->arbitration.prop, params->arbitration.phase1, params->arbitration.phase2, params->arbitration.sjw, params->arbitration.brp);
#endif
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_SetDriverMode(KvaserUSB_Device_t *device, const KvaserUSB_DriverMode_t mode) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_SET_DRIVERMODE_REQ w/o response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillSetDriverModeReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, mode);
    retVal = SendRequest(device, buffer, size);

    return retVal;
}

CANUSB_Return_t Mhydra_GetDriverMode(KvaserUSB_Device_t *device, KvaserUSB_DriverMode_t *mode) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !mode)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_DRIVERMODE_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetDriverModeReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_DRIVERMODE_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: driver mode
             * - byte 5: (reserved)
             * - byte 6..31: (not used)
             */
            *mode = BUF2UINT8(buffer[4]);
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_StartChip(KvaserUSB_Device_t *device, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;
    (void)timeout;  // FIXME: why isn't it used as in 'Mhydra_StopChip'

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_START_CHIP_RESP and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillStartChipReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_START_CHIP_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: (reserved)
             * - byte 5..31: (not used)
             */
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_StopChip(KvaserUSB_Device_t *device, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_STOP_CHIP_REQ and wait for response (optional) */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillStopChipReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (timeout > 0U)) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_STOP_CHIP_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: (reserved)
             * - byte 5..31: (not used)
             */
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_ResetChip(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_CHIP_REQ w/o response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillResetChipReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Mhydra_ResetCard(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_CARD_REQ w/o response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillResetCardReq(buffer, HYDRA_CMD_SIZE);
    retVal = SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Mhydra_RequestChipState(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_CHIP_STATE_REQ w/o response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetChipStateReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Mhydra_SendMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_EXT_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !message)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* refuse certain CAN messages depending on the operation mode */
    if (message->xtd && (device->recvData.opMode & CANMODE_NXTD))
        return CANUSB_ERROR_ILLPARA;
    if (message->rtr && (device->recvData.opMode & CANMODE_NRTR))
        return CANUSB_ERROR_ILLPARA;
    if (message->fdf && !(device->recvData.opMode & CANMODE_FDOE))
        return CANUSB_ERROR_ILLPARA;
    if (message->brs && !(device->recvData.opMode & CANMODE_BRSE))
        return CANUSB_ERROR_ILLPARA;
    if (message->brs && !message->fdf)
        return CANUSB_ERROR_ILLPARA;
    if (message->sts)  /* note: error frames cannot be sent */
        return CANUSB_ERROR_ILLPARA;

    /* check for pending transmit messages */
    if (device->recvData.txAck.cntMsg < device->recvData.txAck.maxMsg)
        device->recvData.txAck.transId = (device->recvData.txAck.transId + 1U) % device->recvData.txAck.maxMsg;
    else
        return CANUSB_ERROR_BUSY;

    /* channel no. and transaction id */
    uint8_t channel = device->hydraData.channel2he;
    uint8_t transId = device->recvData.txAck.transId;

    /* send request CMD_EXTENDED[CMD_TX_CAN_MESSAGE_FD] and wait for ackknowledge (optional) */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillTxCanMessageReq(buffer, HYDRA_CMD_EXT_SIZE, channel, transId, message);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_EXTENDED;
        if (timeout > 0U) {
            /* expect a response (timed or blocking write) */
            device->recvData.txAck.noAck = false;
            retVal = ReadResponse(device, buffer, size, resp, timeout);
            /* note: the transaction id. is ckecked by the callback routine
             */
            if (retVal == CANUSB_SUCCESS) {
                /* extended command response:
                 * - byte 0: command code
                 * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
                 * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
                 * - byte 4..5: command length (32..max. 96 bytes, we expect 32 bytes)
                 * - byte 6: extended command code
                 * - byte 7: (reserved)
                 * - byte 8..11: flags
                 * - byte 12..15: (padding)
                 * - byte 16..23: FPGA time-stamp (64-bit)
                 * - byte 24..31: (not used)
                 */
                if (buffer[6] == CMD_TX_ACKNOWLEDGE_FD) {
                    // TODO: what to do with them?
                } else
                    retVal = CANUSB_ERROR_FATAL;  // TODO: meaningful error code
            }
        } else {
            /* skip the response in the callback routine */
            device->recvData.txAck.noAck = true;
        }
        /* one more transmit message pending*/
        device->recvData.txAck.cntMsg++;
    }
    /* counting */
    if (retVal == CANUSB_SUCCESS)
        device->sendData.msgCounter++;
    else
        device->sendData.errCounter++;
    return retVal;
}

CANUSB_Return_t Mhydra_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout) {
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

CANUSB_Return_t Mhydra_FlushQueue(KvaserUSB_Device_t *device/*, uint8_t flags*/) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;
    uint8_t flags = 0x00U;  // TODO: symbolic constants

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_FLUSH_QUEUE_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillFlushQueueReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, flags);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_FLUSH_QUEUE_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..7: flags
             * - byte 8..31: (not used)
             */
             /* flags = BUF2UINT32(buffer[4]); */
            // TODO: what to do with them
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_ResetErrorCounter(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_ERROR_COUNTER w/o response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillResetErrorCounterReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Mhydra_ResetStatistics(KvaserUSB_Device_t *device, uint16_t delay) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_RESET_STATISTICS w/o response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillResetStatisticsReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t Mhydra_ReadClock(KvaserUSB_Device_t *device, uint64_t *nsec) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !nsec)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_READ_CLOCK_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillReadClockReq(buffer, HYDRA_CMD_SIZE, 0x00U);  // TODO: flags
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_READ_CLOCK_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..9: time (48-bit)
             * - byte 10..31: (not used)
             */
            uint64_t ticks = 0ULL;
            ticks |= (uint64_t)BUF2UINT16(buffer[4]) << 0;
            ticks |= (uint64_t)BUF2UINT16(buffer[6]) << 16;
            ticks |= (uint64_t)BUF2UINT16(buffer[8]) << 32;
            /* note: when software options not read before, assume clock running at 80MHz
             */
            if (device->recvData.timerFreq == 0U)
                device->recvData.timerFreq = KvaserDEV_GetTimerFreqInMHz(device->productId);
            *nsec = KvaserUSB_NanosecondsFromTicks(ticks, device->recvData.timerFreq);
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !load)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_BUSLOAD_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetBusLoadReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_BUSLOAD_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..9: time-stamp (absolute time)
             * - byte 10+11: sampling interval (in [usec])
             * - byte 12+13: number of samples where tx or rx was active
             * - byte 14+15: milliseconds since last response
             * - byte 16..31: (not used)
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

CANUSB_Return_t Mhydra_GetCardInfo(KvaserUSB_Device_t *device, KvaserUSB_CardInfo_t *info/*, int8_t dataLevel*/) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_CARD_INFO_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetCardInfoReq(buffer, HYDRA_CMD_SIZE, /*dataLevel*/0);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_CARD_INFO_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..7: serialNumber
             * - byte 8..11: clockResolution
             * - byte 12..15: mfgDate
             * - byte 16..23: EAN[8]
             * - byte 24: hwRevision
             * - byte 25: usbHsMode
             * - byte 26: hwType
             * - byte 27: canTimeStampRef
             * - byte 28: channelCount
             * - byte 29..31: (not used)
             */
            info->serialNumber = BUF2UINT32(buffer[4]);
            info->clockResolution = BUF2UINT32(buffer[8]);
            info->mfgDate = BUF2UINT32(buffer[12]);
            info->EAN[0] = BUF2UINT8(buffer[16]);
            info->EAN[1] = BUF2UINT8(buffer[17]);
            info->EAN[2] = BUF2UINT8(buffer[28]);
            info->EAN[3] = BUF2UINT8(buffer[29]);
            info->EAN[4] = BUF2UINT8(buffer[20]);
            info->EAN[5] = BUF2UINT8(buffer[21]);
            info->EAN[6] = BUF2UINT8(buffer[22]);
            info->EAN[7] = BUF2UINT8(buffer[23]);
            info->hwRevision = BUF2UINT8(buffer[24]);
            info->usbHsMode = BUF2UINT8(buffer[25]);
            info->hwType = BUF2UINT8(buffer[26]);
            info->canTimeStampRef = BUF2UINT8(buffer[27]);
            info->channelCount = BUF2UINT8(buffer[28]);
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_GetSoftwareInfo(KvaserUSB_Device_t *device, KvaserUSB_SoftwareInfo_t *info/*, uint8_t hydraExt*/) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_SOFTWARE_DETAILS_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetSoftwareDetailsReq(buffer, HYDRA_CMD_SIZE, /*hydraExt*/1);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_SOFTWARE_DETAILS_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..7: swOptions
             * - byte 8..11: swVersion
             * - byte 12..15: swName
             * - byte 16..23: EAN code (LSB first)
             * - byte 24..27: maxBitrate
             * - byte 28..31: (not used)
             */
            info->swOptions = BUF2UINT32(buffer[4]);
            info->firmwareVersion = BUF2UINT32(buffer[8]);
            info->swName = BUF2UINT32(buffer[12]);
            info->EAN[0] = BUF2UINT8(buffer[16]);
            info->EAN[1] = BUF2UINT8(buffer[17]);
            info->EAN[2] = BUF2UINT8(buffer[18]);
            info->EAN[3] = BUF2UINT8(buffer[19]);
            info->EAN[4] = BUF2UINT8(buffer[20]);
            info->EAN[5] = BUF2UINT8(buffer[21]);
            info->EAN[6] = BUF2UINT8(buffer[22]);
            info->EAN[7] = BUF2UINT8(buffer[23]);
            info->maxBitrate = BUF2UINT32(buffer[24]);

            /* send request CMD_GET_SOFTWARE_INFO_REQ and wait for response */
            bzero(buffer, HYDRA_CMD_SIZE);
            size = FillGetMaxOutstandingTxReq(buffer, HYDRA_CMD_SIZE);
            retVal = SendRequest(device, buffer, size);
            if (retVal == CANUSB_SUCCESS) {
                size = HYDRA_CMD_SIZE;
                resp = CMD_GET_SOFTWARE_INFO_RESP;
                retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
                if (retVal == CANUSB_SUCCESS) {
                    /* command response:
                     * - byte 0: command code
                     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
                     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
                     * - byte 4..7: (reserved)
                     * - byte 8..11: (reserved)
                     * - byte 12..13: maxOutstandingTx
                     * - byte 14..31: (not used)
                     */
                    info->maxOutstandingTx = BUF2UINT16(buffer[12]);
                }
            }
        }
    }
    return retVal;
}

#ifdef INTERFACE_INFO  // TODO: remove this (not implemented by any device)
CANUSB_Return_t Mhydra_GetInterfaceInfo(KvaserUSB_Device_t *device, KvaserUSB_InterfaceInfo_t *info) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* note: this command seem not to work on the Leaf Pro HS v2 device!
     */
    /* send request CMD_GET_INTERFACE_INFO_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetInterfaceInfoReq(buffer, HYDRA_CMD_SIZE);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_INTERFACE_INFO_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0..3: (header)
             * - byte 4..7: channelCapabilities
             * - byte 8: canChipType
             * - byte 9: canChipSubType
             * - byte 10..31: (not used)
             */
            info->channelCapabilities = BUF2UINT32(buffer[4]);
            info->canChipType = BUF2UINT8(buffer[8]);
            info->canChipSubType = BUF2UINT8(buffer[9]);
        }
    }
    return retVal;
}
#endif

CANUSB_Return_t Mhydra_GetCapabilities(KvaserUSB_Device_t *device, KvaserUSB_Capabilities_t *capabilities) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
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
    capabilities->errorGen = KvaserDEV_IsErrorFrameSupported(device->productId);
    capabilities->busStats = 0;
    capabilities->errorCount = 0;
    capabilities->singleShot = 0;
    capabilities->syncTxFlush = 0;
    capabilities->hasLogger = 0;
    capabilities->hasRemote = 0;
    capabilities->hasScript = 0;
    capabilities->linHybrid = 0;
    capabilities->kdiInfo = 0;
    capabilities->hasKdi = 0;
    capabilities->hasIoApi = 0;
    capabilities->hasTimeQuanta = 0;

    /* sub-commands */
    uint16_t subCmds[14] = {
        CAP_SUB_CMD_SILENT_MODE,
        CAP_SUB_CMD_ERRFRAME,
        CAP_SUB_CMD_BUS_STATS,
        CAP_SUB_CMD_ERRCOUNT_READ,
        CAP_SUB_CMD_SINGLE_SHOT,
        CAP_SUB_CMD_SYNC_TX_FLUSH,
        CAP_SUB_CMD_HAS_LOGGER,
        CAP_SUB_CMD_HAS_REMOTE,
        CAP_SUB_CMD_HAS_SCRIPT,
        CAP_SUB_CMD_LIN_HYBRID,
        CAP_SUB_CMD_KDI_INFO,
        CAP_SUB_CMD_HAS_KDI,
        CAP_SUB_CMD_HAS_IO_API,
        CAP_SUB_CMD_HAS_BUSPARAMS_TQ
    };
    /* send request CMD_GET_CAPABILITIES_REQ w/ sub-command and wait for response */
    for (int i = 0; i < 14; i++) {
        bzero(buffer, HYDRA_CMD_SIZE);
        size = FillGetCapabilitiesReq(buffer, HYDRA_CMD_SIZE, device->hydraData.sysdbg_he, subCmds[i]);
        retVal = SendRequest(device, buffer, size);
        if (retVal == CANUSB_SUCCESS) {
            size = HYDRA_CMD_SIZE;
            resp = CMD_GET_CAPABILITIES_RESP;
            retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
            if (retVal == CANUSB_SUCCESS) {
                /* command response:
                 * - byte 0..3: (header)
                 * - byte 4..5: sub-command
                 * - byte 6..7: status (0=OK, 1=NOT_IMPLEMENTED, 2=UNAVAILABLE)
                 * - byte 8..31: depend on sub-command
                 *   for mask & value responses:
                 * - byte 8..11: mask (bit 0 = CAN1, bit 1 = CAN2, etc.)
                 * - byte 12..15: value (bit 0 = CAN1, bit 1 = CAN2, etc.)
                 */
                uint16_t status = BUF2UINT16(buffer[6]);
                uint32_t mask = BUF2UINT32(buffer[8]);
                uint32_t value = BUF2UINT32(buffer[12]);
                uint32_t channel = (uint32_t)0x1U << device->channelNo;
                if (status == 0) {
                    switch (subCmds[i]) {
                        case CAP_SUB_CMD_SILENT_MODE: capabilities->silentMode = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_ERRFRAME: capabilities->errorGen = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_BUS_STATS: capabilities->busStats = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_ERRCOUNT_READ: capabilities->errorCount = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_SINGLE_SHOT: capabilities->singleShot = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_SYNC_TX_FLUSH: capabilities->syncTxFlush= ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_LOGGER: capabilities->hasLogger = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_REMOTE: capabilities->hasRemote = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_SCRIPT: capabilities->hasScript = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_LIN_HYBRID: capabilities->linHybrid = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_KDI_INFO: capabilities->kdiInfo = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_KDI: capabilities->hasKdi = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_IO_API: capabilities->hasIoApi = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        case CAP_SUB_CMD_HAS_BUSPARAMS_TQ: capabilities->hasTimeQuanta = ((mask & channel) && (value & channel)) ? 1 : 0; break;
                        default: /* nothing to do here */ break;
                    }
                }
                // TODO: decode other stuff if needed (i.e. loggerType, hwStatus, remoteInfo etc.)
            }
        }
    }
    return retVal;
}

CANUSB_Return_t Mhydra_GetTransceiverInfo(KvaserUSB_Device_t *device, KvaserUSB_TransceiverInfo_t *info) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !info)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_GET_TRANSCEIVER_INFO_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillGetTransceiverInfoReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_TRANSCEIVER_INFO_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..7: transceiver capabilities
             * - byte 8: transceiver status
             * - byte 9: transceiver type
             * - byte 10..31: (not used)
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

    /* note: Hydra devices sometimes split a response into two URB packets.
     *       We store the remainder of the first one in a retention buffer.
     */
    KvaserUSB_HydraBuffer_t *hydra = &context->hydraBuf;
    memcpy(&hydra->buffer[hydra->length], buffer, (size_t)size);  // TODO: array boundaries
    hydra->length += size;

    /* Hydra USB response:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..31: (a hydra command is at least 32 bytes long)
     * - byte 32...: (extended commands are longer, max. 96 bytes)
     * note: the total length of an extended command respones is encoded in
     * - byte 4..5: command length (32..max. 96 bytes)
     */
    if (hydra->length >= HYDRA_CMD_SIZE) {
        /* the "command/message pump" */
        while (index < hydra->length) {
            /* get the command length */
            if (hydra->buffer[index] != CMD_EXTENDED)
                nbyte = (UInt32)HYDRA_CMD_SIZE;
            else
                nbyte = (UInt32)BUF2UINT16(hydra->buffer[4]);
            if ((index + nbyte) > hydra->length) {
                /* not enough bytes received (splitted response) */
                MACCAN_LOG_WRITE(&hydra->buffer[index], hydra->length - index, "%");
                break;
            }
            MACCAN_LOG_WRITE(&hydra->buffer[index], nbyte, index ? "+" : "<");
            /* interpret the command code */
            switch (hydra->buffer[index]) {
                case CMD_CHIP_STATE_EVENT:
                case CMD_ERROR_EVENT:
                case CMD_CAN_ERROR_EVENT:
                    /* event message: update event status */
                    (void)UpdateEventData(&context->evData, &hydra->buffer[index], nbyte, context->timerFreq);
                    /* on error event: write packet into the pipe */
                    if (CMD_ERROR_EVENT == hydra->buffer[index]) {
                        (void)CANPIP_Write(context->msgPipe, &hydra->buffer[index], nbyte);
                    }
                    context->errCounter++;
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
                case CMD_FLUSH_QUEUE_RESP:
                case CMD_SET_BUSPARAMS_FD_RESP:
                case CMD_SET_BUSPARAMS_RESP:
                case CMD_GET_CAPABILITIES_RESP:
                case CMD_GET_TRANSCEIVER_INFO_RESP:
                case CMD_GET_BUSPARAMS_TQ_RESP:
                case CMD_SET_BUSPARAMS_TQ_RESP:
                case CMD_MAP_CHANNEL_RESP:
                case CMD_GET_SOFTWARE_DETAILS_RESP:
                    /* command response: write packet into the pipe */
                    (void)CANPIP_Write(context->msgPipe, &hydra->buffer[index], nbyte);
                    break;
                case CMD_EXTENDED:
                    switch (hydra->buffer[6]) {
                        case CMD_EXT_RX_MSG_FD:
                            /* received CAN message: decode and enqueue */
                            if (DecodeMessage(&message, &hydra->buffer[index], nbyte, context->timerFreq)) {
                                /* suppress certain CAN messages depending on the operation mode */
                                if (message.xtd && (context->opMode & CANMODE_NXTD))
                                    break;
                                if (message.rtr && (context->opMode & CANMODE_NRTR))
                                    break;
                                if (message.sts && !(context->opMode & CANMODE_ERR))
                                    break;
                                if (CANQUE_Enqueue(context->msgQueue, (void*)&message) == CANUSB_SUCCESS) {
                                    if (!message.sts)
                                        context->msgCounter++;
                                    else
                                        context->stsCounter++;
                                }
                            } else {
                                /* there are flags that do not belong to a received CAN message */
                                (void)UpdateEventData(&context->evData, &hydra->buffer[index], nbyte, context->timerFreq);
                            }
                            break;
                        case CMD_EXT_TX_ACK_FD:
                            /* transmit ackowledgement: write packet into the pipe only when requested */
                            if (!context->txAck.noAck && (hydra->buffer[index+2] == context->txAck.transId))
                                (void)CANPIP_Write(context->msgPipe, &hydra->buffer[index], nbyte);
                            if (context->txAck.cntMsg > 0)
                                context->txAck.cntMsg--;
                            break;
                        default:
                            /* there are not others */
                            break;
                    }
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
        MACCAN_LOG_WRITE(hydra->buffer, hydra->length, "?");
    }
    /* move remaining bytes to the beginning of the retention buffer */
    if (index < hydra->length) {
        memmove(&hydra->buffer[0], &hydra->buffer[index], (size_t)(hydra->length - index));  // TODO: array boundaries
        hydra->length -= index;
    } else
        hydra->length = 0U;
}

static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_Frequency_t frequency) {
    uint32_t flags = 0U;
    bool result = false;

    if (!event || !buffer)
        return false;
    if (nbyte < HYDRA_CMD_SIZE)
        return false;

    (void)frequency;  // currently not used

    /* Hydra USB response:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..31: (a hydra command is at least 32 bytes long)
     * - byte 32...: (extended commands are longer, max. 96 bytes)
     * note: the total length of an extended command respones is encoded in
     * - byte 4..5: command length (32..max. 96 bytes)
     */
    switch (buffer[0]) {
        case CMD_ERROR_EVENT:
            /* command response:
             * - byte 0..3: (Hydra's head)
             * - byte 4..9: time
             * - byte 10: (not used)
             * - byte 11: error code
             * - byte 12..13: additional information 1
             * - byte 14..15: additional information 2
             */
            event->errorEvent.time[0] = BUF2UINT16(buffer[4]);
            event->errorEvent.time[1] = BUF2UINT16(buffer[6]);
            event->errorEvent.time[2] = BUF2UINT16(buffer[8]);
            event->errorEvent._reserved = BUF2UINT16(buffer[10]);
            event->errorEvent.errorCode = BUF2UINT8(buffer[11]);
            event->errorEvent.addInfo1 = BUF2UINT16(buffer[12]);
            event->errorEvent.addInfo2 = BUF2UINT16(buffer[14]);
            MACCAN_LOG_PRINTF("! error code: %02X\n", event->errorEvent.errorCode);
            result = true;
            break;
        case CMD_CAN_ERROR_EVENT:
            /* command response:
             * - byte 0..3: (Hydra's head)
             * - byte 4..9: time
             * - byte 10: flags
             * - byte 11: (not used)
             * - byte 12: tx error counter
             * - byte 13: rx error counter
             * - byte 14: bus status
             * - byte 15: error factor
             */
            event->canError.channel = 0U;  // TODO: get it from header
            event->canError.time[0] = BUF2UINT16(buffer[4]);
            event->canError.time[1] = BUF2UINT16(buffer[6]);
            event->canError.time[2] = BUF2UINT16(buffer[8]);
            event->canError.flags = BUF2UINT8(buffer[10]);
            event->canError._reserved = BUF2UINT8(buffer[11]);
            event->canError.txErrorCounter = BUF2UINT8(buffer[12]);
            event->canError.rxErrorCounter = BUF2UINT8(buffer[13]);
            event->canError.busStatus = BUF2UINT8(buffer[14]);
            event->canError.errorFactor = BUF2UINT8(buffer[15]);
            MACCAN_LOG_PRINTF("! CAN error: %02X (flags=%02X)\n", event->canError.busStatus, event->canError.flags);
            result = true;
            break;
        case CMD_CHIP_STATE_EVENT:
            /* command response:
             * - byte 0..3: (Hydra's head)
             * - byte 4..9: time
             * - byte 10: tx error counter
             * - byte 11: rx error counter
             * - byte 12: bus status
             * - byte 13..15: (not used)
             */
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
            break;
        case CMD_EXTENDED:
            /* command response:
             * - byte 0..3: (Hydra's head)
             * - byte 4..5: command length (32..max. 96 bytes)
             * - byte 6: extended command code
             * - byte 7: (reserved)
             * - byte 8...: (extended command response)
             */
            if (buffer[6] == CMD_EXT_RX_MSG_FD) {
                /* received CAN message:
                 * - byte 0..3: (Hydra's head)
                 * - byte 4..5: (header)
                 * - byte 8..11: flags
                 * - byte 12..n: (irrelevant)
                 */
                flags = BUF2UINT32(buffer[8]);
                MACCAN_LOG_PRINTF("! Logged flags:");
                if ((flags & MSGFLAG_ERROR_FRAME))
                    MACCAN_LOG_PRINTF(" MSGFLAG_ERROR_FRAME");
                if ((flags & MSGFLAG_OVERRUN))
                    MACCAN_LOG_PRINTF(" MSGFLAG_OVERRUN");
                if ((flags & MSGFLAG_NERR))
                    MACCAN_LOG_PRINTF(" MSGFLAG_NERR");
                if ((flags & MSGFLAG_WAKEUP))
                    MACCAN_LOG_PRINTF(" MSGFLAG_WAKEUP");
                if ((flags & MSGFLAG_TX))
                    MACCAN_LOG_PRINTF(" MSGFLAG_TX");
                if ((flags & MSGFLAG_TXRQ))
                    MACCAN_LOG_PRINTF(" MSGFLAG_TXRQ");
                if ((flags & MSGFLAG_SSM_NACK))
                    MACCAN_LOG_PRINTF(" MSGFLAG_SSM_NACK");
                if ((flags & MSGFLAG_ABL))
                    MACCAN_LOG_PRINTF(" MSGFLAG_ABL");
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
    uint8_t length = 0U;
    uint32_t flags = 0U;
    uint64_t ticks = 0ULL;
    bool result = false;

    if (!message || !buffer)
        return false;
    if (nbyte < KVASER_HYDRA_MIN_EXT_CMD_LENGTH)
        return false;
    if (buffer[0] != CMD_EXTENDED)
        return false;
    if (buffer[6] != CMD_EXT_RX_MSG_FD)
        return false;

    /* extended command:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..5: command length (32..max. 96 bytes)
     * - byte 6: extended command code
     * - byte 7: (reserved)
     * - byte 8..11: flags
     * - byte 12..15: CAN id.
     * - byte 16..19: FPGA id. (whats the difference?)
     * - byte 20..23: FPGA control (CAN DLC in bit 8..11)
     * - byte 24..31: FPGA timestamp (64-bit)
     * - byte 32...: FPGA payload (max. 64 bytes)
     */
    bzero(message, sizeof(KvaserUSB_CanMessage_t));
    /* flags and length */
    flags = BUF2UINT32(buffer[8]);
    if (!(flags & MSGFLAG_ERROR_FRAME))
        length = Dlc2Len(buffer[21] & 0xFU);
    else
        length = 4U;  // error frame data
    /* message: id, dlc, data and flags */
    message->id = BUF2UINT32(buffer[12]) & CAN_MAX_XTD_ID;
    message->dlc = BUF2UINT8(buffer[21]) & CANFD_MAX_DLC;
    memcpy(message->data, &buffer[32], (size_t)length);
    message->xtd = (flags & MSGFLAG_EXT) ? 1 : 0;
    message->rtr = (flags & MSGFLAG_RTR) ? 1 : 0;
    message->fdf = (flags & MSGFLAG_FDF) ? 1 : 0;
    message->brs = (flags & MSGFLAG_BRS) ? 1 : 0;
    message->esi = (flags & MSGFLAG_ESI) ? 1 : 0;
    message->sts = (flags & MSGFLAG_STS) ? 1 : 0;
    /* note: error frames have four byte */
    if (message->sts) message->dlc = 4U;
    /* time-stamp from 64-bit timer value */
    ticks = BUF2UINT64(buffer[24]);
    KvaserUSB_TimestampFromTicks(&message->timestamp, ticks, frequency);
    /* note: we only enqueue ordinary CAN messages and error frames.
     *       The flags MSGFLAG_OVERRUN, MSGFLAG_NERR, MSGFLAG_WAKEUP,
     *       MSGFLAG_TX and MSGFLAG_TXRQ are handled by UpdateEventData
     */
    if (!(flags & ~(MSGFLAG_REMOTE_FRAME | MSGFLAG_ERROR_FRAME |
                    MSGFLAG_EXTENDED_ID | MSGFLAG_FDF | MSGFLAG_BRS | MSGFLAG_ESI)))
        result = true;
    return result;
}

static CANUSB_Return_t MapChannel(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_MAP_CHANNEL_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillMapChannelReq(buffer, HYDRA_CMD_SIZE, device->channelNo);
    retVal = SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_MAP_CHANNEL_RESP;
        retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: HE address
             * - byte 5: position (which position)
             * - byte 6..7: flags (what kind of flags?)
             * - byte 8..31: (not used)
             */
            uint16_t transId = BUF2UINT16(buffer[2]);
            device->hydraData.channel2he = BUF2UINT8(buffer[4]);
            device->hydraData.he2channel = (uint8_t)(transId & 0xFU);
            // TODO: position = BUF2UINT8(buffer[5]);
            // TODO: flags = BUF2UINT16(buffer[6]);

            /* note: command CMD_GET_CAPABILITIES_REQ requires HE address 'SYSDBG' */
            bzero(buffer, HYDRA_CMD_SIZE);
            size = FillMapChannelSysDbgReq(buffer, HYDRA_CMD_SIZE);
            retVal = SendRequest(device, buffer, size);
            if (retVal == CANUSB_SUCCESS) {
                size = HYDRA_CMD_SIZE;
                resp = CMD_MAP_CHANNEL_RESP;
                retVal = ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
                if (retVal == CANUSB_SUCCESS) {
                    /* command response:
                     * - byte 0: command code
                     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
                     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
                     * - byte 4: HE address
                     * - byte 5: position (which position)
                     * - byte 6..7: flags (what kind of flags?)
                     * - byte 8..31: (not used)
                     */
                    device->hydraData.sysdbg_he = BUF2UINT8(buffer[4]);
                }
            }
        }
    }
    return retVal;
}

static CANUSB_Return_t SendRequest(KvaserUSB_Device_t *device, const uint8_t *buffer, uint32_t nbyte) {
#if (OPTION_DEBUG_HE_ADDRESS != 0)
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..31: ...
     */
    assert(buffer);
    UInt8 cmdNo = (UInt8)buffer[0];
    UInt8 dstAddr = (UInt8)(buffer[1] & 0x3FU);
    UInt8 srcChannel = (UInt8)((buffer[1] & 0xC0U) >> 2) | (UInt8)((buffer[3] & 0xF0U) >> 4);
    UInt16 transId = (UInt16)((buffer[3] & 0x0FU) << 8) | (UInt16)buffer[2];
    MACCAN_LOG_PRINTF("> cmd=%02x dst=%x src=%x transId=%d\n", cmdNo, dstAddr, srcChannel, transId);
#endif
    /* note: the callee checks the sanity of the call */
    return KvaserUSB_SendRequest(device, buffer, nbyte);
}

static CANUSB_Return_t ReadResponse(KvaserUSB_Device_t *device, uint8_t *buffer, uint32_t nbyte, uint8_t cmdCode, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device || !buffer)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;
    if (nbyte < KVASER_HYDRA_COMMAND_LENGTH)
        return CANUSB_ERROR_ILLPARA;

    /* note: mhydra commands are always 32 byte long, the first byte contains the command code */
    do {
        retVal = CANPIP_Read(device->recvData.msgPipe, &buffer[0], KVASER_HYDRA_COMMAND_LENGTH, timeout);
        if (retVal != CANUSB_SUCCESS)
            break;
        // TODO: read extended command (?)
#if (OPTION_DEBUG_HE_ADDRESS != 0)
        /* command response:
         * - byte 0: command code
         * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
         * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
         * - byte 4..31: data
         */
        UInt8 cmdNo = (UInt8)buffer[0];
        UInt8 dstAddr = (UInt8)(buffer[1] & 0x3FU);
        UInt8 srcChannel = (UInt8)((buffer[1] & 0xC0U) >> 2) | (UInt8)((buffer[3] & 0xF0U) >> 4);
        UInt16 transId = (UInt16)((buffer[3] & 0x0FU) << 8) | (UInt16)buffer[2];
        MACCAN_LOG_PRINTF("< cmd=%02x dst=%x src=%x transId=%d\n", cmdNo, dstAddr, srcChannel, transId);
#endif
        /* error event:
         * - byte 0: command code
         * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
         * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
         * - byte 4..9: time (48-bit)
         * - byte 10: (reserved)
         * - byte 11: error code (see Linux driver)
         * - byte 12..13; additional information 1
         * - byte 14..15: additional information 2
         * - byte 16..31: (not used)
         */
        if (buffer[0] == CMD_ERROR_EVENT) {
            /* map error code (if possible) */
            switch (buffer[11]) {  // TODO: map firmware error codes
                case FIRMWARE_ERR_OK:
                case FIRMWARE_ERR_CAN:
                case FIRMWARE_ERR_NVRAM_ERROR:
                case FIRMWARE_ERR_NOPRIV:
                case FIRMWARE_ERR_ILLEGAL_ADDRESS:
                case FIRMWARE_ERR_UNKNOWN_CMD:
                case FIRMWARE_ERR_FATAL:
                case FIRMWARE_ERR_CHECKSUM_ERROR:
                case FIRMWARE_ERR_QUEUE_LEVEL:
                    retVal = (CANUSB_Return_t)(-100) - (CANUSB_Return_t)buffer[11];
                    break;
                case FIRMWARE_ERR_PARAMETER:
                    retVal = (CANUSB_Return_t)CANUSB_ERROR_ILLPARA;
                    break;
                default:
                    retVal = (CANUSB_Return_t)(-100) - (CANUSB_Return_t)buffer[11];
                    break;
            }
            break;
        }
    } while (buffer[0] != cmdCode);

    return retVal;
}

static uint32_t FillMapChannelReq(uint8_t *buffer, uint32_t maxbyte, uint8_t channel) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(channel < HYDRA_MAX_CARD_CHANNELS);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..19: name (string "CAN", "LIN", ...)
     * - byte 20: channel
     * - byte 21..31: (not used)
     */
    uint8_t address = SET_DST(0U, ROUTER_HE);
    uint16_t transId = SET_SEQ(0U, (0x0040U | channel));
    buffer[0] = CMD_MAP_CHANNEL_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT16LSB(transId);
    buffer[3] = UINT16MSB(transId);
    /* arguments */
    buffer[4] = UINT8BYTE('C');
    buffer[5] = UINT8BYTE('A');
    buffer[6] = UINT8BYTE('N');
    buffer[20] = UINT8BYTE(channel);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillMapChannelSysDbgReq(uint8_t *buffer, uint32_t maxbyte) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..19: name (string "SYSDBG")
     * - byte 20: channel = 0
     * - byte 21..31: (not used)
     */
    uint8_t address = SET_DST(0U, ROUTER_HE);
    uint16_t transId = SET_SEQ(0U, 0x0061U);
    buffer[0] = CMD_MAP_CHANNEL_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT16LSB(transId);
    buffer[3] = UINT16MSB(transId);
    /* arguments */
    buffer[4] = UINT8BYTE('S');
    buffer[5] = UINT8BYTE('Y');
    buffer[6] = UINT8BYTE('S');
    buffer[7] = UINT8BYTE('D');
    buffer[8] = UINT8BYTE('B');
    buffer[9] = UINT8BYTE('G');
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillSetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParams_t *params) {
    assert(buffer);
    assert(params);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..7: bitRate
     * - byte 8: tseg1
     * - byte 9: tseg2
     * - byte 10: sjw
     * - byte 11: noSamp
     * - byte 12..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_SET_BUSPARAMS_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
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
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillSetBusParamsFdReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParamsFd_t *params) {
    assert(buffer);
    assert(params);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..7: bitRate
     * - byte 8: tseg1
     * - byte 9: tseg2
     * - byte 10: sjw
     * - byte 11: noSamp
     * - byte 12: (reserved)
     * - byte 13..15: (not used)
     * - byte 16..19: bitRate
     * - byte 20: tseg1
     * - byte 21: tseg2
     * - byte 22: sjw
     * - byte 23: noSamp
     * - byte 24: open as CAN FD
     * - byte 25..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_SET_BUSPARAMS_FD_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments */
    buffer[4] = UINT8BYTE(params->nominal.bitRate);
    buffer[5] = UINT8BYTE(params->nominal.bitRate >> 8);
    buffer[6] = UINT8BYTE(params->nominal.bitRate >> 16);
    buffer[7] = UINT8BYTE(params->nominal.bitRate >> 24);
    buffer[8] = UINT8BYTE(params->nominal.tseg1);
    buffer[9] = UINT8BYTE(params->nominal.tseg2);
    buffer[10] = UINT8BYTE(params->nominal.sjw);
    buffer[11] = UINT8BYTE(params->nominal.noSamp);
    buffer[16] = UINT8BYTE(params->data.bitRate);
    buffer[17] = UINT8BYTE(params->data.bitRate >> 8);
    buffer[18] = UINT8BYTE(params->data.bitRate >> 16);
    buffer[19] = UINT8BYTE(params->data.bitRate >> 24);
    buffer[20] = UINT8BYTE(params->data.tseg1);
    buffer[21] = UINT8BYTE(params->data.tseg2);
    buffer[22] = UINT8BYTE(params->data.sjw);
    buffer[23] = UINT8BYTE(params->data.noSamp);
    buffer[24] = UINT8BYTE(params->canFd ? 1 : 0);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

#ifdef BUS_PARAMS_TQ  // TODO: activate when required
static uint32_t FillSetBusParamsTqReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParamsTq_t *params) {
    assert(buffer);
    assert(params);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..5: propagation segment (arbitration phase)
     * - byte 6..7: phase 1 segment (arbitration phase)
     * - byte 8..9: phase 2 segment (arbitration phase)
     * - byte 10..11: synchronous jump width (arbitration phase)
     * - byte 12..13: bit-rate prescaler (arbitration phase)
     * - byte 14..15: propagation segment (CAN FD data phase)
     * - byte 16..17: phase 1 segment (CAN FD data phase)
     * - byte 18..19: phase 2 segment (CAN FD data phase)
     * - byte 20..21: synchronous jump width (CAN FD data phase)
     * - byte 22..23: bit-rate prescaler (CAN FD data phase)
     * - byte 24: open as CAN FD
     * - byte 25..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_SET_BUSPARAMS_TQ_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments */
    buffer[4] =  UINT16LSB(params->arbitration.prop);
    buffer[5] =  UINT16MSB(params->arbitration.prop);
    buffer[6] =  UINT16LSB(params->arbitration.phase1);
    buffer[7] =  UINT16MSB(params->arbitration.phase1);
    buffer[8] =  UINT16LSB(params->arbitration.phase2);
    buffer[9] =  UINT16MSB(params->arbitration.phase2);
    buffer[10] = UINT16LSB(params->arbitration.sjw);
    buffer[11] = UINT16MSB(params->arbitration.sjw);
    buffer[12] = UINT16LSB(params->arbitration.brp);
    buffer[13] = UINT16MSB(params->arbitration.brp);
    buffer[14] = UINT16LSB(params->data.prop);
    buffer[15] = UINT16MSB(params->data.prop);
    buffer[16] = UINT16LSB(params->data.phase1);
    buffer[17] = UINT16MSB(params->data.phase1);
    buffer[18] = UINT16LSB(params->data.phase2);
    buffer[19] = UINT16MSB(params->data.phase2);
    buffer[20] = UINT16LSB(params->data.sjw);
    buffer[21] = UINT16MSB(params->data.sjw);
    buffer[22] = UINT16LSB(params->data.brp);
    buffer[23] = UINT16MSB(params->data.brp);
    buffer[24] = UINT8BYTE(params->canFd ? 1 : 0);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}
#endif

static uint32_t FillGetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, bool canFd) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: param type (0 = CAN 2.0, 1 = CAN FD)
     * - byte 5: (reserved)
     * - byte 6..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_GET_BUSPARAMS_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments */
    buffer[4] = UINT8BYTE(canFd ? 1 : 0);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetBusParamsTqReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, bool canFd) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: param type (0 = CAN 2.0, 1 = CAN FD)
     * - byte 5: (reserved)
     * - byte 6..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_GET_BUSPARAMS_TQ_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments */
    buffer[4] = UINT8BYTE(canFd ? 1 : 0);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillSetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_DriverMode_t mode) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: mode
     * - byte 5: (reserved)
     * - byte 6..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_SET_DRIVERMODE_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments */
    buffer[4] = UINT8BYTE(mode);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_GET_DRIVERMODE_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetChipStateReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_GET_CHIP_STATE_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillStartChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_START_CHIP_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillStopChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_STOP_CHIP_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillResetChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_RESET_CHIP_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillResetCardReq(uint8_t *buffer, uint32_t maxbyte) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, ROUTER_HE);
    buffer[0] = CMD_RESET_CARD_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillTxCanMessageReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint8_t transId, const KvaserUSB_CanMessage_t *message) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_EXT_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..5: command length (32..max. 96 bytes)
     * - byte 6: extended command code
     * - byte 7: (reserved)
     * - byte 8..11: flags
     * - byte 12..15: CAN id.
     * - byte 16..19: FPGA id.
     * - byte 20..23: FPGA control
     * - byte 24: databytes
     * - byte 25: dlc
     * - byte 26..31: (reserved)
     * - byte 32...: FPGA payload (max. 64 bytes)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_EXTENDED;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(transId);
    buffer[3] = UINT8BYTE(0x00);
    uint16_t length = message->dlc ? HYDRA_CMD_EXT_SIZE : HYDRA_CMD_SIZE;
    buffer[4] = UINT16LSB(length);
    buffer[5] = UINT16MSB(length);
    buffer[6] = CMD_TX_CAN_MESSAGE_FD;
    buffer[7] = UINT8BYTE(0x00);
    /* arguments */
    uint32_t flags = MSGFLAG_TX;
    flags |= message->xtd ? MSGFLAG_EXT : 0x0U;
    flags |= message->rtr ? MSGFLAG_RTR : 0x0U;
    flags |= message->fdf ? MSGFLAG_FDF : 0x0U;
    flags |= message->brs ? MSGFLAG_BRS : 0x0U;
    flags |= message->esi ? MSGFLAG_ESI : 0x0U;
    buffer[8] = UINT32LOLO(flags);
    buffer[9] = UINT32LOHI(flags);
    buffer[10] = UINT32HILO(flags);
    buffer[11] = UINT32HIHI(flags);
    buffer[12] = UINT32LOLO(message->id);
    buffer[13] = UINT32LOHI(message->id);
    buffer[14] = UINT32HILO(message->id);
    buffer[15] = UINT32HIHI(message->id);
    uint32_t fpga_id = message->id & 0x1FFFFFFFU;
    fpga_id |= message->xtd ? 0xC0000000U : 0x0U;
    fpga_id |= message->rtr ? 0x20000000U : 0x0U;
    buffer[16] = UINT32LOLO(fpga_id);
    buffer[17] = UINT32LOHI(fpga_id);
    buffer[18] = UINT32HILO(fpga_id);
    buffer[19] = UINT32HIHI(fpga_id);
    uint32_t fpga_ctrl = 0x80000000U;
    fpga_ctrl |= (uint32_t)(transId & 0xFFU) << 0;
    fpga_ctrl |= (uint32_t)(message->dlc & 0xFU) << 8;
    fpga_ctrl |= message->fdf ? 0x8000U : 0x0U;
    fpga_ctrl |= message->brs ? 0x4000U : 0x0U;
    fpga_ctrl |= message->esi ? 0x2000U : 0x0U;
    buffer[20] = UINT32LOLO(fpga_ctrl);
    buffer[21] = UINT32LOHI(fpga_ctrl);
    buffer[22] = UINT32HILO(fpga_ctrl);
    buffer[23] = UINT32HIHI(fpga_ctrl);
    uint8_t dlc = MIN(message->dlc, message->fdf ? CANFD_MAX_DLC : CAN_MAX_DLC);
    buffer[24] = Dlc2Len(dlc);
    buffer[25] = UINT8BYTE(dlc);
    buffer[26] = UINT8BYTE(0x00);
    buffer[27] = UINT8BYTE(0x00);
    buffer[28] = UINT8BYTE(0x00);
    buffer[29] = UINT8BYTE(0x00);
    buffer[30] = UINT8BYTE(0x00);
    buffer[31] = UINT8BYTE(0x00);
    memcpy(&buffer[32], message->data, Dlc2Len(message->dlc));
    /* return request length */
    return (uint32_t)length;
}

static uint32_t FillFlushQueueReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint8_t flags) {
    (void)flags;  // TODO: check for its purpose
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_FLUSH_QUEUE;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillResetErrorCounterReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_RESET_ERROR_COUNTER;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillResetStatisticsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_RESET_STATISTICS;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillReadClockReq(uint8_t *buffer, uint32_t maxbyte, uint8_t flags) {
    (void)flags;  // TODO: check for its purpose
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, ILLEGAL_HE);
    buffer[0] = CMD_READ_CLOCK_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetBusLoadReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    assert(destination < MAX_HE_COUNT);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_GET_BUSLOAD_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetCardInfoReq(uint8_t *buffer, uint32_t maxbyte, int8_t dataLevel) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: data level
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, ILLEGAL_HE);
    buffer[0] = CMD_GET_CARD_INFO_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments */
    buffer[4] = UINT8BYTE(dataLevel);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetSoftwareDetailsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t hydraExt) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: use Hydra extension
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, ILLEGAL_HE);
    buffer[0] = CMD_GET_SOFTWARE_DETAILS_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments */
    buffer[4] = UINT8BYTE(hydraExt);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetMaxOutstandingTxReq(uint8_t *buffer, uint32_t maxbyte) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, ILLEGAL_HE);
    buffer[0] = CMD_GET_SOFTWARE_INFO_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

#ifdef INTERFACE_INFO  // TODO: remove this (not implemented by any device)
static uint32_t FillGetInterfaceInfoReq(uint8_t *buffer, uint32_t maxbyte) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, ILLEGAL_HE);
    buffer[0] = CMD_GET_INTERFACE_INFO_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}
#endif

static uint32_t FillGetCapabilitiesReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint16_t subCmd/*, uint32_t extraInfo*/) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..5: sub-command (tbd.)
     * - byte 6..31: (not used)
     *        or
     * - byte 6..7: (not used)
     * - byte 8..11: address
     * - byte 12..31: (not used)
     *        or
     * - byte 6..7: (not used)
     * - byte 8: analyser no.
     * - byte 9..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_GET_CAPABILITIES_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* arguments (depend on sub-command) */
    buffer[4] = UINT16LSB(subCmd);
    buffer[5] = UINT16MSB(subCmd);
    // TODO: sub-command arguments
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint32_t FillGetTransceiverInfoReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination) {
    assert(buffer);
    assert(maxbyte >= HYDRA_CMD_SIZE);
    bzero(buffer, maxbyte);
    /* command request:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4: (reserved)
     * - byte 5..31: (not used)
     */
    uint8_t address = SET_DST(0U, destination);
    buffer[0] = CMD_GET_TRANSCEIVER_INFO_REQ;
    buffer[1] = UINT8BYTE(address);
    buffer[2] = UINT8BYTE(0x00);
    buffer[3] = UINT8BYTE(0x00);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

static uint8_t Dlc2Len(uint8_t dlc) {
    const static uint8_t dlc_table[16] = {
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U
    };
    return dlc_table[dlc & 0xFU];
}

#ifdef LEN2DLC  // TODO: activate when required
static uint8_t Len2Dlc(uint8_t len) {
    if(len > 48U) return 0x0FU;
    if(len > 32U) return 0x0EU;
    if(len > 24U) return 0x0DU;
    if(len > 20U) return 0x0CU;
    if(len > 16U) return 0x0BU;
    if(len > 12U) return 0x0AU;
    if(len > 8U) return 0x09U;
    return len;
}
#endif

#if (OPTION_PRINT_DEVICE_INFO != 0)
 static void PrintDeviceInfo(const KvaserUSB_DeviceInfo_t *deviceInfo) {
    assert(deviceInfo);
    MACCAN_DEBUG_DRIVER("    - card info:\n");
    MACCAN_DEBUG_DRIVER("      - channel count: %d\n", deviceInfo->card.channelCount);
    MACCAN_DEBUG_DRIVER("      - serial no.: %d\n", deviceInfo->card.serialNumber);
    MACCAN_DEBUG_DRIVER("      - clock resolution: %d\n", deviceInfo->card.clockResolution);
    time_t mfgDate = (time_t)deviceInfo->card.mfgDate;
    MACCAN_DEBUG_DRIVER("      - manufacturing date: %s", ctime(&mfgDate));
    MACCAN_DEBUG_DRIVER("      - EAN code (LSB first): %x%x%x%x%x-%x%x%x%x%x-%x%x%x%x%x-%x(?)\n",
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
    MACCAN_DEBUG_DRIVER("    - software info/details:\n");
    MACCAN_DEBUG_DRIVER("      - software options: 0x%x\n", deviceInfo->software.swOptions);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_CONFIG_MODE: %d\n", (deviceInfo->software.swOptions & SWOPTION_CONFIG_MODE) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_AUTO_TX_BUFFER: %d\n", (deviceInfo->software.swOptions & SWOPTION_AUTO_TX_BUFFER) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_BETA: %d\n", (deviceInfo->software.swOptions & SWOPTION_BETA) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_RC: %d\n", (deviceInfo->software.swOptions & SWOPTION_RC) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_BAD_MOOD: %d\n", (deviceInfo->software.swOptions & SWOPTION_BAD_MOOD) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_XX_MHZ_CLK: %s\n",
                        ((deviceInfo->software.swOptions & SWOPTION_CPU_FQ_MASK) == SWOPTION_16_MHZ_CLK) ? "16" :
                        ((deviceInfo->software.swOptions & SWOPTION_CPU_FQ_MASK) == SWOPTION_80_MHZ_CLK) ? "80" :
                        ((deviceInfo->software.swOptions & SWOPTION_CPU_FQ_MASK) == SWOPTION_24_MHZ_CLK) ? "24" : "XX");
    MACCAN_DEBUG_DRIVER("        - SWOPTION_TIMEOFFSET_VALID: %d\n", (deviceInfo->software.swOptions & SWOPTION_TIMEOFFSET_VALID) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_CAP_REQ: %d\n", (deviceInfo->software.swOptions & SWOPTION_CAP_REQ) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_DELAY_MSGS (mhydra): %d\n", (deviceInfo->software.swOptions & SWOPTION_DELAY_MSGS) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_USE_HYDRA_EXT (mhydra): %d\n", (deviceInfo->software.swOptions & SWOPTION_USE_HYDRA_EXT) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_CANFD_CAP (mhydra): %d\n", (deviceInfo->software.swOptions & SWOPTION_CANFD_CAP) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_NONISO_CAP (mhydra): %d\n", (deviceInfo->software.swOptions & SWOPTION_NONISO_CAP) ? 1 : 0);
    MACCAN_DEBUG_DRIVER("        - SWOPTION_XX_MHZ_CAN_CLK (mhydra): %s\n",
                        ((deviceInfo->software.swOptions & SWOPTION_CAN_CLK_MASK) == SWOPTION_80_MHZ_CAN_CLK) ? "80" :
                        ((deviceInfo->software.swOptions & SWOPTION_CAN_CLK_MASK) == SWOPTION_24_MHZ_CAN_CLK) ? "24" : "??");
    uint8_t major = (uint8_t)(deviceInfo->software.firmwareVersion >> 24);
    uint8_t minor = (uint8_t)(deviceInfo->software.firmwareVersion >> 16);
    uint16_t build = (uint16_t)(deviceInfo->software.firmwareVersion >> 0);
    MACCAN_DEBUG_DRIVER("      - firmware version: %u.%u (build %u)\n", major, minor, build);
    MACCAN_DEBUG_DRIVER("      - max. outstanding Tx: %d\n", deviceInfo->software.maxOutstandingTx);
    MACCAN_DEBUG_DRIVER("      - software name (hydra only): %d\n", deviceInfo->software.swName);
    MACCAN_DEBUG_DRIVER("      - EAN code (LSB first, hydra only): %x%x%x%x%x-%x%x%x%x%x-%x%x%x%x%x-%x\n",
                        (deviceInfo->software.EAN[7] >> 4), (deviceInfo->software.EAN[7] & 0xf),
                        (deviceInfo->software.EAN[6] >> 4), (deviceInfo->software.EAN[6] & 0xf),
                        (deviceInfo->software.EAN[5] >> 4), (deviceInfo->software.EAN[5] & 0xf),
                        (deviceInfo->software.EAN[4] >> 4), (deviceInfo->software.EAN[4] & 0xf),
                        (deviceInfo->software.EAN[3] >> 4), (deviceInfo->software.EAN[3] & 0xf),
                        (deviceInfo->software.EAN[2] >> 4), (deviceInfo->software.EAN[2] & 0xf),
                        (deviceInfo->software.EAN[1] >> 4), (deviceInfo->software.EAN[1] & 0xf),
                        (deviceInfo->software.EAN[0] >> 4), (deviceInfo->software.EAN[0] & 0xf));
    MACCAN_DEBUG_DRIVER("      - max. bit-rate (hydra only, otherwise 1Mbit/s): %d\n", deviceInfo->software.maxBitrate);
    MACCAN_DEBUG_DRIVER("    - transceiver info:\n");
    MACCAN_DEBUG_DRIVER("      - transceiver capabilities: 0x%x\n", deviceInfo->transceiver.transceiverCapabilities);
    MACCAN_DEBUG_DRIVER("      - transceiver status: %d\n", deviceInfo->transceiver.transceiverStatus);
    MACCAN_DEBUG_DRIVER("      - transceiver type: %d\n", deviceInfo->transceiver.transceiverType);
    MACCAN_DEBUG_DRIVER("    - capabilities (if SWOPTION_CAP_REQ):\n");
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_SILENT_MODE: %d\n", deviceInfo->capabilities.silentMode);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_ERRFRAME: %d\n", deviceInfo->capabilities.errorGen);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_BUS_STATS: %d\n", deviceInfo->capabilities.busStats);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_ERRCOUNT_READ: %d\n", deviceInfo->capabilities.errorCount);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_SINGLE_SHOT: %d\n", deviceInfo->capabilities.singleShot);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_SYNC_TX_FLUSH: %d\n", deviceInfo->capabilities.syncTxFlush);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_LOGGER: %d\n", deviceInfo->capabilities.hasLogger);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_REMOTE: %d\n", deviceInfo->capabilities.hasRemote);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_SCRIPT: %d\n", deviceInfo->capabilities.hasScript);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_LIN_HYBRID (mhydra): %d\n", deviceInfo->capabilities.linHybrid);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_KDI_INFO (mhydra): %d\n", deviceInfo->capabilities.kdiInfo);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_KDI (mhydra): %d\n", deviceInfo->capabilities.hasKdi);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_IO_API (mhydra): %d\n", deviceInfo->capabilities.hasIoApi);
    MACCAN_DEBUG_DRIVER("      - CAP_SUB_CMD_HAS_BUSPARAMS_TQ (mhydra): %d\n", deviceInfo->capabilities.hasTimeQuanta);
 }
#endif
