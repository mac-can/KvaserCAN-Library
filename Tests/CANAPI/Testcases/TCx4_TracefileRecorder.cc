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

#ifndef FEATURE_TRACEFILE
#define FEATURE_TRACEFILE  FEATURE_UNSUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_TRACEFILE not set, default = FEATURE_UNSUPPORTED" )
#else
#warning FEATURE_TRACEFILE not set, default = FEATURE_UNSUPPORTED
#endif
#endif
#if (FEATURE_TRACEFILE != FEATURE_UNSUPPORTED)

#define TRACEFILE_FOLDER  "TraceFiles"
#define NONWRITABLE_FOLDER  "/"

#define TEST_FRAMES1  7
#define TEST_FRAMES2  9
#define TEST_FRAMES3  256

#define TEST_TRACESIZE  (CANPARA_TRACE_SIZE_10KB / CANPARA_TRACE_SIZE_10KB)
#define TEST_FILESIZE   (TEST_TRACESIZE * CANPARA_TRACE_SIZE_10KB)

class TraceFile : public testing::Test {
    virtual void SetUp() {
#if defined(_WIN32) || defined(_WIN64)
        struct _stat info;
        if (_stat(TRACEFILE_FOLDER, &info) != 0) {
            _mkdir(TRACEFILE_FOLDER);
        }
#else 
        struct stat info;
        if (stat(TRACEFILE_FOLDER, &info) != 0) {
            mkdir(TRACEFILE_FOLDER, (mode_t)0755);
        }
#endif
    }
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TCx4.0: Trace CAN messages (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(TraceFile, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint8_t traceType;
    uint16_t traceMode;
    uint16_t traceSize;
    char string[CANPROP_MAX_STRING_LENGTH+1] = "";
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test:
    // @- set trace file location to "TraceFiles" in current directory
    strcpy(string, TRACEFILE_FOLDER);
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- open trace file for DUT1 (vendor specific)
    traceType = CANPARA_TRACE_TYPE_VENDOR;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = CANPARA_TRACE_MODE_OVERWRITE;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if trace file is open
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_ON, traceState);
    // @- check trace file format (expect vendor specific)
    traceType = CANPARA_TRACE_TYPE_BINARY;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_VENDOR, traceType);
    // @- check trace file mode (expect overwrite)
    traceMode = CANPARA_TRACE_MODE_DEFAULT;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_MODE_OVERWRITE, traceMode);
    // @- check trace file size (expect 10240 * 10KB = 100MB)
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_DEFAULT, traceSize);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- close trace file for DUT1
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @post:
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.1: Trace CAN messages with invalid interface handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TCx4.2: Trace CAN messages if CAN channel is not initialized
//
// @note: this test is covered by TC21.5 (resp. TC12.5)

// @gtest TCx4.2: Trace CAN messages if CAN controller is not started
//
// @note: this test is covered by TC21.6 (resp. TC12.6)

// @gtest TCx4.4: Trace CAN messages if CAN controller is started
//
// @note: this test is covered by TC21.7 (resp. TC12.7)

// @gtest TCx4.5: Trace CAN messages if CAN controller was previously stopped
//
// @note: this test is covered by TC21.8 (resp. TC12.8)

// @gtest TCx4.6: Trace CAN messages if CAN channel was previously torn down
//
// @note: this test is covered by TC21.9 (resp. TC12.9)

// @gtest TCx4.7: Check trace-file option 'TRACE_TYPE'
//
// @expected: CANERR_ILLPARA for invalid trace types
//
TEST_F(TraceFile, GTEST_TESTCASE(CheckOptionTraceType, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint8_t traceType;
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @test:
    // @- get trace file default type (binary format)
    CCounter counter = CCounter();
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_BINARY, traceType);
    // @- sub(1): with valid type 0x00 (binary format)
    counter.Increment();
    traceType = CANPARA_TRACE_TYPE_BINARY;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_BINARY, traceType);
    // @- sub(2): with valid type 0x01 (logger format)
    counter.Increment();
    traceType = CANPARA_TRACE_TYPE_LOGGER;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_LOGGER, traceType);
    // @- sub(3): with invalid type 0x02 (expect ILLPARA)
    counter.Increment();
    traceType = 0x02;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_LOGGER, traceType);
    // @- sub(4): with invalid type 0x7F (expect ILLPARA)
    counter.Increment();
    traceType = 0x7F;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_LOGGER, traceType);
    // @- sub(5): with valid type 0x80 (vendor specific)
    counter.Increment();
    traceType = CANPARA_TRACE_TYPE_VENDOR;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_VENDOR, traceType);
    // @- sub(6): with invalid type 0x81 (expect ILLPARA)
    counter.Increment();
    traceType = 0x81;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_VENDOR, traceType);
    // @- sub(7): with invalid type 0xFF (expect ILLPARA)
    counter.Increment();
    traceType = UINT8_MAX;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceType = 0x00;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_VENDOR, traceType);
    // @post:
    counter.Clear();
    // @- check if trace file is still closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.8: Check trace-file option 'TRACE_MODE'
//
// @expected: CANERR_ILLPARA for invalid trace modes
//
TEST_F(TraceFile, GTEST_TESTCASE(CheckOptionTraceMode, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint16_t traceMode;
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @test:
    // @- get trace file default mode (0x0000)
    CCounter counter = CCounter();
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_MODE_DEFAULT, traceMode);
    // @- sub(1): with valid mode 0x0000 (default)
    counter.Increment();
    traceMode = CANPARA_TRACE_MODE_DEFAULT;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
#if (0)
    EXPECT_EQ(CANPARA_TRACE_MODE_DEFAULT, traceMode);
#else
    EXPECT_EQ(CANPARA_TRACE_MODE_OVERWRITE, traceMode);
#endif
    // @- sub(2): with valid mode 0x80 (overwrite)
    counter.Increment();
    traceMode = CANPARA_TRACE_MODE_OVERWRITE;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_MODE_OVERWRITE, traceMode);
    // @- sub(3): with valid mode 0x01 (segmented)
    counter.Increment();
    traceMode = CANPARA_TRACE_MODE_SEGMENTED;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
#if (0)
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
#if (0)
    EXPECT_EQ(CANPARA_TRACE_MODE_SEGMENTED, traceMode);
#else
    EXPECT_EQ(CANPARA_TRACE_MODE_SEGMENTED | CANPARA_TRACE_MODE_OVERWRITE, traceMode);
#endif
#else
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
#endif
    // @- sub(4): with valid mode 0x40 (compressed)
    counter.Increment();
    traceMode = CANPARA_TRACE_MODE_COMPRESSED;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
#if (0)
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
#if (0)
    EXPECT_EQ(CANPARA_TRACE_MODE_COMPRESSED, traceMode);
#else
    EXPECT_EQ(CANPARA_TRACE_MODE_COMPRESSED | CANPARA_TRACE_MODE_OVERWRITE, traceMode);
#endif
else
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
#endif
    // @- sub(5): with valid mode 0x02 (date prefix)
    counter.Increment();
    traceMode = CANPARA_TRACE_MODE_PREFIX_DATE;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
#if (0)
    EXPECT_EQ(CANPARA_TRACE_MODE_PREFIX_DATE, traceMode);
#else
    EXPECT_EQ(CANPARA_TRACE_MODE_PREFIX_DATE | CANPARA_TRACE_MODE_OVERWRITE, traceMode);
#endif
    // @- sub(6): with valid mode 0x04 (time prefix)
    counter.Increment();
    traceMode = CANPARA_TRACE_MODE_PREFIX_TIME;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
#if (0)
    EXPECT_EQ(CANPARA_TRACE_MODE_PREFIX_TIME, traceMode);
#else
    EXPECT_EQ(CANPARA_TRACE_MODE_PREFIX_TIME | CANPARA_TRACE_MODE_OVERWRITE, traceMode);
#endif
    // @- sub(7): with valid mode 0x100 (LEN output)
    counter.Increment();
    traceMode = CANPARA_TRACE_MODE_OUTPUT_LEN;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
#if (0)
    EXPECT_EQ(CANPARA_TRACE_MODE_OUTPUT_LEN, traceMode);
#else
    EXPECT_EQ(CANPARA_TRACE_MODE_OUTPUT_LEN | CANPARA_TRACE_MODE_OVERWRITE, traceMode);
#endif
    // @- sub(8): with all valid modes combined
    counter.Increment();
    traceMode = (CANPARA_TRACE_MODE_OVERWRITE | \
               /*CANPARA_TRACE_MODE_SEGMENTED |*/ \
               /*CANPARA_TRACE_MODE_COMPRESSED |*/ \
                 CANPARA_TRACE_MODE_PREFIX_DATE | \
                 CANPARA_TRACE_MODE_PREFIX_TIME | \
                 CANPARA_TRACE_MODE_OUTPUT_LEN);
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ((CANPARA_TRACE_MODE_OVERWRITE | \
             /*CANPARA_TRACE_MODE_SEGMENTED |*/ \
             /*CANPARA_TRACE_MODE_COMPRESSED |*/ \
               CANPARA_TRACE_MODE_PREFIX_DATE | \
               CANPARA_TRACE_MODE_PREFIX_TIME | \
               CANPARA_TRACE_MODE_OUTPUT_LEN), traceMode);
    // @- sub(9): with invalid modes combined (expect ILLPARA)
    counter.Increment();
    traceMode = (CANPARA_TRACE_MODE_OVERWRITE | \
               /*CANPARA_TRACE_MODE_SEGMENTED |*/ \
               /*CANPARA_TRACE_MODE_COMPRESSED |*/ \
                 CANPARA_TRACE_MODE_PREFIX_DATE | \
                 CANPARA_TRACE_MODE_PREFIX_TIME | \
                 CANPARA_TRACE_MODE_OUTPUT_LEN);
    traceMode = ~traceMode;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ((CANPARA_TRACE_MODE_OVERWRITE | \
             /*CANPARA_TRACE_MODE_SEGMENTED |*/ \
             /*CANPARA_TRACE_MODE_COMPRESSED |*/ \
               CANPARA_TRACE_MODE_PREFIX_DATE | \
               CANPARA_TRACE_MODE_PREFIX_TIME | \
               CANPARA_TRACE_MODE_OUTPUT_LEN), traceMode);
    // @- sub(10): with invalid mode 0xFFFF (expect ILLPARA)
    counter.Increment();
    traceMode = UINT16_MAX;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceMode = 0x0000;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ((CANPARA_TRACE_MODE_OVERWRITE | \
             /*CANPARA_TRACE_MODE_SEGMENTED |*/ \
             /*CANPARA_TRACE_MODE_COMPRESSED |*/ \
               CANPARA_TRACE_MODE_PREFIX_DATE | \
               CANPARA_TRACE_MODE_PREFIX_TIME | \
               CANPARA_TRACE_MODE_OUTPUT_LEN), traceMode);
    // @post:
    counter.Clear();
    // @- check if trace file is still closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.9: Check trace-file option 'TRACE_SIZE'
//
// @expected: CANERR_ILLPARA for invalid trace sizes
//
TEST_F(TraceFile, GTEST_TESTCASE(CheckOptionTraceSize, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint16_t traceSize;
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @test:
    CCounter counter = CCounter();
    // @- get default trace file size (10240 * 10KB = 100MB)
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_DEFAULT, traceSize);
    // @- sub(1): with valid size 0 (default)
    counter.Increment();
    traceSize = 0;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_DEFAULT, traceSize);
    // @- sub(2): with valid size 1 (10KB)
    counter.Increment();
    traceSize = 1;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(1, traceSize);
    // @- sub(3): with valid size 10240 (100MB)
    counter.Increment();
    traceSize = CANPARA_TRACE_SIZE_DEFAULT;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_DEFAULT, traceSize);
    // @- sub(4): with valid size 62500 (640MB)
    counter.Increment();
    traceSize = CANPARA_TRACE_SIZE_LIMIT;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_LIMIT, traceSize);
    // @- sub(5): with invalid size 62501 (expect ILLPARA)
    counter.Increment();
    traceSize = CANPARA_TRACE_SIZE_LIMIT + 1;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_LIMIT, traceSize);
    // @- sub(6): with invalid size 65535 (expect ILLPARA)
    counter.Increment();
    traceSize = UINT16_MAX;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    traceSize = 0;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_LIMIT, traceSize);
    // @post:
    counter.Clear();
    // @- check if trace file is still closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.10: Check trace-file option 'TRACE_FOLDER'
//
// @expected: CANERR_NOERROR (validity is checked with 'TRACE_ACTIVE' = 'TRACE_ON')
//
TEST_F(TraceFile, GTEST_TESTCASE(CheckOptionTraceFolder, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    char string[CANPROP_MAX_STRING_LENGTH+1] = "";
    char folder[CANPROP_MAX_STRING_LENGTH+1] = "";
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @test:
    // @- get current working directory
    getcwd(folder, CANPROP_MAX_STRING_LENGTH);
    folder[CANPROP_MAX_STRING_LENGTH] = '\0';
    // @- get default trace file folder (current working directory)
    CCounter counter = CCounter();
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ(folder, string);
    // @- sub(1): with default folder "."
    counter.Increment();
    strcpy(string, ".");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ(".", string);
    // @- sub(2): with parent folder ".."
    counter.Increment();
    strcpy(string, "..");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("..", string);
    // @- sub(3): with user folder "~"
    counter.Increment();
    strcpy(string, "~");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("~", string);
    // @- sub(4): with root folder "/"
    counter.Increment();
    strcpy(string, "/");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("/", string);
    // @- sub(5): with no folder
    counter.Increment();
    string[0] = '\0';
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("", string);  // FIXME: should be "." or not?
    // @- sub(6): with temporary folder "traces"
    counter.Increment();
    strcpy(string, "traces");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("traces", string);
    // @- sub(8): with temporary folder ".traces"
    counter.Increment();
    strcpy(string, ".traces");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ(".traces", string);
    // @- sub(8): with temporary folder "./traces"
    counter.Increment();
    strcpy(string, "./traces");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("./traces", string);
    // @- sub(9): with temporary folder "~/traces"
    counter.Increment();
    strcpy(string, "~/traces");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("~/traces", string);
    // @- sub(10): with temporary folder "~/traces/bin"
    counter.Increment();
    strcpy(string, "~/traces/bin");
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ("~/traces/bin", string);
    // @post:
    counter.Clear();
    // @- check if trace file is still closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.11: Check trace-file option 'TRACE_ACTIVE'
//
// @expected: CANERR_RESOURCE if trace file is already opened
//
TEST_F(TraceFile, GTEST_TESTCASE(CheckOptionTraceActive, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    char string[CANPROP_MAX_STRING_LENGTH+1] = "";
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @test:
    // @- note: set trace file location to "TraceFiles" in current directory
    CCounter counter = CCounter();
    strcpy(string, TRACEFILE_FOLDER);
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- sub(1): set tracing OFF if not started (expect NOERROR)
    counter.Increment();
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- sub(2): set tracing ON with value 42 (expect NOERROR)
    counter.Increment();
    traceState = 42;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_ON, traceState);
    // @- sub(3): set tracing ON if already started (expect RESOURCE error)
    counter.Increment();
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::ResourceError, retVal);
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_ON, traceState);
    // @- sub(4): set tracing OFF to close trace file (expect NOERROR)
    counter.Increment();
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- sub(5): set tracing OFF if already stopped (expect NOERROR)
    counter.Increment();
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @post:
    counter.Clear();
    // @- check if trace file is still closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.12: Check trace-file option 'TRACE_FILE'
//
// @expected: CANERR_NOERROR
//
TEST_F(TraceFile, GTEST_TESTCASE(CheckOptionTraceFileName, GTEST_DISABLED)) {
    // @
    // @todo: implement test case; the device name is part of the trace file name
    // @      provide the device name via CDevice::GetDeviceName()
    // @      provide the current working directory via getcwd()
    // @      sub(1): with default file name
    // @      sub(2): with date/time prefix
    // @      sub(3): with segment number (when implemented)
    // @
}

// @gtest TCx4.13: Trace CAN messages with default options
//
// @expected: CANERR_NOERROR (type=VENDOR, mode=OVERWRITE, size=100MB, folder=".")
//
TEST_F(TraceFile, GTEST_TESTCASE(WithDefaultOptions, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint8_t traceType;
    uint16_t traceMode;
    uint16_t traceSize;
    char string[CANPROP_MAX_STRING_LENGTH+1] = "";
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test:
    // @- note: set trace file location to "TraceFiles" in current directory
    strcpy(string, TRACEFILE_FOLDER);
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- open trace file for DUT1 (default options)
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if trace file is open
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_ON, traceState);
    // @- check trace file format (expect binary)
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_BINARY, traceType);
    // @- check trace file mode (expect default)
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_MODE_DEFAULT, traceMode);
    // @- check trace file size (expect 10240 * 10KB = 100MB)
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_DEFAULT, traceSize);
    // @- check trace file location (expect "TraceFiles" in current directory)
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ(TRACEFILE_FOLDER, string);
    // @- check trace file name (expect extension ".dat")
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FILE, (void*)string, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @  todo: check the entire file name
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- close trace file for DUT1
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.14: Trace CAN messages with option APPEND (if trace file exists)
//
// @todo: implement this test case if option APPEND is ever realized.

// @gtest TCx4.15: Trace CAN messages with option OVERWRITE (if trace file exists)
//
// @expected: CANERR_xyz
//
TEST_F(TraceFile, GTEST_TESTCASE(WithOptionOverwriteIfFileExists, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint16_t traceMode;
    char string[CANPROP_MAX_STRING_LENGTH+1] = "";
    char fname1[CANPROP_MAX_STRING_LENGTH+1] = "";
    char fname2[CANPROP_MAX_STRING_LENGTH+1] = "";
    struct stat fileStat1;
    struct stat fileStat2;
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test:
    // @- note: set trace file location to "TraceFiles" in current directory
    strcpy(string, TRACEFILE_FOLDER);
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- sub(1):
    // @-- open trace file for DUT1 (default options)
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- check if trace file is open
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_ON, traceState);
    // @-- get trace file name (1)
    fname1[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FILE, (void*)fname1, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @-- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames() + TEST_FRAMES1;
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @-- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @-- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @-- close trace file for DUT1
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- check if trace file exists
    EXPECT_EQ(0, stat(fname1, &fileStat1));
    // @- sub(2):
    // @-- set trace option to OVERWRITE
    traceMode = CANPARA_TRACE_MODE_OVERWRITE;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- open trace file for DUT1 (with mode OVERWRITE)
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- check if trace file is open
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_ON, traceState);
    // @-- get trace file name (2) and compare with file name (1)
    fname2[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FILE, (void*)fname2, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @-- send some frames to DUT2 and receive some frames from DUT2
    frames = g_Options.GetNumberOfTestFrames() + TEST_FRAMES2;
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @-- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @-- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @-- close trace file for DUT1
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @-- check if trace file exists (expect same name, but different size)
    EXPECT_EQ(0, stat(fname2, &fileStat2));
    EXPECT_STREQ(fname1, fname2);
    EXPECT_NE(fileStat1.st_size, fileStat2.st_size);
    // @-- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.16: Trace CAN messages with non-writable folder (permission denied)
//
// @expected: CANERR_RESOURCE (errno=EROFS)
//
TEST_F(TraceFile, GTEST_TESTCASE(WithNonWritableFolder, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint8_t traceType;
    uint16_t traceMode;
    uint16_t traceSize;
    char string[CANPROP_MAX_STRING_LENGTH+1] = "";
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test:
    // @- set trace file location to non-writable root folder "/"
    strcpy(string, NONWRITABLE_FOLDER);
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- open trace file for DUT1 (expect RESOURCE error)
    errno = 0;
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::ResourceError, retVal);
    EXPECT_EQ(EROFS, errno);
    // @- check if trace file is open (expect OFF)
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- check trace file format (expect binary)
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_BINARY, traceType);
    // @- check trace file mode (expect default)
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_MODE_DEFAULT, traceMode);
    // @- check trace file size (expect 10240 * 10KB = 100MB)
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_SIZE_DEFAULT, traceSize);
    // @- check trace file location (expect non-writable root folder "/")
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ(NONWRITABLE_FOLDER, string);
    // @- check trace file name (expect RESOURC error)
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FILE, (void*)string, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::ResourceError, retVal);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames();
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- close trace file for DUT1
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.17: Trace CAN messages with limitation
//
// @expected: CANERR_NOERROR (but trace file size is limited)
//
TEST_F(TraceFile, GTEST_TESTCASE(WithSizeLimitation, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    uint8_t traceState;
    uint8_t traceType;
    uint16_t traceMode;
    uint16_t traceSize;
    char string[CANPROP_MAX_STRING_LENGTH+1] = "";
    char fname[CANPROP_MAX_STRING_LENGTH+1] = "";
    struct stat fileStat;
    // @pre:
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test:
    // @- note: set trace file location to "TraceFiles" in current directory
    strcpy(string, TRACEFILE_FOLDER);
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_FOLDER, (void*)string, sizeof(string));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- set trace file size to 1 * 10KB = 10KB
    traceSize = TEST_TRACESIZE;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- open trace file for DUT1 (default options with size limitation)
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if trace file is open
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_ON, traceState);
    // @- check trace file format (expect binary)
    traceType = UINT8_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_TYPE, (void*)&traceType, sizeof(traceType));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_TYPE_BINARY, traceType);
    // @- check trace file mode (expect default)
    traceMode = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_MODE, (void*)&traceMode, sizeof(traceMode));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_MODE_DEFAULT, traceMode);
    // @- check trace file size (expect 1 * 10KB = 10KB)
    traceSize = UINT16_MAX;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_SIZE, (void*)&traceSize, sizeof(traceSize));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(TEST_TRACESIZE, traceSize);
    // @- check trace file location (expect "TraceFiles" in current directory)
    string[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FOLDER, (void*)string, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_STREQ(TRACEFILE_FOLDER, string);
    // @- check trace file name (expect extension ".dat")
    fname[0] = '\0';
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_FILE, (void*)fname, CANPROP_MAX_STRING_LENGTH);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    int32_t frames = g_Options.GetNumberOfTestFrames() + TEST_FRAMES3;
    EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
    EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- close trace file for DUT1
    traceState = CANPARA_TRACE_OFF;
    retVal = dut1.SetProperty(CANPROP_SET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if trace file is closed
    traceState = CANPARA_TRACE_ON;
    retVal = dut1.GetProperty(CANPROP_GET_TRACE_ACTIVE, (void*)&traceState, sizeof(traceState));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANPARA_TRACE_OFF, traceState);
    // @- check if trace file size is limited to 1 * 10KB = 10KB
    EXPECT_EQ(0, stat(fname, &fileStat));
    EXPECT_EQ(TEST_FILESIZE, fileStat.st_size);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCx4.18: Trace CAN messages with segmentation
//
// @expected: CANERR_xyz
//
TEST_F(TraceFile, GTEST_TESTCASE(WithFileSegmentation, GTEST_DISABLED)) {
}

#endif // FEATURE_TRACEFILE != FEATURE_UNSUPPORTED

//  $Id: TCx4_TracefileRecorder.cc 1393 2024-08-13 15:08:28Z makemake $  Copyright (c) UV Software, Berlin.
