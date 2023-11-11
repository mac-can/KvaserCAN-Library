//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2023 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//  All rights reserved.
//
//  This file is part of CAN API V3.
//
//  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License and
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
//  along with CAN API V3.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

//  Generic CAN API V3 Testing with GoogleTest
//
//  (§1) include the header file of the CAN API V3 C++ class of the CAN driver
#include "KvaserCAN.h"

//  (§2) define type CCanDriver with the CAN API V3 C++ class of the CAN driver
typedef CKvaserCAN  CCanDriver;

//  (§3) define macro CAN_LIBRARY, CAN_DEVICE1 and CAN_DEVICE2 for the devices under test
#define CAN_LIBRARY  KVASER_LIB_ID
#define CAN_DEVICE1  KVASER_CAN_CHANNEL1
#define CAN_DEVICE2  KVASER_CAN_CHANNEL0

//  ($4) define macros for driver-specific features
//       at least the mandatory macros (cf. compiler warnings)
#define FEATURE_BITRATE_5K           FEATURE_SUPPORTED
#define FEATURE_BITRATE_800K         FEATURE_SUPPORTED
#define FEATURE_BITRATE_SAM          FEATURE_UNSUPPORTED
#define FEATURE_BITRATE_FD_SAM       FEATURE_UNSUPPORTED
#define FEATURE_BITRATE_SJA1000      FEATURE_UNSUPPORTED
#define FEATURE_ERROR_FRAMES         FEATURE_SUPPORTED
#define FEATURE_ERROR_CODE_CAPTURE   FEATURE_UNSUPPORTED
#define FEATURE_BLOCKING_READ        FEATURE_SUPPORTED
#define FEATURE_BLOCKING_WRITE       FEATURE_SUPPORTED
#define FEATURE_SIZE_RECEIVE_QUEUE   65536U
#define FEATURE_SIZE_TRANSMIT_QUEUE  0U

//  (§5) define macros for CAN Classic bit-rate settings
//       at least BITRATE_1M, BITRATE_500K, BITRATE_250K, BITRATE_125K, 
//                BITRATE_100K, BITRATE_50K, BITRATE_20K, BITRATE_10K
#define BITRATE_1M(x)    KVASER_CAN_BR_1M(x)
#define BITRATE_800K(x)  KVASER_CAN_BR_800K(x)
#define BITRATE_500K(x)  KVASER_CAN_BR_500K(x)
#define BITRATE_250K(x)  KVASER_CAN_BR_250K(x)
#define BITRATE_125K(x)  KVASER_CAN_BR_125K(x)
#define BITRATE_100K(x)  KVASER_CAN_BR_100K(x)
#define BITRATE_50K(x)   KVASER_CAN_BR_50K(x)
#define BITRATE_20K(x)   KVASER_CAN_BR_20K(x)
#define BITRATE_10K(x)   KVASER_CAN_BR_10K(x)
#define BITRATE_5K(x)    KVASER_CAN_BR_5K(x)

//  (§6) define macros for workarounds (e.g. TC01_3_ISSUE)
#define TC04_16_ISSUE_KVASER_STATUS_MESSAGE  WORKAROUND_ENABLED  // 2023-09-05: no status message from U100P
#define TC09_8_ISSUE_BUS_OFF  WORKAROUND_ENABLED  // 2023-09-05: no bus-off from device
//#define TC0x_y_ISSUE_  WORKAROUND_ENABLED
//  (§6.1) old PCANBasic issues (see macros in 'Settings.h')
#define PCBUSB_INIT_DELAY_WORKAROUND  WORKAROUND_DISABLED
#define PCBUSB_QXMTFULL_WORKAROUND    WORKAROUND_DISABLED

//  (§7) define macros for CAN 2.0 bit-rate indexes to be used in the tests
#define CAN_INDEX_DEFAULT  CANBTR_INDEX_250K
#define CAN_INDEX_SLOWER   CANBTR_INDEX_10K
#define CAN_INDEX_FASTER   CANBTR_INDEX_1M

//  (§8) define macros for CAN 2.0 bit-rate settings to be used in the tests
#define CAN_BITRATE_DEFAULT  BITRATE_250K
#define CAN_BITRATE_SLOWER   BITRATE_10K
#define CAN_BITRATE_FASTER   BITRATE_1M


//  (§9) define macro CAN_FD_SUPPORTED if CAN FD operation mode is supported
#define CAN_FD_SUPPORTED FEATURE_SUPPORTED

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
//  (§10) define macros for CAN FD bit-rate settings
//       at least BITRATE_FD_1M8M, BITRATE_FD_500K4M, BITRATE_FD_250K2M, BITRATE_FD_125K1M,
//                BITRATE_FD_1M, BITRATE_FD_500K, BITRATE_FD_250K, BITRATE_FD_125K
#define BITRATE_FD_1M(x)      KVASER_CAN_FD_BR_1M(x)
#define BITRATE_FD_500K(x)    KVASER_CAN_FD_BR_500K(x)
#define BITRATE_FD_250K(x)    KVASER_CAN_FD_BR_250K(x)
#define BITRATE_FD_125K(x)    KVASER_CAN_FD_BR_125K(x)
#define BITRATE_FD_1M8M(x)    KVASER_CAN_FD_BR_1M8M(x)
#define BITRATE_FD_500K4M(x)  KVASER_CAN_FD_BR_500K4M(x)
#define BITRATE_FD_250K2M(x)  KVASER_CAN_FD_BR_250K2M(x)
#define BITRATE_FD_125K1M(x)  KVASER_CAN_FD_BR_125K1M(x)

//  (§11) define macros for workarounds for CAN FD operation mode (e.g. TC01_3_ISSUE_FD)
//#define TC0x_y_ISSUE_FD_  WORKAROUND_ENABLED

//  (§12) define macros for CAN FD bit-rate settings to be used in the tests, if supported
#define CAN_BITRATE_FD_DEFAULT  BITRATE_FD_250K2M
#define CAN_BITRATE_FD_SLOWER   BITRATE_FD_125K1M
#define CAN_BITRATE_FD_FASTER   BITRATE_FD_1M8M

#endif // CAN_FD_SUPPORTED
#endif // DRIVER_H_INCLUDED

// $Id$  Copyright (c) UV Software, Berlin //
