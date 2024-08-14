//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2024 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//  All rights reserved.
//
//  This file is part of CAN API V3.
//
//  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v3.0 (or any later version).
//  You can choose between one of them if you use this file.
//
//  BSD 2-Clause "Simplified" License:
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  CAN API V3 IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF CAN API V3, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
//  CAN API V3 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  CAN API V3 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with CAN API V3.  If not, see <https://www.gnu.org/licenses/>.
//
#include "pch.h"
#include <string.h>
#include <limits.h>
#include <iostream>

#define MAX_PROPERTIES  54

#define LIB_PARAM  true
#define DRV_PARAM  false

#define PROP_GETTER  false
#define PROP_SETTER  true

#define PROP_REQUIRED  true
#define PROP_OPTIONAL  false

#if (FEATURE_FILTERING != FEATURE_SUPPORTED)
#define PROP_FILTERING  PROP_OPTIONAL
#else
#define PROP_FILTERING  PROP_REQUIRED
#endif

#if (FEATURE_TRACEFILE != FEATURE_SUPPORTED)
#define PROP_TRACEFILE  PROP_OPTIONAL
#else
#define PROP_TRACEFILE  PROP_REQUIRED
#endif

#define MODE_RUNNING  true
#define MODE_STOPPED  false

static struct SProperty {
    uint16_t m_nId;
    uint32_t m_nSize;
    bool m_fPreInit;
    bool m_fSetter;
    bool m_fRequired;
    bool m_fModeRunning;
    const char* m_szMnemonic;
    const char* m_szDescription;
} aProperties[MAX_PROPERTIES + 1] = {
    { CANPROP_GET_SPEC            , sizeof(uint16_t),        LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_SPEC", "version of the wrapper specification (uint16_t)" },
    { CANPROP_GET_VERSION         , sizeof(uint16_t),        LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_VERSION", "version number of the library (uint16_t)" },
    { CANPROP_GET_PATCH_NO        , sizeof(uint8_t),         LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_PATCH_NO", "patch number of the library (uint8_t)" },
    { CANPROP_GET_BUILD_NO        , sizeof(uint32_t),        LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_BUILD_NO", "build number of the library (uint32_t)" },
    { CANPROP_GET_LIBRARY_ID      , sizeof(int32_t),         LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_LIBRARY_ID", "library id of the library (int32_t)" },
    { CANPROP_GET_LIBRARY_VENDOR  , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_LIBRARY_VENDOR", "vendor name of the library (char[])" },
    { CANPROP_GET_LIBRARY_DLLNAME , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_LIBRARY_DLLNAME", "file name of the library DLL (char[])" },
    { CANPROP_GET_DEVICE_TYPE     , sizeof(int32_t),         DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_DEVICE_TYPE", "device type of the CAN interface (int32_t)" },
    { CANPROP_GET_DEVICE_NAME     , CANPROP_MAX_BUFFER_SIZE, DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_DEVICE_NAME", "device name of the CAN interface (char[])" },
    { CANPROP_GET_DEVICE_VENDOR   , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_DEVICE_VENDOR", "vendor name of the CAN interface (char[])" },
    { CANPROP_GET_DEVICE_DLLNAME  , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_DEVICE_DLLNAME", "file name of the CAN interface DLL (char[])" },
    { CANPROP_GET_DEVICE_PARAM    , CANPROP_MAX_BUFFER_SIZE, DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_DEVICE_PARAM", "device parameter of the CAN interface (char[])" },
    { CANPROP_GET_OP_CAPABILITY   , sizeof(uint8_t),         DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_OP_CAPABILITY", "supported operation modes of the CAN controller (uint8_t)" },
    { CANPROP_GET_OP_MODE         , sizeof(uint8_t),         DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_OP_MODE", "active operation mode of the CAN controller (uint8_t)" },
    { CANPROP_GET_BITRATE         , sizeof(can_bitrate_t),   DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_BITRATE", "active bit-rate of the CAN controller (can_bitrate_t)" },
    { CANPROP_GET_SPEED           , sizeof(can_speed_t),     DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_SPEED", "active bus speed of the CAN controller (can_speed_t)" },
    { CANPROP_GET_STATUS          , sizeof(can_mode_t),      DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_STATUS", "current status register of the CAN controller (uint8_t)" },
    { CANPROP_GET_BUSLOAD         , sizeof(uint16_t),        DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_BUSLOAD", "current bus load of the CAN controller (uint16_t)" },
    { CANPROP_GET_NUM_CHANNELS    , sizeof(uint8_t),         DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_NUM_CHANNELS", "numbers of CAN channels on the CAN interface (uint8_t)" },
    { CANPROP_GET_CAN_CHANNEL     , sizeof(uint8_t),         DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_CAN_CHANNEL", "active CAN channel on the CAN interface (uint8_t)" },
    { CANPROP_GET_CAN_CLOCK       , sizeof(int32_t),         DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_CAN_CLOCK", "frequency of the CAN controller clock in [Hz] (int32_t)" },
    { CANPROP_GET_TX_COUNTER      , sizeof(uint64_t),        DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_TX_COUNTER", "total number of sent messages (uint64_t)" },
    { CANPROP_GET_RX_COUNTER      , sizeof(uint64_t),        DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_RX_COUNTER", "total number of reveiced messages (uint64_t)" },
    { CANPROP_GET_ERR_COUNTER     , sizeof(uint64_t),        DRV_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_ERR_COUNTER", "total number of reveiced error frames (uint64_t)" },
    { CANPROP_GET_RCV_QUEUE_SIZE  , sizeof(uint32_t),        DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_RCV_QUEUE_SIZE", "maximum number of message the receive queue can hold (uint32_t)" },
    { CANPROP_GET_RCV_QUEUE_HIGH  , sizeof(uint32_t),        DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_RCV_QUEUE_HIGH", "maximum number of message the receive queue has hold (uint32_t)" },
    { CANPROP_GET_RCV_QUEUE_OVFL  , sizeof(uint64_t),        DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_RCV_QUEUE_OVFL", "overflow counter of the receive queue (uint64_t)" },
    { CANPROP_GET_TRM_QUEUE_SIZE  , sizeof(uint32_t),        DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_TRM_QUEUE_SIZE", "maximum number of message the transmit queue can hold (uint32_t)" },
    { CANPROP_GET_TRM_QUEUE_HIGH  , sizeof(uint32_t),        DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_TRM_QUEUE_HIGH", "maximum number of message the transmit queue has hold (uint32_t)" },
    { CANPROP_GET_TRM_QUEUE_OVFL  , sizeof(uint64_t),        DRV_PARAM, PROP_GETTER, PROP_OPTIONAL,  MODE_RUNNING, "CANPROP_GET_TRM_QUEUE_OVFL", "overflow counter of the transmit queue (uint64_t)" },
    { CANPROP_GET_FILTER_11BIT    , sizeof(uint64_t),        DRV_PARAM, PROP_GETTER, PROP_FILTERING, MODE_RUNNING, "CANPROP_GET_FILTER_11BIT", "acceptance filter code and mask for 11-bit identifier (uint64_t)" },
    { CANPROP_GET_FILTER_29BIT    , sizeof(uint64_t),        DRV_PARAM, PROP_GETTER, PROP_FILTERING, MODE_RUNNING, "CANPROP_GET_FILTER_29BIT", "acceptance filter code and mask for 29-bit identifier (uint64_t)" },
    { CANPROP_SET_FILTER_11BIT    , sizeof(uint64_t),        DRV_PARAM, PROP_SETTER, PROP_FILTERING, MODE_STOPPED, "CANPROP_SET_FILTER_11BIT", "set value for acceptance filter code and mask for 11-bit identifier (uint64_t)" },
    { CANPROP_SET_FILTER_29BIT    , sizeof(uint64_t),        DRV_PARAM, PROP_SETTER, PROP_FILTERING, MODE_STOPPED, "CANPROP_SET_FILTER_29BIT", "set value for acceptance filter code and mask for 29-bit identifier (uint64_t)" },
    { CANPROP_SET_FILTER_RESET    , 0U /* NULL pointer*/,    DRV_PARAM, PROP_SETTER, PROP_FILTERING, MODE_STOPPED, "CANPROP_SET_FILTER_RESET", "reset acceptance filter code and mask to default values (NULL)" },
    { CANPROP_SET_FROMTO_11BIT    , sizeof(uint64_t),        DRV_PARAM, PROP_SETTER, PROP_OPTIONAL,  MODE_STOPPED, "CANPROP_SET_FROMTO_11BIT", "add an 11-bit identifier range to the from-to filter list (uint64_t)" },
    { CANPROP_SET_FROMTO_29BIT    , sizeof(uint64_t),        DRV_PARAM, PROP_SETTER, PROP_OPTIONAL,  MODE_STOPPED, "CANPROP_SET_FROMTO_29BIT", "add an 11-bit identifier range to the from-to filter list (uint64_t)" },
    { CANPROP_GET_TRACE_ACTIVE    , sizeof(uint8_t),         DRV_PARAM, PROP_GETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_GET_TRACE_ACTIVE", "trace file activation state: STOPPED/RUNNING (uint8_t)" },
    { CANPROP_GET_TRACE_FOLDER    , CANPROP_MAX_BUFFER_SIZE, DRV_PARAM, PROP_GETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_GET_TRACE_FOLDER", "trace file folder location (directory only) (char[])" },
    { CANPROP_GET_TRACE_TYPE      , sizeof(uint8_t),         DRV_PARAM, PROP_GETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_GET_TRACE_TYPE", "trace file type (for possible values see below) (uint8_t)" },
    { CANPROP_GET_TRACE_MODE      , sizeof(uint16_t),        DRV_PARAM, PROP_GETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_GET_TRACE_MODE", "trace file mode (for possible options see below) (uint16_t)" },
    { CANPROP_GET_TRACE_SIZE      , sizeof(uint16_t),        DRV_PARAM, PROP_GETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_GET_TRACE_SIZE", "trace file segment size (in 10 KB steps, 0 = 100MB) (uint 16_t)" },
    /* note: CANPROP_GET_TRACE_FILE can only be called if a trace-file session is active (property is tested separately) */
    { CANPROP_SET_TRACE_ACTIVE    , sizeof(uint8_t),         DRV_PARAM, PROP_SETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_SET_TRACE_ACTIVE", "start/stop trace file logging with configured settings (uint8_t)" },
    { CANPROP_SET_TRACE_FOLDER    , CANPROP_MAX_BUFFER_SIZE, DRV_PARAM, PROP_SETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_SET_TRACE_FOLDER", "set trace file folder location (directory only) (char[])" },
    { CANPROP_SET_TRACE_TYPE      , sizeof(uint8_t),         DRV_PARAM, PROP_SETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_SET_TRACE_TYPE", "set trace file type (for possible values see below) (uint8_t)" },
    { CANPROP_SET_TRACE_MODE      , sizeof(uint16_t),        DRV_PARAM, PROP_SETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_SET_TRACE_MODE", "set trace file mode (for possible options see below) (uint16_t)" },
    { CANPROP_SET_TRACE_SIZE      , sizeof(uint16_t),        DRV_PARAM, PROP_SETTER, PROP_TRACEFILE, MODE_RUNNING, "CANPROP_SET_TRACE_SIZE", "set trace file segment size (in 10 KB steps, 0 = 100MB) (uint16_t)" },
    /* note:  SET_FIRST_CHANNEL must be called before any GET_CHANNEL_xyz, therefore we define it as a PROP_GETTER because it gets a (virtual) index */
    { CANPROP_SET_FIRST_CHANNEL   , 0U /* NULL pointer*/,    LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_SET_FIRST_CHANNEL", "set index to the first entry in the interface list (NULL)" },
    { CANPROP_GET_CHANNEL_NO      , sizeof(int32_t),         LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_CHANNEL_NO", "get channel no. at actual index in the interface list (int32_t)" },
    { CANPROP_GET_CHANNEL_NAME    , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_CHANNEL_NAME", "get channel name at actual index in the interface list (char[])" },
    { CANPROP_GET_CHANNEL_DLLNAME , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_CHANNEL_DLLNAME", "get file name of the DLL at actual index in the interface list (char[])" },
    { CANPROP_GET_CHANNEL_VENDOR_ID , sizeof(int32_t),       LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_GET_CHANNEL_VENDOR_ID", "get library id at actual index in the interface list (int32_t)" },
    { CANPROP_GET_CHANNEL_VENDOR_NAME , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, PROP_GETTER, PROP_REQUIRED, MODE_RUNNING, "CANPROP_GET_CHANNEL_VENDOR_NAME", "get vendor name at actual index in the interface list(char[])" },
    /* note:  SET_NEXT_CHANNEL is also defined as a setter because it gets the next (virtual) index (see above), but it can be EOF when there is no device entry */
    { CANPROP_SET_NEXT_CHANNEL    , 0U /* NULL pointer*/,    LIB_PARAM, PROP_GETTER, PROP_REQUIRED,  MODE_RUNNING, "CANPROP_SET_NEXT_CHANNEL", "set index to the next entry in the interface list (NULL)" },
    /* end marker */
    { CANPROP_INVALID, 0U, false, false, false, false, "", "" }
};

CProperties::CProperties() {
    m_nIndex = MAX_PROPERTIES;
}

TProperty CProperties::GetFirstEntry() {
    m_nIndex = 0;
    return aProperties[m_nIndex].m_nId;
}

TProperty CProperties::GetNextEntry() {
    if (m_nIndex < MAX_PROPERTIES)
        m_nIndex += 1;
    else
        m_nIndex = MAX_PROPERTIES;
    // note: last entry returns CANPROP_INVALID
    return aProperties[m_nIndex].m_nId;
}

bool CProperties::IsSetter() {
    if (m_nIndex < MAX_PROPERTIES)
        return aProperties[m_nIndex].m_fSetter;
    else
        return false;
}

bool CProperties::IsPreInit() {
    if (m_nIndex < MAX_PROPERTIES)
        return aProperties[m_nIndex].m_fPreInit;
    else
        return false;
}

bool CProperties::IsRequired() {
    if (m_nIndex < MAX_PROPERTIES)
        return aProperties[m_nIndex].m_fRequired;
    else
        return false;
}

bool CProperties::IsModeRunning() {
    if (m_nIndex < MAX_PROPERTIES)
        return aProperties[m_nIndex].m_fModeRunning;
    else
        return false;
}

uint32_t CProperties::SizeOf()  {
    if (m_nIndex < MAX_PROPERTIES)
        return aProperties[m_nIndex].m_nSize;
    else
        return (size_t)0;
}

const char* CProperties::Mnemonic() {
    if (m_nIndex < MAX_PROPERTIES)
        return aProperties[m_nIndex].m_szMnemonic;
    else
        return aProperties[MAX_PROPERTIES].m_szMnemonic;
}

const char* CProperties::Description() {
    if (m_nIndex < MAX_PROPERTIES)
        return aProperties[m_nIndex].m_szDescription;
    else
        return aProperties[MAX_PROPERTIES].m_szDescription;
}

// $Id: Properties.cpp 1390 2024-08-09 19:43:19Z makemake $  Copyright (c) UV Software, Berlin //
