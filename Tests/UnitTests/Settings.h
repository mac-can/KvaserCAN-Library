//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2021 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
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
#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#import "Tester.h"
#import "Timer.h"

//  Device under Test (2 devices required)
#define DUT1  (SInt32)CAN_DEVICE1
#define DUT2  (SInt32)CAN_DEVICE2

//  Default operation mode and bit-rate settings
#define TEST_CANMODE  CANMODE_DEFAULT
#define TEST_BTRINDEX  CANBTR_INDEX_250K

//  General test settings:
//  - number of CAN frames to be send during test cases
//    note: a small number speeds up the test duration.
#define TEST_FRAMES  8  /// default = 8
//  - number of CAN frames to be send during smoke test
//    note: long-term stress test with a large number.
#define TEST_TRAFFIC  2048  /// default = 2048
//  - number of CAN frames to be send during time-stamp test
//    note: not too small because of statistic evaluation.
#define TEST_TIMESTAMP  1000  /// default = 1000
//  - number of CAN frames to be send until queue overrun
//    note: maybe delay for buffered transmission required.
#define TEST_QUEUE_FULL  65536  /// default = 65'536
#if (TX_ACKNOWLEDGE_UNSUPPORTED != 0)
#define TEST_AFTER_BURNER  3000  /// default = 3000 (in [ms])
#endif
//  - enable/disable sending of CAN frames during sunnyday scenarios
//    note: disable this option to not repeat the same crap forever.
#define SEND_TEST_FRAMES  1  /// default = enabled
//  - enable/disable sending of CAN frames with non-default baudrate
//    note: disable this option when a 3rd CAN device is on the bus.
#define SEND_WITH_NONE_DEFAULT_BAUDRATE  1  /// default = enabled
//  - enable/disable exiting loops over properties on error
//    note: enable this option to debug a failing property.
#define EXIT_PROPERTY_LOOP_ON_ERROR  0  /// default = disabled

//  Settings for time-stamp accuracy:
//  - time-stamp test with 10ms transmission delay
#define TEST_TIMESTAMP_10MS   1  /// default = enabled
#define TIMESTAMP_LOWER_10MS  9500  /// lower threshold = delay - 500usec
#define TIMESTAMP_UPPER_10MS  10500 /// upper threshold = delay + 500usec
//  - time-stamp test with 7ms transmission delay
#define TEST_TIMESTAMP_7MS   1  /// default = enabled
#define TIMESTAMP_LOWER_7MS  6500  /// lower threshold = delay - 500usec
#define TIMESTAMP_UPPER_7MS  7500  /// upper threshold = delay + 500usec
//  - time-stamp test with 5ms transmission delay
#define TEST_TIMESTAMP_5MS   1  /// default = enabled
#define TIMESTAMP_LOWER_5MS  4500  /// lower threshold = delay - 500usec
#define TIMESTAMP_UPPER_5MS  5500  /// upper threshold = delay + 500usec
//  - time-stamp test with 2ms transmission delay
#define TEST_TIMESTAMP_2MS   1  /// default = enabled
#define TIMESTAMP_LOWER_2MS  1500  /// lower threshold = delay - 500usec
#define TIMESTAMP_UPPER_2MS  2500  /// upper threshold = delay + 500usec
//  - time-stamp test with 1ms transmission delay
#define TEST_TIMESTAMP_1MS   1  /// default = enabled
#define TIMESTAMP_LOWER_1MS  500  /// lower threshold = delay - 500usec
#define TIMESTAMP_UPPER_1MS  1500 /// upper threshold = delay + 500usec
//  - time-stamp test without transmission delay
#define TEST_TIMESTAMP_0MS   1   /// cannot be disabled
#define TIMESTAMP_UPPER_0MS  25000  /// approx. time @ 5kbps

//  Useful stuff:
#define INVALID_HANDLE  (-1)

#endif // SETTINGS_H_INCLUDED

// $Id: Settings.h 1084 2022-01-07 13:31:39Z makemake $  Copyright (c) UV Software, Berlin //
