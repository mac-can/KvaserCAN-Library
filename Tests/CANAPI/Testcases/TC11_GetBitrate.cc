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

#define CLEAR_BTR(btr)  memset(&btr, 0, sizeof(CANAPI_Bitrate_t))
#define CLEAR_BPS(bps)  memset(&bps, 0, sizeof(CANAPI_BusSpeed_t))

class GetBitrate : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC11.0: Get CAN bit-rate settings (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(GetBitrate, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
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
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @test:
    // @- get bit-rate settings from DUT1
    retVal = dut1.GetBitrate(bitrate);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get transmission rate from DUT1
    retVal = dut1.GetBusSpeed(speed);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare configured and actual bit-rate settings
    // dut1.ShowBitrateSettings();
#if (OPTION_CAN_2_0_ONLY != 0)
    EXPECT_TRUE(CCanDevice::CompareBitrates(dut1.GetBitrate(), bitrate, false));
#else
    EXPECT_TRUE(CCanDevice::CompareBitrates(dut1.GetBitrate(), bitrate, dut1.GetOpMode().brse));
#endif
    // @post
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
    // @- end
}

// @gtest TC11.1: Get CAN bit-rate settings with invalid interface handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC11.2: Give a NULL pointer as argument for parameter 'bitrate'
//
// @note: passing a pointer for 'bitrate' is not possible with the C++ API!

// @gtest TC11.3: Give a NULL pointer as argument for parameter 'speed'
//
// @note: passing a pointer for 'speed' is not possible with the C++ API!

// @gtest TC11.4: Give a NULL pointer as argument for parameter 'bitrate' and 'speed'
//
// @note: passing a pointer for 'bitrate' and 'speed' is not possible with the C++ API!

// @gtest TC11.5: Get CAN bit-rate settings if CAN channel is not initialized
//
// @expected: CANERR_NOTINIT
//
TEST_F(GetBitrate, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @test:
    // @- get bit-rate settings from DUT1
    retVal = dut1.GetBitrate(bitrate);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @- get transmission rate from DUT1
    retVal = dut1.GetBusSpeed(speed);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @post
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

// @gtest TC11.6: Get CAN bit-rate settings if CAN controller is not started
//
// @expected: CANERR_NOERROR
// @todo: check CAN API V3!
//
TEST_F(GetBitrate, GTEST_TESTCASE(IfChannelInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
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
    // @test:
    // @- get bit-rate settings from DUT1
    retVal = dut1.GetBitrate(bitrate);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get transmission rate from DUT1
    retVal = dut1.GetBusSpeed(speed);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post
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

// @gtest TC11.7: Get CAN bit-rate settings if CAN controller is started
//
// @expected: CANERR_NOERROR
//
TEST_F(GetBitrate, GTEST_TESTCASE(IfControllerStarted, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
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
    // @- get bit-rate settings from DUT1
    retVal = dut1.GetBitrate(bitrate);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get transmission rate from DUT1
    retVal = dut1.GetBusSpeed(speed);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @post
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

// @gtest TC11.8: Get CAN bit-rate settings if CAN controller is stopped
//
// @expected: CANERR_NOERROR
// @todo: check CAN API V3!
//
TEST_F(GetBitrate, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
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
    // @test:
    // @- get bit-rate settings from DUT1
    retVal = dut1.GetBitrate(bitrate);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get transmission rate from DUT1
    retVal = dut1.GetBusSpeed(speed);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC11.9: Get CAN bit-rate settings if CAN channel is torn down
//
// @expected: CANERR_NOTINIT
//
TEST_F(GetBitrate, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
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
    // @- get bit-rate settings from DUT1
    retVal = dut1.GetBitrate(bitrate);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @- get transmission rate from DUT1
    retVal = dut1.GetBusSpeed(speed);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @end.
}

// @gtest TC11.10: Get CAN bit-rate settings if running with various CAN 2.0 bit-rate settings and check for correctness
//
// @expected: CANERR_NOERROR
//
TEST_F(GetBitrate, GTEST_TESTCASE(WithVariousCanBitrateSettings, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
    CANAPI_OpMode_t opMode = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @loop over valid CAN FD bit-rate settings
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
#if (TC11_10_ISSUE_RUSOKU_BITRATE_10K != WORKAROUND_DISABLED)
        // @! issue(MacCAN-TouCAN): 10kbps hardware bug (known issue)
        if (i == 7)
            continue;
#endif
        // @pre:
        // @- initialize DUT1 in CAN 2.0 operation mode
#if (OPTION_CAN_2_0_ONLY == 0)
        opMode.fdoe = 0;
        opMode.brse = 0;
#endif
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
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
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        // @- get bit-rate settings from DUT1
        retVal = dut1.GetBitrate(bitrate);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- get transmission rate from DUT1
        retVal = dut1.GetBusSpeed(speed);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- compare configured and actual bit-rate settings
        // dut1.ShowBitrateSettings();
#if (OPTION_CAN_2_0_ONLY != 0)
        EXPECT_TRUE(CCanDevice::CompareBitrates(dut1.GetBitrate(), bitrate, false));
#else
        EXPECT_TRUE(CCanDevice::CompareBitrates(dut1.GetBitrate(), bitrate, dut1.GetOpMode().brse));
#endif
        // @post
        if (!g_Options.Is3rdDevicePresent()) {
            counter.Clear();
            // @  only if no other CAN device is on bus:
            // @- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        }
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
        // @- endloop
    }
    counter.Clear();
    // @end.
}

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @gtest TC11.11: Get CAN bit-rate settings if running with various CAN FD bit-rate settings and check for correctness
//
// @expected: CANERR_NOERROR
//
TEST_F(GetBitrate, GTEST_TESTCASE(WithVariousCanFdBitrateSettings, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
    CANAPI_OpMode_t opMode = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @loop over valid CAN FD bit-rate settings
    CCounter counter = CCounter();
    counter.Increment();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        switch (i) {
        // @sub(1): nominal 1Mbps
        case 0: BITRATE_FD_1M(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(2): nominal 500kbps
        case 1: BITRATE_FD_500K(bitrate);  opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(3): nominal 250kbps
        case 2: BITRATE_FD_250K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(4): nominal 125kbps
        case 3: BITRATE_FD_125K(bitrate); opMode.fdoe = 1; opMode.brse = 0; break;
        // @sub(5): nominal 1Mbps, data phase 8Mbps
        case 4: BITRATE_FD_1M8M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(6): nominal 500kbps, data phase 4Mbps
        case 5: BITRATE_FD_500K4M(bitrate);  opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(7): nominal 250kbps, data phase 2Mbps
        case 6: BITRATE_FD_250K2M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        // @sub(8): nominal 125kbps, data phase 1Mbps
        case 7: BITRATE_FD_125K1M(bitrate); opMode.fdoe = 1; opMode.brse = 1; break;
        default: return;  // Get out of here!
        }
        // @pre:
        // @- initialize DUT1 in CAN FD operation mode
        dut1.SetOpMode(opMode);
        // dut1.ShowOperationMode();
        dut1.SetBitrate(bitrate);
        // dut1.ShowBitrateSettings();
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
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        // @- get bit-rate settings from DUT1
        retVal = dut1.GetBitrate(bitrate);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- get transmission rate from DUT1
        retVal = dut1.GetBusSpeed(speed);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- compare configured and actual bit-rate settings
        // dut1.ShowBitrateSettings();
        EXPECT_TRUE(CCanDevice::CompareBitrates(dut1.GetBitrate(), bitrate, dut1.GetOpMode().brse));
        // @post
        if (!g_Options.Is3rdDevicePresent()) {
            counter.Clear();
            // @  only if no other CAN device is on bus:
            // @- tell DUT2 about the changed settings
            dut2.SetOpMode(opMode);
            dut2.SetBitrate(bitrate);
            // @- send some frames to DUT2 and receive some frames from DUT2
            int32_t frames = g_Options.GetNumberOfTestFrames();
            EXPECT_EQ(frames, dut1.SendSomeFrames(dut2, frames));
            EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
        }
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
        // @- endloop
    }
    counter.Clear();
    // @end.
}
#endif  // (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)

//  $Id: TC11_GetBitrate.cc 1204 2023-09-24 15:26:57Z makemake $  Copyright (c) UV Software, Berlin.
