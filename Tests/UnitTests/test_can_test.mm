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

@interface test_can_test : XCTestCase

@end

@implementation test_can_test

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    (void)can_exit(CANKILL_ALL);
}

// @xctest TC01.1: Probe interface when not initialized
//
// @expected CANERR_NOERROR and interface state CANBRD_PRESENT
//
- (void)testWhenInterfaceNotOccupied {
    int state = CANBRD_NOT_TESTABLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- probe DUT1 with configured settings
    rc = can_test(DUT1, TEST_CANMODE, NULL, &state);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertEqual(CANBRD_PRESENT, state);
}

// @xctest TC01.2: Probe interface when already initialized (by own process)
//
// @expected CANERR_NOERROR and interface state CANBRD_OCCUPIED
//
- (void)testWhenInterfaceOccupiedByOwnProcess {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    int state = CANBRD_NOT_TESTABLE;
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);

    // @test:
    // @- probe DUT1 with configured settings
    rc = can_test(DUT1, TEST_CANMODE, NULL, &state);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertEqual(CANBRD_OCCUPIED, state);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- probe DUT1 with configured settings
    rc = can_test(DUT1, TEST_CANMODE, NULL, &state);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertEqual(CANBRD_OCCUPIED, state);
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
    // @- probe DUT1 with configured settings
    rc = can_test(DUT1, TEST_CANMODE, NULL, &state);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertEqual(CANBRD_OCCUPIED, state);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @post:
    // @- probe DUT1 with configured settings
    rc = can_test(DUT1, TEST_CANMODE, NULL, &state);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertEqual(CANBRD_PRESENT, state);
}

// @xctest TC01.3: Probe interface when used by another process
//
// @expected CANERR_NOERROR and interface state CANBRD_OCCUPIED
//
- (void)testWhenInterfaceOccupiedByAnotherProcess {
    // @note: this scenario is not testable:
    //        1) up to now I didn´t found an I/O service to detect this
    //        2) the other process must be started manually (or forked)
    XCTAssertTrue(true);
}

// @xctest TC01.4: Probe interface with valid channel number(s).
//
// @expected CANERR_NOERROR
//
- (void)testWithValidChannelNo {
    SInt32 channel = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- loop over the list of devices to get the channel no.
    rc = can_property(INVALID_HANDLE, CANPROP_SET_FIRST_CHANNEL, (void*)NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    while (CANERR_NOERROR == rc) {
        rc = can_property(INVALID_HANDLE, CANPROP_GET_CHANNEL_TYPE, (void*)&channel, sizeof(SInt32));
        if (CANERR_NOERROR == rc) {
            // @-- probe found channel with configured settings
            rc = can_test(channel, TEST_CANMODE, NULL, NULL);
            XCTAssertEqual(CANERR_NOERROR, rc);
            // @note: state can take all possible values.
        }
        rc = can_property(INVALID_HANDLE, CANPROP_SET_NEXT_CHANNEL, (void*)NULL, 0U);
    }
}

// @xctest TC01.5: Probe interface with invalid channel number(s)
//
// @expected CANERR_NOTINIT or CANERR_VENDOR
//
- (void)testWithInvalidChannelNo {
    int state = CANBRD_NOT_TESTABLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- probe with invalid channel no. -1
    rc = can_test((SInt32)(-1), TEST_CANMODE, NULL, &state);
    XCTAssertNotEqual(CANERR_NOERROR, rc);
    XCTAssert((CANERR_NOTINIT == rc) || (CANERR_VENDOR >= rc));
    XCTAssertEqual(CANBRD_NOT_TESTABLE, state);
    // @- probe with invalid channel no. INT8_MIN
    rc = can_test((SInt32)INT8_MIN, TEST_CANMODE, NULL, &state);
    XCTAssertNotEqual(CANERR_NOERROR, rc);
    XCTAssert((CANERR_NOTINIT == rc) || (CANERR_VENDOR >= rc));
    XCTAssertEqual(CANBRD_NOT_TESTABLE, state);
    // @- probe with invalid channel no. INT16_MIN
    rc = can_test((SInt32)INT16_MIN, TEST_CANMODE, NULL, &state);
    XCTAssertNotEqual(CANERR_NOERROR, rc);
    XCTAssert((CANERR_NOTINIT == rc) || (CANERR_VENDOR >= rc));
    XCTAssertEqual(CANBRD_NOT_TESTABLE, state);
    // @- probe with invalid channel no. INT32_MIN
    rc = can_test((SInt32)INT32_MIN, TEST_CANMODE, NULL, &state);
    XCTAssertNotEqual(CANERR_NOERROR, rc);
    XCTAssert((CANERR_NOTINIT == rc) || (CANERR_VENDOR >= rc));
    XCTAssertEqual(CANBRD_NOT_TESTABLE, state);

    // @note: channel numbers are defined by the CAN device vendor.
    //        Therefore, no assumptions can be made for positive values!
}

// @xctest TC01.6: Probe interface with its full operation mode capability
//
// @expected CANERR_NOERROR
//
- (void)testOperationModeCapability {
    can_mode_t capa = { CANMODE_DEFAULT };
    can_mode_t mode = { CANMODE_DEFAULT };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get operation capability from DUT1
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, (void*)&capa.byte, sizeof(UInt8));
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    mode.byte = capa.byte;
    // @- probe DUT1 with all bits from operation capacity
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC01.7: Probe interface with operation mode bit MON set (listen-only mode)
//
// @expected CANERR_NOERROR or CANERR_ILLPARA if listen-only mode is not supported
//
- (void)testMonitorModeEnableDisable {
    can_mode_t capa = { CANMODE_DEFAULT };
    can_mode_t mode = { CANMODE_DEFAULT };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get operation capability from DUT1
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, (void*)&capa.byte, sizeof(UInt8));
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    mode.mon = 1;
    // @- probe DUT1 with operation mode bit MON set
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    if (capa.mon) {
        XCTAssertEqual(CANERR_NOERROR, rc);
    } else {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    }

}

// @xctest TC01.8: Probe interface with operation mode bit ERR set (error frame reception)
//
// @expected CANERR_NOERROR or CANERR_ILLPARA if error frame reception is not supported
//
- (void)testErrorFramesEnableDisable {
    can_mode_t capa = { CANMODE_DEFAULT };
    can_mode_t mode = { CANMODE_DEFAULT };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get operation capability from DUT1
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, (void*)&capa.byte, sizeof(UInt8));
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    mode.err = 1;
    // @- probe DUT1 with operation mode bit ERR set
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    if (capa.err) {
        XCTAssertEqual(CANERR_NOERROR, rc);
    } else {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    }
}

// @xctest TC01.9: Probe interface with operation mode bit NRTR set (suppress remote frames)
//
// @expected CANERR_NOERROR or CANERR_ILLPARA if suppressing of remote frames is not supported
//
- (void)testRemoteFramesDisableEnable {
    can_mode_t capa = { CANMODE_DEFAULT };
    can_mode_t mode = { CANMODE_DEFAULT };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get operation capability from DUT1
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, (void*)&capa.byte, sizeof(UInt8));
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    mode.nrtr = 1;
    // @- probe DUT1 with operation mode bit NRTR set
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    if (capa.nrtr) {
        XCTAssertEqual(CANERR_NOERROR, rc);
    } else {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    }
}

// @xctest TC01.10: Probe interface with operation mode bit NXTD set (suppress extended frames)
//
// @expected CANERR_NOERROR or CANERR_ILLPARA if suppressing of extended frames is not supported
//
- (void)testExtendedFramesDisableEnable {
    can_mode_t capa = { CANMODE_DEFAULT };
    can_mode_t mode = { CANMODE_DEFAULT };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get operation capability from DUT1
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, (void*)&capa.byte, sizeof(UInt8));
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    mode.nxtd = 1;
    // @- probe DUT1 with operation mode bit NXTD set
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    if (capa.nxtd) {
        XCTAssertEqual(CANERR_NOERROR, rc);
    } else {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    }
}

// @xctest TC01.11: Probe interface with operation mode bit FDOE set (CAN FD operation enabled)
//
// @expected CANERR_NOERROR or CANERR_ILLPARA if CAN FD operation mode is not supported
//
- (void)testCanFdOperationEnableDisable {
    can_mode_t capa = { CANMODE_DEFAULT };
    can_mode_t mode = { CANMODE_DEFAULT };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get operation capability from DUT1
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, (void*)&capa.byte, sizeof(UInt8));
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    mode.fdoe = 1;
    mode.brse = 0;
    // @- probe DUT1 with operation mode bit FDOE set
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    if (capa.fdoe) {
        XCTAssertEqual(CANERR_NOERROR, rc);
    } else {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    }
}

// @xctest TC01.12: Probe interface with operation mode bit FDOE and BRSE set (CAN FD operation with bit-rate switching enabled)
//
// @expected CANERR_NOERROR or CANERR_ILLPARA if CAN FD operation mode or bit-rate switching is not supported
//
- (void)testBitrateSwitchingEnableDisable {
    can_mode_t capa = { CANMODE_DEFAULT };
    can_mode_t mode = { CANMODE_DEFAULT };
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get operation capability from DUT1
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, (void*)&capa.byte, sizeof(UInt8));
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);

    // @test:
    mode.fdoe = 1;
    mode.brse = 1;
    // @- probe DUT1 with operation mode bit FDOE and BRSE set
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    if (capa.fdoe && capa.brse) {
        XCTAssertEqual(CANERR_NOERROR, rc);
    } else {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    }

    // @test:
    mode.fdoe = 0;
    mode.brse = 1;
    // @- probe DUT1 with operation mode bit BRSE set and not FDOE
    rc = can_test(DUT1, mode.byte, NULL, NULL);
    if (capa.fdoe && capa.brse) {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    } else {
        XCTAssertEqual(CANERR_ILLPARA, rc);
    }
}

@end

// $Id: test_can_test.mm 1062 2022-07-03 16:53:27Z makemake $  Copyright (c) UV Software, Berlin //
