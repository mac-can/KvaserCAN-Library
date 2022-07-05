### macOS&reg; User-Space Driver for USB CAN Interfaces from Kvaser

_Copyright &copy; 2020-2022 Uwe Vogt, UV Software, Berlin (info@mac-can.com)_

# Running CAN and CAN FD on Mac&reg;

_Running CAN and CAN FD on Mac_ is the mission of the MacCAN project.
The macOS driver for USB CAN interfaces from [Kvaser](https://www.kvaser.com) is based on _MacCAN-Core_ which is an abstraction (or rather a wrapper) of Apple´s IOUsbKit to create USB user-space drivers for CAN interfaces from various vendors under macOS.

## MacCAN-KvaserCAN

This repo contains the source code for the _MacCAN-KvaserCAN_ driver and several alternatives to build dynamic libraries for macOS,
either as a C++ class library ([_libKvaserCAN_](#libKvaserCAN)),
or as a _CAN API V3_ driver library ([_libUVCANKVL_](#libUVCANKVL)),
as well as my beloved CAN utilities [`can_moni`](#can_moni) and [`can_test`](#can_test),
and some C/C++, Swift, and Python example programs.

_CAN API V3_ is a wrapper specification to have a uniform CAN Interface API for various CAN interfaces from different vendors running under multiple operating systems.
See header file `CANAPI.h` for the CAN API V3 wrapper specification.

Note: _This project does not aim to implement Kvaser´s CANlib library on macOS._

### MacCAN-KvaserCAN API

```C++
/// \name   KvaserCAN API
/// \brief  CAN API V3 driver for Kvaser CAN interfaces
/// \note   See CCanApi for a description of the overridden methods
/// \{
class CKvaserCAN : public CCanApi {
public:
    // constructor / destructor
    CKvaserCAN();
    ~CKvaserCAN();

    // CCanApi overrides
    static bool GetFirstChannel(SChannelInfo &info, void *param = NULL);
    static bool GetNextChannel(SChannelInfo &info, void *param = NULL);

    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param, EChannelState &state);
    static CANAPI_Return_t ProbeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, EChannelState &state);

    CANAPI_Return_t InitializeChannel(int32_t channel, const CANAPI_OpMode_t &opMode, const void *param = NULL);
    CANAPI_Return_t TeardownChannel();
    CANAPI_Return_t SignalChannel();

    CANAPI_Return_t StartController(CANAPI_Bitrate_t bitrate);
    CANAPI_Return_t ResetController();

    CANAPI_Return_t WriteMessage(CANAPI_Message_t message, uint16_t timeout = 0U);
    CANAPI_Return_t ReadMessage(CANAPI_Message_t &message, uint16_t timeout = CANREAD_INFINITE);

    CANAPI_Return_t GetStatus(CANAPI_Status_t &status);
    CANAPI_Return_t GetBusLoad(uint8_t &load);

    CANAPI_Return_t GetBitrate(CANAPI_Bitrate_t &bitrate);
    CANAPI_Return_t GetBusSpeed(CANAPI_BusSpeed_t &speed);

    CANAPI_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte);
    CANAPI_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte);

    char *GetHardwareVersion();  // (for compatibility reasons)
    char *GetFirmwareVersion();  // (for compatibility reasons)
    static char *GetVersion();  // (for compatibility reasons)

    static CANAPI_Return_t MapIndex2Bitrate(int32_t index, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapString2Bitrate(const char *string, CANAPI_Bitrate_t &bitrate);
    static CANAPI_Return_t MapBitrate2String(CANAPI_Bitrate_t bitrate, char *string, size_t length);
    static CANAPI_Return_t MapBitrate2Speed(CANAPI_Bitrate_t bitrate, CANAPI_BusSpeed_t &speed);

    static uint8_t Dlc2Len(uint8_t dlc) { return CCanApi::Dlc2Len(dlc); }
    static uint8_t Len2Dlc(uint8_t len) { return CCanApi::Len2Dlc(len); }
};
/// \}
```

### Build Targets

_Important note_: To build any of the following build targets run the `build_no.sh` script to generate a pseudo build number.
```
uv-pc013mac:~ eris$ cd ~/Projects/MacCAN/KvaserCAN
uv-pc013mac:KvaserCAN eris$ ./build_no.sh
```
Repeat this step after each `git commit`, `git pull`, `git clone`, etc.

Then you can build all targets by typing the usual commands:
```
uv-pc013mac:~ eris$ cd ~/Projects/MacCAN/KvaserCAN
uv-pc013mac:KvaserCAN eris$ make clean
uv-pc013mac:KvaserCAN eris$ make all
uv-pc013mac:KvaserCAN eris$ sudo make install
uv-pc013mac:KvaserCAN eris$
```
_(The version number of the library can be adapted by editing the appropriated `Makefile` and changing the variable `VERSION` accordingly. Don´t forget to set the version number also in the source files.)_

#### libKvaserCAN

___libKvaserCAN___ is a dynamic library with a CAN API V3 compatible application programming interface for use in __C++__ applications.
See header file `KvaserCAN.h` for a description of all class members.

#### libUVCANKVL

___libUVCANKVL___ is a dynamic library with a CAN API V3 compatible application programming interface for use in __C__ applications.
See header file `can_api.h` for a description of all API functions.

#### can_moni

`can_moni` is a command line tool to view incoming CAN messages.
I hate this messing around with binary masks for identifier filtering.
So I wrote this little program to have an exclude list for single identifiers or identifier ranges (see program option `--exclude` or just `-x`). Precede the list with a `~` and you get an include list.

Type `can_moni --help` to display all program options.

#### can_test

`can_test` is a command line tool to test CAN communication.
Originally developed for electronic environmental tests on an embedded Linux system with SocketCAN, I´m using it for many years as a traffic generator for CAN stress-tests.

Type `can_test --help` to display all program options.

### Target Platform

- macOS 11.0 and later (Intel x64 and Apple silicon)

### Development Environment

#### macOS Monterey

- macOS Monterey (12.4) on a Mac mini (M1, 2020)
- Apple clang version 13.1.6 (clang-1316.0.21.2.5)
- Xcode Version 13.4.1 (13F100)

#### macOS Big Sur

- macOS Big Sur (11.6.7) on a MacBook Pro (2019)
- Apple clang version 13.0.0 (clang-1300.0.29.30)
- Xcode Version 13.2.1 (13C100)

### Supported CAN Hardware

- Kvaser Leaf Light v2 (EAN: 73-30130-00685-0)
- Kvaser Leaf Pro HS v2 (EAN: 73-30130-00843-4)
- Kvaser U100P (EAN: 73-30130-01174-8)

Since version 0.3 theoretically all (single-channel) CAN interfaces from the device family *Leaf Interfaces* (CAN 2.0 interfaces, e.g. Leaf Light v2) and from the device family *Mhydra Interfaces* (CAN FD interfaces, e.g. U100P) are supported.
To add a new CAN USB interface from Kvaser, only its USB ProductID and some device specific attributes have to be entered or enabled in the module `KvaserCAN_Devices`.

Note: _The CAN interfaces listed above are the ones that are enabled and have been tested._

### Testing

The Xcode project for the trial program includes an xctest target with one test suite for each CAN API V3 **C** interface function.
To run the test suites or single test cases two CAN devices are required.
General test settings can be adapted in the file `Settings.h`.

## Known Bugs and Caveats

- For a list of known bugs and caveats see tab [Issues](https://github.com/mac-can/MacCAN-KvaserCAN/issues) in the GitHub repo.

## This and That

### Wrapper Library for Windows&reg;

A CAN API V3 compatible Wrapper Library for Windows is also available.
It is build upon Kvaser´s CANlib DLL and can be downloaded from / cloned at [GitHub](https://github.com/uv-software/KvaserCAN-Wrapper).

### Dual-License

This work is dual-licensed under the terms of the BSD 2-Clause "Simplified" License
and under the terms of the GNU General Public License v3.0 (or any later version).
You can choose between one of them if you use this work in whole or in part.

`SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later`

### Trademarks

Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other countries. \
Windows is a registered trademark of Microsoft Corporation in the United States and/or other countries. \
Linux is a registered trademark of Linus Torvalds. \
All other company, product and service names mentioned herein are trademarks, registered trademarks or service marks of their respective owners.

### Hazard Note

_If you connect your CAN device to a real CAN network when using this library, you might damage your application._

### Contact

E-Mail: mailto://info@mac.can.com \
Internet: https://www.mac-can.net
