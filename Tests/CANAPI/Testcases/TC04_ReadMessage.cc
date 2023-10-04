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
#ifndef FEATURE_ERROR_FRAMES
#define FEATURE_ERROR_FRAMES  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_ERROR_FRAMES not set, default = FEATURE_SUPPORTED" )
#else
#warning FEATURE_ERROR_FRAMES not set, default = FEATURE_SUPPORTED
#endif
#endif
#define TC04_8_DEBUG   0
#define TC04_15_DEBUG  0
#define TC04_16_DEBUG  0
#define TC04_18_DEBUG  0

class ReadMessage : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC04.0: Read a CAN message (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(ReadMessage, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x400U;
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
    CCounter counter = CCounter(true);
    // @- DUT2 send out one message
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- DUT1 read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare sent and received message
    EXPECT_TRUE(dut1.CompareMessages(trmMsg, rcvMsg));
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

// @gtest TC04.1: Read a CAN message with invalid channel handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC04.2: Give a NULL pointer as argument for parameter 'message'
//
// @note: passing a pointer for 'message' is not possible with the C++ API!

// @gtest TC04.3: Read a CAN message if CAN channel is not initialized
//
// @expected: CANERR_HANDLE (would it not be better to return NOTINIT in C++ API?)
//
TEST_F(ReadMessage, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x403U;
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
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- DUT2 send out one message
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::InvalidHandle, retVal);
    // @post:
    counter.Clear();
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
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- DUT1 read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare sent and received message
    EXPECT_TRUE(dut1.CompareMessages(trmMsg, rcvMsg));
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

// @gtest TC04.4: Read a CAN message if CAN controller is not started
//
// @expected: CANERR_OFFLINE
//
TEST_F(ReadMessage, GTEST_TESTCASE(IfControllerNotStarted, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x404U;
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
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- DUT2 send out one message
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ControllerOffline, retVal);
    // @post:
    counter.Clear();
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- DUT1 read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare sent and received message
    EXPECT_TRUE(dut1.CompareMessages(trmMsg, rcvMsg));
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

// @gtest TC04.5: Read a CAN message if CAN controller was previously stopped
//
// @expected: CANERR_OFFLINE
//
TEST_F(ReadMessage, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x405U;
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
    // @- DUT2 send out one message
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ControllerOffline, retVal);
    // @post:
    counter.Clear();
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC04.6: Read a CAN message if CAN channel was previously torn down
//
// @expected: CANERR_HANDLE (would it not be better to return NOTINIT in C++ API?)
//
TEST_F(ReadMessage, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x406U;
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
    // @- DUT2 send out one message
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::InvalidHandle, retVal);
    // @post:
    counter.Clear();
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC04.7: Read a CAN message if receive queue is empty
//
// @expected: CANERR_RX_EMPTY and status bit 'receiver_empty' is set
//
TEST_F(ReadMessage, GTEST_TESTCASE(IfReceiveQueueEmpty, GTEST_ENABLED)) {
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

// @gtest TC04.8: Read a CAN message from receive queue after overrun
//
// @expected: CANERR_NOERROR but status bit 'queue_overrun' set
//
#if (FEATURE_SIZE_RECEIVE_QUEUE != 0)
#define GTEST_RECEIVE_QUEUE_FULL  GTEST_ENABLED
#else
#define GTEST_RECEIVE_QUEUE_FULL  GTEST_DISABLED
#endif
TEST_F(ReadMessage, GTEST_TESTCASE(IfReceiveQueueFull, GTEST_RECEIVE_QUEUE_FULL)) {
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
            retVal = dut2.WriteMessage(trmMsg, DEVICE_SEND_TIMEOUT);
            if (retVal == CCanApi::TransmitterBusy)
                PCBUSB_QXMT_DELAY();
        } while (retVal == CCanApi::TransmitterBusy);
        CTimer::Delay(dut2.TransmissionTime(dut2.GetBitrate(), 1, CCanApi::Dlc2Len(trmMsg.dlc)));
        // on error abort
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.WriteMessage() failed with error code " << retVal;
        progress.Update(i + 1, 0);
    }
#if (TC04_8_DEBUG != 0)
    // get current time: end of transmission
    t1 = CTimer::GetTime();;
#endif
    // @- an additional delay to ensure that the last message is received by DUT1
    uint32_t delay = dut2.TransmissionTime(dut2.GetBitrate(), 10, CCanApi::Dlc2Len(trmMsg.dlc));
    CTimer::Delay(delay);
    // @- DUT1 read them all to empty the reception queue
    CTimer timer = CTimer(((dut1.TransmissionTime(dut1.GetBitrate(), (spam + DEVICE_LOOP_EXTRA)) *
        DEVICE_LOOP_FACTOR) / DEVICE_LOOP_DIVISOR));  // bit-rate dependent timeout
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
    timer.Restart((DEVICE_SEND_TIMEOUT + DEVICE_SEND_TIMEOUT) * CTimer::MSEC);
    do {
        retVal = dut2.WriteMessage(trmMsg, DEVICE_SEND_TIMEOUT);
        if (retVal == CCanApi::TransmitterBusy)
            PCBUSB_QXMT_DELAY();
    } while ((retVal == CCanApi::TransmitterBusy) && !timer.Timeout());
    do {
        // read message by message (with time-out)
        retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    } while ((retVal == CCanApi::ReceiverEmpty) && !timer.Timeout());
    // @- todo: check if this also works with PeakCAN driver/wrapper!
#endif
    // @- get status of DUT1 and check if bit 'queue_overrun' is set
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.queue_overrun);
    // @post:
    progress.Clear();
#if (TC04_8_DEBUG != 0)
    uint64_t ovfl = 0U; // PCAN_EXT_RX_QUE_OVERRUN (0x84): receive queue overrun counter (optional)
    if (dut1.GetProperty((CANPROP_GET_VENDOR_PROP + 0x84U), (void*)&ovfl, sizeof(ovfl)) == CCanApi::NoError)
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

// @gtest TC04.9: Read a CAN message after message lost
//
// @expected: CANERR_xyz
//
TEST_F(ReadMessage, GTEST_TESTCASE(IfMessageLost, GTEST_DISABLED)) {
    // @unsolved: how to get a message lost?
    // @end.
}

// @gtest TC04.10: Measure the time-stamp accuracy of the device
//
// @expected: CANERR_xyz
//
TEST_F(ReadMessage, GTEST_TESTCASE(MeasureTimestampAccuracy, GTEST_DISABLED)) {
    // @todo: postponed, it's a special test case
}

// @gtest TC04.11: Read a CAN message with flag XTD set
//
// @expected: CANERR_NOERROR
//
TEST_F(ReadMessage, GTEST_TESTCASE(WithFlagXtdInOperationModeXtd, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x1FFFF40BU;
    trmMsg.xtd = 1;
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
    // dut1.ShowOperationMode();
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- get operation capabilities of DUT1
    retVal = dut1.GetOpCapabilities(opCapa);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if supressing extended frames is supported
    if (!opCapa.nxtd)
        GTEST_SKIP() << "Supressing extended frames is not supported by DUT1!";
    // @- get operation mode of DUT1 and check bit NXTD is not set
    retVal = dut1.GetOpMode(opMode);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(opMode.nxtd);
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
    // dut2.ShowOperationMode();
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
    CCounter counter = CCounter(true);
    // @- DUT2 send out one message with extended id.
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- DUT1 read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare sent and received message
    EXPECT_TRUE(dut1.CompareMessages(trmMsg, rcvMsg));
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

// @gtest TC04.12: Read a CAN message with flag XTD set but operation mode NXTD is selected (suppress extended frames)
//
// @expected: CANERR_RX_EMPTY if suppressing extended frames is supported
//
TEST_F(ReadMessage, GTEST_TESTCASE(WithFlagXtdInOperationModeNoXtd, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x1FFFF40CU;
    trmMsg.xtd = 1;
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
    // @- get operation capabilities from DUT1
    retVal = dut1.GetOpCapabilities(opCapa);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- tear down DUT1 again
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @ - check if supressing extended frames is supported
    if (!opCapa.nxtd)
        GTEST_SKIP() << "Supressing extended frames is not supported by DUT1!";
    // @- set operation mode bit NXTD (suppress extended frames)
    opMode = dut1.GetOpMode();
    opMode.nxtd = 1;
    dut1.SetOpMode(opMode);
    // @- initialize DUT1 with bit NXTD set
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // dut1.ShowOperationMode();
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- get operation mode of DUT1 and check bit NXTD is set
    retVal = dut1.GetOpMode(opMode);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(opMode.nxtd);
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
    // dut2.ShowOperationMode();
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
    CCounter counter = CCounter(true);
    // @- DUT2 send out one message with extended id.
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- DUT1 wait in vain for the message
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
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

// @gtest TC04.13: Request a CAN message with flag RTR set
//
// @expected: CANERR_NOERROR
//
TEST_F(ReadMessage, GTEST_TESTCASE(WithFlagRtrInOperationModeRtr, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x40DU;
    trmMsg.xtd = 0;
    trmMsg.rtr = 1;
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
    // @
    // @note: This test cannot run in CAN FD operation mode
    if (g_Options.GetOpMode(DUT1).fdoe || g_Options.GetOpMode(DUT2).fdoe)
        GTEST_SKIP() << "This test cannot run in CAN FD operation mode!";
#endif
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // dut1.ShowOperationMode();
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- get operation capabilities of DUT1
    retVal = dut1.GetOpCapabilities(opCapa);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- check if supressing remote frames is supported
    if (!opCapa.nrtr)
        GTEST_SKIP() << "Supressing remote frames is not supported by DUT1!";
    // @- get operation mode of DUT1 and check bit NRTR is not set
    retVal = dut1.GetOpMode(opMode);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(opMode.nrtr);
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
    // dut2.ShowOperationMode();
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
    CCounter counter = CCounter(true);
    // @- DUT2 send out one message with flag RTR
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- DUT1 read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- compare sent and received message (w/o dlc and data)
    EXPECT_EQ(trmMsg.id, rcvMsg.id);
    EXPECT_EQ(trmMsg.rtr, rcvMsg.rtr);
    EXPECT_EQ(trmMsg.xtd, rcvMsg.xtd);
    EXPECT_EQ(trmMsg.sts, rcvMsg.sts);
#if (OPTION_CAN_2_0_ONLY == 0)
    EXPECT_EQ(trmMsg.fdf, rcvMsg.fdf);
    EXPECT_EQ(trmMsg.brs, rcvMsg.brs);
    EXPECT_EQ(trmMsg.esi, rcvMsg.esi);
#endif
    // @post:
    counter.Clear();
    std::cout << "[   INFO   ] Received message with flag 'RTR' has DLC of " << (int)rcvMsg.dlc << std::endl;
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

// @gtest TC04.14: Request a CAN message with flag RTR set but operation mode NRTR is selected (suppress remote frames)
//
// @expected: CANERR_RX_EMPTY if suppressing remote frames is supported
//
TEST_F(ReadMessage, GTEST_TESTCASE(WithFlagRtrInOperationModeNoRtr, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x40EU;
    trmMsg.xtd = 0;
    trmMsg.rtr = 1;
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
    // @
    // @note: This test cannot run in CAN FD operation mode
    if (g_Options.GetOpMode(DUT1).fdoe || g_Options.GetOpMode(DUT2).fdoe)
        GTEST_SKIP() << "This test cannot run in CAN FD operation mode!";
#endif
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
    // @ - check if supressing remote frames is supported
    if (!opCapa.nrtr)
        GTEST_SKIP() << "Supressing remote frames is not supported by DUT1!";
    // @- set operation mode bit NRTR (suppress remote frames)
     opMode = dut1.GetOpMode();
    opMode.nrtr = 1;
    dut1.SetOpMode(opMode);
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // dut1.ShowOperationMode();
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- get operation mode of DUT1 and check bit NRTR is not set
    retVal = dut1.GetOpMode(opMode);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(opMode.nrtr);
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
    // dut2.ShowOperationMode();
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
    CCounter counter = CCounter(true);
    // @- DUT2 send out one message with flag RTR
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- DUT1 wait in vain for the message
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
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

// @gtest TC04.15: Read an error frame with flag STS set
//
// @expected: CANERR_NOERROR but status bit 'warning_level' is set and no status frame in the receive queue
//
TEST_F(ReadMessage, GTEST_TESTCASE(WithFlagStsInOperationModeNoErr, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t newBtr1 = {}, oldBtr1 = {};
    CANAPI_Bitrate_t newBtr2 = {}, oldBtr2 = {};
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x40FU;
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
#if (TC04_15_ISSUE_PCBUSB_WARNING_LEVEL == WORKAROUND_ENABLED)
    ASSERT_TRUE(false) << "[  TC04.15 ] No warning level from device!";
#endif
    // @
    // @note: This test cannot run if there is another device on bus!
    if (g_Options.Is3rdDevicePresent())
        GTEST_SKIP() << "This test cannot run if there is another device on bus!";
    // @pre:
    // @- initialize DUT1 configured settings
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
        if (status.warning_level) {
            // @-- DUT1 read recieve queue (expect empty)
            retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
            EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
        }
    } while ((i < TEST_MAX_EWRN) && !status.warning_level && !status.bus_off);
    EXPECT_TRUE(status.warning_level);
    // @post:
    progress.Clear();
#if (TC04_15_DEBUG != 0)
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

// @gtest TC04.16: Read an error frame with flag STS set and operation mode ERR is selected (error frame reception)
//
// @expected: CANERR_NOERROR but status bit 'warning_level' is set and a status frame in the receive queue
//
#if (FEATURE_ERROR_FRAMES != FEATURE_UNSUPPORTED)
#define GTEST_STATUS_MESSAGE  GTEST_ENABLED
#else
#define GTEST_STATUS_MESSAGE  GTEST_DISABLED
#endif
TEST_F(ReadMessage, GTEST_TESTCASE(WithFlagStsInOperationModeErr, GTEST_STATUS_MESSAGE)) {
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
    trmMsg.id = 0x410U;
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
        GTEST_SKIP() << "Error frames are not supported by the device!";
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
    if (opMode.fdoe) SLOW_BITRATE_FD(newBtr1);
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
    if (opMode.fdoe) FAST_BITRATE_FD(newBtr2);
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
        if (status.warning_level) {
            // @-- DUT1 read recieve queue (expect a status frame)
            retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
#if (TC04_16_ISSUE_KVASER_STATUS_MESSAGE != WORKAROUND_ENABLED)
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_TRUE(rcvMsg.sts);
#else
            if (retVal != CCanApi::ReceiverEmpty) {
                EXPECT_EQ(CCanApi::NoError, retVal);
                EXPECT_TRUE(rcvMsg.sts);
            } else {
                // @!! issue(Kavser U100P): device does not send status frames
                EXPECT_TRUE(false) << "[  TC04.16  ] Status message not received! (U100P issue)";
            }
#endif
        }
    } while ((i < TEST_MAX_EWRN) && !status.warning_level && !status.bus_off);
    EXPECT_TRUE(status.warning_level);
    // @post:
    progress.Clear();
#if (TC04_16_DEBUG != 0)
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

// @gtest TC04.17: Read a CAN message but operation mode MON is selected (listen-only mode)
//
// @expected: CANERR_NOERROR
//
TEST_F(ReadMessage, GTEST_TESTCASE(InOperationModeListenOnly, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x411U;
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
    // @
    // @note: This test cannot run in CAN FD operation mode
    if (!g_Options.Is3rdDevicePresent())
        GTEST_SKIP() << "This test can only run if there is a 3rd device present!";
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
    // @! exit test if listen-only mode is not supported
    if (!opCapa.mon)
        GTEST_SKIP() << "Listen-only mode is not supported by the device!";
    // @- enable listen-only mode for DUT1
    opMode = dut1.GetOpMode();
    opMode.mon = 1;
    dut1.SetOpMode(opMode);
    // @- initialize DUT1 with configured settings
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    //dut1.ShowOperationMode();
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @- get operation mode of DUT1 and check bit MON is set
    retVal = dut1.GetOpMode(opMode);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(opMode.mon);
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
    // dut2.ShowOperationMode();
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
    CCounter counter = CCounter(true);
    // @- DUT2 send out one message
    retVal = dut2.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- DUT1 read the message (with time-out)
    retVal = dut1.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- compare sent and received message
    EXPECT_TRUE(dut1.CompareMessages(trmMsg, rcvMsg));
    // @post:
    counter.Clear();
    // @- todo: try to send some frames to DUT2 (should not succeed)
    // @- todo: receive some frames from DUT2 (fix mode MON in Device.cpp)
    //int32_t frames = g_Options.GetNumberOfTestFrames();
    //EXPECT_EQ(CANERR_TIMEOUT, dut1.SendSomeFrames(dut2, frames));
    //EXPECT_EQ(frames, dut1.ReceiveSomeFrames(dut2, frames));
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

// @gtest TC04.18: Read a CAN message from empty queue with different time-out values
// 
// @expected: CANERR_RX_EMPTY after time-out time has expired
// 
TEST_F(ReadMessage, GTEST_TESTCASE(WithDifferentTimeoutValues, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t rcvMsg = {};
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
    CCounter counter = CCounter(true);
    struct timespec t0 = {}, t1 = {};
    double dt;
    // @sub(1): timeout = 0ms
    t0 = CTimer::GetTime();
    // @- DUT1 try to read a message from empty queue
    retVal = dut1.ReadMessage(rcvMsg, 0U);
    t1 = CTimer::GetTime();
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- check if expired time is greater than 0ms
    dt = CTimer::DiffTime(t0, t1);
    EXPECT_LE((double)0.0000, dt);
#if (TC04_18_DEBUG != 0)
    counter.Clear();
    std::cout << "[  0.000ms ] dt=" << dt << std::endl;
#endif
    // @sub(2): timeout = 1ms
    t0 = CTimer::GetTime();
    // @- DUT1 try to read a message from empty queue
    retVal = dut1.ReadMessage(rcvMsg, 1U);
    t1 = CTimer::GetTime();
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- check if expired time is at least 0ms
    dt = CTimer::DiffTime(t0, t1);
    EXPECT_LE((double)0.000, dt);
#if (TC04_18_DEBUG != 0)
    std::cout <<  "[  0.001ms ] dt=" << dt << std::endl;
#endif
    // @sub(3): timeout = 10ms
    t0 = CTimer::GetTime();
    // @- DUT1 try to read a message from empty queue
    retVal = dut1.ReadMessage(rcvMsg, 10U);
    t1 = CTimer::GetTime();
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- check if expired time is at least 9ms
    dt = CTimer::DiffTime(t0, t1);
    EXPECT_LE((double)0.009, dt);
#if (TC04_18_DEBUG != 0)
    std::cout <<  "[  0.010ms ] dt=" << dt << std::endl;
#endif
    // @sub(4): timeout = 100ms
    t0 = CTimer::GetTime();
    // @- DUT1 try to read a message from empty queue
    retVal = dut1.ReadMessage(rcvMsg, 100U);
    t1 = CTimer::GetTime();
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- check if expired time is at least 99ms
    dt = CTimer::DiffTime(t0, t1);
    EXPECT_LE((double)0.099, dt);
#if (TC04_18_DEBUG != 0)
    std::cout <<  "[  0.100ms ] dt=" << dt << std::endl;
#endif
    // @sub(5): timeout = 1000ms
    t0 = CTimer::GetTime();
    // @- DUT1 try to read a message from empty queue
    retVal = dut1.ReadMessage(rcvMsg, 1000U);
    t1 = CTimer::GetTime();
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- check if expired time is at least 999ms
    dt = CTimer::DiffTime(t0, t1);
    EXPECT_LE((double)0.999, dt);
#if (TC04_18_DEBUG != 0)
    std::cout <<  "[  1.000ms ] dt=" << dt << std::endl;
#endif
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

// @todo: (1) blocking read
// @todo: (2) test reentrancy

//  $Id: TC04_ReadMessage.cc 1201 2023-09-13 11:09:28Z makemake $  Copyright (c) UV Software, Berlin.
