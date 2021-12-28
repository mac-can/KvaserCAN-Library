/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  CAN Interface API, Version 3 (for Kvaser CAN Leaf Interfaces)
 *
 *  Copyright (c) 2017-2021 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *  All rights reserved.
 *
 *  This file is part of MacCAN-KvaserCAN.
 *
 *  MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v3.0 (or any later version). You can
 *  choose between one of them if you use MacCAN-KvaserCAN in whole or in part.
 *
 *  BSD 2-Clause "Simplified" License:
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  MacCAN-KvaserCAN IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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
/** @addtogroup  can_api
 *  @{
 */
#include "build_no.h"
#define VERSION_MAJOR    0
#define VERSION_MINOR    1
#define VERSION_PATCH    2
#define VERSION_BUILD    BUILD_NO
#define VERSION_STRING   TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH) " (" TOSTRING(BUILD_NO) ")"
#if defined(__APPLE__)
#define PLATFORM        "macOS"
#else
#error Unsupported architecture
#endif
static const char version[] = "CAN API V3 for Kvaser CAN Leaf Interfaces, Version " VERSION_STRING;

/*  -----------  includes  -----------------------------------------------
 */
#include "can_defs.h"
#include "can_api.h"
#include "can_btr.h"

#include "KvaserCAN_Driver.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

/*  -----------  options  ------------------------------------------------
 */
#if (OPTION_CANAPI_KVASERCAN_DYLIB != 0)
__attribute__((constructor))
static void _initializer() {
}
__attribute__((destructor))
static void _finalizer() {
}
#define EXPORT  __attribute__((visibility("default")))
#else
#define EXPORT
#endif

/*  -----------  defines  ------------------------------------------------
 */
#ifndef KVASER_MAX_HANDLES
#define KVASER_MAX_HANDLES      (8)     // maximum number of open handles
#endif
#define INVALID_HANDLE          (-1)
#define IS_HANDLE_VALID(hnd)    ((0 <= (hnd)) && ((hnd) < KVASER_MAX_HANDLES))
#ifndef DLC2LEN
#define DLC2LEN(x)              dlc_table[(x) & 0xF]
#endif
#ifndef LEN2DLC
#define LEN2DLC(x)              ((x) > 48) ? 0xF : \
                                ((x) > 32) ? 0xE : \
                                ((x) > 24) ? 0xD : \
                                ((x) > 20) ? 0xC : \
                                ((x) > 16) ? 0xB : \
                                ((x) > 12) ? 0xA : \
                                ((x) > 8) ?  0x9 : (x)
#endif

/*  -----------  types  --------------------------------------------------
 */
typedef struct {                        // frame counters:
    uint64_t tx;                        //   number of transmitted CAN frames
    uint64_t rx;                        //   number of received CAN frames
    uint64_t err;                       //   number of receiced error frames
}   can_counter_t;

typedef struct {                        // Kvaser CAN interface:
    KvaserUSB_Device_t device;          //   USB device handle
    can_mode_t mode;                    //   CAN operation mode
    can_status_t status;                //   8-bit status register
    can_counter_t counters;             //   statistical counters
}   can_interface_t;

/*  -----------  prototypes  ---------------------------------------------
 */
static int map_bitrate2busparams(const can_bitrate_t *bitrate, KvaserUSB_BusParams_t *busParams);
static int map_busparams2bitrate(const KvaserUSB_BusParams_t *busParams, can_bitrate_t *bitrate);
static int map_bitrate2busparams_fd(const can_bitrate_t *bitrate, KvaserUSB_BusParamsFd_t *busParams);
static int map_busparams2bitrate_fd(const KvaserUSB_BusParamsFd_t *busParams, can_bitrate_t *bitrate);
static int lib_parameter(uint16_t param, void *value, size_t nbyte);
static int drv_parameter(int handle, uint16_t param, void *value, size_t nbyte);

/*  -----------  variables  ----------------------------------------------
 */
EXPORT
can_board_t can_boards[8+1] = {  // list of supported CAN Interfaces
    // TODO: rework this (either by an own can_defs.h or by a json file)
    {KVASER_CAN_CHANNEL0, (char *)"Kvaser CAN Channel 0"},
    {KVASER_CAN_CHANNEL1, (char *)"Kvaser CAN Channel 1"},
    {KVASER_CAN_CHANNEL2, (char *)"Kvaser CAN Channel 2"},
    {KVASER_CAN_CHANNEL3, (char *)"Kvaser CAN Channel 3"},
    {KVASER_CAN_CHANNEL4, (char *)"Kvaser CAN Channel 4"},
    {KVASER_CAN_CHANNEL5, (char *)"Kvaser CAN Channel 5"},
    {KVASER_CAN_CHANNEL6, (char *)"Kvaser CAN Channel 6"},
    {KVASER_CAN_CHANNEL7, (char *)"Kvaser CAN Channel 7"},
    {EOF, NULL}
};
//static const uint8_t dlc_table[16] = {  // DLC to length
//    0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64
//};
static can_interface_t can[KVASER_MAX_HANDLES]; // interface handles
static int init =  0;  // initialization flag

/*  -----------  functions  ----------------------------------------------
 */
EXPORT
int can_test(int32_t channel, uint8_t mode, const void *param, int *result)
{
    int rc = CANERR_FATAL;              // return value
    int i;

    if (result)                         // the last resort
        *result = CANBRD_NOT_TESTABLE;
    if (!init) {                        // when not initialized:
        for (i = 0; i < KVASER_MAX_HANDLES; i++) {
            memset(&can[i], 0, sizeof(can_interface_t));
            can[i].device.configured = false;
            can[i].mode.byte = CANMODE_DEFAULT;
            can[i].status.byte = CANSTAT_RESET;
        }
        // initialize the driver (MacCAN-Core driver)
        if ((rc = KvaserCAN_InitializeDriver()) != CANERR_NOERROR)
            return rc;
        init = 1;
    }
    if (!init)                          // must be initialized
        return CANERR_FATAL;
    if (!IS_HANDLE_VALID(channel))      // must be a valid channel!
#ifndef OPTION_CANAPI_RETVALS
        return CANERR_HANDLE;
#else
        // note: can_test shall return vendor-specific error code or
        //       CANERR_NOTINIT in this case
        return CANERR_NOTINIT;
#endif
    // attention: check first CAN FD operation dependent mode flags
    bool wrong = (!(mode & CANMODE_FDOE) && ((mode & CANMODE_BRSE) || (mode & CANMODE_NISO))) ? true : false;
    // probe the CAN channel and check it selected operation mode is supported by the CAN controller
    rc = KvaserCAN_ProbeChannel(channel, mode, result);
    // note: 1. parameter 'result' is checked for NULL pointer by the called function
    //       2. error code CANERR_ILLPARA is return in case the operation mode is not supported
    (void) param;
    return ((rc == CANERR_NOERROR) && wrong) ? (int)CANERR_ILLPARA : (int)rc;
}

EXPORT
int can_init(int32_t channel, uint8_t mode, const void *param)
{
    int rc = CANERR_FATAL;              // return value
    int i;

    if (!init) {                        // when not initialized:
        for (i = 0; i < KVASER_MAX_HANDLES; i++) {
            memset(&can[i], 0, sizeof(can_interface_t));
            can[i].device.configured = false;
            can[i].mode.byte = CANMODE_DEFAULT;
            can[i].status.byte = CANSTAT_RESET;
        }
        // initialize the driver (MacCAN-Core driver)
        if ((rc = KvaserCAN_InitializeDriver()) != CANERR_NOERROR)
            return rc;
        init = 1;
    }
    if (!init)                          // must be initialized
        return CANERR_FATAL;
    if (!IS_HANDLE_VALID(channel))      // must be a valid channel!
#ifndef OPTION_CANAPI_RETVALS
        return CANERR_HANDLE;
#else
        // note: can_init shall return vendor-specific error code or
        //       CANERR_NOTINIT in this case
        return CANERR_NOTINIT;
#endif
    // attention: check first CAN FD operation dependent mode flags
    if (!(mode & CANMODE_FDOE) && ((mode & CANMODE_BRSE) || (mode & CANMODE_NISO)))
        return CANERR_ILLPARA;
    // initialize CAN channel with selected operation mode
    if ((rc = KvaserCAN_InitializeChannel(channel, mode, &can[channel].device)) < CANERR_NOERROR)
        return rc;
    can[channel].mode.byte = mode;      // store selected operation mode
    can[channel].status.byte = CANSTAT_RESET; // CAN not started yet
    (void)param;
    return (int)channel;                // return the handle (channel)
}

EXPORT
int can_exit(int handle)
{
    int rc = CANERR_FATAL;              // return value
    int i;

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (handle != CANEXIT_ALL) {
        if (!IS_HANDLE_VALID(handle))   // must be a valid handle
            return CANERR_HANDLE;
        if (!can[handle].device.configured) // must be an opened handle
            return CANERR_HANDLE;
        /*if (!can[handle].status.can_stopped) // go to CAN INIT mode (bus off)*/
            (void)KvaserCAN_CanBusOff(&can[handle].device);
        if ((rc = KvaserCAN_TeardownChannel(&can[handle].device)) < CANERR_NOERROR)
            return rc;
        can[handle].status.byte |= CANSTAT_RESET; // CAN controller in INIT state
        can[handle].device.configured = false;    // handle can be used again
    }
    else {
        for (i = 0; i < KVASER_MAX_HANDLES; i++) {
            if (can[i].device.configured) // must be an opened handle
            {
                /*if (!can[handle].status.can_stopped) // go to CAN INIT mode (bus off)*/
                    (void)KvaserCAN_CanBusOff(&can[i].device);
                (void)KvaserCAN_TeardownChannel(&can[i].device);
                can[i].status.byte |= CANSTAT_RESET; // CAN controller in INIT state
                can[i].device.configured = false;    // handle can be used again
            }
        }
    }
    // teardown the driver when all interfaces released
    for (i = 0; i < KVASER_MAX_HANDLES; i++) {
        if (can[i].device.configured)
            break;
    }
    if (i == KVASER_MAX_HANDLES) {
        (void)KvaserCAN_TeardownDriver();
        init = 0;
    }
    return CANERR_NOERROR;
}

EXPORT
int can_kill(int handle)
{
    int rc = CANERR_FATAL;              // return value
    int i;

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (handle != CANEXIT_ALL) {
        if (!IS_HANDLE_VALID(handle))   // must be a valid handle
            return CANERR_HANDLE;
        if (!can[handle].device.configured) // must be an opened handle
            return CANERR_HANDLE;
        if ((rc = KvaserCAN_SignalChannel(&can[handle].device)) < CANERR_NOERROR)
            return rc;
    }
    else {
        for (i = 0; i < KVASER_MAX_HANDLES; i++) {
            if (can[i].device.configured) // must be an opened handle
                (void)KvaserCAN_SignalChannel(&can[i].device);
        }
    }
    return CANERR_NOERROR;
}

EXPORT
int can_start(int handle, const can_bitrate_t *bitrate)
{
    int rc = CANERR_FATAL;              // return value

    can_bitrate_t tmpBitrate;           // bit-rate settings
    can_speed_t tmpSpeed;               // transmission speed
    KvaserUSB_BusParams_t busParams;    // Kvaser bus parameter
    KvaserUSB_BusParamsFd_t busParamsFd;// Kvaser FD bus parameter

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;
    if (bitrate == NULL)                // check for null-pointer
        return CANERR_NULLPTR;
    if (!can[handle].status.can_stopped) // must be stopped
        return CANERR_ONLINE;

    memcpy(&tmpBitrate, bitrate, sizeof(can_bitrate_t));
    memset(&tmpSpeed, 0, sizeof(can_speed_t));
    memset(&busParams, 0, sizeof(KvaserUSB_BusParams_t));
    memset(&busParamsFd, 0, sizeof(KvaserUSB_BusParamsFd_t));
    bool fdoe = can[handle].mode.fdoe ? true : false;
    bool brse = can[handle].mode.brse ? true : false;

    // CAN 2.0 operation mode:
    if (!can[handle].mode.fdoe) {
        // (a) check bit-rate settings (possibly after conversion from index)
        if (bitrate->btr.frequency <= 0) {
            // note: bit-rate settings are checked by the conversion function
            if (btr_index2bitrate(bitrate->index, &tmpBitrate) < 0)
                return CANERR_BAUDRATE;
        } else {
            // note: bit-rate settings are checked by the conversion function
            if (btr_bitrate2speed(&tmpBitrate, fdoe, brse, &tmpSpeed) < 0)
                return CANERR_BAUDRATE;
        }
        // (b) convert bit-rate settings to Kvaser bus parameter
        if (map_bitrate2busparams(&tmpBitrate, &busParams) < 0)
            return CANERR_BAUDRATE;
        // (c) set bit-rate (with respect of the selected operation mode)
        if ((rc = KvaserCAN_SetBusParams(&can[handle].device, &busParams)) < 0)
            return (rc != CANUSB_ERROR_ILLPARA) ? rc : CANERR_BAUDRATE;
    }
    // CAN FD operation mode:
    else {
        // (a) check bit-rate settings (w/0 conversion from index)
        if (bitrate->btr.frequency <= 0) {
            return CANERR_BAUDRATE;
        } else {
            // note: bit-rate settings are checked by the conversion function
            if (btr_bitrate2speed(&tmpBitrate, fdoe, brse, &tmpSpeed) < 0)
                return CANERR_BAUDRATE;
        }
        // (b) convert bit-rate settings to Kvaser bus parameter
        if (map_bitrate2busparams_fd(&tmpBitrate, &busParamsFd) < 0)
            return CANERR_BAUDRATE;
        // (c) set bit-rate (with respect of the selected operation mode)
        if ((rc = KvaserCAN_SetBusParamsFd(&can[handle].device, &busParamsFd)) < 0)
            return (rc != CANUSB_ERROR_ILLPARA) ? rc : CANERR_BAUDRATE;
    }
    // (d) clear status, counters, and the receive queue
    can[handle].status.byte = CANSTAT_RESET;
    can[handle].counters.tx = 0U;
    can[handle].counters.rx = 0U;
    can[handle].counters.err = 0U;
    (void)CANQUE_Reset(can[handle].device.recvData.msgQueue);
    // (e) start the CAN controller with the selected operation mode
    rc = KvaserCAN_CanBusOn(&can[handle].device, can[handle].mode.mon ? true : false);
    can[handle].status.can_stopped = (rc == CANUSB_SUCCESS) ? 0 : 1;
    return rc;
}

EXPORT
int can_reset(int handle)
{
    int rc = CANERR_FATAL;              // return value

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;
    if (can[handle].status.can_stopped) // must be running
#ifndef OPTION_CANAPI_RETVALS
        return CANERR_OFFLINE;
#else
        // note: can_reset shall return CANERR_NOERROR even when
        //       the CAN controller has not been started
        return CANERR_NOERROR;
#endif
    // stop the CAN controller (INIT state)
    rc = KvaserCAN_CanBusOff(&can[handle].device);
    can[handle].status.can_stopped = (rc == CANUSB_SUCCESS) ? 1 : 0;
    return rc;
}

EXPORT
int can_write(int handle, const can_message_t *message, uint16_t timeout)
{
    int rc = CANERR_FATAL;              // return value

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;
    if (message == NULL)                // check for null-pointer
        return CANERR_NULLPTR;
    if (can[handle].status.can_stopped) // must be running
        return CANERR_OFFLINE;

    if (message->id > (uint32_t)(message->xtd ? CAN_MAX_XTD_ID : CAN_MAX_STD_ID))
        return CANERR_ILLPARA;          // invalid identifier
    if (message->xtd && can[handle].mode.nxtd)
        return CANERR_ILLPARA;          // suppress extended frames
    if (message->rtr && can[handle].mode.nrtr)
        return CANERR_ILLPARA;          // suppress remote frames
    if (message->fdf && !can[handle].mode.fdoe)
        return CANERR_ILLPARA;          // long frames only with CAN FD
    if (message->brs && !can[handle].mode.brse)
        return CANERR_ILLPARA;          // fast frames only with CAN FD
    if (message->brs && !message->fdf)
        return CANERR_ILLPARA;          // bit-rate switching only with CAN FD
    if (message->sts)
        return CANERR_ILLPARA;          // error frames cannot be sent
    if (message->dlc > (uint8_t)(message->fdf ? CANFD_MAX_DLC : CAN_MAX_DLC))
        return CANERR_ILLPARA;          // invalid data length code

    // transmit the given CAN message (w/ or w/o acknowledgment)
    rc = KvaserCAN_WriteMessage(&can[handle].device, message, timeout);
    can[handle].status.transmitter_busy = (rc != CANUSB_SUCCESS) ? 1 : 0;
    can[handle].counters.tx += (rc == CANUSB_SUCCESS) ? 1U : 0U;
    return rc;
}

EXPORT
int can_read(int handle, can_message_t *message, uint16_t timeout)
{
    int rc = CANERR_FATAL;              // return value

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;
    if (message == NULL)                // check for null-pointer
        return CANERR_NULLPTR;
    if (can[handle].status.can_stopped) // must be running
        return CANERR_OFFLINE;

    // read one CAN message from the message queue, if any
    rc = KvaserCAN_ReadMessage(&can[handle].device, message, timeout);
    can[handle].status.receiver_empty = (rc != CANUSB_SUCCESS) ? 1 : 0;
    can[handle].status.queue_overrun = CANQUE_OverflowFlag(can[handle].device.recvData.msgQueue) ? 1 : 0;
    can[handle].counters.rx += ((rc == CANUSB_SUCCESS) && !message->sts) ? 1U : 0U;
    can[handle].counters.err += ((rc == CANUSB_SUCCESS) && message->sts) ? 1U : 0U;
    return rc;
}

EXPORT
int can_status(int handle, uint8_t *status)
{
    int rc = CANERR_FATAL;              // return value

    KvaserUSB_BusStatus_t busStatus = 0x00U;

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;

    // get status-register from device
    if ((rc = KvaserCAN_GetBusStatus(&can[handle].device, &busStatus)) == CANUSB_SUCCESS) {
        can[handle].status.bus_off = (busStatus & BUSSTAT_BUSOFF)? 1 : 0;
        can[handle].status.bus_error = (busStatus & BUSSTAT_ERROR_PASSIVE)? 1 : 0;
        can[handle].status.warning_level = (busStatus & BUSSTAT_ERROR_WARNING)? 1 : 0;
        // TODO: can[handle].status.message_lost |= (busStatus & canSTAT_RXERR)? 1 : 0;
        // TODO: can[handle].status.transmitter_busy |= (busStatus & canSTAT_TX_PENDING)? 1 : 0;
    }
    if (status)                         // status-register
      *status = can[handle].status.byte;

    return rc;
}

EXPORT
int can_busload(int handle, uint8_t *load, uint8_t *status)
{
    int rc = CANERR_FATAL;              // return value

    KvaserUSB_BusLoad_t busLoad = 0U;

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;

    // get bus load from device (0..10000 ==> 0%..100%)
    if ((rc = KvaserCAN_GetBusLoad(&can[handle].device, &busLoad)) == CANUSB_SUCCESS) {
        // get status-register from device
        rc = can_status(handle, status);
    }
    else
        busLoad = 0U;
    if (load)
        *load = (uint8_t)((busLoad + 50U) / 100U);
    return rc;
}

EXPORT
int can_bitrate(int handle, can_bitrate_t *bitrate, can_speed_t *speed)
{
    int rc = CANERR_FATAL;              // return value

    can_bitrate_t tmpBitrate;           // bit-rate settings
    can_speed_t tmpSpeed;               // transmission speed
    KvaserUSB_BusParams_t busParams;    // Kvaser bus parameter
    KvaserUSB_BusParamsFd_t busParamsFd;// Kvaser FD bus parameter

    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;

    memset(&tmpBitrate, 0, sizeof(can_bitrate_t));
    memset(&tmpSpeed, 0, sizeof(can_speed_t));
    memset(&busParams, 0, sizeof(KvaserUSB_BusParams_t));
    memset(&busParamsFd, 0, sizeof(KvaserUSB_BusParamsFd_t));
    bool fdoe = can[handle].mode.fdoe ? true : false;
    bool brse = can[handle].mode.brse ? true : false;

    // CAN 2.0 operation mode:
    if (!can[handle].mode.fdoe) {
        // get bit-rate settings from device
        if ((rc = KvaserCAN_GetBusParams(&can[handle].device, &busParams)) == CANUSB_SUCCESS) {
            if ((rc = map_busparams2bitrate(&busParams, &tmpBitrate)) == CANUSB_SUCCESS) {
                rc = btr_bitrate2speed(&tmpBitrate, fdoe, brse, &tmpSpeed);
            }
        }
    }
    // CAN FD operation mode:
    else {
        // get bit-rate settings from device
        if ((rc = KvaserCAN_GetBusParamsFd(&can[handle].device, &busParamsFd)) == CANUSB_SUCCESS) {
            if ((rc = map_busparams2bitrate_fd(&busParamsFd, &tmpBitrate)) == CANUSB_SUCCESS) {
                rc = btr_bitrate2speed(&tmpBitrate, fdoe, brse, &tmpSpeed);
            }
        }
    }
    if (bitrate)
        memcpy(bitrate, &tmpBitrate, sizeof(can_bitrate_t));
    if (speed)
        memcpy(speed, &tmpSpeed, sizeof(can_speed_t));
#ifdef OPTION_CANAPI_RETVALS
    // note: can_bitrate shall return CANERR_OFFLINE when
    //       the CAN controller has not been started
    if (can[handle].status.can_stopped)
        rc = CANERR_OFFLINE;
#endif
    return rc;
}

EXPORT
int can_property(int handle, uint16_t param, void *value, uint32_t nbyte)
{
    if (!init || !IS_HANDLE_VALID(handle)) {
        // note: library properties can be queried w/o a handle
        return lib_parameter(param, value, (size_t)nbyte);
    }
    if (!init)                          // must be initialized
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return CANERR_HANDLE;
    if (!can[handle].device.configured) // must be an opened handle
        return CANERR_HANDLE;
    // note: device properties must be queried with a valid handle
    return drv_parameter(handle, param, value, (size_t)nbyte);
}

EXPORT
char *can_hardware(int handle)
{
    static char string[CANPROP_MAX_BUFFER_SIZE] = "(unknown)";

    if (!init)                          // must be initialized
        return NULL;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return NULL;
    if (!can[handle].device.configured) // must be an opened handle
        return NULL;

    // get hardware version (zero-terminated string)
    uint8_t major = (uint8_t)can[handle].device.deviceInfo.card.hwRevision;
    uint8_t minor = (uint8_t)0;
#if (0)
    sprintf(string, "%s, hardware revision %u.%u", can[handle].device.name, major, minor);
#else
    uint8_t type = (uint8_t)can[handle].device.deviceInfo.card.hwType;
    sprintf(string, "%s, hardware revision %u.%u (type %u)", can[handle].device.name, major, minor, type);
#endif
    return string;
}

EXPORT
char *can_software(int handle)
{
    static char string[CANPROP_MAX_BUFFER_SIZE] = "(unknown)";

    if (!init)                          // must be initialized
        return NULL;
    if (!IS_HANDLE_VALID(handle))       // must be a valid handle
        return NULL;
    if (!can[handle].device.configured) // must be an opened handle
        return NULL;

    // get software version (zero-terminated string)
    uint8_t major = (uint8_t)(can[handle].device.deviceInfo.software.firmwareVersion >> 24);
    uint8_t minor = (uint8_t)(can[handle].device.deviceInfo.software.firmwareVersion >> 16);
#if (0)
    sprintf(string, "%s, firmware version %u.%u", can[handle].device.name, major, minor);
#else
    uint16_t build = (uint16_t)(can[handle].device.deviceInfo.software.firmwareVersion >> 0);
    sprintf(string, "%s, firmware version %u.%u (build %u)", can[handle].device.name, major, minor, build);
#endif
    return string;
}

/*  -----------  local functions  ----------------------------------------
 */
static int map_bitrate2busparams(const can_bitrate_t *bitrate, KvaserUSB_BusParams_t *busParams)
{
    // sanity check
    if (!bitrate || !busParams)
        return CANERR_NULLPTR;
    if ((bitrate->btr.nominal.brp < CANBTR_NOMINAL_BRP_MIN) || (CANBTR_NOMINAL_BRP_MAX < bitrate->btr.nominal.brp))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg1 < CANBTR_NOMINAL_TSEG1_MIN) || (CANBTR_NOMINAL_TSEG1_MAX < bitrate->btr.nominal.tseg1))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg2 < CANBTR_NOMINAL_TSEG2_MIN) || (CANBTR_NOMINAL_TSEG2_MAX < bitrate->btr.nominal.tseg2))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.sjw < CANBTR_NOMINAL_SJW_MIN) || (CANBTR_NOMINAL_SJW_MAX < bitrate->btr.nominal.sjw))
        return CANERR_BAUDRATE;
    if (bitrate->btr.nominal.brp == 0)   // devide-by-zero!
        return CANERR_BAUDRATE;

    // bit-rate = frequency / (brp * (1 + tseg1 + tseg2))
    busParams->bitRate = (int32_t)bitrate->btr.frequency /
        ((int32_t)bitrate->btr.nominal.brp * (1l + (int32_t)bitrate->btr.nominal.tseg1 + (int32_t)bitrate->btr.nominal.tseg2));
    busParams->tseg1 = (uint16_t)bitrate->btr.nominal.tseg1;
    busParams->tseg2 = (uint16_t)bitrate->btr.nominal.tseg2;
    busParams->sjw = (uint16_t)bitrate->btr.nominal.sjw;
    busParams->noSamp = (uint16_t)((bitrate->btr.nominal.sam != 0) ? 3 : 1);  // SJA1000: single or triple sampling
    //busParams->syncmode = (uint16_t)0;

    return CANERR_NOERROR;
}

static int map_busparams2bitrate(const KvaserUSB_BusParams_t *busParams, can_bitrate_t *bitrate)
{
    // sanity check
    if (!busParams || !bitrate)
        return CANERR_NULLPTR;

    // Kvaser canLib32 doesn't offer the used controller frequency and bit-rate prescaler.
    // We suppose it's running with 80MHz and calculate the bit-rate prescaler as follows:
    //
    // (1) brp = 80MHz / (bit-rate * (1 + tseg1 + tseq2))
    //
    if (busParams->bitRate <= 0)   // divide-by-zero!
        return CANERR_BAUDRATE;
    bitrate->btr.frequency = (int32_t)80000000;
    bitrate->btr.nominal.brp = (uint16_t)(80000000L
                             / (busParams->bitRate * (int32_t)(1u + busParams->tseg1 + busParams->tseg2)));
    bitrate->btr.nominal.tseg1 = (uint16_t)busParams->tseg1;
    bitrate->btr.nominal.tseg2 = (uint16_t)busParams->tseg2;
    bitrate->btr.nominal.sjw = (uint16_t)busParams->sjw;
    bitrate->btr.nominal.sam = (uint8_t)((busParams->noSamp < 3) ? 0 : 1);  // SJA1000: single or triple sampling
#if (OPTION_CAN_2_0_ONLY == 0)
    bitrate->btr.data.brp = bitrate->btr.nominal.brp;
    bitrate->btr.data.tseg1 = bitrate->btr.nominal.tseg1;
    bitrate->btr.data.tseg2 = bitrate->btr.nominal.tseg2;
    bitrate->btr.data.sjw = bitrate->btr.nominal.sjw;
#endif
    // range check
    if ((bitrate->btr.nominal.brp < CANBTR_NOMINAL_BRP_MIN) || (CANBTR_NOMINAL_BRP_MAX < bitrate->btr.nominal.brp))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg1 < CANBTR_NOMINAL_TSEG1_MIN) || (CANBTR_NOMINAL_TSEG1_MAX < bitrate->btr.nominal.tseg1))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg2 < CANBTR_NOMINAL_TSEG2_MIN) || (CANBTR_NOMINAL_TSEG2_MAX < bitrate->btr.nominal.tseg2))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.sjw < CANBTR_NOMINAL_SJW_MIN) || (CANBTR_NOMINAL_SJW_MAX < bitrate->btr.nominal.sjw))
        return CANERR_BAUDRATE;
    return CANERR_NOERROR;
}

static int map_bitrate2busparams_fd(const can_bitrate_t *bitrate, KvaserUSB_BusParamsFd_t *busParams)
{
    // sanity check
    if (!bitrate || !busParams)
        return CANERR_NULLPTR;
    if ((bitrate->btr.nominal.brp < CANBTR_NOMINAL_BRP_MIN) || (CANBTR_NOMINAL_BRP_MAX < bitrate->btr.nominal.brp))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg1 < CANBTR_NOMINAL_TSEG1_MIN) || (CANBTR_NOMINAL_TSEG1_MAX < bitrate->btr.nominal.tseg1))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg2 < CANBTR_NOMINAL_TSEG2_MIN) || (CANBTR_NOMINAL_TSEG2_MAX < bitrate->btr.nominal.tseg2))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.sjw < CANBTR_NOMINAL_SJW_MIN) || (CANBTR_NOMINAL_SJW_MAX < bitrate->btr.nominal.sjw))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.brp < CANBTR_DATA_BRP_MIN) || (CANBTR_DATA_BRP_MAX < bitrate->btr.data.brp))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.tseg1 < CANBTR_DATA_TSEG1_MIN) || (CANBTR_DATA_TSEG1_MAX < bitrate->btr.data.tseg1))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.tseg2 < CANBTR_DATA_TSEG2_MIN) || (CANBTR_DATA_TSEG2_MAX < bitrate->btr.data.tseg2))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.sjw < CANBTR_DATA_SJW_MIN) || (CANBTR_DATA_SJW_MAX < bitrate->btr.data.sjw))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.brp == 0) || (bitrate->btr.data.brp == 0))   // devide-by-zero!
        return CANERR_BAUDRATE;

    // arbitration phase: bit-rate = frequency / (brp * (1 + tseg1 + tseg2))
    busParams->nominal.bitRate = (int32_t)bitrate->btr.frequency /
        ((int32_t)bitrate->btr.nominal.brp * (1l + (int32_t)bitrate->btr.nominal.tseg1 + (int32_t)bitrate->btr.nominal.tseg2));
    busParams->nominal.tseg1 = (uint16_t)bitrate->btr.nominal.tseg1;
    busParams->nominal.tseg2 = (uint16_t)bitrate->btr.nominal.tseg2;
    busParams->nominal.sjw = (uint16_t)bitrate->btr.nominal.sjw;
    busParams->nominal.noSamp = (uint16_t)((bitrate->btr.nominal.sam != 0) ? 3 : 1);  // SJA1000: single or triple sampling
    //busParams->nominal.syncmode = (uint16_t)0;

    // data phase: bit-rate = frequency / (brp * (1 + tseg1 + tseg2))
    busParams->data.bitRate = (int32_t)bitrate->btr.frequency /
        ((int32_t)bitrate->btr.data.brp * (1l + (int32_t)bitrate->btr.data.tseg1 + (int32_t)bitrate->btr.data.tseg2));
    busParams->data.tseg1 = (uint16_t)bitrate->btr.data.tseg1;
    busParams->data.tseg2 = (uint16_t)bitrate->btr.data.tseg2;
    busParams->data.sjw = (uint16_t)bitrate->btr.data.sjw;
    busParams->data.noSamp = (uint16_t)((bitrate->btr.nominal.sam != 0) ? 3 : 1);  // SJA1000: single or triple sampling
    //busParams->data.syncmode = (uint16_t)0;

    // operate in CAN FD mode
    busParams->canFd = true;
    
    return CANERR_NOERROR;
}

static int map_busparams2bitrate_fd(const KvaserUSB_BusParamsFd_t *busParams, can_bitrate_t *bitrate)
{
    // sanity check
    if (!busParams || !bitrate)
        return CANERR_NULLPTR;

    // Kvaser canLib32 doesn't offer the used controller frequency and bit-rate prescaler.
    // We suppose it's running with 80MHz and calculate the bit-rate prescaler as follows:
    //
    // (1) brp = 80MHz / (bit-rate * (1 + tseg1 + tseq2))
    //
    if ((busParams->nominal.bitRate <= 0) || (busParams->data.bitRate <= 0))   // divide-by-zero!
        return CANERR_BAUDRATE;
    bitrate->btr.frequency = (int32_t)80000000;
    bitrate->btr.nominal.brp = (uint16_t)(80000000L
                             / (busParams->nominal.bitRate * (int32_t)(1u + busParams->nominal.tseg1 + busParams->nominal.tseg2)));
    bitrate->btr.nominal.tseg1 = (uint16_t)busParams->nominal.tseg1;
    bitrate->btr.nominal.tseg2 = (uint16_t)busParams->nominal.tseg2;
    bitrate->btr.nominal.sjw = (uint16_t)busParams->nominal.sjw;
    bitrate->btr.nominal.sam = (uint8_t)((busParams->nominal.noSamp < 3) ? 0 : 1);  // SJA1000: single or triple sampling
    bitrate->btr.data.brp = (uint16_t)(80000000L
                          / (busParams->data.bitRate * (int32_t)(1u + busParams->data.tseg1 + busParams->data.tseg2)));
    bitrate->btr.data.tseg1 = (uint16_t)busParams->data.tseg1;
    bitrate->btr.data.tseg2 = (uint16_t)busParams->data.tseg2;
    bitrate->btr.data.sjw = (uint16_t)busParams->data.sjw;
    // range check
    if ((bitrate->btr.nominal.brp < CANBTR_NOMINAL_BRP_MIN) || (CANBTR_NOMINAL_BRP_MAX < bitrate->btr.nominal.brp))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg1 < CANBTR_NOMINAL_TSEG1_MIN) || (CANBTR_NOMINAL_TSEG1_MAX < bitrate->btr.nominal.tseg1))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.tseg2 < CANBTR_NOMINAL_TSEG2_MIN) || (CANBTR_NOMINAL_TSEG2_MAX < bitrate->btr.nominal.tseg2))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.nominal.sjw < CANBTR_NOMINAL_SJW_MIN) || (CANBTR_NOMINAL_SJW_MAX < bitrate->btr.nominal.sjw))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.brp < CANBTR_DATA_BRP_MIN) || (CANBTR_DATA_BRP_MAX < bitrate->btr.data.brp))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.tseg1 < CANBTR_DATA_TSEG1_MIN) || (CANBTR_DATA_TSEG1_MAX < bitrate->btr.data.tseg1))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.tseg2 < CANBTR_DATA_TSEG2_MIN) || (CANBTR_DATA_TSEG2_MAX < bitrate->btr.data.tseg2))
        return CANERR_BAUDRATE;
    if ((bitrate->btr.data.sjw < CANBTR_DATA_SJW_MIN) || (CANBTR_DATA_SJW_MAX < bitrate->btr.data.sjw))
        return CANERR_BAUDRATE;
    return CANERR_NOERROR;
}

/*  - - - - - -  CAN API V3 properties  - - - - - - - - - - - - - - - - -
 */
static int lib_parameter(uint16_t param, void *value, size_t nbyte)
{
    int rc = CANERR_ILLPARA;            // suppose an invalid parameter

    static int idx_board = EOF;         // actual index in the interface list

    if (value == NULL) {                // check for null-pointer
        if ((param != CANPROP_SET_FIRST_CHANNEL) &&
           (param != CANPROP_SET_NEXT_CHANNEL))
            return CANERR_NULLPTR;
    }
    /* CAN library properties */
    switch (param) {
    case CANPROP_GET_SPEC:              // version of the wrapper specification (uint16_t)
        if (nbyte >= sizeof(uint16_t)) {
            *(uint16_t*)value = (uint16_t)CAN_API_SPEC;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_VERSION:           // version number of the library (uint16_t)
        if (nbyte >= sizeof(uint16_t)) {
            *(uint16_t*)value = ((uint16_t)VERSION_MAJOR << 8)
                              | ((uint16_t)VERSION_MINOR & 0xFu);
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_PATCH_NO:          // patch number of the library (uint8_t)
        if (nbyte >= sizeof(uint8_t)) {
            *(uint8_t*)value = (uint8_t)VERSION_PATCH;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_BUILD_NO:          // build number of the library (uint32_t)
        if (nbyte >= sizeof(uint32_t)) {
            *(uint32_t*)value = (uint32_t)VERSION_BUILD;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_LIBRARY_ID:        // library id of the library (int32_t)
        if (nbyte >= sizeof(int32_t)) {
            *(int32_t*)value = (int32_t)KVASER_LIB_ID;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_LIBRARY_VENDOR:    // vendor name of the library (char[256])
        if ((nbyte > strlen(CAN_API_VENDOR)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            strcpy((char*)value, CAN_API_VENDOR);
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_LIBRARY_DLLNAME:   // file name of the library (char[256])
        if ((nbyte > strlen(KVASER_LIB_WRAPPER)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            strcpy((char*)value, KVASER_LIB_WRAPPER);
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_DEVICE_VENDOR:     // vendor name of the CAN interface (char[256])
        if ((nbyte > strlen(KVASER_LIB_VENDOR)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            strcpy((char*)value, KVASER_LIB_VENDOR);
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_DEVICE_DLLNAME:    // file name of the CAN interface (char[256])
        if ((nbyte > strlen(KVASER_LIB_CANLIB)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            strcpy((char*)value, KVASER_LIB_CANLIB);
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_SET_FIRST_CHANNEL:     // set index to the first entry in the interface list (NULL)
        idx_board = 0;
        rc = (can_boards[idx_board].type != EOF) ? CANERR_NOERROR : CANERR_RESOURCE;
        break;
    case CANPROP_SET_NEXT_CHANNEL:      // set index to the next entry in the interface list (NULL)
        if ((0 <= idx_board) && (idx_board < KVASER_BOARDS)) {
            if (can_boards[idx_board].type != EOF)
                idx_board++;
            rc = (can_boards[idx_board].type != EOF) ? CANERR_NOERROR : CANERR_RESOURCE;
        }
        else
            rc = CANERR_RESOURCE;
        break;
    case CANPROP_GET_CHANNEL_TYPE:      // get device type at actual index in the interface list (int32_t)
        if (nbyte >= sizeof(int32_t)) {
            if ((0 <= idx_board) && (idx_board < KVASER_BOARDS) &&
                (can_boards[idx_board].type != EOF)) {
                *(int32_t*)value = (int32_t)can_boards[idx_board].type;
                rc = CANERR_NOERROR;
            }
            else
                rc = CANERR_RESOURCE;
        }
        break;
    case CANPROP_GET_CHANNEL_NAME:      // get device name at actual index in the interface list (char[256])
        if ((0U < nbyte) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            if ((0 <= idx_board) && (idx_board < KVASER_BOARDS) &&
                (can_boards[idx_board].type != EOF)) {
                strncpy((char*)value, can_boards[idx_board].name, nbyte);
                ((char*)value)[(nbyte - 1)] = '\0';
                rc = CANERR_NOERROR;
            }
            else
                rc = CANERR_RESOURCE;
        }
        break;
    case CANPROP_GET_CHANNEL_DLLNAME:   // get file name of the DLL at actual index in the interface list (char[256])
        if ((0U < nbyte) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            if ((0 <= idx_board) && (idx_board < KVASER_BOARDS) &&
                (can_boards[idx_board].type != EOF)) {
                strncpy((char*)value, KVASER_LIB_CANLIB, nbyte);
                ((char*)value)[(nbyte - 1)] = '\0';
                rc = CANERR_NOERROR;
            }
            else
                rc = CANERR_RESOURCE;
        }
        break;
    case CANPROP_GET_CHANNEL_VENDOR_ID: // get library id at actual index in the interface list (int32_t)
        if (nbyte >= sizeof(int32_t)) {
            if ((0 <= idx_board) && (idx_board < KVASER_BOARDS) &&
                (can_boards[idx_board].type != EOF)) {
                *(int32_t*)value = (int32_t)KVASER_LIB_ID;
                rc = CANERR_NOERROR;
            }
            else
                rc = CANERR_RESOURCE;
        }
        break;
    case CANPROP_GET_CHANNEL_VENDOR_NAME: // get vendor name at actual index in the interface list (char[256])
        if ((0U < nbyte) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            if ((0 <= idx_board) && (idx_board < KVASER_BOARDS) &&
                (can_boards[idx_board].type != EOF)) {
                strncpy((char*)value, KVASER_LIB_VENDOR, nbyte);
                ((char*)value)[(nbyte - 1)] = '\0';
                rc = CANERR_NOERROR;
            }
            else
                rc = CANERR_RESOURCE;
        }
        break;
    case CANPROP_GET_DEVICE_TYPE:       // device type of the CAN interface (int32_t)
    case CANPROP_GET_DEVICE_NAME:       // device name of the CAN interface (char[256])
    case CANPROP_GET_OP_CAPABILITY:     // supported operation modes of the CAN controller (uint8_t)
    case CANPROP_GET_OP_MODE:           // active operation mode of the CAN controller (uint8_t)
    case CANPROP_GET_BITRATE:           // active bit-rate of the CAN controller (can_bitrate_t)
    case CANPROP_GET_SPEED:             // active bus speed of the CAN controller (can_speed_t)
    case CANPROP_GET_STATUS:            // current status register of the CAN controller (uint8_t)
    case CANPROP_GET_BUSLOAD:           // current bus load of the CAN controller (uint8_t)
    case CANPROP_GET_TX_COUNTER:        // total number of sent messages (uint64_t)
    case CANPROP_GET_RX_COUNTER:        // total number of reveiced messages (uint64_t)
    case CANPROP_GET_ERR_COUNTER:       // total number of reveiced error frames (uint64_t)
        // note: a device parameter requires a valid handle.
        if (!init)
            rc = CANERR_NOTINIT;
        else
            rc = CANERR_HANDLE;
        break;
    default:
        rc = CANERR_NOTSUPP;
        break;
    }
    return rc;
}

static int drv_parameter(int handle, uint16_t param, void *value, size_t nbyte)
{
    int rc = CANERR_ILLPARA;            // suppose an invalid parameter
    can_bitrate_t bitrate;
    can_speed_t speed;
    uint8_t status;
    uint8_t load;

    assert(IS_HANDLE_VALID(handle));    // just to make sure

    if (value == NULL) {                // check for null-pointer
        if ((param != CANPROP_SET_FIRST_CHANNEL) &&
           (param != CANPROP_SET_NEXT_CHANNEL))
            return CANERR_NULLPTR;
    }
    /* CAN interface properties */
    switch (param) {
    case CANPROP_GET_DEVICE_TYPE:       // device type of the CAN interface (int32_t)
        if (nbyte >= sizeof(int32_t)) {
            *(int32_t*)value = (int32_t)can[handle].device.deviceInfo.card.hwType;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_DEVICE_NAME:       // device name of the CAN interface (char[256])
        if ((nbyte > strlen(can[handle].device.name)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            strcpy((char*)value, can[handle].device.name);
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_DEVICE_VENDOR:     // file name of the CAN interface DLL (char[256])
        if ((nbyte > strlen(can[handle].device.vendor)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            strcpy((char*)value, can[handle].device.vendor);
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_DEVICE_DLLNAME:    // vendor name of the CAN interface (char[256])
        if ((nbyte > strlen("(driverless)")) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
            strcpy((char*)value, "(driverless)");  // note: there is no kernel driver!
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_OP_CAPABILITY:     // supported operation modes of the CAN controller (uint8_t)
        if (nbyte >= sizeof(uint8_t)) {
            *(uint8_t*)value = (uint8_t)can[handle].device.opCapability;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_OP_MODE:           // active operation mode of the CAN controller (uint8_t)
        if (nbyte >= sizeof(uint8_t)) {
            *(uint8_t*)value = (uint8_t)can[handle].mode.byte;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_BITRATE:           // active bit-rate of the CAN controller (can_bitrate_t)
        if (nbyte >= sizeof(can_bitrate_t)) {
            if (((rc = can_bitrate(handle, &bitrate, NULL)) == CANERR_NOERROR) || (rc == CANERR_OFFLINE)) {
                memcpy(value, &bitrate, sizeof(can_bitrate_t));
                rc = CANERR_NOERROR;
            }
        }
        break;
    case CANPROP_GET_SPEED:             // active bus speed of the CAN controller (can_speed_t)
        if (nbyte >= sizeof(can_speed_t)) {
            if (((rc = can_bitrate(handle, NULL, &speed)) == CANERR_NOERROR) || (rc == CANERR_OFFLINE)) {
                memcpy(value, &speed, sizeof(can_speed_t));
                rc = CANERR_NOERROR;
            }
        }
        break;
    case CANPROP_GET_STATUS:            // current status register of the CAN controller (uint8_t)
        if (nbyte >= sizeof(uint8_t)) {
            if ((rc = can_status(handle, &status)) == CANERR_NOERROR) {
                *(uint8_t*)value = (uint8_t)status;
                rc = CANERR_NOERROR;
            }
        }
        break;
    case CANPROP_GET_BUSLOAD:           // current bus load of the CAN controller (uint8_t)
        if (nbyte >= sizeof(uint8_t)) {
            if ((rc = can_busload(handle, &load, NULL)) == CANERR_NOERROR) {
                *(uint8_t*)value = (uint8_t)load;
                rc = CANERR_NOERROR;
            }
        }
        break;
    case CANPROP_GET_TX_COUNTER:        // total number of sent messages (uint64_t)
        if (nbyte >= sizeof(uint64_t)) {
            *(uint64_t*)value = (uint64_t)can[handle].counters.tx;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_RX_COUNTER:        // total number of reveiced messages (uint64_t)
        if (nbyte >= sizeof(uint64_t)) {
            *(uint64_t*)value = (uint64_t)can[handle].counters.rx;
            rc = CANERR_NOERROR;
        }
        break;
    case CANPROP_GET_ERR_COUNTER:       // total number of reveiced error frames (uint64_t)
        if (nbyte >= sizeof(uint64_t)) {
            *(uint64_t*)value = (uint64_t)can[handle].counters.err;
            rc = CANERR_NOERROR;
        }
        break;
    default:
//        if ((CANPROP_GET_VENDOR_PROP <= param) &&  // get a vendor-specific property value (void*)
//           (param < (CANPROP_GET_VENDOR_PROP + CANPROP_VENDOR_PROP_RANGE))) {
//            if ((sts = canIoCtl(can[handle].handle, (unsigned int)(param - CANPROP_GET_VENDOR_PROP),
//                                                           (void*)value, (DWORD)nbyte)) == canOK)
//                rc = CANERR_NOERROR;
//            else
//                rc = kvaser_error(sts);
//        }
//        else if ((CANPROP_SET_VENDOR_PROP <= param) &&  // set a vendor-specific property value (void*)
//                (param < (CANPROP_SET_VENDOR_PROP + CANPROP_VENDOR_PROP_RANGE))) {
//            if ((sts = canIoCtl(can[handle].handle, (unsigned int)(param - CANPROP_SET_VENDOR_PROP),
//                                                           (void*)value, (DWORD)nbyte)) == canOK)
//                rc = CANERR_NOERROR;
//            else
//                rc = kvaser_error(sts);
//        }
//        else                            // or general library properties (see lib_parameter)
            rc = lib_parameter(param, value, nbyte);
        break;
    }
    return rc;
}


/*  -----------  revision control  ---------------------------------------
 */

EXPORT
char *can_version(void)
{
    return (char*)version;
}
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
