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
#import "Settings.h"
#import "Bitrates.h"
#import "can_btr.h"
#import <XCTest/XCTest.h>

#ifndef CAN_FD_SUPPORTED
#define CAN_FD_SUPPORTED  FEATURE_SUPPORTED
#warning CAN_FD_SUPPORTED not set, default=FEATURE_SUPPORTED
#endif

#define CAN_BR_1M(x)         DEFAULT_CAN_BR_1M(x)
#define CAN_BR_800K(x)       DEFAULT_CAN_BR_800K(x)
#define CAN_BR_500K(x)       DEFAULT_CAN_BR_500K(x)
#define CAN_BR_250K(x)       DEFAULT_CAN_BR_250K(x)
#define CAN_BR_125K(x)       DEFAULT_CAN_BR_125K(x)
#define CAN_BR_100K(x)       DEFAULT_CAN_BR_100K(x)
#define CAN_BR_50K(x)        DEFAULT_CAN_BR_50K(x)
#define CAN_BR_20K(x)        DEFAULT_CAN_BR_20K(x)
#define CAN_BR_10K(x)        DEFAULT_CAN_BR_10K(x)
#define CAN_BR_5K(x)         DEFAULT_CAN_BR_5K(x)

#define CAN_FD_BR_1M(x)      DEFAULT_CAN_FD_BR_1M(x)
#define CAN_FD_BR_500K(x)    DEFAULT_CAN_FD_BR_500K(x)
#define CAN_FD_BR_250K(x)    DEFAULT_CAN_FD_BR_250K(x)
#define CAN_FD_BR_125K(x)    DEFAULT_CAN_FD_BR_125K(x)
#define CAN_FD_BR_1M8M(x)    DEFAULT_CAN_FD_BR_1M8M(x)
#define CAN_FD_BR_500K4M(x)  DEFAULT_CAN_FD_BR_500K4M(x)
#define CAN_FD_BR_250K2M(x)  DEFAULT_CAN_FD_BR_250K2M(x)
#define CAN_FD_BR_125K1M(x)  DEFAULT_CAN_FD_BR_125K1M(x)

@interface test_can_btr : XCTestCase

@end

@implementation test_can_btr

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

// @xctest TC0B.1.1: call 'btr_check_bitrate' with valid indexes
//
// @expected BTRERR_NOERROR
//
- (void)testCheckBitrateWithValidIndexes {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid indexes to predefined bit-rate table
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bzero(&bitrate, sizeof(btr_bitrate_t));
        bitrate.index = -index;
        // @-- check the bit-rate settings (index)
        rc = btr_check_bitrate(&bitrate, false, false);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        bzero(&bitrate, sizeof(btr_bitrate_t));
        (void)btr_index2bitrate(-index, &bitrate);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", index+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
    }
}

// @xctest TC0B.1.2: call 'btr_check_bitrate' with invalid indexes
//
// @expected BTRERR_BAUDRATE
//
- (void)testCheckBitrateWithInvalidIndexes {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: index = -10
    bzero(&bitrate, sizeof(btr_bitrate_t));
    bitrate.index = -BTR_SJA1000_ENTRIES;
    // @-- check the bit-rate settings (index)
    rc = btr_check_bitrate(&bitrate, false, false);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // note: just to see what's the conversion result
    bzero(&bitrate, sizeof(btr_bitrate_t));
    (void)btr_index2bitrate(-BTR_SJA1000_ENTRIES, &bitrate);
    NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", 1,
          speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
          bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
    // @test: index = INT32_MIN
    bzero(&bitrate, sizeof(btr_bitrate_t));
    bitrate.index = INT32_MIN;
    // @-- check the bit-rate settings (index)
    rc = btr_check_bitrate(&bitrate, false, false);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // note: just to see what's the conversion result
    bzero(&bitrate, sizeof(btr_bitrate_t));
    (void)btr_index2bitrate(INT32_MIN, &bitrate);
    NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", 2,
          speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
          bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
}

// @xctest TC0B.1.3: call 'btr_check_bitrate' with valid CAN 2.0 bit-rate settings
//
// @expected BTRERR_NOERROR
//
- (void)testCheckBitrateWithValidCan20Values {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over valid bit-rate settings
    for (int i = 0; i < 12; i++) {
        CAN_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 kbps, but freq = 1
            case 0: bitrate.btr.frequency = 1; break;
            // @sub(2): CAN 250 kbps, but freq = INT32_MAX
            case 1: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(3): CAN 250 kbps, but brp = 1
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN; break;
            // @sub(4): CAN 250 kbps, but brp = 102
            case 3: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX; break;
            // @sub(5): CAN 250 kbps, but tseg = 1
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN; break;
            // @sub(6): CAN 250 kbps, but tseg1 = 256
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX; break;
            // @sub(7): CAN 250 kbps, but tseg2 = 1
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN; break;
            // @sub(8): CAN 250 kbps, but tseg2 = 128
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX; break;
            // @sub(9): CAN 250 kbps, but sjw = 1
            case 8: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN; break;
            // @sub(10): CAN 250 kbps, but sjw = 128
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX; break;
            // @sub(11): CAN 250 kbps, but sam = 0
            case 10: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_SINGLE; break;
            // @sub(12): CAN 250 kbps, but sam = 1
            case 11: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, false, false);
        XCTAssertEqual(BTRERR_NOERROR, rc);
    }
}

// @xctest TC0B.1.4: call 'btr_check_bitrate' with invalid CAN 2.0 bit-rate settings
//
// @expected BTRERR_BAUDRATE
//
- (void)testCheckBitrateWithInvalidCan20Values {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 15; i++) {
        CAN_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 kbps, but brp = 0
            case 0: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(2): CAN 250 kbps, but brp = 1025
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(3): CAN 250 kbps, but brp = USHRT_MAX
            case 2: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(4): CAN 250 kbps, but tseg = 0
            case 3: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(5): CAN 250 kbps, but tseg1 = 257
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(6): CAN 250 kbps, but tseg1 = USHRT_MAX
            case 5: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(7): CAN 250 kbps, but tseg2 = 0
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(8): CAN 250 kbps, but tseg2 = 129
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(9): CAN 250 kbps, but tseg2 = USHRT_MAX
            case 8: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(10): CAN 250 kbps, but sjw = 0
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(11): CAN 250 kbps, but sjw = 129
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(12): CAN 250 kbps, but sjw = USHRT_MAX
            case 11: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(13): CAN 250 kbps, but sam = 2
            case 12: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 1U; break;
            // @sub(14): CAN 250 kbps, but sam = 3
            case 13: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 2U; break;
            // @sub(15): CAN 250 kbps, but sam = UCHAR_MAX
            case 14: bitrate.btr.nominal.sam = UCHAR_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, false, false);
        XCTAssertEqual(BTRERR_BAUDRATE, rc);
    }
}

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.1.5: call 'btr_check_bitrate' with valid CAN FD bit-rate settings and BRS disabled
//
// @expected BTRERR_NOERROR
//
- (void)testCheckBitrateWithValidCanFdValuesBrsDisabled {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over valid bit-rate settings
    for (int i = 0; i < 11; i++) {
        CAN_FD_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN FD 250 kbps, but freq = 1
            case 0: bitrate.btr.frequency = 1; break;
            // @sub(2): CAN FD 250 kbps, but freq = INT32_MAX
            case 1: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(3): CAN FD 250 kbps, but brp = 1
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN; break;
            // @sub(4): CAN FD 250 kbps, but brp = 102
            case 3: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX; break;
            // @sub(5): CAN FD 250 kbps, but tseg = 1
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN; break;
            // @sub(6): CAN FD 250 kbps, but tseg1 = 256
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX; break;
            // @sub(7): CAN FD 250 kbps, but tseg2 = 1
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN; break;
            // @sub(8): CAN FD 250 kbps, but tseg2 = 128
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX; break;
            // @sub(9): CAN FD 250 kbps, but sjw = 1
            case 8: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN; break;
            // @sub(10): CAN FD 250 kbps, but sjw = 128
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX; break;
            // @sub(11): CAN FD 250 kbps, but sam = unspecific
            case 10: bitrate.btr.nominal.sam = UCHAR_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, true, false);
        XCTAssertEqual(BTRERR_NOERROR, rc);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.1.6: call 'btr_check_bitrate' with invalid CAN FD bit-rate settings and BRS disabled
//
// @expected BTRERR_BAUDRATE
//
- (void)testCheckBitrateWithInvalidCanFdValuesBrsDisabled {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 12; i++) {
        CAN_FD_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 kbps, but brp = 0
            case 0: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(2): CAN 250 kbps, but brp = 1025
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(3): CAN 250 kbps, but brp = USHRT_MAX
            case 2: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(4): CAN 250 kbps, but tseg = 0
            case 3: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(5): CAN 250 kbps, but tseg1 = 257
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(6): CAN 250 kbps, but tseg1 = USHRT_MAX
            case 5: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(7): CAN 250 kbps, but tseg2 = 0
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(8): CAN 250 kbps, but tseg2 = 129
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(9): CAN 250 kbps, but tseg2 = USHRT_MAX
            case 8: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(10): CAN 250 kbps, but sjw = 0
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(11): CAN 250 kbps, but sjw = 129
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(12): CAN 250 kbps, but sjw = USHRT_MAX
            case 11: bitrate.btr.nominal.sjw = USHRT_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, true, false);
        XCTAssertEqual(BTRERR_BAUDRATE, rc);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.1.7: call 'btr_check_bitrate' with valid CAN FD bit-rate settings and BRS enabled
//
// @expected BTRERR_NOERROR
//
- (void)testCheckBitrateWithValidCanFdValuesBrsEnabled {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over valid bit-rate settings
    for (int i = 0; i < 19; i++) {
        CAN_FD_BR_250K2M(bitrate);
        switch (i) {
            // @sub(1): CAN FD 250 / 2000 kbps, but freq = 1
            case 0: bitrate.btr.frequency = 1; break;
            // @sub(2): CAN FD 250 / 2000 kbps, but freq = INT32_MAX
            case 1: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(3): CAN FD 250 / 2000 kbps, but brp = 1
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN; break;
            // @sub(4): CAN FD 250 / 2000 kbps, but brp = 1024
            case 3: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX; break;
            // @sub(5): CAN FD 250 / 2000 kbps, but tseg = 1
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN; break;
            // @sub(6): CAN FD 250 / 2000 kbps, but tseg1 = 256
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX; break;
            // @sub(7): CAN FD 250 / 2000 kbps, but tseg2 = 1
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN; break;
            // @sub(8): CAN FD 250 / 2000 kbps, but tseg2 = 128
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX; break;
            // @sub(9): CAN FD 250 / 2000 kbps, but sjw = 1
            case 8: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN; break;
            // @sub(10): CAN FD 250 / 2000 kbps, but sjw = 128
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX; break;
            // @sub(11): CAN FD 250 / 2000 kbps, but sam = unspecific
            case 10: bitrate.btr.nominal.sam = UCHAR_MAX; break;
            // @sub(12): CAN FD 250 / 2000 kbps, but data brp = 1
            case 11: bitrate.btr.data.brp = BTR_DATA_BRP_MIN; break;
            // @sub(13): CAN FD 250 / 2000 kbps, but data brp = 1024
            case 12: bitrate.btr.data.brp = BTR_DATA_BRP_MAX; break;
            // @sub(14): CAN FD 250 / 2000 kbps, but data tseg = 1
            case 13: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MIN; break;
            // @sub(15): CAN FD 250 / 2000 kbps, but data tseg1 = 32
            case 14: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MAX; break;
            // @sub(16): CAN FD 250 / 2000 kbps, but data tseg2 = 1
            case 15: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MIN; break;
            // @sub(17): CAN FD 250 / 2000 kbps, but data tseg2 = 16
            case 16: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MAX; break;
            // @sub(18): CAN FD 250 / 2000 kbps, but data sjw = 1
            case 17: bitrate.btr.data.sjw = BTR_DATA_SJW_MIN; break;
            // @sub(19): CAN FD 250 / 2000 kbps, but data sjw = 16
            case 18: bitrate.btr.data.sjw = BTR_DATA_SJW_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, true, true);
        XCTAssertEqual(BTRERR_NOERROR, rc);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.1.8: call 'btr_check_bitrate' with invalid CAN FD bit-rate settings and BRS enabled
//
// @expected BTRERR_BAUDRATE
//
- (void)testCheckBitrateWithInvalidCanFdValuesBrsEnabled {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 24; i++) {
        CAN_FD_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 / 2000 kbps, but brp = 0
            case 0: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(2): CAN 250 / 2000 kbps, but brp = 1025
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(3): CAN 250 / 2000 kbps, but brp = USHRT_MAX
            case 2: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(4): CAN 250 / 2000 kbps, but tseg = 0
            case 3: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(5): CAN 250 / 2000 kbps, but tseg1 = 257
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(6): CAN 250 / 2000 kbps, but tseg1 = USHRT_MAX
            case 5: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(7): CAN 250 / 2000 kbps, but tseg2 = 0
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(8): CAN 250 / 2000 kbps, but tseg2 = 129
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(9): CAN 250 / 2000 kbps, but tseg2 = USHRT_MAX
            case 8: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(10): CAN 250 / 2000 kbps, but sjw = 0
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(11): CAN 250 / 2000 kbps, but sjw = 129
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(12): CAN 250 / 2000 kbps, but sjw = USHRT_MAX
            case 11: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(13): CAN 250 / 2000 kbps, but brp = 0
            case 12: bitrate.btr.data.brp = BTR_DATA_BRP_MIN - 1U; break;
            // @sub(14): CAN 250 / 2000 kbps, but data brp = 1025
            case 13: bitrate.btr.data.brp = BTR_DATA_BRP_MAX + 1U; break;
            // @sub(15): CAN 250 / 2000 kbps, but data brp = USHRT_MAX
            case 14: bitrate.btr.data.brp = USHRT_MAX; break;
            // @sub(16): CAN 250 / 2000 kbps, but data tseg = 0
            case 15: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MIN - 1U; break;
            // @sub(17): CAN 250 / 2000 kbps, but data tseg1 = 257
            case 16: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MAX + 1U; break;
            // @sub(18): CAN 250 / 2000 kbps, but data tseg1 = USHRT_MAX
            case 17: bitrate.btr.data.tseg1 = USHRT_MAX; break;
            // @sub(19): CAN 250 / 2000 kbps, but data tseg2 = 0
            case 18: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MIN - 1U; break;
            // @sub(20): CAN 250 / 2000 kbps, but data tseg2 = 129
            case 19: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MAX + 1U; break;
            // @sub(21): CAN 250 / 2000 kbps, but data tseg2 = USHRT_MAX
            case 20: bitrate.btr.data.tseg2 = USHRT_MAX; break;
            // @sub(22): CAN 250 / 2000 kbps, but data sjw = 0
            case 21: bitrate.btr.data.sjw = BTR_DATA_SJW_MIN - 1U; break;
            // @sub(23): CAN 250 / 2000 kbps, but data sjw = 129
            case 22: bitrate.btr.data.sjw = BTR_DATA_SJW_MAX + 1U; break;
            // @sub(24): CAN 250 / 2000 kbps, but data sjw = USHRT_MAX
            case 23: bitrate.btr.data.sjw = USHRT_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, true, true);
        XCTAssertEqual(BTRERR_BAUDRATE, rc);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.1.9: call 'btr_check_bitrate' with valid CAN FD bit-rate settings and BRS enabled, but FDO disabled
//
// @expected BTRERR_NOERROR
//
- (void)testCheckBitrateWithValidCanFdValuesBrsEnabledFdoNot {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over valid bit-rate settings
    for (int i = 0; i < 19; i++) {
        CAN_FD_BR_250K2M(bitrate);
        switch (i) {
            // @sub(1): CAN FD 250 / 2000 kbps, but freq = 1
            case 0: bitrate.btr.frequency = 1; break;
            // @sub(2): CAN FD 250 / 2000 kbps, but freq = INT32_MAX
            case 1: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(3): CAN FD 250 / 2000 kbps, but brp = 1
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN; break;
            // @sub(4): CAN FD 250 / 2000 kbps, but brp = 1024
            case 3: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX; break;
            // @sub(5): CAN FD 250 / 2000 kbps, but tseg = 1
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN; break;
            // @sub(6): CAN FD 250 / 2000 kbps, but tseg1 = 256
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX; break;
            // @sub(7): CAN FD 250 / 2000 kbps, but tseg2 = 1
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN; break;
            // @sub(8): CAN FD 250 / 2000 kbps, but tseg2 = 128
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX; break;
            // @sub(9): CAN FD 250 / 2000 kbps, but sjw = 1
            case 8: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN; break;
            // @sub(10): CAN FD 250 / 2000 kbps, but sjw = 128
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX; break;
            // @sub(11): CAN FD 250 / 2000 kbps, but sam = 1
            case 10: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE; break;
            // @sub(12): CAN FD 250 / 2000 kbps, but data brp = 0
            case 11: bitrate.btr.data.brp = BTR_DATA_BRP_MIN - 1U; break;
            // @sub(13): CAN FD 250 / 2000 kbps, but data brp = 1025
            case 12: bitrate.btr.data.brp = BTR_DATA_BRP_MAX + 1U; break;
            // @sub(14): CAN FD 250 / 2000 kbps, but data tseg = 0
            case 13: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MIN - 1U; break;
            // @sub(15): CAN FD 250 / 2000 kbps, but data tseg1 = 33
            case 14: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MAX + 1U; break;
            // @sub(16): CAN FD 250 / 2000 kbps, but data tseg2 = 0
            case 15: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MIN - 1U; break;
            // @sub(17): CAN FD 250 / 2000 kbps, but data tseg2 = 17
            case 16: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MAX + 1U; break;
            // @sub(18): CAN FD 250 / 2000 kbps, but data sjw = 0
            case 17: bitrate.btr.data.sjw = BTR_DATA_SJW_MIN - 1U; break;
            // @sub(19): CAN FD 250 / 2000 kbps, but data sjw = 17
            case 18: bitrate.btr.data.sjw = BTR_DATA_SJW_MAX + 1U; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, false, true);
        XCTAssertEqual(BTRERR_NOERROR, rc);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.1.10: call 'btr_check_bitrate' with invalid CAN FD bit-rate settings and BRS enabled, but FDO disabled
//
// @expected BTRERR_BAUDRATE
//
- (void)testCheckBitrateWithInvalidFdValuesBrsEnabledFdoNot {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 12; i++) {
        CAN_FD_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 / 2000 kbps, but brp = 0
            case 0: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(2): CAN 250 / 2000 kbps, but brp = 1025
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(3): CAN 250 / 2000 kbps, but brp = USHRT_MAX
            case 2: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(4): CAN 250 / 2000 kbps, but tseg = 0
            case 3: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(5): CAN 250 / 2000 kbps, but tseg1 = 257
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(6): CAN 250 / 2000 kbps, but tseg1 = USHRT_MAX
            case 5: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(7): CAN 250 / 2000 kbps, but tseg2 = 0
            case 6: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(8): CAN 250 / 2000 kbps, but tseg2 = 129
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(9): CAN 250 / 2000 kbps, but tseg2 = USHRT_MAX
            case 8: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(10): CAN 250 / 2000 kbps, but sjw = 0
            case 9: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(11): CAN 250 / 2000 kbps, but sjw = 129
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(12): CAN 250 / 2000 kbps, but sjw = USHRT_MAX
            case 11: bitrate.btr.nominal.sjw = USHRT_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, false, true);
        XCTAssertEqual(BTRERR_BAUDRATE, rc);
    }
}
#endif

// @xctest TC0B.1.11: call 'btr_check_bitrate' with NULL pointer for 'bitrate'
//
// @expected BTRERR_NULLPTR
//
- (void)testCheckBitrateWithNullPointer {
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_check_bitrate' with NULL pointer for 'bitrate'
    rc = btr_check_bitrate(NULL, false, false);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    rc = btr_check_bitrate(NULL, true, false);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    rc = btr_check_bitrate(NULL, true, true);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.2.x: call 'btr_compare_bitrates' with valid indexes
//
// @expected 0 if bit-rate of index1 is equal to bit-rate of index2, or a none-zero value otherwise
//
- (void)testCompareBitratesWithValidIndexes {
    btr_bitrate_t bitrate1;
    btr_bitrate_t bitrate2;
    // @test: 1 Mbps > 250 kbps
    bitrate1.index = BTR_INDEX_1M;
    bitrate2.index = BTR_INDEX_250K;
    // @- call 'btr_check_bitrate' if 'bitrate1' is greater than 'bitrate2'
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false), 0);
    // @test: 250 kbps < 1 Mbps
    bitrate1.index = BTR_INDEX_250K;
    bitrate2.index = BTR_INDEX_1M;
    // @- call 'btr_check_bitrate' if 'bitrate1' is less than 'bitrate2'
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true), 0);
    // @test: 250 kbps == 250 kbps
    bitrate1.index = BTR_INDEX_250K;
    bitrate2.index = BTR_INDEX_250K;
    // @- call 'btr_check_bitrate' if 'bitrate1' is equal to 'bitrate2'
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
}

// @xctest TC0B.2.x: call 'btr_compare_bitrates' with invalid indexes
//
// @expected 0 if bit-rate of index1 is equal to bit-rate of index2, or a none-zero value otherwise
//
- (void)testCompareBitratesWithInvalidIndexes {
    btr_bitrate_t bitrate1;
    btr_bitrate_t bitrate2;
    // @test: 1 Mbps > invalid
    bitrate1.index = BTR_INDEX_1M;
    bitrate2.index = INT32_MIN;
    // @- call 'btr_check_bitrate' if 'bitrate1' is greater than 'bitrate2'
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false), 0);
    // @test: invalid < 1 Mbps
    bitrate1.index = INT32_MIN;
    bitrate2.index = BTR_INDEX_250K;
    // @- call 'btr_check_bitrate' if 'bitrate1' is less than 'bitrate2'
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true), 0);
    // @test: invalid == invalid
    bitrate1.index = INT32_MIN;
    bitrate2.index = INT32_MIN;
    // @- call 'btr_check_bitrate' if 'bitrate1' is equal to 'bitrate2'
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
}

// @xctest TC0B.2.x: call 'btr_compare_bitrates' with equal values
//
// note: function 'btr_compare_bitrates' is extensively used in TC0B.7.9
//
// @expected 0 if the bit-rates are equal
//
- (void)testCompareBitratesWithEqualValues {
    btr_bitrate_t bitrate1;
    btr_bitrate_t bitrate2;
    // @test: in CAN 2.0 mode
    CAN_BR_250K(bitrate1);
    CAN_BR_250K(bitrate2);
    // @- frequency and BRP of 'bitrate2' multiplied by 10
    bitrate2.btr.frequency *= 10;
    bitrate2.btr.nominal.brp *= 10U;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false));
    // @- frequency set to 1 and other fields to 0
    memset(&bitrate1, 0x00, sizeof(btr_bitrate_t));
    memset(&bitrate2, 0x00, sizeof(btr_bitrate_t));
    bitrate1.btr.frequency = 1;
    bitrate2.btr.frequency = 1;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true));
    // @- frequency set to INT32_MAX and other fields to their maximum
    memset(&bitrate1, 0xFF, sizeof(btr_bitrate_t));
    memset(&bitrate2, 0xFF, sizeof(btr_bitrate_t));
    bitrate1.btr.frequency = INT32_MAX;
    bitrate2.btr.frequency = INT32_MAX;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true));
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    // @test: in CAN FD mode w/o BRSE
    CAN_FD_BR_250K(bitrate1);
    CAN_FD_BR_250K(bitrate2);
    // @- frequency and BRP of 'bitrate2' devided by 2
    bitrate2.btr.frequency /= 2;
    bitrate2.btr.nominal.brp /= 2U;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true));
    // @- frequency set to 1 and other fields to 0
    memset(&bitrate1, 0x00, sizeof(btr_bitrate_t));
    memset(&bitrate2, 0x00, sizeof(btr_bitrate_t));
    bitrate1.btr.frequency = 1;
    bitrate2.btr.frequency = 1;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true));
    // @- frequency set to INT32_MAX and other fields to their maximum
    memset(&bitrate1, 0xFF, sizeof(btr_bitrate_t));
    memset(&bitrate2, 0xFF, sizeof(btr_bitrate_t));
    bitrate1.btr.frequency = INT32_MAX;
    bitrate2.btr.frequency = INT32_MAX;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true));
    // @test: in CAN FD mode with BRSE
    CAN_FD_BR_250K2M(bitrate1);
    CAN_FD_BR_250K2M(bitrate2);
    // @- frequency and BRPs of 'bitrate1' devided by 2
    bitrate1.btr.frequency /= 2;
    bitrate1.btr.nominal.brp /= 2U;
    bitrate1.btr.data.brp /= 2U;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
    // @- frequency set to 1 and other fields to 0
    memset(&bitrate1, 0x00, sizeof(btr_bitrate_t));
    memset(&bitrate2, 0x00, sizeof(btr_bitrate_t));
    bitrate1.btr.frequency = 1;
    bitrate2.btr.frequency = 1;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
    // @- frequency set to INT32_MAX and other fields to their maximum
    memset(&bitrate1, 0xFF, sizeof(btr_bitrate_t));
    memset(&bitrate2, 0xFF, sizeof(btr_bitrate_t));
    bitrate1.btr.frequency = INT32_MAX;
    bitrate2.btr.frequency = INT32_MAX;
    // @- call 'btr_check_bitrate' (resulting bit-rates should be equal))
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
#endif
}

// @xctest TC0B.2.x: call 'btr_compare_bitrates' with unequal values
//
// @expected none-zero value if the bit-rates are not equal
//
- (void)testCompareBitratesWithUnequalValues {
    btr_bitrate_t bitrate1;
    btr_bitrate_t bitrate2;
    uint32_t tseg1_store;
    // @test: in CAN 2.0 mode
    // @- with different bit-rates
    CAN_BR_10K(bitrate1);
    CAN_BR_1M(bitrate2);
    // @-- call 'btr_check_bitrate' (should fail)
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, false, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, false, false, true), 0);
    // @- with same bit-rate but different sample-points
    CAN_BR_250K(bitrate1);
    CAN_BR_250K(bitrate2);
    tseg1_store = bitrate2.btr.nominal.tseg1;
    bitrate2.btr.nominal.tseg1 = bitrate2.btr.nominal.tseg2;
    bitrate2.btr.nominal.tseg2 = tseg1_store;
    // @-- call 'btr_check_bitrate' (should fail with 'cmp_sp' == true)
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false));
    XCTAssertNotEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true));
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    // @test: in CAN FD mode w/o BRSE
    // @- with different bit-rates
    CAN_FD_BR_125K(bitrate1);
    CAN_FD_BR_1M(bitrate2);
    // @-- call 'btr_check_bitrate' (should fail)
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, true, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, true, false, true), 0);
    // @- with same bit-rate but different sample-points
    CAN_FD_BR_250K(bitrate1);
    CAN_FD_BR_250K(bitrate2);
    tseg1_store = bitrate2.btr.nominal.tseg1;
    bitrate2.btr.nominal.tseg1 = bitrate2.btr.nominal.tseg2;
    bitrate2.btr.nominal.tseg2 = tseg1_store;
    // @-- call 'btr_check_bitrate'  (should fail with 'cmp_sp' == true)
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false));
    XCTAssertNotEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true));
    // @test: in CAN FD mode with BRSE
    // @- with different bit-rates
    CAN_FD_BR_125K1M(bitrate1);
    CAN_FD_BR_1M8M(bitrate2);
    // @-- call 'btr_check_bitrate' (should fail)
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, true, true, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, true, true, true), 0);
    // @- with different data phase bit-rates
    CAN_FD_BR_125K1M(bitrate1);
    CAN_FD_BR_1M8M(bitrate2);
    bitrate2.btr.nominal = bitrate1.btr.nominal;
    // @-- call 'btr_check_bitrate' (should fail)
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false), 0);
    XCTAssertLessThan(btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, true, true, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate2, &bitrate1, true, true, true), 0);
    // @- with same bit-rate but different nominal sample-points
    CAN_FD_BR_250K2M(bitrate1);
    CAN_FD_BR_250K2M(bitrate2);
    tseg1_store = bitrate1.btr.nominal.tseg1;
    bitrate1.btr.nominal.tseg1 = bitrate1.btr.nominal.tseg2;
    bitrate1.btr.nominal.tseg2 = tseg1_store;
    // @-- call 'btr_check_bitrate' (should fail with 'cmp_sp' == true)
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false));
    XCTAssertNotEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
    // @- with same bit-rate but different data phase sample-points
    CAN_FD_BR_250K2M(bitrate1);
    CAN_FD_BR_250K2M(bitrate2);
    tseg1_store = bitrate1.btr.data.tseg1;
    bitrate1.btr.data.tseg1 = bitrate1.btr.data.tseg2;
    bitrate1.btr.data.tseg2 = tseg1_store;
    // @-- call 'btr_check_bitrate' (should fail with 'cmp_sp' == true)
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false));
    XCTAssertNotEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
#endif
}

// @xctest TC0B.2.x: call 'btr_compare_bitrates' with NULL pointer for 'bitrate1' and 'bitrate2'
//
// @expected 0 if both are NULL pointer, or a none-zero value otherwise
//
- (void)testCompareBitratesWithNullPointer {
    btr_bitrate_t bitrate1;
    btr_bitrate_t bitrate2;
    // @test:
    // @- call 'btr_check_bitrate' with NULL pointer for 'bitrate2'
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, NULL, false, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, NULL, false, false, true), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, NULL, true, false, false), 0);
    XCTAssertGreaterThan(btr_compare_bitrates(&bitrate1, NULL, true, true, false), 0);
    // @- call 'btr_check_bitrate' with NULL pointer for 'bitrate2'
    XCTAssertLessThan(btr_compare_bitrates(NULL, &bitrate2, false, false, false), 0);
    XCTAssertLessThan(btr_compare_bitrates(NULL, &bitrate2, false, false, true), 0);
    XCTAssertLessThan(btr_compare_bitrates(NULL, &bitrate2, true, false, true), 0);
    XCTAssertLessThan(btr_compare_bitrates(NULL, &bitrate2, true, true, true), 0);
    // @- call 'btr_check_bitrate' with NULL pointer for both
    XCTAssertEqual(0, btr_compare_bitrates(NULL, NULL, false, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(NULL, NULL, false, false, true));
    XCTAssertEqual(0, btr_compare_bitrates(NULL, NULL, true, false, true));
    XCTAssertEqual(0, btr_compare_bitrates(NULL, NULL, true, true, true));
}

// @xctest TC0B.2.x: call 'btr_compare_bitrates' with division by zero
//
// @expected 0 (bit-rates are equal)
//
- (void)testCompareBitratesWithDivisionByZero {
    btr_bitrate_t bitrate1;
    btr_bitrate_t bitrate2;
    // @test: in CAN 2.0 mode
    CAN_BR_250K(bitrate1);
    bitrate1.btr.nominal.brp = 0U;
    CAN_BR_250K(bitrate2);
    bitrate2.btr.nominal.brp = 0U;
    // @- call 'btr_check_bitrate' both 'nom_brp' set to 0
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, false, false, true));
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    // @test: in CAN FD mode w/o BRSE
    CAN_FD_BR_250K2M(bitrate1);
    bitrate1.btr.nominal.brp = 0U;
    CAN_FD_BR_250K2M(bitrate2);
    bitrate2.btr.nominal.brp = 0U;
    // @- call 'btr_check_bitrate' both 'nom_brp' set to 0
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, false, true));
    // @test: in CAN FD mode with BRSE
    CAN_FD_BR_250K2M(bitrate1);
    bitrate1.btr.data.brp = 0U;
    CAN_FD_BR_250K2M(bitrate2);
    bitrate2.btr.data.brp = 0U;
    // @- call 'btr_check_bitrate' both 'data_brp' set to 0
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, false));
    XCTAssertEqual(0, btr_compare_bitrates(&bitrate1, &bitrate2, true, true, true));
#endif
}

// @xctest TC0B.3.1: call 'btr_bitrate2speed' with valid indexes
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToSpeedWithValidIndexes {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid indexes to predefined bit-rate table
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bzero(&bitrate, sizeof(btr_bitrate_t));
        bitrate.index = -index;
        // @-- convert index into transmission rete
        rc = btr_bitrate2speed(&bitrate, &speed);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        bzero(&bitrate, sizeof(btr_bitrate_t));
        (void)btr_index2bitrate(-index, &bitrate);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", index+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
    }
}

// @xctest TC0B.3.2: call 'btr_bitrate2speed' with invalid indexes
//
// @expected BTRERR_BAUDRATE
//
- (void)testBitrateToSpeedWithInvalidIndexes {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: index = -10
    bzero(&bitrate, sizeof(btr_bitrate_t));
    bitrate.index = -BTR_SJA1000_ENTRIES;
    // @-- try to convert -10  into transmission rete
    rc = btr_bitrate2speed(&bitrate, &speed);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // note: just to see what's the conversion result
    bzero(&bitrate, sizeof(btr_bitrate_t));
    (void)btr_index2bitrate(-BTR_SJA1000_ENTRIES, &bitrate);
    NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", 1,
          speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
          bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
    // @test: index = INT32_MIN
    bzero(&bitrate, sizeof(btr_bitrate_t));
    bitrate.index = INT32_MIN;
    // @-- try to convert INT32_MIN into transmission rete
    rc = btr_bitrate2speed(&bitrate, &speed);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // note: just to see what's the conversion result
    bzero(&bitrate, sizeof(btr_bitrate_t));
    (void)btr_index2bitrate(INT32_MIN, &bitrate);
    NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", 2,
          speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
          bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
}

// @xctest TC0B.3.3: call 'btr_bitrate2speed' with valid CAN 2.0 bit-rate settings
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToSpeedWithValidCan20Values {
    btr_bitrate_t bitrate[10];
    CAN_BR_1M(bitrate[0]);
    CAN_BR_800K(bitrate[1]);
    CAN_BR_500K(bitrate[2]);
    CAN_BR_250K(bitrate[3]);
    CAN_BR_125K(bitrate[4]);
    CAN_BR_100K(bitrate[5]);
    CAN_BR_50K(bitrate[6]);
    CAN_BR_20K(bitrate[7]);
    CAN_BR_10K(bitrate[8]);
    CAN_BR_5K(bitrate[9]);
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid CAN 2.0 bit-rate settings
    for (btr_index_t i = 0; i < 10; i++) {
        bzero(&speed, sizeof(btr_speed_t));
        // @-- convert bit-rate into transmission rete
        rc = btr_bitrate2speed(&bitrate[i], &speed);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate[i].btr.nominal.sjw, bitrate[i].btr.nominal.sam);
    }
}

// @xctest TC0B.3.4: call 'btr_bitrate2speed' with invalid CAN 2.0 bit-rate settings
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToSpeedWithInvalidCan20Values {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 16; i++) {
        CAN_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 kbps, but freq = INT32_MAX MHz
            case 0: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(2): CAN 250 kbps, but brp = 0
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(3): CAN 250 kbps, but brp = 1025
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(4): CAN 250 kbps, but brp = USHRT_MAX
            case 3: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(5): CAN 250 kbps, but tseg = 0
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(6): CAN 250 kbps, but tseg1 = 257
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(7): CAN 250 kbps, but tseg1 = USHRT_MAX
            case 6: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(8): CAN 250 kbps, but tseg2 = 0
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(9): CAN 250 kbps, but tseg2 = 129
            case 8: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(10): CAN 250 kbps, but tseg2 = USHRT_MAX
            case 9: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(11): CAN 250 kbps, but sjw = 0
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(12): CAN 250 kbps, but sjw = 129
            case 11: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(13): CAN 250 kbps, but sjw = USHRT_MAX
            case 12: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(14): CAN 250 kbps, but sam = 2
            case 13: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 1U; break;
            // @sub(15): CAN 250 kbps, but sam = 3
            case 14: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 2U; break;
            // @sub(16): CAN 250 kbps, but sam = UCHAR_MAX
            case 15: bitrate.btr.nominal.sam = UCHAR_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        // @-- convert bit-rate into transmission rete
        rc = btr_bitrate2speed(&bitrate, &speed);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
    }
}

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.3.5: call 'btr_bitrate2speed' with valid CAN FD bit-rate settings
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToSpeedWithValidCanFdValues {
    btr_bitrate_t bitrate[8];
    CAN_FD_BR_1M(bitrate[0]);
    CAN_FD_BR_500K(bitrate[1]);
    CAN_FD_BR_250K(bitrate[2]);
    CAN_FD_BR_125K(bitrate[3]);
    CAN_FD_BR_1M8M(bitrate[4]);
    CAN_FD_BR_500K4M(bitrate[5]);
    CAN_FD_BR_250K2M(bitrate[6]);
    CAN_FD_BR_125K1M(bitrate[7]);
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid CAN 2.0 bit-rate settings
    for (int i = 0; i < 8; i++) {
        bzero(&speed, sizeof(btr_speed_t));
        // @-- convert bit-rate into transmission rete
        rc = btr_bitrate2speed(&bitrate[i], &speed);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.3.6: call 'btr_bitrate2speed' with invalid CAN FD bit-rate settings
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToSpeedWithInvalidCanFdValues {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 29; i++) {
        CAN_FD_BR_250K2M(bitrate);
        switch (i) {
            // @sub(1): CAN FD 250 / 2000 kbps, but freq = INT32_MAX MHz
            case 0: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(2): CAN FD 250 / 2000 kbps, but brp = 0
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(3): CAN FD 250 / 2000 kbps, but brp = 1025
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(4): CAN FD 250 / 2000 kbps, but brp = USHRT_MAX
            case 3: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(5): CAN FD 250 / 2000 kbps, but tseg = 0
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(6): CAN FD 250 / 2000 kbps, but tseg1 = 257
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(7): CAN FD 250 / 2000 kbps, but tseg1 = USHRT_MAX
            case 6: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(8): CAN FD 250 / 2000 kbps, but tseg2 = 0
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(9): CAN FD 250 / 2000 kbps, but tseg2 = 129
            case 8: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(10): CAN FD 250 / 2000 kbps, but tseg2 = USHRT_MAX
            case 9: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(11): CAN FD 250 / 2000 kbps, but sjw = 0
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(12): CAN FD 250 / 2000 kbps, but sjw = 129
            case 11: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(13): CAN FD 250 / 2000 kbps, but sjw = USHRT_MAX
            case 12: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(14): CAN FD 250 / 2000 kbps, but sam = 2
            case 13: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 1U; break;
            // @sub(15): CAN FD 250 / 2000 kbps, but sam = 3
            case 14: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 2U; break;
            // @sub(16): CAN FD 250 / 2000 kbps, but sam = UCHAR_MAX
            case 15: bitrate.btr.nominal.sam = UCHAR_MAX; break;
            // @sub(17): CAN FD 250 / 2000 kbps, but data brp = 0
            case 16: bitrate.btr.data.brp = BTR_DATA_BRP_MIN - 1U; break;
            // @sub(18): CAN FD 250 / 2000 kbps, but data brp = 1025
            case 17: bitrate.btr.data.brp = BTR_DATA_BRP_MAX + 1U; break;
            // @sub(19): CAN FD 250 / 2000 kbps, but data brp = USHRT_MAX
            case 18: bitrate.btr.data.brp = USHRT_MAX; break;
            // @sub(20): CAN FD 250 / 2000 kbps, but data tseg = 0
            case 19: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MIN - 1U; break;
            // @sub(21): CAN FD 250 / 2000 kbps, but data tseg1 = 33
            case 20: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MAX + 1U; break;
            // @sub(22): CAN FD 250 / 2000 kbps, but data tseg1 = USHRT_MAX
            case 21: bitrate.btr.data.tseg1 = USHRT_MAX; break;
            // @sub(23): CAN FD 250 / 2000 kbps, but data tseg2 = 0
            case 22: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MIN - 1U; break;
            // @sub(24): CAN FD 250 / 2000 kbps, but data tseg2 = 17
            case 23: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MAX + 1U; break;
            // @sub(25): CAN FD 250 / 2000 kbps, but data tseg2 = USHRT_MAX
            case 24: bitrate.btr.data.tseg2 = USHRT_MAX; break;
            // @sub(26): CAN FD 250 / 2000 kbps, but data sjw = 0
            case 25: bitrate.btr.data.sjw = BTR_DATA_SJW_MIN - 1U; break;
            // @sub(27): CAN FD 250 / 2000 kbps, but data sjw = 17
            case 26: bitrate.btr.data.sjw = BTR_DATA_SJW_MAX + 1U; break;
            // @sub(28): CAN FD 250 / 2000 kbps, but data sjw = USHRT_MAX
            case 27: bitrate.btr.data.sjw = USHRT_MAX; break;
            // @sub(29): CAN FD 250 / 2000 kbps, but data fields = 0
            case 28: bitrate.btr.data.brp = bitrate.btr.data.tseg1 = bitrate.btr.data.tseg2 = bitrate.btr.data.sjw = 0U; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        // @-- convert bit-rate into transmission rete
        rc = btr_bitrate2speed(&bitrate, &speed);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: %.3f kbps @ %.1f%% / %.3f kbps @ %.1f%%\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              speed.data.speed / 1000.0, speed.data.samplepoint * 100.0);
    }
}
#endif

// @xctest TC0B.3.7: call 'btr_bitrate2speed' with NULL pointer for 'bitrate' and 'speed'
//
// @expected BTRERR_NULLPTR
//
- (void)testBitrateToSpeedWithNullPointer {
    btr_bitrate_t bitrate;
    CAN_BR_250K(bitrate);
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_bitrate2speed' with NULL pointer for 'speed'
    rc = btr_bitrate2speed(&bitrate, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2speed' with NULL pointer for 'bitrate'
    rc = btr_bitrate2speed(NULL, &speed);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2speed' with NULL pointer for both
    rc = btr_bitrate2speed(NULL, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.3.8: call 'btr_bitrate2speed' with division by zero
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToSpeedWithDivisonByZero {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test: CAN 2.0 250 kbps
    CAN_BR_250K(bitrate);
    bitrate.btr.nominal.brp = 0;
    // @- call 'btr_bitrate2speed' with nominal BRP set to zero
    rc = btr_bitrate2speed(&bitrate, &speed);
    XCTAssertEqual(BTRERR_NOERROR, rc);
    XCTAssertEqual(speed.nominal.speed, INFINITY);
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    // @test: CAN FD 250 kbps : 2 Mbps
    CAN_FD_BR_250K2M(bitrate);
    bitrate.btr.data.brp = 0;
    // @- call 'btr_bitrate2speed' with data phase BRP set to zero
    rc = btr_bitrate2speed(&bitrate, &speed);
    XCTAssertEqual(BTRERR_NOERROR, rc);
    XCTAssertEqual(speed.data.speed, INFINITY);
    XCTAssertNotEqual(speed.nominal.speed, INFINITY);
#endif
}

// @xctest TC0B.4.1: call 'btr_index2bitrate' with valid indexes
//
// @expected BTRERR_NOERROR
//
- (void)testIndexToBitrateWithValidIndexes {
    btr_bitrate_t predefined[BTR_SJA1000_ENTRIES];
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
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    btr_index_t result;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid indexes to predefined bit-rate table
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&predefined[index], &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", index+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        // @-- convert index into bit-rate settings
        rc = btr_index2bitrate(-index, &bitrate);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @-- check result against its predefined bit-rate table entry
        XCTAssertEqual(predefined[index].btr.frequency, bitrate.btr.frequency);
        XCTAssertEqual(predefined[index].btr.nominal.brp, bitrate.btr.nominal.brp);
        XCTAssertEqual(predefined[index].btr.nominal.tseg1, bitrate.btr.nominal.tseg1);
        XCTAssertEqual(predefined[index].btr.nominal.tseg2, bitrate.btr.nominal.tseg2);
        XCTAssertEqual(predefined[index].btr.nominal.sjw, bitrate.btr.nominal.sjw);
        XCTAssertEqual(predefined[index].btr.nominal.sam, bitrate.btr.nominal.sam);
        // @-- convert result back to index
        rc = btr_bitrate2index(&bitrate, &result);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        XCTAssertEqual(-index, result);
    }
}

// @xctest TC0B.4.2: call 'btr_index2bitrate' with invalid indexes
//
// @expected BTRERR_NULLPTR
//
- (void)testIndexToBitrateWithInvalidIndexes {
    btr_bitrate_t bitrate;
    int rc = BTRERR_FATAL;

    // @test:
    // @- call 'btr_index2sja1000' with +1 for 'index'
    rc = btr_index2bitrate(BTR_INDEX_1M+1, &bitrate);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // @test:
    // @- call 'btr_index2bitrate' with -10 for 'index'
    rc = btr_index2bitrate(BTR_INDEX_10K-2, &bitrate);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // @test:
    // @- call 'btr_index2bitrate' with INT_MAX for 'index'
    rc = btr_index2bitrate(INT_MAX, &bitrate);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // @test:
    // @- call 'btr_index2bitrate' with INT_MIN for 'index'
    rc = btr_index2bitrate(INT_MIN, &bitrate);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
}

// @xctest TC0B.4.3: call 'btr_index2bitrate' with NULL pointer for 'btr0btr1'
//
// @expected BTRERR_NULLPTR
//
- (void)testIndexToBitrateWithNullPointer {
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_index2bitrate' with NULL pointer for 'btr0btr1'
    rc = btr_index2bitrate(BTR_INDEX_1M, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.5.1: call 'btr_bitrate2index' with valid values
//
// @expected BTRERR_NULLPTR
//
- (void)testBitrateToIndexWithValidValues {
    btr_bitrate_t predefined[BTR_SJA1000_ENTRIES];
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
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    btr_index_t result;
    int rc = BTRERR_FATAL;
    // @test: SJA1000 bit-rate settings (!)
    // @- loop over valid values from predefined bit-rate table
    for (int i = 0; i < BTR_SJA1000_ENTRIES; i++) {
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&predefined[i], &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              predefined[i].btr.nominal.sjw, predefined[i].btr.nominal.sam);
        // @-- convert bit-rate settings into index
        rc = btr_bitrate2index(&predefined[i], &result);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        XCTAssertEqual((btr_index_t)-i, result);
    }
    // @test: index instead of bit-rate settings
    // @- loop over valid indexes to predefined bit-rate settings
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bitrate.index = -index;
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, IDX=%i)\n", index+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.index);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, false, false);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @-- convert bit-rate settings into index
        rc = btr_bitrate2index(&bitrate, &result);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        XCTAssertEqual(-index, result);
    }
}

// @xctest TC0B.5.2: call 'btr_bitrate2index' with invalid values
//
// @expected BTRERR_NULLPTR
//
- (void)testBitrateToIndexWithInvalidValues {
    btr_bitrate_t bitrate[BTR_SJA1000_ENTRIES];
    SJA1000_BR_1M(bitrate[0]);   bitrate[0].btr.frequency = BTR_FREQ_80MHz;
    SJA1000_BR_800K(bitrate[1]); bitrate[1].btr.nominal.brp = (bitrate[1].btr.nominal.brp != BTR_SJA1000_BRP_MAX) ? BTR_SJA1000_BRP_MAX : BTR_SJA1000_BRP_MIN;
    SJA1000_BR_500K(bitrate[2]); bitrate[2].btr.nominal.brp = (bitrate[2].btr.nominal.brp != BTR_SJA1000_BRP_MAX) ? BTR_SJA1000_BRP_MAX : BTR_SJA1000_BRP_MIN;
    SJA1000_BR_250K(bitrate[3]); bitrate[3].btr.nominal.tseg1 = (bitrate[3].btr.nominal.tseg1 != BTR_SJA1000_TSEG1_MAX) ? BTR_SJA1000_TSEG1_MAX : BTR_SJA1000_TSEG1_MIN;
    SJA1000_BR_125K(bitrate[4]); bitrate[4].btr.nominal.tseg1 = (bitrate[4].btr.nominal.tseg1 != BTR_SJA1000_TSEG1_MAX) ? BTR_SJA1000_TSEG1_MAX : BTR_SJA1000_TSEG1_MIN;
    SJA1000_BR_100K(bitrate[5]); bitrate[5].btr.nominal.tseg2 = (bitrate[5].btr.nominal.tseg2 != BTR_SJA1000_TSEG2_MAX) ? BTR_SJA1000_TSEG2_MAX : BTR_SJA1000_TSEG2_MIN;
    SJA1000_BR_50K(bitrate[6]);  bitrate[6].btr.nominal.tseg2 = (bitrate[6].btr.nominal.tseg2 != BTR_SJA1000_TSEG2_MAX) ? BTR_SJA1000_TSEG2_MAX : BTR_SJA1000_TSEG2_MIN;
    SJA1000_BR_20K(bitrate[7]);  bitrate[7].btr.nominal.sjw = (bitrate[7].btr.nominal.sjw != BTR_SJA1000_SJW_MAX) ? BTR_SJA1000_SJW_MAX : BTR_SJA1000_SJW_MIN;
    SJA1000_BR_10K(bitrate[8]);  bitrate[8].btr.nominal.sjw = (bitrate[8].btr.nominal.sjw != BTR_SJA1000_SJW_MAX) ? BTR_SJA1000_SJW_MAX : BTR_SJA1000_SJW_MIN;
    SJA1000_BR_5K(bitrate[9]);   bitrate[9].btr.nominal.sam = (bitrate[9].btr.nominal.sam != BTR_SJA1000_SAM_TRIPLE) ? BTR_SJA1000_SAM_TRIPLE : BTR_SJA1000_SAM_SINGLE;
    btr_speed_t speed;
    btr_index_t result;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid values not in the predefined bit-rate table
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate[index], &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", index+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate[index].btr.nominal.sjw, bitrate[index].btr.nominal.sam);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate[index], false, false);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @-- try to convert bit-rate settings into index
        rc = btr_bitrate2index(&bitrate[index], &result);
        XCTAssertEqual(BTRERR_BAUDRATE, rc);
    }
    // @test: with an invalid index
    bitrate[0].index = INT32_MIN;
    // @- try to convert bit-rate settings into index
    rc = btr_bitrate2index(&bitrate[0], &result);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
}

// @xctest TC0B.5.3: call 'btr_bitrate2index' with NULL pointer for 'bitrate' and 'index'
//
// @expected BTRERR_NULLPTR
//
- (void)testBitrateToIndexWithNullPointer {
    btr_bitrate_t bitrate;
    SJA1000_BR_250K(bitrate);
    btr_index_t index;
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_bitrate2index' with NULL pointer for 'index'
    rc = btr_bitrate2index(&bitrate, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2index' with NULL pointer for 'bitrate'
    rc = btr_bitrate2index(NULL, &index);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2index' with NULL pointer for both
    rc = btr_bitrate2index(NULL, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.6.1: call 'btr_string2bitrate' with well-formed strings
//
// @expected BTRERR_NOERROR
//
- (void)testStringToBitratewithCorrectStrings {
    btr_string_t string = NULL;
    btr_bitrate_t bitrate;
    bool data, sam;
    int i = 0;
    int rc = BTRERR_FATAL;

    CBitrates testcase = CBitrates();
    // @test: loop over list of test strings
    string = (btr_string_t)testcase.GetFirstEntry(true);
    while (string) {
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", ++i, string);
        // @- call 'btr_string2bitrate' with a well-formed string
        rc = btr_string2bitrate(string, &bitrate, &data, &sam);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // get next correct bit-rate string
        string = (btr_string_t)testcase.GetNextEntry(true);
    }
}

// @xctest TC0B.6.2: call 'btr_string2bitrate' with valid strings (in range)
//
// @expected BTRERR_NOERROR
//
- (void)testStringToBitratewithValidStrings {
    btr_string_t string = NULL;
    btr_bitrate_t bitrate;
    bool data, sam;
    int i = 0;
    int rc = BTRERR_FATAL;

    CBitrates testcase = CBitrates();
    // @test: loop over list of test strings
    string = (btr_string_t)testcase.GetFirstEntry(true);
    while (string) {
        // @- if the string is well-formed and in range:
        if (testcase.IsInRange()) {
            NSLog(@"Execute sub-testcase %d: \"%s\"\n", ++i, string);
            // @-- call 'btr_string2bitrate' with a valid string
            rc = btr_string2bitrate(string, &bitrate, &data, &sam);
            XCTAssertEqual(BTRERR_NOERROR, rc);
            // check only CAN 2.0 and CAN FD w/o BRSE bit-rates
            if (!testcase.IsCanFdWithBrse()) {
                // @-- call 'btr_check_bitrate' with CAN 2.0 range check
                rc = btr_check_bitrate(&bitrate, false, false);
                XCTAssertEqual(BTRERR_NOERROR, rc);
                // @-- note: also CAN FD w/o FRSE must pass
            }
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
            // @-- call 'btr_check_bitrate' with CAN FD range check
            rc = btr_check_bitrate(&bitrate, true, data);
            XCTAssertEqual(BTRERR_NOERROR, rc);
#endif
        }
        // get next correct bit-rate string
        string = (btr_string_t)testcase.GetNextEntry(true);
    }
}

// @xctest TC0B.6.3: call 'btr_string2bitrate' with invalid strings (out of range)
//
// @expected BTRERR_NOERROR, but range check returns BTRERR_BAUDRATE
//
- (void)testStringToBitratewithInvalidStrings {
    btr_string_t string = NULL;
    btr_bitrate_t bitrate;
    bool data, sam;
    int i = 0;
    int rc = BTRERR_FATAL;

    CBitrates testcase = CBitrates();
    // @test: loop over list of test strings
    string = (btr_string_t)testcase.GetFirstEntry(true);
    while (string) {
        // @- if the string is well-formed, but out of range:
        if (!testcase.IsInRange()) {
            NSLog(@"Execute sub-testcase %d: \"%s\"\n", ++i, string);
            // @-- call 'btr_string2bitrate' with a valid string
            rc = btr_string2bitrate(string, &bitrate, &data, &sam);
            XCTAssertEqual(BTRERR_NOERROR, rc);
            // check only CAN 2.0 and CAN FD w/o BRSE bit-rates
            if (!testcase.IsCanFdWithBrse()) {
                // @-- call 'btr_check_bitrate' with CAN 2.0 range check
                rc = btr_check_bitrate(&bitrate, false, false);
                XCTAssertEqual(BTRERR_BAUDRATE, rc);
                // @-- note: also CAN FD w/o FRSE must fail
            }
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
            if (!sam || ((bitrate.btr.nominal.sam == BTR_NOMINAL_SAM_SINGLE) ||
                         (bitrate.btr.nominal.sam == BTR_NOMINAL_SAM_TRIPLE))) {
                // @-- call 'btr_check_bitrate' with CAN FD range check
                rc = btr_check_bitrate(&bitrate, true, data);
                XCTAssertEqual(BTRERR_BAUDRATE, rc);
                // @-- note: if 'nom_sam' is out of range the conversion result will
                // @         not be checked because SAM is not specified in CAN FD.
                // @         In this case a second out-of-range error in any field
                // @         of the conversion result will not be detected.
            }
#endif
        }
        // get next correct bit-rate string
        string = (btr_string_t)testcase.GetNextEntry(true);
    }
}

// @xctest TC0B.6.4: call 'btr_string2bitrate' with wrong strings
//
// @expected BTRERR_BAUDRATE
//
- (void)testStringToBitratewithWrongStrings {
    btr_string_t string = NULL;
    btr_bitrate_t bitrate;
    bool data, sam;
    int i = 0;
    int rc = BTRERR_FATAL;

    CBitrates testcase = CBitrates();
    // @test: loop over list of test strings
    string = (btr_string_t)testcase.GetFirstEntry(false);
    while (string) {
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", ++i, string);
        // @- call 'btr_string2bitrate' with a not well-formed string
        rc = btr_string2bitrate(string, &bitrate, &data, &sam);
        XCTAssertEqual(BTRERR_ILLPARA, rc);
        // get next correct bit-rate string
        string = (btr_string_t)testcase.GetNextEntry(false);
    }
}

// @xctest TC0B.6.5: call 'btr_string2bitrate' with NULL pointer for 'string', 'bitrate', 'data' and 'sam'
//
// @expected BTRERR_NULLPTR
//
- (void)testStringToBitrateWithNullPointer {
    btr_string_t string = (btr_string_t)"";
    btr_bitrate_t bitrate;
    bool data, sam;
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_string2bitrate' with NULL pointer for 'sam'
    rc = btr_string2bitrate(string, &bitrate, &data, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @- call 'btr_string2bitrate' with NULL pointer for 'data'
    rc = btr_string2bitrate(string, &bitrate, NULL, &sam);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @- call 'btr_string2bitrate' with NULL pointer for 'bitrate'
    rc = btr_string2bitrate(string, NULL, &data, &sam);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_string2bitrate' with NULL pointer for 'string'
    rc = btr_string2bitrate(NULL, &bitrate, &data, &sam);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_string2bitrate' with NULL pointer for all
    rc = btr_string2bitrate(NULL, NULL, NULL, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.7.1: call 'btr_bitrate2string' with valid indexes
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithValidIndexes {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid indexes to predefined bit-rate table
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bzero(&bitrate, sizeof(btr_bitrate_t));
        bitrate.index = -index;
        // @- call 'btr_bitrate2string' with valid index to a predefined bit-rate
        rc = btr_bitrate2string(&bitrate, false, true, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (index+1), buffer);
    }
}

// @xctest TC0B.7.2: call 'btr_bitrate2string' with invalid indexes
//
// @expected BTRERR_BAUDRATE
//
- (void)testBitrateToStringWithInvalidIndexes {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate;
    int rc = BTRERR_FATAL;
    // @test: index = -10
    bzero(&bitrate, sizeof(btr_bitrate_t));
    bitrate.index = -BTR_SJA1000_ENTRIES;
    // @- call 'btr_bitrate2string' with invalid index to a predefined bit-rate
    rc = btr_bitrate2string(&bitrate, false, true, buffer, BTR_STRING_LENGTH);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // note: just to see what's the conversion result
    NSLog(@"Execute sub-testcase %d: \"%s\"\n", 1, buffer);
    // @test: index = INT32_MIN
    bzero(&bitrate, sizeof(btr_bitrate_t));
    bitrate.index = INT32_MIN;
    // @- call 'btr_bitrate2string' with invalid index to a predefined bit-rate
    rc = btr_bitrate2string(&bitrate, false, true, buffer, BTR_STRING_LENGTH);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // note: just to see what's the conversion result
    NSLog(@"Execute sub-testcase %d: \"%s\"\n", 2, buffer);
}

// @xctest TC0B.7.3: call 'btr_bitrate2string' with valid CAN 2.0 bit-rate settings
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithValidCan20Values {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate[10];
    CAN_BR_1M(bitrate[0]);
    CAN_BR_800K(bitrate[1]);
    CAN_BR_500K(bitrate[2]);
    CAN_BR_250K(bitrate[3]);
    CAN_BR_125K(bitrate[4]);
    CAN_BR_100K(bitrate[5]);
    CAN_BR_50K(bitrate[6]);
    CAN_BR_20K(bitrate[7]);
    CAN_BR_10K(bitrate[8]);
    CAN_BR_5K(bitrate[9]);
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid CAN 2.0 bit-rate settings
    for (btr_index_t i = 0; i < 10; i++) {
        // @- call 'btr_bitrate2string' with valid CAN 2.0 bit-rate value
        rc = btr_bitrate2string(&bitrate[i], false, true, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (i+1), buffer);
    }
}

// @xctest TC0B.7.4: call 'btr_bitrate2string' with invalid CAN 2.0 bit-rate settings
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithInvalidCan20Values {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 16; i++) {
        CAN_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 kbps, but freq = INT32_MAX MHz
            case 0: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(2): CAN 250 kbps, but brp = 0
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(3): CAN 250 kbps, but brp = 1025
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(4): CAN 250 kbps, but brp = USHRT_MAX
            case 3: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(5): CAN 250 kbps, but tseg = 0
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(6): CAN 250 kbps, but tseg1 = 257
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(7): CAN 250 kbps, but tseg1 = USHRT_MAX
            case 6: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(8): CAN 250 kbps, but tseg2 = 0
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(9): CAN 250 kbps, but tseg2 = 129
            case 8: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(10): CAN 250 kbps, but tseg2 = USHRT_MAX
            case 9: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(11): CAN 250 kbps, but sjw = 0
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(12): CAN 250 kbps, but sjw = 129
            case 11: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(13): CAN 250 kbps, but sjw = USHRT_MAX
            case 12: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(14): CAN 250 kbps, but sam = 2
            case 13: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 1U; break;
            // @sub(15): CAN 250 kbps, but sam = 3
            case 14: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 2U; break;
            // @sub(16): CAN 250 kbps, but sam = UCHAR_MAX
            case 15: bitrate.btr.nominal.sam = UCHAR_MAX; break;
        }
        // @- call 'btr_bitrate2string' with invalid CAN 2.0 bit-rate value
        rc = btr_bitrate2string(&bitrate, false, true, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (i+1), buffer);
    }
}

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.7.5: call 'btr_bitrate2string' with valid CAN FD bit-rate settings and BRS disabled
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithValidCanFdValuesBrsDisabled {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate[8];
    CAN_FD_BR_1M(bitrate[0]);
    CAN_FD_BR_500K(bitrate[1]);
    CAN_FD_BR_250K(bitrate[2]);
    CAN_FD_BR_125K(bitrate[3]);
    CAN_FD_BR_1M8M(bitrate[4]);
    CAN_FD_BR_500K4M(bitrate[5]);
    CAN_FD_BR_250K2M(bitrate[6]);
    CAN_FD_BR_125K1M(bitrate[7]);
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid CAN 2.0 bit-rate settings
    for (int i = 0; i < 8; i++) {
        // @- call 'btr_bitrate2string' with valid CAN FD bit-rate value w/o BRSE
        rc = btr_bitrate2string(&bitrate[i], false, false, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (i+1), buffer);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.7.6: call 'btr_bitrate2string' with invalid CAN FD bit-rate settings and BRS disabled
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithInvalidCanFdValuesBrsDisabled {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 16; i++) {
        CAN_BR_250K(bitrate);
        switch (i) {
            // @sub(1): CAN 250 kbps, but freq = INT32_MAX MHz
            case 0: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(2): CAN 250 kbps, but brp = 0
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(3): CAN 250 kbps, but brp = 1025
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(4): CAN 250 kbps, but brp = USHRT_MAX
            case 3: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(5): CAN 250 kbps, but tseg = 0
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(6): CAN 250 kbps, but tseg1 = 257
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(7): CAN 250 kbps, but tseg1 = USHRT_MAX
            case 6: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(8): CAN 250 kbps, but tseg2 = 0
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(9): CAN 250 kbps, but tseg2 = 129
            case 8: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(10): CAN 250 kbps, but tseg2 = USHRT_MAX
            case 9: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(11): CAN 250 kbps, but sjw = 0
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(12): CAN 250 kbps, but sjw = 129
            case 11: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(13): CAN 250 kbps, but sjw = USHRT_MAX
            case 12: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(14): CAN 250 kbps, but sam = 2
            case 13: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 1U; break;
            // @sub(15): CAN 250 kbps, but sam = 3
            case 14: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 2U; break;
            // @sub(16): CAN 250 kbps, but sam = UCHAR_MAX
            case 15: bitrate.btr.nominal.sam = UCHAR_MAX; break;
        }
        // @- call 'btr_bitrate2string' with invalid CAN FD bit-rate value w/o BRSE
        rc = btr_bitrate2string(&bitrate, false, false, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (i+1), buffer);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.7.7: call 'btr_bitrate2string' with valid CAN FD bit-rate settings and BRS enabled
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithValidCanFdValuesBrsEnabled {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate[3];
    CAN_FD_BR_500K4M(bitrate[0]);
    CAN_FD_BR_250K2M(bitrate[1]);
    CAN_FD_BR_125K1M(bitrate[2]);
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid CAN 2.0 bit-rate settings
    for (int i = 0; i < 3; i++) {
        // @- call 'btr_bitrate2string' with valid CAN FD bit-rate value with BRSE
        rc = btr_bitrate2string(&bitrate[i], true, false, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (i+1), buffer);
    }
}
#endif

#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
// @xctest TC0B.7.8: call 'btr_bitrate2string' with invalid CAN FD bit-rate settings and BRS enabled
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithInvalidCanFdValuesBrsEnabled {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 29; i++) {
        CAN_FD_BR_250K2M(bitrate);
        switch (i) {
            // @sub(1): CAN FD 250 / 2000 kbps, but freq = INT32_MAX MHz
            case 0: bitrate.btr.frequency = INT32_MAX; break;
            // @sub(2): CAN FD 250 / 2000 kbps, but brp = 0
            case 1: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MIN - 1U; break;
            // @sub(3): CAN FD 250 / 2000 kbps, but brp = 1025
            case 2: bitrate.btr.nominal.brp = BTR_NOMINAL_BRP_MAX + 1U; break;
            // @sub(4): CAN FD 250 / 2000 kbps, but brp = USHRT_MAX
            case 3: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(5): CAN FD 250 / 2000 kbps, but tseg = 0
            case 4: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MIN - 1U; break;
            // @sub(6): CAN FD 250 / 2000 kbps, but tseg1 = 257
            case 5: bitrate.btr.nominal.tseg1 = BTR_NOMINAL_TSEG1_MAX + 1U; break;
            // @sub(7): CAN FD 250 / 2000 kbps, but tseg1 = USHRT_MAX
            case 6: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(8): CAN FD 250 / 2000 kbps, but tseg2 = 0
            case 7: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MIN - 1U; break;
            // @sub(9): CAN FD 250 / 2000 kbps, but tseg2 = 129
            case 8: bitrate.btr.nominal.tseg2 = BTR_NOMINAL_TSEG2_MAX + 1U; break;
            // @sub(10): CAN FD 250 / 2000 kbps, but tseg2 = USHRT_MAX
            case 9: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(11): CAN FD 250 / 2000 kbps, but sjw = 0
            case 10: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MIN - 1U; break;
            // @sub(12): CAN FD 250 / 2000 kbps, but sjw = 129
            case 11: bitrate.btr.nominal.sjw = BTR_NOMINAL_SJW_MAX + 1U; break;
            // @sub(13): CAN FD 250 / 2000 kbps, but sjw = USHRT_MAX
            case 12: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(14): CAN FD 250 / 2000 kbps, but sam = 2
            case 13: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 1U; break;
            // @sub(15): CAN FD 250 / 2000 kbps, but sam = 3
            case 14: bitrate.btr.nominal.sam = BTR_NOMINAL_SAM_TRIPLE + 2U; break;
            // @sub(16): CAN FD 250 / 2000 kbps, but sam = UCHAR_MAX
            case 15: bitrate.btr.nominal.sam = UCHAR_MAX; break;
            // @sub(17): CAN FD 250 / 2000 kbps, but data brp = 0
            case 16: bitrate.btr.data.brp = BTR_DATA_BRP_MIN - 1U; break;
            // @sub(18): CAN FD 250 / 2000 kbps, but data brp = 1025
            case 17: bitrate.btr.data.brp = BTR_DATA_BRP_MAX + 1U; break;
            // @sub(19): CAN FD 250 / 2000 kbps, but data brp = USHRT_MAX
            case 18: bitrate.btr.data.brp = USHRT_MAX; break;
            // @sub(20): CAN FD 250 / 2000 kbps, but data tseg = 0
            case 19: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MIN - 1U; break;
            // @sub(21): CAN FD 250 / 2000 kbps, but data tseg1 = 33
            case 20: bitrate.btr.data.tseg1 = BTR_DATA_TSEG1_MAX + 1U; break;
            // @sub(22): CAN FD 250 / 2000 kbps, but data tseg1 = USHRT_MAX
            case 21: bitrate.btr.data.tseg1 = USHRT_MAX; break;
            // @sub(23): CAN FD 250 / 2000 kbps, but data tseg2 = 0
            case 22: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MIN - 1U; break;
            // @sub(24): CAN FD 250 / 2000 kbps, but data tseg2 = 17
            case 23: bitrate.btr.data.tseg2 = BTR_DATA_TSEG2_MAX + 1U; break;
            // @sub(25): CAN FD 250 / 2000 kbps, but data tseg2 = USHRT_MAX
            case 24: bitrate.btr.data.tseg2 = USHRT_MAX; break;
            // @sub(26): CAN FD 250 / 2000 kbps, but data sjw = 0
            case 25: bitrate.btr.data.sjw = BTR_DATA_SJW_MIN - 1U; break;
            // @sub(27): CAN FD 250 / 2000 kbps, but data sjw = 17
            case 26: bitrate.btr.data.sjw = BTR_DATA_SJW_MAX + 1U; break;
            // @sub(28): CAN FD 250 / 2000 kbps, but data sjw = USHRT_MAX
            case 27: bitrate.btr.data.sjw = USHRT_MAX; break;
            // @sub(29): CAN FD 250 / 2000 kbps, but data fields = 0
            case 28: bitrate.btr.data.brp = bitrate.btr.data.tseg1 = bitrate.btr.data.tseg2 = bitrate.btr.data.sjw = 0U; break;
        }
        // @- call 'btr_bitrate2string' with invalid CAN FD bit-rate value with BRSE
        rc = btr_bitrate2string(&bitrate, true, false, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (i+1), buffer);
    }
}
#endif

// @xctest TC0B.7.9: call 'btr_bitrate2string' from well-formed bit-rate strings
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringFromWellformedStrings {
    char buffer[BTR_STRING_LENGTH];
    btr_string_t string = NULL;
    btr_bitrate_t source;
    btr_bitrate_t target;
    bool data, sam;
    int i = 0;
    int rc = BTRERR_FATAL;

    CBitrates testcase = CBitrates();
    // @test: loop over list of test strings
    string = (btr_string_t)testcase.GetFirstEntry(true);
    while (string) {
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", ++i, string);
        // @- call 'btr_string2bitrate' with a well-formed string
        rc = btr_string2bitrate(string, &source, &data, &sam);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @- call 'btr_bitrate2string' with the resulting bit-rate value
        rc = btr_bitrate2string(&source, data, sam, buffer, BTR_STRING_LENGTH);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @- call 'btr_string2bitrate' with the resulting bit-rate string
        rc = btr_string2bitrate(string, &target, &data, &sam);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @- compare both bit-rate settings in CAN 2.0 mode
        XCTAssertEqual(0, btr_compare_bitrates(&source, &target, false, false, true));
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
        // @- compare both bit-rate settings in CAN FD mode (with or w/o BRSE)
        XCTAssertEqual(0, btr_compare_bitrates(&source, &target, true, testcase.IsCanFdWithBrse(), true));
#endif
        // note: just to see what's the conversion result
        NSLog(@"                    ==> \"%s\"\n", buffer);
        // get next correct bit-rate string
        string = (btr_string_t)testcase.GetNextEntry(true);
    }
}

// @xctest TC0B.7.10: call 'btr_bitrate2string' with buffer size too small
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToStringWithBufferSizeTooSmall {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate[19];
    bitrate[0].index = BTR_INDEX_1M;
    bitrate[1].index = BTR_INDEX_800K;
    bitrate[2].index = BTR_INDEX_500K;
    bitrate[3].index = BTR_INDEX_250K;
    bitrate[4].index = BTR_INDEX_125K;
    bitrate[5].index = BTR_INDEX_100K;
    bitrate[6].index = BTR_INDEX_50K;
    bitrate[7].index = BTR_INDEX_20K;
    bitrate[8].index = BTR_INDEX_10K;
    CAN_BR_1M(bitrate[9]);
    CAN_BR_800K(bitrate[10]);
    CAN_BR_500K(bitrate[11]);
    CAN_BR_250K(bitrate[12]);
    CAN_BR_125K(bitrate[13]);
    CAN_BR_100K(bitrate[14]);
    CAN_BR_50K(bitrate[15]);
    CAN_BR_20K(bitrate[16]);
    CAN_BR_10K(bitrate[17]);
    CAN_BR_5K(bitrate[18]);
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid CAN 2.0 bit-rate settings
    for (btr_index_t i = 0; i < 19; i++) {
        // @- call 'btr_bitrate2string' with too small buffer
        rc = btr_bitrate2string(&bitrate[i], false, true, buffer, (i+1));
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // note: just to see what's the conversion result
        NSLog(@"Execute sub-testcase %d: \"%s\"\n", (i+1), buffer);
    }
}

// @xctest TC0B.7.11: call 'btr_bitrate2string' with buffer size zero
//
// @expected BTRERR_ILLPARA
//
- (void)testBitrateToStringWithBufferSizeZero {
    char buffer[BTR_STRING_LENGTH];
    btr_bitrate_t bitrate;
    CAN_BR_250K(bitrate);
    btr_speed_t speed;
   int rc = BTRERR_FATAL;
    // @test:
    bzero(&speed, sizeof(btr_speed_t));
    (void)btr_bitrate2speed(&bitrate, &speed);
    // @- call 'btr_bitrate2string' with buffer size 0
    rc = btr_bitrate2string(&bitrate, false, true, buffer, 0);
    XCTAssertEqual(BTRERR_ILLPARA, rc);
    // note: just to see what's the conversion result
    NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", 1,
          speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
          bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
}

// @xctest TC0B.7.12: call 'btr_bitrate2string' with NULL pointer for 'bitrate' and 'string'
//
// @expected BTRERR_NULLPTR
//
- (void)testBitrateToStringWithNullPointer {
    btr_string_t string = (btr_string_t)"";
    btr_bitrate_t bitrate;
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_bitrate2string' with NULL pointer for 'string'
    rc = btr_bitrate2string(&bitrate, true, false, NULL, 0);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2string' with NULL pointer for 'bitrate'
    rc = btr_bitrate2string(NULL, false, true, string, 256);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2string' with NULL pointer for both
    rc = btr_bitrate2string(NULL, true, true, NULL, 65536);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.8.1: call 'btr_sja10002bitrate' with valid values
//
// @expected BTRERR_NOERROR
//
- (void)testSja1000ToBitrateWithValidValues {
    const btr_sja1000_t sja1000[BTR_SJA1000_ENTRIES] = {
        SJA1000_1M, SJA1000_800K, SJA1000_500K, SJA1000_250K, SJA1000_125K,
        SJA1000_100K, SJA1000_50K, SJA1000_20K, SJA1000_10K, SJA1000_5K
    };
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    btr_sja1000_t btr0btr1;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid BTR0BTR1 register values
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bzero(&bitrate, sizeof(btr_bitrate_t));
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_sja10002bitrate(sja1000[index], &bitrate);
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", index+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        // @-- convert BTR0BTR1 register to bit-rate settings
        rc = btr_sja10002bitrate(sja1000[index], &bitrate);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @-- check for SJA1000 CAN clock (8 MHZ)
        XCTAssertEqual(BTR_FREQ_SJA1000, bitrate.btr.frequency);
        // @-- check data phase fields (acc. OPTION_CANBTR_DATA_FIELDS)
#if (OPTION_CANBTR_DATA_FIELDS == 1)
        XCTAssertEqual(bitrate.btr.nominal.brp, bitrate.btr.data.brp);
        XCTAssertEqual(bitrate.btr.nominal.tseg1, bitrate.btr.data.tseg1);
        XCTAssertEqual(bitrate.btr.nominal.tseg2, bitrate.btr.data.tseg2);
        XCTAssertEqual(bitrate.btr.nominal.sjw, bitrate.btr.data.sjw);
#elif (OPTION_CANBTR_DATA_FIELDS == 2)
        XCTAssertEqual(0U, bitrate.btr.data.brp);
        XCTAssertEqual(0U, bitrate.btr.data.tseg1);
        XCTAssertEqual(0U, bitrate.btr.data.tseg2);
        XCTAssertEqual(0U, bitrate.btr.data.sjw);
#endif
        // @-- convert result back to BTR0BTR1 register
        rc = btr_bitrate2sja1000(&bitrate, &btr0btr1);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        XCTAssertEqual(sja1000[index], btr0btr1);
    }
}

// @xctest TC0B.8.2: call 'btr_sja10002bitrate' with suspective values
//
// @expected BTRERR_NOERROR
//
- (void)testSja1000ToBitrateWithsuspectiveValues {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_sja10002bitrate' with 0x0000 for 'bitrate'
    rc = btr_sja10002bitrate(0x0000U, &bitrate);
    XCTAssertEqual(BTRERR_NOERROR, rc);
    // @-- check if the bit-rate settings are valid
    rc = btr_check_bitrate(&bitrate, false, false);
    XCTAssertEqual(BTRERR_NOERROR, rc);
    // note: just to see what's the conversion result
    bzero(&speed, sizeof(btr_speed_t));
    (void)btr_bitrate2speed(&bitrate, &speed);
    NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", 1,
          speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
          bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
    // @test:
    // @- call 'btr_sja10002bitrate' with 0xFFFF for 'bitrate'
    rc = btr_sja10002bitrate(0xFFFFU, &bitrate);
    XCTAssertEqual(BTRERR_NOERROR, rc);
    // @-- check if the bit-rate settings are valid
    rc = btr_check_bitrate(&bitrate, false, false);
    XCTAssertEqual(BTRERR_NOERROR, rc);
    // note: just to see what's the conversion result
    bzero(&speed, sizeof(btr_speed_t));
    (void)btr_bitrate2speed(&bitrate, &speed);
    NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.2f%%, SJW=%u, SAM=%u)\n", 2,
          speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
          bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
}

// @xctest TC0B.8.3: call 'btr_sja10002bitrate' with NULL pointer for 'bitrate'
//
// @expected BTRERR_NULLPTR
//
- (void)testSja1000ToBitrateWithNullPointer {
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_sja10002bitrate' with NULL pointer for 'bitrate'
    rc = btr_sja10002bitrate(SJA1000_1M, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.9.1: call 'btr_bitrate2sja1000' with valid values
//
// @expected BTRERR_NOERROR
//
- (void)testBitrateToSja1000WithValidValues {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    btr_sja1000_t btr0btr1;
    int rc = BTRERR_FATAL;
    // @test: loop over valid bit-rate settings
    for (int i = 0; i < 10; i++) {
        SJA1000_BR_250K(bitrate);
        switch (i) {
            // @sub(1): SJA1000 250 kbps, but brp = 1
            case 0: bitrate.btr.nominal.brp = BTR_SJA1000_BRP_MIN; break;
            // @sub(2): SJA1000 250 kbps, but brp = 64
            case 1: bitrate.btr.nominal.brp = BTR_SJA1000_BRP_MAX; break;
            // @sub(3): SJA1000 250 kbps, but tseg = 1
            case 2: bitrate.btr.nominal.tseg1 = BTR_SJA1000_TSEG1_MIN; break;
            // @sub(4): SJA1000 250 kbps, but tseg1 = 16
            case 3: bitrate.btr.nominal.tseg1 = BTR_SJA1000_TSEG1_MAX; break;
            // @sub(5): SJA1000 250 kbps, but tseg2 = 1
            case 4: bitrate.btr.nominal.tseg2 = BTR_SJA1000_TSEG2_MIN; break;
            // @sub(6): SJA1000 250 kbps, but tseg2 = 8
            case 5: bitrate.btr.nominal.tseg2 = BTR_SJA1000_TSEG2_MAX; break;
            // @sub(7): SJA1000 250 kbps, but sjw = 1
            case 6: bitrate.btr.nominal.sjw = BTR_SJA1000_SJW_MIN; break;
            // @sub(8): SJA1000 250 kbps, but sjw = 4
            case 7: bitrate.btr.nominal.sjw = BTR_SJA1000_SJW_MAX; break;
            // @sub(9): SJA1000 250 kbps, but sam = 0
            case 8: bitrate.btr.nominal.sam = BTR_SJA1000_SAM_SINGLE; break;
            // @sub(10): SJA1000 250 kbps, but sam = 1
            case 9: bitrate.btr.nominal.sam = BTR_SJA1000_SAM_TRIPLE; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        // @-- check if the bit-rate settings are valid
        rc = btr_check_bitrate(&bitrate, false, false);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @-- call 'btr_bitrate2sja1000' with valid bit-rate settings
        rc = btr_bitrate2sja1000(&bitrate, &btr0btr1);
        XCTAssertEqual(BTRERR_NOERROR, rc);
    }
}

// @xctest TC0B.9.2: call 'btr_bitrate2sja1000' with invalid values
//
// @expected BTRERR_BAUDRATE
//
- (void)testBitrateToSja1000WithInalidValues {
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    btr_sja1000_t btr0btr1;
    int rc = BTRERR_FATAL;
    // @test: loop over invalid bit-rate settings
    for (int i = 0; i < 16; i++) {
        SJA1000_BR_250K(bitrate);
        switch (i) {
            // @sub(1): SJA1000 250 kbps, but freq != 8000000 MHz
            case 0: bitrate.btr.frequency = 0; break;
            // @sub(2): SJA1000 250 kbps, but brp = 0
            case 1: bitrate.btr.nominal.brp = BTR_SJA1000_BRP_MIN - 1U; break;
            // @sub(3): SJA1000 250 kbps, but brp = 65
            case 2: bitrate.btr.nominal.brp = BTR_SJA1000_BRP_MAX + 1U; break;
            // @sub(4): SJA1000 250 kbps, but brp = USHRT_MAX
            case 3: bitrate.btr.nominal.brp = USHRT_MAX; break;
            // @sub(5): SJA1000 250 kbps, but tseg = 0
            case 4: bitrate.btr.nominal.tseg1 = BTR_SJA1000_TSEG1_MIN - 1U; break;
            // @sub(6): SJA1000 250 kbps, but tseg1 = 17
            case 5: bitrate.btr.nominal.tseg1 = BTR_SJA1000_TSEG1_MAX + 1U; break;
            // @sub(7): SJA1000 250 kbps, but tseg1 = USHRT_MAX
            case 6: bitrate.btr.nominal.tseg1 = USHRT_MAX; break;
            // @sub(8): SJA1000 250 kbps, but tseg2 = 0
            case 7: bitrate.btr.nominal.tseg2 = BTR_SJA1000_TSEG2_MIN - 1U; break;
            // @sub(9): SJA1000 250 kbps, but tseg2 = 9
            case 8: bitrate.btr.nominal.tseg2 = BTR_SJA1000_TSEG2_MAX + 1U; break;
            // @sub(10): SJA1000 250 kbps, but tseg2 = USHRT_MAX
            case 9: bitrate.btr.nominal.tseg2 = USHRT_MAX; break;
            // @sub(11): SJA1000 250 kbps, but sjw = 0
            case 10: bitrate.btr.nominal.sjw = BTR_SJA1000_SJW_MIN - 1U; break;
            // @sub(12): SJA1000 250 kbps, but sjw = 5
            case 11: bitrate.btr.nominal.sjw = BTR_SJA1000_SJW_MAX + 1U; break;
            // @sub(13): SJA1000 250 kbps, but sjw = USHRT_MAX
            case 12: bitrate.btr.nominal.sjw = USHRT_MAX; break;
            // @sub(14): SJA1000 250 kbps, but sam = 2
            case 13: bitrate.btr.nominal.sam = BTR_SJA1000_SAM_TRIPLE + 1U; break;
            // @sub(15): SJA1000 250 kbps, but sam = 3
            case 14: bitrate.btr.nominal.sam = BTR_SJA1000_SAM_TRIPLE + 2U; break;
            // @sub(16): SJA1000 250 kbps, but sam = UCHAR_MAX
            case 15: bitrate.btr.nominal.sam = UCHAR_MAX; break;
        }
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", i+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        // @-- call 'btr_bitrate2sja1000' with invalid bit-rate settings
        rc = btr_bitrate2sja1000(&bitrate, &btr0btr1);
        XCTAssertEqual(BTRERR_BAUDRATE, rc);
    }
}

// @xctest TC0B.9.3: call 'btr_bitrate2sja1000' with NULL pointer for 'bitrate' and 'btr0btr1'
//
// @expected BTRERR_NULLPTR
//
- (void)testBitrateToSja1000WithNullPointer {
    btr_bitrate_t bitrate;
    SJA1000_BR_250K(bitrate);
    btr_sja1000_t btr0btr1;
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_bitrate2sja1000' with NULL pointer for 'btr0btr1'
    rc = btr_bitrate2sja1000(&bitrate, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2sja1000' with NULL pointer for 'bitrate'
    rc = btr_bitrate2sja1000(NULL, &btr0btr1);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
    // @test:
    // @- call 'btr_bitrate2sja1000' with NULL pointer for both
    rc = btr_bitrate2sja1000(NULL, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

// @xctest TC0B.10.1: call 'btr_index2sja1000' with valid indexes
//
// @expected BTRERR_NOERROR
//
- (void)testIndexToSja1000WithValidIndexes {
    const btr_sja1000_t sja1000[BTR_SJA1000_ENTRIES] = {
        SJA1000_1M, SJA1000_800K, SJA1000_500K, SJA1000_250K, SJA1000_125K,
        SJA1000_100K, SJA1000_50K, SJA1000_20K, SJA1000_10K, SJA1000_5K
    };
    btr_bitrate_t bitrate;
    btr_speed_t speed;
    btr_sja1000_t btr0btr1;
    int rc = BTRERR_FATAL;
    // @test:
    // @- loop over valid indexes to predefined bit-rate table
    for (btr_index_t index = 0; index < BTR_SJA1000_ENTRIES; index++) {
        bzero(&bitrate, sizeof(btr_bitrate_t));
        bzero(&speed, sizeof(btr_speed_t));
        (void)btr_sja10002bitrate(sja1000[index], &bitrate);
        (void)btr_bitrate2speed(&bitrate, &speed);
        NSLog(@"Execute sub-testcase %d: %.3f kbps (SP=%.1f%%, SJW=%u, SAM=%u)\n", index+1,
              speed.nominal.speed / 1000.0, speed.nominal.samplepoint * 100.0,
              bitrate.btr.nominal.sjw, bitrate.btr.nominal.sam);
        // @-- convert index to BTR0BTR1 register
        rc = btr_index2sja1000(-index, &btr0btr1);
        XCTAssertEqual(BTRERR_NOERROR, rc);
        // @-- check result against its SJA1000 BTR0BTR1 table entry
        XCTAssertEqual(sja1000[index], btr0btr1);
    }
}

// @xctest TC0B.10.2: call 'btr_index2sja1000' with invalid indexes
//
// @expected BTRERR_NOERROR
//
- (void)testIndexToSja1000WithInalidIndexes {
    btr_sja1000_t btr0btr1;
    int rc = BTRERR_FATAL;

    // @test:
    // @- call 'btr_index2sja1000' with +1 for 'index'
    rc = btr_index2sja1000(BTR_INDEX_1M+1, &btr0btr1);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // @test:
    // @- call 'btr_index2sja1000' with -10 for 'index'
    rc = btr_index2sja1000(BTR_INDEX_10K-2, &btr0btr1);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // @test:
    // @- call 'btr_index2sja1000' with INT_MAX for 'index'
    rc = btr_index2sja1000(INT_MAX, &btr0btr1);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
    // @test:
    // @- call 'btr_index2sja1000' with INT_MIN for 'index'
    rc = btr_index2sja1000(INT_MIN, &btr0btr1);
    XCTAssertEqual(BTRERR_BAUDRATE, rc);
}

// @xctest TC0B.10.3: call 'btr_index2sja1000' with NULL pointer for 'btr0btr1'
//
// @expected BTRERR_NULLPTR
//
- (void)testIndexToSja1000WithNullPointer {
    int rc = BTRERR_FATAL;
    // @test:
    // @- call 'btr_index2sja1000' with NULL pointer for 'btr0btr1'
    rc = btr_index2sja1000(BTR_INDEX_1M, NULL);
    XCTAssertEqual(BTRERR_NULLPTR, rc);
}

@end
