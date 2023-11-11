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

class WriteMessage : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC05.0: Send a CAN message (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(WriteMessage, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x500U;
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
    // @- DUT1 send out one message
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- DUT2 wait for one message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare sent and received message
    EXPECT_TRUE(dut2.CompareMessages(trmMsg, rcvMsg));
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

// @gtest TC05.1: Send a CAN message with invalid channel handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC05.2: Give a NULL pointer as argument for parameter 'message'
//
// @note: passing a pointer for 'message' is not possible with the C++ API!

// @gtest TC05.3: Send a CAN message if CAN channel is not initialized
//
// @expected: CANERR_HANDLE (would it not be better to return NOTINIT in C++ API?)
//
TEST_F(WriteMessage, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x503U;
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
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to send out one message
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::InvalidHandle, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
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
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
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

// @gtest TC05.4: Send a CAN message if CAN controller is not started
//
// @expected: CANERR_OFFLINE
//
TEST_F(WriteMessage, GTEST_TESTCASE(IfControllerNotStarted, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x504U;
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
    // @- issue(PeakCAN): why do we need a delay here?
    PCBUSB_INIT_DELAY();
    // @- get status of DUT2 and check to be in RUNNING state
    retVal = dut2.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to send out one message
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::ControllerOffline, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @post:
    counter.Clear();
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
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

// @gtest TC05.5: Send a CAN message if CAN controller was previously stopped
//
// @expected: CANERR_OFFLINE
//
TEST_F(WriteMessage, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x505U;
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
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to send out one message
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::ControllerOffline, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
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

// @gtest TC05.6: Send a CAN message if CAN channel was previously torn down
//
// @expected: CANERR_HANDLE (would it not be better to return NOTINIT in C++ API?)
//
TEST_F(WriteMessage, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x506U;
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
    // @test:
    CCounter counter = CCounter(true);
    // @- DUT1 try to send out one message
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::InvalidHandle, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @post:
    counter.Clear();
    // @- tear down DUT2
    retVal = dut2.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TC05.7: Send CAN messages with valid 11-bit identifier(s) and check its correct transmission on receiver side
//
// @expected: CANERR_NOERROR
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithValid11bitIdentifier, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x507U;
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
    // @test:
    CCounter counter = CCounter();
    // @- loop over all 11-bit CAN identifier (0x000 to 0x7FF with +1)
    for (uint32_t canId = 0x000U; canId <= CAN_MAX_STD_ID; canId++) {
        trmMsg.id = canId;
        trmMsg.data[0] = (uint8_t)(canId >> 0);
        trmMsg.data[1] = (uint8_t)(canId >> 8);
        trmMsg.data[2] = (uint8_t)(canId >> 16);
        trmMsg.data[3] = (uint8_t)(canId >> 24);
        counter.Increment();
        // @-- DUT1 send out the message with valid STD id.
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- DUT2 wait for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare sent and received message
        EXPECT_TRUE(dut2.CompareMessages(trmMsg, rcvMsg));
    }
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

// @gtest TC05.8: Send CAN messages with invalid 11-bit identifier(s)
//
// @expected: CANERR_ILLPARA
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithInvalid11bitIdentifier, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x508U;
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
    // @test:
    CCounter counter = CCounter();
    // @- sub(1): 0x800
    trmMsg.id = 0x800U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(2): 0xFFF
    trmMsg.id = 0xFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(3): 0x1000
    trmMsg.id = 0x1000U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(4): 0xFFFF
    trmMsg.id = 0xFFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(5): 0x10000
    trmMsg.id = 0x10000U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(6): 0xFFFFF
    trmMsg.id = 0xFFFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(7): 0x100000
    trmMsg.id = 0x100000U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(8): 0xFFFFFF
    trmMsg.id = 0xFFFFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(9): 0x1000000
    trmMsg.id = 0x1000000U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(A): 0xFFFFFFF
    trmMsg.id = 0xFFFFFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(B): 0x10000000
    trmMsg.id = 0x10000000U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(C): 0x1FFFFFFF
    trmMsg.id = 0x1FFFFFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(D): 0x20000000
    trmMsg.id = 0x20000000U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(E): 0xFFFFFFFF
    trmMsg.id = 0xFFFFFFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

// @gtest TC05.9: Send CAN messages with valid 29-bit identifier(s) and check its correct transmission on receiver side
//
// @expected: CANERR_NOERROR
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithValid29bitIdentifier, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x509U;
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
    CCounter counter = CCounter();
    // @- loop over many 29-bit CAN identifier (0x000 to 0x1FFFFFFF with (<<1)+1)
    for (uint32_t canId = 0x000U; canId <= CAN_MAX_XTD_ID; canId = ((canId << 1) + 1U)) {
        trmMsg.id = canId;
        trmMsg.data[0] = (uint8_t)(canId >> 0);
        trmMsg.data[1] = (uint8_t)(canId >> 8);
        trmMsg.data[2] = (uint8_t)(canId >> 16);
        trmMsg.data[3] = (uint8_t)(canId >> 24);
        counter.Increment();
        // @-- DUT1 send out the message with valid XTD id.
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- DUT2 wait for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare sent and received message
        EXPECT_TRUE(dut2.CompareMessages(trmMsg, rcvMsg));
    }
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

// @gtest TC05.10: Send CAN messages with invalid 29-bit identifier(s)
//
// @expected: CANERR_ILLPARA
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithInvalid29bitIdentifier, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x50AU;
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
    CCounter counter = CCounter();
    // @- sub(1): 0x20000000
    trmMsg.id = 0x20000000U;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(2): 0xFFFFFFFF
    trmMsg.id = 0xFFFFFFFFU;
    counter.Increment();
    // @-- DUT1 try to send the message with invalid XTD
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

// @gtest TC05.11: Send CAN messages with valid Data Length Code(s) and check its correct transmission on receiver side
//
// @expected: CANERR_NOERROR
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithValidDataLengthCode, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x50BU;
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
    // @test:
    CCounter counter = CCounter();
    // @- loop over all Data Length Codes (0 to 8 or 15 with +1)
#if (OPTION_CAN_2_0_ONLY != 0)
    for (uint8_t canDlc = 0x0U; canDlc <= CAN_MAX_DLC; canDlc++) {
#else
    for (uint8_t canDlc = 0x0U; canDlc <= (trmMsg.fdf ? CANFD_MAX_DLC : CAN_MAX_DLC); canDlc++) {
#endif
        trmMsg.dlc = canDlc;
        trmMsg.data[0] = canDlc;
        counter.Increment();
        // @-- DUT1 send out the message with valid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- DUT2 wait for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- compare sent and received message
        EXPECT_TRUE(dut2.CompareMessages(trmMsg, rcvMsg));
    }
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

// @gtest TC05.12: Send CAN messages with invalid Data Length Code(s)
//
// @expected: CANERR_ILLPARA
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithInvalidDataLengthCode, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x50CU;
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
    // @test:
    CCounter counter = CCounter();
#if (OPTION_CAN_2_0_ONLY == 0)
    if (!trmMsg.fdf)
#endif
    {
        // @- sub(1): 9 (invalid in CAN 2.0)
        trmMsg.dlc = 0x9U;
        trmMsg.data[0] = trmMsg.dlc;
        counter.Increment();
        // @-- DUT1 try to send out the message with invalid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- DUT2 wait in vain for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
        // @- sub(2): 10 (invalid in CAN 2.0)
        trmMsg.dlc = 0xAU;
        trmMsg.data[0] = trmMsg.dlc;
        counter.Increment();
        // @-- DUT1 try to send out the message with invalid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- DUT2 wait in vain for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
        // @- sub(3): 11 (invalid in CAN 2.0)
        trmMsg.dlc = 0xBU;
        trmMsg.data[0] = trmMsg.dlc;
        counter.Increment();
        // @-- DUT1 try to send out the message with invalid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- DUT2 wait in vain for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
        // @- sub(4): 12 (invalid in CAN 2.0)
        trmMsg.dlc = 0xCU;
        trmMsg.data[0] = trmMsg.dlc;
        counter.Increment();
        // @-- DUT1 try to send out the message with invalid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- DUT2 wait in vain for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
        // @- sub(5): 13 (invalid in CAN 2.0)
        trmMsg.dlc = 0xDU;
        trmMsg.data[0] = trmMsg.dlc;
        counter.Increment();
        // @-- DUT1 try to send out the message with invalid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- DUT2 wait in vain for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
        // @- sub(6): 14 (invalid in CAN 2.0)
        trmMsg.dlc = 0xEU;
        trmMsg.data[0] = trmMsg.dlc;
        counter.Increment();
        // @-- DUT1 try to send out the message with invalid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- DUT2 wait in vain for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
        // @- sub(7): 15 (invalid in CAN 2.0)
        trmMsg.dlc = 0xFU;
        trmMsg.data[0] = trmMsg.dlc;
        counter.Increment();
        // @-- DUT1 try to send out the message with invalid DLC
        retVal = dut1.WriteMessage(trmMsg);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // @-- DUT2 wait in vain for the message
        retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
        EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    }
    // @- sub(8): 0x10 (invalid in CAN 2.0 and CAN FD)
    trmMsg.dlc = 0x10U;
    trmMsg.data[0] = trmMsg.dlc;
    counter.Increment();
    // @-- DUT1 try to send out the message with invalid DLC
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::ReceiverEmpty, retVal);
    // @- sub(9): 0xFF (invalid in CAN 2.0 and CAN FD)
    trmMsg.dlc = 0xFFU;
    trmMsg.data[0] = trmMsg.dlc;
    counter.Increment();
    // @-- DUT1 try to send out the message with invalid DLC
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @-- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

// @gtest TC05.13: Send a CAN message with flag XTD set but operation mode NXTD is selected (suppress extended frames)
//
// @expected: CANERR_ILLPARA if suppressing extended frames is supported
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithFlagXtdInOperationModeNoXtd, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x1FFFF50DU;
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
    opMode = g_Options.GetOpMode(DUT1);
    opMode.nxtd = 1;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    // @- initialize DUT1 with configured settings plus bit NXTD
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
    // @- DUT1 try to send out a message with flag XTD set
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

// @gtest TC05.14: Send a CAN message with flag RTR set but operation mode NRTR is selected (suppress remote frames)
//
// @expected: CANERR_ILLPARA if suppressing remote frames is supported
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithFlagRtrInOperationModeNoRtr, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x50EU;
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
    opMode = g_Options.GetOpMode(DUT1);
    opMode.nrtr = 1;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    // @- initialize DUT1 with configured settings plus bit NRTR
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
    // @- DUT1 try to send out a message with flag RTR set
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

#if (OPTION_CAN_2_0_ONLY == 0)
// @gtest TC05.15: Send a CAN message with flag FDF set in CAN 2.0 operation mode (FDOE = 0)
//
// @expected: CANERR_ILLPARA (CAN FD format not supported in CAN classic mode)
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithFlagFdfInCan20OperationMode, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { TEST_BTRINDEX };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x50FU;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
    trmMsg.fdf = 1;
    trmMsg.brs = 0;
    trmMsg.esi = 0;
    trmMsg.dlc = CAN_MAX_DLC;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
    // @pre:
    opMode.fdoe = 0;
    opMode.brse = 0;
    // @- initialize DUT1 in CAN 2.0 operation mode
    dut1.SetOpMode(opMode);
    dut1.SetBitrate(bitrate);
    // dut1.ShowOperationMode();
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
    // @- initialize DUT2 in CAN 2.0 operation mode
    dut2.SetOpMode(opMode);
    dut2.SetBitrate(bitrate);
    // dut2.ShowOperationMode();
    // dut2.ShowBitrateSettings();
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
    // @- DUT1 try to send out a message with flag FDF set
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

// @gtest TC05.16: Send a CAN message with flag BRS set in CAN 2.0 operation mode (FDOE = 0 and BRSE = 0)
//
// @expected: CANERR_ILLPARA (CAN FD format not supported in CAN classic mode)
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithFlagBrsInCan20OperationMode, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { TEST_BTRINDEX };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x510U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
    trmMsg.fdf = 0;
    trmMsg.brs = 1;
    trmMsg.esi = 0;
    trmMsg.dlc = CAN_MAX_DLC;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
    // @pre:
    opMode.fdoe = 0;
    opMode.brse = 0;
    // @- initialize DUT1 in CAN 2.0 operation mode
    dut1.SetOpMode(opMode);
    dut1.SetBitrate(bitrate);
    // dut1.ShowOperationMode();
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
    // @- initialize DUT2 in CAN 2.0 operation mode
    dut2.SetOpMode(opMode);
    dut2.SetBitrate(bitrate);
    // dut2.ShowOperationMode();
    // dut2.ShowBitrateSettings();
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
    // @- DUT1 try to send out a message with flag BRS set
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @gtest TC05.17: Send a CAN message with flag BRS set in CAN FD operation mode (FDOE = 1) but bit-rate switching is not enabled (BRSE = 0)
//
// @expected: CANERR_ILLPARA (flag BRS set without flag FDF set is an illegal combination in CAN FD mode)
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithFlagBrsWithoutFlagFdfInCanFdOperationMode, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Bitrate_t bitrate = { TEST_BTRINDEX };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x511U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
    trmMsg.fdf = 0;
    trmMsg.brs = 1;
    trmMsg.esi = 0;
    trmMsg.dlc = CAN_MAX_DLC;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
    // @
    // @note: This test requires two CAN FD capable devices!
    if ((!dut1.IsCanFdCapable() || !dut2.IsCanFdCapable()) || g_Options.RunCanClassicOnly())
        GTEST_SKIP() << "At least one device is not CAN FD capable!";
    // @pre:
    opMode.fdoe = 1;
    opMode.brse = 1;
    TEST_BITRATE_FD(bitrate);
    // @- initialize DUT1 in CAN FD operation mode
    dut1.SetOpMode(opMode);
    dut1.SetBitrate(bitrate);
    // dut1.ShowOperationMode();
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
    // @- initialize DUT2 in CAN FD operation mode
    dut2.SetOpMode(opMode);
    dut2.SetBitrate(bitrate);
    // dut2.ShowOperationMode();
    // dut2.ShowBitrateSettings();
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
    // @- DUT1 try to send out a message with flag BRS set but not FDF
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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
#endif  // (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
#endif  // (OPTION_CAN_2_0_ONLY == 0)

// @gtest TC05.18: Send a CAN message with flag STS set (status message)
//
// @expected: CANERR_ILLPARA (status messages/error frames cannot be sent)
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithFlagSts, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = { TEST_CANMODE };
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x512U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 1;
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
    // @- set operation mode bit ERR (error frame reception) if supported
    opMode = g_Options.GetOpMode(DUT1);
    opMode.err = opCapa.err;
    dut1.SetOpMode(opMode);
    // dut1.ShowOperationMode();
    // @- initialize DUT1 with configured settings plus bit ERR
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
    // @- DUT1 try to send out a message with flag STS set
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- DUT2 wait in vain for the message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
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

// @gtest TC05.19: Send a CAN message if transmitter is busy (transmit queue full)
//
// @expected: CANERR_TX_BUSY and status bit 'transmitter_busy' is set
//
#if (FEATURE_SIZE_TRANSMIT_QUEUE != 0)
#define GTEST_TC04_19_ENABLED  GTEST_ENABLED
#else
#define GTEST_TC04_19_ENABLED  GTEST_DISABLED
#endif
TEST_F(WriteMessage, GTEST_TESTCASE(IfTransmitterBusy, GTEST_TC04_19_ENABLED)) {
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

#if (OPTION_CAN_2_0_ONLY == 0)
// @gtest TC05.20: Send a CAN message with flag ESI set (error indicator)
//
// @expected: CANERR_NOERROR (is this correct?)
//
TEST_F(WriteMessage, GTEST_TESTCASE(WithFlagEsi, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Message_t trmMsg = {};
    CANAPI_Message_t rcvMsg = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // CAN message
    trmMsg.id = 0x514U;
    trmMsg.xtd = 0;
    trmMsg.rtr = 0;
    trmMsg.sts = 0;
    trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
    trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
    trmMsg.esi = 1;
    trmMsg.dlc = g_Options.GetOpMode(DUT1).fdoe ? CANFD_MAX_DLC : CAN_MAX_DLC;
    memset(trmMsg.data, 0, CANFD_MAX_LEN);
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
    // @- DUT1 send out one message
    retVal = dut1.WriteMessage(trmMsg);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- DUT2 wait for one message
    retVal = dut2.ReadMessage(rcvMsg, TEST_READ_TIMEOUT);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- compare sent and received message
    EXPECT_TRUE(dut2.CompareMessages(trmMsg, rcvMsg));
    // @  note: flag ESI should also be set in received message in CAN FD mode
    if (dut2.GetOpMode().fdoe)
        EXPECT_EQ(1, rcvMsg.esi);
    else
        EXPECT_EQ(0, rcvMsg.esi);
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
#endif  // (OPTION_CAN_2_0_ONLY == 0)

// @todo: (1) blocking write
// @todo: (2) test reentrancy

//  $Id: TC05_WriteMessage.cc 1218 2023-10-14 12:18:19Z makemake $  Copyright (c) UV Software, Berlin.
