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

class GetProperty : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TC12.0: Get property values (sunnyday scenario)
//
// @expected: CANERR_NOERROR (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(SunnydayScenario, GTEST_SUNNYDAY)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    CCanApi::EChannelState state;
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
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
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        counter.Increment();
        // printf("param=%i (%s)\n", param, testcase.Description());
        // @-- get property value (incl. pre-initialization parameter)
        retVal = dut1.GetProperty(param, (void*)buffer, testcase.SizeOf());
        if (testcase.IsRequired()) {
            EXPECT_EQ(CCanApi::NoError, retVal);
        } else if (retVal != CCanApi::NoError) {
            EXPECT_EQ(CCanApi::NotSupported, retVal);
        }
        // next please
        param = testcase.GetNextEntry();
    }
    counter.Clear();
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
    // @- end
}

// @gtest TC12.1: Get property values with invalid interface handle(s)
//
// @note: checking channel handles is not possible with the C++ API!

// @gtest TC12.2: Give a NULL pointer as argument for parameter 'value'
//
// @expected: CANERR_NULLPTR (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(WithNullPointerForValue, GTEST_ENABLED)) {
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
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        counter.Increment();
        // @-- exclude properties with size zero (they return no value)
        if (testcase.SizeOf() != 0U) {
            // printf("param=%i (%s)\n", param, testcase.Description());
            // @--- try to get the property value (incl. pre-initialization parameter)
            retVal = dut1.GetProperty(param, NULL, testcase.SizeOf());
            if (testcase.IsRequired()) {
                EXPECT_EQ(CCanApi::NullPointer, retVal);
            } else if (retVal != CCanApi::NullPointer) {
                EXPECT_EQ(CCanApi::NotSupported, retVal);
            }
        }
        // next please
        param = testcase.GetNextEntry();
    }
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
    // @- end
}

// @gtest TC12.3: Give an invalid value for parameter 'param'
//
// @expected: CANERR_NOTSUPP
//
TEST_F(GetProperty, GTEST_TESTCASE(WithInvalidParmeterValue, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
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
    CCounter counter = CCounter();
    // @- try to get a property value with invalid value for parameter 'param'
    retVal = dut1.GetProperty(CANPROP_INVALID, (void*)buffer, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NotSupported, retVal);
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
    // @- end
}

// @gtest TC12.4: Get property values with wrong parameter size (too small)
//
// @expected: CANERR_ILLPARA (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(WithWrongParameterSize, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
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
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        // @-- exclude properties with size zero (they return no value)
        if (testcase.SizeOf() != 0U) {
            counter.Increment();
            uint32_t size = CANPROP_MAX_BUFFER_SIZE;
            if ((testcase.SizeOf() != CANPROP_MAX_BUFFER_SIZE) && (param != CANPROP_GET_BUSLOAD))
                size = testcase.SizeOf() - 1U;
            else
                size = 0U;
            // printf("param=%i (%s) with size of %u byte(s)\n", param, testcase.Description(), size);
            // @--- try to get the property value (incl. pre-initialization parameter) with
            // @    parameter 'nbyte' one byte smaller than the size of the property or
            // @    parameter 'nbyte' set to zero if it is a string-type property
            // @    note: property 'CANPROP_GET_BUSLOAD' has type UINT16 but type UINT8 is still
            // @          supported, so we set 'nbyte' to zero to have a buffer that's too small
            retVal = dut1.GetProperty(param, (void*)buffer, size);
            if (testcase.IsRequired()) {
                EXPECT_EQ(CCanApi::IllegalParameter, retVal);
            } else if (retVal != CCanApi::IllegalParameter) {
                EXPECT_EQ(CCanApi::NotSupported, retVal);
            }
        }
        // next please
        param = testcase.GetNextEntry();
    }
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
    // @- end
}

// @gtest TC12.5: Get property values if CAN channel is not initialized
//
// @expected: CANERR_NOTINIT or CANERR_NOERROR if it is a pre-initialization property
// @                       (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(IfChannelNotInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    // @test:
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        counter.Increment();
        // printf("param=%i (%s)\n", param, testcase.Description());
        // @-- get property value (incl. pre-initialization parameter)
        retVal = dut1.GetProperty(param, (void*)buffer, testcase.SizeOf());
        if (testcase.IsPreInit()) {
            if (testcase.IsRequired()) {
                EXPECT_EQ(CCanApi::NoError, retVal);
            } else if (retVal != CCanApi::NoError) {
                EXPECT_EQ(CCanApi::NotSupported, retVal);
            }
        }
        else {
            if (testcase.IsRequired()) {
                EXPECT_EQ(CCanApi::NotInitialized, retVal);
            } else if (retVal != CCanApi::NotInitialized) {
                EXPECT_EQ(CCanApi::NotSupported, retVal);
            }
        }
        // next please
        param = testcase.GetNextEntry();
    }
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

// @gtest TC12.6: Get property values if CAN channel is initialized (but CAN controller not started)
//
// @expected: CANERR_NOERROR (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(IfChannelInitialized, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
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
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        counter.Increment();
        // printf("param=%i (%s)\n", param, testcase.Description());
        // @-- get property value (incl. pre-initialization parameter)
        retVal = dut1.GetProperty(param, (void*)buffer, testcase.SizeOf());
        if (testcase.IsRequired()) {
            EXPECT_EQ(CCanApi::NoError, retVal);
        } else if (retVal != CCanApi::NoError) {
            EXPECT_EQ(CCanApi::NotSupported, retVal);
        }
        // next please
        param = testcase.GetNextEntry();
    }
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
    counter.Clear();
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
    // @- end
}

// @gtest TC12.7: Get property values if CAN controller is started
//
// @expected: CANERR_NOERROR (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(IfControllerStarted, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
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
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        counter.Increment();
        // printf("param=%i (%s)\n", param, testcase.Description());
        // @-- get property value (incl. pre-initialization parameter)
        retVal = dut1.GetProperty(param, (void*)buffer, testcase.SizeOf());
        if (testcase.IsRequired()) {
            EXPECT_EQ(CCanApi::NoError, retVal);
        } else if (retVal != CCanApi::NoError){
            EXPECT_EQ(CCanApi::NotSupported, retVal);
        }
        // next please
        param = testcase.GetNextEntry();
    }
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
    // @- end
}

// @gtest TC12.8: Get property values if CAN controller is stopped
//
// @expected: CANERR_NOERROR (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(IfControllerStopped, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
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
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        counter.Increment();
        // printf("param=%i (%s)\n", param, testcase.Description());
        // @-- get property value (incl. pre-initialization parameter)
        retVal = dut1.GetProperty(param, (void*)buffer, testcase.SizeOf());
        if (testcase.IsRequired()) {
            EXPECT_EQ(CCanApi::NoError, retVal);
        } else if (retVal != CCanApi::NoError) {
            EXPECT_EQ(CCanApi::NotSupported, retVal);
        }
        // next please
        param = testcase.GetNextEntry();
    }
    // @- get status of DUT1 and check to be in INIT state
    retVal = dut1.GetStatus(status);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_TRUE(status.can_stopped);
    // @post:
    counter.Clear();
    // @- tear down DUT1
    retVal = dut1.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @- end
}

// @gtest TC12.9: Get property values if CAN channel is torn down
//
// @expected: CANERR_NOTINIT or CANERR_NOERROR if it is a pre-initialization property
// @                       (or CANERR_NOTSUPP if an optional property is not supported)
//
TEST_F(GetProperty, GTEST_TESTCASE(IfChannelTornDown, GTEST_ENABLED)) {
    CCanDevice dut1 = CCanDevice(TEST_DEVICE(DUT1));
    CCanDevice dut2 = CCanDevice(TEST_DEVICE(DUT2));
    uint8_t buffer[CANPROP_MAX_BUFFER_SIZE];
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
    CProperties testcase = CProperties();
    // @- loop over list of CAN API properties
    CCounter counter = CCounter();
    uint16_t param = testcase.GetFirstEntry();
    while (param != CANPROP_INVALID) {
        counter.Increment();
        // printf("param=%i (%s)\n", param, testcase.Description());
        // @-- get property value (incl. pre-initialization parameter)
        retVal = dut1.GetProperty(param, (void*)buffer, testcase.SizeOf());
        if (testcase.IsPreInit()) {
            if (testcase.IsRequired()) {
                EXPECT_EQ(CCanApi::NoError, retVal);
            } else if (retVal != CCanApi::NoError) {
                EXPECT_EQ(CCanApi::NotSupported, retVal);
            }
        }
        else {
            if (testcase.IsRequired()) {
                EXPECT_EQ(CCanApi::NotInitialized, retVal);
            } else if (retVal != CCanApi::NotInitialized) {
                EXPECT_EQ(CCanApi::NotSupported, retVal);
            }
        }
        // next please
        param = testcase.GetNextEntry();
    }
    counter.Clear();
    // @- end
}


//  $Id: TC12_GetProperty.cc 1201 2023-09-13 11:09:28Z makemake $  Copyright (c) UV Software, Berlin.
