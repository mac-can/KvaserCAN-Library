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
#include "KvaserUSB_Device.h"
#include "MacCAN_Debug.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

static CANUSB_Return_t GetUsbConfiguration(CANUSB_Handle_t handle, KvaserUSB_CanChannel_t channel, KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    uint8_t dir, type;
    uint16_t size;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (device->configured)
        return CANUSB_ERROR_YETINIT;

    /* get product ID and revision no. from device */
    device->handle = CANUSB_INVALID_HANDLE;
    if ((retVal = CANUSB_GetDeviceProductId(handle, &device->productId)) < 0)
        return retVal;
    if ((retVal = CANUSB_GetDeviceReleaseNo(handle, &device->releaseNo)) < 0)
        return retVal;

    /* get number of CAN channels from device */
    if ((retVal = CANUSB_GetDeviceNumCanChannels(handle, &device->numChannels)) < 0)
        return retVal;
    /* set CAN channel number to be used */
    if ((channel < device->numChannels) && (channel < KVASER_MAX_CAN_CHANNELS))
        device->channelNo = channel;
    else
        return CANUSB_ERROR_FATAL;

    /* get number of endpoints from device */
    if ((retVal = CANUSB_GetInterfaceNumEndpoints(handle, &device->endpoints.numEndpoints)) < 0)
        return retVal;
    /* get endpoint properties from device */
    // FIXME: Nah, nah, there's gotta be something better!
    for (uint8_t i = 1U; (i <= device->endpoints.numEndpoints) && (i < (1U + (KVASER_MAX_CAN_CHANNELS * 2U))); i++) {
        if (CANUSB_GetInterfaceEndpointDirection(handle, i, &dir) < 0)
            return retVal;
        if (CANUSB_GetInterfaceEndpointTransferType(handle, i, &type) < 0)
            return retVal;
        if (CANUSB_GetInterfaceEndpointMaxPacketSize(handle, i, &size) < 0)
            return retVal;
        if ((i % 2)) {  /* bulk in: odd endpoint numbers {1..} */
            if ((dir == USBPIPE_DIR_IN) && (type == USBPIPE_TYPE_BULK))
                device->endpoints.bulkIn.packetSize = size;
            else
                device->endpoints.bulkIn.packetSize = 0U;
            device->endpoints.bulkIn.pipeRef = i;
        } else {  /* bulk out: even endpoint numbers {2..} */
            if ((dir == USBPIPE_DIR_OUT) && (type == USBPIPE_TYPE_BULK))
                device->endpoints.bulkOut.packetSize = size;
            else
                device->endpoints.bulkOut.packetSize = 0U;
            device->endpoints.bulkOut.pipeRef = i;
        }
    }
    /* set device name, vendor name and website (zero-terminated strings) */
    if (CANUSB_GetDeviceName(handle, device->name, KVASER_MAX_NAME_LENGTH) < 0)
        strncpy(device->name, "(unkown)", KVASER_MAX_NAME_LENGTH);
    strncpy(device->vendor, KVASER_NAME, KVASER_MAX_NAME_LENGTH);
    strncpy(device->website, KVASER_WEBSITE, KVASER_MAX_NAME_LENGTH);
    
    /* the USB handle and the CAN channel number are valid now,
     * but the configuration must be confirmed for the device! */
    device->handle = handle;
    retVal = CANUSB_SUCCESS;

    return retVal;
}

CANUSB_Return_t KvaserUSB_ProbeUsbDevice(CANUSB_Index_t channel, uint16_t *productId) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    CANUSB_Index_t index = channel;

    /* check if the device is present (available) and opened (occupied) */
    if (!CANUSB_IsDevicePresent(index)) {
//        MACCAN_DEBUG_INFO("+++ MacCAN-Core: device (%02x) not available\n", channel);
        retVal = CANERR_HANDLE;
    } else if (!CANUSB_IsDeviceOpened(index)) {
//        MACCAN_DEBUG_INFO("+++ MacCAN-Core: device (%02x) available\n", channel);
        retVal = CANERR_NOERROR;
    } else {
//        MACCAN_DEBUG_INFO("+++ MacCAN-Core: device (%02x) occupied\n", channel);
        retVal = CANERR_NOERROR + 1;
    }
    /* get the product ID of the USB device (NULL pointer is checked there) */
    (void)CANUSB_GetDeviceProductId(index, productId);
    
    return retVal;
}

CANUSB_Return_t KvaserUSB_OpenUsbDevice(CANUSB_Index_t channel, KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    CANUSB_Handle_t handle = CANUSB_INVALID_HANDLE;
    CANUSB_Index_t index = channel;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (device->configured)
        return CANUSB_ERROR_YETINIT;

    /* open the USB device at index, if and only if the vendor ID matches */
    handle = CANUSB_OpenDevice(index, KVASER_VENDOR_ID, CANUSB_ANY_PRODUCT_ID);
    if (handle < 0) {
//        MACCAN_DEBUG_ERROR("+++ MacCAN-Core: device could not be opened (%02x)\n", channel);
        return CANUSB_ERROR_NOTINIT;
    }
    /* get the USB configuration of the device (also check the CAN channel number) */
    retVal = GetUsbConfiguration(handle, 0U, device);  // TODO: multi-channel devices
    if (retVal < 0) {
//        MACCAN_DEBUG_ERROR("+++ MacCAN-Core: configuration could not be read (%02x)\n", channel);
        (void)CANUSB_CloseDevice(handle);
        return retVal;
    }
    /* create a pipe for data exchange */
    device->recvData.msgPipe = CANPIP_Create();
    if (device->recvData.msgPipe == NULL) {
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: pipe could not be created (NULL)\n", device->name, device->channelNo+1);
        (void)CANUSB_CloseDevice(handle);
        return retVal;;
    }
    /* create a message queue for received CAN frames */
    device->recvData.msgQueue = CANQUE_Create(KVASER_RECEIVE_QUEUE_SIZE, sizeof(KvaserUSB_CanMessage_t));
    if (device->recvData.msgQueue == NULL) {
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: message queue could not be created (NULL)\n", device->name, device->channelNo+1);
        (void)CANPIP_Destroy(device->recvData.msgPipe);
        (void)CANUSB_CloseDevice(handle);
        return retVal;;
    }
    /* create a pipe context for the selected CAN channel on the device */
    uint8_t pipeRef = device->endpoints.bulkIn.pipeRef;
    size_t bufSize = device->endpoints.bulkIn.packetSize;
    device->recvPipe = CANUSB_CreatePipeAsync(device->handle, pipeRef, bufSize);
    if (device->recvPipe == NULL) {
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: asynchronous pipe context could not be created (NULL)\n", device->name, device->channelNo+1);
        (void)CANQUE_Destroy(device->recvData.msgQueue);
        (void)CANPIP_Destroy(device->recvData.msgPipe);
        (void)CANUSB_CloseDevice(handle);
        return CANUSB_ERROR_RESOURCE;
    }
    return retVal;
}

CANUSB_Return_t KvaserUSB_CloseUsbDevice(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;

    /* abort asynchronous pipe */
    retVal = CANUSB_ReadPipeAsyncAbort(device->recvPipe);
//    if (retVal < 0)
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: asynchronous pipe could not be stopped (%i)\n", device->name, device->channelNo+1, retVal);
    /* close the USB device */
    retVal = CANUSB_CloseDevice(device->handle);
//    if (retVal < 0)
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: device could not be closed (%i)\n", device->name, device->channelNo+1, retVal);
    /* destroy the pipe context */
    retVal = CANUSB_DestroyPipeAsync(device->recvPipe);
//    if (retVal < 0)
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: asynchronous pipe context could not be released (%i)\n", device->name, device->channelNo+1, retVal);
    /* destroy the message queue */
    retVal = CANQUE_Destroy(device->recvData.msgQueue);
//    if (retVal < 0)
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: message queue could not be released (%i)\n", device->name, device->channelNo+1, retVal);
    /* destroy the pipe */
    retVal = CANPIP_Destroy(device->recvData.msgPipe);
//    if (retVal < 0)
//        MACCAN_DEBUG_ERROR("+++ %s CAN%u: pipe could not be released (%i)\n", device->name, device->channelNo+1, retVal);
    /* Live long and prosper! */
    device->handle = CANUSB_INVALID_HANDLE;
    device->recvData.msgQueue = NULL;
    device->recvData.msgPipe = NULL;
    device->recvPipe = NULL;
    device->configured = false;
    
    return retVal;
}

CANUSB_Return_t KvaserUSB_SendRequest(KvaserUSB_Device_t *device, const uint8_t *buffer, uint32_t nbyte) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    /* sanity check */
    if (!device || !buffer)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;
    
    retVal = CANUSB_WritePipe(device->handle, device->endpoints.bulkOut.pipeRef, buffer, nbyte, 0U);  // FIXME: time-out
    if (retVal == CANUSB_SUCCESS)
        MACCAN_LOG_WRITE((uint8_t*)buffer, nbyte, ">");
    else
        MACCAN_LOG_PRINTF("error(%i)\n", retVal);
    return retVal;
}

CANUSB_Return_t KvaserUSB_ReadResponse(KvaserUSB_Device_t *device, uint8_t *buffer, uint32_t nbyte, uint8_t cmdCode, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device || !buffer)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;
    if (nbyte < KVASER_MIN_COMMAND_LENGTH)
        return CANUSB_ERROR_ILLPARA;

    /* command response:
     * - byte 0: command length
     * - byte 1: command code
     * - byte 2: transaction id.
     * - byte 3: variable field
     * - byte 4.. max. 31: data
     */
    do {
        retVal = CANPIP_Read(device->recvData.msgPipe, &buffer[0], KVASER_MIN_COMMAND_LENGTH, timeout);
        if (retVal != CANUSB_SUCCESS)
            break;
        if (buffer[0] < KVASER_MIN_COMMAND_LENGTH)  /* then we have a serious problem */
            break;
        retVal = CANPIP_Read(device->recvData.msgPipe, &buffer[KVASER_MIN_COMMAND_LENGTH], (buffer[0] - KVASER_MIN_COMMAND_LENGTH), 0U);
        if (retVal != CANUSB_SUCCESS)
            break;
#if (0)
        else if (buffer[1] == cmdCode)
            MACCAN_LOG_WRITE(buffer, nbyte, "!");
        else
            MACCAN_LOG_WRITE(buffer, nbyte, "#");
#endif
    } while (buffer[1] != cmdCode);

    return retVal;
}

CANUSB_Return_t KvaserUSB_StartReception(KvaserUSB_Device_t *device, CANUSB_Callback_t callback) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* start asynchronous read on endpoint */
    retVal = CANUSB_ReadPipeAsyncStart(device->recvPipe, callback, (void*)&device->recvData);
//    if (retVal < 0)
//        MACCAN_DEBUG_ERROR("+++ %s #%u: reception loop could not be started (%i)\n", device->name, device->channelNo, retVal);
    
    return retVal;
}

CANUSB_Return_t KvaserUSB_AbortReception(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* stop asynchronous read on endpoint */
    retVal = CANUSB_ReadPipeAsyncAbort(device->recvPipe);
//    if (retVal < 0)
//        MACCAN_DEBUG_ERROR("+++ %s #%u: reception loop could not be stopped (%i)\n", device->name, device->channelNo, retVal);
    
    return retVal;
}

uint64_t KvaserUSB_NanosecondsFromTicks(KvaserUSB_CpuTicks_t cpuTicks, KvaserUSB_CpuClock_t cpuFreq) {
    /*
     *  param[in]   cpuTicks   - 48-bit timer value from device
     *  param[in]   cpuFreq    - CPU frequency in [MHz]
     */
    if (cpuFreq == 0) cpuFreq = 1;  // to avoid devide-by-zero!
    
    return ((uint64_t)cpuTicks * (uint64_t)1000) / (uint64_t)cpuFreq;
}

void KvaserUSB_TimestampFromTicks(KvaserUSB_Timestamp_t *timeStamp, KvaserUSB_CpuTicks_t cpuTicks, KvaserUSB_CpuClock_t cpuFreq) {
    /*
     *  param[out]  timeStamp  - struct timespec (with nanoseconds resolution)
     *  param[in]   cpuTicks   - 48-bit timer value from device
     *  param[in]   cpuFreq    - CPU frequency in [MHz]
     */
    assert(timeStamp);
    
    uint64_t nsec = KvaserUSB_NanosecondsFromTicks(cpuTicks, cpuFreq);
    
    timeStamp->tv_sec = (time_t)(nsec / 1000000000ULL);
    timeStamp->tv_nsec =  (long)(nsec % 1000000000ULL);
}
