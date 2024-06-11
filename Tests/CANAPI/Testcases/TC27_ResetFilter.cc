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

#ifndef FEATURE_FILTERING
#define FEATURE_FILTERING  FEATURE_UNSUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_FILTERING not set, default = FEATURE_UNSUPPORTED" )
#else
#warning FEATURE_FILTERING not set, default = FEATURE_UNSUPPORTED
#endif
#endif
#if (FEATURE_FILTERING != FEATURE_UNSUPPORTED)

class ResetFilter : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC27.0: Reset CAN acceptance filter (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(ResetFilter, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    CANAPI_Status_t status = {};
    uint32_t codeGet, maskGet;
    uint32_t codeSet, maskSet;
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
    // @sub(1): 11-bit identifier
    // @- get 11-bit filter (code 0x000 and mask 0x000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_11BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_11BIT, maskGet);
    // @- set 11-bit filter (code 0x000 and mask 0x500)
    // @  note: DUT2 is sending test frames with 0x200!
    codeSet = 0x00000000U; maskSet = 0x0000000500U;
    retVal = dut1.SetFilter11Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 11-bit filter (code 0x000 and mask 0x500)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(codeSet, codeGet);
    EXPECT_EQ(maskGet, maskGet);
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 11-bit filter (code 0x000 and mask 0x000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_11BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_11BIT, maskGet);
    // @sub(2): 29-bit identifier
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @- set 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeSet = 0x00001000U; maskSet = 0x00000500U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(codeSet, codeGet);
    EXPECT_EQ(maskGet, maskGet);
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
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

// @gtest TC27.1: Reset CAN acceptance filter with invalid interface handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC27.2: Reset CAN acceptance filter if CAN channel is not initialized
//
// @expected: CANERR_NOTINIT
//
TEST_F(ResetFilter, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
 
    // @test:
    // @- try to reset acceptance filter
    retVal = dut1.ResetFilters();
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

// @gtest TC27.2: Reset CAN acceptance filter if CAN controller is not started
//
// @expected: CANERR_NOERROR
//
TEST_F(ResetFilter, GTEST_TESTCASE(IfControllerNotStarted, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    uint32_t codeGet, maskGet;
    uint32_t codeSet, maskSet;
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
    // @sub(1): 11-bit identifier
    // @- get 11-bit filter (code 0x000 and mask 0x000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_11BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_11BIT, maskGet);
    // @- set 11-bit filter (code 0x000 and mask 0x500)
    // @  note: DUT2 is sending test frames with 0x200!
    codeSet = 0x00000000U; maskSet = 0x0000000500U;
    retVal = dut1.SetFilter11Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 11-bit filter (code 0x000 and mask 0x500)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(codeSet, codeGet);
    EXPECT_EQ(maskGet, maskGet);
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 11-bit filter (code 0x000 and mask 0x000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_11BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_11BIT, maskGet);
    // @sub(2): 29-bit identifier
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @- set 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeSet = 0x00001000U; maskSet = 0x00000500U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(codeSet, codeGet);
    EXPECT_EQ(maskGet, maskGet);
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
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

// @gtest TC27.4: Reset CAN acceptance filter if CAN controller is started
//
// @expected: CANERR_ONLINE
//
TEST_F(ResetFilter, GTEST_TESTCASE(IfControllerStarted, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    uint32_t codeGet, maskGet;
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
    // @sub(1): 11-bit identifier
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::ControllerOnline, retVal);
    // @- get 11-bit filter (code 0x000 and mask 0x000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_11BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_11BIT, maskGet);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @sub(2): 29-bit identifier
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::ControllerOnline, retVal);
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- try to reset acceptance filter
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::ControllerOnline, retVal);   
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

// @gtest TC27.5: Reset CAN acceptance filter if CAN controller was previously stopped
//
// @expected: CANERR_NOERROR
//
TEST_F(ResetFilter, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    uint32_t codeGet, maskGet;
    uint32_t codeSet, maskSet;
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
    // @sub(1): 11-bit identifier
    // @- get 11-bit filter (code 0x000 and mask 0x000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_11BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_11BIT, maskGet);
    // @- set 11-bit filter (code 0x000 and mask 0x500)
    // @  note: DUT2 is sending test frames with 0x200!
    codeSet = 0x00000000U; maskSet = 0x0000000500U;
    retVal = dut1.SetFilter11Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 11-bit filter (code 0x000 and mask 0x500)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(codeSet, codeGet);
    EXPECT_EQ(maskGet, maskGet);
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 11-bit filter (code 0x000 and mask 0x000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter11Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_11BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_11BIT, maskGet);
    // @sub(2): 29-bit identifier
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @- set 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeSet = 0x00001000U; maskSet = 0x00000500U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(codeSet, codeGet);
    EXPECT_EQ(maskGet, maskGet);
    // @- reset acceptance filter
    // @  note: SJA100 has only one filter for 11-bit and 29-bit identifier!
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @post:
    // @- start DUT1 with configured bit-rate settings
    retVal = dut1.StartController();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- get status of DUT1 and check to be in RUNNING state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_FALSE(status.can_stopped);
    // @- send some frames to DUT2 and receive some frames from DUT2
    /*int32_t*/ frames = g_Options.GetNumberOfTestFrames();
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

// @gtest TC27.6: Reset CAN acceptance filter if CAN channel was previously torn down
//
// @expected: CANERR_NOTINIT
//
TEST_F(ResetFilter, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
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
    // @- try to reset acceptance filter
    retVal = dut1.ResetFilters();
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @end.
}

#endif // FEATURE_FILTERING != FEATURE_UNSUPPORTED

//  $Id: TC27_ResetFilter.cc 1272 2024-04-16 19:55:27Z makemake $  Copyright (c) UV Software, Berlin.
