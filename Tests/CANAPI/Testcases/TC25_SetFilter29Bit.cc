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

//  @note: This test suite tests the following methods:
//  @      - CCanApi::SetFilter29Bit()  [TC25.*]
//  @      - CCanApi::GetFilter29Bit()  [TC26.*]
//  @
#ifndef FEATURE_FILTERING
#define FEATURE_FILTERING  FEATURE_UNSUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_FILTERING not set, default = FEATURE_UNSUPPORTED" )
#else
#warning FEATURE_FILTERING not set, default = FEATURE_UNSUPPORTED
#endif
#endif
#if (FEATURE_FILTERING != FEATURE_UNSUPPORTED)

class SetFilter29Bit : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    void CheckFilter29Bit(CCanDevice& dut1, uint32_t accCode, uint32_t accMask) {
        CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
        CANAPI_Message_t trmMsg = {};
        CANAPI_Message_t rcvMsg = {};
        CANAPI_Return_t retVal;
        // CAN message
        trmMsg.id = 0U;
        trmMsg.xtd = 1;
        trmMsg.rtr = 0;
        trmMsg.sts = 0;
#if (OPTION_CAN_2_0_ONLY != 0)
        trmMsg.dlc = 0U; // CAN_MAX_DLC;
        memset(trmMsg.data, 0, CAN_MAX_LEN);
#else
        trmMsg.fdf = g_Options.GetOpMode(DUT1).fdoe ? 1 : 0;
        trmMsg.brs = g_Options.GetOpMode(DUT1).brse ? 1 : 0;
        trmMsg.esi = 0;
        trmMsg.dlc = 0U; // g_Options.GetOpMode(DUT1).fdoe ? CANFD_MAX_DLC : CAN_MAX_DLC;
        memset(trmMsg.data, 0, CANFD_MAX_LEN);
#endif
        // initialize DUT2 with configured settings
        retVal = dut2.InitializeChannel();
        ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut2.InitializeChannel() failed with error code " << retVal;
        // start DUT2 with configured bit-rate settings
        retVal = dut2.StartController();
        EXPECT_EQ(CCanApi::NoError, retVal);
        // DUT2 send 2048 standard messages
        int n = 0, frames = 2048;
        CProgress progress = CProgress(frames);
        for (int i = 0; i < frames; i++) {
            trmMsg.id = (uint32_t)i;
            // send one message (retry if busy)
            do {
                retVal = dut2.WriteMessage(trmMsg, DEVICE_SEND_TIMEOUT);
                if (retVal == CCanApi::TransmitterBusy)
                    PCBUSB_QXMT_DELAY();
            } while (retVal == CCanApi::TransmitterBusy);
            // on error abort
            if (retVal != CCanApi::NoError) {
                EXPECT_EQ(retVal, CCanApi::NoError);
                n = 0;
                break;
            }
            PCBUSB_QXMT_DELAY();  // TODO: why here?
            // update number of transmitted CAN messages
            progress.Update((int)(i + 1), (int)0);
            // update number of accepted CAN messages
            if (dut1.IsCanIdAccepted(trmMsg.id, accCode, accMask)) {
                n++;
            }
        }
        // DUT1 process all messages with acceptance filter
        uint64_t timeout = (((uint64_t)dut1.TransmissionTime(dut1.GetBitrate(), (n + DEVICE_LOOP_EXTRA))
                         *   (uint64_t)DEVICE_LOOP_FACTOR) / (uint64_t)DEVICE_LOOP_DIVISOR);  // bit-rate dependent timeout
        CTimer timer = CTimer(timeout);
        int j = 0;
        while ((j < n) && !timer.Timeout()) {
            // read one message from receiver's queue, if any
            retVal = dut1.ReadMessage(rcvMsg, DEVICE_READ_TIMEOUT);
            if (retVal == CCanApi::NoError) {
                // ignore status messages/error frames
                if (!rcvMsg.sts) {
                    // update number of received CAN messages
                    progress.Update((int)frames, (int)(j + 1));
                    // check if received message is accepted by acceptance filter
                    if (!dut1.IsCanIdAccepted(rcvMsg.id, accCode, accMask)) {
                        EXPECT_TRUE(dut1.IsCanIdAccepted(rcvMsg.id, accCode, accMask));
                        break;
                    }
                    j++;
                }
            }
            // on error abort (if receiver is empty, continue)
            else if (retVal != CCanApi::ReceiverEmpty) {
                EXPECT_EQ(retVal, CCanApi::ReceiverEmpty);
                break;
            }
        }
        progress.Clear();
        // tear down DUT2
        retVal = dut2.TeardownChannel();
        EXPECT_EQ(CCanApi::NoError, retVal);
    }
};

// @gtest TC25/TC26.0: CAN acceptance filter for 29-bit identifier (sunnyday scenario)
//
// @expected: CANERR_NOERROR
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
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

// @gtest TC25/TC26.1: CAN acceptance filter for 29-bit identifier with invalid interface handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC26.2: Give a NULL pointer as argument for parameter 'code and 'mask'
//
// @note: passing pointers for 'code and 'mask' is not possible with the C++ API!

// @gtest TC25/TC26.3: CAN acceptance filter for 29-bit identifier if CAN channel is not initialized
//
// @expected: CANERR_NOTINIT
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    uint32_t codeGet, maskGet;
    uint32_t codeSet, maskSet;
    CANAPI_Return_t retVal;
 
    // @test:
	// @sub(1): 11-bit identifier
    // @- try to get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @- try to set 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeSet = 0x00001000U; maskSet = 0x00000500U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
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

// @gtest TC25/TC26.4: CAN acceptance filter for 29-bit identifier if CAN controller is not started
//
// @expected: CANERR_NOERROR
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(IfControllerNotStarted, GTEST_ENABLED)) {
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

// @gtest TC25/TC26.5: CAN acceptance filter for 29-bit identifier if CAN controller is started
//
// @expected: CANERR_ONLINE
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(IfControllerStarted, GTEST_ENABLED)) {
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
    // @test:
    // @- try to set 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeSet = 0x00001000U; maskSet = 0x00000500U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
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

// @gtest TC25/TC26.6: CAN acceptance filter for 29-bit identifier if CAN controller was previously stopped
//
// @expected: CANERR_NOERROR
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
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

// @gtest TC25/TC26.7: CAN acceptance filter for 29-bit identifier if CAN channel was previously torn down
//
// @expected: CANERR_NOTINIT
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
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
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @test:
    // @- try to get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @- try to set 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeSet = 0x00001000U; maskSet = 0x00000500U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::NotInitialized, retVal);
    // @end.
}

// @gtest TC25.8: CAN acceptance filter for 29-bit identifier with valid values
//
// @expected: CANERR_NOERROR
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(WithValidValues, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_Status_t status = {};
    uint32_t codeGet, maskGet;
    const uint32_t codeSet[4] = { 0x00000000U, 0x000000F0U, 0x0000070FU, 0x000007FFU };
    const uint32_t maskSet[4] = { 0x00000000U, 0x000000F0U, 0x0000070FU, 0x000007FFU };
    CANAPI_Return_t retVal;
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
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @test:
    CCounter counter = CCounter();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            counter.Increment();
            // @-- set 29-bit filter
            retVal = dut1.SetFilter29Bit(codeSet[i], maskSet[j]);
            EXPECT_EQ(CCanApi::NoError, retVal);
            // @-- get 29-bit filter
            codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
            retVal = dut1.GetFilter29Bit(codeGet, maskGet);
            EXPECT_EQ(CCanApi::NoError, retVal);
            EXPECT_EQ(codeSet[i], codeGet);
            EXPECT_EQ(maskSet[j], maskGet);
            // @-- start DUT1 with configured bit-rate settings
            retVal = dut1.StartController();
            EXPECT_EQ(CCanApi::NoError, retVal);
            // @-- check acceptance filter on DUT1 (DUT2 sends frames with 29-bit identifier)
            counter.Clear();
            CheckFilter29Bit(dut1, codeSet[i], maskSet[j]);
            // @-- stop/reset DUT1
            retVal = dut1.ResetController();
            EXPECT_EQ(CCanApi::NoError, retVal);
        }
    }
    counter.Reset();
    // @post:
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

// @gtest TC25.9: CAN acceptance filter for 29-bit identifier with invalid values
//
// @expected: CANERR_ILLPARAM
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(WithInvalidValues, GTEST_ENABLED)) {
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
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @test:
    // @- try to set 29-bit filter with invalid value (code 0xE0000000 and mask 0xFFFFFFFF)
    codeSet = 0xE0000000U; maskSet = 0xFFFFFFFFU;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- try to set 29-bit filter with invalid value (code 0x00000000 and mask 0xFFFFFFFF)
    codeSet = 0x00000000U; maskSet = 0xFFFFFFFFU;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @- try to set 29-bit filter with invalid value (code 0xE0000000 and mask 0x00000000)
    codeSet = 0xE0000000U; maskSet = 0x00000000U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @post:
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
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

// @gtest TC25.10: CAN acceptance filter for 29-bit identifier if extended identifiers are suppressed
//
// @expected: CANERR_ILLPARAM
//
TEST_F(SetFilter29Bit, GTEST_TESTCASE(IfXtdFramesSuppressed, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opMode = { TEST_CANMODE };
    CANAPI_Status_t status = {};
    uint32_t codeGet, maskGet;
    uint32_t codeSet, maskSet;
    CANAPI_Return_t retVal;
    // @pre:
    // @- set operation mode bit NXTD (suppress extended frames)
    opMode = dut1.GetOpMode();
    opMode.nxtd = 1;
    dut1.SetOpMode(opMode);
    //dut1.ShowOperationMode();
    // @test:
    // @- initialize DUT1 with operation mode bit NXTD set
    retVal = dut1.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut1.InitializeChannel() failed with error code " << retVal;
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @test:
    // @- get 29-bit filter (code 0x00000000 and mask 0x00000000)
    codeGet = 0xFFFFFFFFU; maskGet = 0xFFFFFFFFU;
    retVal = dut1.GetFilter29Bit(codeGet, maskGet);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(CANACC_CODE_29BIT, codeGet);
    EXPECT_EQ(CANACC_MASK_29BIT, maskGet);
    // @- try to set 29-bit filter (code 0x00001000 and mask 0x00000500)
    codeSet = 0x00001000U; maskSet = 0x00000500U;
    retVal = dut1.SetFilter29Bit(codeSet, maskSet);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
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

#endif // FEATURE_FILTERING != FEATURE_UNSUPPORTED

//  $Id: TC25_SetFilter29Bit.cc 1272 2024-04-16 19:55:27Z makemake $  Copyright (c) UV Software, Berlin.
