/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
 *
 *  Copyright (c) 2020-2021 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *  All rights reserved.
 *
 *  This file is part of MacCAN-KvaserCAN.
 *
 *  MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v3.0 (or any later version). You can
 *  choose between one of them if you use MacCAN-KvaserCAN in whole or in part.
 *
 *  BSD 2-Clause Simplified License:
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  MacCAN-KvaserCAN IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF MacCAN-KvaserCAN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  GNU General Public License v3.0 or later:
 *  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MacCAN-KvaserCAN.  If not, see <http://www.gnu.org/licenses/>.
 */
#import "Settings.h"
#import "KvaserCAN_Driver.h"
#import <XCTest/XCTest.h>

#ifndef TEST_UV_BUS_PARAMS
#define TEST_UV_BUS_PARAMS  0
#endif
#define KV_BUSPARAMS_1M(x)    do { x.bitRate=1000000; x.tseg1=5;  x.tseg2=2; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define UV_BUSPARAMS_800K(x)  do { x.bitRate=800000;  x.tseg1=7;  x.tseg2=2; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_500K(x)  do { x.bitRate=500000;  x.tseg1=5;  x.tseg2=2; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_250K(x)  do { x.bitRate=250000;  x.tseg1=5;  x.tseg2=2; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_125K(x)  do { x.bitRate=125000;  x.tseg1=11; x.tseg2=4; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_100K(x)  do { x.bitRate=100000;  x.tseg1=11; x.tseg2=4; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_83K(x)   do { x.bitRate=83333;   x.tseg1=5;  x.tseg2=2; x.sjw=2; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_62K(x)   do { x.bitRate=62500;   x.tseg1=11; x.tseg2=4; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_50K(x)   do { x.bitRate=50000;   x.tseg1=11; x.tseg2=4; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define UV_BUSPARAMS_20K(x)   do { x.bitRate=20000;   x.tseg1=11; x.tseg2=4; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_10K(x)   do { x.bitRate=10000;   x.tseg1=11; x.tseg2=4; x.sjw=1; x.noSamp=NO_SAMP_VALUE; } while(0)

#define NO_SAMP_VALUE  1
#define NO_SAMP_1_VALID  1
#define NO_SAMP_3_VALID  0

@interface test_drv_BusParams : XCTestCase

@end

@implementation test_drv_BusParams

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
    (void)KvaserCAN_InitializeDriver();
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    (void)KvaserCAN_TeardownDriver();
}

// @xctest TC1.1: ...
//
// @expected: Tbd.
//
- (void)testWithValidBusParams {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); break;
            case 1: KV_BUSPARAMS_500K(parSet); break;
            case 2: KV_BUSPARAMS_250K(parSet); break;
            case 3: KV_BUSPARAMS_125K(parSet); break;
            case 4: KV_BUSPARAMS_100K(parSet); break;
            case 5: KV_BUSPARAMS_83K(parSet); break;
            case 6: KV_BUSPARAMS_62K(parSet); break;
            case 7: KV_BUSPARAMS_50K(parSet); break;
            case 8: KV_BUSPARAMS_10K(parSet); break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); break;
            case 10: UV_BUSPARAMS_20K(parSet); break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- write bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.2: ...
//
// @expected: Tbd.
//
- (void)testWithAllFieldSetToZero {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    parGet.bitRate = parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0;
    
    // @- try to write invalid bus params into DUT1
    NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
        parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    if (retVal != CANUSB_SUCCESS) return;

    // @- if nevertheless successful read bus params from DUT1
    NSLog(@"- GetBusParams()\n");
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    if (retVal != CANUSB_SUCCESS) return;
    NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
        parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

    // @- and compare written and read bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);

    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.3: ...
//
// @expected: Tbd.
//
- (void)testWithField_bitRate_SetToZero {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.bitRate = 0; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.bitRate = 0; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.bitRate = 0; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.bitRate = 0; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.bitRate = 0; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.bitRate = 0; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.bitRate = 0; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.bitRate = 0; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.bitRate = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.bitRate = 0; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.bitRate = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.4: ...
//
// @expected: Tbd.
//
- (void)testWithField_bitRate_SetOnly {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- write valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);  // FIXME: Leaf Light returns OK
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.5: ...
//
// @expected: Tbd.
//
- (void)testWithField_tseg1_SetToZero {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.tseg1 = 0; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.tseg1 = 0; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.tseg1 = 0; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.tseg1 = 0; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.tseg1 = 0; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.tseg1 = 0; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.tseg1 = 0; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.tseg1 = 0; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.tseg1 = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.tseg1 = 0; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.tseg1 = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.6: ...
//
// @expected: Tbd.
//
- (void)testWithField_tseg2_SetToZero {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.tseg2 = 0; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.tseg2 = 0; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.tseg2 = 0; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.tseg2 = 0; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.tseg2 = 0; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.tseg2 = 0; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.tseg2 = 0; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.tseg2 = 0; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.tseg2 = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.tseg2 = 0; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.tseg2 = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.7: ...
//
// @expected: Tbd.
//
- (void)testWithField_tseg2_Invalid {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.tseg2 = 255; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.tseg2 = 128; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.tseg2 = 255; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.tseg2 = 128; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.tseg2 = 255; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.tseg2 = 128; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.tseg2 = 255; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.tseg2 = 128; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.tseg2 = 255; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.tseg2 = 128; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.tseg2 = 255; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.8: ...
//
// @expected: Tbd.
//
- (void)testWithField_sjw_SetToZero {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.sjw = 0; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.sjw = 0; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.sjw = 0; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.sjw = 0; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.sjw = 0; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.sjw = 0; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.sjw = 0; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.sjw = 0; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.sjw = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.sjw = 0; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.sjw = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.9: ...
//
// @expected: Tbd.
//
- (void)testWithField_sjw_Invalid {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.sjw = 255; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.sjw = 128; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.sjw = 255; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.sjw = 128; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.sjw = 255; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.sjw = 128; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.sjw = 255; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.sjw = 128; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.sjw = 255; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.bitRate = 128; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.bitRate = 255; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.10: ...
//
// @expected: Tbd.
//
- (void)testWithField_noSamp_SetToZero {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 0; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 0; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 0; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 0; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 0; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 0; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 0; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 0; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 0; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.11: ...
//
// @expected: Tbd.
//
- (void)testWithField_noSamp_Invalid {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 2; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 4; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 127; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 128; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 255; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 128; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 127; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 4; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 2; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 4; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 255; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- try to write invalid bus params into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if nevertheless successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.12: ...
//
// @expected: Tbd.
//
- (void)testWithField_noSamp_SetToOne {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- write valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test:
    // @- ...
    for (int i = 0, valid = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- write bus params (with valid and invalid 'noSamp') into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        if (valid)
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
        else
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.13: ...
//
// @expected: Tbd.
//
- (void)testWithField_noSamp_SetToThree {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- write valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test:
    // @- ...
    for (int i = 0, valid = 0, leave = 0; !leave; i++) {
        switch (i) {
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
#if (TEST_UV_BUS_PARAMS != 0)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @- write bus params (with valid and invalid 'noSamp') into DUT1
        NSLog(@"- SetBusParams(freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u)\n",
            parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        if (valid)
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
        else
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @- if successful read bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :freq=%u, tseg1=%u, tseg2=%u, sjw=%u, noSam=%u\n",
            parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @- and compare written and read bus params
        XCTAssertEqual(parGet.bitRate, parSet.bitRate);
        XCTAssertEqual(parGet.tseg1, parSet.tseg1);
        XCTAssertEqual(parGet.tseg2, parSet.tseg2);
        XCTAssertEqual(parGet.sjw, parSet.sjw);
        XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    }
    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.14: ...
//
// @expected: Tbd.
//
- (void)testWhenNotInitialized{
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    // @pre:
    // @- set valid bus params (500Kbps)
    KV_BUSPARAMS_500K(parSet);

    // @test:
    // @- try to write valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

    // @post:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- write valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare written and read bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.15: ...
//
// @expected: Tbd.
//
- (void)testWhenAlreadyShutdown{
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500Kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- write valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare written and read bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test:
    // @- try to write valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.16: ...
//
// @expected: Tbd.
//
- (void)testWhileBusOn{
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500Kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- write valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare written and read bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- go bus on
    retVal = KvaserCAN_CanBusOn(&device, false);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test:
    // @- set valid bus params (250Kbps)
    KV_BUSPARAMS_250K(parSet);
    // @- write valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);  // FIXME: not what I would expect
    // @- read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare written and read bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);

    // @post:
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.17: ...
//
// @expected: Tbd.
//
- (void)testWithNullPointerForArgument_device{
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500Kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- try to write bus params with device NULL into DUT1
    retVal = KvaserCAN_SetBusParams(NULL, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to read bus params with device NULL from DUT1
    retVal = KvaserCAN_GetBusParams(NULL, &parGet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

    // @post:
    // @- write valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare written and read bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC1.18: ...
//
// @expected: Tbd.
//
- (void)testWithNullPointerForArgument_params{
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500Kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- try to write bus params with device NULL into DUT1
    retVal = KvaserCAN_SetBusParams(&device, NULL);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to read bus params with device NULL from DUT1
    retVal = KvaserCAN_GetBusParams(&device, NULL);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

    // @post:
    // @- write valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- read bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare written and read bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

@end
