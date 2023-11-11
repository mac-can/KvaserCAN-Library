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
#include <math.h>
#include <limits.h>

#ifndef OPTION_CAN_2_0_ONLY
#define OPTION_CAN_2_0_ONLY  OPTION_DISABLED
#ifdef _MSC_VER
#pragma message ( "OPTION_CAN_2_0_ONLY not set, default = OPTION_DISABLED" )
#else
#warning OPTION_CAN_2_0_ONLY not set, default = OPTION_DISABLED
#endif
#endif
#ifndef FEATURE_BITRATE_SJA1000
#define FEATURE_BITRATE_SJA1000  FEATURE_SUPPORTED
#ifdef _MSC_VER
#pragma message ( "FEATURE_BITRATE_SJA1000 not set, default = FEATURE_SUPPORTED" )
#else
#warning FEATURE_BITRATE_SJA1000 not set, default = FEATURE_SUPPORTED
#endif
#endif
#define SHOW_RESULT  0  // set to non-zero valus to see conversion results

#define NOM_BRP_MIN     CANBTR_NOMINAL_BRP_MIN
#define NOM_BRP_MAX     CANBTR_NOMINAL_BRP_MAX
#define NOM_TSEG1_MIN   CANBTR_NOMINAL_TSEG1_MIN
#define NOM_TSEG1_MAX   CANBTR_NOMINAL_TSEG1_MAX
#define NOM_TSEG2_MIN   CANBTR_NOMINAL_TSEG2_MIN
#define NOM_TSEG2_MAX   CANBTR_NOMINAL_TSEG2_MAX
#define NOM_SJW_MIN     CANBTR_NOMINAL_SJW_MIN
#define NOM_SJW_MAX     CANBTR_NOMINAL_SJW_MAX
#define NOM_SAM_SINGLE  CANBTR_NOMINAL_SAM_SINGLE
#define NOM_SAM_TRIPLE  CANBTR_NOMINAL_SAM_TRIPLE

#define DATA_BRP_MIN    CANBTR_DATA_BRP_MIN
#define DATA_BRP_MAX    CANBTR_DATA_BRP_MAX
#define DATA_TSEG1_MIN  CANBTR_DATA_TSEG1_MIN
#define DATA_TSEG1_MAX  CANBTR_DATA_TSEG1_MAX
#define DATA_TSEG2_MIN  CANBTR_DATA_TSEG2_MIN
#define DATA_TSEG2_MAX  CANBTR_DATA_TSEG2_MAX
#define DATA_SJW_MIN    CANBTR_DATA_SJW_MIN
#define DATA_SJW_MAX    CANBTR_DATA_SJW_MAX

#define CLEAR_BTR(btr)  memset(&btr, 0, sizeof(CANAPI_Bitrate_t))
#define CLEAR_BPS(bps)  memset(&bps, 0, sizeof(CANAPI_BusSpeed_t))

class BitrateConverter : public testing::Test {
    virtual void SetUp() {}
    virtual void TearDown() {}
protected:
    bool CheckBitrate(CANAPI_Bitrate_t bitrate, bool data, bool sam) {
        if (bitrate.index <= CANBTR_INDEX_1M) { // CAN 2.0 bit-rate index
            if (bitrate.index < CANBTR_INDEX_5K)
                return false;
        }
        else {                                  // CAN nominal bit-rate settings
            if ((bitrate.btr.nominal.brp < NOM_BRP_MIN) || (NOM_BRP_MAX < bitrate.btr.nominal.brp))
                return false;
            if ((bitrate.btr.nominal.tseg1 < NOM_TSEG1_MIN) || (NOM_TSEG1_MAX < bitrate.btr.nominal.tseg1))
                return false;
            if ((bitrate.btr.nominal.tseg2 < NOM_TSEG2_MIN) || (NOM_TSEG2_MAX < bitrate.btr.nominal.tseg2))
                return false;
            if ((bitrate.btr.nominal.sjw < NOM_SJW_MIN) || (NOM_SJW_MAX < bitrate.btr.nominal.sjw))
                return false;
            if (sam && !data) {                 // CAN 2.0: check SAM
                if ((bitrate.btr.nominal.sam != NOM_SAM_SINGLE) && (NOM_SAM_TRIPLE != bitrate.btr.nominal.sam))
                    return false;
            }
#if (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)
            if (data) {                         // CAN FD data phase settinge
                if ((bitrate.btr.data.brp < DATA_BRP_MIN) || (DATA_BRP_MAX < bitrate.btr.data.brp))
                    return false;
                if ((bitrate.btr.data.tseg1 < DATA_TSEG1_MIN) || (DATA_TSEG1_MAX < bitrate.btr.data.tseg1))
                    return false;
                if ((bitrate.btr.data.tseg2 < DATA_TSEG2_MIN) || (DATA_TSEG2_MAX < bitrate.btr.data.tseg2))
                    return false;
                if ((bitrate.btr.data.sjw < DATA_SJW_MIN) || (DATA_SJW_MAX < bitrate.btr.data.sjw))
                    return false;
            }
#else
            (void)data;
#endif
        }
        return true;
    }
};

// @gtest TCx2.1.1: Check bit-rate with valid index(es)
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.2: Check bit-rate with invalid index(es)
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.3: Check bit-rate with valid CAN 2.0 bit-rate settings
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.4: Check bit-rate with invalid CAN 2.0 bit-rate settings
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.5: Check bit-rate with valid CAN FD bit-rate settings and BRS disabled
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.6: Check bit-rate with invalid CAN FD bit-rate settings and BRS disabled
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.7: Check bit-rate with valid CAN FD bit-rate settings and BRS enabled
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.8: Check bit-rate with invalid CAN FD bit-rate settings and BRS enabled
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.9: Check bit-rate with valid CAN FD bit-rate settings and BRS enabled, but FDO disabled
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.10: Check bit-rate with invalid CAN FD bit-rate settings and BRS enabled, but FDO disabled
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.

// @gtest TCx2.1.11: Check bit-rate with NULL pointer as argument for parameter 'bitrate'
//
// @note: C interface function 'btr_check_bitrate' is not wrapped by the C++ API.
//
// @note: passing a pointer for 'bitrate' is not possible with the C++ API!

// @gtest TCx2.2.1: Compare bit-rates with valid index(es)
//
// @note: C interface function 'btr_compare_bitrates' is not wrapped by the C++ API.

// @gtest TCx2.2.2: Compare bit-rates with invalid index(es)
//
// @note: C interface function 'btr_compare_bitrates' is not wrapped by the C++ API.

// @gtest TCx2.2.3: Compare bit-rates with equal values
//
// @note: C interface function 'btr_compare_bitrates' is not wrapped by the C++ API.

// @gtest TCx2.2.4: Compare bit-rates with unequal values
//
// @note: C interface function 'btr_compare_bitrates' is not wrapped by the C++ API.

// @gtest TCx2.2.5: Compare bit-rates with NULL pointer as argument for parameter 'bitrate1' and 'bitrate2'
//
// @note: C interface function 'btr_compare_bitrates' is not wrapped by the C++ API.
//
// @note: passing a pointer for 'bitrate1' and 'bitrate2' is not possible with the C++ API!

// @gtest TCx2.2.6: Compare bit-rates with division by zero
//
// @note: C interface function 'btr_compare_bitrates' is not wrapped by the C++ API.

// @gtest TCx2.3.1: Map bit-rate to speed with valid index(es)
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToSpeedWithValidIndexes, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    // @- loop over valid indexes to predefined bit-rate table
    // @  note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @        The index must be given as negative value to 'bitrate.index'!
    // @        Remark: The CiA bit-timing table has only 9 entries!
    CCounter counter = CCounter();
    for (int32_t index = CANBTR_INDEX_1M; index >= CANBTR_INDEX_5K; index--) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        bitrate.index = index;
        // @-- convert index to bit-timing table into transmission rate
        retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        // note: just to see what's the conversion result
        CLEAR_BTR(bitrate);
        (void)CCanDevice::MapIndex2Bitrate(index, bitrate);
        printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
            speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
            bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#endif
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.3.2: Map bit-rate to speed with invalid index(es)
//
// @expected: CANERR_BAUDRATE
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToSpeedWithInvalidIndexes, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    CCounter counter = CCounter();
    // @- sub(1): index = -10
    counter.Increment();
    CLEAR_BTR(bitrate);
    CLEAR_BPS(speed);
    bitrate.index = (CANBTR_INDEX_5K - 1);
    // @-- try to convert index to bit-timing table into transmission rate
    retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
    EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
#if (SHOW_RESULT != 0)
    // note: just to see what's the conversion result
    CLEAR_BTR(bitrate);
    (void)CCanDevice::MapIndex2Bitrate((CANBTR_INDEX_5K - 1), bitrate);
    printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
        speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
        bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#endif
    // @- sub(2): index = INT32_MIN
    counter.Increment();
    CLEAR_BTR(bitrate);
    CLEAR_BPS(speed);
    bitrate.index = INT32_MIN;
    // @-- try to convert index to bit-timing table into transmission rate
    retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
    EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
#if (SHOW_RESULT != 0)
    // note: just to see what's the conversion result
    CLEAR_BTR(bitrate);
    (void)CCanDevice::MapIndex2Bitrate(INT32_MIN, bitrate);
    printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
        speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
        bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#endif
    counter.Clear();
    // @end.
}

// @gtest TCx2.3.3: Map bit-rate to speed with valid CAN 2.0 bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToSpeedWithValidCan20Values, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    // @- loop over valid CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 10; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        switch (i) {
        // @- sub(1): CAN 1000 kbps
        case 0: DEFAULT_CAN_BR_1M(bitrate); break;
        // @- sub(2): CAN 800 kbps
        case 1: DEFAULT_CAN_BR_800K(bitrate); break;
        // @- sub(3): CAN 500 kbps
        case 2: DEFAULT_CAN_BR_500K(bitrate); break;
        // @- sub(4): CAN 250 kbps
        case 3: DEFAULT_CAN_BR_250K(bitrate); break;
        // @- sub(5): CAN 125 kbps
        case 4: DEFAULT_CAN_BR_125K(bitrate); break;
        // @- sub(6): CAN 100 kbps
        case 5: DEFAULT_CAN_BR_100K(bitrate); break;
        // @- sub(7): CAN 50 kbps
        case 6: DEFAULT_CAN_BR_50K(bitrate); break;
        // @- sub(8): CAN 20 kbps
        case 7: DEFAULT_CAN_BR_20K(bitrate); break;
        // @- sub(9): CAN 10 kbps
        case 8: DEFAULT_CAN_BR_10K(bitrate); break;
        // @- sub(10): CAN 5 kbps
        case 9: DEFAULT_CAN_BR_5K(bitrate); break;
        default: return;  // Get out of here!
        }
        // @-- convert bit-rate settings into transmission rate
        retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        // note: just to see what's the conversion result
        printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
            speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
            bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#endif
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.3.4: Map bit-rate to speed with invalid CAN 2.0 bit-rate settings
//
// @expected: CANERR_NOERROR (input is not checked for valid ranges)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToSpeedWithInvalidCanValues, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    // @- loop over invalid CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 16; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        DEFAULT_CAN_BR_250K(bitrate);
        switch (i) {
        // @sub(1): CAN 250 kbps, but freq = INT32_MAX MHz
        case 0: bitrate.btr.frequency = INT32_MAX; break;
        // @sub(2): CAN 250 kbps, but brp = 0
        case 1: bitrate.btr.nominal.brp = (NOM_BRP_MIN - 1U); break;
        // @sub(3): CAN 250 kbps, but brp = 1025
        case 2: bitrate.btr.nominal.brp = (NOM_BRP_MAX + 1U); break;
        // @sub(4): CAN 250 kbps, but brp = USHRT_MAX
        case 3: bitrate.btr.nominal.brp = USHRT_MAX; break;
        // @sub(5): CAN 250 kbps, but tseg = 0
        case 4: bitrate.btr.nominal.tseg1 = (NOM_TSEG1_MIN - 1U); break;
        // @sub(6): CAN 250 kbps, but tseg1 = 257
        case 5: bitrate.btr.nominal.tseg1 = (NOM_TSEG1_MAX + 1U); break;
        // @sub(7): CAN 250 kbps, but tseg1 = USHRT_MAX
        case 6: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
        // @sub(8): CAN 250 kbps, but tseg2 = 0
        case 7: bitrate.btr.nominal.tseg2 = (NOM_TSEG2_MIN - 1U); break;
        // @sub(9): CAN 250 kbps, but tseg2 = 129
        case 8: bitrate.btr.nominal.tseg2 = (NOM_TSEG2_MAX + 1U); break;
        // @sub(10): CAN 250 kbps, but tseg2 = USHRT_MAX
        case 9: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
        // @sub(11): CAN 250 kbps, but sjw = 0
        case 10: bitrate.btr.nominal.sjw = (NOM_SJW_MIN - 1U); break;
        // @sub(12): CAN 250 kbps, but sjw = 129
        case 11: bitrate.btr.nominal.sjw = (NOM_SJW_MAX + 1U); break;
        // @sub(13): CAN 250 kbps, but sjw = USHRT_MAX
        case 12: bitrate.btr.nominal.sjw = USHRT_MAX; break;
        // @sub(14): CAN 250 kbps, but sam = 2
        case 13: bitrate.btr.nominal.sam = (NOM_SAM_TRIPLE + 1U); break;
        // @sub(15): CAN 250 kbps, but sam = 3
        case 14: bitrate.btr.nominal.sam = (NOM_SAM_TRIPLE + 2U); break;
        // @sub(16): CAN 250 kbps, but sam = UCHAR_MAX
        case 15: bitrate.btr.nominal.sam = UCHAR_MAX; break;
        default: return;  // Get out of here!
        }
        // @-- convert bit-rate settings into transmission rate
        retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        // note: just to see what's the conversion result
        printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
            speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
            bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#endif
    }
    counter.Clear();
    // @end.
}

#if (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)
// @gtest TCx2.3.5: Map bit-rate to speed with valid CAN FD bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToSpeedWithValidCanFdValues, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    // @- loop over valid CAN FD bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 8; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        switch (i) {
        // @sub(1): nominal 1Mbps (mode FDOE)
        case 0: DEFAULT_CAN_FD_BR_1M(bitrate); break;
        // @sub(2): nominal 500kbps (mode FDOE)
        case 1: DEFAULT_CAN_FD_BR_500K(bitrate); break;
        // @sub(3): nominal 250kbps (mode FDOE)
        case 2: DEFAULT_CAN_FD_BR_250K(bitrate); break;
        // @sub(4): nominal 125kbps (mode FDOE)
        case 3: DEFAULT_CAN_FD_BR_125K(bitrate); break;
        // @sub(5): nominal 1Mbps, data phase 8Mbps (mode FDOE+BRSE)
        case 4: DEFAULT_CAN_FD_BR_1M8M(bitrate); break;
        // @sub(6): nominal 500kbps, data phase 4Mbps (mode FDOE+BRSE)
        case 5: DEFAULT_CAN_FD_BR_500K4M(bitrate); break;
        // @sub(7): nominal 250kbps, data phase 2Mbps (mode FDOE+BRSE)
        case 6: DEFAULT_CAN_FD_BR_250K2M(bitrate); break;
        // @sub(8): nominal 125kbps, data phase 1Mbps (mode FDOE+BRSE)
        case 7: DEFAULT_CAN_FD_BR_125K1M(bitrate); break;
        default: return;  // Get out of here!
        }
        // @-- convert bit-rate settings into transmission rate
        retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        // note: just to see what's the conversion result
        printf("  %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n",
            speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
            speed.data.speed / 1000.f, speed.data.samplepoint * 100.f);
#endif
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.3.6: Map bit-rate to speed with invalid CAN FD bit-rate settings
//
// @expected: CANERR_NOERROR (input is not checked for valid ranges)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToSpeedWithInvalidCanFdValues, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    // @- loop over invalid CAN FD bit-rate settings
    CCounter counter = CCounter();
    for (int i = 0; i < 30; i++) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        DEFAULT_CAN_FD_BR_250K2M(bitrate);
        switch (i) {
        // @sub(1): set all fields to 0 (note: 'frequency' == 0 is CiA Index 0, set to 1 instead)
        case 0: bitrate.btr.frequency = 1;
            bitrate.btr.nominal.brp = 0; bitrate.btr.nominal.tseg1 = 0; bitrate.btr.nominal.tseg2 = 0; bitrate.btr.nominal.sjw = 0; bitrate.btr.nominal.sam = 0;
            bitrate.btr.data.brp = 0; bitrate.btr.data.tseg1 = 0; bitrate.btr.data.tseg2 = 0; bitrate.btr.data.sjw = 0; break;
        // @sub(2): set all fields to MAX value (acc. data type)
        case 1: bitrate.btr.frequency = INT32_MAX;
            bitrate.btr.nominal.brp = UINT16_MAX; bitrate.btr.nominal.tseg1 = UINT16_MAX; bitrate.btr.nominal.tseg2 = UINT16_MAX; bitrate.btr.nominal.sjw = UINT16_MAX; bitrate.btr.nominal.sam = UINT8_MAX;
            bitrate.btr.data.brp = UINT16_MAX; bitrate.btr.data.tseg1 = UINT16_MAX; bitrate.btr.data.tseg2 = UINT16_MAX; bitrate.btr.data.sjw = UINT16_MAX; break;
        // @sub(3): set field 'frequency' to 1 (note: see above)
        case 2: bitrate.btr.frequency = 1; break;
        // @sub(4): set field 'frequency' to INT32_MAX
        case 3: bitrate.btr.frequency = INT32_MAX; break;
        // @sub(5): set field 'brp' to 0
        case 4: bitrate.btr.nominal.brp = 0; break;
        // @sub(6): set field 'brp' to 1025
        case 5: bitrate.btr.nominal.brp = NOM_BRP_MAX + 1; break;
        // @sub(7): set field 'brp' to UINT16_MAX
        case 6: bitrate.btr.nominal.brp = UINT16_MAX; break;
        // @sub(8): set field 'tseg1' to 0
        case 7: bitrate.btr.nominal.tseg1 = 0; break;
        // @sub(9): set field 'tseg1' to 257
        case 8: bitrate.btr.nominal.tseg1 = NOM_TSEG1_MAX + 1; break;
        // @sub(10): set field 'tseg1' to UINT16_MAX
        case 9: bitrate.btr.nominal.tseg1 = UINT16_MAX; break;
        // @sub(11): set field 'tseg2' to 0
        case 10: bitrate.btr.nominal.tseg2 = 0; break;
        // @sub(12): set field 'tseg2' to 129
        case 11: bitrate.btr.nominal.tseg2 = NOM_TSEG2_MAX + 1; break;
        // @sub(13): set field 'tseg2' to UINT16_MAX
        case 12: bitrate.btr.nominal.tseg2 = UINT16_MAX; break;
        // @sub(14): set field 'sjw' to 0
        case 13: bitrate.btr.nominal.sjw = 0; break;
        // @sub(15): set field 'sjw' to 129
        case 14: bitrate.btr.nominal.sjw = NOM_SJW_MAX + 1; break;
        // @sub(16): set field 'sjw' to UINT16_MAX
        case 15: bitrate.btr.nominal.sjw = UINT16_MAX; break;
        // @sub(17): set field 'data.brp' to 0
        case 16: bitrate.btr.data.brp = 0; break;
        // @sub(18): set field 'data.brp' to 1025
        case 17: bitrate.btr.data.brp = DATA_BRP_MAX + 1; break;
        // @sub(19): set field 'data.brp' to UINT16_MAX
        case 18: bitrate.btr.data.brp = UINT16_MAX; break;
        // @sub(20): set field 'data.tseg1' to 0
        case 19: bitrate.btr.data.tseg1 = 0; break;
        // @sub(21): set field 'data.tseg1' to 33
        case 20: bitrate.btr.data.tseg1 = DATA_TSEG1_MAX + 1; break;
        // @sub(22): set field 'data.tseg1' to UINT16_MAX
        case 21: bitrate.btr.data.tseg1 = UINT16_MAX; break;
        // @sub(23): set field 'data.tseg2' to 0
        case 22: bitrate.btr.data.tseg2 = 0; break;
        // @sub(24): set field 'data.tseg2' to 17
        case 23: bitrate.btr.data.tseg2 = DATA_TSEG2_MAX + 1; break;
        // @sub(25): set field 'data.tseg2' to UINT16_MAX
        case 24: bitrate.btr.data.tseg2 = UINT16_MAX; break;
        // @sub(26): set field 'data.sjw' to 0
        case 25: bitrate.btr.data.sjw = 0; break;
        // @sub(27): set field 'data.sjw' to 17
        case 26: bitrate.btr.data.sjw = DATA_SJW_MAX + 1; break;
        // @sub(28): set field 'data.sjw' to UINT16_MAX
        case 27: bitrate.btr.data.sjw = UINT16_MAX; break;
        // @sub(29): set field 'sam' to 2 (optional)
        case 28: bitrate.btr.nominal.sam = 2; break;
        // @sub(30): set field 'sam' to UINT8_MAX (optional)
        case 29: bitrate.btr.nominal.sam = UINT8_MAX; break;
        default: return;  // Get out of here!
        }
        // @-- convert bit-rate settings into transmission rate
        retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        // note: just to see what's the conversion result
        printf("  %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n",
            speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
            speed.data.speed / 1000.f, speed.data.samplepoint * 100.f);
#endif
    }
    counter.Clear();
    // @end.
}
#endif  // (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)

// @gtest TCx2.3.7: Map bit-rate to speed with NULL pointer as argument for parameter 'bitrate' and 'speed'
//
// @note: passing a pointer for 'bitrate' and 'speed' is not possible with the C++ API!

// @gtest TCx2.3.8: Map bit-rate to speed with division by zero
//
// @expected: CANERR_NOERROR (input is not checked for valid ranges)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToSpeedWithDivisonByZero, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    CCounter counter = CCounter();
    // @- sub(1): CAN 2.0 250 kbps with nominal BRP set to zero
    counter.Increment();
    CLEAR_BTR(bitrate);
    CLEAR_BPS(speed);
    BITRATE_250K(bitrate);
    bitrate.btr.nominal.brp = 0;
    // @-- convert bit-rate settings into transmission rate
    retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(speed.nominal.speed, INFINITY);
#if (SHOW_RESULT != 0)
    // note: just to see what's the conversion result
    printf("  %.3f kbps @ %.1f%%\n",
        speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f);
#endif
#if (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)
    // @- sub(2): CAN FD 250 kbps : 2 Mbps with data BRP set to zero
    counter.Increment();
    CLEAR_BTR(bitrate);
    CLEAR_BPS(speed);
    DEFAULT_CAN_FD_BR_250K2M(bitrate);
    bitrate.btr.data.brp = 0;
    // @-- convert bit-rate settings into transmission rate
    retVal = CCanDevice::MapBitrate2Speed(bitrate, speed);
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(speed.data.speed, INFINITY);
#if (SHOW_RESULT != 0)
    // note: just to see what's the conversion result
    printf("  %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n",
        speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
        speed.data.speed / 1000.f, speed.data.samplepoint * 100.f);
#endif
#endif
    counter.Clear();
    // @end.
}

// @gtest TCx2.4.1: Map index to bit-rate with valid index(es)
//
// @expected: CANERR_NOERROR
//
#if (FEATURE_BITRATE_SJA1000 != FEATURE_UNSUPPORTED)
#define GTEST_TCx2_4_1_ENABLED  GTEST_ENABLED
#else
#define GTEST_TCx2_4_1_ENABLED  GTEST_DISABLED
#endif
TEST_F(BitrateConverter, GTEST_TESTCASE(IndexToBitrateWithValidIndexes, GTEST_TCx2_4_1_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    CANAPI_Bitrate_t predefined[10];
    SJA1000_BR_1M(predefined[0]);
    SJA1000_BR_800K(predefined[1]);
    SJA1000_BR_500K(predefined[2]);
    SJA1000_BR_250K(predefined[3]);
    SJA1000_BR_125K(predefined[4]);
    SJA1000_BR_100K(predefined[5]);
    SJA1000_BR_50K(predefined[6]);
    SJA1000_BR_20K(predefined[7]);
    SJA1000_BR_10K(predefined[8]);
    SJA1000_BR_5K(predefined[9]);
    // @- loop over valid indexes to predefined bit-rate table
    // @  note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @        The index must be given as negative value to 'bitrate.index'!
    // @        Remark: The CiA bit-timing table has only 9 entries!
    CCounter counter = CCounter();
    for (int32_t index = CANBTR_INDEX_1M; index >= CANBTR_INDEX_5K; index--) {
        counter.Increment();
        CLEAR_BTR(bitrate);
        CLEAR_BPS(speed);
        // @-- convert index to bit-timing table into bit-rate settings
        retVal = CCanDevice::MapIndex2Bitrate(index, bitrate);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        // note: just to see what's the conversion result
        (void)CCanDevice::MapBitrate2Speed(bitrate, speed);
        printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
            speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
            bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#else
        (void)speed;
#endif
        // @-- check result against its predefined bit-rate table entry
        EXPECT_EQ(predefined[-index].btr.frequency, bitrate.btr.frequency);
        EXPECT_EQ(predefined[-index].btr.nominal.brp, bitrate.btr.nominal.brp);
        EXPECT_EQ(predefined[-index].btr.nominal.tseg1, bitrate.btr.nominal.tseg1);
        EXPECT_EQ(predefined[-index].btr.nominal.tseg2, bitrate.btr.nominal.tseg2);
        EXPECT_EQ(predefined[-index].btr.nominal.sjw, bitrate.btr.nominal.sjw);
        EXPECT_EQ(predefined[-index].btr.nominal.sam, bitrate.btr.nominal.sam);
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.4.2: Map index to bit-rate with invalid index(es)
//
// @expected: CANERR_BAUDRATE
//
TEST_F(BitrateConverter, GTEST_TESTCASE(IndexToBitrateWithInvalidIndexes, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_BusSpeed_t speed;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    CCounter counter = CCounter();
    // @- sub(1): index = -10
    counter.Increment();
    CLEAR_BTR(bitrate);
    CLEAR_BPS(speed);
    // @-- try to convert index to bit-timing table into bit-rate settings
    retVal = CCanDevice::MapIndex2Bitrate((CANBTR_INDEX_5K - 1), bitrate);
    EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
#if (SHOW_RESULT != 0)
    // note: just to see what's the conversion result
    (void)CCanDevice::MapBitrate2Speed(bitrate, speed);
    printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
        speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
        bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#else
    (void)speed;
#endif
    // @- sub(2): index = INT32_MIN
    counter.Increment();
    CLEAR_BTR(bitrate);
    CLEAR_BPS(speed);
    // @-- try to convert index to bit-timing table into bit-rate settings
    retVal = CCanDevice::MapIndex2Bitrate(INT32_MIN, bitrate);
    EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
#if (SHOW_RESULT != 0)
    // note: just to see what's the conversion result
    (void)CCanDevice::MapBitrate2Speed(bitrate, speed);
    printf("  %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n",
        speed.nominal.speed / 1000.f, speed.nominal.samplepoint * 100.f,
        bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
#else
    (void)speed;
#endif
    counter.Clear();
    // @end.
}

// @gtest TCx2.4.3: Map index to bit-rate with NULL pointer as argument for parameter 'btr0btr1'
//
// @note: passing a pointer for 'btr0btr1' is not possible with the C++ API!

// @gtest TCx2.5.1: Map bit-rate to index with valid values
//
// @note: C interface function 'btr_bitrate2index' is not wrapped by the C++ API.

// @gtest TCx2.5.2: Map bit-rate to index with invalid values
//
// @note: C interface function 'btr_bitrate2index' is not wrapped by the C++ API.

// @gtest TCx2.5.3: Map bit-rate to index NULL pointer as argument for parameter 'bitrate' and 'index'
//
// @note: C interface function 'btr_bitrate2index' is not wrapped by the C++ API.
//
// @note: passing a pointer for 'bitrate' and 'index' is not possible with the C++ API!

// @gtest TCx2.6.1: Scan bit-rate strings from well-formed strings
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(StringToBitrateFromCorrectStrings, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_Return_t retVal;
    char* string = NULL;
    bool data, sam;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    CBitrates testcase = CBitrates();
    // @- loop over list of well-formed test strings
    CCounter counter = CCounter();
    string = (char*)testcase.GetFirstEntry(true);
    while (string) {
        counter.Increment();
#if (SHOW_RESULT != 0)
        printf(" %s\n", string);
#endif
        // @-- convert string into bit-rate settings
        retVal = CCanDevice::MapString2Bitrate(string, bitrate, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // get next correct bit-rate string
        string = (char*)testcase.GetNextEntry(true);
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.6.2: Scan bit-rate strings from valid strings (in range)
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(StringToBitrateFromValidStrings, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_Return_t retVal;
    char* string = NULL;
    bool data, sam;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    CBitrates testcase = CBitrates();
    // @- loop over list of well-formed test strings
    CCounter counter = CCounter();
    string = (char*)testcase.GetFirstEntry(true);
    while (string) {
        // @-- if the string is well-formed and in range:
        if (testcase.IsInRange()) {
            counter.Increment();
#if (SHOW_RESULT != 0)
            printf(" %s\n", string);
#endif
            // @-- convert string into bit-rate settings
            retVal = CCanDevice::MapString2Bitrate(string, bitrate, data, sam);
            EXPECT_EQ(CCanApi::NoError, retVal);
            // @-- and check result for valid ranges (should pass)
            EXPECT_TRUE(CheckBitrate(bitrate, data, sam));
        }
        // get next correct bit-rate string
        string = (char*)testcase.GetNextEntry(true);
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.6.3: Scan bit-rate strings from invalid strings (out of range)
//
// @expected: CANERR_NOERROR (but 'can_start' would return CANERR_BAUDRATE)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(StringToBitrateFromInvalidStrings, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_Return_t retVal;
    char* string = NULL;
    bool data, sam;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    CBitrates testcase = CBitrates();
    // @- loop over list of well-formed test strings
    CCounter counter = CCounter();
    string = (char*)testcase.GetFirstEntry(true);
    while (string) {
        // @-- if the string is well-formed, but out of range:
        if (!testcase.IsInRange()) {
            counter.Increment();
#if (SHOW_RESULT != 0)
            printf(" %s\n", string);
#endif
            // @-- convert string into bit-rate settings
            retVal = CCanDevice::MapString2Bitrate(string, bitrate, data, sam);
            EXPECT_EQ(CCanApi::NoError, retVal);
            // @-- and check result for valid ranges (should fail)
            EXPECT_FALSE(CheckBitrate(bitrate, data, sam));
        }
        // get next correct bit-rate string
        string = (char*)testcase.GetNextEntry(true);
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.6.4: Scan bit-rate strings from wrong strings
//
// @expected: CANERR_ILLPARA
//
TEST_F(BitrateConverter, GTEST_TESTCASE(StringToBitrateFromWrongStrings, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_Return_t retVal;
    char* string = NULL;
    bool data, sam;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    CBitrates testcase = CBitrates();
    // @- loop over list of not well-formed test strings
    CCounter counter = CCounter();
    string = (char*)testcase.GetFirstEntry(false);
    while (string) {
        counter.Increment();
#if (SHOW_RESULT != 0)
        printf(" %s\n", string);
#endif
        // @-- convert string into bit-rate settings
        retVal = CCanDevice::MapString2Bitrate(string, bitrate, data, sam);
        EXPECT_EQ(CCanApi::IllegalParameter, retVal);
        // get next correct bit-rate string
        string = (char*)testcase.GetNextEntry(false);
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.6.5: Scan bit-rate strings with NULL pointer as argument for parameter 'string', 'bitrate', 'data' and 'sam'
//
// @note: passing a pointer for 'bitrate', 'data' and 'sam' is not possible with the C++ API!
//
// @expected: CANERR_NULLPTR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(StringToBitrateWithNullPointerForString, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_Return_t retVal;
    char* string = NULL;
    bool data, sam;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    // @- try to convert with NULL pointer for 'string'
    retVal = CCanDevice::MapString2Bitrate(string, bitrate, data, sam);
    EXPECT_EQ(CCanApi::NullPointer, retVal);
    // @end.
}

// @gtest TCx2.7.1: Print bit-rate strings from valid index(es)
//
// @expected: CANERR_NOERROR
//
#if (FEATURE_BITRATE_SJA1000 != FEATURE_UNSUPPORTED)
#define GTEST_TCx2_7_1_ENABLED  GTEST_ENABLED
#else
#define GTEST_TCx2_7_1_ENABLED  GTEST_DISABLED
#endif
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromValidIndexes, GTEST_TCx2_7_1_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- loop over valid indexes to predefined bit-rate table
    // @  note: predefined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    // @        The index must be given as negative value to 'bitrate.index'!
    // @        Remark: The CiA bit-timing table has only 9 entries!
    CCounter counter = CCounter();
    bool data, sam;
    for (int32_t index = CANBTR_INDEX_1M; index >= CANBTR_INDEX_5K; index--) {
        counter.Increment();
        CLEAR_BTR(source);
        CLEAR_BTR(target);
        source.index = index;
        // @- convert index to bit-timing table to string
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, false, true);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert index to bit-timing table into bit-rate settings (source)
        retVal = CCanDevice::MapIndex2Bitrate(index, source);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- convert string into bit-rate settings (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate settings
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.7.2: Print bit-rate strings from invalid index(es)
//
// @expected: CANERR_BAUDRATE
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromInvalidIndexes, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate;
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test: 
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    CCounter counter = CCounter();
    // @- sub(1): index = -10
    counter.Increment();
    CLEAR_BTR(bitrate);
    bitrate.index = (CANBTR_INDEX_5K - 1);
    // @-- try to convert index to bit-timing table to string
    retVal = CCanDevice::MapBitrate2String(bitrate, buffer, CANPROP_MAX_BUFFER_SIZE, false, true);
    EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
#if (SHOW_RESULT != 0)
    printf(" %s\n", buffer);
#endif
    // @- sub(2): index = INT32_MIN
    counter.Increment();
    CLEAR_BTR(bitrate);
    bitrate.index = INT32_MIN;
    // @-- try to convert index to bit-timing table to string
    retVal = CCanDevice::MapBitrate2String(bitrate, buffer, CANPROP_MAX_BUFFER_SIZE, false, true);
    EXPECT_EQ(CCanApi::InvalidBaudrate, retVal);
#if (SHOW_RESULT != 0)
    printf(" %s\n", buffer);
#endif
    counter.Clear();
    // @end.
}

// @gtest TCx2.7.3: Print bit-rate strings from valid CAN 2.0 bit-rate settings
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromValidCanValues, GTEST_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- loop over valid CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    bool data, sam;
    for (int i = 0; i < 10; i++) {
        counter.Increment();
        CLEAR_BTR(source);
        CLEAR_BTR(target);
        switch (i) {
        // @- sub(1): CAN 1000 kbps
        case 0: DEFAULT_CAN_BR_1M(source); break;
        // @- sub(2): CAN 800 kbps
        case 1: DEFAULT_CAN_BR_800K(source); break;
        // @- sub(3): CAN 500 kbps
        case 2: DEFAULT_CAN_BR_500K(source); break;
        // @- sub(4): CAN 250 kbps
        case 3: DEFAULT_CAN_BR_250K(source); break;
        // @- sub(5): CAN 125 kbps
        case 4: DEFAULT_CAN_BR_125K(source); break;
        // @- sub(6): CAN 100 kbps
        case 5: DEFAULT_CAN_BR_100K(source); break;
        // @- sub(7): CAN 50 kbps
        case 6: DEFAULT_CAN_BR_50K(source); break;
        // @- sub(8): CAN 20 kbps
        case 7: DEFAULT_CAN_BR_20K(source); break;
        // @- sub(9): CAN 10 kbps
        case 8: DEFAULT_CAN_BR_10K(source); break;
        // @- sub(10): CAN 5 kbps
        case 9: DEFAULT_CAN_BR_5K(source); break;
        default: return;  // Get out of here!
        }
        // @- convert index to bit-timing table to string
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, false, true);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert string into bit-rate settings (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate settings
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.7.4: Print bit-rate strings from invalid CAN 2.0 bit-rate settings
//
// @expected: CANERR_NOERROR (input is not checked for valid ranges)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromInvalidCanValues, GTEST_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- loop over invalid CAN 2.0 bit-rate settings
    CCounter counter = CCounter();
    bool data, sam;
    for (int i = 0; i < 16; i++) {
        counter.Increment();
        CLEAR_BTR(source);
        CLEAR_BTR(target);
        DEFAULT_CAN_BR_250K(source);
        switch (i) {
        // @sub(1): CAN 250 kbps, but freq = INT32_MAX MHz
        case 0: source.btr.frequency = INT32_MAX; break;
        // @sub(2): CAN 250 kbps, but brp = 0
        case 1: source.btr.nominal.brp = (NOM_BRP_MIN - 1U); break;
        // @sub(3): CAN 250 kbps, but brp = 1025
        case 2: source.btr.nominal.brp = (NOM_BRP_MAX + 1U); break;
        // @sub(4): CAN 250 kbps, but brp = USHRT_MAX
        case 3: source.btr.nominal.brp = USHRT_MAX; break;
        // @sub(5): CAN 250 kbps, but tseg = 0
        case 4: source.btr.nominal.tseg1 = (NOM_TSEG1_MIN - 1U); break;
        // @sub(6): CAN 250 kbps, but tseg1 = 257
        case 5: source.btr.nominal.tseg1 = (NOM_TSEG1_MAX + 1U); break;
        // @sub(7): CAN 250 kbps, but tseg1 = USHRT_MAX
        case 6: source.btr.nominal.tseg1 = USHRT_MAX; break;
        // @sub(8): CAN 250 kbps, but tseg2 = 0
        case 7: source.btr.nominal.tseg2 = (NOM_TSEG2_MIN - 1U); break;
        // @sub(9): CAN 250 kbps, but tseg2 = 129
        case 8: source.btr.nominal.tseg2 = (NOM_TSEG2_MAX + 1U); break;
        // @sub(10): CAN 250 kbps, but tseg2 = USHRT_MAX
        case 9: source.btr.nominal.tseg2 = USHRT_MAX; break;
        // @sub(11): CAN 250 kbps, but sjw = 0
        case 10: source.btr.nominal.sjw = (NOM_SJW_MIN - 1U); break;
        // @sub(12): CAN 250 kbps, but sjw = 129
        case 11: source.btr.nominal.sjw = (NOM_SJW_MAX + 1U); break;
        // @sub(13): CAN 250 kbps, but sjw = USHRT_MAX
        case 12: source.btr.nominal.sjw = USHRT_MAX; break;
        // @sub(14): CAN 250 kbps, but sam = 2
        case 13: source.btr.nominal.sam = (NOM_SAM_TRIPLE + 1U); break;
        // @sub(15): CAN 250 kbps, but sam = 3
        case 14: source.btr.nominal.sam = (NOM_SAM_TRIPLE + 2U); break;
        // @sub(16): CAN 250 kbps, but sam = UCHAR_MAX
        case 15: source.btr.nominal.sam = UCHAR_MAX; break;
        default: return;  // Get out of here!
        }
        // @- convert index to bit-timing table to string
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, false, true);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert string into bit-rate settings (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate settings
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
    }
    counter.Clear();
    // @end.
}

#if (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)
// @gtest TCx2.7.5: Print bit-rate strings from valid CAN FD bit-rate settings and BRS disabled
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromValidCanFdValuesBrsDisabled, GTEST_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- loop over valid CAN FD bit-rate settings
    CCounter counter = CCounter();
    bool data, sam;
    for (int i = 0; i < 4; i++) {
        counter.Increment();
        CLEAR_BTR(source);
        CLEAR_BTR(target);
        switch (i) {
        // @sub(1): nominal 1Mbps (mode FDOE)
        case 0: DEFAULT_CAN_FD_BR_1M(source); break;
        // @sub(2): nominal 500kbps (mode FDOE)
        case 1: DEFAULT_CAN_FD_BR_500K(source); break;
        // @sub(3): nominal 250kbps (mode FDOE)
        case 2: DEFAULT_CAN_FD_BR_250K(source); break;
        // @sub(4): nominal 125kbps (mode FDOE)
        case 3: DEFAULT_CAN_FD_BR_125K(source); break;
        default: return;  // Get out of here!
        }
        // @- convert index to bit-timing table to string
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, false, false);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert string into bit-rate settings (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate settings
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.7.6: Print bit-rate strings from invalid CAN FD bit-rate settings and BRS disabled
//
// @expected: CANERR_NOERROR (input is not checked for valid ranges)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromInvalidCanFdValuesBrsDisabled, GTEST_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- loop over valid CAN FD bit-rate settings
    CCounter counter = CCounter();
    bool data, sam;
    for (int i = 0; i < 18; i++) {
        counter.Increment();
        CLEAR_BTR(source);
        CLEAR_BTR(target);
        DEFAULT_CAN_FD_BR_250K(source);
        switch (i) {
        // @sub(1): set all fields to 0 (note: 'frequency' == 0 is CiA Index 0, set to 1 instead)
        case 0: source.btr.frequency = 1;
                source.btr.nominal.brp = 0;
                source.btr.nominal.tseg1 = 0;
                source.btr.nominal.tseg2 = 0;
                source.btr.nominal.sjw = 0;
                source.btr.nominal.sam = 0;
            break;
        // @sub(2): set all fields to MAX value (acc. data type)
        case 1: source.btr.frequency = INT32_MAX;
                source.btr.nominal.brp = UINT16_MAX;
                source.btr.nominal.tseg1 = UINT16_MAX;
                source.btr.nominal.tseg2 = UINT16_MAX;
                source.btr.nominal.sjw = UINT16_MAX;
                source.btr.nominal.sam = UINT8_MAX;
            break;
        // @sub(3): set field 'frequency' to 1 (note: see above)
        case 2: source.btr.frequency = 1; break;
        // @sub(4): set field 'frequency' to INT32_MAX
        case 3: source.btr.frequency = INT32_MAX; break;
        // @sub(5): set field 'brp' to 0
        case 4: source.btr.nominal.brp = 0; break;
        // @sub(6): set field 'brp' to 1025
        case 5: source.btr.nominal.brp = NOM_BRP_MAX + 1; break;
        // @sub(7): set field 'brp' to UINT16_MAX
        case 6: source.btr.nominal.brp = UINT16_MAX; break;
        // @sub(8): set field 'tseg1' to 0
        case 7: source.btr.nominal.tseg1 = 0; break;
        // @sub(9): set field 'tseg1' to 257
        case 8: source.btr.nominal.tseg1 = NOM_TSEG1_MAX + 1; break;
        // @sub(10): set field 'tseg1' to UINT16_MAX
        case 9: source.btr.nominal.tseg1 = UINT16_MAX; break;
        // @sub(11): set field 'tseg2' to 0
        case 10: source.btr.nominal.tseg2 = 0; break;
        // @sub(12): set field 'tseg2' to 129
        case 11: source.btr.nominal.tseg2 = NOM_TSEG2_MAX + 1; break;
        // @sub(13): set field 'tseg2' to UINT16_MAX
        case 12: source.btr.nominal.tseg2 = UINT16_MAX; break;
        // @sub(14): set field 'sjw' to 0
        case 13: source.btr.nominal.sjw = 0; break;
        // @sub(15): set field 'sjw' to 129
        case 14: source.btr.nominal.sjw = NOM_SJW_MAX + 1; break;
        // @sub(16): set field 'sjw' to UINT16_MAX
        case 15: source.btr.nominal.sjw = UINT16_MAX; break;
        // @sub(17): set field 'sam' to 2 (optional)
        case 16: source.btr.nominal.sam = 2; break;
        // @sub(18): set field 'sam' to UINT8_MAX (optional)
        case 17: source.btr.nominal.sam = UINT8_MAX; break;
        default: return;  // Get out of here!
        }
        // @- convert index to bit-timing table to string
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, false, true);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert string into bit-rate settings (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate settings
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.7.7: Print bit-rate strings from valid CAN FD bit-rate settings and BRS enabled
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromValidCanFdValuesBrsEnabled, GTEST_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- loop over valid CAN FD bit-rate settings
    CCounter counter = CCounter();
    bool data, sam;
    for (int i = 0; i < 4; i++) {
        counter.Increment();
        CLEAR_BTR(source);
        CLEAR_BTR(target);
        switch (i) {
        // @sub(1): nominal 1Mbps, data phase 8Mbps (mode FDOE+BRSE)
        case 0: DEFAULT_CAN_FD_BR_1M8M(source); break;
        // @sub(2): nominal 500kbps, data phase 4Mbps (mode FDOE+BRSE)
        case 1: DEFAULT_CAN_FD_BR_500K4M(source); break;
        // @sub(3): nominal 250kbps, data phase 2Mbps (mode FDOE+BRSE)
        case 2: DEFAULT_CAN_FD_BR_250K2M(source); break;
        // @sub(4): nominal 125kbps, data phase 1Mbps (mode FDOE+BRSE)
        case 3: DEFAULT_CAN_FD_BR_125K1M(source); break;
        default: return;  // Get out of here!
        }
        // @- convert index to bit-timing table to string
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, true, false);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert string into bit-rate settings (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate settings
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.7.8: Print bit-rate strings from invalid CAN FD bit-rate settings and BRS enabled
//
// @expected: CANERR_NOERROR (input is not checked for valid ranges)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromInvalidCanFdValuesBrsEnabled, GTEST_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- loop over valid CAN FD bit-rate settings
    CCounter counter = CCounter();
    bool data, sam;
    for (int i = 0; i < 30; i++) {
        counter.Increment();
        CLEAR_BTR(source);
        CLEAR_BTR(target);
        DEFAULT_CAN_FD_BR_250K2M(source);
        switch (i) {
        // @sub(1): set all fields to 0 (note: 'frequency' == 0 is CiA Index 0, set to 1 instead)
        case 0: source.btr.frequency = 1;
                source.btr.nominal.brp = 0;
                source.btr.nominal.tseg1 = 0;
                source.btr.nominal.tseg2 = 0;
                source.btr.nominal.sjw = 0;
                source.btr.nominal.sam = 0;
                source.btr.data.brp = 0;
                source.btr.data.tseg1 = 0;
                source.btr.data.tseg2 = 0;
                source.btr.data.sjw = 0;
            break;
        // @sub(2): set all fields to MAX value (acc. data type)
        case 1: source.btr.frequency = INT32_MAX;
                source.btr.nominal.brp = UINT16_MAX;
                source.btr.nominal.tseg1 = UINT16_MAX;
                source.btr.nominal.tseg2 = UINT16_MAX;
                source.btr.nominal.sjw = UINT16_MAX;
                source.btr.nominal.sam = UINT8_MAX;
                source.btr.data.brp = UINT16_MAX;
                source.btr.data.tseg1 = UINT16_MAX;
                source.btr.data.tseg2 = UINT16_MAX;
                source.btr.data.sjw = UINT16_MAX;
                break;
        // @sub(3): set field 'frequency' to 1 (note: see above)
        case 2: source.btr.frequency = 1; break;
        // @sub(4): set field 'frequency' to INT32_MAX
        case 3: source.btr.frequency = INT32_MAX; break;
        // @sub(5): set field 'brp' to 0
        case 4: source.btr.nominal.brp = 0; break;
        // @sub(6): set field 'brp' to 1025
        case 5: source.btr.nominal.brp = NOM_BRP_MAX + 1; break;
        // @sub(7): set field 'brp' to UINT16_MAX
        case 6: source.btr.nominal.brp = UINT16_MAX; break;
        // @sub(8): set field 'tseg1' to 0
        case 7: source.btr.nominal.tseg1 = 0; break;
        // @sub(9): set field 'tseg1' to 257
        case 8: source.btr.nominal.tseg1 = NOM_TSEG1_MAX + 1; break;
        // @sub(10): set field 'tseg1' to UINT16_MAX
        case 9: source.btr.nominal.tseg1 = UINT16_MAX; break;
        // @sub(11): set field 'tseg2' to 0
        case 10: source.btr.nominal.tseg2 = 0; break;
        // @sub(12): set field 'tseg2' to 129
        case 11: source.btr.nominal.tseg2 = NOM_TSEG2_MAX + 1; break;
        // @sub(13): set field 'tseg2' to UINT16_MAX
        case 12: source.btr.nominal.tseg2 = UINT16_MAX; break;
        // @sub(14): set field 'sjw' to 0
        case 13: source.btr.nominal.sjw = 0; break;
        // @sub(15): set field 'sjw' to 129
        case 14: source.btr.nominal.sjw = NOM_SJW_MAX + 1; break;
        // @sub(16): set field 'sjw' to UINT16_MAX
        case 15: source.btr.nominal.sjw = UINT16_MAX; break;
        // @sub(17): set field 'data.brp' to 0
        case 16: source.btr.data.brp = 0; break;
        // @sub(18): set field 'data.brp' to 1025
        case 17: source.btr.data.brp = DATA_BRP_MAX + 1; break;
        // @sub(19): set field 'data.brp' to UINT16_MAX
        case 18: source.btr.data.brp = UINT16_MAX; break;
        // @sub(20): set field 'data.tseg1' to 0
        case 19: source.btr.data.tseg1 = 0; break;
        // @sub(21): set field 'data.tseg1' to 33
        case 20: source.btr.data.tseg1 = DATA_TSEG1_MAX + 1; break;
        // @sub(22): set field 'data.tseg1' to UINT16_MAX
        case 21: source.btr.data.tseg1 = UINT16_MAX; break;
        // @sub(23): set field 'data.tseg2' to 0
        case 22: source.btr.data.tseg2 = 0; break;
        // @sub(24): set field 'data.tseg2' to 17
        case 23: source.btr.data.tseg2 = DATA_TSEG2_MAX + 1; break;
        // @sub(25): set field 'data.tseg2' to UINT16_MAX
        case 24: source.btr.data.tseg2 = UINT16_MAX; break;
        // @sub(26): set field 'data.sjw' to 0
        case 25: source.btr.data.sjw = 0; break;
        // @sub(27): set field 'data.sjw' to 17
        case 26: source.btr.data.sjw = DATA_SJW_MAX + 1; break;
        // @sub(28): set field 'data.sjw' to UINT16_MAX
        case 27: source.btr.data.sjw = UINT16_MAX; break;
#if (FEATURE_DEFAULT_CAN_BR_FD_SAM != FEATURE_SUPPORTED)
        // @sub(29): set field 'sam' to 2 (optional)
        case 28: source.btr.nominal.sam = 2; break;
        // @sub(30): set field 'sam' to UINT8_MAX (optional)
        case 29: source.btr.nominal.sam = UINT8_MAX; break;
#endif
        default: return;  // Get out of here!
        }
        // @- convert index to bit-timing table to string
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, true, true);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert string into bit-rate settings (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate settings
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
    }
    counter.Clear();
    // @end.
}
#endif  // (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)

// @gtest TCx2.7.9: Print bit-rate to strings from well-formed bit-rate strings
//
// @expected: CANERR_NOERROR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringFromWellFormedStrings, GTEST_ENABLED)) {
    CANAPI_Bitrate_t source = {};
    CANAPI_Bitrate_t target = {};
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    CBitrates testcase = CBitrates();
    // @- loop over list of well-formed test strings
    CCounter counter = CCounter();
    bool data, sam;
    char* string = (char*)testcase.GetFirstEntry(true);
    while (string) {
        counter.Increment();
        // @-- convert string into bit-rate (source)
        retVal = CCanDevice::MapString2Bitrate(string, source, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @- convert bit-rate to string (from source)
        retVal = CCanDevice::MapBitrate2String(source, buffer, CANPROP_MAX_BUFFER_SIZE, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
#if (SHOW_RESULT != 0)
        printf(" %s\n", buffer);
#endif
        // @-- convert string into bit-rate (target)
        retVal = CCanDevice::MapString2Bitrate(buffer, target, data, sam);
        EXPECT_EQ(CCanApi::NoError, retVal);
        // @-- and compare with (source) bit-rate
        EXPECT_TRUE(CCanDevice::CompareBitrates(source, target, data));
        // get next correct bit-rate string
        string = (char*)testcase.GetNextEntry(true);
    }
    counter.Clear();
    // @end.
}

// @gtest TCx2.7.10: Print bit-rate strings with buffer size too small
//
// @expected: CANERR_NOERROR (but resulting string is truncated)
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringWithBufferSizeTooSmall, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_250K };
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    size_t length = CANPROP_MAX_BUFFER_SIZE - 1U;
    // @sub(1): only one character plus zero-terminator
    length = 1U;
    // @- convert bit-rate to string (string length is 1)
    retVal = CCanDevice::MapBitrate2String(bitrate, buffer, (length + 1U));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(length, strlen(buffer));
    // @sub(2): only space for zero-terminator
    length = 0U;
    // @- convert bit-rate to string (string length is 0)
    retVal = CCanDevice::MapBitrate2String(bitrate, buffer, (length + 1U));
    EXPECT_EQ(CCanApi::NoError, retVal);
    EXPECT_EQ(length, strlen(buffer));
    // @end.
}

// @gtest TCx2.7.11: Print bit-rate strings with buffer size zero
//
// @expected: CANERR_ILLPARA
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringWithBufferSizeZero, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_250K };
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    char buffer[CANPROP_MAX_BUFFER_SIZE] = "";
    // @- try to convert with buffer size zero
    retVal = CCanDevice::MapBitrate2String(bitrate, buffer, 0);
    EXPECT_EQ(CCanApi::IllegalParameter, retVal);
    // @end.
}

// @gtest TCx2.7.12: Print bit-rate strings with NULL pointer as argument for parameter 'bitrate' and 'string'
//
// @note: passing a pointer for 'bitrate' is not possible with the C++ API!
//
// @expected: CANERR_NULLPTR
//
TEST_F(BitrateConverter, GTEST_TESTCASE(BitrateToStringWithNullPointerForString, GTEST_ENABLED)) {
    CANAPI_Bitrate_t bitrate = { CANBTR_INDEX_250K };
    CANAPI_Return_t retVal;
    // @
    // @note: This test is optional!
    if (!g_Options.RunTestBitrateConverter())
        GTEST_SKIP() << "This test is optional: '--run_all=YES'";
    // @test:
    // @- try to convert with NULL pointer for 'string'
    retVal = CCanDevice::MapBitrate2String(bitrate, NULL, CANPROP_MAX_BUFFER_SIZE);
    EXPECT_EQ(CCanApi::NullPointer, retVal);
    // @end.
}

// @gtest TCx2.8.1: Map SJA1000 register to bit-rate with valid values
//
// @note: C interface function 'btr_sja10002bitrate' is not wrapped by the C++ API.

// @gtest TCx2.8.2: Map SJA1000 register to bit-rate with suspective values
//
// @note: C interface function 'btr_sja10002bitrate' is not wrapped by the C++ API.

// @gtest TCx2.8.3: Map SJA1000 register to bit-rate with NULL pointer as argument for parameter 'bitrate'
//
// @note: C interface function 'btr_sja10002bitrate' is not wrapped by the C++ API.
//
// @note: passing a pointer for 'bitrate' is not possible with the C++ API!

// @gtest TCx2.9.1: Map bit-rate to SJA1000 register with valid values
//
// @note: C interface function '' is not wrapped by the C++ API.

// @gtest TCx2.9.2: Map bit-rate to SJA1000 register with invalid values
//
// @note: C interface function 'btr_bitrate2sja1000' is not wrapped by the C++ API.

// @gtest TCx2.9.3: Map bit-rate to SJA1000 register with NULL pointer as argument for parameter 'bitrate' and 'btr0btr1'
//
// @note: C interface function 'btr_bitrate2sja1000' is not wrapped by the C++ API.
//
// @note: passing a pointer for 'bitrate' and 'btr0btr1' is not possible with the C++ API!

// @gtest TCx2.10.1: Map index to SJA1000 register with valid index(es)
//
// @note: C interface function 'btr_index2sja1000' is not wrapped by the C++ API.

// @gtest TCx2.10.2: Map index to SJA1000 register with invalid index(es)
//
// @note: C interface function 'btr_index2sja1000' is not wrapped by the C++ API.

// @gtest TCx2.10.3: Map index to SJA1000 register with NULL pointer as argument for parameter 'btr0btr1'
//
// @note: C interface function 'btr_index2sja1000' is not wrapped by the C++ API.
//
// @note: passing a pointer for 'btr0btr1' is not possible with the C++ API!

//  $Id: TCx2_BitrateConverter.cc 1218 2023-10-14 12:18:19Z makemake $  Copyright (c) UV Software, Berlin.
