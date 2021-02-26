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
#include "LeafLight.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <assert.h>

#include "MacCAN_Debug.h"

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

#define MIN(x,y)  (((x) < (y)) ? (x) : (y))

static void ReceptionCallback(void *refCon, UInt8 *buffer, UInt32 size);
static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq);
static bool DecodeMessage(KvaserUSB_CanMessage_t *message, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq);

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

void LeafLight_GetOperationCapability(KvaserUSB_OpMode_t *opMode) {
    if (opMode) {
        *opMode = 0x00U;
        *opMode |= LEAF_LIGHT_MODE_FDOE ? CANMODE_FDOE : 0x00U;
        *opMode |= LEAF_LIGHT_MODE_BRSE ? CANMODE_BRSE : 0x00U;
        *opMode |= LEAF_LIGHT_MODE_NISO ? CANMODE_NISO : 0x00U;
        *opMode |= LEAF_LIGHT_MODE_SHRD ? CANMODE_SHRD : 0x00U;
        *opMode |= LEAF_LIGHT_MODE_NXTD ? CANMODE_NXTD : 0x00U;
        *opMode |= LEAF_LIGHT_MODE_NRTR ? CANMODE_NRTR : 0x00U;
        *opMode |= LEAF_LIGHT_MODE_ERR ? CANMODE_ERR : 0x00U;
        *opMode |= LEAF_LIGHT_MODE_MON ? CANMODE_MON : 0x00U;
    }
}

bool LeafLight_ConfigureChannel(KvaserUSB_Device_t *device) {
    /* sanity check */
    if (!device)
        return false;
    if (device->configured)
        return false;
    if (device->handle == CANUSB_INVALID_HANDLE)
        return false;
    if (device->productId != LEAF_LIGHT_PRODUCT_ID)
        return false;
    if (device->numChannels != LEAF_LIGHT_NUM_CHANNELS)
        return false;
    if (device->endpoints.numEndpoints != LEAF_LIGHT_NUM_ENDPOINTS)
        return false;

    /* set CAN channel properties and defaults */
    device->channelNo = 0U;  /* note: only one CAN channel */
    device->recvData.cpuFreq = LEAF_LIGHT_CPU_FREQUENCY;
    device->recvData.txAck.maxMsg = LEAF_LIGHT_MAX_OUTSTANDING_TX;
    LeafLight_GetOperationCapability(&device->opCapability);

    /* Gotcha! */
    device->configured = true;
    
    return device->configured;
}

CANUSB_Return_t LeafLight_InitializeChannel(KvaserUSB_Device_t *device, const KvaserUSB_OpMode_t opMode) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* check requested operation mode */
    if ((opMode & ~device->opCapability)) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): unsupported operation mode (%02x)\n", device->name, device->handle, (opMode & ~device->opCapability));
        return CANUSB_ERROR_NOTSUPP;
    }
    MACCAN_DEBUG_DRIVER("    Initializing %s driver...\n", device->name);
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = LeafLight_StopChip(device, 0U);  /* 0 = don't wait for response */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
        goto end_init;
    }
    /* set driver mode OFF */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set driver mode OFF\n", device->name, device->handle);
    retVal = LeafLight_SetDriverMode(device, DRIVERMODE_OFF);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): driver mode could not be set (%i)\n", device->name, device->handle, retVal);
        goto end_init;
    }
    /* trigger chip state event */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): trigger chip state event\n", device->name, device->handle);
    retVal = LeafLight_RequestChipState(device, 0U);  /* 0 = no delay */
    if (retVal != CANUSB_SUCCESS) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip state event could not be triggered (%i)\n", device->name, device->handle, retVal);
        goto end_init;
    }
    /* start the reception loop */
    retVal = KvaserUSB_StartReception(device, ReceptionCallback);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): reception loop could not be started (%i)\n", device->name, device->handle, retVal);
        goto end_init;
    }
    /* get device information (don't care about the result) */
    retVal = LeafLight_GetCardInfo(device, &device->deviceInfo.card);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): card information could not be read (%i)\n", device->name, device->handle, retVal);
        retVal = CANUSB_SUCCESS;
    }
    retVal = LeafLight_GetSoftwareInfo(device, &device->deviceInfo.software);
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): firmware information could not be read (%i)\n", device->name, device->handle, retVal);
        retVal = CANUSB_SUCCESS;
    }
    /* get reference time (amount of time in seconds and nanoseconds since the Epoch) */
    (void)clock_gettime(CLOCK_REALTIME, &device->recvData.timeRef);
    /* get CPU clock frequency (in [MHz]) from software options */
    switch (device->deviceInfo.software.swOptions & SWOPTION_CPU_FQ_MASK) {
        case SWOPTION_16_MHZ_CLK: device->recvData.cpuFreq = 16U; break;
        case SWOPTION_32_MHZ_CLK: device->recvData.cpuFreq = 32U; break;
        case SWOPTION_24_MHZ_CLK: device->recvData.cpuFreq = 24U; break;
        default:
            device->recvData.cpuFreq = LEAF_LIGHT_CPU_FREQUENCY;
            break;
    }
    /* get max. outstanding transmit messages */
    if ((0U < device->deviceInfo.software.maxOutstandingTx) &&
        (device->deviceInfo.software.maxOutstandingTx < MIN(LEAF_LIGHT_MAX_OUTSTANDING_TX, 255U)))
        device->recvData.txAck.maxMsg = (uint8_t)device->deviceInfo.software.maxOutstandingTx;
    else
        device->recvData.txAck.maxMsg = (uint8_t)MIN(LEAF_LIGHT_MAX_OUTSTANDING_TX, 255U);
    /* store demanded CAN operation mode*/
    device->recvData.opMode = opMode;
    retVal = CANUSB_SUCCESS;
end_init:
    return retVal;
}

CANUSB_Return_t LeafLight_TeardownChannel(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    MACCAN_DEBUG_DRIVER("    Teardown %s driver...\n", device->name);
    /* stop chip (go bus OFF) */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): stop chip (go bus OFF)\n", device->name, device->handle);
    retVal = LeafLight_StopChip(device, 0U);  /* 0 = don't wait for response */
    if (retVal < 0) {
        MACCAN_DEBUG_ERROR("+++ %s (device #%u): chip could not be stopped (%i)\n", device->name, device->handle, retVal);
        //goto end_exit;
    }
    /* set driver mode OFF */
    MACCAN_DEBUG_DRIVER(">>> %s (device #%u): set driver mode OFF\n", device->name, device->handle);
    retVal = LeafLight_SetDriverMode(device, DRIVERMODE_OFF);
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

CANUSB_Return_t LeafLight_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params) {
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

    return retVal;
}

CANUSB_Return_t LeafLight_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params) {
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
        }
    }
    return retVal;
}

CANUSB_Return_t LeafLight_SetDriverMode(KvaserUSB_Device_t *device, const KvaserUSB_DriverMode_t mode) {
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

CANUSB_Return_t LeafLight_GetDriverMode(KvaserUSB_Device_t *device, KvaserUSB_DriverMode_t *mode) {
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

CANUSB_Return_t LeafLight_StartChip(KvaserUSB_Device_t *device, uint16_t timeout) {
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

CANUSB_Return_t LeafLight_StopChip(KvaserUSB_Device_t *device, uint16_t timeout) {
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

CANUSB_Return_t LeafLight_ResetChip(KvaserUSB_Device_t *device, uint16_t delay)  {
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

CANUSB_Return_t LeafLight_ResetCard(KvaserUSB_Device_t *device, uint16_t delay) {
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

CANUSB_Return_t LeafLight_RequestChipState(KvaserUSB_Device_t *device, uint16_t delay)  {
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

CANUSB_Return_t LeafLight_SendMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout) {
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

CANUSB_Return_t LeafLight_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout) {
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

CANUSB_Return_t LeafLight_FlushQueue(KvaserUSB_Device_t *device/*, uint8_t flags*/) {
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
            flags = BUF2UINT32(buffer[4]);
            // TODO: what to do with them
        }
    }
    return retVal;
}

CANUSB_Return_t LeafLight_ResetErrorCounter(KvaserUSB_Device_t *device, uint16_t delay) {
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

CANUSB_Return_t LeafLight_ResetStatistics(KvaserUSB_Device_t *device, uint16_t delay) {
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

CANUSB_Return_t LeafLight_ReadClock(KvaserUSB_Device_t *device, uint64_t *nsec) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || nsec)
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
            if (device->recvData.cpuFreq == 0U)
                device->recvData.cpuFreq = 24U;
            *nsec = KvaserUSB_NanosecondsFromTicks(ticks, device->recvData.cpuFreq);
        }
    }
    return retVal;
}

CANUSB_Return_t LeafLight_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t buffer[KVASER_MAX_COMMAND_LENGTH];
    uint32_t size;
    uint8_t resp;

    /* sanity check */
    if (!device || load)
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

CANUSB_Return_t LeafLight_GetCardInfo(KvaserUSB_Device_t *device, KvaserUSB_CardInfo_t *info/*, uint8_t dataLevel*/) {
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

CANUSB_Return_t LeafLight_GetSoftwareInfo(KvaserUSB_Device_t *device, KvaserUSB_SoftwareInfo_t *info) {
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

CANUSB_Return_t LeafLight_GetInterfaceInfo(KvaserUSB_Device_t *device, KvaserUSB_InterfaceInfo_t *info) {
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
                    (void)UpdateEventData(&context->evData, &buffer[index], nbyte, context->cpuFreq);
                    break;
                case CMD_GET_BUSPARAMS_RESP:
                case CMD_GET_DRIVERMODE_RESP:
                case CMD_START_CHIP_RESP:
                case CMD_STOP_CHIP_RESP:
                case CMD_READ_CLOCK_RESP:
                case CMD_GET_CARD_INFO_RESP:
                case CMD_GET_SOFTWARE_INFO_RESP:
                case CMD_GET_BUSLOAD_RESP:
                case CMD_FILO_FLUSH_QUEUE_RESP:
                    /* command response: write packet in the pipe */
                    (void)CANPIP_Write(context->msgPipe, &buffer[index], nbyte);
                    break;
                case CMD_LOG_MESSAGE:
                    /* logged CAN message: decode and enqueue */
                    if (DecodeMessage(&message, &buffer[index], nbyte, context->cpuFreq)) {
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
                        (void)UpdateEventData(&context->evData, &buffer[index], nbyte, context->cpuFreq);
                    }
                    break;
                case CMD_TX_ACKNOWLEDGE:
                    // TODO: ...
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

static bool UpdateEventData(KvaserUSB_EventData_t *event, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq) {
    bool result = false;
    
    if (!event || !buffer)
        return false;
    if (nbyte < KVASER_MIN_COMMAND_LENGTH)
        return false;
    
    (void)cpuFreq;  // currently not used
    
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

static bool DecodeMessage(KvaserUSB_CanMessage_t *message, uint8_t *buffer, uint32_t nbyte, KvaserUSB_CpuClock_t cpuFreq) {
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
    KvaserUSB_TimestampFromTicks(&message->timestamp, ticks, cpuFreq);
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
    buffer[0] = message->xtd ? LEN_TX_EXT_MESSAGE : LEN_TX_STD_MESSAGE;
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
