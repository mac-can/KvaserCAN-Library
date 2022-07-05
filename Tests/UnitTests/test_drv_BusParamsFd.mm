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

#define KV_BUSPARAMS_1M(x)       do { x.canFd=false; x.nominal.bitRate=1000000; x.nominal.tseg1=31;  x.nominal.tseg2= 8; x.nominal.sjw= 8; x.nominal.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_500K(x)     do { x.canFd=false; x.nominal.bitRate= 500000; x.nominal.tseg1=63;  x.nominal.tseg2=16; x.nominal.sjw=16; x.nominal.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_250K(x)     do { x.canFd=false; x.nominal.bitRate= 250000; x.nominal.tseg1=63;  x.nominal.tseg2=16; x.nominal.sjw=16; x.nominal.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_125K(x)     do { x.canFd=false; x.nominal.bitRate= 125000; x.nominal.tseg1=63;  x.nominal.tseg2=16; x.nominal.sjw=16; x.nominal.noSamp=NO_SAMP_VALUE; } while(0)

#define KV_BUSPARAMS_1M_8M(x)    do { x.canFd= true; x.nominal.bitRate=1000000; x.nominal.tseg1=31;  x.nominal.tseg2= 8; x.nominal.sjw= 8; x.nominal.noSamp=NO_SAMP_VALUE; \
                                                        x.data.bitRate=8000000;    x.data.tseg1= 7;     x.data.tseg2= 2;    x.data.sjw= 2;    x.data.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_500K_4M(x)  do { x.canFd= true; x.nominal.bitRate= 500000; x.nominal.tseg1=63;  x.nominal.tseg2=16; x.nominal.sjw=16; x.nominal.noSamp=NO_SAMP_VALUE; \
                                                        x.data.bitRate=4000000;    x.data.tseg1= 7;     x.data.tseg2= 2;    x.data.sjw= 2;    x.data.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_250K_2M(x)  do { x.canFd= true; x.nominal.bitRate= 250000; x.nominal.tseg1=63;  x.nominal.tseg2=16; x.nominal.sjw=16; x.nominal.noSamp=NO_SAMP_VALUE; \
                                                        x.data.bitRate=2000000;    x.data.tseg1=15;     x.data.tseg2= 4;    x.data.sjw= 4;    x.data.noSamp=NO_SAMP_VALUE; } while(0)
#define KV_BUSPARAMS_125K_1M(x)  do { x.canFd= true; x.nominal.bitRate= 125000; x.nominal.tseg1=63;  x.nominal.tseg2=16; x.nominal.sjw=16; x.nominal.noSamp=NO_SAMP_VALUE; \
                                                        x.data.bitRate=1000000;    x.data.tseg1=31;     x.data.tseg2= 8;    x.data.sjw= 8;    x.data.noSamp=NO_SAMP_VALUE; } while(0)

#define NO_SAMP_VALUE  1
#define NO_SAMP_1_VALID  1
#define NO_SAMP_3_VALID  0

@interface test_drv_BusParamsFd : XCTestCase

@end

@implementation test_drv_BusParamsFd

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
    (void)KvaserCAN_InitializeDriver();
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    (void)KvaserCAN_TeardownDriver();
}

// @xctest TC1: Set CAN FD bus params with valid settings
//
// @expected CANUSB_SUCCESS
//
- (void)testWithValidBusParams {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected valid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps
                case 0: KV_BUSPARAMS_1M(parSet); break;
                // @sub(2): 500kbps
                case 1: KV_BUSPARAMS_500K(parSet); break;
                // @sub(3): 250kbps
                case 2: KV_BUSPARAMS_250K(parSet); break;
                // @sub(4): 125kbps
                case 3: KV_BUSPARAMS_125K(parSet); break;
                // @sub(5): 1Mbps : 8Mbps
                case 4: KV_BUSPARAMS_1M_8M(parSet); break;
                // @sub(6): 500kbps : 4Mbps
                case 5: KV_BUSPARAMS_500K_4M(parSet); break;
                // @sub(7): 250kbps : 2Mbps
                case 6: KV_BUSPARAMS_250K_2M(parSet); break;
                // @sub(8): 125kbps : 1Mbps
                case 7: KV_BUSPARAMS_125K_1M(parSet); break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- set bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC2: Set CAN FD bus params with all fields set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithAllFieldSetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: set all fields to zero
        for (int i = 0; i < 2; i++) {
            parSet.nominal.bitRate = parSet.nominal.tseg1 = parSet.nominal.tseg2 = parSet.nominal.sjw = parSet.nominal.noSamp = 0;
            parSet.data.bitRate = parSet.data.tseg1 = parSet.data.tseg2 = parSet.data.sjw = parSet.data.noSamp = 0;
            switch (i) {
                // @sub(1): and field 'canFd' false
                case 0: parSet.canFd = false; break;
                // @sub(2): and field 'canFd' true
                case 1: parSet.canFd = true; break;
                default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC3: Set CAN FD bus params with field 'nominal.bitRate' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_bitRate_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.bitRate' set to 0
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.bitRate = 0; break;
                // @sub(2): 500kbps with field 'nominal.bitRate' set to 0
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.bitRate = 0; break;
                // @sub(3): 250kbps with field 'nominal.bitRate' set to 0
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.bitRate = 0; break;
                // @sub(4): 125kbps with field 'nominal.bitRate' set to 0
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.bitRate = 0; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.bitRate' set to 0
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.bitRate = 0; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.bitRate' set to 0
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.bitRate = 0; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.bitRate' set to 0
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.bitRate = 0; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.bitRate' set to 0
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.bitRate = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC4: Set CAN FD bus params with field 'nominal.bitRate' set only
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_bitRate_SetOnly {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.bitRate' set only
                case 0: KV_BUSPARAMS_1M(parSet); break;
                // @sub(2): 500kbps with field 'nominal.bitRate' set only
                case 1: KV_BUSPARAMS_500K(parSet); break;
                // @sub(3): 250kbps with field 'nominal.bitRate' set only
                case 2: KV_BUSPARAMS_250K(parSet); break;
                // @sub(4): 125kbps with field 'nominal.bitRate' set only
                case 3: KV_BUSPARAMS_125K(parSet); break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.bitRate' set only
                case 4: KV_BUSPARAMS_1M_8M(parSet); break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.bitRate' set only
                case 5: KV_BUSPARAMS_500K_4M(parSet); break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.bitRate' set only
                case 6: KV_BUSPARAMS_250K_2M(parSet); break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.bitRate' set only
                case 7: KV_BUSPARAMS_125K_1M(parSet); break;
               default: return;  // Get out of here!
            }
            parSet.nominal.tseg1 = parSet.nominal.tseg2 = parSet.nominal.sjw = parSet.nominal.noSamp = 0;  // look here!
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC5: Set CAN FD bus params with field 'nominal.tseg1' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_tseg1_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.tseg1' set to 0
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.tseg1 = 0; break;
                // @sub(2): 500kbps with field 'nominal.tseg1' set to 0
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.tseg1 = 0; break;
                // @sub(3): 250kbps with field 'nominal.tseg1' set to 0
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.tseg1 = 0; break;
                // @sub(4): 125kbps with field 'nominal.tseg1' set to 0
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.tseg1 = 0; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.tseg1' set to 0
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.tseg1 = 0; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.tseg1' set to 0
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.tseg1 = 0; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.tseg1' set to 0
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.tseg1 = 0; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.tseg1' set to 0
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.tseg1 = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC6: Set CAN FD bus params with field 'nominal.tseg2' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_tseg2_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.tseg2' set to 0
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.tseg2 = 0; break;
                // @sub(2): 500kbps with field 'nominal.tseg2' set to 0
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.tseg2 = 0; break;
                // @sub(3): 250kbps with field 'nominal.tseg2' set to 0
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.tseg2 = 0; break;
                // @sub(4): 125kbps with field 'nominal.tseg2' set to 0
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.tseg2 = 0; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.tseg2' set to 0
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.tseg2 = 0; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.tseg2' set to 0
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.tseg2 = 0; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.tseg2' set to 0
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.tseg2 = 0; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.tseg2' set to 0
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.tseg2 = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC7: Set CAN FD bus params with field 'nominal.tseg2' set to invalid value
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_tseg2_Invalid {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.tseg2' set to 129
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.tseg2 = 129; break;
                // @sub(2): 500kbps with field 'nominal.tseg2' set to 255
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.tseg2 = 255; break;
                // @sub(3): 250kbps with field 'nominal.tseg2' set to 129
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.tseg2 = 129; break;
                // @sub(4): 125kbps with field 'nominal.tseg2' set to 255
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.tseg2 = 255; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.tseg2' set to 255
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.tseg2 = 255; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.tseg2' set to 129
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.tseg2 = 129; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.tseg2' set to 255
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.tseg2 = 255; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.tseg2' set to 129
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.tseg2 = 129; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC8: Set CAN FD bus params with field 'nominal.sjw' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_sjw_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.sjw' set to 0
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.sjw = 0; break;
                // @sub(2): 500kbps with field 'nominal.sjw' set to 0
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.sjw = 0; break;
                // @sub(3): 250kbps with field 'nominal.sjw' set to 0
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.sjw = 0; break;
                // @sub(4): 125kbps with field 'nominal.sjw' set to 0
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.sjw = 0; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.sjw' set to 0
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.sjw = 0; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.sjw' set to 0
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.sjw = 0; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.sjw' set to 0
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.sjw = 0; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.sjw' set to 0
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.sjw = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC9: Set CAN FD bus params with field 'nominal.sjw' set to invalid value
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_sjw_Invalid {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.sjw' set to 129
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.sjw = 129; break;
                // @sub(2): 500kbps with field 'nominal.sjw' set to 255
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.sjw = 255; break;
                // @sub(3): 250kbps with field 'nominal.sjw' set to 129
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.sjw = 129; break;
                // @sub(4): 125kbps with field 'nominal.sjw' set to 255
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.sjw = 255; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.sjw' set to 255
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.sjw = 255; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.sjw' set to 129
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.sjw = 129; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.sjw' set to 255
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.sjw = 255; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.sjw' set to 129
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.sjw = 129; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC10: Set CAN FD bus params with field 'nominal.noSamp' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_noSamp_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.noSamp' set to 1
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.noSamp = 0; break;
                // @sub(2): 500kbps with field 'nominal.noSamp' set to 0
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.noSamp = 0; break;
                // @sub(3): 250kbps with field 'nominal.noSamp' set to 0
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.noSamp = 0; break;
                // @sub(4): 125kbps with field 'nominal.noSamp' set to 0
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.noSamp = 0; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.noSamp' set to 0
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.noSamp = 0; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.noSamp' set to 0
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.noSamp = 0; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.noSamp' set to 0
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.noSamp = 0; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.noSamp' set to 0
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.noSamp = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC11: Set CAN FD bus params with field 'nominal.noSamp' set to invalid value
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_noSamp_Invalid {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.noSamp' set to 2
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.noSamp = 2; break;
                // @sub(2): 500kbps with field 'nominal.noSamp' set to 4
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.noSamp = 4; break;
                // @sub(3): 250kbps with field 'nominal.noSamp' set to 128
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.noSamp = 128; break;
                // @sub(4): 125kbps with field 'nominal.noSamp' set to 255
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.noSamp = 255; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.noSamp' set to 255
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.noSamp = 255; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.noSamp' set to 128
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.noSamp = 128; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.noSamp' set to 4
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.noSamp = 4; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.noSamp' set to 2
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.noSamp = 2; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC12: Set CAN FD bus params with field 'nominal.noSamp' set to one
//
// @note: value 1 seems to be the only valid value for field 'noSamp'.
//
// @expected CANUSB_SUCCESS
//
- (void)testWithField_noSamp_SetToOne {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected valid bus params
        for (int i = 0, valid = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.noSamp' set to 1
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(2): 500kbps with field 'nominal.noSamp' set to 1
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(3): 250kbps with field 'nominal.noSamp' set to 1
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(4): 125kbps with field 'nominal.noSamp' set to 1
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.noSamp' set to 1
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.noSamp' set to 1
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.noSamp' set to 1
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.noSamp' set to 1
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.noSamp = 1; valid = NO_SAMP_1_VALID; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- set (or try to set) bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            if (valid)
                XCTAssertEqual(CANUSB_SUCCESS, retVal);
            else
                XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC13: Set CAN FD bus params with field 'nominal.noSamp' set to three
//
// @note: value 1 seems to be the only valid value for field 'noSamp', but who knows.
//
// @expected CANUSB_SUCCESS
//
- (void)testWithField_noSamp_SetToThree {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0, valid = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps with field 'nominal.noSamp' set to 3
                case 0: KV_BUSPARAMS_1M(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(2): 500kbps with field 'nominal.noSamp' set to 3
                case 1: KV_BUSPARAMS_500K(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(3): 250kbps with field 'nominal.noSamp' set to 3
                case 2: KV_BUSPARAMS_250K(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(4): 125kbps with field 'nominal.noSamp' set to 3
                case 3: KV_BUSPARAMS_125K(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(5): 1Mbps : 8Mbps with field 'nominal.noSamp' set to 3
                case 4: KV_BUSPARAMS_1M_8M(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(6): 500kbps : 4Mbps with field 'nominal.noSamp' set to 3
                case 5: KV_BUSPARAMS_500K_4M(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(7): 250kbps : 2Mbps with field 'nominal.noSamp' set to 3
                case 6: KV_BUSPARAMS_250K_2M(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(8): 125kbps : 1Mbps with field 'nominal.noSamp' set to 3
                case 7: KV_BUSPARAMS_125K_1M(parSet); parSet.nominal.noSamp = 3; valid = NO_SAMP_3_VALID; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set (or set) bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            if (valid)
                XCTAssertEqual(CANUSB_SUCCESS, retVal);
            else
                XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC14: Set CAN FD bus params with field 'data.bitRate' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_bitRate_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.bitRate' set to 0
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.bitRate = 0; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.bitRate' set to 0
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.bitRate = 0; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.bitRate' set to 0
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.bitRate = 0; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.bitRate' set to 0
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.bitRate = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC15: Set CAN FD bus params with field 'data.bitRate' set only
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_bitRate_SetOnly {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.bitRate' set only
                case 0: KV_BUSPARAMS_1M_8M(parSet); break;
                // @sub(2): 500kbps : 4Mbps with field 'data.bitRate' set only
                case 1: KV_BUSPARAMS_500K_4M(parSet); break;
                // @sub(3): 250kbps : 2Mbps with field 'data.bitRate' set only
                case 2: KV_BUSPARAMS_250K_2M(parSet); break;
                // @sub(4): 125kbps : 1Mbps with field 'data.bitRate' set only
                case 3: KV_BUSPARAMS_125K_1M(parSet); break;
               default: return;  // Get out of here!
            }
            parSet.data.tseg1 = parSet.data.tseg2 = parSet.data.sjw = parSet.data.noSamp = 0;  // look here!
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC16: Set CAN FD bus params with field 'data.tseg1' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_tseg1_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.tseg1' set to 0
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.tseg1 = 0; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.tseg1' set to 0
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.tseg1 = 0; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.tseg1' set to 0
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.tseg1 = 0; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.tseg1' set to 0
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.tseg1 = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC17: Set CAN FD bus params with field 'data.tseg1' set to invalid value
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_tseg1_Invalid {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.tseg1' set to 33
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.tseg1 = 33; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.tseg1' set to 255
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.tseg1 = 255; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.tseg1' set to 255
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.tseg1 = 255; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.tseg1' set to 33
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.tseg1 = 33; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC18: Set CAN FD bus params with field 'data.tseg2' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_tseg2_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.tseg2' set to 0
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.tseg2 = 0; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.tseg2' set to 0
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.tseg2 = 0; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.tseg2' set to 0
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.tseg2 = 0; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.tseg2' set to 0
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.tseg2 = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC19: Set CAN FD bus params with field 'data.tseg2' set to invalid value
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_tseg2_Invalid {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.tseg2' set to 255
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.tseg2 = 255; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.tseg2' set to 17
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.tseg2 = 17; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.tseg2' set to 255
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.tseg2 = 255; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.tseg2' set to 17
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.tseg2 = 17; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC20: Set CAN FD bus params with field 'data.sjw' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_sjw_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.sjw' set to 0
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.sjw = 0; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.sjw' set to 0
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.sjw = 0; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.sjw' set to 0
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.sjw = 0; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.sjw' set to 0
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.sjw = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC21: Set CAN FD bus params with field 'data.sjw' set to invalid value
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_sjw_Invalid {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.sjw' set to 255
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.sjw = 255; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.sjw' set to 17
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.sjw = 17; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.sjw' set to 255
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.sjw = 255; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.sjw' set to 17
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.sjw = 17; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC22: Set CAN FD bus params with field 'data.noSamp' set to zero
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_noSamp_SetToZero {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.noSamp' set to 0
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.noSamp = 0; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.noSamp' set to 0
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.noSamp = 0; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.noSamp' set to 0
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.noSamp = 0; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.noSamp' set to 0
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.noSamp = 0; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC23: Set CAN FD bus params with field 'data.noSamp' set to invalid value
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithField_data_noSamp_Invalid {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0; i < 4; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.noSamp' set to 255
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.noSamp = 255; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.noSamp' set to 128
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.noSamp = 128; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.noSamp' set to 4
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.noSamp = 4; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.noSamp' set to 2
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.noSamp = 2; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set invalid bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if nevertheless successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC24: Set CAN FD bus params with field 'data.noSamp' set to one
//
// @note: value 1 seems to be the only valid value for field 'noSamp'.
//
// @expected CANUSB_SUCCESS
//
- (void)testWithField_data_noSamp_SetToOne {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected valid bus params
        for (int i = 0, valid = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.noSamp' set to 1
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.noSamp' set to 1
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.noSamp' set to 1
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.noSamp = 1; valid = NO_SAMP_1_VALID; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.noSamp' set to 1
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.noSamp = 1; valid = NO_SAMP_1_VALID; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- set (or try to set) bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            if (valid)
                XCTAssertEqual(CANUSB_SUCCESS, retVal);
            else
                XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC25: Set CAN FD bus params with field 'data.noSamp' set to three
//
// @note: value 1 seems to be the only valid value for field 'noSamp', but who knows.
//
// @expected CANUSB_SUCCESS
//
- (void)testWithField_data_noSamp_SetToThree {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test: loop over selected invalid bus params
        for (int i = 0, valid = 0; i < 8; i++) {
            switch (i) {
                // @sub(1): 1Mbps : 8Mbps with field 'data.noSamp' set to 3
                case 0: KV_BUSPARAMS_1M_8M(parSet); parSet.data.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(2): 500kbps : 4Mbps with field 'data.noSamp' set to 3
                case 1: KV_BUSPARAMS_500K_4M(parSet); parSet.data.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(3): 250kbps : 2Mbps with field 'data.noSamp' set to 3
                case 2: KV_BUSPARAMS_250K_2M(parSet); parSet.data.noSamp = 3; valid = NO_SAMP_3_VALID; break;
                // @sub(4): 125kbps : 1Mbps with field 'data.noSamp' set to 3
                case 3: KV_BUSPARAMS_125K_1M(parSet); parSet.data.noSamp = 3; valid = NO_SAMP_3_VALID; break;
               default: return;  // Get out of here!
            }
            NSLog(@"Execute sub-testcase %d:\n", i+1);

            // @-- try to set (or set) bus params into DUT1
            if (parSet.canFd) {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.data.bitRate, parSet.data.tseg1, parSet.data.tseg2, parSet.data.sjw, parSet.data.noSamp);
            } else {
                NSLog(@"- SetBusParamsFd(bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u)\n",
                      parSet.nominal.bitRate, parSet.nominal.tseg1, parSet.nominal.tseg2, parSet.nominal.sjw, parSet.nominal.noSamp);
            }
            retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
            if (valid)
                XCTAssertEqual(CANUSB_SUCCESS, retVal);
            else
                XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;

            // @-- if successful get bus params from DUT1
            NSLog(@"- GetBusParamsFd()\n");
            retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
            XCTAssertEqual(CANUSB_SUCCESS, retVal);
            if (retVal != CANUSB_SUCCESS) continue;
            if (parSet.canFd) {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u,\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
                NSLog(@"                 bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.data.bitRate, parGet.data.tseg1, parGet.data.tseg2, parGet.data.sjw, parGet.data.noSamp);
            } else {
                NSLog(@"                :bitRate=%7u, tseg1=%2u, tseg2=%2u, sjw=%2u, noSamp=%u\n",
                      parGet.nominal.bitRate, parGet.nominal.tseg1, parGet.nominal.tseg2, parGet.nominal.sjw, parGet.nominal.noSamp);
            }
            // @-- and compare set and get bus params
            XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
            XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
            XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
            XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
            XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
            // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
            if (parSet.canFd) {
                XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
                XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
                XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
                XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
                XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
            }
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC26: Set CAN FD bus params when device is not initialized
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWhenNotInitialized {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;

        // @pre:
        // @- set valid bus params (500kbps : 4Mbps)
        KV_BUSPARAMS_500K_4M(parSet);

        // @test:
        // @- try to set valid bus params into DUT1
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        // @- try to get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

        // @post:
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- set valid bus params into DUT1
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- compare set and get bus params
        XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
        XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
        XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
        XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
        XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
        // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
        if (parSet.canFd) {
            XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
            XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
            XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
            XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
            XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
        }
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);

    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC27: Set CAN FD bus params when device already shutdown
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWhenAlreadyShutdown{
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- set valid bus params (500kbps : 4Mbps)
        KV_BUSPARAMS_500K_4M(parSet);
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- set valid bus params into DUT1
        KV_BUSPARAMS_500K(parSet);
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- compare set and get bus params
        XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
        XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
        XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
        XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
        XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
        // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
        if (parSet.canFd) {
            XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
            XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
            XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
            XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
            XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
        }
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);

        // @test:
        // @- try to set valid bus params into DUT1
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        // @- try to get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC28: Set CAN FD bus params while device is bus on
//
// @expected CANUSB_SUCCESS
//
- (void)testWhileBusOn {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- set valid bus params (500kbps : 4Mbps)
        KV_BUSPARAMS_500K_4M(parSet);
        // @- initialize DUT1 with configured settings
        retVal = KvaserCAN_InitializeChannel(DUT1, TEST_CANMODE, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- set valid bus params into DUT1
        KV_BUSPARAMS_500K(parSet);
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- compare set and get bus params
        XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
        XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
        XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
        XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
        XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
        // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
        if (parSet.canFd) {
            XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
            XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
            XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
            XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
            XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
        }
        // @- go bus on
        retVal = KvaserCAN_CanBusOn(&device, false);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);

        // @test:
        // @- set valid bus params (250kbps : 2Mbps)
        KV_BUSPARAMS_250K_2M(parSet);
        // @- set valid bus params into DUT1
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);  // FIXME: not what I would expect
        // @- get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- compare set and get bus params
        XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
        XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
        XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
        XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
        XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
        // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
        if (parSet.canFd) {
            XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
            XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
            XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
            XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
            XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
        }
        // @post:
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC29: Give a NULL pointer as argument for parameter 'device'
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithNullPointerForArgument_device {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- set valid bus params (500kbps : 4Mbps)
        KV_BUSPARAMS_500K_4M(parSet);
        // @- initialize DUT1 in CAN FD operation mode settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test:
        // @- try to set bus params with 'device = NULL'
        retVal = KvaserCAN_SetBusParamsFd(NULL, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        // @- try to get bus params with 'device = NULL'
        retVal = KvaserCAN_GetBusParamsFd(NULL, &parSet);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

        // @post:
        // @- set valid bus params into DUT1
        KV_BUSPARAMS_500K_4M(parSet);
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- compare set and get bus params
        XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
        XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
        XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
        XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
        XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
        // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
        if (parSet.canFd) {
            XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
            XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
            XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
            XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
            XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
        }
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC30: Give a NULL pointer as argument for parameter 'params'
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithNullPointerForArgument_params {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- set valid bus params (500kbps : 4Mbps)
        KV_BUSPARAMS_500K_4M(parSet);
        // @- initialize DUT1 in CAN FD operation mode settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test:
        // @- try to set bus params with 'params = NULL' into DUT1
        retVal = KvaserCAN_SetBusParamsFd(&device, NULL);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        // @- try to get bus params with 'params = NULL' from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, NULL);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

        // @post:
        // @- set valid bus params into DUT1
        KV_BUSPARAMS_500K_4M(parSet);
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- compare set and get bus params
        XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
        XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
        XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
        XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
        XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
        // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
        if (parSet.canFd) {
            XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
            XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
            XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
            XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
            XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
        }
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

// @xctest TC31: Give NULL pointers as argument for both parameters
//
// @expected Not CANUSB_SUCCESS
//
- (void)testWithNullPointerForBothArguments {
    KvaserUSB_OpMode_t mode = (CANMODE_FDOE | CANMODE_BRSE);
 
    // @note: this test requires a CAN FD capable device
    if (KvaserCAN_ProbeChannel(DUT1, mode, NULL) == CANUSB_SUCCESS) {
        KvaserUSB_Device_t device = {};
        KvaserUSB_BusParamsFd_t parSet = {};
        KvaserUSB_BusParamsFd_t parGet = {};
        CANUSB_Return_t retVal = CANUSB_ERROR_FATAL;
        
        // @pre:
        // @- set valid bus params (500kbps : 4Mbps)
        KV_BUSPARAMS_500K_4M(parSet);
        // @- initialize DUT1 in CAN FD operation mode settings
        retVal = KvaserCAN_InitializeChannel(DUT1, mode, &device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        
        // @test:
        // @- try to set bus params with both arguments NULL
        retVal = KvaserCAN_SetBusParamsFd(NULL, NULL);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);
        // @- try to get bus params with both arguments NULL
        retVal = KvaserCAN_GetBusParamsFd(NULL, NULL);
        XCTAssertNotEqual(CANUSB_SUCCESS, retVal);

        // @post:
        // @- set valid bus params into DUT1
        KV_BUSPARAMS_500K_4M(parSet);
        retVal = KvaserCAN_SetBusParamsFd(&device, &parSet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- get bus params from DUT1
        retVal = KvaserCAN_GetBusParamsFd(&device, &parGet);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
        // @- compare set and get bus params
        XCTAssertEqual(parGet.nominal.bitRate, parSet.nominal.bitRate);
        XCTAssertEqual(parGet.nominal.tseg1, parSet.nominal.tseg1);
        XCTAssertEqual(parGet.nominal.tseg2, parSet.nominal.tseg2);
        XCTAssertEqual(parGet.nominal.sjw, parSet.nominal.sjw);
        XCTAssertEqual(parGet.nominal.noSamp, parSet.nominal.noSamp);
        // FIXME: XCTAssertEqual(parGet.canFd, parSet.canFd);
        if (parSet.canFd) {
            XCTAssertEqual(parGet.data.bitRate, parSet.data.bitRate);
            XCTAssertEqual(parGet.data.tseg1, parSet.data.tseg1);
            XCTAssertEqual(parGet.data.tseg2, parSet.data.tseg2);
            XCTAssertEqual(parGet.data.sjw, parSet.data.sjw);
            XCTAssertEqual(parGet.data.noSamp, parSet.data.noSamp);
        }
        // @- shutdown DUT1
        retVal = KvaserCAN_TeardownChannel(&device);
        XCTAssertEqual(CANUSB_SUCCESS, retVal);
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by the device under test.");
    }
}

@end
