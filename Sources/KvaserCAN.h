//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (for Kvaser CAN Interfaces)
//
//  Copyright (c) 2020-2021 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//  All rights reserved.
//
//  This file is part of MacCAN-KvaserCAN.
//
//  MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v3.0 (or any later version). You can
//  choose between one of them if you use MacCAN-KvaserCAN in whole or in part.
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
//  MacCAN-KvaserCAN IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF MacCAN-KvaserCAN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
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
#ifndef KVASERCAN_H_INCLUDED
#define KVASERCAN_H_INCLUDED

#include "CANAPI.h"

/// \name   KvaserCAN
/// \brief  KvaserCAN dynamic library
/// \{
#define KVASERCAN_LIBRARY_ID  CANLIB_KVASER_32
#if (OPTION_CANAPI_KVASERCAN_DYLIB != 0)
  #define KVASERCAN_LIBRARY_NAME  CANDLL_KVASERCAN
#else
  #define KVASERCAN_LIBRARY_NAME  "libKvaserCAN.dylib"
#endif
#define KVASERCAN_LIBRARY_VENDOR  "UV Software, Berlin"
#define KVASERCAN_LIBRARY_LICENSE  "BSD-2-Clause OR GPL-3.0-or-later"
#define KVASERCAN_LIBRARY_COPYRIGHT  "Copyright (c) 2020-2021  Uwe Vogt, UV Software, Berlin"
#define KVASERCAN_LIBRARY_HAZARD_NOTE  "If you connect your CAN device to a real CAN network when using this library,\n" \
                                       "you might damage your application."
/// \}


/// \name   KvaserCAN API
/// \brief  CAN API V3 driver for Kvaser CAN interfaces
/// \note   See CCanApi for a description of the overridden methods
/// \{
class CANCPP CKvaserCAN : public CCanApi {
private:
    CANAPI_Handle_t m_Handle;  ///< CAN interface handle
    CANAPI_OpMode_t m_OpMode;  ///< CAN operation mode
    CANAPI_Bitrate_t m_Bitrate;  ///< CAN bitrate settings
    struct {
        uint64_t u64TxMessages;  ///< number of transmitted CAN messages
        uint64_t u64RxMessages;  ///< number of received CAN messages
        uint64_t u64ErrorFrames;  ///< number of received status messages
    } m_Counter;
public:
    // constructor / destructor
    CKvaserCAN();
    ~CKvaserCAN();
    // CKvaserCAN-specific error codes (CAN API V3 extension)
    enum EErrorCodes {
        // note: range 0...-99 is reserved by CAN API V3
        GeneralError = VendorSpecific
    };
    // CCANAPI overrides
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param, EChannelState &state);
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, EChannelState &state);

    CANAPI_Return_t InitializeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param = NULL);
    CANAPI_Return_t TeardownChannel();
    CANAPI_Return_t SignalChannel();

    CANAPI_Return_t StartController(CANAPI_Bitrate_t bitrate);
    CANAPI_Return_t ResetController();

    CANAPI_Return_t WriteMessage(CANAPI_Message_t message, uint16_t timeout = 0U);
    CANAPI_Return_t ReadMessage(CANAPI_Message_t &message, uint16_t timeout = CANREAD_INFINITE);

    CANAPI_Return_t GetStatus(CANAPI_Status_t &status);
    CANAPI_Return_t GetBusLoad(uint8_t &load);

    CANAPI_Return_t GetBitrate(CANAPI_Bitrate_t &bitrate);
    CANAPI_Return_t GetBusSpeed(CANAPI_BusSpeed_t &speed);

    CANAPI_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte);
    CANAPI_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte);

    char *GetHardwareVersion();  // (for compatibility reasons)
    char *GetFirmwareVersion();  // (for compatibility reasons)
    static char *GetVersion();  // (for compatibility reasons)

    static CANAPI_Return_t MapIndex2Bitrate(int32_t index, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapString2Bitrate(const char *string, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapBitrate2String(CANAPI_Bitrate_t bitrate, char *string, size_t length);
    static CANAPI_Return_t MapBitrate2Speed(CANAPI_Bitrate_t bitrate, CANAPI_BusSpeed_t &speed);
private:
    CANAPI_Return_t MapBitrate2Sja1000(CANAPI_Bitrate_t bitrate, uint16_t &btr0btr1);
    CANAPI_Return_t MapSja10002Bitrate(uint16_t btr0btr1, CANAPI_Bitrate_t &bitrate);
public:
    static uint8_t Dlc2Len(uint8_t dlc) { return CCanApi::Dlc2Len(dlc); }
    static uint8_t Len2Dlc(uint8_t len) { return CCanApi::Len2Dlc(len); }
};
/// \}

/// \name   KvaserCAN Property IDs
/// \brief  Properties that can be read (or written)
/// \{
#define KVASERCAN_PROPERTY_CANAPI         (CANPROP_GET_SPEC)
#define KVASERCAN_PROPERTY_VERSION        (CANPROP_GET_VERSION)
#define KVASERCAN_PROPERTY_PATCH_NO       (CANPROP_GET_PATCH_NO)
#define KVASERCAN_PROPERTY_BUILD_NO       (CANPROP_GET_BUILD_NO)
#define KVASERCAN_PROPERTY_LIBRARY_ID     (CANPROP_GET_LIBRARY_ID)
#define KVASERCAN_PROPERTY_LIBRARY_NAME   (CANPROP_GET_LIBRARY_DLLNAME)
#define KVASERCAN_PROPERTY_LIBRARY_VENDOR (CANPROP_GET_LIBRARY_VENDOR)
#define KVASERCAN_PROPERTY_DEVICE_TYPE    (CANPROP_GET_DEVICE_TYPE)
#define KVASERCAN_PROPERTY_DEVICE_NAME    (CANPROP_GET_DEVICE_NAME)
#define KVASERCAN_PROPERTY_DEVICE_DRIVER  (CANPROP_GET_DEVICE_DLLNAME)
#define KVASERCAN_PROPERTY_DEVICE_VENDOR  (CANPROP_GET_DEVICE_VENDOR)
#define KVASERCAN_PROPERTY_OP_CAPABILITY  (CANPROP_GET_OP_CAPABILITY)
#define KVASERCAN_PROPERTY_OP_MODE        (CANPROP_GET_OP_MODE)
#define KVASERCAN_PROPERTY_BITRATE        (CANPROP_GET_BITRATE)
#define KVASERCAN_PROPERTY_SPEED          (CANPROP_GET_SPEED)
#define KVASERCAN_PROPERTY_STATUS         (CANPROP_GET_STATUS)
#define KVASERCAN_PROPERTY_BUSLOAD        (CANPROP_GET_BUSLOAD)
#define KVASERCAN_PROPERTY_TX_COUNTER     (CANPROP_GET_TX_COUNTER)
#define KVASERCAN_PROPERTY_RX_COUNTER     (CANPROP_GET_RX_COUNTER)
#define KVASERCAN_PROPERTY_ERR_COUNTER    (CANPROP_GET_ERR_COUNTER)
/// \}
#endif // KVASERCAN_H_INCLUDED
