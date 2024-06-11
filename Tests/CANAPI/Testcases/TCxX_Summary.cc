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

#include <inttypes.h>

class Summary : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    // ...
};

// @gtest TCxX.1: Get library information
//
// @expected: CANERR_NOERROR
//
TEST_F(Summary, GTEST_TESTCASE(GetLibraryInformation, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opMode = {};
    CANAPI_Bitrate_t bitrate = {};
    CANAPI_BusSpeed_t speed = {};
    CANAPI_Return_t retVal;

    uint8_t u8Val = 0U;
    uint16_t u16Val = 0U;
    uint32_t u32Val = 0U;
    int32_t i32Val = 0U;
    char string[CANPROP_MAX_BUFFER_SIZE + 1];
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    // @- version of the wrapper specification
    retVal = dut.GetProperty(CANPROP_GET_SPEC, (void*)&u16Val, sizeof(uint16_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "V%u.%u", (u16Val >> 8), (u16Val & 0xFFU));
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("CanApi", string);
    }
    // @- version number of the library
    retVal = dut.GetProperty(CANPROP_GET_VERSION, (void*)&u16Val, sizeof(uint16_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "V%u.%u", (u16Val >> 8), (u16Val & 0xFFU));
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("Version", string);
    }
    // @- patch number of the library
    retVal = dut.GetProperty(CANPROP_GET_PATCH_NO, (void*)&u8Val, sizeof(uint8_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%u", u8Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("PatchNo", string);
    }
    // @- build number of the library
    retVal = dut.GetProperty(CANPROP_GET_BUILD_NO, (void*)&u32Val, sizeof(uint32_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%07" PRIx32 "", u32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("BuildNo", string);
    }
    // @- library id of the library
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_ID, (void*)&i32Val, sizeof(int32_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%" PRIi32 "", i32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("LibraryId", string);
    }
    // @- file name of the library DLL
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_DLLNAME, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("LibraryName", string);
    }
    // @- vendor name of the library
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_VENDOR, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("LibraryVendor", string);
    }
    // @- requested operation mode
    opMode = g_Options.GetOpMode(DUT1);
#if (OPTION_CAN_2_0_ONLY == 0)
    if (opMode.fdoe)
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "CAN FD (FDOE=Yes BRSE=%s)", opMode.brse ? "Yes" : "No");
    else
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "CAN CC (2.0)");
    string[CANPROP_MAX_BUFFER_SIZE] = '\0';
    RecordProperty("OperationMode", string);
#else
    RecordProperty("OperationMode", "CAN CC (2.0)");
#endif
    // @ regested bit-rate settings
    bitrate = g_Options.GetBitrate(DUT1);
    if (CCanDevice::MapBitrate2Speed(bitrate, speed) == CCanApi::NoError) {
#if (OPTION_CAN_2_0_ONLY == 0)
        if (opMode.fdoe) {
            snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%.3f kbps (SP=%.2f%%, SJW=%u)\n",
                speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
                bitrate.btr.nominal.sjw);
            string[CANPROP_MAX_BUFFER_SIZE] = '\0';
            RecordProperty("BusSpeed.Nominal", string);
            snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%.3f kbps (SP=%.2f%%, SJW=%u)\n",
                speed.data.speed / 1000.f, speed.data.samplepoint * 100.f,
                bitrate.btr.data.sjw);
            string[CANPROP_MAX_BUFFER_SIZE] = '\0';
            RecordProperty("BusSpeed.DataPhase", string);
        }
        else {
            snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
                speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
                bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
            string[CANPROP_MAX_BUFFER_SIZE] = '\0';
            RecordProperty("BusSpeed.Nominal", string);
        }
#else
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
            speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
            bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("BusSpeed.Nominal", string);
#endif
    }
    // @end.
}

// @gtest TCxX.2: Get information of DUT1
//
// @expected: CANERR_NOERROR
//
TEST_F(Summary, GTEST_TESTCASE(GetDevice1Information, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT1));
    CANAPI_OpMode_t opCapa = {};
    CANAPI_Return_t retVal;

    uint8_t u8Val = 0U;
    uint16_t u16Val = 0U;
    uint32_t u32Val = 0U;
    int32_t i32Val = 0U;
    char string[CANPROP_MAX_BUFFER_SIZE + 1];
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    // @- initialize DUT1 with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut.InitializeChannel() failed with error code " << retVal;
    // @- version of the wrapper specification
    retVal = dut.GetProperty(CANPROP_GET_SPEC, (void*)&u16Val, sizeof(uint16_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "V%u.%u", (u16Val >> 8), (u16Val & 0xFFU));
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("CanApiVersion", string);
    }
    // @- version number of the library
    retVal = dut.GetProperty(CANPROP_GET_VERSION, (void*)&u16Val, sizeof(uint16_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "V%u.%u", (u16Val >> 8), (u16Val & 0xFFU));
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperVersion", string);
    }
    // @- patch number of the library
    retVal = dut.GetProperty(CANPROP_GET_PATCH_NO, (void*)&u8Val, sizeof(uint8_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%u", u8Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperPatchNo", string);
    }
    // @- build number of the library
    retVal = dut.GetProperty(CANPROP_GET_BUILD_NO, (void*)&u32Val, sizeof(uint32_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%07" PRIx32 "", u32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperBuildNo", string);
    }
    // @- library id of the library
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_ID, (void*)&i32Val, sizeof(int32_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%" PRIi32 "", i32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperLibraryId", string);
    }
    // @- file name of the library DLL
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_DLLNAME, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperLibraryName", string);
    }
    // @- vendor name of the library
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_VENDOR, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperLibraryVendor", string);
    }
    // @- channel id. of the interface
    i32Val = dut.GetChannelNo();
    if (0 <= i32Val) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%" PRIi32 "", i32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceChannelNo", string);
    }
    // @- channel name of the CAN interface
    retVal = dut.GetProperty(CANPROP_GET_DEVICE_NAME, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceChannelName", string);
    }
    // @- file name of the CAN interface DLL
    retVal = dut.GetProperty(CANPROP_GET_DEVICE_DLLNAME, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceLibraryName", string);
    }
    // @- vendor name of the CAN interface
    retVal = dut.GetProperty(CANPROP_GET_DEVICE_VENDOR, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceLibraryVendor", string);
    }
    // @- supported operation capabilities
    retVal = dut.GetOpCapabilities(opCapa);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
#if (OPTION_CAN_2_0_ONLY == 0)
        RecordProperty("DeviceCapability.FDOE", opCapa.fdoe ? "Yes" : "No");
        RecordProperty("DeviceCapability.BRSE", opCapa.brse ? "Yes" : "No");
        RecordProperty("DeviceCapability.NISO", opCapa.niso ? "Yes" : "No");
#endif
        RecordProperty("DeviceCapability.SHRD", opCapa.shrd ? "Yes" : "No");
        RecordProperty("DeviceCapability.NXTD", opCapa.nxtd ? "Yes" : "No");
        RecordProperty("DeviceCapability.NRTR", opCapa.nrtr ? "Yes" : "No");
        RecordProperty("DeviceCapability.ERR", opCapa.err ? "Yes" : "No");
        RecordProperty("DeviceCapability.MON", opCapa.mon ? "Yes" : "No");
    }
    // @- tear down DUT1
    retVal = dut.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

// @gtest TCxX.3: Get information of DUT2
//
// @expected: CANERR_NOERROR
//
TEST_F(Summary, GTEST_TESTCASE(GetDevice2Information, GTEST_ENABLED)) {
    CCanDevice dut = CCanDevice(TEST_DEVICE(DUT2));
    CANAPI_OpMode_t opCapa = {};
    CANAPI_Return_t retVal;

    uint8_t u8Val = 0U;
    uint16_t u16Val = 0U;
    uint32_t u32Val = 0U;
    int32_t i32Val = 0U;
    char string[CANPROP_MAX_BUFFER_SIZE + 1];
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    // @- initialize DUT2 with configured settings
    retVal = dut.InitializeChannel();
    ASSERT_EQ(CCanApi::NoError, retVal) << "[  ERROR!  ] dut.InitializeChannel() failed with error code " << retVal;
    // @- version of the wrapper specification
    retVal = dut.GetProperty(CANPROP_GET_SPEC, (void*)&u16Val, sizeof(uint16_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "V%u.%u", (u16Val >> 8), (u16Val & 0xFFU));
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("CanApiVersion", string);
    }
    // @- version number of the library
    retVal = dut.GetProperty(CANPROP_GET_VERSION, (void*)&u16Val, sizeof(uint16_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "V%u.%u", (u16Val >> 8), (u16Val & 0xFFU));
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperVersion", string);
    }
    // @- patch number of the library
    retVal = dut.GetProperty(CANPROP_GET_PATCH_NO, (void*)&u8Val, sizeof(uint8_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%u", u8Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperPatchNo", string);
    }
    // @- build number of the library
    retVal = dut.GetProperty(CANPROP_GET_BUILD_NO, (void*)&u32Val, sizeof(uint32_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%07" PRIx32 "", u32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperBuildNo", string);
    }
    // @- library id of the library
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_ID, (void*)&i32Val, sizeof(int32_t));
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%" PRIi32 "", i32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperLibraryId", string);
    }
    // @- file name of the library DLL
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_DLLNAME, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperLibraryName", string);
    }
    // @- vendor name of the library
    retVal = dut.GetProperty(CANPROP_GET_LIBRARY_VENDOR, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("WrapperLibraryVendor", string);
    }
    // @- channel id. of the interface
    i32Val = dut.GetChannelNo();
    if (0 <= i32Val) {
        snprintf(string, CANPROP_MAX_BUFFER_SIZE, "%" PRIi32 "", i32Val);
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceChannelNo", string);
    }
    // @- channel name of the CAN interface
    retVal = dut.GetProperty(CANPROP_GET_DEVICE_NAME, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceChannelName", string);
    }
    // @- file name of the CAN interface DLL
    retVal = dut.GetProperty(CANPROP_GET_DEVICE_DLLNAME, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceLibraryName", string);
    }
    // @- vendor name of the CAN interface
    retVal = dut.GetProperty(CANPROP_GET_DEVICE_VENDOR, (void*)string, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
        string[CANPROP_MAX_BUFFER_SIZE] = '\0';
        RecordProperty("DeviceLibraryVendor", string);
    }
    // @- supported operation capabilities
    retVal = dut.GetOpCapabilities(opCapa);
    EXPECT_EQ(CCanApi::NoError, retVal);
    if (CCanApi::NoError == retVal) {
#if (OPTION_CAN_2_0_ONLY == 0)
        RecordProperty("DeviceCapability.FDOE", opCapa.fdoe ? "Yes" : "No");
        RecordProperty("DeviceCapability.BRSE", opCapa.brse ? "Yes" : "No");
        RecordProperty("DeviceCapability.NISO", opCapa.niso ? "Yes" : "No");
#endif
        RecordProperty("DeviceCapability.SHRD", opCapa.shrd ? "Yes" : "No");
        RecordProperty("DeviceCapability.NXTD", opCapa.nxtd ? "Yes" : "No");
        RecordProperty("DeviceCapability.NRTR", opCapa.nrtr ? "Yes" : "No");
        RecordProperty("DeviceCapability.ERR", opCapa.err ? "Yes" : "No");
        RecordProperty("DeviceCapability.MON", opCapa.mon ? "Yes" : "No");
    }
    // @- tear down DUT2
    retVal = dut.TeardownChannel();
    EXPECT_EQ(CCanApi::NoError, retVal);
    // @end.
}

//  $Id: TCxX_Summary.cc 1333 2024-06-01 22:18:12Z makemake $  Copyright (c) UV Software, Berlin.
