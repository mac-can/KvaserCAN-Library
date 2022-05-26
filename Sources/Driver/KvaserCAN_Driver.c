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
#include "KvaserCAN_Driver.h"

#include "KvaserCAN_Devices.h"
#include "KvaserUSB_LeafDevice.h"
#include "KvaserUSB_MhydraDevice.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

CANUSB_Return_t KvaserCAN_ProbeChannel(KvaserUSB_Channel_t channel, const KvaserUSB_OpMode_t opMode, int *state) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    KvaserUSB_OpMode_t opCapa = CANMODE_DEFAULT;
    uint16_t productId = 0xFFFFU;

    /* test USB device at given index (channel) if it is present and possibly opened */
    retVal = KvaserUSB_ProbeUsbDevice(channel, &productId);
    if (retVal < 0) {
        if (state)
            *state = CANUSB_BOARD_NOT_AVAILABLE;
        return CANUSB_SUCCESS;
    } else {
        if (state)
            *state = retVal > 0 ? CANUSB_BOARD_OCCUPIED : CANUSB_BOARD_AVAILABLE;
        retVal = CANUSB_SUCCESS;
    }
    /* get operation capability of the appropriate CAN controller */
    /* note: we cannot ask the device itself because it is not initialized at this point
     *       and we cannot/will not initialize it only to read its operation capability.
     *       so we have to take the preconfigured operation capability properties.
     */
    opCapa |= KvaserDEV_IsCanFdSupported(productId) ? CANMODE_FDOE : 0x00U;
    opCapa |= KvaserDEV_IsCanFdSupported(productId) ? CANMODE_BRSE : 0x00U;
    opCapa |= KvaserDEV_IsNonIsoCanFdSupported(productId) ? CANMODE_NISO : 0x00U;
    opCapa |= KvaserDEV_IsErrorFrameSupported(productId) ? CANMODE_ERR : 0x00U;
    opCapa |= KvaserDEV_IsSilentModeSupported(productId) ? CANMODE_MON : 0x00U;
    opCapa |= CANMODE_NXTD;
    opCapa |= CANMODE_NRTR;
    /* check given operation mode against the operation capability */
    if ((opMode & ~opCapa) != 0) {
        retVal = CANUSB_ERROR_ILLPARA;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_InitializeChannel(KvaserUSB_Channel_t channel, const KvaserUSB_OpMode_t opMode, KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* open USB device at given index (channel) and allocate required resources (pipe context) */
    /* note: the device context is preinitialized, but must be confirmed by the CAN channel */
    retVal = KvaserUSB_OpenUsbDevice(channel, device);
    if (retVal < 0) {
        return retVal;
    }
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            /* configure and confirm device context for Mhydra compatible device and
             * initialize the CAN channel (CAN controller is in INIT state) */
            if (Mhydra_ConfigureChannel(device))
                retVal = Mhydra_InitializeChannel(device, opMode);
            else
                retVal = CANUSB_ERROR_NOTINIT;
            break;
        case USB_LEAF_DRIVER:
            /* configure and confirm device context for Leaf compatible device and
             * initialize the CAN channel (CAN controller is in INIT state) */
            if (Leaf_ConfigureChannel(device))
                retVal = Leaf_InitializeChannel(device, opMode);
            else
                retVal = CANUSB_ERROR_NOTINIT;
            break;
        default:
            /* oh, an unsupported Kvaser USB device was found!
             * Send me some Dollars and I will give my best. */
            retVal = CANUSB_ERROR_NOTINIT;
    }
    if (retVal < 0) {
        (void)KvaserUSB_CloseUsbDevice(device);
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_TeardownChannel(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* teardown the whole ... */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_TeardownChannel(device);
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_TeardownChannel(device);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    /* close USB device and release all resources */
    /* note: don't care about the result */
    (void)KvaserUSB_CloseUsbDevice(device);

    return retVal;
}

CANUSB_Return_t KvaserCAN_SignalChannel(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* signal wait condition */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
        case USB_LEAF_DRIVER:
            retVal = CANQUE_Signal(device->recvData.msgQueue);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* set bus parameters */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_SetBusParams(device, params);
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_SetBusParams(device, params);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* get bus parameters */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_GetBusParams(device, params);
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_GetBusParams(device, params);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_SetBusParamsFd(KvaserUSB_Device_t *device, const KvaserUSB_BusParamsFd_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* set bus parameters */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_SetBusParamsFd(device, params);
            break;
        case USB_LEAF_DRIVER:
            retVal = CANUSB_ERROR_NOTSUPP;
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_GetBusParamsFd(KvaserUSB_Device_t *device, KvaserUSB_BusParamsFd_t *params) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* get bus parameters */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_GetBusParamsFd(device, params);
            break;
        case USB_LEAF_DRIVER:
            retVal = CANUSB_ERROR_NOTSUPP;
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}


CANUSB_Return_t KvaserCAN_CanBusOn(KvaserUSB_Device_t *device, bool silent) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* start CAN controller */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            // TODO: (void)Mhydra_ResetStatistics(device, KVASER_USB_REQUEST_DELAY);
            // TODO: (void)Mhydra_ResetErrorCounter(device, KVASER_USB_REQUEST_DELAY);
            retVal = Mhydra_SetDriverMode(device, silent ? DRIVERMODE_SILENT : DRIVERMODE_NORMAL);
            if (retVal == CANUSB_SUCCESS)
                retVal = Mhydra_StartChip(device, KVASER_USB_COMMAND_TIMEOUT);
           break;
        case USB_LEAF_DRIVER:
            (void)Leaf_ResetStatistics(device, KVASER_USB_REQUEST_DELAY);
            (void)Leaf_ResetErrorCounter(device, KVASER_USB_REQUEST_DELAY);
            retVal = Leaf_SetDriverMode(device, silent ? DRIVERMODE_SILENT : DRIVERMODE_NORMAL);
            if (retVal == CANUSB_SUCCESS)
                retVal = Leaf_StartChip(device, KVASER_USB_COMMAND_TIMEOUT);
           break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_CanBusOff(KvaserUSB_Device_t *device) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* reset CAN controller */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_StopChip(device, KVASER_USB_COMMAND_TIMEOUT);
            if (retVal == CANUSB_SUCCESS)
                retVal = Mhydra_SetDriverMode(device, DRIVERMODE_NORMAL/*_OFF*/);  // OFF doesn't work
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_StopChip(device, KVASER_USB_COMMAND_TIMEOUT);
            if (retVal == CANUSB_SUCCESS)
                retVal = Leaf_SetDriverMode(device, DRIVERMODE_OFF);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_WriteMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* send a CAN message */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_SendMessage(device, message, timeout);
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_SendMessage(device, message, timeout);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* read a CAN message from message queue, if any */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_ReadMessage(device, message, timeout);
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_ReadMessage(device, message, timeout);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_GetBusStatus(KvaserUSB_Device_t *device, KvaserUSB_BusStatus_t *status) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* get CAN bus status */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_RequestChipState(device, KVASER_USB_REQUEST_DELAY);
            if (status) {
                *status = device->recvData.evData.chipState.busStatus;
            }
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_RequestChipState(device, KVASER_USB_REQUEST_DELAY);
            if (status) {
                *status = device->recvData.evData.chipState.busStatus;
            }
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

CANUSB_Return_t KvaserCAN_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load) {
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    /* sanity check */
    if (!device)
        return CANUSB_ERROR_NULLPTR;
    if (!device->configured)
        return CANUSB_ERROR_NOTINIT;

    /* call device specific function */
    switch (device->driverType) {
        case USB_MHYDRA_DRIVER:
            retVal = Mhydra_GetBusLoad(device, load);
            break;
        case USB_LEAF_DRIVER:
            retVal = Leaf_GetBusLoad(device, load);
            break;
        default:
            retVal = CANUSB_ERROR_FATAL;
            break;
    }
    return retVal;
}

/* ---  MacCAN IOUsbKit initialization  ---
 */
CANUSB_Return_t KvaserCAN_InitializeDriver(void) {
    /* initialize the MacCAN IOUsbKit */
    return CANUSB_Initialize();
}

CANUSB_Return_t KvaserCAN_TeardownDriver(void) {
    /* release the MacCAN IOUsbKit */
    return CANUSB_Teardown();
}

uint32_t KvaserCAN_DriverVersion(void) {
    /* version of the MacCAN IOUsbKit */
    return CANUSB_GetVersion();
}

/* ---  mother's little helper  ---
 */
uint8_t KvaserCAN_Dlc2Len(uint8_t dlc) {
    const static uint8_t dlc_table[16] = {
#if (OPTION_CAN_2_0_ONLY == 0)
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U
#else
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 8U, 8U, 8U, 8U, 8U, 8U, 8U
#endif
    };
    return dlc_table[dlc & 0xFU];
}

uint8_t KvaserCAN_Len2Dlc(uint8_t len) {
#if (OPTION_CAN_2_0_ONLY == 0)
    if(len > 48U) return 0x0FU;
    if(len > 32U) return 0x0EU;
    if(len > 24U) return 0x0DU;
    if(len > 20U) return 0x0CU;
    if(len > 16U) return 0x0BU;
    if(len > 12U) return 0x0AU;
    if(len > 8U) return 0x09U;
#else
    if(len > 8U) return 0x08U;
#endif
    return len;
}
