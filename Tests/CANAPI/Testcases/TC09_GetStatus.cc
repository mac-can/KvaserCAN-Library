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
#ifndef FEATURE_ERROR_CODE_CAPTURE
#define FEATURE_ERROR_CODE_CAPTURE  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_ERROR_CODE_CAPTURE not set, default = FEATURE_SUPPORTED" )
#else
#warning FEATURE_ERROR_CODE_CAPTURE not set, default = FEATURE_SUPPORTED
#endif
#endif
#define TC09_8_DEBUG   0
#define TC09_9_DEBUG   0
#define TC09_10_DEBUG  0

class GetStatus : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC09.0: Get CAN controller status (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(GetStatus, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
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
    // @test:
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
    // @post:
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC09.1: Get CAN controller status with invalid interface handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC09.2: Give a NULL pointer as argument for parameter 'status'
//
// @note: passing a pointer for 'status' is not possible with the C++ API!

// @gtest TC09.3: Get CAN controller status if CAN channel is not initialized
//
// @expected: CANERR_NOTINIT
//
TEST_F(GetStatus, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @test:
    retVal = dut1.GetStatus(status);
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

// @gtest TC09.4: Get CAN controller status if CAN controller is not started
//
// @expected: CANERR_NOERROR and status bit 'can_stopped' is set
//
// @disabled: This test is part of the default scenario and already tested many times!
//
TEST_F(GetStatus, GTEST_TESTCASE(IfChannelInitialized, GTEST_DISABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @test:
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
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

// @gtest TC09.5: Get CAN controller status if CAN controller is started
//
// @expected: CANERR_NOERROR and status bit 'can_stopped' is not set
//
// @disabled: This test is part of the default scenario and already tested many times!
//
TEST_F(GetStatus, GTEST_TESTCASE(IfControllerStarted, GTEST_DISABLED)) {
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
    // @test:
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
    // @post:
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

// @gtest TC09.6: Get CAN controller status if CAN controller was previously stopped
//
// @expected: CANERR_NOERROR and status bit 'can_stopped' is set
//
// @disabled: This test is part of the default scenario and already tested many times!
//
TEST_F(GetStatus, GTEST_TESTCASE(IfControllerStopped, GTEST_DISABLED)) {
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
    // @test:
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

// @gtest TC09.7: Get CAN controller status if CAN channel was previously torn down
//
// @expected: CANERR_NOTINIT
//
TEST_F(GetStatus, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
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
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @end.
}

// @gtest TC09.8: Get CAN controller status if in bus off state
//
// @expected: CANERR_NOERROR but status bit 'bus_off' is set
//
TEST_F(GetStatus, GTEST_TESTCASE(IfInBusOffState, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t newBtr1 = {}, oldBtr1 = {};
    CANAPI_Bitrate_t newBtr2 = {}, oldBtr2 = {};
    CANAPI_Message_t trmMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x008U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
#if (OPTION_CAN_2_0_ONLY != 0)
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CAN_MAX_LEN);
#else
    trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
    trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
    trmMsg.esi = 0;
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
#endif
#if (TC09_8_ISSUE_BUS_OFF == WORKAROUND_ENABLED)
    ASSERT_TRUE(false) << "[  TC09.8  ] No bus-off state from device!";
#endif
    // @
    // @note: This test cannot run if there is another device on bus!
    if (g_Options.Is3rdDevicePresent())
        GTEST_SKIP() << "This test cannot run if there is another device on bus!";
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- change bit-rate settings: DUT1 w/ slow bit-rate
    SLOW_BITRATE(newBtr1);
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    if (dut1.GetOpMode().fdoe) SLOW_BITRATE_FD(newBtr1);
    // @  note: w/ BRSE if DUT1 is CAN FD capable
#endif
    oldBtr1 = dut1.GetBitrate();
    dut1.SetBitrate(newBtr1);
    // dut1.ShowBitrateSettings("[   DUT1   ]");
    // @- start DUT1 with changes bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- initialize DUT2 with configured settings
    retVal = dut2.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- change bit-rate settings: DUT2 w/ fast bit-rate
    FAST_BITRATE(newBtr2);
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    if (dut2.GetOpMode().fdoe) FAST_BITRATE_FD(newBtr2);
    // @  note: w/ BRSE if DUT2 is CAN FD capable
#endif
    oldBtr2 = dut2.GetBitrate();
    dut2.SetBitrate(newBtr2);
    // dut2.ShowBitrateSettings("[   DUT2   ]");
    // @- start DUT2 with changed bit-rate settings
    retVal = dut2.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @test:
    int32_t i = 0;
    // @- repeat until warning level reached (status bit 'bus_off' set)
    CProgress progress = CProgress(TEST_MAX_BOFF);
    do {
        // @-- DUT1 send a message (with wrong bit-rate)
        (void)dut1.WriteMessage(trmMsg, TEST_WRITE_TIMEOUT);
        progress.Update(i++, 0);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
        // @-- if DUT1 reaches warning level:
        if (status.warning_level) {
            // @-- DUT2 send a message (to enter buss off state)
            (void)dut2.WriteMessage(trmMsg, TEST_WRITE_TIMEOUT);
        }
    } while ((i < TEST_MAX_BOFF) && !status.bus_off);
    EXPECT_TRUE(status.bus_off);
    // @post:
    progress.Clear();
#if (TC09_8_DEBUG != 0)
    std::cout << "[   BOFF   ] State" << (status.bus_off? " " : " not ") << "reached after " << i << " frame(s)" << std::endl;
#endif
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- stop/reset DUT2
    retVal = dut2.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- restore configure bit-rate settings
    dut1.SetBitrate(oldBtr1);
    dut2.SetBitrate(oldBtr2);
    // dut1.ShowBitrateSettings("[   DUT1   ]");
    // dut2.ShowBitrateSettings("[   DUT2   ]");
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
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC09.9: Get CAN controller status if warning level reached
//
// @expected: CANERR_NOERROR but status bit 'warning_level' is set
//
TEST_F(GetStatus, GTEST_TESTCASE(IfWarningLevelReached, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t newBtr1 = {}, oldBtr1 = {};
    CANAPI_Bitrate_t newBtr2 = {}, oldBtr2 = {};
    CANAPI_Message_t trmMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x009U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
#if (OPTION_CAN_2_0_ONLY != 0)
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CAN_MAX_LEN);
#else
    trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
    trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
    trmMsg.esi = 0;
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
#endif
#if (TC09_9_ISSUE_PCBUSB_WARNING_LEVEL == WORKAROUND_ENABLED)
    ASSERT_TRUE(false) << "[  TC09.9  ] No warning level from device!";
#endif
    // @
    // @note: This test cannot run if there is another device on bus!
    if (g_Options.Is3rdDevicePresent())
        GTEST_SKIP() << "This test cannot run if there is another device on bus!";
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- change bit-rate settings: DUT1 w/ slow bit-rate
    SLOW_BITRATE(newBtr1);
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    if (dut1.GetOpMode().fdoe) SLOW_BITRATE_FD(newBtr1);
    // @  note: w/ BRSE if DUT1 is CAN FD capable
#endif
    oldBtr1 = dut1.GetBitrate();
    dut1.SetBitrate(newBtr1);
    // dut1.ShowBitrateSettings("[   DUT1   ]");
    // @- start DUT1 with changes bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- initialize DUT2 with configured settings
    retVal = dut2.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- change bit-rate settings: DUT2 w/ fast bit-rate
    FAST_BITRATE(newBtr2);
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    if (dut2.GetOpMode().fdoe) FAST_BITRATE_FD(newBtr2);
    // @  note: w/ BRSE if DUT2 is CAN FD capable
#endif
    oldBtr2 = dut2.GetBitrate();
    dut2.SetBitrate(newBtr2);
    // dut2.ShowBitrateSettings("[   DUT2   ]");
    // @- start DUT2 with changed bit-rate settings
    retVal = dut2.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @test:
    int32_t i = 0;
    // @- repeat until warning level reached (status bit 'warning_level' set)
    CProgress progress = CProgress(TEST_MAX_EWRN);
    do {
        // @-- DUT1 send a message (with wrong bit-rate)
        (void)dut1.WriteMessage(trmMsg, TEST_WRITE_TIMEOUT);
        progress.Update(i++, 0);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    } while ((i < TEST_MAX_EWRN) && !status.warning_level && !status.bus_off);
    EXPECT_TRUE(status.warning_level);
    // @post:
    progress.Clear();
#if (TC09_9_DEBUG != 0)
    std::cout << "[   EWRN   ] State" << (status.warning_level ? " " : " not ") << "reached after " << i << " frame(s)" << std::endl;
#endif
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- stop/reset DUT2
    retVal = dut2.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- restore configure bit-rate settings
    dut1.SetBitrate(oldBtr1);
    dut2.SetBitrate(oldBtr2);
    // dut1.ShowBitrateSettings("[   DUT1   ]");
    // dut2.ShowBitrateSettings("[   DUT2   ]");
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
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC09.10: Get CAN controller status if errors on bus (LEC)
//
// @expected: CANERR_NOERROR but status bit 'bus_error' is set
//
#if (FEATURE_ERROR_CODE_CAPTURE != FEATURE_UNSUPPORTED)
#define GTEST_TC09_10_ENABLED  GTEST_ENABLED
#else
#define GTEST_TC09_10_ENABLED  GTEST_DISABLED
#endif
TEST_F(GetStatus, GTEST_TESTCASE(IfErrorsOnBus, GTEST_TC09_10_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t newBtr1 = {}, oldBtr1 = {};
    CANAPI_Bitrate_t newBtr2 = {}, oldBtr2 = {};
    CANAPI_OpMode_t  opCapa = {}, opMode = {};
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x00AU;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
#if (OPTION_CAN_2_0_ONLY != 0)
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CAN_MAX_LEN);
#else
    trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
    trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
    trmMsg.esi = 0;
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
#endif
    // @
    // @note: This test cannot run if there is another device on bus!
    if (g_Options.Is3rdDevicePresent())
        GTEST_SKIP() << "This test cannot run if there is another device on bus!";
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get operation capabilities from DUT1
    retVal = dut1.GetOpCapabilities(opCapa);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- tear down DUT1 again
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @! exit test if error frames are not supported
    if (!opCapa.err)
        GTEST_SKIP() << "Test skipped: error frames are not supported by the device!";
    // @- enable error frame reception for DUT1
    opMode = dut1.GetOpMode();
    opMode.err = 1;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode("[   MODE   ]");
    // @- initialize DUT1 with error frames enabled
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- change bit-rate settings: DUT1 w/ slow bit-rate
    SLOW_BITRATE(newBtr1);
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    if (dut1.GetOpMode().fdoe) SLOW_BITRATE_FD(newBtr1);
    // @  note: w/ BRSE if DUT1 is CAN FD capable
#endif
    oldBtr1 = dut1.GetBitrate();
    dut1.SetBitrate(newBtr1);
    // dut1.ShowBitrateSettings("[   DUT1   ]");
    // @- start DUT1 with changes bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- initialize DUT2 with configured settings
    retVal = dut2.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- change bit-rate settings: DUT2 w/ fast bit-rate
    FAST_BITRATE(newBtr2);
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    if (dut2.GetOpMode().fdoe) FAST_BITRATE_FD(newBtr2);
    // @  note: w/ BRSE if DUT2 is CAN FD capable
#endif
    oldBtr2 = dut2.GetBitrate();
    dut2.SetBitrate(newBtr2);
    // dut2.ShowBitrateSettings("[   DUT2   ]");
    // @- start DUT2 with changed bit-rate settings
    retVal = dut2.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @test:
    int32_t i = 0;
    // @- repeat until errors on bus (status bit 'bus_error' set)
    CProgress progress = CProgress(TEST_MAX_BERR);
    do {
        // @-- DUT1 send a message (with wrong bit-rate)
        (void)dut1.WriteMessage(trmMsg, TEST_WRITE_TIMEOUT);
        progress.Update(i++, 0);
        // @-- DUT1 read recieve queue to catch error frames
        (void)dut1.ReadMessage(rcvMsg, 0U);
        // @-- get status of DUT1 and check to be in RUNNING state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_FALSE(status.can_stopped);
    } while ((i < TEST_MAX_BERR) && !status.bus_error && !status.bus_off);
    EXPECT_TRUE(status.bus_error);
    // @post:
    progress.Clear();
#if (TC09_10_DEBUG != 0)
    std::cout <<  "[   BERR   ] State" << (status.bus_error ? " " : " not ") << "reached after " << i << " frame(s)" << std::endl;
#endif
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- stop/reset DUT2
    retVal = dut2.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- restore configure bit-rate settings
    dut1.SetBitrate(oldBtr1);
    dut2.SetBitrate(oldBtr2);
    // dut1.ShowBitrateSettings("[   DUT1   ]");
    // dut2.ShowBitrateSettings("[   DUT2   ]");
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
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC09.11: Get CAN controller status if transmitter is busy
//
// @expected: CANERR_TX_BUSY and status bit 'transmitter_busy' is set
//
// @disabled: This test is already covered by TC05.19 (WriteMessage.IfTransmitterBusy)!
//
#if (FEATURE_SIZE_TRANSMIT_QUEUE != 0)
#define GTEST_TC09_11_ENABLED  GTEST_DISABLED
#else
#define GTEST_TC09_11_ENABLED  GTEST_DISABLED
#endif
TEST_F(GetStatus, GTEST_TESTCASE(IfTransmitterBusy, GTEST_TC09_11_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x513U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
#if (OPTION_CAN_2_0_ONLY != 0)
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CAN_MAX_LEN);
#else
    trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
    trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
    trmMsg.esi = 0;
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
#endif
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
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
    // @- initialize DUT2 with configured settings
    retVal = dut2.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT2 with configured bit-rate settings
    retVal = dut2.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @test:
    int32_t i = 0, n = (FEATURE_SIZE_TRANSMIT_QUEUE * 2);
    CProgress progress = CProgress(n);
    // @- DUT1 send out messages until transmit queue full
    do {
        retVal = dut1.WriteMessage(trmMsg);
        if (retVal == CCanApi::NoError) {
            progress.Update(i++, 0);
            // up-counting message content
            trmMsg.data[0] = (uint8_t)((uint64_t)i >> 0); if ((uint64_t)i > (uint64_t)0x00) trmMsg.dlc = 1U;
            trmMsg.data[1] = (uint8_t)((uint64_t)i >> 8); if ((uint64_t)i > (uint64_t)0x00FF) trmMsg.dlc = 2U;
            trmMsg.data[2] = (uint8_t)((uint64_t)i >> 16); if ((uint64_t)i > (uint64_t)0x0FFFF) trmMsg.dlc = 3U;
            trmMsg.data[3] = (uint8_t)((uint64_t)i >> 24); if ((uint64_t)i > (uint64_t)0x0FFFFFF) trmMsg.dlc = 4U;
            trmMsg.data[4] = (uint8_t)((uint64_t)i >> 32); if ((uint64_t)i > (uint64_t)0x0FFFFFFFF) trmMsg.dlc = 5U;
            trmMsg.data[5] = (uint8_t)((uint64_t)i >> 40); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFF) trmMsg.dlc = 6U;
            trmMsg.data[6] = (uint8_t)((uint64_t)i >> 48); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFFFF) trmMsg.dlc = 7U;
            trmMsg.data[7] = (uint8_t)((uint64_t)i >> 56); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFFFFFF) trmMsg.dlc = 8U;
        }
    } while ((retVal != CCanApi::TransmitterBusy) && (i < n));
    EXPECT_EQ(CCanApi::TransmitterBusy, retVal);
    // @- get status of DUT1 and check if bit 'transmitter_busy' is set
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.transmitter_busy);
    // @- DUT2 read them all to empty the reception queue
    n = i;
    i = 0;
    CTimer timer = CTimer(((dut2.TransmissionTime(dut2.GetBitrate(), n) * (uint64_t)DEVICE_LOOP_FACTOR) / (uint64_t)DEVICE_LOOP_DIVISOR));
    do {
        // read message by message (with time-out)
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        if (retVal == CCanApi::NoError) {
            // ignore status messages/error frames
            if (!rcvMsg.sts)
                progress.Update(n, i++);
        }
    } while ((i < n) && !timer.Timeout());
    EXPECT_EQ(n, i);
    // @post:
    progress.Clear();
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
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC09.12: Get CAN controller status if receive queue is empty
//
// @expected: CANERR_RX_EMPTY and status bit 'receiver_empty' is set 
//
// @disabled: This test is already covered by TC04.7 (ReadMessage.IfReceiveQueueEmpty)!
//
TEST_F(GetStatus, GTEST_TESTCASE(IfReceiveQueueEmpty, GTEST_DISABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x407U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
#if (OPTION_CAN_2_0_ONLY != 0)
    trmMsg.dlc = CAN_MAX_DLC;
    memset(trmMsg.data, 0, CAN_MAX_LEN);
#else
    trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
    trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
    trmMsg.esi = 0;
    trmMsg.dlc = g_Options.GetOpMode(DUT1).fdoe ? CANFD_MAX_DLC : CAN_MAX_DLC;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
#endif
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
    // @- initialize DUT2 with configured settings
    retVal = dut2.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT2 with configured bit-rate settings
    retVal = dut2.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- DUT2 send out one message
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- DUT1 read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare sent and received message
    EXPECT_TRUE(dut1.CompareMessages(trmMsg, rcvMsg));
    // @test:
    CCounter counter = CCounter(true);
    // @- get status of DUT1 and check if bit 'receiver_empty' is reset
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.receiver_empty);
    // @- DUT1 try to read a message from empty queue (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- get status of DUT1 and check if bit 'receiver_empty' is set
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.receiver_empty);
    // @post:
    counter.Clear();
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
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC09.13: Get CAN controller status after message lost
//
// @expected: CANERR_
//
TEST_F(GetStatus, GTEST_TESTCASE(IfMessageLost, GTEST_DISABLED)) {
    // @unsolved: how to get a message lost?
    // @end.
}

// @gtest TC09.14: Get CAN controller status after receive queue overrun
//
// @expected: CANERR_NOERROR but status bit 'queue_overrun' set
//
// @disabled: This test is already covered by TC04.8 (ReadMessage.IfReceiveQueueFull)!
//
#if (FEATURE_SIZE_RECEIVE_QUEUE != 0)
#define GTEST_TC09_14_ENABLED  GTEST_DISABLED
#else
#define GTEST_TC09_14_ENABLED  GTEST_DISABLED
#endif
TEST_F(GetStatus, GTEST_TESTCASE(IfReceiveQueueFull, GTEST_TC09_14_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x408U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
#if (OPTION_CAN_2_0_ONLY != 0)
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CAN_MAX_LEN);
#else
    trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
    trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
    trmMsg.esi = 0;
    trmMsg.dlc = 0;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
#endif
#if (TC04_8_DEBUG != 0)
    struct timespec t0 = {}, t1 = {};
    struct timespec m0 = {}, m1 = {};
#endif
    // @
    // @note: This test can take a very long time
    if (g_Options.RunQuick())
        GTEST_SKIP() << "This test can take a very long time!";
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
    // @- initialize DUT2 with configured settings
    retVal = dut2.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT2 and check to be in INIT state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT2 with configured bit-rate settings
    retVal = dut2.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
#if (TC04_8_DEBUG != 0)
    // get current time: start of transmission
    t0 = CTimer::GetTime();
#endif
    // @test:
    int32_t spam = (FEATURE_SIZE_RECEIVE_QUEUE + TEST_QRCVFULL);
#if (TC04_8_ISSUE_QUEUE_SIZE == WORKAROUND_ENABLED)
    // @- issue(PCBUSB.TOS): last element of the receive queue is not accessible
    spam -= 1;
#endif
    CProgress progress = CProgress(spam);
    // @- DUT2 spam the receive queue of DUT1 (with one message more than the queue can hold)
    for (int32_t i = 0; i < spam; i++) {
        // up-counting message content
        trmMsg.data[0] = (uint8_t)((uint64_t)i >> 0); if ((uint64_t)i > (uint64_t)0x00) trmMsg.dlc = 1U;
        trmMsg.data[1] = (uint8_t)((uint64_t)i >> 8); if ((uint64_t)i > (uint64_t)0x00FF) trmMsg.dlc = 2U;
        trmMsg.data[2] = (uint8_t)((uint64_t)i >> 16); if ((uint64_t)i > (uint64_t)0x0FFFF) trmMsg.dlc = 3U;
        trmMsg.data[3] = (uint8_t)((uint64_t)i >> 24); if ((uint64_t)i > (uint64_t)0x0FFFFFF) trmMsg.dlc = 4U;
        trmMsg.data[4] = (uint8_t)((uint64_t)i >> 32); if ((uint64_t)i > (uint64_t)0x0FFFFFFFF) trmMsg.dlc = 5U;
        trmMsg.data[5] = (uint8_t)((uint64_t)i >> 40); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFF) trmMsg.dlc = 6U;
        trmMsg.data[6] = (uint8_t)((uint64_t)i >> 48); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFFFF) trmMsg.dlc = 7U;
        trmMsg.data[7] = (uint8_t)((uint64_t)i >> 56); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFFFFFF) trmMsg.dlc = 8U;
        // send one message (w/ delay calculated from bit-rate and data length)
        do {
            retVal = dut2.WriteMessage(trmMsg, 0U);
            if (retVal == CCanApi::TransmitterBusy)
                PCBUSB_QXMT_DELAY();
        } while (retVal == CCanApi::TransmitterBusy);
        // wait for transmission to complete
        uint64_t delay = dut2.TransmissionTime(dut2.GetBitrate(), 1, CCanApi::Dlc2Len(trmMsg.dlc));
#if (TC04_8_ISSUE_PCBUSB_TRANSMIT_COMPLETE == WORKAROUND_ENABLED)
        // @- issue(PCBUSB.TNG): PCAN-USB devices need more time as estimated
        CANAPI_OpMode_t opCapa = { CANMODE_DEFAULT };
        retVal = dut1.GetOpCapabilities(opCapa);
        if ((CCanApi::NoError == retVal) && !opCapa.fdoe)
            delay *= 2U;
#endif
        CTimer::Delay(delay);
        // on error abort
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.WriteMessage() failed with error code " << retVal;
        progress.Update(i + 1, 0);
    }
#if (TC04_8_DEBUG != 0)
    // get current time: end of transmission
    t1 = CTimer::GetTime();;
#endif
    // @- an additional delay to ensure that the last message is received by DUT1
    uint64_t delay = dut2.TransmissionTime(dut2.GetBitrate(), 10, CCanApi::Dlc2Len(trmMsg.dlc));
    CTimer::Delay(delay);
    // @- DUT1 read them all to empty the reception queue
    CTimer timer = CTimer(((dut1.TransmissionTime(dut1.GetBitrate(), (spam + DEVICE_LOOP_EXTRA)) *
        (uint64_t)DEVICE_LOOP_FACTOR) / (uint64_t)DEVICE_LOOP_DIVISOR));  // bit-rate dependent timeout
    int32_t rcv = 0, sts = 0;
    do {
        // read message by message (with time-out)
        retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        if (retVal == CCanApi::NoError) {
#if (TC04_8_DEBUG != 0)
            if (rcv == 0)  // first message
                m0 = rcvMsg.timestamp;
            m1 = rcvMsg.timestamp;
#endif
            // count status messages/error frames separately
            if (!rcvMsg.sts)
                progress.Update(spam, rcv++);
            else
                sts++;
        }
    } while ((retVal == CCanApi::NoError) && !timer.Timeout());
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    EXPECT_EQ((spam - TEST_QRCVFULL), (rcv + sts));
#if (1)
    // @- DUT2 send / DUT1 read one more message to catch the overrun flag
    trmMsg.dlc = 5U;
    trmMsg.data[0] = (uint8_t)'E';
    trmMsg.data[1] = (uint8_t)'x';
    trmMsg.data[2] = (uint8_t)'t';
    trmMsg.data[3] = (uint8_t)'r';
    trmMsg.data[4] = (uint8_t)'a';
    timer.Restart((uint64_t)((TEST_READ_TIMEOUT * DEVICE_LOOP_FACTOR) / DEVICE_LOOP_DIVISOR) * CTimer::MSEC);
    do {
        retVal = dut2.WriteMessage(trmMsg, 0U);
        if (retVal == CCanApi::TransmitterBusy)
            PCBUSB_QXMT_DELAY();
    } while ((retVal == CCanApi::TransmitterBusy) && !timer.Timeout());
    do {
        // read message by message (with time-out)
        retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    } while ((retVal == CCanApi::ReceiverEmpty) && !timer.Timeout());
    // @- todo: check why this extra message is required
#endif
    // @- get status of DUT1 and check if bit 'queue_overrun' is set
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.queue_overrun);
    // @post:
    progress.Clear();
#if (TC04_8_DEBUG != 0)
    uint64_t ovfl = 0U;
    if ((dut1.GetProperty(CANPROP_GET_RCV_QUEUE_OVFL, (void*)&ovfl, sizeof(ovfl)) == CCanApi::NoError) ||
        (dut1.GetProperty(CANPROP_GET_VENDOR_PROP + 0x84U, (void*)&ovfl, sizeof(ovfl)) == CCanApi::NoError))
        std::cout << "[   RCVQ   ] ov=" << ovfl << std::endl;
    dut2.ShowTimeDifference("[   SEND   ]", t0, t1);
    std::cout << "[          ]  + " << ((float)delay / 1000.f) << "ms" << std::endl;
    dut1.ShowTimeDifference("[   READ   ]", m0, m1);
    std::cout << "[   DIFF   ] dt=" << ((float)(dut1.TimeDifference(t0, t1) - dut2.TimeDifference(m0, m1)) / 1000.f) << "ms" << std::endl;
#endif
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
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

//  $Id: TC09_GetStatus.cc 1218 2023-10-14 12:18:19Z makemake $  Copyright (c) UV Software, Berlin.
