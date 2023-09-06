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

class InitializeChannel : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC02.0: Initialize CAN channel (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(InitializeChannel, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
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
    // @test:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
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

// @gtest TC02.1: Initialize CAN channel if CAN channel is already initialized
//
// @expected: CANERR_YETINIT
//
TEST_F(InitializeChannel, GTEST_TESTCASE(IfChannelInitialized, GTEST_ENABLED)) {
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
    // @test:
    // @- try to initialize DUT1 with configured settings again
    retVal = dut1.InitializeChannel();
    EXPECT_EQ(CCanApi::AlreadyInitialized, retVal);
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

// @gtest TC02.2: Initialize CAN channel if CAN controller is already started
//
// @expected: CANERR_YETINIT
//
TEST_F(InitializeChannel, GTEST_TESTCASE(IfControllerStarted, GTEST_ENABLED)) {
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
    // @- try to initialize DUT1 with configured settings again
    retVal = dut1.InitializeChannel();
    EXPECT_EQ(CCanApi::AlreadyInitialized, retVal);
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

// @gtest TC02.3: Initialize CAN channel if CAN controller was previously stopped
//
// @expected: CANERR_YETINIT
//
TEST_F(InitializeChannel, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
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
    // @test:
    // @- try to initialize DUT1 with configured settings again
    retVal = dut1.InitializeChannel();
    EXPECT_EQ(CCanApi::AlreadyInitialized, retVal);
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

// @gtest TC02.4: Initialize CAN channel if CAN channel was previously torn down
//
// @expected: CANERR_NOERROT
//
TEST_F(InitializeChannel, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
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
    // @- initialize DUT1 with configured settings again
    retVal = dut1.InitializeChannel();
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

// @gtest TC02.5: Initialize CAN channel with valid channel number(s)
//
// @expected: CANERR_NOERROR if available, otherwise CANERR_NOTINIT or vendor-specific error code
//
TEST_F(InitializeChannel, GTEST_TESTCASE(WithValidChannelNo, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::SChannelInfo info = {};
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @test:
    // @- loop over the list of devices to get the channel no.
    CCounter counter = CCounter();
    bool found = CCanDevice::GetFirstChannel(info);
    while (found) {
        // @-- try to initialize found channel with default mode (must be supported)
        retVal = dut1.InitializeChannel(info.m_nChannelNo, opMode);
        if (retVal == CCanApi::NoError) {
            // @-- on success: tear down the channel straightaway
            (void)dut1.TeardownChannel();
        }
        else {
            // @-- otherwise: error code could be vendor-specific
            EXPECT_TRUE((retVal == CCanApi::NotInitialized) || (retVal <= CCanApi::VendorSpecific));
        }
        counter.Increment();
        // next please
        found = CCanDevice::GetNextChannel(info);
    }
    counter.Clear();
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

// @gtest TC02.6: Initialize CAN channel with invalid channel number(s)
//
// @expected: CANERR_NOTINIT or vendor-specific error code
//
TEST_F(InitializeChannel, GTEST_TESTCASE(WithInvalidChannelNo, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @test:
    // @- try to initialize channel with invalid channel no. -1
    retVal = dut1.InitializeChannel((-1), opMode);
    EXPECT_TRUE((retVal == CCanApi::NotInitialized) || (retVal <= CCanApi::VendorSpecific));
    // @- try to initialize channel with invalid channel no. -2
    retVal = dut1.InitializeChannel((-2), opMode);
    EXPECT_TRUE((retVal == CCanApi::NotInitialized) || (retVal <= CCanApi::VendorSpecific));
    // @- try to initialize channel with invalid channel no. INT32_MIN
    retVal = dut1.InitializeChannel(INT32_MIN, opMode);
    EXPECT_TRUE((retVal == CCanApi::NotInitialized) || (retVal <= CCanApi::VendorSpecific));
    // @note: channel numbers are defined by the CAN device vendor.
    // @      Therefore, no assumptions can be made for positive values!
    //
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

// @gtest TC02.7: Check if CAN channel can be initialized with its full operation mode capability
//
// @expected: CANERR_NOERROR
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckOperationModeCapability, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Bitrate_t bitrate = { TEST_BTRINDEX };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- set operation mode to full operation mode capabilities
    opMode.byte = opCapa.byte;
    // @test:
    // @- initialize DUT1 with configured settings
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
    // @- start DUT1 with configured bit-rate settings
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    if (opMode.fdoe) TEST_BITRATE_FD(bitrate);
    // @  note: 250kbps:2Mbps if DUT1 is CAN FD capable
#endif
    retVal = dut1.StartController(bitrate);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    if (!opMode.mon) {
        // @- note: transmitter is off if monitor mode is selected
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
    // @end.
}

// @gtest TC02.8: Check if CAN channel can be initialized with operation mode bit MON set (listen-only mode)
//
// @expected: CANERR_NOERROR or CANERR_ILLPARA if listen-only mode is not supported
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckMonitorModeEnabledDisabled, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- set operation mode bit MON (listen-only mode)
    opMode.mon = 1;
    // @test:
    // @- initialize DUT1 with operation mode bit MON set
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    if (opCapa.mon) {
        // @- on success:
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- get operation mode from DUT1 (bit MON must be set)
        retVal = dut1.GetOpMode(opMode);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(opMode.mon);
        // @   todo: try to send a frame & receive some frames
        // TODO: insert coin here
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
    } else {
        // @- otherwise: expect error code ILLPARA
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- tear down DUT1 (just to make sure it's gone)
        retVal = dut1.TeardownChannel();
        EXPECT_NE(CCanApi::NoError, retVal);
    }
    // @end.
}

// @gtest TC02.9: Check if CAN channel can be initialized with operation mode bit ERR set (error frame reception)
//
// @expected: CANERR_NOERROR or CANERR_ILLPARA if listen-only mode is not supported
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckErrorFramesEnabledDisabled, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- set operation mode bit ERR (error frame reception)
    opMode.err = 1;
    // @test:
    // @- initialize DUT1 with operation mode bit ERR set
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    if (opCapa.err) {
        // @- on success:
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- get operation mode from DUT1 (bit ERR must be set)
        retVal = dut1.GetOpMode(opMode);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(opMode.err);
        // @   todo: send receive some frames
        // TODO: insert coin here
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
    }
    else {
        // @- otherwise: expect error code ILLPARA
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- tear down DUT1 (just to make sure it's gone)
        retVal = dut1.TeardownChannel();
        EXPECT_NE(CCanApi::NoError, retVal);
    }
    // @end.
}

// @gtest TC02.10: Check if CAN channel can be initialized with operation mode bit NRTR set (suppress remote frames)
//
// @expected: CANERR_NOERROR or CANERR_ILLPARA if suppressing remote frames is not supported
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckRemoteFramesDisabledEnabled, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- set operation mode bit NRTR (suppress remote frames)
    opMode.nrtr = 1;
    // @test:
    // @- initialize DUT1 with operation mode bit NRTR set
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    if (opCapa.nrtr) {
        // @- on success:
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- get operation mode from DUT1 (bit NRTR must be set)
        retVal = dut1.GetOpMode(opMode);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(opMode.nrtr);
        // @   todo: send receive some frames
        // TODO: insert coin here
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
    }
    else {
        // @- otherwise: expect error code ILLPARA
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- tear down DUT1 (just to make sure it's gone)
        retVal = dut1.TeardownChannel();
        EXPECT_NE(CCanApi::NoError, retVal);
    }
    // @end.
}

// @gtest TC02.11: Check if CAN channel can be initialized with operation mode bit NXTD set (suppress extended frames)
//
// @expected: CANERR_NOERROR or CANERR_ILLPARA if suppressing extended frames is not supported
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckExtendedFramesDisabledEnabled, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- set operation mode bit NXTD (suppress extended frames)
    opMode.nxtd = 1;
    // @test:
    // @- initialize DUT1 with operation mode bit NXTD set
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    if (opCapa.nxtd) {
        // @- on success:
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- get operation mode from DUT1 (bit NXTD must be set)
        retVal = dut1.GetOpMode(opMode);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(opMode.nxtd);
        // @   todo: send receive some frames
        // TODO: insert coin here
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
    }
    else {
        // @- otherwise: expect error code ILLPARA
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- tear down DUT1 (just to make sure it's gone)
        retVal = dut1.TeardownChannel();
        EXPECT_NE(CCanApi::NoError, retVal);
    }
    // @end.
}

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @gtest TC02.12: Check if CAN channel can be initialized with operation mode bit FDOE set (CAN FD operation enabled)
//
// @expected: CANERR_NOERROR or CANERR_ILLPARA if CAN FD operation is not supported
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckCanFdOperationEnabledDisabled, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- set operation mode bit FDOE (CAN FD operation enabled)
    opMode.fdoe = 1;
    opMode.brse = 0;
    // @test:
    // @- initialize DUT1 with operation mode bit FDOE set
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    if (opCapa.fdoe) {
        // @- on success:
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- get operation mode from DUT1 (bit FDOE must be set, but not BRSE)
        retVal = dut1.GetOpMode(opMode);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(opMode.fdoe);
        EXPECT_FALSE(opMode.brse);
        // @   todo: send receive some frames
        // TODO: insert coin here
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
    }
    else {
        // @- otherwise: expect error code ILLPARA
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- tear down DUT1 (just to make sure it's gone)
        retVal = dut1.TeardownChannel();
        EXPECT_NE(CCanApi::NoError, retVal);
    }
    // @end.
}

// @gtest TC02.13: Check if CAN channel can be initialized with operation mode bit FDOE and BRSE set (CAN FD operation with bit-rate switching enabled)
//
// @expected: CANERR_NOERROR or CANERR_ILLPARA if CAN FD operation or bit-rate switching is not supported
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckBitrateSwitchingEnabledDisabled, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
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
    // @- set operation mode bit FDOE and BRSE (CAN FD operation with bit-rate switching enabled)
    opMode.fdoe = 1;
    opMode.brse = 1;
    // @test:
    // @- initialize DUT1 with operation mode bit FDOE and BRSE set
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    if (opCapa.fdoe && opCapa.brse) {
        // @- on success:
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- get status of DUT1 and check to be in INIT state
        retVal = dut1.GetStatus(status);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(status.can_stopped);
        // @-- get operation mode from DUT1 (bit FDOE and BRSE must be set)
        retVal = dut1.GetOpMode(opMode);
        EXPECT_EQ(CCanApi::NoError, retVal);
        EXPECT_TRUE(opMode.fdoe);
        EXPECT_TRUE(opMode.brse);
        // @   todo: send receive some frames
        // TODO: insert coin here
        // @-- tear down DUT1
        retVal = dut1.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
    }
    else {
        // @- otherwise: expect error code ILLPARA
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- tear down DUT1 (just to make sure it's gone)
        retVal = dut1.TeardownChannel();
        EXPECT_NE(CCanApi::NoError, retVal);
    }
    // @end.
}

// @gtest TC02.14: Check if CAN channel can be initialized with operation mode bit BRSE set but not FDOE (invalid combination)
//
// @expected: CANERR_ILLPARA (invalid combinatio for CAN 2.0 operation and CAN FD operation)
//
TEST_F(InitializeChannel, GTEST_TESTCASE(CheckBitrateSwitchingEnabledWithoutCanFdEnabled, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Return_t retVal;
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
    // @- set operation mode bit BRSE but not FDOE (invalid combination)
    opMode.fdoe = 0;
    opMode.brse = 1;
    // @test:
    // @- initialize DUT1 with operation mode bit BRSE but not FDOE
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    retVal = dut1.InitializeChannel();
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- tear down DUT1 (just to make sure it's gone)
    retVal = dut1.TeardownChannel();
    EXPECT_NE(CCanApi::NoError, retVal);
    // @end.
}
#endif  // (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)

#if (OPTION_CANAPI_LIBRARY != OPTION_DISBALED)
// @gtest TC02.15: Initialize CAN channel with invalid library number(s)
//
// @expected: CANERR_xyz
//
TEST_F(InitializeChannel, GTEST_TESTCASE(WithInvalidLibraryId, GTEST_DISABLED)) {
    // @note: this test case can only run with the loader library
    // @todo: implement with CAN API V3 loader liabrary
    // TODO: insert coin here
    // @end.
}
#endif  // (OPTION_CANAPI_LIBRARY != OPTION_DISBALED)

//  $Id: TC02_InitializeChannel.cc 1169 2023-08-22 19:55:08Z makemake $  Copyright (c) UV Software, Berlin.
