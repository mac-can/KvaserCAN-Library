//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2021 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
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
#import "can_api.h"
#import <XCTest/XCTest.h>

@interface test_can_start : XCTestCase

@end

@implementation test_can_start

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    (void)can_exit(CANKILL_ALL);
}

- (void)testWhenInterfaceNotInitialized {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @test:
    // @- try to start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOTINIT, rc);
    
    // @post:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testWhenInterfaceStarted {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @test:
    // @- try to start DUT1 again with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_ONLINE, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testWhenInterfaceStopped {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    
    // @test:
    // @- start DUT1 again with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0)
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testWhenInterfaceShutdown {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    // @- try to start DUT1 again with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOTINIT, rc);
}

- (void)testCheckCiaIndex0 {
    can_bitrate_t bitrate = { CANBTR_INDEX_1M };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(0, bitrate.index);
    // @- start DUT1 with CiA table index 0 (1000kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex1 {
    can_bitrate_t bitrate = { CANBTR_INDEX_800K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-1, bitrate.index);
    // @- start DUT1 with CiA table index 1 (800kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex2 {
    can_bitrate_t bitrate = { CANBTR_INDEX_500K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-2, bitrate.index);
    // @- start DUT1 with CiA table index 1 (500kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex3 {
    can_bitrate_t bitrate = { CANBTR_INDEX_250K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-3, bitrate.index);
    // @- start DUT1 with CiA table index 3 (250kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex4 {
    can_bitrate_t bitrate = { CANBTR_INDEX_125K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-4, bitrate.index);
    // @- start DUT1 with CiA table index 4 (125kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex5 {
    can_bitrate_t bitrate = { CANBTR_INDEX_100K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-5, bitrate.index);
    // @- start DUT1 with CiA table index 5 (100kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex6 {
    can_bitrate_t bitrate = { CANBTR_INDEX_50K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-6, bitrate.index);
    // @- start DUT1 with CiA table index 6 (50kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex7 {
    can_bitrate_t bitrate = { CANBTR_INDEX_20K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-7, bitrate.index);
    // @- start DUT1 with CiA table index 7 (20kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckCiaIndex8 {
    can_bitrate_t bitrate = { CANBTR_INDEX_10K };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    XCTAssertEqual(-8, bitrate.index);
    // @- start DUT1 with CiA table index 8 (10kbps)
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testCheckInvalidCiaIndex {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @pre:
    // @- initialize DUT1 in CAN 2.0 operation mode
    handle = can_init(DUT1, CANMODE_DEFAULT, NULL);
    XCTAssertGreaterThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @test:
    // @note: pre-defined BTR0BTR1 bit-timing table has 10 entries, index 0 to 9.
    //        But the index must be given as negative value to 'bitbate.index'!
    //        Remark: The CiA bit-timing table has only 9 entries!
    bitrate.index = -10/*-BTR_SJA1000_MAX_INDEX*/;
    // @- try to start DUT1 with invalid index value -10
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    // @note: Positive values represent the CAN clock in Hertz, but there will
    //        probably be no clock below 10 Hertz (or above 999'999'999 Hertz).
    bitrate.index = CANBDR_800;
    XCTAssertEqual(1, bitrate.index);
    // @- try to start DUT1 with invalid index value 1
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    bitrate.index = CANBDR_500;
    XCTAssertEqual(2, bitrate.index);
    // @- try to start DUT1 with invalid index value 2
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    bitrate.index = CANBDR_250;
    XCTAssertEqual(3, bitrate.index);
    // @- try to start DUT1 with invalid index value 3
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    bitrate.index = CANBDR_125;
    XCTAssertEqual(4, bitrate.index);
    // @- try to start DUT1 with invalid index value 4
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    bitrate.index = CANBDR_100;
    XCTAssertEqual(5, bitrate.index);
    // @- try to start DUT1 with invalid index value 5
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    bitrate.index = CANBDR_50;
    XCTAssertEqual(6, bitrate.index);
    // @- try to start DUT1 with invalid index value 6
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    bitrate.index = CANBDR_20;
    XCTAssertEqual(7, bitrate.index);
    // @- try to start DUT1 with invalid index value 7
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    bitrate.index = CANBDR_10;
    XCTAssertEqual(8, bitrate.index);
    // @- try to start DUT1 with invalid index value 8
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    // @- try to start DUT1 with invalid index value INT32_MAX
    bitrate.index = INT32_MAX;
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);
    // @- try to start DUT1 with invalid index value INT8_MIN
    bitrate.index = INT8_MIN;
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);
    // @- try to start DUT1 with invalid index value INT16_MIN
    bitrate.index = INT16_MIN;
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);
    // @- try to start DUT1 with invalid index value INT32_MIN+1
    bitrate.index = INT32_MIN+1;
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);
    // @- try to start DUT1 with invalid index value INT32_MIN
    bitrate.index = INT32_MIN;
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_BAUDRATE, rc);

    // @post: shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

- (void)testWithSameCiaIndexAfterCanStopped {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @test: loop over CiA bit-timing table indexes 0 to 8
    for (SInt32 index = CANBTR_INDEX_1M; index >= CANBTR_INDEX_10K; index--) {
        bitrate.index = index;
        // @- initialize DUT1 with configured settings
        handle = can_init(DUT1, TEST_CANMODE, NULL);
        XCTAssertGreaterThanOrEqual(0, handle);
        // @- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);
        // @- start DUT1 with current bit-rate settings
        rc = can_start(handle, &bitrate);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in RUNNING state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertFalse(status.can_stopped);
        // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
        CTester tester;
        XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
        XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
        // @- stop/reset DUT1
        rc = can_reset(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);

        // @- start DUT1 again with current bit-rate settings
        rc = can_start(handle, &bitrate);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in RUNNING state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertFalse(status.can_stopped);
        // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
        XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
        XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
        // @- stop/reset DUT1
        rc = can_reset(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);
        // @- shutdown DUT1
        rc = can_exit(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);
    }
}

- (void)testWithDifferentCiaIndexAfterCanStopped {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    
    // @test: loop over CiA bit-timing table indexes 0 to 8
    for (SInt32 index = CANBTR_INDEX_1M; index >= CANBTR_INDEX_10K; index--) {
        bitrate.index = index;
        // @- initialize DUT1 with configured settings
        handle = can_init(DUT1, TEST_CANMODE, NULL);
        XCTAssertGreaterThanOrEqual(0, handle);
        // @- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);
        // @- start DUT1 with current bit-rate settings
        rc = can_start(handle, &bitrate);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in RUNNING state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertFalse(status.can_stopped);
        // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
        CTester tester;
        XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
        XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
        // @- stop/reset DUT1
        rc = can_reset(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);

        // @- new CiA bit-timing table index = 8 - old
        bitrate.index = CANBTR_INDEX_10K - index;
        // @- start DUT1 again with a different bit-rate settings
        rc = can_start(handle, &bitrate);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in RUNNING state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertFalse(status.can_stopped);
        // @- sunnyday traffic (optional):
#if (OPTION_SEND_TEST_FRAMES != 0) && (OPTION_SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
        XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
        XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
#endif
        // @- stop/reset DUT1
        rc = can_reset(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);
        // @- shutdown DUT1
        rc = can_exit(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);

    }
}

//- (void)testWithValidCan20BitrateSettings {
//        TODO: insert coin here
//}

//- (void)testWithInvalidCan20BitrateSettings {
//        TODO: insert coin here
//}

//- (void)testWithSameCan20BitrateSettingsAfterCanStopped {
//        TODO: insert coin here
//}

//- (void)testWithDifferentCan20BitrateSettingsAfterCanStopped {
//        TODO: insert coin here
//}

//- (void)testWithValidCanFdBitrateSettings {
//        TODO: insert coin here
//}

//- (void)testWithInvalidCanFdBitrateSettings {
//        TODO: insert coin here
//}

//- (void)testWithSameCanFdBitrateSettingsAfterCanStopped {
//        TODO: insert coin here
//}

//- (void)testWithDifferentCanFdBitrateSettingsAfterCanStopped {
//        TODO: insert coin here
//}

//- (void)testWithCan20CiaIndexInCanFdMode {
//        TODO: insert coin here
//}

//- (void)testWithCan20BitrateSettingsInCanFdMode {
//        TODO: insert coin here
//}

//- (void)testWithCanFdBitrateSettingsInCan20Mode {
//        TODO: insert coin here
//}

@end
