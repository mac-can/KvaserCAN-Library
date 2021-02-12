//
//  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
//
//  Copyright (C) 2020-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//
//  This file is part of MacCAN-KvaserCAN.
//
//  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with MacCAN-KvaserCAN.  If not, see <https://www.gnu.org/licenses/>.
//
#ifndef KVASERCAN_H_INCLUDED
#define KVASERCAN_H_INCLUDED

#include "MacCAN.h"

/// \name   KvaserCAN
/// \brief  KvaserCAN dynamic library
/// \{
#define KVASERCAN_LIBRARY_ID  CANLIB_KVASER_32
#if (OPTION_CANAPI_KVASERCAN_DYLIB != 0)
  #define KVASERCAN_LIBRARY_NAME  CANDLL_KVASERCAN
#else
  #define KVASERCAN_LIBRARY_NAME  "libKvaserCAN.dylib"
#endif
#define KVASERCAN_LIBRARY_VENDOR  "UV Software, Berlin"
#define KVASERCAN_LIBRARY_LICENSE  "GNU Lesser General Public License, Version 3"
#define KVASERCAN_LIBRARY_COPYRIGHT  "Copyright (C) 2020-2021  Uwe Vogt, UV Software, Berlin"
#define KVASERCAN_LIBRARY_HAZARD_NOTE  "If you connect your CAN device to a real CAN network when using this library,\n" \
                                       "you might damage your application."
/// \}

/// \name   KvaserCAN API
/// \brief  MacCAN driver for Kvaser CAN Leaf  interfaces
/// \note   See CMacCAN for a description of the overridden methods
/// \{
class CKvaserCAN : public CMacCAN {
private:
    MacCAN_OpMode_t m_OpMode;  ///< CAN operation mode
    MacCAN_Status_t m_Status;  ///< CAN status register
    struct {
        uint64_t m_u64TxMessages;  ///< number of transmitted CAN messages
        uint64_t m_u64RxMessages;  ///< number of received CAN messages
        uint64_t m_u64ErrorFrames;  ///< number of received status messages
    } m_Counter;
    // opaque data type
    struct SCAN;  ///< C++ forward declaration
    SCAN *m_pCAN;  ///< KvaserCAN USB device interface
public:
    // constructor / destructor
    CKvaserCAN();
    ~CKvaserCAN();
    // CKvaserCAN-specific error codes (CAN API V3 extension)
    enum EErrorCodes {
        // note: range 0...-99 is reserved by CAN API V3
        GeneralError = VendorSpecific
    };
    // CMacCAN overrides
    static MacCAN_Return_t ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, const void *param, EChannelState &state);
    static MacCAN_Return_t ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, EChannelState &state);

    MacCAN_Return_t InitializeChannel(int32_t channel, MacCAN_OpMode_t opMode, const void *param = NULL);
    MacCAN_Return_t TeardownChannel();
    MacCAN_Return_t SignalChannel();

    MacCAN_Return_t StartController(MacCAN_Bitrate_t bitrate);
    MacCAN_Return_t ResetController();

    MacCAN_Return_t WriteMessage(MacCAN_Message_t message, uint16_t timeout = 0U);
    MacCAN_Return_t ReadMessage(MacCAN_Message_t &message, uint16_t timeout = CANREAD_INFINITE);

    MacCAN_Return_t GetStatus(MacCAN_Status_t &status);
    MacCAN_Return_t GetBusLoad(uint8_t &load);

    MacCAN_Return_t GetBitrate(MacCAN_Bitrate_t &bitrate);
    MacCAN_Return_t GetBusSpeed(MacCAN_BusSpeed_t &speed);

    MacCAN_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte);
    MacCAN_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte);

    char *GetHardwareVersion();  // (for compatibility reasons)
    char *GetFirmwareVersion();  // (for compatibility reasons)
    static char *GetVersion();  // (for compatibility reasons)
private:
    MacCAN_Return_t MapBitrate2BusParams(MacCAN_Bitrate_t bitrate,
                                         long &freq,
                                         unsigned int &tseg1,
                                         unsigned int &tseg2,
                                         unsigned int &sjw,
                                         unsigned int &noSamp,
                                         unsigned int &syncmode);
    MacCAN_Return_t MapBusParams2Bitrate(long freq,
                                         unsigned int tseg1,
                                         unsigned int tseg2,
                                         unsigned int sjw,
                                         unsigned int noSamp,
                                         unsigned int syncmode,
                                         MacCAN_Bitrate_t &bitrate);
};
/// \}

/// \name   KvaserCAN Property IDs
/// \brief  Properties that can be read (or written)
/// \{
#define KVASERCAN_PROPERTY_CANAPI         (CANPROP_GET_SPEC)
#define KVASERCAN_PROPERTY_VERSION        (CANPROP_GET_VERSION)
#define KVASERCAN_PROPERTY_PATCH_NO       (CANPROP_GET_PATCH_NO)
#define KVASERCAN_PROPERTY_BUILD_NO       (CANPROP_GET_BUILD_NO)
#define KVASERCAN_PROPERTY_LIBRARY_ID     (CANPROP_GET_LIBRARY_ID)
#define KVASERCAN_PROPERTY_LIBRARY_NAME   (CANPROP_GET_LIBRARY_DLLNAME)
#define KVASERCAN_PROPERTY_LIBRARY_VENDOR (CANPROP_GET_LIBRARY_VENDOR)
#define KVASERCAN_PROPERTY_DEVICE_TYPE    (CANPROP_GET_DEVICE_TYPE)
#define KVASERCAN_PROPERTY_DEVICE_NAME    (CANPROP_GET_DEVICE_NAME)
#define KVASERCAN_PROPERTY_DEVICE_DRIVER  (CANPROP_GET_DEVICE_DLLNAME)
#define KVASERCAN_PROPERTY_DEVICE_VENDOR  (CANPROP_GET_DEVICE_VENDOR)
#define KVASERCAN_PROPERTY_OP_CAPABILITY  (CANPROP_GET_OP_CAPABILITY)
#define KVASERCAN_PROPERTY_OP_MODE        (CANPROP_GET_OP_MODE)
#define KVASERCAN_PROPERTY_BITRATE        (CANPROP_GET_BITRATE)
#define KVASERCAN_PROPERTY_SPEED          (CANPROP_GET_SPEED)
#define KVASERCAN_PROPERTY_STATUS         (CANPROP_GET_STATUS)
#define KVASERCAN_PROPERTY_BUSLOAD        (CANPROP_GET_BUSLOAD)
#define KVASERCAN_PROPERTY_TX_COUNTER     (CANPROP_GET_TX_COUNTER)
#define KVASERCAN_PROPERTY_RX_COUNTER     (CANPROP_GET_RX_COUNTER)
#define KVASERCAN_PROPERTY_ERR_COUNTER    (CANPROP_GET_ERR_COUNTER)
/// \}

#endif /* KVASERCAN_H_INCLUDED */
