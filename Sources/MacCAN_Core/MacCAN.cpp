//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  MacCAN - macOS User-Space Driver for USB-to-CAN Interfaces
//
//  Copyright (c) 2012-2021 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//  All rights reserved.
//
//  This file is part of MacCAN-Core.
//
//  MacCAN-Core is dual-licensed under the BSD 2-Clause "Simplified" License and
//  under the GNU General Public License v3.0 (or any later version).
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
//  MacCAN-Core IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF MacCAN-Core, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
//  MacCAN-Core is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MacCAN-Core is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MacCAN-Core.  If not, see <http://www.gnu.org/licenses/>.
//
/// \file        MacCAN.cpp
///
/// \brief       MacCAN - macOS User-Space Driver for CAN to USB Interfaces
///
/// \author      $Author: eris $
///
/// \version     $Rev: 1001 $
///
/// \addtogroup  mac_can
/// \{
#include "MacCAN.h"
#include "MacCAN_IOUsbKit.h"
#include "can_btr.h"

#include <stdio.h>
#include <string.h>

#if (OPTION_MACCAN_METHODS_VISIBLE != 0)
#define EXPORT  __attribute__((visibility("default")))
#else
#define EXPORT
#endif

//  Methods to initialize and release the MacCAN IOUsbKit
//
MacCAN_Return_t CMacCAN::Initializer() {
    return (MacCAN_Return_t)CANUSB_Initialize();
}

MacCAN_Return_t CMacCAN::Finalizer() {
    return (MacCAN_Return_t)CANUSB_Teardown();
}

//  Methods for bit-rate conversion
//
EXPORT
MacCAN_Return_t CMacCAN::MapIndex2Bitrate(int32_t index, MacCAN_Bitrate_t &bitrate) {
    return (MacCAN_Return_t)btr_index2bitrate(index, &bitrate);
}

EXPORT
MacCAN_Return_t CMacCAN::MapString2Bitrate(const char *string, MacCAN_Bitrate_t &bitrate) {
    bool brse = false;
    // TODO: rework function 'btr_string2bitrate'
    return (MacCAN_Return_t)btr_string2bitrate((const btr_string_t)string, &bitrate, &brse);
}

EXPORT
MacCAN_Return_t CMacCAN::MapBitrate2String(MacCAN_Bitrate_t bitrate, char *string, size_t length) {
    (void)length;
    // TODO: rework function 'btr_bitrate2string'
    return (MacCAN_Return_t)btr_bitrate2string(&bitrate, false, (btr_string_t)string);
}

EXPORT
MacCAN_Return_t CMacCAN::MapBitrate2Speed(MacCAN_Bitrate_t bitrate, MacCAN_BusSpeed_t &speed) {
    // TODO: rework function 'btr_bitrate2speed'
    return (MacCAN_Return_t)btr_bitrate2speed(&bitrate, false, false, &speed);
}

//  Methods for DLC conversion
//
EXPORT
uint8_t CMacCAN::Dlc2Len(uint8_t dlc) {
    const static uint8_t dlc_table[16] = {
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U
    };
    return dlc_table[dlc & 0xFU];
}

EXPORT
uint8_t CMacCAN::Len2Dlc(uint8_t len) {
    if(len > 48U) return 0x0FU;
    if(len > 32U) return 0x0EU;
    if(len > 24U) return 0x0DU;
    if(len > 20U) return 0x0CU;
    if(len > 16U) return 0x0BU;
    if(len > 12U) return 0x0AU;
    if(len > 8U) return 0x09U;
    return len;
}

//  Method to retrieve version information about the MacCAN Core
//
EXPORT
char *CMacCAN::GetVersion() {
    static char string[CANPROP_MAX_BUFFER_SIZE] = "The torture never stops.";
    UInt32 version = CANUSB_GetVersion();
    sprintf(string, "macOS Driver Kit for USB-to-CAN Interfaces (MacCAN Core %u.%u.%u)",
                    (UInt8)(version >> 24), (UInt8)(version >> 16), (UInt8)(version >> 8));
    return (char *)string;
}

// Default implementation for static method ProbeChannel (when not overridden)
//
EXPORT
MacCAN_Return_t CMacCAN::ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, const void *param, EChannelState &state) {
    (void)channel;
    (void)opMode;
    (void)param;
    state = CMacCAN::ChannelNotTestable;
    return (MacCAN_Return_t)CMacCAN::NotSupported;
}

EXPORT
MacCAN_Return_t CMacCAN::ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, EChannelState &state) {
    return ProbeChannel(channel, opMode, NULL, state);
}
/// \}
