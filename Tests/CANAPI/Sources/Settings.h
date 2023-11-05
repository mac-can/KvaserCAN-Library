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
#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

//  general defines
//  - search path for JSON files
#if defined(__APPLE__)
#define TEST_SEARCH_PATH  "~/Projects/CAN/DRV/API/json"
#elif defined(__linux__)
#define TEST_SEARCH_PATH  "~/Projects/CAN/DRV/API/json"
#else
#define TEST_SEARCH_PATH  "C:\\Projekte\\CAN\\DRV\\API\\json"
#endif
//  - devices under test (2 devices are required)
#define TEST_DEVICE(dut)  g_Options.GetLibraryId(dut), g_Options.GetChannelNo(dut), g_Options.GetOpMode(dut), g_Options.GetBitrate(dut), g_Options.GetParameter(dut)
//  - default CAN interfaces (from Driver.h)
#define TEST_LIBRARY   (int32_t)CAN_LIBRARY
#define TEST_CHANNEL1  (int32_t)CAN_DEVICE1
#define TEST_CHANNEL2  (int32_t)CAN_DEVICE2
//  - default CAN operation mode (CAN 2.0)
#define TEST_CANMODE   (uint8_t)CANMODE_DEFAULT
//  - default CAN 2.0 bit-timing indexes
#define TEST_BTRINDEX  (int32_t)CAN_INDEX_DEFAULT
#define SLOW_BTRINDEX  (int32_t)CAN_INDEX_SLOWER
#define FAST_BTRINDEX  (int32_t)CAN_INDEX_FASTER
//  - default CAN 2.0 bit-rate settings
#define TEST_BITRATE(x)  CAN_BITRATE_DEFAULT(x)
#define SLOW_BITRATE(x)  CAN_BITRATE_SLOWER(x)
#define FAST_BITRATE(x)  CAN_BITRATE_FASTER(x)
//  - default CAN FD bit-rate settings
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
#define TEST_BITRATE_FD(x)  CAN_BITRATE_FD_DEFAULT(x)
#define SLOW_BITRATE_FD(x)  CAN_BITRATE_FD_SLOWER(x)
#define FAST_BITRATE_FD(x)  CAN_BITRATE_FD_FASTER(x)
#endif
//  general test defines
//  - number of CAN frames to be send during test cases
//    note: a small number speeds up the test duration.
#define TEST_FRAMES  256  // default = 8
//  - number of CAN frames to be send during smoke test
//    note: long-term stress test with a large number.
#define TEST_TRAFFIC  2048  // default = 2048
//  - number of CAN frames to be send while in monitor mode
//    note: not too big because of time-out interval.
#define TEST_MONITOR  16  // default = 16
//  - additional number of CAN frames to be send if recive queue is full
//    note: not too big because of time-out interval.
#define TEST_QRCVFULL  1  // default = 1
//  - maximum number of CAN frames to be send until in bus off state
//    note: bus off state should be reached afer 256 subsequent errors.
#define TEST_MAX_BOFF  2048  // default = 2048
//  - maximum number of CAN frames to be send until warning level reached
//    note: warning level should be reached afer 96 subsequent errors.
#define TEST_MAX_EWRN  2048  // default = 2048
//  - maximum number of CAN frames to be send until error frames present
//    note: error frames are disabled by default and not supported by all wrapper.
#define TEST_MAX_BERR  2048  // default = 2048
//  - time-out for reception of one message
//    note: CAN API V3 in [ms] while CTimer in [us]!
#if (FEATURE_BLOCKING_READ != FEATURE_UNSUPPORTED)
#define TEST_READ_TIMEOUT  250U  // default = 250ms (blocking read)
#else
#define TEST_READ_TIMEOUT  0U    // default = 0ms (polling)
#endif
//  - time-out for transmission of one message
//    note: CAN API V3 in [ms] while CTimer in [us]!
#if (FEATURE_BLOCKING_WRITE != FEATURE_UNSUPPORTED)
#define TEST_WRITE_TIMEOUT 250U  // default = 250ms (acknowledged)
#else
#define TEST_WRITE_TIMEOUT 0U    // default = 0ms (buffered/queued)
#endif
//  - enable/disable sunnyday scenario(s)
//    note: same test sequence as the smoke-test (default scenario)
#define GTEST_SUNNYDAY  GTEST_ENABLED

//  general workarounds
#if (PCBUSB_INIT_DELAY_WORKAROUND != WORKAROUND_DISABLED)
//  - When initializing two PCAN-USB devices then a delay of 100ms is required
//    before sending messages. The receiver swallows the first few (issue #291)
#define PCBUSB_INIT_DELAY()  do { CTimer::Delay((uint64_t)100 * CTimer::MSEC); } while(0)
#else
#define PCBUSB_INIT_DELAY()  while(0)
#endif
#if (PCBUSB_QXMTFULL_WORKAROUND != WORKAROUND_DISABLED)
//  - Up to now no solution found to catch QXMTFULL event when sending a lot of
//    messages back to back with buffered transfer (no acknowledge, issue #101)
#define PCBUSB_QXMT_DELAY()  do { CTimer::Delay((uint64_t)0 * CTimer::MSEC); } while(0)
#else
#define PCBUSB_QXMT_DELAY()  while(0)
#endif
//  defines for transmission/reception loop(s)
//  - time-out for transmission of one message
#define DEVICE_SEND_TIMEOUT  TEST_WRITE_TIMEOUT
//  - time-out for reception of one message
#define DEVICE_READ_TIMEOUT  TEST_READ_TIMEOUT
//  - number of retries on reception errors
#define DEVICE_READ_RETRIES  17
//  - time-out factors for loops
#define DEVICE_LOOP_FACTOR   25U
#define DEVICE_LOOP_DIVISOR  10U
#define DEVICE_LOOP_EXTRA    10

//  useful stuff
//  - invalid interface handle
#define INVALID_HANDLE  (-1)
//  - SJA1000 BTR0BTR1 bit-timing table has 10 entries, CiA table only 9
#define CANBTR_INDEX_5K   (CANBTR_INDEX_10K-1)
#ifdef _MSC_VER
//not #if defined(_WIN32) || defined(_WIN64) because we have strncasecmp in mingw
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif
#endif // SETTINGS_H_INCLUDED

//  $Id: Settings.h 1217 2023-10-10 19:28:31Z haumea $  Copyright (c) UV Software, Berlin.
