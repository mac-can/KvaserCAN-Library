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

class SmokeTest : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

TEST_F(SmokeTest, GTEST_TESTCASE(CheckGoogleTest, GTEST_DISABLED)) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

TEST_F(SmokeTest, GTEST_TESTCASE(CheckTimer, GTEST_DISABLED)) {
    time_t now = time(NULL);
    CCounter counter = CCounter(true);
    CTimer timer((uint64_t)2 * CTimer::SEC);
    counter.Increment();
    while (!timer.Timeout());
    EXPECT_GE(time(NULL), now + 1);
    counter.Increment();
    CTimer::Delay((uint64_t)1 * CTimer::SEC);
    EXPECT_GE(time(NULL), now + 2);

    counter.Reset(true);
    struct timespec t0 = {}, t1 = {};
    t0 = CTimer::GetTime();
    uint64_t delay = 100U * CTimer::MSEC;
    uint64_t times = 10U;
    for (uint64_t dt = 0U; dt < (times * delay); dt += delay) {
        counter.Increment();
        CTimer::Delay(delay);
        t1 = CTimer::GetTime();
    }
    EXPECT_GE((double)(times * delay), CTimer::DiffTime(t0, t1));
    counter.Clear();
}

TEST_F(SmokeTest, GTEST_TESTCASE(CheckDataLengthCode, GTEST_DISABLED)) {
    uint8_t dlc, len;
    CCounter counter = CCounter();
    dlc = 0U; len = 0U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 1U; len = 1U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 2U; len = 2U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 3U; len = 3U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 4U; len = 4U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 5U; len = 5U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 6U; len = 6U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 7U; len = 7U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 8U; len = 8U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    counter.Increment();
#if (OPTION_CAN_2_0_ONLY == 0)
    dlc = 0x9U; len = 12U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(9U));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(11U));
    counter.Increment();
    dlc = 0xAU; len = 16U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(13U));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(15U));
    counter.Increment();
    dlc = 0xBU; len = 20U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(17U));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(19U));
    counter.Increment();
    dlc = 0xCU; len = 24U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(21U));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(23U));
    counter.Increment();
    dlc = 0xDU; len = 32U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(25U));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(31U));
    counter.Increment();
    dlc = 0xEU; len = 48U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(33U));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(47U));
    counter.Increment();
    dlc = 0xFU; len = 64U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(len));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(49U));
    EXPECT_EQ(dlc, CCanApi::Len2Dlc(63U));
    counter.Increment();
    dlc = 0x10U; len = 64U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(15U, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 0xFFU; len = 64U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(15U, CCanApi::Len2Dlc(len));
#else
    dlc = 9U; len = 8U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(8U, CCanApi::Len2Dlc(len));
    counter.Increment();
    dlc = 0xFFU; len = 8U;
    EXPECT_EQ(len, CCanApi::Dlc2Len(dlc));
    EXPECT_EQ(8U, CCanApi::Len2Dlc(len));
#endif
    counter.Clear();
}

TEST_F(SmokeTest, GTEST_TESTCASE(CheckTransmissionTime, GTEST_DISABLED)) {
    CANAPI_Bitrate_t bitrate = {};
    for (int32_t index = CANBDR_1000; index <= CANBDR_10; index++) {
        bitrate.index = -index;
        std::cout << "[   IDX" << index << "   ] " << CCanDevice::TransmissionTime(bitrate) << "us" << std::endl;
    }
}

// @gtest TC00.0: A test case
//
// @expected: CANERR_NOERROR
//
TEST_F(SmokeTest, GTEST_TESTCASE(SomeTest, GTEST_DISABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opMode;
    CANAPI_Bitrate_t bitRate;
    CANAPI_BusSpeed_t busSpeed;
    CANAPI_Return_t retVal;

    // @test:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get operation capabilities and actual mode from DUT1
    retVal = dut1.GetOpCapabilities(opMode);
    EXPECT_EQ(CCanApi::NoError, retVal);
    retVal = dut1.GetOpMode(opMode);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get actual bit-rate and transmission speed from DUT1
    retVal = dut1.GetBitrate(bitRate);
    EXPECT_EQ(CCanApi::NoError, retVal);
    retVal = dut1.GetBusSpeed(busSpeed);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- stop/reset DUT1
    retVal = dut1.ResetController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC00.1: The default scenario, suitable for long-term stress tests
//
// @expected: CANERR_NOERROR
//
TEST_F(SmokeTest, DefaultScenario) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    
    // @pre:
    // @- show test configuration
    dut1.ShowDeviceInformation("[   DUT1   ]");
    dut2.ShowDeviceInformation("[   DUT2   ]");
    dut1.ShowOperationMode("[   MODE   ]");
    dut1.ShowBitrateSettings("[   BAUD   ]");
    // @- probe if DUT1 is present and not occupied
    retVal = dut1.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT1) << " is not available";
    // @- show channel information of DUT1
    dut1.ShowChannelInformation("[   CH:1   ]");
    dut1.ShowChannelCapabilities("[   CAPA   ]");
    // @- probe if DUT2 is present and not occupied
    retVal = dut2.ProbeChannel(state);
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.ProbeChannel() failed with error code " << retVal;
    ASSERT_EQ(CCanApi::ChannelAvailable, state) << "[  ERROR!  ] " << g_Options.GetDeviceName(DUT2) << " is not available";
    // @- show channel information of DUT2
    dut2.ShowChannelInformation("[   CH:2   ]");
    dut2.ShowChannelCapabilities("[   CAPA   ]");
    // @- check if different channels have been selected
    ASSERT_TRUE((g_Options.GetChannelNo(DUT1) != g_Options.GetChannelNo(DUT2)) || \
                (g_Options.GetLibraryId(DUT1) != g_Options.GetLibraryId(DUT2))) << "[  ERROR!  ] same channel selected twice";
    // @test:
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
    int32_t frames = g_Options.GetNumberOfSmokeTestFrames();
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

//  $Id: TC00_SmokeTest.cc 1217 2023-10-10 19:28:31Z haumea $  Copyright (c) UV Software, Berlin.
