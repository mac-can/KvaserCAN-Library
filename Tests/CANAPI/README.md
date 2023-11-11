### CAN API V3 Testing with GoogleTest

_Copyright &copy; 2004-2023 Uwe Vogt, UV Software, Berlin (info@uv-software.com)_ \
_All rights reserved._

Version $Rev: 1219 $

# CAN Interface Wrapper Specification

[CAN API V3](https://mac-can.github.io/wrapper/canapi-v3/) is a wrapper specification by UV Software to have a uniform Application Programming Interface (API) for various CAN interface devices from different vendors running under multiple operating systems.
Due to the fact that the CAN APIs of the different vendors are not compatible with each other, UV Software has defined a CAN Interface Wrapper specification.
Its goal is to have a multi-vendor, cross-platform CAN Interface API.

# CAN Interface API, Version 3

In case of doubt the source code:

```C++
class CCanApi {
public:
    enum EErrorCodes {
        NoError = CANERR_NOERROR,
        BusOFF = CANERR_BOFF,
        ErrorWarning = CANERR_EWRN,
        BusError = CANERR_BERR,
        ControllerOffline = CANERR_OFFLINE,
        ControllerOnline = CANERR_ONLINE,
        MessageLost = CANERR_MSG_LST,
        TransmitterBusy = CANERR_TX_BUSY,
        ReceiverEmpty = CANERR_RX_EMPTY,
        QueueOverrun = CANERR_QUE_OVR,
        Timeout = CANERR_TIMEOUT,
        ResourceError = CANERR_RESOURCE,
        InvalidBaudrate = CANERR_BAUDRATE,
        InvalidHandle = CANERR_HANDLE,
        IllegalParameter = CANERR_ILLPARA,
        NullPointer = CANERR_NULLPTR,
        NotInitialized = CANERR_NOTINIT,
        AlreadyInitialized = CANERR_YETINIT,
        InvalidLibrary = CANERR_LIBRARY,
        NotSupported = CANERR_NOTSUPP,
        FatalError = CANERR_FATAL,
        VendorSpecific = CANERR_VENDOR
    };
    enum EChannelState {
        ChannelOccupied = CANBRD_OCCUPIED,
        ChannelAvailable = CANBRD_PRESENT,
        ChannelNotAvailable = CANBRD_NOT_PRESENT,
        ChannelNotTestable  = CANBRD_NOT_TESTABLE 
    };
    struct SChannelInfo {
        int32_t m_nChannelNo;
        char m_szDeviceName[CANPROP_MAX_BUFFER_SIZE];
        char m_szDeviceDllName[CANPROP_MAX_BUFFER_SIZE];
        int32_t m_nLibraryId;
        char m_szVendorName[CANPROP_MAX_BUFFER_SIZE];
    };
#if (OPTION_CANAPI_LIBRARY != 0)
    struct SLibraryInfo {
        int32_t m_nLibraryId;
        char m_szVendorName[CANPROP_MAX_BUFFER_SIZE];
        char m_szVendorDllName[CANPROP_MAX_BUFFER_SIZE];
    };
    static bool GetFirstLibrary(SLibraryInfo &info);
    static bool GetNextLibrary(SLibraryInfo &info);
    static bool GetFirstChannel(int32_t library, SChannelInfo &info, void *param = NULL);
    static bool GetNextChannel(int32_t library, SChannelInfo &info, void *param = NULL);
#else
    static bool GetFirstChannel(SChannelInfo &info, void *param = NULL);
    static bool GetNextChannel(SChannelInfo &info, void *param = NULL);
#endif
#if (OPTION_CANAPI_LIBRARY != 0)
    static CANAPI_Return_t ProbeChannel(int32_t library, int32_t channel, const CANAPI_OpMode_t &opMode, const void *param, EChannelState &state);
    static CANAPI_Return_t ProbeChannel(int32_t library, int32_t channel, const CANAPI_OpMode_t &opMode, EChannelState &state);
#else
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param, EChannelState &state);
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, EChannelState &state);
#endif
#if (OPTION_CANAPI_LIBRARY != 0)
    virtual CANAPI_Return_t InitializeChannel(int32_t library, int32_t channel, const CANAPI_OpMode_t &opMode, const void *param = NULL) = 0;
#else
    virtual CANAPI_Return_t InitializeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param = NULL) = 0;
#endif
    virtual CANAPI_Return_t SignalChannel() = 0;
    virtual CANAPI_Return_t StartController(CANAPI_Bitrate_t bitrate) = 0;
    virtual CANAPI_Return_t ResetController() = 0;
    virtual CANAPI_Return_t WriteMessage(CANAPI_Message_t message, uint16_t timeout = 0U) = 0;
    virtual CANAPI_Return_t ReadMessage(CANAPI_Message_t &message, uint16_t timeout = CANREAD_INFINITE) = 0;
    virtual CANAPI_Return_t GetStatus(CANAPI_Status_t &status) = 0;
    virtual CANAPI_Return_t GetBusLoad(uint8_t &load) = 0;  // deprecated
    virtual CANAPI_Return_t GetBitrate(CANAPI_Bitrate_t &bitrate) = 0;
    virtual CANAPI_Return_t GetBusSpeed(CANAPI_BusSpeed_t &speed) = 0;
    virtual CANAPI_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte) = 0;
    virtual CANAPI_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte) = 0;
    virtual char *GetHardwareVersion() = 0;  // deprecated
    virtual char *GetFirmwareVersion() = 0;  // deprecated
    static char *GetVersion();
public:
    static CANAPI_Return_t MapIndex2Bitrate(int32_t index, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapString2Bitrate(const char *string, CANAPI_Bitrate_t &bitrate, bool &data, bool &sam);
    static CANAPI_Return_t MapBitrate2String(CANAPI_Bitrate_t bitrate, char *string, size_t length, bool data = false, bool sam = false);
    static CANAPI_Return_t MapBitrate2Speed(CANAPI_Bitrate_t bitrate, CANAPI_BusSpeed_t &speed);
public:
    static uint8_t Dlc2Len(uint8_t dlc);
    static uint8_t Len2Dlc(uint8_t len);
};
```
See header file `CANAPI.h` for a description of the provided methods.

## CAN API V3 Testing

### GoogleTest

GoogleTest (aka gtest) is a unit testing library for the C++ programming language based on the xUnit architecture.
GoogleTest is used for testing of CAN API V3 C++ wrapper implementations.

See `README.txt` in `$(PROJROOT)/Tests/GoogleTest` for more information about GoogleTest.

### Usage

Two CAN devices on the same CAN bus are required to run the test program.
The test execution can be controlled by manifold options at the command line.

Call the test program with option `--help` to see all GoogleTest test options or
call the test program with option `--can_help` to see advanced test options to control the test execution.

## This and That

### SVN Repo

The CAN API V3 Testing sources are maintained in a SVN repo to synchronized them between the different CAN API V3 wrapper repos.

### Dual-License

This work is dual-licensed under the terms of the BSD 2-Clause "Simplified" License and under the terms of the GNU General Public License v3.0 (or any later version).
You can choose between one of them if you use this work in whole or in part.

`SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later`

### GoogleTest License

This work uses header files and generated binary code from **Google Test Framework** (https://github.com/google/googletest).
The Google Test Framework (GoogleTest) itself is not part of this work.

Please note the following notices, information, and license agreement.

```
Copyright 2008, Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
    * Neither the name of Google Inc. nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

### Trademarks

All company, product and service names mentioned herein may be trademarks, registered trademarks or service marks of their respective owners.

### Contact

E-Mail: mailto://info@uv-software.com \
Internet: https://www.uv-software.com
