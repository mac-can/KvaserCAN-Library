//
//  CAN Interface API, Version 3 (for Kvaser CAN Leaf Interfaces)
//
//  Copyright (C) 2017-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//
//  This file is part of MacCAN-KvaserCAN.
//
//  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MacCAN-KvaserCAN.  If not, see <http://www.gnu.org/licenses/>.
//
#include "build_no.h"
#define VERSION_MAJOR    0
#define VERSION_MINOR    1
#define VERSION_PATCH    0
#define VERSION_BUILD    BUILD_NO
#define VERSION_STRING   TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH) " (" TOSTRING(BUILD_NO) ")"
#if defined(__APPLE__)
#define PLATFORM        "macOS"
#else
#error Unsupported architecture
#endif
static const char version[] = "CAN API V3 for Kvaser CAN Leaf Interfaces, Version " VERSION_STRING;

#include "can_defs.h"
#include "can_api.h"

#include "MacCAN.h"
#include "KvaserCAN.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#if (OPTION_CANAPI_KVASERCAN_DYLIB != 0)
__attribute__((constructor))
static void _initializer() {
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "[%s] [%s]\n", __FILE__, __FUNCTION__);
    MacCAN_Return_t retVal = CMacCAN::Initializer();
    fprintf(stdout, "CMacCAN::Initializer returned %i\n", retVal);
#else
    (void) CMacCAN::Initializer();
#endif
}
__attribute__((destructor))
static void _finalizer() {
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "[%s] [%s]\n", __FILE__, __FUNCTION__);
    MacCAN_Return_t retVal = CMacCAN::Finalizer();
    fprintf(stdout, "CMacCAN::Finalizer returned %i\n", retVal);
#else
    (void) CMacCAN::Finalizer();
#endif
}
#define EXPORT  __attribute__((visibility("default")))
#else
#define EXPORT
#endif
#ifndef CAN_MAX_HANDLES
#define CAN_MAX_HANDLES  16
#endif
#define IS_HANDLE_VALID(hnd)  ((0 <= (hnd)) && ((hnd) < CAN_MAX_HANDLES))

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
static CKvaserCAN canDevices[CAN_MAX_HANDLES];  // list of KvaserCAN device driver
static int init =  0;  // initialization flag

EXPORT
int can_test(int32_t channel, uint8_t mode, const void *param, int *result)
{
    CMacCAN::EChannelState state = CMacCAN::ChannelNotTestable;
    MacCAN_Return_t retVal = CMacCAN::FatalError;
    MacCAN_OpMode_t opMode = {
        .byte = mode
    };
    // probe if the CAN interface (hardware and driver) is present
    retVal = CKvaserCAN::ProbeChannel(channel, opMode, state);
    if (result) {
        switch (state) {
        case CMacCAN::ChannelOccupied:
            *result = CANBRD_OCCUPIED;
            break;
        case CMacCAN::ChannelAvailable:
            *result = CANBRD_PRESENT;
            break;
        case CMacCAN::ChannelNotAvailable:
            *result = CANBRD_NOT_PRESENT;
            break;
        default:
            *result = CANBRD_NOT_TESTABLE;
            break;
        }
    }
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN::ProbeChannel(%i): returned %i\n", channel, retVal);
#endif
    (void) param;
    return (int)retVal;
}

EXPORT
int can_init(int32_t channel, uint8_t mode, const void *param)
{
    int result = CANERR_HANDLE;
    MacCAN_Return_t retVal = CMacCAN::FatalError;
    MacCAN_OpMode_t opMode = {
        .byte = mode
    };
    if (!init) {
        // TODO: ...
        init = 1;
    }
    // sanity check
    if (!init)
        return CANERR_FATAL;
    if (!IS_HANDLE_VALID(channel))
        return CANERR_HANDLE;

    // initialize the CAN interface (hardware and driver)
    retVal = canDevices[channel].InitializeChannel(channel, opMode, param);
    if (retVal >= CANERR_NOERROR)
        result = (int)channel;  // on success the channel number is the handle
    else
        result = (int)retVal;   // on error we return the error code (< 0)!
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].InitializeChannel(%i): returned %i\n", channel, channel, retVal);
#endif
    (void) param;
    return result;
}

EXPORT
int can_exit(int handle)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (handle != CANEXIT_ALL) {
        if (!IS_HANDLE_VALID(handle))
            return CANERR_HANDLE;

        // stop any operation of the CAN interface
        retVal = canDevices[handle].TeardownChannel();
    } else {
        for (int i = 0; i < CAN_MAX_HANDLES; i++)
            (void) canDevices[i].TeardownChannel();
        // ignore induvidual error code
        retVal = CANERR_NOERROR;
    }
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].TeardownChannel: returned %i\n", handle, retVal);
#endif
    return (int)retVal;
}

EXPORT
int can_kill(int handle)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (handle != CANEXIT_ALL) {
        if (!IS_HANDLE_VALID(handle))
            return CANERR_HANDLE;

        // signal waitable objects (e.g. blocking operations)
        retVal = canDevices[handle].SignalChannel();
    } else {
        for (int i = 0; i < CAN_MAX_HANDLES; i++)
            (void) canDevices[i].SignalChannel();
        // ignore induvidual error code
        retVal = CANERR_NOERROR;
    }
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].SignalChannel: returned %i\n", handle, retVal);
#endif
    return (int)retVal;
}

EXPORT
int can_start(int handle, const can_bitrate_t *bitrate)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;
    if (!bitrate)
        return CANERR_NULLPTR;

    // start CAN communication (operation maode and bit-rate settings)
    retVal = canDevices[handle].StartController(*bitrate);
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].StartController: returned %i\n", handle, retVal);
#endif
    return (int)retVal;
}

EXPORT
int can_reset(int handle)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;

    // pause CAN communication
    retVal = canDevices[handle].ResetController();
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].ResetController: returned %i\n", handle, retVal);
#endif
    return (int)retVal;
}

EXPORT
int can_write(int handle, const can_message_t *message, uint16_t timeout)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;
    if (!message)
        return CANERR_NULLPTR;

    // send a CAN message
    retVal = canDevices[handle].WriteMessage(*message, timeout);
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].WriteMessage: returned %i\n", handle, retVal);
#endif
    return (int)retVal;
}

EXPORT
int can_read(int handle, can_message_t *message, uint16_t timeout)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;
    if (!message)
        return CANERR_NULLPTR;

    // read a CAN message from reception queue, if any
    retVal = canDevices[handle].ReadMessage(*message, timeout);
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    if (retVal != CMacCAN::ReceiverEmpty)
        fprintf(stdout, "CKvaserCAN[%i].ReadMessage: returned %i\n", handle, retVal);
#endif
    return (int)retVal;
}

EXPORT
int can_status(int handle, uint8_t *status)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;
    MacCAN_Status_t tmpStatus;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;

    // get status register
    retVal = canDevices[handle].GetStatus(tmpStatus);
    if (status)
        *status = tmpStatus.byte;
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].GetStatus: returned %i\n", handle, retVal);
#endif
    return (int)retVal;
}

EXPORT
int can_busload(int handle, uint8_t *load, uint8_t *status)
{
    MacCAN_Return_t retVal1 = CMacCAN::FatalError;
    MacCAN_Return_t retVal2 = CMacCAN::FatalError;
    MacCAN_Status_t tmpStatus;
    uint8_t tmpLoad;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;

    // get bus-load
    retVal1 = canDevices[handle].GetBusLoad(tmpLoad);
    if (load && (retVal1 == CANERR_NOERROR))
        *load = tmpLoad;
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].GetBusLoad: returned %i\n", handle, retVal1);
#endif
    // and status register
    retVal2 = canDevices[handle].GetStatus(tmpStatus);
    if (status && (retVal2 == CANERR_NOERROR))
        *status = tmpStatus.byte;
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].GetStatus: returned %i\n", handle, retVal2);
#endif
    return (retVal1 != CANERR_NOERROR) ? (int)retVal1 : (int)retVal2;
}

EXPORT
int can_bitrate(int handle, can_bitrate_t *bitrate, can_speed_t *speed)
{
    MacCAN_Return_t retVal1 = CMacCAN::FatalError;
    MacCAN_Return_t retVal2 = CMacCAN::FatalError;
    MacCAN_Bitrate_t tmpBitrate;
    MacCAN_BusSpeed_t tmpSpeed;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;

    // get bit-rate settings
    retVal1 = canDevices[handle].GetBitrate(tmpBitrate);
    if (bitrate && (retVal1 == CANERR_NOERROR))
        *bitrate = tmpBitrate;
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].GetBitrate: returned %i\n", handle, retVal1);
#endif
    // and transmission rate
    retVal2 = canDevices[handle].GetBusSpeed(tmpSpeed);
    if (speed && (retVal2 == CANERR_NOERROR))
        *speed = tmpSpeed;
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].GetBusSpeed: returned %i\n", handle, retVal2);
#endif
    return (retVal1 != CANERR_NOERROR) ? (int)retVal1 : (int)retVal2;
}

EXPORT
int can_property(int handle, uint16_t param, void *value, uint32_t nbyte)
{
    MacCAN_Return_t retVal = CMacCAN::FatalError;

    // sanity check
    if (!init)
        return CANERR_NOTINIT;
    if (!IS_HANDLE_VALID(handle))
        return CANERR_HANDLE;

    // read a property value
    retVal = canDevices[handle].GetProperty(param, value, nbyte);
#if (OPTION_CANAPI_DEBUG_LEVEL != 0)
    fprintf(stdout, "CKvaserCAN[%i].GetProperty: returned %i\n", handle, retVal);
#endif
    // TODO: Hmm, what's about SetProperty? (Lucky me, there's nothing to be set)

    return (int)retVal;
}

EXPORT
char *can_hardware(int handle)
{
    // sanity check
    if (!init)
        return NULL;
    if (!IS_HANDLE_VALID(handle))
        return NULL;

    // get hardware version (zero-terminated string)
    return (char *)canDevices[handle].GetHardwareVersion();
}

EXPORT
char *can_software(int handle)
{
    // sanity check
    if (!init)
        return NULL;
    if (!IS_HANDLE_VALID(handle))
        return NULL;

    // get software version (zero-terminated string)
    return (char *)canDevices[handle].GetFirmwareVersion();
}

EXPORT
char *can_version(void)
{
    // get module version (zero-terminated string)
    return (char *)version;
}
