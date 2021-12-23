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

@interface test_can_property : XCTestCase

@end

@implementation test_can_property

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    (void)can_exit(CANKILL_ALL);
}

// @xctest TC12.1.1: Get property values from library with invalid interface handle(s).
//
// @expected: CANERR_HANDLE
//
//- (void)testWithInvalidHandleFromLibrary {
//        TODO: insert coin here
//}

// @xctest TC12.1: Get property values from device with invalid interface handle(s).
//
// @todo: rework this
//
// @expected: CANERR_HANDLE
//
- (void)testWithInvalidHandleFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    // @- get property VERSION from DUT1 with invalid handle -1 (should succeed)
    rc = can_property(INVALID_HANDLE, CANPROP_GET_VERSION, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get property LIBRARY_ID from DUT1 with invalid handle INT32_MIN (should succeed)
    rc = can_property(INT32_MIN, CANPROP_GET_LIBRARY_ID, value, CANPROP_MAX_BUFFER_SIZE);
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
    // @- try to get property DEVICE_TYPE from DUT1 with invalid handle INT32_MAX
    rc = can_property(INT32_MAX, CANPROP_GET_DEVICE_TYPE, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_HANDLE, rc);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC12.2: Give a NULL pointer as argument for parameter 'value'
//
// @expected: CANERR_NULLPTR and CANERR_NOERROR for parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL
//
- (void)testWithNullPointerForValue {
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
    // @- get library properties from DUT1
    rc = can_property(handle, CANPROP_GET_SPEC, NULL, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_VERSION, NULL, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_PATCH_NO, NULL, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_BUILD_NO, NULL, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_ID, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_VENDOR, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_DLLNAME, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_SET_FIRST_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_TYPE, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_NAME, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_DLLNAME, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_ID, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_NAME, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_SET_NEXT_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get device properties from DUT1
    rc = can_property(handle, CANPROP_GET_DEVICE_TYPE, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_NAME, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_VENDOR, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_DLLNAME, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_PARAM, NULL, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NULLPTR, rc);  // @note: device params not used by the driver!
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, NULL, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_OP_MODE, NULL, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_BITRATE, NULL, sizeof(can_bitrate_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_SPEED, NULL, sizeof(can_speed_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_STATUS, NULL, sizeof(can_status_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_BUSLOAD, NULL, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_TX_COUNTER, NULL, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_RX_COUNTER, NULL, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_ERR_COUNTER, NULL, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_MAX, NULL, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_HIGH, NULL, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_OVFL, NULL, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_CODE, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_MASK, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_CODE, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_MASK, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_CODE, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_MASK, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_CODE, NULL, sizeof(int32_t));
    XCTAssertEqual(CANERR_NULLPTR, rc);
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_MASK, NULL, sizeof(int32_t));
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

// @xctest TC12.3.1: Get property values from libray with invalid value for parameter 'param'.
//
// @expected: CANERR_NOTSUPP
//
//- (void)testWithInvalidParmeterValueFromLibrary {
//        TODO: insert coin here
//}

// @xctest TC12.3.2: Get property values from device with invalid value for parameter 'param'.
//
// @todo: rework this
//
// @expected: CANERR_NOTSUPP
//
- (void)testWithInvalidParmeterValueFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    
    // @test:
    // @- try to get property from DUT1 with invalid parameter value 768
    rc = can_property(handle, CANPROP_SET_VENDOR_PROP+CANPROP_VENDOR_PROP_RANGE, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);
    // @- try to get property from DUT1 with invalid parameter value UINT16_MAX
    rc = can_property(handle, UINT16_MAX, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);
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

// @xctest TC12.4.1: Get property values from library with wrong parameter size (too small).
//
// @expected: CANERR_ILLPARA and CANERR_NOERROR for parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL
//
//- (void)testWithWrongParameterSizeFromLibrary {
//        TODO: insert coin here
//}

// @xctest TC12.4.2: Get property values from device with wrong parameter size (too small).
//
// @todo: rework this
//
// @expected: CANERR_ILLPARA and CANERR_NOERROR for parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL
//
- (void)testWithWrongParameterSizeFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    
    // @test
    // @- get library properties from DUT1
    rc = can_property(handle, CANPROP_GET_SPEC, value, sizeof(uint16_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_VERSION, value, sizeof(uint16_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_PATCH_NO, value, sizeof(uint8_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_BUILD_NO, value, sizeof(uint32_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_ID, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_VENDOR, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_DLLNAME, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_SET_FIRST_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_TYPE, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_NAME, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_DLLNAME, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_ID, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_NAME, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_SET_NEXT_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get device properties from DUT1
    rc = can_property(handle, CANPROP_GET_DEVICE_TYPE, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_NAME, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_VENDOR, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_DLLNAME, value, 0U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_PARAM, value, 0U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // @note: device params not used by the driver!
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, value, sizeof(can_mode_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_OP_MODE, value, sizeof(can_mode_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_BITRATE, value, sizeof(can_bitrate_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_SPEED, value, sizeof(can_speed_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_STATUS, value, sizeof(can_status_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_BUSLOAD, value, sizeof(uint8_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_TX_COUNTER, value, sizeof(uint64_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_RX_COUNTER, value, sizeof(uint64_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_ERR_COUNTER, value, sizeof(uint64_t)-1U);
    XCTAssertEqual(CANERR_ILLPARA, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_MAX, value, sizeof(uint32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_HIGH, value, sizeof(uint32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_OVFL, value, sizeof(uint64_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_CODE, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_MASK, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_CODE, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_MASK, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_CODE, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_MASK, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_CODE, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_MASK, value, sizeof(int32_t)-1U);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_ILLPARA once when realized

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

// @xctest TC12.5.1: Get property values from library when interface is not initialized.
//
// @expected: CANERR_NOERROR
//
//- (void)testWhenInterfaceNotInitializedFromLibrary {
//        TODO: insert coin here
//}

// @xctest TC12.5.2: Get property values from device when interface is not initialized.
//
// @todo: rework this
//
// @expected: CANERR_NOTINIT
//
- (void)testWhenInterfaceNotInitializedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @test:
    // @- get library property from DUT1 with invalid handle -1 (should succeed)
    rc = can_property(INVALID_HANDLE, CANPROP_GET_SPEC, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_VERSION, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_PATCH_NO, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_BUILD_NO, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_LIBRARY_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_LIBRARY_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_LIBRARY_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_SET_FIRST_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_CHANNEL_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_CHANNEL_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_CHANNEL_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_CHANNEL_VENDOR_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_GET_CHANNEL_VENDOR_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(INVALID_HANDLE, CANPROP_SET_NEXT_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get property LIBRARY_ID from DUT1 with invalid handle INT32_MIN (should succeed)
    rc = can_property(INT32_MIN, CANPROP_GET_LIBRARY_ID, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- try to get property DEVICE_TYPE from DUT1 with invalid handle INT32_MAX
    rc = can_property(INT32_MAX, CANPROP_GET_DEVICE_TYPE, value, CANPROP_MAX_BUFFER_SIZE);
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

// @xctest TC12.6.1: Get property values fom library when interface initialized (but CAN controller not started).
//
// @expected: CANERR_NOERROR
//
//- (void)testWhenInterfaceInitializedFromLibrary {
//        TODO: insert coin here
//}

// @xctest TC12.6.2: Get property values fom device when interface initialized (but CAN controller not started).
//
// @todo: rework this
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceInitializedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    
    // @test
    // @- get library properties from DUT1
    rc = can_property(handle, CANPROP_GET_SPEC, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_VERSION, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_PATCH_NO, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BUILD_NO, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_FIRST_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_NEXT_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get device properties from DUT1
    rc = can_property(handle, CANPROP_GET_DEVICE_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_PARAM, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // @note: device params not used by the driver!
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_OP_MODE, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BITRATE, value, sizeof(can_bitrate_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_SPEED, value, sizeof(can_speed_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_STATUS, value, sizeof(can_status_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BUSLOAD, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_TX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_RX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_ERR_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_MAX, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_HIGH, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_OVFL, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized

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

// @xctest TC12.7.1: Get property values from library when CAN controller started.
//
// @expected: CANERR_NOERROR
//
//- (void)testWhenInterfaceStartedFromLibrary {
//        TODO: insert coin here
//}

// @xctest TC12.7.2: Get property values from device when CAN controller started.
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceStartedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
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
    // @- get library properties from DUT1
    rc = can_property(handle, CANPROP_GET_SPEC, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_VERSION, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_PATCH_NO, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BUILD_NO, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_FIRST_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_NEXT_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get device properties from DUT1
    rc = can_property(handle, CANPROP_GET_DEVICE_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_PARAM, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // @note: device params not used by the driver!
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_OP_MODE, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BITRATE, value, sizeof(can_bitrate_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_SPEED, value, sizeof(can_speed_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_STATUS, value, sizeof(can_status_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BUSLOAD, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_TX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_RX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_ERR_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_MAX, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_HIGH, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_OVFL, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
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

// @xctest TC12.8.1: Get property values from library when CAN controller stopped.
//
// @expected: CANERR_NOERROR
//
//- (void)testWhenInterfaceStoppedFromLibray {
//        TODO: insert coin here
//}

// @xctest TC12.8.2: Get property values from device when CAN controller stopped.
//
// @todo: rework this
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceStoppedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
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
    // @- get library properties from DUT1
    rc = can_property(handle, CANPROP_GET_SPEC, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_VERSION, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_PATCH_NO, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BUILD_NO, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_FIRST_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_NEXT_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get device properties from DUT1
    rc = can_property(handle, CANPROP_GET_DEVICE_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_PARAM, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // @note: device params not used by the driver!
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_OP_MODE, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BITRATE, value, sizeof(can_bitrate_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_SPEED, value, sizeof(can_speed_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_STATUS, value, sizeof(can_status_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BUSLOAD, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_TX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_RX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_ERR_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_MAX, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_HIGH, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_OVFL, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOERROR once when realized
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    
    // @post:
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC12.9.1: Get property values from library when interface already shutdown.
//
// @expected: CANERR_NOERROR
//
//- (void)testWhenInterfaceShutdownFromLibray {
//        TODO: insert coin here
//}

// @xctest TC12.9.2: Get property values from device when interface already shutdown.
//
// @todo: rework this
//
// @expected: CANERR_NOTINIT
//
- (void)testWhenInterfaceShutdownFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
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
    // @- get library properties from DUT1
    rc = can_property(handle, CANPROP_GET_SPEC, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_VERSION, value, sizeof(uint16_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_PATCH_NO, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_BUILD_NO, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_LIBRARY_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_FIRST_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_ID, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_GET_CHANNEL_VENDOR_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);
    rc = can_property(handle, CANPROP_SET_NEXT_CHANNEL, NULL, 0U);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- get device properties from DUT1
    rc = can_property(handle, CANPROP_GET_DEVICE_TYPE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_NAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_DEVICE_VENDOR, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);  // @note: param DEVICE_VENDOR does not require a valid handle.
    rc = can_property(handle, CANPROP_GET_DEVICE_DLLNAME, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOERROR, rc);  // @note: param DEVICE_DLLNAME does not require a valid handle.
    rc = can_property(handle, CANPROP_GET_DEVICE_PARAM, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // @note: device params not used by the driver.
    rc = can_property(handle, CANPROP_GET_OP_CAPABILITY, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_OP_MODE, value, sizeof(can_mode_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_BITRATE, value, sizeof(can_bitrate_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_SPEED, value, sizeof(can_speed_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_STATUS, value, sizeof(can_status_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_BUSLOAD, value, sizeof(uint8_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_TX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_RX_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_ERR_COUNTER, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOTINIT, rc);
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_MAX, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_HIGH, value, sizeof(uint32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_GET_RCV_QUEUE_OVFL, value, sizeof(uint64_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_GET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_GET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_SET_FLT_11BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_CODE, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
    rc = can_property(handle, CANPROP_SET_FLT_29BIT_MASK, value, sizeof(int32_t));
    XCTAssertEqual(CANERR_NOTSUPP, rc);  // TODO: CANERR_NOTINIT once when realized
}

@end

// $Id: test_can_property.mm 1035 2021-12-21 12:03:27Z makemake $  Copyright (c) UV Software, Berlin //
