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

// @xctest TC1: Set CAN bus params with valid settings
//
// @expected CANUSB_SUCCESS
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
    
    // @test: loop over selected valid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps
            case 0: KV_BUSPARAMS_1M(parSet); break;
            // @sub(2): 500kbps
            case 1: KV_BUSPARAMS_500K(parSet); break;
            // @sub(3): 250kbps
            case 2: KV_BUSPARAMS_250K(parSet); break;
            // @sub(4): 125kbps
            case 3: KV_BUSPARAMS_125K(parSet); break;
            // @sub(5): 100kbps
            case 4: KV_BUSPARAMS_100K(parSet); break;
            // @sub(6): 83.333kbps
            case 5: KV_BUSPARAMS_83K(parSet); break;
            // @sub(7): 62.5kbps
            case 6: KV_BUSPARAMS_62K(parSet); break;
            // @sub(8): 50kbps
            case 7: KV_BUSPARAMS_50K(parSet); break;
            // @sub(9): 10kbps
            case 8: KV_BUSPARAMS_10K(parSet); break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps (optional)
            case 9: UV_BUSPARAMS_800K(parSet); break;
            // @sub(11): 20kbps (optional)
            case 10: UV_BUSPARAMS_20K(parSet); break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- set bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- compare set and get bus params
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

// @xctest TC2: Set CAN bus params with all fields set to zero
//
// @expected Not CANUSB_SUCCESS
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
    // @- set all fields to zero
    parSet.bitRate = parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0;
    
    // @- try to set invalid bus params into DUT1
    NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
          parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    if (retVal != CANUSB_SUCCESS) return;

    // @- if nevertheless successful get bus params from DUT1
    NSLog(@"- GetBusParams()\n");
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    if (retVal != CANUSB_SUCCESS) return;
    NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
          parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

    // @- and compare set and get bus params
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

// @xctest TC3: Set CAN bus params with field 'bitRate' set to zero
//
// @expected Not CANUSB_SUCCESS
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
    
    // @test: loop over selected invalid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'bitRate' set to 0
            case 0: KV_BUSPARAMS_1M(parSet); parSet.bitRate = 0; break;
            // @sub(2): 500kbps with field 'bitRate' set to 0
            case 1: KV_BUSPARAMS_500K(parSet); parSet.bitRate = 0; break;
            // @sub(3): 250kbps with field 'bitRate' set to 0
            case 2: KV_BUSPARAMS_250K(parSet); parSet.bitRate = 0; break;
            // @sub(4): 125kbps with field 'bitRate' set to 0
            case 3: KV_BUSPARAMS_125K(parSet); parSet.bitRate = 0; break;
            // @sub(5): 100kbps with field 'bitRate' set to 0
            case 4: KV_BUSPARAMS_100K(parSet); parSet.bitRate = 0; break;
            // @sub(6): 83.333kbps with field 'bitRate' set to 0
            case 5: KV_BUSPARAMS_83K(parSet); parSet.bitRate = 0; break;
            // @sub(7): 62.5kbps with field 'bitRate' set to 0
            case 6: KV_BUSPARAMS_62K(parSet); parSet.bitRate = 0; break;
            // @sub(8): 50kbps with field 'bitRate' set to 0
            case 7: KV_BUSPARAMS_50K(parSet); parSet.bitRate = 0; break;
            // @sub(9): 10kbps with field 'bitRate' set to 0
            case 8: KV_BUSPARAMS_10K(parSet); parSet.bitRate = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'bitRate' set to 0 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.bitRate = 0; break;
            // @sub(11): 20kbps with field 'bitRate' set to 0 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.bitRate = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC4: Set CAN bus params with field 'bitRate' set only
//
// @expected Not CANUSB_SUCCESS
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
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test: loop over selected invalid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'bitRate'set only
            case 0: KV_BUSPARAMS_1M(parSet); break;
            // @sub(2): 500kbps with field 'bitRate'set only
            case 1: KV_BUSPARAMS_500K(parSet); break;
            // @sub(3): 250kbps with field 'bitRate'set only
            case 2: KV_BUSPARAMS_250K(parSet); break;
            // @sub(4): 125kbps with field 'bitRate'set only
            case 3: KV_BUSPARAMS_125K(parSet); break;
            // @sub(5): 100kbps with field 'bitRate'set only
            case 4: KV_BUSPARAMS_100K(parSet); break;
            // @sub(6): 83.333kbps with field 'bitRate'set only
            case 5: KV_BUSPARAMS_83K(parSet); break;
            // @sub(7): 62.5kbps with field 'bitRate'set only
            case 6: KV_BUSPARAMS_62K(parSet); break;
            // @sub(8): 50kbps with field 'bitRate'set only
            case 7: KV_BUSPARAMS_50K(parSet); break;
            // @sub(9): 10kbps with field 'bitRate'set only
            case 8: KV_BUSPARAMS_10K(parSet); break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'bitRate'set only (optional)
            case 9: UV_BUSPARAMS_800K(parSet); break;
            // @sub(11): 20kbps with field 'bitRate'set only (optional)
            case 10: UV_BUSPARAMS_20K(parSet); break;
#endif
            default: leave = 1; continue;
        }
        parSet.tseg1 = parSet.tseg2 = parSet.sjw = parSet.noSamp = 0;  // look here!
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);  // FIXME: Leaf Light returns OK
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC5: Set CAN bus params with field 'tseg1' set to zero
//
// @expected Not CANUSB_SUCCESS
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
    
    // @test: loop over selected invalid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'tseg1' set to 0
            case 0: KV_BUSPARAMS_1M(parSet); parSet.tseg1 = 0; break;
            // @sub(2): 500kbps with field 'tseg1' set to 0
            case 1: KV_BUSPARAMS_500K(parSet); parSet.tseg1 = 0; break;
            // @sub(3): 250kbps with field 'tseg1' set to 0
            case 2: KV_BUSPARAMS_250K(parSet); parSet.tseg1 = 0; break;
            // @sub(4): 125kbps with field 'tseg1' set to 0
            case 3: KV_BUSPARAMS_125K(parSet); parSet.tseg1 = 0; break;
            // @sub(5): 100kbps with field 'tseg1' set to 0
            case 4: KV_BUSPARAMS_100K(parSet); parSet.tseg1 = 0; break;
            // @sub(6): 83.333kbps with field 'tseg1' set to 0
            case 5: KV_BUSPARAMS_83K(parSet); parSet.tseg1 = 0; break;
            // @sub(7): 62.5kbps with field 'tseg1' set to 0
            case 6: KV_BUSPARAMS_62K(parSet); parSet.tseg1 = 0; break;
            // @sub(8): 50kbps with field 'tseg1' set to 0
            case 7: KV_BUSPARAMS_50K(parSet); parSet.tseg1 = 0; break;
            // @sub(9): 10kbps with field 'tseg1' set to 0
            case 8: KV_BUSPARAMS_10K(parSet); parSet.tseg1 = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'tseg1' set to 0 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.tseg1 = 0; break;
            // @sub(11): 20kbps with field 'tseg1' set to 0 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.tseg1 = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC6: Set CAN bus params with field 'tseg2' set to zero
//
// @expected Not CANUSB_SUCCESS
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
    
    // @test: loop over selected invalid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'tseg2' set to 0
            case 0: KV_BUSPARAMS_1M(parSet); parSet.tseg2 = 0; break;
            // @sub(2): 500kbps with field 'tseg2' set to 0
            case 1: KV_BUSPARAMS_500K(parSet); parSet.tseg2 = 0; break;
            // @sub(3): 250kbps with field 'tseg2' set to 0
            case 2: KV_BUSPARAMS_250K(parSet); parSet.tseg2 = 0; break;
            // @sub(4): 125kbps with field 'tseg2' set to 0
            case 3: KV_BUSPARAMS_125K(parSet); parSet.tseg2 = 0; break;
            // @sub(5): 100kbps with field 'tseg2' set to 0
            case 4: KV_BUSPARAMS_100K(parSet); parSet.tseg2 = 0; break;
            // @sub(6): 83.333kbps with field 'tseg2' set to 0
            case 5: KV_BUSPARAMS_83K(parSet); parSet.tseg2 = 0; break;
            // @sub(7): 62.5kbps with field 'tseg2' set to 0
            case 6: KV_BUSPARAMS_62K(parSet); parSet.tseg2 = 0; break;
            // @sub(8): 50kbps with field 'tseg2' set to 0
            case 7: KV_BUSPARAMS_50K(parSet); parSet.tseg2 = 0; break;
            // @sub(9): 10kbps with field 'tseg2' set to 0
            case 8: KV_BUSPARAMS_10K(parSet); parSet.tseg2 = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'tseg2' set to 0 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.tseg2 = 0; break;
            // @sub(11): 20kbps with field 'tseg2' set to 0 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.tseg2 = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC7: Set CAN bus params with field 'tseg2' set to invalid value
//
// @expected Not CANUSB_SUCCESS
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
    
    // @test: loop over selected invalid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'tseg2' set to 255
            case 0: KV_BUSPARAMS_1M(parSet); parSet.tseg2 = 255; break;
            // @sub(2): 500kbps with field 'tseg2' set to 129
            case 1: KV_BUSPARAMS_500K(parSet); parSet.tseg2 = 129; break;
            // @sub(3): 250kbps with field 'tseg2' set to 255
            case 2: KV_BUSPARAMS_250K(parSet); parSet.tseg2 = 255; break;
            // @sub(4): 125kbps with field 'tseg2' set to 129
            case 3: KV_BUSPARAMS_125K(parSet); parSet.tseg2 = 129; break;
            // @sub(5): 100kbps with field 'tseg2' set to 255
            case 4: KV_BUSPARAMS_100K(parSet); parSet.tseg2 = 255; break;
            // @sub(6): 83.333kbps with field 'tseg2' set to 129
            case 5: KV_BUSPARAMS_83K(parSet); parSet.tseg2 = 129; break;
            // @sub(7): 62.5kbps with field 'tseg2' set to 255
            case 6: KV_BUSPARAMS_62K(parSet); parSet.tseg2 = 255; break;
            // @sub(8): 50kbps with field 'tseg2' set to 129
            case 7: KV_BUSPARAMS_50K(parSet); parSet.tseg2 = 129; break;
            // @sub(9): 10kbps with field 'tseg2' set to 255
            case 8: KV_BUSPARAMS_10K(parSet); parSet.tseg2 = 255; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'tseg2' set to 129 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.tseg2 = 129; break;
            // @sub(11): 20kbps with field 'tseg2' set to 255 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.tseg2 = 255; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC8: Set CAN bus params with field 'sjw' set to zero
//
// @expected Not CANUSB_SUCCESS
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
    
    // @test:/ @- ...
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'sjw' set to 0
            case 0: KV_BUSPARAMS_1M(parSet); parSet.sjw = 0; break;
            // @sub(2): 500kbps with field 'sjw' set to 0
            case 1: KV_BUSPARAMS_500K(parSet); parSet.sjw = 0; break;
            // @sub(3): 250kbps with field 'sjw' set to 0
            case 2: KV_BUSPARAMS_250K(parSet); parSet.sjw = 0; break;
            // @sub(4): 125kbps with field 'sjw' set to 0
            case 3: KV_BUSPARAMS_125K(parSet); parSet.sjw = 0; break;
            // @sub(5): 100kbps with field 'sjw' set to 0
            case 4: KV_BUSPARAMS_100K(parSet); parSet.sjw = 0; break;
            // @sub(6): 83.333kbps with field 'sjw' set to 0
            case 5: KV_BUSPARAMS_83K(parSet); parSet.sjw = 0; break;
            // @sub(7): 62.5kbps with field 'sjw' set to 0
            case 6: KV_BUSPARAMS_62K(parSet); parSet.sjw = 0; break;
            // @sub(8): 50kbps with field 'sjw' set to 0
            case 7: KV_BUSPARAMS_50K(parSet); parSet.sjw = 0; break;
            // @sub(9): 10kbps with field 'sjw' set to 0
            case 8: KV_BUSPARAMS_10K(parSet); parSet.sjw = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'sjw' set to 0 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.sjw = 0; break;
            // @sub(11): 20kbps with field 'sjw' set to 0 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.sjw = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC9: Set CAN bus params with field 'sjw' set to invalid value
//
// @expected Not CANUSB_SUCCESS
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
            // @sub(1): 1Mbps with field 'sjw' set to 255
            case 0: KV_BUSPARAMS_1M(parSet); parSet.sjw = 255; break;
            // @sub(2): 500kbps with field 'sjw' set to 129
            case 1: KV_BUSPARAMS_500K(parSet); parSet.sjw = 129; break;
            // @sub(3): 250kbps with field 'sjw' set to 255
            case 2: KV_BUSPARAMS_250K(parSet); parSet.sjw = 255; break;
            // @sub(4): 125kbps with field 'sjw' set to 129
            case 3: KV_BUSPARAMS_125K(parSet); parSet.sjw = 129; break;
            // @sub(5): 100kbps with field 'sjw' set to 255
            case 4: KV_BUSPARAMS_100K(parSet); parSet.sjw = 255; break;
            // @sub(6): 83.333kbps with field 'sjw' set to 129
            case 5: KV_BUSPARAMS_83K(parSet); parSet.sjw = 129; break;
            // @sub(7): 62.5kbps with field 'sjw' set to 255
            case 6: KV_BUSPARAMS_62K(parSet); parSet.sjw = 255; break;
            // @sub(8): 50kbps with field 'sjw' set to 129
            case 7: KV_BUSPARAMS_50K(parSet); parSet.sjw = 129; break;
            // @sub(9): 10kbps with field 'sjw' set to 255
            case 8: KV_BUSPARAMS_10K(parSet); parSet.sjw = 255; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'sjw' set to 129 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.sjw = 129; break;
            // @sub(11): 20kbps with field 'sjw' set to 255 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.sjw = 255; break;
#endif
             default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC10: Set CAN bus params with field 'noSamp' set to zero
//
// @expected Not CANUSB_SUCCESS
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
    
    // @test: loop over selected invalid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'noSamp' set to 0
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 0; break;
            // @sub(2): 500kbps with field 'noSamp' set to 0
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 0; break;
            // @sub(3): 250kbps with field 'noSamp' set to 0
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 0; break;
            // @sub(4): 125kbps with field 'noSamp' set to 0
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 0; break;
            // @sub(5): 100kbps with field 'noSamp' set to 0
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 0; break;
            // @sub(6): 83.333kbps with field 'noSamp' set to 0
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 0; break;
            // @sub(7): 62.5kbps with field 'noSamp' set to 0
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 0; break;
            // @sub(8): 50kbps with field 'noSamp' set to 0
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 0; break;
            // @sub(9): 10kbps with field 'noSamp' set to 0
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 0; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'noSamp' set to 0 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 0; break;
            // @sub(11): 20kbps with field 'noSamp' set to 0 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 0; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC11: Set CAN bus params with field 'noSamp' set to invalid value
//
// @expected Not CANUSB_SUCCESS
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
    
    // @test: loop over selected invalid bus params
    for (int i = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'noSamp' set to 2
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 2; break;
            // @sub(2): 500kbps with field 'noSamp' set to 4
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 4; break;
            // @sub(3): 250kbps with field 'noSamp' set to 127
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 127; break;
            // @sub(4): 125kbps with field 'noSamp' set to 128
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 128; break;
            // @sub(5): 100kbps with field 'noSamp' set to 255
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 255; break;
            // @sub(6): 83.333kbps with field 'noSamp' set to 129
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 129; break;
            // @sub(7): 62.5kbps with field 'noSamp' set to 128
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 128; break;
            // @sub(8): 50kbps with field 'noSamp' set to 4
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 4; break;
            // @sub(9): 10kbps with field 'noSamp' set to 2
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 2; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'noSamp' set to 4 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 4; break;
            // @sub(11): 20kbps with field 'noSamp' set to 255 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 255; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set invalid bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if nevertheless successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC12: Set CAN bus params with field 'noSamp' set to one
//
// @note: value 1 seems to be the only valid value for field 'noSamp'.
//
// @expected CANUSB_SUCCESS
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
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test: loop over selected valid bus params
    for (int i = 0, valid = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'noSamp' set to 1
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(2): 500kbps with field 'noSamp' set to 1
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(3): 250kbps with field 'noSamp' set to 1
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(4): 125kbps with field 'noSamp' set to 1
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(5): 100kbps with field 'noSamp' set to 1
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(6): 83.333kbps with field 'noSamp' set to 1
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(7): 62.5kbps with field 'noSamp' set to 1
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(8): 50kbps with field 'noSamp' set to 1
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(9): 10kbps with field 'noSamp' set to 1
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'noSamp' set to 1 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
            // @sub(11): 20kbps with field 'noSamp' set to 1 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 1; valid = NO_SAMP_1_VALID; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- set (or try to set) bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        if (valid)
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
        else
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC13: Set CAN bus params with field 'noSamp' set to three
//
// @note: value 1 seems to be the only valid value for field 'noSamp', but who knows.
//
// @expected CANUSB_SUCCESS
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
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test: loop over selected invalid bus params
    for (int i = 0, valid = 0, leave = 0; !leave; i++) {
        switch (i) {
            // @sub(1): 1Mbps with field 'noSamp' set to 3
            case 0: KV_BUSPARAMS_1M(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(2): 500kbps with field 'noSamp' set to 3
            case 1: KV_BUSPARAMS_500K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(3): 250kbps with field 'noSamp' set to 3
            case 2: KV_BUSPARAMS_250K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(4): 125kbps with field 'noSamp' set to 3
            case 3: KV_BUSPARAMS_125K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(5): 100kbps with field 'noSamp' set to 3
            case 4: KV_BUSPARAMS_100K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(6): 83.333kbps with field 'noSamp' set to 3
            case 5: KV_BUSPARAMS_83K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(7): 62.5kbps with field 'noSamp' set to 3
            case 6: KV_BUSPARAMS_62K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(8): 50kbps with field 'noSamp' set to 3
            case 7: KV_BUSPARAMS_50K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(9): 10kbps with field 'noSamp' set to 3
            case 8: KV_BUSPARAMS_10K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
#if (TEST_UV_BUS_PARAMS != 0)
            // @sub(10): 800kbps with field 'noSamp' set to 3 (optional)
            case 9: UV_BUSPARAMS_800K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
            // @sub(11): 20kbps with field 'noSamp' set to 3 (optional)
            case 10: UV_BUSPARAMS_20K(parSet); parSet.noSamp = 3; valid = NO_SAMP_3_VALID; break;
#endif
            default: leave = 1; continue;
        }
        NSLog(@"Execute sub-testcase %d:\n", i+1);

        // @-- try to set (or set) bus params into DUT1
        NSLog(@"- SetBusParams(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
              parSet.bitRate, parSet.tseg1, parSet.tseg2, parSet.sjw, parSet.noSamp);
        retVal = KvaserCAN_SetBusParams(&device, &parSet);
        if (valid)
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
        else
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;

        // @-- if successful get bus params from DUT1
        NSLog(@"- GetBusParams()\n");
        retVal = KvaserCAN_GetBusParams(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        if (retVal != CANUSB_SUCCESS) continue;
        NSLog(@"              :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
              parGet.bitRate, parGet.tseg1, parGet.tseg2, parGet.sjw, parGet.noSamp);

        // @-- and compare set and get bus params
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

// @xctest TC14: Set CAN bus params when device is not initialized
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWhenNotInitialized {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

    // @pre:
    // @- set valid bus params (500kbps)
    KV_BUSPARAMS_500K(parSet);

    // @test:
    // @- try to set valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

    // @post:
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- set valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare set and get bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC15: Set CAN bus params when device already shutdown
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWhenAlreadyShutdown {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare set and get bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test:
    // @- try to set valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC16: Set CAN bus params while device is bus on
//
// @expected CANUSB_SUCCESS
//
- (void)testWhileBusOn {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare set and get bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- go bus on
    retVal = KvaserCAN_CanBusOn(&device, false);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);

    // @test:
    // @- set valid bus params (250kbps)
    KV_BUSPARAMS_250K(parSet);
    // @- set valid bus params into DUT1
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);  // FIXME: not what I would expect
    // @- get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare set and get bus params
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

// @xctest TC17: Give a NULL pointer as argument for parameter 'device'
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithNullPointerForArgument_device {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- try to set bus params with 'device = NULL'
    retVal = KvaserCAN_SetBusParams(NULL, &parSet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to get bus params with 'device = NULL'
    retVal = KvaserCAN_GetBusParams(NULL, &parGet);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

    // @post:
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare set and get bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC18: Give a NULL pointer as argument for parameter 'params'
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithNullPointerForArgument_params {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- try to set bus params with 'params = NULL' into DUT1
    retVal = KvaserCAN_SetBusParams(&device, NULL);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to get bus params with 'params = NULL' from DUT1
    retVal = KvaserCAN_GetBusParams(&device, NULL);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

    // @post:
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare set and get bus params
    XCTAssertEqual(parGet.bitRate, parSet.bitRate);
    XCTAssertEqual(parGet.tseg1, parSet.tseg1);
    XCTAssertEqual(parGet.tseg2, parSet.tseg2);
    XCTAssertEqual(parGet.sjw, parSet.sjw);
    XCTAssertEqual(parGet.noSamp, parSet.noSamp);
    // @- shutdown DUT1
    retVal = KvaserCAN_TeardownChannel(&device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
}

// @xctest TC19: Give NULL pointers as argument for both parameter
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithNullPointerForBothArguments {
    KvaserUSB_Device_t device = {};
    KvaserUSB_BusParams_t parSet = {};
    KvaserUSB_BusParams_t parGet = {};
    CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
    
    // @pre:
    // @- set valid bus params (500kbps)
    KV_BUSPARAMS_500K(parSet);
    // @- initialize DUT1 with configured settings
    retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    
    // @test:
    // @- try to set bus params with all arguments NULL
    retVal = KvaserCAN_SetBusParams(NULL, NULL);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    // @- try to get bus params with all arguments NULL
    retVal = KvaserCAN_GetBusParams(NULL, NULL);
    XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

    // @post:
    // @- set valid bus params into DUT1
    KV_BUSPARAMS_500K(parSet);
    retVal = KvaserCAN_SetBusParams(&device, &parSet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- get bus params from DUT1
    retVal = KvaserCAN_GetBusParams(&device, &parGet);
    XCTAssertEqual(CANUSB_SUCCESS, retVal);
    // @- compare set and get bus params
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
