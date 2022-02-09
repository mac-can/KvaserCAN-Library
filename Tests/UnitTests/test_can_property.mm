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

#define MAX_PROPERTIES  33

#define LIB_PARAM  true
#define DRV_PARAM  false

#define GETTER  false
#define SETTER  true

#define REQUIRED  true
#define OPTIONAL  false

static struct property {
    uint16_t param;
    uint32_t nbyte;
    bool isLibParam;
    bool isSetParam;
    bool isRequired;
} properties[MAX_PROPERTIES] = {
    { CANPROP_GET_SPEC            , sizeof(uint16_t),        LIB_PARAM, GETTER, REQUIRED },  // version of the wrapper specification (uint16_t)
    { CANPROP_GET_VERSION         , sizeof(uint16_t),        LIB_PARAM, GETTER, REQUIRED },  // version number of the library (uint16_t)
    { CANPROP_GET_PATCH_NO        , sizeof(uint8_t),         LIB_PARAM, GETTER, REQUIRED },  // patch number of the library (uint8_t)
    { CANPROP_GET_BUILD_NO        , sizeof(uint32_t),        LIB_PARAM, GETTER, REQUIRED },  // build number of the library (uint32_t)
    { CANPROP_GET_LIBRARY_ID      , sizeof(int32_t),         LIB_PARAM, GETTER, REQUIRED },  // library id of the library (int32_t)
    { CANPROP_GET_LIBRARY_VENDOR  , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, GETTER, REQUIRED },  // vendor name of the library (char[256])
    { CANPROP_GET_LIBRARY_DLLNAME , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, GETTER, REQUIRED },  // file name of the library DLL (char[256])
    { CANPROP_GET_DEVICE_TYPE     , sizeof(int32_t),         DRV_PARAM, GETTER, REQUIRED },  // device type of the CAN interface (int32_t)
    { CANPROP_GET_DEVICE_NAME     , CANPROP_MAX_BUFFER_SIZE, DRV_PARAM, GETTER, REQUIRED },  // device name of the CAN interface (char[256])
    { CANPROP_GET_DEVICE_VENDOR   , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, GETTER, REQUIRED },  // vendor name of the CAN interface (char[256])
    { CANPROP_GET_DEVICE_DLLNAME  , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, GETTER, REQUIRED },  // file name of the CAN interface DLL (char[256])
    { CANPROP_GET_DEVICE_PARAM    , CANPROP_MAX_BUFFER_SIZE, DRV_PARAM, GETTER, OPTIONAL },  // device parameter of the CAN interface (char[256])
    { CANPROP_GET_OP_CAPABILITY   , sizeof(uint8_t),         DRV_PARAM, GETTER, REQUIRED },  // supported operation modes of the CAN controller (uint8_t)
    { CANPROP_GET_OP_MODE         , sizeof(uint8_t),         DRV_PARAM, GETTER, REQUIRED },  // active operation mode of the CAN controller (uint8_t)
    { CANPROP_GET_BITRATE         , sizeof(can_bitrate_t),   DRV_PARAM, GETTER, REQUIRED },  // active bit-rate of the CAN controller (can_bitrate_t)
    { CANPROP_GET_SPEED           , sizeof(can_speed_t),     DRV_PARAM, GETTER, REQUIRED },  // active bus speed of the CAN controller (can_speed_t)
    { CANPROP_GET_STATUS          , sizeof(can_mode_t),      DRV_PARAM, GETTER, REQUIRED },  // current status register of the CAN controller (uint8_t)
    { CANPROP_GET_BUSLOAD         , sizeof(uint16_t),        DRV_PARAM, GETTER, REQUIRED },  // current bus load of the CAN controller (uint16_t)
    { CANPROP_GET_NUM_CHANNELS    , sizeof(uint8_t),         DRV_PARAM, GETTER, OPTIONAL },  // numbers of CAN channels on the CAN interface (uint8_t)
    { CANPROP_GET_CAN_CHANNEL     , sizeof(uint8_t),         DRV_PARAM, GETTER, OPTIONAL },  // active CAN channel on the CAN interface (uint8_t)
    { CANPROP_GET_TX_COUNTER      , sizeof(uint64_t),        DRV_PARAM, GETTER, REQUIRED },  // total number of sent messages (uint64_t)
    { CANPROP_GET_RX_COUNTER      , sizeof(uint64_t),        DRV_PARAM, GETTER, REQUIRED },  // total number of reveiced messages (uint64_t)
    { CANPROP_GET_ERR_COUNTER     , sizeof(uint64_t),        DRV_PARAM, GETTER, REQUIRED },  // total number of reveiced error frames (uint64_t)
    { CANPROP_GET_RCV_QUEUE_SIZE  , sizeof(uint32_t),        DRV_PARAM, GETTER, OPTIONAL },  // maximum number of message the receive queue can hold (uint32_t)
    { CANPROP_GET_RCV_QUEUE_HIGH  , sizeof(uint32_t),        DRV_PARAM, GETTER, OPTIONAL },  // maximum number of message the receive queue has hold (uint32_t)
    { CANPROP_GET_RCV_QUEUE_OVFL  , sizeof(uint64_t),        DRV_PARAM, GETTER, OPTIONAL },  // overflow counter of the receive queue (uint64_t)
    /* note:  SET_FIRST_CHANNEL must be called before any GET_CHANNEL_xyz, therefore we define it as a getter because it gets a (virtual) index */
    { CANPROP_SET_FIRST_CHANNEL   , 0U /* NULL pointer*/,    LIB_PARAM, GETTER, REQUIRED },  // set index to the first entry in the interface list (NULL) */
    { CANPROP_GET_CHANNEL_NO      , sizeof(int32_t),         LIB_PARAM, GETTER, REQUIRED },  // get channel no. at actual index in the interface list (int32_t) */
    { CANPROP_GET_CHANNEL_NAME    , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, GETTER, REQUIRED },  // get channel name at actual index in the interface list (char[256]) */
    { CANPROP_GET_CHANNEL_DLLNAME , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, GETTER, REQUIRED },  // get file name of the DLL at actual index in the interface list (char[256]) */
    { CANPROP_GET_CHANNEL_VENDOR_ID , sizeof(int32_t),       LIB_PARAM, GETTER, REQUIRED },  // get library id at actual index in the interface list (int32_t) */
    { CANPROP_GET_CHANNEL_VENDOR_NAME , CANPROP_MAX_BUFFER_SIZE, LIB_PARAM, GETTER, REQUIRED }, // get vendor name at actual index in the interface list (char[256]) */
    /* note:  SET_NEXT_CHANNEL is also defined as a setter because it gets the next (virtual) index (see above), but it can be EOF when there is no device entry */
    { CANPROP_SET_NEXT_CHANNEL    , 0U /* NULL pointer*/,    LIB_PARAM, GETTER, REQUIRED },  // set index to the next entry in the interface list (NULL) */
};

@implementation test_can_property

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    (void)can_exit(CANKILL_ALL);
}

// @xctest TC12.1.1: Get library property values with invalid interface handle(s)
//
// @expected: CANERR_HANDLE
//
- (void)testWithInvalidHandleFromLibrary {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);

    // @test:
    // @- loop over all library properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 with invalid handle -1
            rc = can_property(INVALID_HANDLE, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- loop over all library properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 with invalid handle INT32_MIN
            rc = can_property(INT32_MIN, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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
    // @- loop over all library properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 with invalid handle INT32_MAX
            rc = can_property(INT32_MAX, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @post:
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC12.1.2: Get device property values with invalid interface handle(s)
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
    int i;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);

    // @test:
    // @- loop over all device properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read device property from DUT1 with invalid handle -1
            rc = can_property(INVALID_HANDLE, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_HANDLE) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_HANDLE) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    // @- start DUT1 with configured bit-rate settings
    rc = can_start(handle, &bitrate);
    XCTAssertEqual(CANERR_NOERROR, rc);
    // @- loop over all device properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read device property from DUT1 with invalid handle INT32_MIN
            rc = can_property(INT32_MIN, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_HANDLE) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
            if (!((rc == CANERR_HANDLE) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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
    // @- loop over all device properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read device property from DUT1 with invalid handle INT32_MAX
            rc = can_property(INT32_MIN, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_HANDLE) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_HANDLE) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);

    // @post:
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
    int i;

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
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isSetParam) {
            // @-- read device property from DUT1 with NULL for 'value'
            rc = can_property(handle, properties[i].param, NULL, properties[i].nbyte);
            if ((properties[i].param != CANPROP_SET_FIRST_CHANNEL) && (properties[i].param != CANPROP_SET_NEXT_CHANNEL)) {
                XCTAssertTrue((rc == CANERR_NULLPTR));
                // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
                if (!(rc == CANERR_NULLPTR))
                    break;
#endif
            } else {
                // @note: parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL accecpt NULL pointer.
                XCTAssertEqual(CANERR_NOERROR, rc);
            }
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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

// @xctest TC12.3.1: Get library property values with invalid value for parameter 'param'
//
// @expected: CANERR_NOTSUPP
//
- (void)testWithInvalidParmeterValueFromLibrary {
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
    // @- try to get library property from DUT1 with invalid parameter value 768
    rc = can_property(INVALID_HANDLE, CANPROP_SET_VENDOR_PROP+CANPROP_VENDOR_PROP_RANGE, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);
    // @- try to get library property from DUT1 with invalid parameter value UINT16_MAX
    rc = can_property(INVALID_HANDLE, UINT16_MAX, value, CANPROP_MAX_BUFFER_SIZE);
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

// @xctest TC12.3.2: Get device property values with invalid value for parameter 'param'
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
    // @- try to get device property from DUT1 with invalid parameter value 768
    rc = can_property(handle, CANPROP_SET_VENDOR_PROP+CANPROP_VENDOR_PROP_RANGE, value, CANPROP_MAX_BUFFER_SIZE);
    XCTAssertEqual(CANERR_NOTSUPP, rc);
    // @- try to get device property from DUT1 with invalid parameter value UINT16_MAX
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

// @xctest TC12.4.1: Get library property values with wrong parameter size (too small)
//
// @expected: CANERR_ILLPARA and CANERR_NOERROR for parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL
//
- (void)testWithWrongParameterSizeFromLibrary {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    
    // @test
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 with wrong size (too small)
            rc = can_property(INVALID_HANDLE, properties[i].param, (void*)value, 0U);  // FIXME: none string properties (size - 1)
            if ((properties[i].param != CANPROP_SET_FIRST_CHANNEL) && (properties[i].param != CANPROP_SET_NEXT_CHANNEL)) {
                XCTAssertTrue((rc == CANERR_ILLPARA) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
                // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
                if (!((rc == CANERR_ILLPARA) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                    break;
#endif
            } else {
                // @note: parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL accecpt NULL pointer.
                XCTAssertEqual(CANERR_NOERROR, rc);
            }
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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

// @xctest TC12.4.2: Get device property values with wrong parameter size (too small)
//
// @expected: CANERR_ILLPARA and CANERR_NOERROR for parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL
//
- (void)testWithWrongParameterSizeFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    
    // @test
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isSetParam) {
            // @-- read property from DUT1 with wrong size (too small)
            rc = can_property(handle, properties[i].param, (void*)value, 0U);  // FIXME: none string properties (size - 1)
            if ((properties[i].param != CANPROP_SET_FIRST_CHANNEL) && (properties[i].param != CANPROP_SET_NEXT_CHANNEL)) {
                XCTAssertTrue((rc == CANERR_ILLPARA) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
                // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
                if (!((rc == CANERR_ILLPARA) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                    break;
#endif
            } else {
                // @note: parameter CANPROP_SET_FIRST_CHANNEL and CANPROP_SET_NEXT_CHANNEL accecpt NULL pointer.
                XCTAssertEqual(CANERR_NOERROR, rc);
            }
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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

// @xctest TC12.5.1: Get library property values when interface is not initialized
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceNotInitializedFromLibrary {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    // @test:
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 when not initialized (should succeed)
            rc = can_property(0, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);

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

// @xctest TC12.5.2: Get device property values when interface is not initialized
//
// @expected: CANERR_NOTINIT
//
- (void)testWhenInterfaceNotInitializedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    // @test:
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read device property from DUT1 when not initialized (should fail)
            rc = can_property(0, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOTINIT) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOTINIT) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);

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

// @xctest TC12.6.1: Get library property values when interface initialized (but CAN controller not started)
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceInitializedFromLibrary {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    
    // @test
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 when not initialized (should succeed)
            rc = can_property(INVALID_HANDLE, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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

// @xctest TC12.6.2: Get device property values when interface initialized (but CAN controller not started)
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceInitializedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

    // @pre:
    // @- initialize DUT1 with configured settings
    handle = can_init(DUT1, TEST_CANMODE, NULL);
    XCTAssertLessThanOrEqual(0, handle);
    
    // @test
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isSetParam) {
            // @-- read property from DUT1 when not initialized (should succeed)
            rc = can_property(handle, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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

// @xctest TC12.7.1: Get library property values when CAN controller started
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceStartedFromLibrary {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

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
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 with handle -1
            rc = can_property(INVALID_HANDLE, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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

// @xctest TC12.7.2: Get device property values when CAN controller started
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceStartedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

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
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isSetParam) {
            // @-- read property from DUT1
            rc = can_property(handle, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
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

// @xctest TC12.8.1: Get library property values when CAN controller stoppe
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceStoppedFromlibrary {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

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
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 with handle -1
            rc = can_property(INVALID_HANDLE, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    
    // @post:
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC12.8.2: Get device property values when CAN controller stopped
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceStoppedFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

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
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isSetParam) {
            // @-- read property from DUT1
            rc = can_property(handle, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
           if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
    // @- get status of DUT1 and check to be in INIT state
    rc = can_status(handle, &status.byte);
    XCTAssertEqual(CANERR_NOERROR, rc);
    XCTAssertTrue(status.can_stopped);
    
    // @post:
    // @- shutdown DUT1
    rc = can_exit(handle);
    XCTAssertEqual(CANERR_NOERROR, rc);
}

// @xctest TC12.9.1: Get library property values when interface already shutdown
//
// @expected: CANERR_NOERROR
//
- (void)testWhenInterfaceShutdownFromlibrary {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

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
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read library property from DUT1 when not initialized (should fail)
            rc = can_property(handle, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOERROR) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
}

// @xctest TC12.9.2: Get device property values when interface already shutdown
//
// @expected: CANERR_NOTINIT
//
- (void)testWhenInterfaceShutdownFromDevice {
    can_bitrate_t bitrate = { TEST_BTRINDEX };
    can_status_t status = { CANSTAT_RESET };
    uint8_t value[CANPROP_MAX_BUFFER_SIZE] = {};
    int handle = INVALID_HANDLE;
    int rc = CANERR_FATAL;
    int i;

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
    // @- loop over all properties
    for (i = 0; i < MAX_PROPERTIES; i++) {
        if (!properties[i].isLibParam && !properties[i].isSetParam) {
            // @-- read property from DUT1 when not initialized (should fail)
            rc = can_property(handle, properties[i].param, (void*)value, properties[i].nbyte);
            XCTAssertTrue((rc == CANERR_NOTINIT) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP)));
            // @-- exit loop when failed
#if (EXIT_PROPERTY_LOOP_ON_ERROR != 0)
            if (!((rc == CANERR_NOTINIT) || (!properties[i].isRequired && (rc == CANERR_NOTSUPP))))
                break;
#endif
        }
    }
    XCTAssertEqual(MAX_PROPERTIES, i);
}

@end

// $Id: test_can_property.mm 1092 2022-01-15 21:14:19Z makemake $  Copyright (c) UV Software, Berlin //
