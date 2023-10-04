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
#include "pch.h"

#ifndef CAN_FD_SUPPORTED
#define CAN_FD_SUPPORTED  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "CAN_FD_SUPPORTED not set, default = FEATURE_SUPPORTED" )
#else
#warning CAN_FD_SUPPORTED not set, default = FEATURE_SUPPORTED
#endif
#endif
#ifndef FEATURE_BITRATE_5K
#define FEATURE_BITRATE_5K  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_BITRATE_5K not set, default = FEATURE_SUPPORTED" )
#else
#warning FEATURE_BITRATE_5K not set, default = FEATURE_SUPPORTED
#endif
#endif
#ifndef FEATURE_BITRATE_800K
#define FEATURE_BITRATE_800K  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_BITRATE_800K not set, default = FEATURE_SUPPORTED" )
#else
#warning FEATURE_BITRATE_800K not set, default = FEATURE_SUPPORTED
#endif
#endif
#ifndef FEATURE_BITRATE_SAM
#define FEATURE_BITRATE_SAM  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_BITRATE_SAM not set, default = FEATURE_SUPPORTED" )
#else
#warning FEATURE_BITRATE_SAM not set, default = FEATURE_SUPPORTED
#endif
#endif
#ifndef FEATURE_BITRATE_FD_SAM
#define FEATURE_BITRATE_FD_SAM  FEATURE_UNSUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_BITRATE_FD_SAM not set, default = FEATURE_UNSUPPORTED" )
#else
#warning FEATURE_BITRATE_FD_SAM not set, default = FEATURE_UNSUPPORTED
#endif
#endif
#ifndef FEATURE_BITRATE_SJA1000
#define FEATURE_BITRATE_SJA1000  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_BITRATE_SJA1000 not set, default = FEATURE_SUPPORTED" )
#else
#warning FEATURE_BITRATE_SJA1000 not set, default = FEATURE_SUPPORTED
#endif
#endif

#define NOM_BRP_MIN    CANBTR_NOMINAL_BRP_MIN
#define NOM_BRP_MAX    CANBTR_NOMINAL_BRP_MAX
#define NOM_TSEG1_MIN  CANBTR_NOMINAL_TSEG1_MIN
#define NOM_TSEG1_MAX  CANBTR_NOMINAL_TSEG1_MAX
#define NOM_TSEG2_MIN  CANBTR_NOMINAL_TSEG2_MIN
#define NOM_TSEG2_MAX  CANBTR_NOMINAL_TSEG2_MAX
#define NOM_SJW_MIN    CANBTR_NOMINAL_SJW_MIN
#define NOM_SJW_MAX    CANBTR_NOMINAL_SJW_MAX

#define DATA_BRP_MIN    CANBTR_DATA_BRP_MIN
#define DATA_BRP_MAX    CANBTR_DATA_BRP_MAX
#define DATA_TSEG1_MIN  CANBTR_DATA_TSEG1_MIN
#define DATA_TSEG1_MAX  CANBTR_DATA_TSEG1_MAX
#define DATA_TSEG2_MIN  CANBTR_DATA_TSEG2_MIN
#define DATA_TSEG2_MAX  CANBTR_DATA_TSEG2_MAX
#define DATA_SJW_MIN    CANBTR_DATA_SJW_MIN
#define DATA_SJW_MAX    CANBTR_DATA_SJW_MAX

#define CLEAR_BTR(btr)  memset(&btr, 0, sizeof(CANAPI_Bitrate_t))

class StartController : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC03.0: Start CAN controller (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @post:
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

// @gtest TC03.1: Start CAN controller with invalid channel handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC03.2: Give a NULL pointer as argument for parameter 'bitrate'
//
// @note: passing a pointer for 'bitrate' is not possible with the C++ API!

// @gtest TC03.3: Start CAN controller if CAN channel is not initialized
//
// @expected: CANERR_NOTINIT
//
TEST_F(StartController, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @test:
    // @- try to start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @post:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
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

// @gtest TC03.4: Start CAN controller if CAN controller is already started
//
// @expected: CANERR_ONLINE
//
TEST_F(StartController, GTEST_TESTCASE(IfControllerStarted, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @test:
    // @- try to start DUT1 with configured bit-rate settings again
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::ControllerOnline, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @post:
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

// @gtest TC03.5: Start CAN controller if CAN controller was previously stopped
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send one frame to DUT2 and receive one frame from DUT2
    int32_t one = 1;
    EXPECT_EQ(one, dut1.SendSomeFrames(dut2, one));
    EXPECT_EQ(one, dut1.ReceiveSomeFrames(dut2, one));
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
    // @test:
    // @- start DUT1 with configured bit-rate settings again
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @post:
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

// @gtest TC03.6: Start CAN controller if CAN channel was previously torn down
//
// @expected: CANERR_NOTINIT
//
TEST_F(StartController, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
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
    // @test:
    // @- try to start DUT1 with configured bit-rate settings again
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @end.
}

// @gtest TC03.7: Start CAN controller with valid CAN 2.0 bit-timing indexes
//
// @remarks: TC03.8 to TC03.15 joined into TC03.7 with the CAN API C3 xctest suite
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithValidCanBitrateIndex, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @loop over selected CAN 2.0 bit-timing indexes
    // @note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @      The index must be given as negative value to 'bitrate.index'!
    // @      Remark: The CiA bit-timing table has only 9 entries!
    CCounter counter = CCounter();
    for (int i = 0; i < 10; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): index 0 (1Mbps)
        case 0: bitrate.index = CANBTR_INDEX_1M; break;
        // @sub(2): index 1 (800kbps, not supported by all CAN controllers)
        case 1: bitrate.index = CANBTR_INDEX_800K; break;
        // @sub(3): index 2 (500kbps)
        case 2: bitrate.index = CANBTR_INDEX_500K; break;
        // @sub(4): index 3 (250kbps)
        case 3: bitrate.index = CANBTR_INDEX_250K; break;
        // @sub(5): index 4 (125kbps)
        case 4: bitrate.index = CANBTR_INDEX_125K; break;
        // @sub(6): index 5 (100kbps)
        case 5: bitrate.index = CANBTR_INDEX_100K; break;
        // @sub(7): index 6 (50kbps)
        case 6: bitrate.index = CANBTR_INDEX_50K; break;
        // @sub(8): index 7 (20kbps)
        case 7: bitrate.index = CANBTR_INDEX_20K; break;
        // @sub(9): index 8 (10kbps)
        case 8: bitrate.index = CANBTR_INDEX_10K; break;
        // @sub(10): index 9 (5kbps, not supported by all CAN API SDK's)
        case 9: bitrate.index = CANBTR_INDEX_5K; break;
        default: return;  // Get out of here!
        }
#if (FEATURE_BITRATE_800K != FEATURE_SUPPORTED)
        if (bitrate.index == CANBTR_INDEX_800K)
            continue;
#endif
#if (FEATURE_BITRATE_5K != FEATURE_SUPPORTED)
        if (bitrate.index == CANBTR_INDEX_5K)
            continue;
#endif
#if (TC03_7_ISSUE_RUSOKU_BITRATE_10K != WORKAROUND_DISABLED)
        // @! issue(MacCAN-TouCAN): 10kbps hardware bug (known issue)
        if (bitrate.index == CANBTR_INDEX_10K)
            continue;
#endif
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN 2.0 operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- start DUT1 with selected bit-timing index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        retVal = CCanDevice::MapIndex2Bitrate(expected.index, expected);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            counter.Clear();
            // @   only if no other CAN device is on bus:
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.16: Start CAN controller with invalid CAN 2.0 bit-timing indexes
//
// @expected: CANERR_BAUDRATE
//
TEST_F(StartController, GTEST_TESTCASE(WithInvalidCanBitrateIndex, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test: loop over invalid CAN 2.0 bit-timing indexes
    // @note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @      The index must be given as negative value to 'bitrate.index'!
    // @      Remark: The CiA bit-timing table has only 9 entries!
    // @note: Positive values represent the CAN clock in Hertz, but there will
    // @      be probably no clock below 10 Hertz (or above 999'999'999 Hertz).
    CCounter counter = CCounter();
    for (int i = 0; i < 14; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): invalid index -10
#if (FEATURE_BITRATE_5K == FEATURE_SUPPORTED)
        case 0: bitrate.index = CANBTR_INDEX_5K - 1; break;
#else
        case 0: bitrate.index = CANBTR_INDEX_10K - 1; break;
#endif
        // @sub(2): invalid index INT8_MIN
        case 1: bitrate.index = INT8_MIN; break;
        // @sub(3): invalid index INT16_MIN
        case 2: bitrate.index = INT16_MIN; break;
        // @sub(4): invalid index INT32_MIN+1
        case 3: bitrate.index = INT32_MIN + 1; break;
        // @sub(5): invalid index INT32_MIN
        case 4: bitrate.index = INT32_MIN; break;
        // @sub(6): invalid index 1
        case 5: bitrate.index = CANBDR_800; EXPECT_EQ(1, bitrate.index); break;
        // @sub(7): invalid index 2
        case 6: bitrate.index = CANBDR_500; EXPECT_EQ(2, bitrate.index); break;
        // @sub(8): invalid index 3
        case 7: bitrate.index = CANBDR_250; EXPECT_EQ(3, bitrate.index); break;
        // @sub(9): invalid index 4
        case 8: bitrate.index = CANBDR_125; EXPECT_EQ(4, bitrate.index); break;
        // @sub(10): invalid index 5
        case 9: bitrate.index = CANBDR_100; EXPECT_EQ(5, bitrate.index); break;
        // @sub(11): invalid index 6
        case 10: bitrate.index = CANBDR_50; EXPECT_EQ(6, bitrate.index); break;
        // @sub(12): invalid index 7
        case 11: bitrate.index = CANBDR_20; EXPECT_EQ(7, bitrate.index); break;
        // @sub(13): invalid index 8
        case 12: bitrate.index = CANBDR_10; EXPECT_EQ(8, bitrate.index); break;
        // @sub(14): invalid index INT32_MAX
        case 13: bitrate.index = INT32_MAX; break;
        default: return;  // Get out of here!
        }
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- try to start DUT1 with invalid index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @   note: stop/reset DUT1 if started anyway
        if (!status.can_stopped)
            (void)dut1.ResetController();
        // @- endloop
    }
    counter.Clear();
    // @post:
    // @- start DUT1 with default bit-timing index
    bitrate.index = TEST_BTRINDEX;
    dut1.SetBitrate(bitrate);
    // dut1.ShowBitrateSettings();
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    if (!g_Options.Is3rdDevicePresent()) {
        // @  only if no other CAN device is on bus:
        // @- tell DUT2 about the changed settings
        dut2.SetOpMode(opMode);
        dut2.SetBitrate(bitrate);
        // @- send some frames to DUT2 and receive some frames from DUT2
        int32_t frames = g_Options.GetNumberOfTestFrames();
        EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
        EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        // @- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    }
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

// @gtest TC03.17: Re-Start CAN controller with the same CAN 2.0 bit-timing index
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithSameCanBitrateIndexAfterCanStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
    // @loop over selected CAN 2.0 bit-timing indexes
    // @note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @      The index must be given as negative value to 'bitrate.index'!
    // @      Remark: The CiA bit-timing table has only 9 entries!
    CCounter counter = CCounter();
    for (int i = 0; i < 3; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): index 0 (1Mbps)
        case 0: bitrate.index = FAST_BTRINDEX; break;
        // @sub(2): index 3 (250kbps)
        case 1: bitrate.index = TEST_BTRINDEX; break;
        // @sub(3): index 8 (10kbps)
        case 2: bitrate.index = SLOW_BTRINDEX; break;
        default: return;  // Get out of here!
        }
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN 2.0 operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- start DUT1 with selected bit-timing index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        retVal = CCanDevice::MapIndex2Bitrate(expected.index, expected);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- re-start DUT1 with selected bit-timing index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        expected = dut1.GetBitrate();
        retVal = CCanDevice::MapIndex2Bitrate(expected.index, expected);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.18: Re-Start CAN controller with a different CAN 2.0 bit-timing index
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithDifferentCanBitrateIndexAfterCanStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
    // @loop over selected CAN 2.0 bit-timing indexes
    // @note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @      The index must be given as negative value to 'bitrate.index'!
    // @      Remark: The CiA bit-timing table has only 9 entries!
    CCounter counter = CCounter();
    for (int i = 0; i < 3; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): index 0 (1Mbps)
        case 0: bitrate.index = FAST_BTRINDEX; break;
        // @sub(2): index 3 (250kbps)
        case 1: bitrate.index = TEST_BTRINDEX; break;
        // @sub(3): index 8 (10kbps)
        case 2: bitrate.index = SLOW_BTRINDEX; break;
        default: return;  // Get out of here!
        }
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN 2.0 operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- start DUT1 with selected bit-timing index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        retVal = CCanDevice::MapIndex2Bitrate(expected.index, expected);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- change bit-timing index: 
        CLEAR_BTR(bitrate);
        switch (i) {
        // @   - sub(3): index 8 (10kbps) ==> 0 (1Mbps)
        case 2: bitrate.index = FAST_BTRINDEX; break;
        // @   - sub(2): index 3 (250kbps) ==> 8 (10kbps)
        case 1: bitrate.index = SLOW_BTRINDEX; break;
        // @   - sub(1): index 0 (1Mbps) ==> 3 (250kbps)
        case 0: bitrate.index = TEST_BTRINDEX; break;
        default: return;  // Get out of here!
        }
        // @-- re-start DUT1 with changed bit-timing index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        expected = dut1.GetBitrate();
        retVal = CCanDevice::MapIndex2Bitrate(expected.index, expected);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.19: Start CAN controller with valid CAN 2.0 bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithValidCanBitrateSettings, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @loop over selected CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): 1Mbps
        case 0: BITRATE_1M(bitrate); break;
        // @sub(2): 500kbps
        case 1: BITRATE_500K(bitrate); break;
        // @sub(3): 250kbps
        case 2: BITRATE_250K(bitrate); break;
        // @sub(4): 125kbps
        case 3: BITRATE_125K(bitrate); break;
        // @sub(5): 100kbps
        case 4: BITRATE_100K(bitrate); break;
        // @sub(6): 50kbps
        case 5: BITRATE_50K(bitrate); break;
        // @sub(7): 20kbps
        case 6: BITRATE_20K(bitrate); break;
        // @sub(8): 10kbps
        case 7: BITRATE_10K(bitrate); break;
        default: return;  // Get out of here!
        }
#if (TC03_19_ISSUE_RUSOKU_BITRATE_10K != WORKAROUND_DISABLED)
        // @! issue(MacCAN-TouCAN): 10kbps hardware bug (known issue)
        if (i == 7)
            continue;
#endif
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN 2.0 operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.20: Start CAN controller with invalid CAN 2.0 bit-rate settings
//
// @expected: CANERR_BAUDRATE
//
TEST_F(StartController, GTEST_TESTCASE(WithInvalidCanBitrateSettings, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test: loop over invalid CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 18; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        BITRATE_250K(bitrate);
        switch (i) {
        // @sub(1): set all fields to 0 (note: 'frequency' == 0 is CiA Index 0, set to 1 instead)
        case 0: bitrate.btr.frequency = 1; bitrate.btr.nominal.brp = 0; bitrate.btr.nominal.tseg1 = 0; bitrate.btr.nominal.tseg2 = 0; bitrate.btr.nominal.sjw = 0; bitrate.btr.nominal.sam = 0; break;
        // @sub(2): set all fields to MAX (acc. data type)
        case 1: bitrate.btr.frequency = INT32_MAX; bitrate.btr.nominal.brp = UINT16_MAX; bitrate.btr.nominal.tseg1 = UINT16_MAX; bitrate.btr.nominal.tseg2 = UINT16_MAX; bitrate.btr.nominal.sjw = UINT16_MAX; bitrate.btr.nominal.sam = UINT8_MAX; break;
        // @sub(3): set field 'frequency' to 1 (note: see above)
        case 2: bitrate.btr.frequency = 1; break;
        // @sub(4): set field 'frequency' to INT32_MAX
        case 3: bitrate.btr.frequency = INT32_MAX; break;
        // @sub(5): set field 'brp' to 0
        case 4: bitrate.btr.nominal.brp = 0; break;
        // @sub(6): set field 'brp' to 1025
        case 5: bitrate.btr.nominal.brp = NOM_BRP_MAX + 1; break;
        // @sub(7): set field 'brp' to UINT16_MAX
        case 6: bitrate.btr.nominal.brp = UINT16_MAX; break;
        // @sub(8): set field 'tseg1' to 0
        case 7: bitrate.btr.nominal.tseg1 = 0; break;
        // @sub(9): set field 'tseg1' to 257
        case 8: bitrate.btr.nominal.tseg1 = NOM_TSEG1_MAX + 1; break;
        // @sub(10): set field 'tseg1' to UINT16_MAX
        case 9: bitrate.btr.nominal.tseg1 = UINT16_MAX; break;
        // @sub(11): set field 'tseg2' to 0
        case 10: bitrate.btr.nominal.tseg2 = 0; break;
        // @sub(12): set field 'tseg2' to 129
        case 11: bitrate.btr.nominal.tseg2 = NOM_TSEG2_MAX + 1; break;
        // @sub(13): set field 'tseg2' to UINT16_MAX
        case 12: bitrate.btr.nominal.tseg2 = UINT16_MAX; break;
        // @sub(14): set field 'sjw' to 0
        case 13: bitrate.btr.nominal.sjw = 0; break;
        // @sub(15): set field 'sjw' to 129
        case 14: bitrate.btr.nominal.sjw = NOM_SJW_MAX + 1; break;
        // @sub(16): set field 'sjw' to UINT16_MAX
        case 15: bitrate.btr.nominal.sjw = UINT16_MAX; break;
        // @sub(17): set field 'sam' to 2 (note: SAM not supported by all CAN controller)
        case 16: bitrate.btr.nominal.sam = 2; break;
        // @sub(18): set field 'sam' to UINT8_MAX (note: SAM not supported by all CAN controller)
        case 17: bitrate.btr.nominal.sam = UINT8_MAX; break;
        default: return;  // Get out of here!
        }
#if (FEATURE_BITRATE_SAM != FEATURE_SUPPORTED)
        if ((i == 16) || (i == 17))
            continue;
#endif
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- try to start DUT1 with invalid bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @   note: stop/reset DUT1 if started anyway
        if (!status.can_stopped)
            (void)dut1.ResetController();
        // @- endloop
    }
    counter.Clear();
    // @post:
    // @- start DUT1 with default bit-rate settings
    CLEAR_BTR(bitrate);
    BITRATE_250K(bitrate);
    dut1.SetBitrate(bitrate);
    // dut1.ShowBitrateSettings();
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    if (!g_Options.Is3rdDevicePresent()) {
        // @  only if no other CAN device is on bus:
        // @- tell DUT2 about the changed settings
        dut2.SetOpMode(opMode);
        dut2.SetBitrate(bitrate);
        // @- send some frames to DUT2 and receive some frames from DUT2
        int32_t frames = g_Options.GetNumberOfTestFrames();
        EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
        EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        // @- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    }
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

// @gtest TC03.21: Re-Start CAN controller with same CAN 2.0 bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithSameCanBitrateSettingsAfterCanStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
    // @loop over selected CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 3; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): 1Mbps
        case 0: FAST_BITRATE(bitrate); break;
        // @sub(2): 250kbps
        case 1: TEST_BITRATE(bitrate); break;
        // @sub(3): 10kbps
        case 2: SLOW_BITRATE(bitrate); break;
        default: return;  // Get out of here!
        }
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN 2.0 operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- re-start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.22: Re-Start CAN controller with different CAN 2.0 bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithDifferentCanBitrateSettingsAfterCanStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
    // @loop over selected CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 3; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): 1Mbps
        case 0: FAST_BITRATE(bitrate); break;
        // @sub(2): 250kbps
        case 1: TEST_BITRATE(bitrate); break;
        // @sub(3): 10kbps
        case 2: SLOW_BITRATE(bitrate); break;
        default: return;  // Get out of here!
        }
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN 2.0 operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- change bit-timing index: 
        CLEAR_BTR(bitrate);
        switch (i) {
        // @   - sub(3): 10kbps ==> 1Mbps
        case 2: FAST_BITRATE(bitrate); break;
        // @   - sub(2): 250kbps ==> 0kbps
        case 1: SLOW_BITRATE(bitrate); break;
        // @   - sub(1): 1Mbps ==> 250kbbps
        case 0: TEST_BITRATE(bitrate); break;
        default: return;  // Get out of here!
        }
        // @-- re-start DUT1 with changed bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @gtest TC03.23: Start CAN controller with valid CAN FD bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithValidCanFdBitrateSettings, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @loop over selected CAN FD bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): nominal 1Mbps (mode FDOE)
        case 0: BITRATE_FD_1M(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(2): nominal 500kbps (mode FDOE)
        case 1: BITRATE_FD_500K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(3): nominal 250kbps (mode FDOE)
        case 2: BITRATE_FD_250K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(4): nominal 125kbps (mode FDOE)
        case 3: BITRATE_FD_125K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(5): nominal 1Mbps, data phase 8Mbps (mode FDOE+BRSE)
        case 4: BITRATE_FD_1M8M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(6): nominal 500kbps, data phase 4Mbps (mode FDOE+BRSE)
        case 5: BITRATE_FD_500K4M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(7): nominal 250kbps, data phase 2Mbps (mode FDOE+BRSE)
        case 6: BITRATE_FD_250K2M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(8): nominal 125kbps, data phase 1Mbps (mode FDOE+BRSE)
        case 7: BITRATE_FD_125K1M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        default: return;  // Get out of here!
        }
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN FD operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual, opMode.brse ? true : false));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.24: Start CAN controller with invalid CAN FD bit-rate settings
//
// @expected: CANERR_BAUDRATE
//
TEST_F(StartController, GTEST_TESTCASE(WithInvalidCanFdBitrateSettings, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @pre:
    // @- initialize DUT1 in CAN FD operation mode
    opMode.fdoe = 1;
    opMode.brse = 1;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test: loop over invalid CAN FD bit-rate settings
    CCounter counter = CCounter();
    counter.Increment();
#if (FEATURE_BITRATE_FD_SAM == FEATURE_UNSUPPORTED)
    for (int i = 0; i < 28; i++) {  // @note: CAN FD does not specify number-of-samples attribute,
#else
    for (int i = 0; i < 30; i++) {  // @      but some drivers define the field SAM for CAN FD.
#endif
        CLEAR_BTR(bitrate);
        BITRATE_FD_250K2M(bitrate);
        switch (i) {
        // @sub(1): set all fields to 0 (note: 'frequency' == 0 is CiA Index 0, set to 1 instead)
        case 0: bitrate.btr.frequency = 1;
            bitrate.btr.nominal.brp = 0; bitrate.btr.nominal.tseg1 = 0; bitrate.btr.nominal.tseg2 = 0; bitrate.btr.nominal.sjw = 0; bitrate.btr.nominal.sam = 0;
            bitrate.btr.data.brp = 0; bitrate.btr.data.tseg1 = 0; bitrate.btr.data.tseg2 = 0; bitrate.btr.data.sjw = 0; break;
        // @sub(2): set all fields to MAX value (acc. data type)
        case 1: bitrate.btr.frequency = INT32_MAX;
            bitrate.btr.nominal.brp = UINT16_MAX; bitrate.btr.nominal.tseg1 = UINT16_MAX; bitrate.btr.nominal.tseg2 = UINT16_MAX; bitrate.btr.nominal.sjw = UINT16_MAX; bitrate.btr.nominal.sam = UINT8_MAX;
            bitrate.btr.data.brp = UINT16_MAX; bitrate.btr.data.tseg1 = UINT16_MAX; bitrate.btr.data.tseg2 = UINT16_MAX; bitrate.btr.data.sjw = UINT16_MAX; break;
        // @sub(3): set field 'frequency' to 1 (note: see above)
        case 2: bitrate.btr.frequency = 1; break;
        // @sub(4): set field 'frequency' to INT32_MAX
        case 3: bitrate.btr.frequency = INT32_MAX; break;
        // @sub(5): set field 'brp' to 0
        case 4: bitrate.btr.nominal.brp = 0; break;
        // @sub(6): set field 'brp' to 1025
        case 5: bitrate.btr.nominal.brp = NOM_BRP_MAX + 1; break;
        // @sub(7): set field 'brp' to UINT16_MAX
        case 6: bitrate.btr.nominal.brp = UINT16_MAX; break;
        // @sub(8): set field 'tseg1' to 0
        case 7: bitrate.btr.nominal.tseg1 = 0; break;
        // @sub(9): set field 'tseg1' to 257
        case 8: bitrate.btr.nominal.tseg1 = NOM_TSEG1_MAX + 1; break;
        // @sub(10): set field 'tseg1' to UINT16_MAX
        case 9: bitrate.btr.nominal.tseg1 = UINT16_MAX; break;
        // @sub(11): set field 'tseg2' to 0
        case 10: bitrate.btr.nominal.tseg2 = 0; break;
        // @sub(12): set field 'tseg2' to 129
        case 11: bitrate.btr.nominal.tseg2 = NOM_TSEG2_MAX + 1; break;
        // @sub(13): set field 'tseg2' to UINT16_MAX
        case 12: bitrate.btr.nominal.tseg2 = UINT16_MAX; break;
        // @sub(14): set field 'sjw' to 0
        case 13: bitrate.btr.nominal.sjw = 0; break;
        // @sub(15): set field 'sjw' to 129
        case 14: bitrate.btr.nominal.sjw = NOM_SJW_MAX + 1; break;
        // @sub(16): set field 'sjw' to UINT16_MAX
        case 15: bitrate.btr.nominal.sjw = UINT16_MAX; break;
        // @sub(17): set field 'data.brp' to 0
        case 16: bitrate.btr.data.brp = 0; break;
        // @sub(18): set field 'data.brp' to 1025
        case 17: bitrate.btr.data.brp = DATA_BRP_MAX + 1; break;
        // @sub(19): set field 'data.brp' to UINT16_MAX
        case 18: bitrate.btr.data.brp = UINT16_MAX; break;
        // @sub(20): set field 'data.tseg1' to 0
        case 19: bitrate.btr.data.tseg1 = 0; break;
        // @sub(21): set field 'data.tseg1' to 33
        case 20: bitrate.btr.data.tseg1 = DATA_TSEG1_MAX + 1; break;
        // @sub(22): set field 'data.tseg1' to UINT16_MAX
        case 21: bitrate.btr.data.tseg1 = UINT16_MAX; break;
        // @sub(23): set field 'data.tseg2' to 0
        case 22: bitrate.btr.data.tseg2 = 0; break;
        // @sub(24): set field 'data.tseg2' to 17
        case 23: bitrate.btr.data.tseg2 = DATA_TSEG2_MAX + 1; break;
        // @sub(25): set field 'data.tseg2' to UINT16_MAX
        case 24: bitrate.btr.data.tseg2 = UINT16_MAX; break;
        // @sub(26): set field 'data.sjw' to 0
        case 25: bitrate.btr.data.sjw = 0; break;
        // @sub(27): set field 'data.sjw' to 17
        case 26: bitrate.btr.data.sjw = DATA_SJW_MAX + 1; break;
        // @sub(28): set field 'data.sjw' to UINT16_MAX
        case 27: bitrate.btr.data.sjw = UINT16_MAX; break;
#if (FEATURE_BITRATE_FD_SAM != FEATURE_SUPPORTED)
        // @sub(29): set field 'sam' to 2 (optional)
        case 28: bitrate.btr.nominal.sam = 2; break;
        // @sub(30): set field 'sam' to UINT8_MAX (optional)
        case 29: bitrate.btr.nominal.sam = UINT8_MAX; break;
#endif
        default: return;  // Get out of here!
        }
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- try to start DUT1 with invalid index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @   note: stop/reset DUT1 if started anyway
        if (!status.can_stopped)
            (void)dut1.ResetController();
        // @- endloop
    }
    counter.Clear();
    // @post:
    // @- start DUT1 with default bit-timing index
    CLEAR_BTR(bitrate);
    BITRATE_FD_250K2M(bitrate);
    dut1.SetBitrate(bitrate);
    // dut1.ShowBitrateSettings();
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    if (!g_Options.Is3rdDevicePresent()) {
        // @  only if no other CAN device is on bus:
        // @- tell DUT2 about the changed settings
        dut2.SetOpMode(opMode);
        dut2.SetBitrate(bitrate);
        // @- send some frames to DUT2 and receive some frames from DUT2
        int32_t frames = g_Options.GetNumberOfTestFrames();
        EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
        EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        // @- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    }
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

// @gtest TC03.25: Re-Start CAN controller with same CAN FD bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithSameCanFdBitrateSettingsAfterCanStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
    // @loop over selected CAN FD bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): nominal 1Mbps (mode FDOE)
        case 0: BITRATE_FD_1M(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(2): nominal 500kbps (mode FDOE)
        case 1: BITRATE_FD_500K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(3): nominal 250kbps (mode FDOE)
        case 2: BITRATE_FD_250K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(4): nominal 125kbps (mode FDOE)
        case 3: BITRATE_FD_125K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(5): nominal 1Mbps, data phase 8Mbps (mode FDOE+BRSE)
        case 4: BITRATE_FD_1M8M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(6): nominal 500kbps, data phase 4Mbps (mode FDOE+BRSE)
        case 5: BITRATE_FD_500K4M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(7): nominal 250kbps, data phase 2Mbps (mode FDOE+BRSE)
        case 6: BITRATE_FD_250K2M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(8): nominal 125kbps, data phase 1Mbps (mode FDOE+BRSE)
        case 7: BITRATE_FD_125K1M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        default: return;  // Get out of here!
        }
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN FD operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual, opMode.brse ? true : false));
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- re-start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual, opMode.brse ? true : false));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.26: Re-Start CAN controller with different CAN FD bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(StartController, GTEST_TESTCASE(WithDifferentCanFdBitrateSettingsAfterCanStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
    // @loop over selected CAN FD bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): nominal 1Mbps (mode FDOE)
        case 0: BITRATE_FD_1M(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(2): nominal 500kbps (mode FDOE)
        case 1: BITRATE_FD_500K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(3): nominal 250kbps (mode FDOE)
        case 2: BITRATE_FD_250K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(4): nominal 125kbps (mode FDOE)
        case 3: BITRATE_FD_125K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(5): nominal 1Mbps, data phase 8Mbps (mode FDOE+BRSE)
        case 4: BITRATE_FD_1M8M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(6): nominal 500kbps, data phase 4Mbps (mode FDOE+BRSE)
        case 5: BITRATE_FD_500K4M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(7): nominal 250kbps, data phase 2Mbps (mode FDOE+BRSE)
        case 6: BITRATE_FD_250K2M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(8): nominal 125kbps, data phase 1Mbps (mode FDOE+BRSE)
        case 7: BITRATE_FD_125K1M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        default: return;  // Get out of here!
        }
        // @pre:
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- initialize DUT1 in CAN FD operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        retVal = dut1.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- start DUT1 with selected bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        CANAPI_Bitrate_t actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        CANAPI_Bitrate_t expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual, opMode.brse ? true : false));
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @test:
        // @-- change bit-timing index: 
        CLEAR_BTR(bitrate);
        switch (i) {
        // @   - sub(4): nominal 1Mbps ==> nominal 125kbps (mode FDOE)
        case 3: BITRATE_FD_1M(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @   - sub(3): nominal 500kbps ==> nominal 250kbps (mode FDOE)
        case 2: BITRATE_FD_500K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @   - sub(2): nominal 250kbps ==> nominal 500kbps (mode FDOE)
        case 1: BITRATE_FD_250K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @   - sub(1): nominal 125kbps ==> nominal 1Mbps (mode FDOE)
        case 0: BITRATE_FD_125K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @   - sub(8): nominal 1Mbps, data phase 8Mbps ==> nominal 125kbps, data phase 1Mbps (mode FDOE+BRSE)
        case 7: BITRATE_FD_1M8M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @   - sub(7): nominal 500kbps, data phase 4Mbps ==> nominal 250kbps, data phase 2Mbps (mode FDOE+BRSE)
        case 6: BITRATE_FD_500K4M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @   - sub(6): nominal 250kbps, data phase 2Mbps ==> nominal 500kbps, data phase 4Mbps (mode FDOE+BRSE)
        case 5: BITRATE_FD_250K2M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @   - sub(5): nominal 125kbps, data phase 1Mbps ==> nominal 1Mbps, data phase 8Mbps(mode FDOE+BRSE)
        case 4: BITRATE_FD_125K1M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        default: return;  // Get out of here!
        }
        // @-- re-start DUT1 with changed bit-rate settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- get actual bit-rate settings from DUT1
        actual = {};
        retVal = dut1.GetBitrate(actual);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare actual with configured bit-rate settings
        expected = dut1.GetBitrate();
        EXPECT_TRUE(CCanDevice::CompareBitrates(expected, actual, opMode.brse ? true : false));
        // @post:
        if (!g_Options.Is3rdDevicePresent()) {
            // @   only if no other CAN device is on bus:
            counter.Clear();
            // @-- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @-- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
            // @-- get status of DUT1 and check to be in RUNNING state
            retVal = dut1.GetStatus(status);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_FALSE(status.can_stopped);
        }
        // @-- stop/reset DUT1
        retVal = dut1.ResetController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- endloop
    }
    counter.Clear();
    // @end.
}

// @gtest TC03.27: Start CAN controller with CAN 2.0 bit-timing index in CAN FD operation mode (w/o bit-rate switching)
//
// @expected: CANERR_BAUDRATE
//
TEST_F(StartController, GTEST_TESTCASE(WithCanBitrateIndexInCanFdMode, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @pre:
    // @- initialize DUT1 in CAN FD operation mode
    opMode.fdoe = 1;
    opMode.brse = 0;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test: loop over selected CAN 2.0 bit-timing indexes
    // @note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @      The index must be given as negative value to 'bitrate.index'!
    // @      Remark: The CiA bit-timing table has only 9 entries!
    CCounter counter = CCounter();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): index 0 (1Mbps)
        case 0: bitrate.index = CANBTR_INDEX_1M; break;
        // @sub(2): index 2 (500kbps)
        case 1: bitrate.index = CANBTR_INDEX_500K; break;
        // @sub(3): index 3 (250kbps)
        case 2: bitrate.index = CANBTR_INDEX_250K; break;
        // @sub(4): index 4 (125kbps)
        case 3: bitrate.index = CANBTR_INDEX_125K; break;
        // @sub(5): index 5 (100kbps)
        case 4: bitrate.index = CANBTR_INDEX_100K; break;
        // @sub(6): index 6 (50kbps)
        case 5: bitrate.index = CANBTR_INDEX_50K; break;
        // @sub(7): index 7 (20kbps)
        case 6: bitrate.index = CANBTR_INDEX_20K; break;
        // @sub(8): index 8 (10kbps)
        case 7: bitrate.index = CANBTR_INDEX_10K; break;
        default: return;  // Get out of here!
        }
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- try to start DUT1 with valid index
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @   note: stop/reset DUT1 if started anyway
        if (!status.can_stopped)
            (void)dut1.ResetController();
        // @- endloop
    }
    counter.Clear();
    // @post:
    // @- start DUT1 with default bit-rate settings
    CLEAR_BTR(bitrate);
    BITRATE_FD_250K(bitrate);
    dut1.SetBitrate(bitrate);
    // dut1.ShowBitrateSettings();
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    if (!g_Options.Is3rdDevicePresent()) {
        // @  only if no other CAN device is on bus:
        // @- tell DUT2 about the changed settings
        dut2.SetOpMode(opMode);
        dut2.SetBitrate(bitrate);
        // @- send some frames to DUT2 and receive some frames from DUT2
        int32_t frames = g_Options.GetNumberOfTestFrames();
        EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
        EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        // @- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    }
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

// @gtest TC03.28: Start CAN controller with CAN 2.0 bit-rate settings in CAN FD operation mode (w/o bit-rate switching)
//
// @expected: CANERR_BAUDRATE (requires solely SJA1000 bit rates in CAN 2.0, e.g. PCAN-USB w/o FD)
//
#if (FEATURE_BITRATE_SJA1000 != FEATURE_UNSUPPORTED)
#define GTEST_SJA1000_IN_CAN_FD  GTEST_ENABLED
#else
#define GTEST_SJA1000_IN_CAN_FD  GTEST_DISABLED
#endif
TEST_F(StartController, GTEST_TESTCASE(WithCanBitrateSettingsInCanFdMode, GTEST_SJA1000_IN_CAN_FD)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @pre:
    // @- initialize DUT1 in CAN FD operation mode
    opMode.fdoe = 1;
    opMode.brse = 0;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test: loop over valid CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    counter.Increment();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): 1Mbps
        case 0: BITRATE_1M(bitrate); break;
        // @sub(2): 500kbps
        case 1: BITRATE_500K(bitrate); break;
        // @sub(3): 250kbps
        case 2: BITRATE_250K(bitrate); break;
        // @sub(4): 125kbps
        case 3: BITRATE_125K(bitrate); break;
        // @sub(5): 100kbps
        case 4: BITRATE_100K(bitrate); break;
        // @sub(6): 50kbps
        case 5: BITRATE_50K(bitrate); break;
        // @sub(7): 20kbps
        case 6: BITRATE_20K(bitrate); break;
        // @sub(8): 10kbps
        case 7: BITRATE_10K(bitrate); break;
        default: return;  // Get out of here!
        }
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- try to start DUT1 with valid settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @   note: stop/reset DUT1 if started anyway
        if (!status.can_stopped)
            (void)dut1.ResetController();
        // @- endloop
    }
    counter.Clear();
    // @post:
    // @- start DUT1 with default bit-rate settings
    CLEAR_BTR(bitrate);
    BITRATE_FD_250K(bitrate);
    dut1.SetBitrate(bitrate);
    // dut1.ShowBitrateSettings();
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    if (!g_Options.Is3rdDevicePresent()) {
        // @  only if no other CAN device is on bus:
        // @- tell DUT2 about the changed settings
        dut2.SetOpMode(opMode);
        dut2.SetBitrate(bitrate);
        // @- send some frames to DUT2 and receive some frames from DUT2
        int32_t frames = g_Options.GetNumberOfTestFrames();
        EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
        EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        // @- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    }
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

// @gtest TC03.29: Start CAN controller with CAN FD bit-rate settings in CAN 2.0 operation mode
//
// @expected: CANERR_BAUDRATE (requires solely SJA1000 bit rates in CAN 2.0, e.g. PCAN-USB w/o FD)
//
#if (FEATURE_BITRATE_SJA1000 != FEATURE_UNSUPPORTED)
#define GTEST_CAN_FD_IN_CAN_CLASSIC  GTEST_ENABLED
#else
#define GTEST_CAN_FD_IN_CAN_CLASSIC  GTEST_DISABLED
#endif
TEST_F(StartController, GTEST_TESTCASE(WithCanFdBitrateSettingsInCan20Mode, GTEST_CAN_FD_IN_CAN_CLASSIC)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_1M };
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    opMode.fdoe = 0;
    opMode.brse = 0;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test: loop over valid CAN FD bit-rate settings
    CCounter counter = CCounter();
    counter.Increment();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): nominal 1Mbps
        case 0: BITRATE_FD_1M(bitrate); break;
        // @sub(2): nominal 500kbps
        case 1: BITRATE_FD_500K(bitrate);  break;
        // @sub(3): nominal 250kbps
        case 2: BITRATE_FD_250K(bitrate); break;
        // @sub(4): nominal 125kbps
        case 3: BITRATE_FD_125K(bitrate); break;
        // @sub(5): nominal 1Mbps, data phase 8Mbps
        case 4: BITRATE_FD_1M8M(bitrate); break;
        // @sub(6): nominal 500kbps, data phase 4Mbps
        case 5: BITRATE_FD_500K4M(bitrate);  break;
        // @sub(7): nominal 250kbps, data phase 2Mbps
        case 6: BITRATE_FD_250K2M(bitrate); break;
        // @sub(8): nominal 125kbps, data phase 1Mbps
        case 7: BITRATE_FD_125K1M(bitrate); break;
        default: return;  // Get out of here!
        }
        // printf("[   SUB%-3i ] ...\n", (i + 1));
        // @-- try to start DUT1 with valid settings
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
        retVal = dut1.StartController();
        EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @   note: stop/reset DUT1 if started anyway
        if (!status.can_stopped)
            (void)dut1.ResetController();
        // @- endloop
    }
    counter.Clear();
    // @post:
    // @- start DUT1 with default bit-rate settings
    CLEAR_BTR(bitrate);
    BITRATE_250K(bitrate);
    dut1.SetBitrate(bitrate);
    // dut1.ShowBitrateSettings();
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    if (!g_Options.Is3rdDevicePresent()) {
        // @  only if no other CAN device is on bus:
        // @- tell DUT2 about the changed settings
        dut2.SetOpMode(opMode);
        dut2.SetBitrate(bitrate);
        // @- send some frames to DUT2 and receive some frames from DUT2
        int32_t frames = g_Options.GetNumberOfTestFrames();
        EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
        EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        // @- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    }
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
#endif  // (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)

//  $Id: TC03_StartController.cc 1204 2023-09-24 15:26:57Z makemake $  Copyright (c) UV Software, Berlin.
