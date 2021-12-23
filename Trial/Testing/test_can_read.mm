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

@interface test_can_read : XCTestCase

@end

@implementation test_can_read

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    (void)can_exit(CANKILL_ALL);
}

// @xctest TC04.1: Read a CAN message with invalid interface handle(s).
//
// @expected: CANERR_HANDLE
//
- (void)testWithInvalidHandle {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_message_t message = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- try to read a message from DUT1 with invalid handle -1
    rc = can_read(INVALID_HANDLE, &message, 0U);
    XCTAssertEqual(CANERR_HANDLE, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- try to read a message from DUT1 with invalid handle INT32_MIN
    rc = can_read(INT32_MIN, &message, 0U);
    XCTAssertEqual(CANERR_HANDLE, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- sunnyday traffic (optional):
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
    // @- try to read a message from DUT1 with invalid handle INT32_MIN
    rc = can_read(INT32_MIN, &message, 0U);
    XCTAssertEqual(CANERR_HANDLE, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC04.2: Give a NULL pointer as argument for parameter 'message'.
//
// @expected: CANERR_NULLPTR
//
- (void)testWithNullPointerForMessage {
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

    // @test:
    // @- read a message from DUT1 with NULL for parameter 'message'
    rc = can_read(handle, NULL, 0U);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    
    // @post:
    // @- sunnyday traffic (optional):
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

// @xctest TC04.3: Read a CAN message when interface is not initialized.
//
// @expected: CANERR_NOTINIT
//
- (void)testWhenInterfaceNotInitialized {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_message_t message = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- try to read a message from DUT1 with invalid handle -1
    rc = can_read(INVALID_HANDLE, &message, 0U);
    XCTAssertEqual(CANERR_NOTINIT, rc);
    // @- try to read a message from DUT1 with invalid handle INT32_MIN
    rc = can_read(INT32_MIN, &message, 0U);
    XCTAssertEqual(CANERR_NOTINIT, rc);
    // @- try to read a message from DUT1 with invalid handle INT32_MIN
    rc = can_read(INT32_MIN, &message, 0U);
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
    // @- sunnyday traffic (optional):
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

// @xctest TC04.4: Read a CAN message when CAN controller is not started.
//
// @expected: CANERR_OFFLINE
//
- (void)testWhenInterfaceNotStarted {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_message_t message = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);

    // @test:
    // @- try to read a message from DUT1
    rc = can_read(handle, &message, 0U);
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
    // @- sunnyday traffic (optional):
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

// @xctest TC04.5: Read a CAN message when CAN controller already stopped.
//
// @expected: CANERR_OFFLINE
//
- (void)testWhenInterfaceStopped {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_message_t message = {};
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
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- sunnyday traffic (optional):
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
    // @- try to read a message from DUT1
    rc = can_read(handle, &message, 0U);
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

// @xctest TC04.6: Read a CAN message when interface already shutdown.
//
// @expected: CANERR_NOTINIT
//
- (void)testWhenInterfaceShutdown {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_message_t message = {};
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
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- sunnyday traffic (optional):
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
    // @- try to read a message from DUT1
    rc = can_read(handle, &message, 0U);
    XCTAssertEqual(CANERR_NOTINIT, rc);
}

// @xctest TC04.7: Read a CAN message when reception queue is empty.
//
// @expected: CANERR_RX_EMPTY
//
- (void)testWhenReceiveQueueIsEmpty {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_message_t message = {};
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
    // @- try to read a message from DUT1 when there in none
    rc = can_read(handle, &message, 0U);
    XCTAssertEqual(CANERR_RX_EMPTY, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- check if bit CANSTAT_RX_EMPTY is set in status register
    XCTAssertTrue(status.receiver_empty);
    // @- sunnyday traffic (optional):
#if (SEND_TEST_FRAMES != 0)
    CTester tester;
    XCTAssertEqual(TEST_FRAMES, tester.SendSomeFrames(handle, DUT2, TEST_FRAMES));
    XCTAssertEqual(TEST_FRAMES, tester.ReceiveSomeFrames(handle, DUT2, TEST_FRAMES));
    // @- try to read a message from DUT1 when there in none
    rc = can_read(handle, &message, 0U);
    XCTAssertEqual(CANERR_RX_EMPTY, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- check if bit CANSTAT_RX_EMPTY is set in status register
    XCTAssertTrue(status.receiver_empty);
#endif
    // @post:
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

// @xctest TC04.8: Read a CAN message from reception queue after overrun.
//
// @expected: CANERR_NOERROR, but status bit 'queue_overrun' = 1
//
- (void)testWhenReceiveQueueIsFull {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    can_message_t message1 = {};
    can_message_t message2 = {};
    can_mode_t mode = { TEST_CANMODE };
    int handle1 = INVALID_HANDLE;
    int handle2 = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    message2.id = 0x400U;
    message2.fdf = mode.fdoe ? 1 : 0;
    message2.brs = mode.brse ? 1 : 0;
    message2.xtd = 0;
    message2.rtr = 0;
    message2.esi = 0;
    message2.sts = 0;
    message2.dlc = mode.fdoe ? CANFD_MAX_DLC : CAN_MAX_DLC;
    memset(message2.data, 0, CANFD_MAX_LEN);
    
    // @pre:
    mode.nxtd = 0;
    // @- initialize DUT1 with configured settings
    handle1 = can_init(DUT1, mode.byte, NULL);
    XCTAssertLessThanOrEqual(0, handle1);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle1, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- initialize DUT2 with configured settings
    handle2 = can_init(DUT2, mode.byte, NULL);
    XCTAssertLessThanOrEqual(0, handle1);
    // @- get status of DUT2 and check to be in INIT state
    rc = can_status(handle2, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle1, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle1, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- start DUT2 with configured bit-rate settings
    rc = can_start(handle2, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT2 and check to be in RUNNING state
    rc = can_status(handle2, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);

    // @test:
    NSLog(@"Be patient...");
    // @- spam the receive queue of DUT1 with one message too much
    for (i = 0; i <= TEST_QUEUE_FULL; i++) {
        message2.data[0] = (uint8_t)((uint64_t)i >> 0);
        message2.data[1] = (uint8_t)((uint64_t)i >> 8);
        message2.data[2] = (uint8_t)((uint64_t)i >> 16);
        message2.data[3] = (uint8_t)((uint64_t)i >> 24);
        message2.data[4] = (uint8_t)((uint64_t)i >> 32);
        message2.data[5] = (uint8_t)((uint64_t)i >> 40);
        message2.data[6] = (uint8_t)((uint64_t)i >> 48);
        message2.data[7] = (uint8_t)((uint64_t)i >> 56);
        do {
            rc = can_write(handle2, &message2, 1000U);
        } while (CANERR_TX_BUSY == rc);
        XCTAssertEqual(CANERR_NOERROR, rc);
        if (CANERR_NOERROR != rc)
            break;
    }
#if (TX_ACKNOWLEDGE_UNSUPPORTED != 0)
    // @note: a delay (after burner) to guarantee that all CAN messages are really sent
    //        is required when messages are not acknowlegded by the CAN controller.
    CTimer::Delay(TEST_AFTER_BURNER*CTimer::MSEC);
    // @note: the delay depends on the bit-rate (set TEST_AFTER_BURNER in "Settings.h").
#endif
    NSLog(@"%d frame(s) sent", i);
    // @- read a message from DUT1 (there should be at least one)
    rc = can_read(handle1, &message1, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle1, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- check if bit CANSTAT_QUE_OVR is set in status register
    XCTAssertTrue(status.queue_overrun);
    // @- stop/reset DUT1 (this should clear the receive queue)
    rc = can_reset(handle1);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- start DUT1 with configured bit-rate settings again
    rc = can_start(handle1, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle1, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- try to read a message from DUT1 (now there should none)
    rc = can_read(handle1, &message1, 0U);
    XCTAssertEqual(CANERR_RX_EMPTY, rc);
    // @- get status of DUT1 and check to be in RUNNING state
    rc = can_status(handle1, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertFalse(status.can_stopped);
    // @- check if bit CANSTAT_QUE_OVR is cleared in status register
    XCTAssertFalse(status.queue_overrun);

    // @post:
    // @- stop/reset DUT1
    rc = can_reset(handle1);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle1, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle1);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT2
    rc = can_exit(handle2);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC04.9: tbd.
//
//- (void)testWhenMessageLost {
//        TODO: insert coin here
//        FIXME: How to loose a message?
//
@end

// $Id: test_can_read.mm 1037 2021-12-21 19:27:26Z makemake $  Copyright (c) UV Software, Berlin //
