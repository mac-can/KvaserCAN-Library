/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
 *
 *  Copyright (c) 2021 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
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
#include "LeafPro.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <assert.h>

#include "MacCAN_Debug.h"

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
static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq);
static bool DecodeMessage(KvaserUSB_CanMessage_t *message, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq);

static CANUSB_Return_t HydraUSB_MapChannel(KvaserUSB_Device_t *device);

static CANUSB_Return_t HydraUSB_ReadResponse(KvaserUSB_Device_t *device, uint8_t *buffer, uint32_t nbyte,
                                                                         uint8_t cmdCode, /*uint8_t transId,*/ uint16_t timeout);

static uint32_t FillMapChannelReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillSetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParams_t *params);
static uint32_t FillSetBusParamsFdReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_BusParamsFd_t *params);
static uint32_t FillGetBusParamsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, bool canFd);
static uint32_t FillSetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, const KvaserUSB_DriverMode_t mode);
static uint32_t FillGetDriverModeReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillGetChipStateReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillStartChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillStopChipReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillTxCanMessageReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint8_t transId, const KvaserUSB_CanMessage_t *message);
static uint32_t FillFlushQueueReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination, uint8_t flags);
static uint32_t FillReadClockReq(uint8_t *buffer, uint32_t maxbyte, uint8_t flags);
static uint32_t FillGetBusLoadReq(uint8_t *buffer, uint32_t maxbyte, uint8_t destination);
static uint32_t FillGetCardInfoReq(uint8_t *buffer, uint32_t maxbyte, int8_t dataLevel);
static uint32_t FillGetSoftwareDetailsReq(uint8_t *buffer, uint32_t maxbyte, uint8_t hydraExt);
static uint32_t FillGetMaxOutstandingTxReq(uint8_t *buffer, uint32_t maxbyte);

static uint8_t Dlc2Len(uint8_t dlc);
//static uint8_t Len2Dlc(uint8_t len);

void LeafPro_GetOperationCapability(KvaserUSB_OpMode_t *opMode) {
    if (opMode) {
        *opMode = 0x00U;
        *opMode |= LEAF_PRO_MODE_FDOE ? CANMODE_FDOE : 0x00U;
        *opMode |= LEAF_PRO_MODE_BRSE ? CANMODE_BRSE : 0x00U;
        *opMode |= LEAF_PRO_MODE_NISO ? CANMODE_NISO : 0x00U;
        *opMode |= LEAF_PRO_MODE_SHRD ? CANMODE_SHRD : 0x00U;
        *opMode |= LEAF_PRO_MODE_NXTD ? CANMODE_NXTD : 0x00U;
        *opMode |= LEAF_PRO_MODE_NRTR ? CANMODE_NRTR : 0x00U;
        *opMode |= LEAF_PRO_MODE_ERR ? CANMODE_ERR : 0x00U;
        *opMode |= LEAF_PRO_MODE_MON ? CANMODE_MON : 0x00U;
    }
}

bool LeafPro_ConfigureChannel(KvaserUSB_Device_t *device) {
    /* sanity check */
    if (!device)
        return false;
    if (device->configured)
        return false;
    if (device->handle == CANUSB_INVALID_HANDLE)
        return false;
    if (device->productId != LEAF_PRO_PRODUCT_ID)
        return false;
    if (device->numChannels != LEAF_PRO_NUM_CHANNELS)
        return false;
    if (device->endpoints.numEndpoints != LEAF_PRO_NUM_ENDPOINTS)
        return false;

    /* set CAN channel properties and defaults */
    device->channelNo = 0U;  /* note: only one CAN channel */
    device->recvData.cpuFreq = LEAF_PRO_CPU_FREQUENCY;
    device->recvData.txAck.maxMsg = LEAF_PRO_MAX_OUTSTANDING_TX;
    LeafPro_GetOperationCapability(&device->opCapability);

    // initialize Hydra HE address and channel no. */
    device->hydraData.channel2he = ILLEGAL_HE;
    device->hydraData.he2channel = 0xFFU;

    /* Gotcha! */
    device->configured = true;

    return device->configured;
}

CANUSB_Return_t LeafPro_InitializeChannel(KvaserUSB_Device_t *device, const KvaserUSB_OpMode_t opMode) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* check requested operation mode */
    if ((opMode & ~device->opCapability)) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): unsupported operation mode (%02x)\n", device->name, device->handle, (opMode & ~device->opCapability));
        return CANUSB_ERROR_ILLPARA;  /* [2021-05-30]: cf. CAN API V3 function 'can_test' */
    }
    MACCAN_DEBUG_DRIVER("    Initializing %s driver...\n", device->name);
    /* start the reception loop */
    retVal = KvaserUSB_StartReception(device, ReceptionCallback);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): reception loop could not be started (%i)\n", device->name, device->handle, retVal);
        goto end_init;
    }
    /* map channel (It is not clear to me what the hydra is all about.) */
    retVal = HydraUSB_MapChannel(device);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): device could not be mapped (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = LeafPro_StopChip(device, HYDRA_CMD_RESP_TIMEOUT);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* set driver mode NORMAL (mode OFF seems not to be supported) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set driver mode NORMAL\n", device->name, device->handle);
    retVal = LeafPro_SetDriverMode(device, DRIVERMODE_NORMAL);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): driver mode could not be set (%i)\n", device->name, device->handle, retVal);
        goto err_init;
    }
    /* trigger chip state event */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): trigger chip state event\n", device->name, device->handle);
    retVal = LeafPro_RequestChipState(device, 0U);  /* 0 = no delay */
    if (retVal != CANUSB_SUCCESS) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip state event could not be triggered (%i)\n", device->name, device->handle, retVal);
        goto end_init;
    }
    /* get device information (don't care about the result) */
    retVal = LeafPro_GetCardInfo(device, &device->deviceInfo.card);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): card information could not be read (%i)\n", device->name, device->handle, retVal);
    }
    retVal = LeafPro_GetSoftwareInfo(device, &device->deviceInfo.software);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): firmware information could not be read (%i)\n", device->name, device->handle, retVal);
    }
    /* get reference time (amount of time in seconds and nanoseconds since the Epoch) */
    (void)clock_gettime(CLOCK_REALTIME, &device->recvData.timeRef);
    /* get CPU clock frequency (in [MHz]) from software options */
    switch (device->deviceInfo.software.swOptions & SWOPTION_CPU_FQ_MASK) {
        case SWOPTION_80_MHZ_CLK: device->recvData.cpuFreq = 80U; break;
        case SWOPTION_24_MHZ_CLK: device->recvData.cpuFreq = 24U; break;
        default:
            device->recvData.cpuFreq = LEAF_PRO_CPU_FREQUENCY;
            break;
    }
    /* get max. outstanding transmit messages */
    if ((0U < device->deviceInfo.software.maxOutstandingTx) &&
        (device->deviceInfo.software.maxOutstandingTx < MIN(LEAF_PRO_MAX_OUTSTANDING_TX, 255U)))
        device->recvData.txAck.maxMsg = (uint8_t)device->deviceInfo.software.maxOutstandingTx;
    else
        device->recvData.txAck.maxMsg = (uint8_t)MIN(LEAF_PRO_MAX_OUTSTANDING_TX, 255U);
    /* store demanded CAN operation mode*/
    device->recvData.opMode = opMode;
    retVal = CANUSB_SUCCESS;
end_init:
    return retVal;
err_init:
    (void)KvaserUSB_AbortReception(device);
    return retVal;
}

CANUSB_Return_t LeafPro_TeardownChannel(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    MACCAN_DEBUG_DRIVER("    Teardown %s driver...\n", device->name);
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = LeafPro_StopChip(device, 0U);  /* 0 = don't wait for response */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
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

CANUSB_Return_t LeafPro_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_SET_BUSPARAMS_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillSetBusParamsReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, params);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_SET_BUSPARAMS_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: driver mode
             * - byte 5: (reserved)
             * - byte 6..31: (not used)
             */
            // TODO: error handling
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_SetBusParamsFd(KvaserUSB_Device_t *device, const KvaserUSB_BusParamsFd_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || !params)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_SET_BUSPARAMS_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillSetBusParamsFdReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, params);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_SET_BUSPARAMS_FD_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: driver mode
             * - byte 5: (reserved)
             * - byte 6..31: (not used)
             */
            // TODO: error handling
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_BUSPARAMS_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
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
            // TODO: error handling
            params->bitRate = BUF2UINT32(buffer[4]);
            params->tseg1 = BUF2UINT8(buffer[8]);
            params->tseg2 = BUF2UINT8(buffer[9]);
            params->sjw = BUF2UINT8(buffer[10]);
            params->noSamp = BUF2UINT8(buffer[11]);
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_GetBusParamsFd(KvaserUSB_Device_t *device, KvaserUSB_BusParamsFd_t *params) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_BUSPARAMS_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
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
            // TODO: error handling
            params->nominal.bitRate = BUF2UINT32(buffer[4]);
            params->nominal.tseg1 = BUF2UINT8(buffer[8]);
            params->nominal.tseg2 = BUF2UINT8(buffer[9]);
            params->nominal.sjw = BUF2UINT8(buffer[10]);
            params->nominal.noSamp = BUF2UINT8(buffer[11]);

            /* send request CMD_GET_BUSPARAMS_REQ (CAN FD) and wait for response */
            bzero(buffer, HYDRA_CMD_SIZE);
            size = FillGetBusParamsReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he, true);
            retVal = KvaserUSB_SendRequest(device, buffer, size);
            if (retVal == CANUSB_SUCCESS) {
                size = HYDRA_CMD_SIZE;
                resp = CMD_GET_BUSPARAMS_RESP;
                retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
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
                    // TODO: error handling
                    params->data.bitRate = BUF2UINT32(buffer[4]);
                    params->data.tseg1 = BUF2UINT8(buffer[8]);
                    params->data.tseg2 = BUF2UINT8(buffer[9]);
                    params->data.sjw = BUF2UINT8(buffer[10]);
                    params->data.noSamp = BUF2UINT8(buffer[11]);
                }
            }
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_SetDriverMode(KvaserUSB_Device_t *device, const KvaserUSB_DriverMode_t mode) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);

    return retVal;
}

CANUSB_Return_t LeafPro_GetDriverMode(KvaserUSB_Device_t *device, KvaserUSB_DriverMode_t *mode) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_DRIVERMODE_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: driver mode
             * - byte 5: (reserved)
             * - byte 6..31: (not used)
             */
            // TODO: error handling
            *mode = BUF2UINT8(buffer[4]);
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_StartChip(KvaserUSB_Device_t *device, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[HYDRA_CMD_SIZE];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send request CMD_STOP_CHIP_REQ and wait for response */
    bzero(buffer, HYDRA_CMD_SIZE);
    size = FillStartChipReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_START_CHIP_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: (reserved)
             * - byte 5..31: (not used)
             */
            // TODO: error handling
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_StopChip(KvaserUSB_Device_t *device, uint16_t timeout) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (timeout > 0U)) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_STOP_CHIP_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: (reserved)
             * - byte 5..31: (not used)
             */
            // TODO: error handling
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_RequestChipState(KvaserUSB_Device_t *device, uint16_t delay) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if ((retVal == CANUSB_SUCCESS) && (delay > 0U)) {
        usleep((unsigned int)delay * 1000);
    }
    return retVal;
}

CANUSB_Return_t LeafPro_SendMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout) {
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
    if (message->esi && !(device->recvData.opMode & CANMODE_FDOE))  // TODO: check this
        return CANUSB_ERROR_ILLPARA;
    if (message->esi && !message->fdf)  // TODO: and also this
        return CANUSB_ERROR_ILLPARA;
    if (message->sts)  /* note: error frames cannot be sent */
        return CANUSB_ERROR_ILLPARA;

    /* check for pending transmit messages */
    if (device->recvData.txAck.cntMsg < device->recvData.txAck.maxMsg)
        device->recvData.txAck.transId = (device->recvData.txAck.transId + 1U) % device->recvData.txAck.maxMsg;
    else
        return CANUSB_ERROR_FULL;

    /* channel no. and transaction id */
    uint8_t channel = device->hydraData.channel2he;
    uint8_t transId = device->recvData.txAck.transId;

    /* send request CMD_EXTENDED[CMD_TX_CAN_MESSAGE_FD] and wait for ackknowledge (optional) */
    bzero(buffer, KVASER_MAX_COMMAND_LENGTH);
    size = FillTxCanMessageReq(buffer, HYDRA_CMD_EXT_SIZE, channel, transId, message);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_EXTENDED;
        if (timeout > 0U) {
            /* expect a response (timed or blocking write) */
            device->recvData.txAck.noAck = false;
            retVal = HydraUSB_ReadResponse(device, buffer, size, resp, timeout);
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
                // TODO: error handling
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
    return retVal;
}

CANUSB_Return_t LeafPro_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout) {
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

CANUSB_Return_t LeafPro_FlushQueue(KvaserUSB_Device_t *device/*, uint8_t flags*/) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_FLUSH_QUEUE_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..7: flags
             * - byte 8..31: (not used)
             */
            // TODO: error handling
            /* flags = BUF2UINT32(buffer[4]); */
            // TODO: what to do with them
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_ReadClock(KvaserUSB_Device_t *device, uint64_t *nsec) {
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
    size = FillReadClockReq(buffer, HYDRA_CMD_SIZE, device->hydraData.channel2he);
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_READ_CLOCK_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4..9: time (48-bit)
             * - byte 10..31: (not used)
             */
            // TODO: error handling
            uint64_t ticks = 0ULL;
            ticks |= (uint64_t)BUF2UINT16(buffer[4]) << 0;
            ticks |= (uint64_t)BUF2UINT16(buffer[6]) << 16;
            ticks |= (uint64_t)BUF2UINT16(buffer[8]) << 32;
            /* note: when software options not read before, assume clock running at 24MHz
             */
            if (device->recvData.cpuFreq == 0U)
                device->recvData.cpuFreq = 24U;
            *nsec = KvaserUSB_NanosecondsFromTicks(ticks, device->recvData.cpuFreq);
        }
    }
    return retVal;
}

CANUSB_Return_t LeafPro_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_BUSLOAD_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
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
            // TODO: error handling
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

CANUSB_Return_t LeafPro_GetCardInfo(KvaserUSB_Device_t *device, KvaserUSB_CardInfo_t *info/*, int8_t dataLevel*/) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_CARD_INFO_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
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
            // TODO: error handling
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

CANUSB_Return_t LeafPro_GetSoftwareInfo(KvaserUSB_Device_t *device, KvaserUSB_SoftwareInfo_t *info/*, uint8_t hydraExt*/) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_GET_SOFTWARE_DETAILS_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
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
            // TODO: error handling
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
            retVal = KvaserUSB_SendRequest(device, buffer, size);
            if (retVal == CANUSB_SUCCESS) {
                size = HYDRA_CMD_SIZE;
                resp = CMD_GET_SOFTWARE_INFO_RESP;
                retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
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
                    // TODO: error handling
                    info->maxOutstandingTx = BUF2UINT16(buffer[12]);
                }
            }
        }
    }
    return retVal;
}

static void ReceptionCallback(void *refCon, UInt8 *buffer, UInt32 size) {
    KvaserUSB_RecvData_t *context = (KvaserUSB_RecvData_t*)refCon;
    KvaserUSB_CanMessage_t message;
    UInt32 index = 0U;
    UInt32 nbyte = 0U;

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
                    (void)UpdateEventData(&context->evData, &hydra->buffer[index], nbyte, context->cpuFreq);
                    break;
                case CMD_GET_BUSPARAMS_RESP:
                case CMD_GET_DRIVERMODE_RESP:
                case CMD_START_CHIP_RESP:
                case CMD_STOP_CHIP_RESP:
                case CMD_READ_CLOCK_RESP:
                case CMD_FLUSH_QUEUE_RESP:
                case CMD_SET_BUSPARAMS_FD_RESP:
                case CMD_SET_BUSPARAMS_RESP:
                case CMD_GET_CARD_INFO_RESP:
                case CMD_GET_SOFTWARE_INFO_RESP:
                case CMD_GET_BUSLOAD_RESP:
                case CMD_MAP_CHANNEL_RESP:
                case CMD_GET_SOFTWARE_DETAILS_RESP:
                    /* command response: write packet in the pipe */
                    (void)CANPIP_Write(context->msgPipe, &hydra->buffer[index], nbyte);
                    break;
                case CMD_EXTENDED:
                    switch (hydra->buffer[6]) {
                        case CMD_EXT_RX_MSG_FD:
                            /* received CAN message: decode and enqueue */
                            if (DecodeMessage(&message, &hydra->buffer[index], nbyte, context->cpuFreq)) {
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
                                (void)UpdateEventData(&context->evData, &hydra->buffer[index], nbyte, context->cpuFreq);
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

static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq) {
    uint32_t flags = 0U;
    bool result = false;

    if (!event || !buffer)
        return false;
    if (nbyte < HYDRA_CMD_SIZE)
        return false;

    (void)cpuFreq;  // currently not used

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

static bool DecodeMessage(KvaserUSB_CanMessage_t *message, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq) {
    uint8_t length = 0U;
    uint16_t flags = 0U;
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
    flags = BUF2UINT16(buffer[8]);
    if (!(flags & MSGFLAG_ERROR_FRAME))
        length = Dlc2Len(buffer[21] & 0xFU);
    else
        length = 4U;  // error fram data
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
    /* time-stamp from 48-bit timer value */
    ticks = BUF2UINT64(buffer[24]);
    KvaserUSB_TimestampFromTicks(&message->timestamp, ticks, cpuFreq);
    /* note: we only enqueue ordinary CAN messages and error frames.
     *       The flags MSGFLAG_OVERRUN, MSGFLAG_NERR, MSGFLAG_WAKEUP,
     *       MSGFLAG_TX and MSGFLAG_TXRQ are handled by UpdateEventData
     */
    if (!(flags & ~(MSGFLAG_REMOTE_FRAME | MSGFLAG_ERROR_FRAME |
                    MSGFLAG_EXTENDED_ID | MSGFLAG_FDF | MSGFLAG_BRS | MSGFLAG_ESI)))
        result = true;
    return result;
}

static CANUSB_Return_t HydraUSB_MapChannel(KvaserUSB_Device_t *device) {
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
    retVal = KvaserUSB_SendRequest(device, buffer, size);
    if (retVal == CANUSB_SUCCESS) {
        size = HYDRA_CMD_SIZE;
        resp = CMD_MAP_CHANNEL_RESP;
        retVal = HydraUSB_ReadResponse(device, buffer, size, resp, HYDRA_CMD_RESP_TIMEOUT);
        if (retVal == CANUSB_SUCCESS) {
            /* command response:
             * - byte 0: command code
             * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
             * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
             * - byte 4: HE address
             * - byte 5: position
             * - byte 6..7: flags (FIXME: what kind of flags?)
             * - byte 8..31: (not used)
             */
            // TODO: error handling
            uint16_t transId = BUF2UINT16(buffer[2]);
            device->hydraData.channel2he = BUF2UINT8(buffer[4]);
            device->hydraData.he2channel = (uint8_t)(transId & 0xFU);
        }
    }
    return retVal;
}

static CANUSB_Return_t HydraUSB_ReadResponse(KvaserUSB_Device_t *device, uint8_t *buffer, uint32_t nbyte, uint8_t cmdCode, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device || !buffer)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;
    if (nbyte < KVASER_HYDRA_COMMAND_LENGTH)
        return CANUSB_ERROR_ILLPARA;

    /* command response:
     * - byte 0: command code
     * - byte 1: HE address (bit 0..5 = dst, bit 6..7 = src MSB)
     * - byte 2..3: transaction id. (bit 0..11 = seq, bit 11..15: src LSB)
     * - byte 4..31: data
     */
    do {
        retVal = CANPIP_Read(device->recvData.msgPipe, &buffer[0], KVASER_HYDRA_COMMAND_LENGTH, timeout);
        if (retVal != CANUSB_SUCCESS)
            break;
        // TODO: read extended command (?)
#if (0)
        else if (buffer[0] == cmdCode)
            MACCAN_LOG_WRITE(buffer, nbyte, "!");
        else
            MACCAN_LOG_WRITE(buffer, nbyte, "#");
#endif
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
    buffer[23] = UINT8BYTE(1/*params->data.noSamp*/);
    buffer[24] = UINT8BYTE(params->canFd ? 1 : 0);
    /* return request length */
    return (uint32_t)HYDRA_CMD_SIZE;
}

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
    flags |= message->esi ? MSGFLAG_ESI : 0x0U;  // TODO: check this
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
    fpga_ctrl |= message->esi ? 0x2000U : 0x0U;  // TODO: check this
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

static uint32_t FillReadClockReq(uint8_t *buffer, uint32_t maxbyte, uint8_t flags) {
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
    uint8_t address = SET_DST(0U, ROUTER_HE);
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
    uint8_t address = SET_DST(0U, ROUTER_HE);
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
    uint8_t address = SET_DST(0U, ROUTER_HE);
    buffer[0] = CMD_GET_SOFTWARE_INFO_REQ;
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

//static uint8_t Len2Dlc(uint8_t len) {
//    if(len > 48U) return 0x0FU;
//    if(len > 32U) return 0x0EU;
//    if(len > 24U) return 0x0DU;
//    if(len > 20U) return 0x0CU;
//    if(len > 16U) return 0x0BU;
//    if(len > 12U) return 0x0AU;
//    if(len > 8U) return 0x09U;
//    return len;
//}
