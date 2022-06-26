//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2022 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
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

@interface test_can_bitrate : XCTestCase

@end

@implementation test_can_bitrate

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    (void)can_exit(CANKILL_ALL);
}

// @xctest TC11.1: Get CAN bit-rate settings with invalid interface handle(s)
//
// @expected CANERR_HANDLE
//
- (void)testWithInvalidHandle {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);

    // @pre:
    // @- try to get bit-rate of DUT1 with invalid handle -1
    rc = can_bitrate(INVALID_HANDLE, &bitrate, NULL);
    XCTAssertEqual(CANERR_HANDLE, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- try to get bit-rate of DUT1 with invalid handle INT32_MIN
    rc = can_bitrate(INT32_MAX, &bitrate, NULL);
    XCTAssertEqual(CANERR_HANDLE, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- try to get bit-rate of DUT1 with invalid handle INT32_MIN
    rc = can_bitrate(INT32_MIN, &bitrate, NULL);
    XCTAssertEqual(CANERR_HANDLE, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @post:
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC11.2: Give a NULL pointer as argument for parameter 'bitrate'
//
// @expected CANERR_NOERROR
//
- (void)testWithNullPointerForBitrate {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_speed_t speed = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    // @- get bit-rate of DUT1 with NULL for parameter 'bitrate'
    rc = can_bitrate(handle, NULL, &speed);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @post:
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
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

// @xctest TC11.3: Give a NULL pointer as argument for parameter 'speed'
//
// @expected CANERR_NOERROR
//
- (void)testWithNullPointerForSpeed {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    // @- get bit-rate of DUT1 with NULL for speed
    rc = can_bitrate(handle, &bitrate, NULL);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @post:
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
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

// @xctest TC11.4: Give a NULL pointer as argument for parameter 'bitrate' and 'speed'
//
// @expected CANERR_NOERROR
//
- (void)testWithNullPointerForBoth {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    // @- get bit-rate of DUT1 with NULL for both
    rc = can_bitrate(handle, NULL, NULL);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @post:
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
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

// @xctest TC11.5: Get CAN bit-rate settings when interface is not initialized
//
// @expected CANERR_NOTINIT
//
- (void)testWhenInterfaceNotInitialized {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- try to get bit-rate of DUT1 with invalid handle -1
    rc = can_bitrate(INVALID_HANDLE, &bitrate, NULL);
    XCTAssertEqual(CANERR_NOTINIT, rc);
    // @- try to get bit-rate of DUT1 with invalid handle INT32_MIN
    rc = can_bitrate(INT32_MIN, &bitrate, NULL);
    XCTAssertEqual(CANERR_NOTINIT, rc);
    // @- try to get bit-rate of DUT1 with invalid handle INT32_MAX
    rc = can_bitrate(INT32_MAX, &bitrate, NULL);
    XCTAssertEqual(CANERR_NOTINIT, rc);

    // @post:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
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
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
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

// @xctest TC11.6: Get CAN bit-rate settings when interface initialized (but CAN controller not started)
//
// @expected CANERR_OFFLINE
//
- (void)testWhenInterfaceInitialized {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);

    // @test:
    // @- get bit-rate of DUT1
    rc = can_bitrate(handle, &bitrate, NULL);
    XCTAssertEqual(CANERR_OFFLINE, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @post:
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
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

// @xctest TC11.7: Get CAN bit-rate settings when CAN controller started
//
// @expected CANERR_NOERROR
//
- (void)testWhenInterfaceStarted {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
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
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
#endif
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @test:
    // @- get bit-rate of DUT1
    rc = can_bitrate(handle, &bitrate, NULL);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @post:
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC11.8: Get CAN bit-rate settings when CAN controller stopped
//
// @expected CANERR_OFFLINE
//
- (void)testWhenInterfaceStopped {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
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
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
#endif
    // @- stop/reset DUT1
    rc = can_reset(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    // @- get bit-rate of DUT1
    rc = can_bitrate(handle, &bitrate, NULL);
    XCTAssertEqual(CANERR_OFFLINE, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @post:
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC11.9: Get CAN bit-rate settings when interface already shutdown
//
// @expected CANERR_NOTINIT
//
- (void)testWhenInterfaceShutdown {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
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
    // @- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
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
    // @- try to get bit-rate of DUT1
    rc = can_bitrate(handle, &bitrate, NULL);
    XCTAssertEqual(CANERR_NOTINIT, rc);
}

// @xctest TC11.10: Get CAN bit-rate settings when CAN controller started with various CAN 2.0 bit-rate settings and check for correctness
//
// @expected CANERR_NOERROR
//
- (void)testWithVariousCan20BitrateSettings {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test: 
    // @- loop over selected CAN 2.0 bit-rate settings
    for (int i = 0; i < 4; i++) {
        switch (i) {
            case 0: BITRATE_1M(bitrate); break;
            case 1: BITRATE_500K(bitrate); break;
            case 2: BITRATE_250K(bitrate); break;
            case 3: BITRATE_125K(bitrate); break;
            default: return;
        }
        can_bitrate_t result = {};
        // @-- initialize DUT1 with configured settings
        handle = can_init(DUT1, TEST_CANMODE, NULL);
        XCTAssertLessThanOrEqual(0, handle);
        // @-- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);
        // @-- start DUT1 with selected bit-rate settings
        rc = can_start(handle, &bitrate);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @-- get status of DUT1 and check to be in RUNNING state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertFalse(status.can_stopped);
        // @-- get bitrate from DUT1 and compare with selected settings
        rc = can_bitrate(handle, &result, NULL);
        XCTAssertEqual(CANERR_NOERROR, rc);
#if (TC11_10_KVASER_BUSPARAMS_WORKAROUND == 0) && (COMPARE_BITRATE_BY_TIME_QUANTA == 0)
        // @issue: if CAN clock unknown on user level, then it is adapted by the wrapper.
        XCTAssertEqual(bitrate.btr.frequency, result.btr.frequency);
        XCTAssertEqual(bitrate.btr.nominal.brp, result.btr.nominal.brp);
#else
        // @workaround: compare bit-rate settings by time quanta (tq = f_clock / brp)
        if (bitrate.btr.nominal.brp && result.btr.nominal.brp)
            XCTAssertEqual((bitrate.btr.frequency / bitrate.btr.nominal.brp), (result.btr.frequency / result.btr.nominal.brp));
        else
            XCTAssertTrue(false, @"Devide by Zero!");
#endif
        XCTAssertEqual(bitrate.btr.nominal.tseg1, result.btr.nominal.tseg1);
        XCTAssertEqual(bitrate.btr.nominal.tseg2, result.btr.nominal.tseg2);
        XCTAssertEqual(bitrate.btr.nominal.sjw, result.btr.nominal.sjw);
#if (TC11_10_KVASER_NOSAMP == 0)
        // @issue: only SAM = 0 supported by Kvaser devices (noSamp = 1)
        XCTAssertEqual(bitrate.btr.nominal.sam, result.btr.nominal.sam);
#else
        // @workaround: do not compare it
#endif
        // @-- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0) && (SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
        CTester tester;
        XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
        XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
        // @-- get status of DUT1 and check to be in RUNNING state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertFalse(status.can_stopped);
#endif
        // @-- stop/reset DUT1
        rc = can_reset(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);
        // @-- get status of DUT1 and check to be in INIT state
        rc = can_status(handle, &status.byte);
        XCTAssertEqual(CANERR_NOERROR, rc);
        XCTAssertTrue(status.can_stopped);
        // @-- shutdown DUT1
        rc = can_exit(handle);
        XCTAssertEqual(CANERR_NOERROR, rc);
    }
}

// @xctest TC11.11: Get CAN bit-rate settings when CAN controller started with various CAN FD bit-rate settings and check for correctness
//
// @expected CANERR_NOERROR
//
- (void)testWithVariousCanFdBitrateSettings {
    uint8_t mode = (CANMODE_FDOE | CANMODE_BRSE);
    
    if ((can_test(DUT1, mode, NULL, NULL) == CANERR_NOERROR) &&
        (can_test(DUT2, mode, NULL, NULL) == CANERR_NOERROR)) {
        can_bitrate_t bitrate = { TEST_BTRINDEX };
        can_status_t status = { CANSTAT_RESET };
        int handle = INVALID_HANDLE;
        int rc = CANERR_FATAL;

        // @test:
        // @- loop over selected CAN FD bit-rate settings
        for (int i = 0; i < 8; i++) {
            switch (i) {
                case 0: BITRATE_FD_1M(bitrate); mode = CANMODE_FDOE; break;
                case 1: BITRATE_FD_500K(bitrate); mode = CANMODE_FDOE; break;
                case 2: BITRATE_FD_250K(bitrate); mode = CANMODE_FDOE; break;
                case 3: BITRATE_FD_125K(bitrate); mode = CANMODE_FDOE; break;
                case 4: BITRATE_FD_1M8M(bitrate); mode = (CANMODE_FDOE | CANMODE_BRSE); break;
                case 5: BITRATE_FD_500K4M(bitrate); mode = (CANMODE_FDOE | CANMODE_BRSE); break;
                case 6: BITRATE_FD_250K2M(bitrate); mode = (CANMODE_FDOE | CANMODE_BRSE); break;
                case 7: BITRATE_FD_125K1M(bitrate); mode = (CANMODE_FDOE | CANMODE_BRSE); break;
               default: return;
            }
            can_bitrate_t result = {};
            // @-- initialize DUT1 with configured settings
            handle = can_init(DUT1, mode, NULL);
            XCTAssertLessThanOrEqual(0, handle);
            // @-- get status of DUT1 and check to be in INIT state
            rc = can_status(handle, &status.byte);
            XCTAssertEqual(CANERR_NOERROR, rc);
            XCTAssertTrue(status.can_stopped);
            // @-- start DUT1 with selected bit-rate settings
            rc = can_start(handle, &bitrate);
            XCTAssertEqual(CANERR_NOERROR, rc);
            // @-- get status of DUT1 and check to be in RUNNING state
            rc = can_status(handle, &status.byte);
            XCTAssertEqual(CANERR_NOERROR, rc);
            XCTAssertFalse(status.can_stopped);
            // @-- get bitrate from DUT1 and compare with selected settings
            rc = can_bitrate(handle, &result, NULL);
            XCTAssertEqual(CANERR_NOERROR, rc);
#if (TC11_11_KVASER_BUSPARAMS_WORKAROUND == 0) && (COMPARE_BITRATE_BY_TIME_QUANTA == 0)
            // @issue: if CAN clock unknown on user level, then it is adapted by the wrapper.
            XCTAssertEqual(bitrate.btr.frequency, result.btr.frequency);
            XCTAssertEqual(bitrate.btr.nominal.brp, result.btr.nominal.brp);
#else
            // @workaround: compare nominal bit-rate settings by time quanta (tq = f_clock / brp)
            if (bitrate.btr.nominal.brp && result.btr.nominal.brp)
                XCTAssertEqual((bitrate.btr.frequency / bitrate.btr.nominal.brp), (result.btr.frequency / result.btr.nominal.brp));
            else
                XCTAssertTrue(false, @"Devide by Zero!");
#endif
            XCTAssertEqual(bitrate.btr.nominal.tseg1, result.btr.nominal.tseg1);
            XCTAssertEqual(bitrate.btr.nominal.tseg2, result.btr.nominal.tseg2);
            XCTAssertEqual(bitrate.btr.nominal.sjw, result.btr.nominal.sjw);
            // @--- compare data phase settings in bit-rate switching enabled
            if (mode & CANMODE_BRSE) {
#if (TC11_11_KVASER_BUSPARAMS_WORKAROUND == 0) && (COMPARE_BITRATE_BY_TIME_QUANTA == 0)
                // @issue: if CAN clock unknown on user level, then it is adapted by the wrapper.
                XCTAssertEqual(bitrate.btr.frequency, result.btr.frequency);
                XCTAssertEqual(bitrate.btr.data.brp, result.btr.data.brp);
#else
                // @workaround: compare data phase bit-rate settings by time quanta (tq = f_clock / brp)
                if (bitrate.btr.data.brp && result.btr.data.brp)
                    XCTAssertEqual((bitrate.btr.frequency / bitrate.btr.data.brp), (result.btr.frequency / result.btr.data.brp));
                else
                    XCTAssertTrue(false, @"Devide by Zero!");
#endif
                XCTAssertEqual(bitrate.btr.data.tseg1, result.btr.data.tseg1);
                XCTAssertEqual(bitrate.btr.data.tseg2, result.btr.data.tseg2);
                XCTAssertEqual(bitrate.btr.data.sjw, result.btr.data.sjw);
            } else {
                XCTAssertEqual(result.btr.nominal.brp, result.btr.data.brp);
                XCTAssertEqual(result.btr.nominal.tseg1, result.btr.data.tseg1);
                XCTAssertEqual(result.btr.nominal.tseg2, result.btr.data.tseg2);
                XCTAssertEqual(result.btr.nominal.sjw, result.btr.data.sjw);
           }
            // @-- send and receive some frames to/from DUT2 (optional)
#if (SEND_TEST_FRAMES != 0) && (SEND_WITH_NONE_DEFAULT_BAUDRATE != 0)
            CTester tester;
            XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
            XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
            // @-- get status of DUT1 and check to be in RUNNING state
            rc = can_status(handle, &status.byte);
            XCTAssertEqual(CANERR_NOERROR, rc);
            XCTAssertFalse(status.can_stopped);
#endif
            // @-- stop/reset DUT1
            rc = can_reset(handle);
            XCTAssertEqual(CANERR_NOERROR, rc);
            // @-- get status of DUT1 and check to be in INIT state
            rc = can_status(handle, &status.byte);
            XCTAssertEqual(CANERR_NOERROR, rc);
            XCTAssertTrue(status.can_stopped);
            // @-- shutdown DUT1
            rc = can_exit(handle);
            XCTAssertEqual(CANERR_NOERROR, rc);
        }
    } else {
        NSLog(@"Test case skipped: CAN FD operation mode not supported by at least one device.");
    }
}

@end

// $Id: test_can_bitrate.mm 1060 2022-06-24 16:26:58Z eris $  Copyright (c) UV Software, Berlin //
