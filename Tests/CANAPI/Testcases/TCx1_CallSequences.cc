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

class CallSequences : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TCx1.1: Call sequence 'Initialize-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.2: Call sequence 'Initialize-Teardown-Initialize-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeTeardownInitializeTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.3: Call sequence 'Initialize-Teardown-Initialize-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeTeardownInitializeStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.4: Call sequence 'Initialize-Teardown-Initialize-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeTeardownInitializeStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.5: Call sequence 'Initialize-Teardown-Initialize-Start-Reset-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeTeardownInitializeStartResetStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.6: Call sequence 'Initialize-Teardown-Initialize-Start-Reset-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeTeardownInitializeStartResetStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.7: Call sequence 'Initialize-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.8: Call sequence 'Initialize-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.9: Call sequence 'Initialize-Start-Reset-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.10: Call sequence 'Initialize-Start-Reset-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.11: Call sequence 'Initialize-Start-Teardown-Initialize-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartTeardownInitializeTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.12: Call sequence 'Initialize-Start-Reset-Teardown-Initialize-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetTeardownInitializeTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
     // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.13: Call sequence 'Initialize-Start-Teardown-Initialize-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartTeardownInitializeStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.14: Call sequence 'Initialize-Start-Reset-Teardown-Initialize-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetTeardownInitializeStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
	// @end.
}

// @gtest TCx1.15: Call sequence 'Initialize-Start-Reset-Teardown-Initialize-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetTeardownInitializeStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
	// @end.
}

// @gtest TCx1.16: Call sequence 'Initialize-Start-Teardown-Initialize-Start-Reset-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartTeardownInitializeStartResetStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.17: Call sequence 'Initialize-Start-Reset-Teardown-Initialize-Start-Reset-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetTeardownInitializeStartResetStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.18: Call sequence 'Initialize-Start-Reset-Teardown-Initialize-Start-Reset-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetTeardownInitializeStartResetStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.19: Call sequence 'Initialize-Start-Reset-Start-Teardown-Initialize-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartTeardownInitializeTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.20: Call sequence 'Initialize-Start-Reset-Start-Reset-Teardown-Initialize-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartResetTeardownInitializeTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.21: Call sequence 'Initialize-Start-Reset-Start-Teardown-Initialize-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartTeardownInitializeStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.22: Call sequence 'Initialize-Start-Reset-Start-Reset-Teardown-Initialize-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartResetTeardownInitializeStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.23: Call sequence 'Initialize-Start-Reset-Start-Reset-Teardown-Initialize-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartResetTeardownInitializeStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.24: Call sequence 'Initialize-Start-Reset-Start-Teardown-Initialize-Start-Reset-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartTeardownInitializeStartResetStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.25: Call sequence 'Initialize-Start-Reset-Start-Reset-Teardown-Initialize-Start-Reset-Start-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartResetTeardownInitializeStartResetStartTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.26: Call sequence 'Initialize-Start-Reset-Start-Reset-Teardown-Initialize-Start-Reset-Start-Reset-Teardown'
//
// @expected: CANERR_NOERROR
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetStartResetTeardownInitializeStartResetStartResetTeardown, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT again with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT again with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.27: Call sequence 'Initialize-Start-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- try to initialize DUT again
    EXPECT_NE(CCanApi::NoError, dut.InitializeChannel());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.28: Call sequence 'Initialize-Start-Reset-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeStartResetInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- start DUT with configured bit rate
    EXPECT_EQ(CCanApi::NoError, dut.StartController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_FALSE(status.can_stopped);
    // @- stop/reset DUT
    EXPECT_EQ(CCanApi::NoError, dut.ResetController());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- try to initialize DUT again
    EXPECT_NE(CCanApi::NoError, dut.InitializeChannel());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.29: Call sequence 'Initialize-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- try to initialize DUT again
    EXPECT_NE(CCanApi::NoError, dut.InitializeChannel());
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.30: Call sequence 'Initialize-Teardown-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(InitializeTeardownTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- try to tear down DUT again
    EXPECT_NE(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.31: Call sequence 'Teardown-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(TeardownInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- try to tear down DUT before initialization
    EXPECT_NE(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.32: Call sequence 'Start-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(StartInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- try to start DUT before initialization
    EXPECT_NE(CCanApi::NoError, dut.StartController());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.33: Call sequence 'Reset-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(ResetInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- try to stop DUT before initialization
    EXPECT_NE(CCanApi::NoError, dut.ResetController());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.34: Call sequence 'Write-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(WriteInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    CANAPI_Message_t msg = {};
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- try to send a message before initialization
    EXPECT_NE(CCanApi::NoError, dut.WriteMessage(msg));
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

// @gtest TCx1.35: Call sequence 'Read-Initialize-Teardown'
//
// @expected: !CANERR_NOERROR
//
// @disabled: This test is redundant!
//
TEST_F(CallSequences, GTEST_TESTCASE(ReadInitializeTeardown, GTEST_DISABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_Return_t retVal = CCanApi::FatalError;
    CANAPI_Status_t status = { CANSTAT_RESET };
    CANAPI_Message_t msg = {};
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestCallSequences())
        GTEST_SKIP() << "This test is optional: '--run_callsequences=YES'";
    // @test:
    // @- try to read a message before initialization
    EXPECT_NE(CCanApi::NoError, dut.ReadMessage(msg));
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @- initialize DUT with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(retVal, CCanApi::NoError) << "[  ERROR!  ] dut1.InitializeChannel() failed w/ return code " << retVal;
    EXPECT_EQ(CCanApi::NoError, dut.GetStatus(status));
    EXPECT_TRUE(status.can_stopped);
    // @- tear down DUT
    EXPECT_EQ(CCanApi::NoError, dut.TeardownChannel());
    EXPECT_NE(CCanApi::NoError, dut.GetStatus(status));
    // @end.
}

//  $Id: TCx1_CallSequences.cc 1134 2023-08-06 17:58:59Z haumea $  Copyright (c) UV Software, Berlin.
