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
#include "build_no.h"
#define VERSION_MAJOR    0
#define VERSION_MINOR    1
#define VERSION_PATCH    0
#define VERSION_BUILD    BUILD_NO
#define VERSION_STRING   TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH) " (" TOSTRING(BUILD_NO) ")"
#if defined(__APPLE__)
#define PLATFORM    "macOS"
#else
#error Unsupported architecture
#endif
static const char version[] = PLATFORM " Driver for Kvaser CAN Leaf Interfaces, Version " VERSION_STRING;

#include "KvaserCAN.h"
#include "KvaserCAN_Driver.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "MacCAN_Debug.h"

#if (OPTION_KVASERCAN_DYLIB != 0)
__attribute__((constructor))
static void _initializer() {
    MACCAN_DEBUG_INFO("@@@ [%s] [%s]\n", __FILE__, __FUNCTION__);
    MacCAN_Return_t retVal = CMacCAN::Initializer();
    if (retVal != CMacCAN::NoError)
        MACCAN_DEBUG_ERROR("!!! Couldn't initialize dynamic library for Kvaser CAN Leaf interfaces (%i)\n", retVal);
}
__attribute__((destructor))
static void _finalizer() {
    MACCAN_DEBUG_INFO("@@@ [%s] [%s]\n", __FILE__, __FUNCTION__);
    MacCAN_Return_t retVal = CMacCAN::Finalizer();
    if (retVal != CMacCAN::NoError)
        MACCAN_DEBUG_ERROR("!!! Couldn't finalize dynamic library for Kvaser CAN Leaf interfaces (%i)\n", retVal);
}
#define EXPORT  __attribute__((visibility("default")))
#else
#define EXPORT
#endif

struct CKvaserCAN::SCAN {
    KvaserUSB_Device_t m_Device;
    // constructor
    SCAN() {
        bzero(&m_Device, sizeof(KvaserUSB_Device_t));
    }
};

EXPORT
CKvaserCAN::CKvaserCAN() {
    m_OpMode.byte = CANMODE_DEFAULT;
    m_Status.byte = CANSTAT_RESET;
    m_Counter.m_u64TxMessages = 0U;
    m_Counter.m_u64RxMessages = 0U;
    m_Counter.m_u64ErrorFrames = 0U;
    // create an empty device context
    m_pCAN = new SCAN;
}

EXPORT
CKvaserCAN::~CKvaserCAN() {
    // set CAN contoller into INIT mode and close USB device
    if (m_pCAN->m_Device.configured)
        (void)KvaserCAN_TeardownChannel(&m_pCAN->m_Device);
}

EXPORT
MacCAN_Return_t CKvaserCAN::ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, EChannelState &state) {
    // delegate this member function
    return ProbeChannel(channel, opMode, NULL, state);
}

EXPORT
MacCAN_Return_t CKvaserCAN::ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, const void *param, EChannelState &state) {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTSUPP;
    int result = CANBRD_NOT_TESTABLE;

    state = CMacCAN::ChannelNotTestable;

    if (channel < 0)
        return CMacCAN::IllegalParameter;
    if (param != NULL)
        return CMacCAN::IllegalParameter;

    // probe the CAN channel and check it given operation mode is supported by the CAN controller
    retVal = KvaserCAN_ProbeChannel((KvaserUSB_Channel_t)channel, opMode.byte, &result);
    switch (result) {
        case CANBRD_OCCUPIED: state = CMacCAN::ChannelOccupied; break;
        case CANBRD_PRESENT: state = CMacCAN::ChannelAvailable; break;
        case CANBRD_NOT_PRESENT: state = CMacCAN::ChannelNotAvailable; break;
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::InitializeChannel(int32_t channel, MacCAN_OpMode_t opMode, const void *param) {
    CANUSB_Return_t retVal = CANUSB_ERROR_YETINIT;

    if (channel < 0)
        return CMacCAN::IllegalParameter;
    if (param != NULL)
        return CMacCAN::IllegalParameter;

    // (§) must not be initialized
    if (!m_pCAN->m_Device.configured) {
        // set up an USB device context for the CAN channel and initialize the CAN controller
        // note: the demanded operation mode is checked against the capability of the CAN channel
        retVal = KvaserCAN_InitializeChannel((KvaserUSB_Channel_t)channel, opMode.byte, &m_pCAN->m_Device);
        if (retVal == CANUSB_SUCCESS) {
            // store CAN operation mode
            m_OpMode.byte = opMode.byte;
            // controller in INIT state
            m_Status.byte = CANSTAT_RESET;
        }
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::TeardownChannel() {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // stop the CAN controller and destroy the USB device context
        retVal = KvaserCAN_TeardownChannel(&m_pCAN->m_Device);
        if (retVal == CANUSB_SUCCESS) {
            // controller in INIT state
            m_Status.can_stopped = 1;
        }
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::SignalChannel() {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // signal all waiting conditions
        retVal = KvaserCAN_SignalChannel(&m_pCAN->m_Device);
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::StartController(MacCAN_Bitrate_t bitrate) {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;
    MacCAN_Bitrate_t tmpBitrate = bitrate;
    MacCAN_BusSpeed_t tmpSpeed = {};

    KvaserUSB_BusParams_t busParams = {};
    long freq = 250000;
    unsigned int tseg1 = 4;
    unsigned int tseg2 = 3;
    unsigned int sjw = 1;
    unsigned int noSamp = 1;
    unsigned int syncmode = 0;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // (a) check bit-rate settings (possibly after conversion from index)
        if (bitrate.btr.frequency <= 0) {
            // note: bit-rate settings are checked by the conversion function
            if (CMacCAN::MapIndex2Bitrate(bitrate.index, tmpBitrate) < 0)
                return CMacCAN::InvalidBaudrate;
        } else {
            // note: bit-rate settings are checked by the conversion function
            if (CMacCAN::MapBitrate2Speed(tmpBitrate, tmpSpeed) < 0)
                return CMacCAN::InvalidBaudrate;
        }
        // (b)convert bit-rate settings to Kvaser bus parameter
        if (CKvaserCAN::MapBitrate2BusParams(tmpBitrate, freq, tseg1, tseg2, sjw, noSamp, syncmode) < 0)
            return CMacCAN::InvalidBaudrate;
        busParams.bitRate = (uint32_t)freq;
        busParams.tseg1 = (uint8_t)tseg1;
        busParams.tseg2 = (uint8_t)tseg2;
        busParams.sjw = (uint8_t)sjw;
        busParams.noSamp = (uint8_t)noSamp;
        // (§) must be in INIT state
        if (m_Status.can_stopped) {
            // set bit-rate (with respect of the demanded operation mode)
            if ((retVal = KvaserCAN_SetBusParams(&m_pCAN->m_Device, &busParams)) < 0)
                return (retVal != CANUSB_ERROR_ILLPARA) ? (MacCAN_Return_t)retVal
                                                        : CMacCAN::InvalidBaudrate;
            // clear status and counters
            m_Status.byte = CANSTAT_RESET;
            m_Counter.m_u64TxMessages = 0U;
            m_Counter.m_u64RxMessages = 0U;
            m_Counter.m_u64ErrorFrames = 0U;
            // start the CAN controller with the demanded operation mode
            retVal = KvaserCAN_CanBusOn(&m_pCAN->m_Device, m_OpMode.mon ? true : false);
            m_Status.can_stopped = (retVal == CANUSB_SUCCESS) ? 0 : 1;
        } else
            retVal = CANERR_ONLINE;
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::ResetController() {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
    // (§) must be running
        if (!m_Status.can_stopped) {
            // stop the CAN controller (INIT state)
            retVal = KvaserCAN_CanBusOff(&m_pCAN->m_Device);
            m_Status.can_stopped = (retVal == CANUSB_SUCCESS) ? 1 : 0;
        } else
#ifndef OPTION_CANAPI_RETVALS
            retVal = CANERR_OFFLINE;
#else
            // note: CAN API `can_reset' returns CANERR_NOERROR even
            //       when the CAN controller has not been started
            retVal = CANERR_NOERROR;
#endif
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::WriteMessage(MacCAN_Message_t message, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // (a) check identifier range
        if (message.id > (message.xtd ? CAN_MAX_XTD_ID : CAN_MAX_STD_ID))
            return CMacCAN::IllegalParameter;
        // (b) check data length code
        if (message.dlc > CAN_MAX_DLC)
            return CMacCAN::IllegalParameter;
        // (c) ckeck extended frames allowed
        if (message.xtd && m_OpMode.nxtd)
            return CMacCAN::IllegalParameter;
        // (d) ckeck remote frames allowed
        if (message.rtr && m_OpMode.nrtr)
            return CMacCAN::IllegalParameter;
        // (§) must be running
        if (!m_Status.can_stopped) {
            // transmit the given CAN message (w/ or w/o acknowledgment)
            retVal = KvaserCAN_WriteMessage(&m_pCAN->m_Device, &message, timeout);
            m_Status.transmitter_busy = (retVal != CANUSB_SUCCESS) ? 1 : 0;
            m_Counter.m_u64TxMessages += (retVal == CANUSB_SUCCESS) ? 1U : 0U;
        } else
            retVal = CANERR_OFFLINE;
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::ReadMessage(MacCAN_Message_t &message, uint16_t timeout) {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // (§) must be running
        if (!m_Status.can_stopped) {
            // read one CAN message from the message queue, if any
            retVal = KvaserCAN_ReadMessage(&m_pCAN->m_Device, &message, timeout);
            m_Status.receiver_empty = (retVal != CANUSB_SUCCESS) ? 1 : 0;
            m_Status.queue_overrun = CANQUE_OverflowFlag(m_pCAN->m_Device.recvData.msgQueue) ? 1 : 0;
            m_Counter.m_u64RxMessages += ((retVal == CANUSB_SUCCESS) && !message.sts) ? 1U : 0U;
            m_Counter.m_u64ErrorFrames += ((retVal == CANUSB_SUCCESS) && message.sts) ? 1U : 0U;
        } else
            retVal = CANERR_OFFLINE;
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::GetStatus(MacCAN_Status_t &status) {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;
    KvaserUSB_BusStatus_t busStatus = 0x00U;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // get status register from device
        retVal = KvaserCAN_GetBusStatus(&m_pCAN->m_Device, &busStatus);
        if (retVal == CANUSB_SUCCESS) {
            m_Status.bus_off = (busStatus & BUSSTAT_BUSOFF)? 1 : 0;
            m_Status.bus_error = (busStatus & BUSSTAT_ERROR_PASSIVE)? 1 : 0;
            m_Status.warning_level = (busStatus & BUSSTAT_ERROR_WARNING)? 1 : 0;
            // TODO: m_Status.message_lost |= (busStatus & canSTAT_RXERR)? 1 : 0;
            // TODO: m_Status.transmitter_busy |= (busStatus & canSTAT_TX_PENDING)? 1 : 0;
        }
        // return updated status register
        status = m_Status;
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::GetBusLoad(uint8_t &load) {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;
    KvaserUSB_BusLoad_t busLoad = 0U;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // get bus load from device (0..10000 ==> 0%..100%)
        retVal = KvaserCAN_GetBusLoad(&m_pCAN->m_Device, &busLoad);
        if (retVal == CANUSB_SUCCESS)
            load = (uint8_t)((busLoad + 50U) / 100U);
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::GetBitrate(MacCAN_Bitrate_t &bitrate) {
    CANUSB_Return_t retVal = CANUSB_ERROR_NOTINIT;
    KvaserUSB_BusParams_t busParams = {};

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // get bit-rate settings from device
        retVal = KvaserCAN_GetBusParams(&m_pCAN->m_Device, &busParams);
        if (retVal == CANUSB_SUCCESS) {
            retVal = CKvaserCAN::MapBusParams2Bitrate((long)busParams.bitRate,
                                                      (unsigned int)busParams.tseg1,
                                                      (unsigned int)busParams.tseg2,
                                                      (unsigned int)busParams.sjw,
                                                      (unsigned int)busParams.noSamp,
                                                      (unsigned int)0, bitrate);
        }
#ifdef OPTION_CANAPI_RETVALS
        // note: CAN API `can_bitrate' returns CANERR_OFFLINE
        //       when the CAN controller has not been started
        if (m_Status.can_stopped)
            retVal = CANERR_OFFLINE;
#endif
    }
    return (MacCAN_Return_t)retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::GetBusSpeed(MacCAN_BusSpeed_t &speed) {
    MacCAN_Return_t retVal = CMacCAN::NotInitialized;
    MacCAN_Bitrate_t bitrate;

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        // get bit-rate settings from device and convert to bus-speed
        retVal = GetBitrate(bitrate);
        if (retVal == CANUSB_SUCCESS)
            retVal = CMacCAN::MapBitrate2Speed(bitrate, speed);
    }
    return retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::GetProperty(uint16_t param, void *value, uint32_t nbyte) {
    MacCAN_Return_t retVal = CMacCAN::IllegalParameter;

    if (!value)
        return CMacCAN::NullPointer;

    switch (param) {
        case KVASERCAN_PROPERTY_CANAPI:
            if ((size_t)nbyte == sizeof(uint16_t)) {
                *(uint16_t*)value = (uint16_t)CAN_API_SPEC;
                retVal = CMacCAN::NoError;
            }
            break;
        case KVASERCAN_PROPERTY_VERSION:
            if ((size_t)nbyte == sizeof(uint16_t)) {
                *(uint16_t*)value = ((uint16_t)VERSION_MAJOR << 8) | (uint16_t)VERSION_MINOR;
                retVal = CMacCAN::NoError;
            }
            break;
        case KVASERCAN_PROPERTY_PATCH_NO:
            if ((size_t)nbyte == sizeof(uint8_t)) {
                *(uint8_t*)value = (uint8_t)VERSION_PATCH;
                retVal = CMacCAN::NoError;
            }
            break;
        case KVASERCAN_PROPERTY_BUILD_NO:
            if ((size_t)nbyte == sizeof(uint32_t)) {
                *(uint32_t*)value = (uint32_t)VERSION_BUILD;
                retVal = CMacCAN::NoError;
            }
            break;
        case KVASERCAN_PROPERTY_LIBRARY_ID:
            if ((size_t)nbyte == sizeof(int32_t)) {
                *(int32_t*)value = (int32_t)KVASERCAN_LIBRARY_ID;
                retVal = CMacCAN::NoError;
            }
            break;
        case KVASERCAN_PROPERTY_LIBRARY_VENDOR:
            if ((nbyte > strlen(KVASERCAN_LIBRARY_VENDOR)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
                strcpy((char*)value, KVASERCAN_LIBRARY_VENDOR);
                retVal = CMacCAN::NoError;
            }
            break;
        case KVASERCAN_PROPERTY_LIBRARY_NAME:
            if ((nbyte > strlen(KVASERCAN_LIBRARY_NAME)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
                strcpy((char*)value, KVASERCAN_LIBRARY_NAME);
                retVal = CMacCAN::NoError;
            }
            break;
        default:
            // (§) CAN interface must be initialized to get these properties
            if (m_pCAN->m_Device.configured) {
                switch (param) {
                    case KVASERCAN_PROPERTY_DEVICE_TYPE:
                        if ((size_t)nbyte == sizeof(int32_t)) {
                            *(int32_t*)value = (int32_t)m_pCAN->m_Device.deviceInfo.card.hwType;
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_DEVICE_NAME:
                        if ((nbyte > strlen(m_pCAN->m_Device.name)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
                            strcpy((char*)value, m_pCAN->m_Device.name);
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_DEVICE_VENDOR:
                        if ((nbyte > strlen(m_pCAN->m_Device.vendor)) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
                            strcpy((char*)value, m_pCAN->m_Device.vendor);
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_DEVICE_DRIVER:
                        if ((nbyte > strlen("(driverless)")) && (nbyte <= CANPROP_MAX_BUFFER_SIZE)) {
                            strcpy((char*)value, "(driverless)");  // note: there is no kernel driver!
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_OP_CAPABILITY:
                        if ((size_t)nbyte == sizeof(uint8_t)) {
                            *(uint8_t*)value = (uint8_t)m_pCAN->m_Device.opCapability;
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_OP_MODE:
                        if ((size_t)nbyte == sizeof(uint8_t)) {
                            *(uint8_t*)value = (uint8_t)m_OpMode.byte;
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_BITRATE:
                        MacCAN_Bitrate_t bitrate;
                        if ((retVal = GetBitrate(bitrate)) == CANERR_NOERROR) {
                            if (nbyte == sizeof(MacCAN_Bitrate_t)) {
                                memcpy(value, &bitrate, sizeof(MacCAN_Bitrate_t));
                                retVal = CMacCAN::NoError;
                            }
                        }
                        break;
                    case KVASERCAN_PROPERTY_SPEED:
                        MacCAN_BusSpeed_t speed;
                        if ((retVal = GetBusSpeed(speed)) == CANERR_NOERROR) {
                            if (nbyte == sizeof(MacCAN_BusSpeed_t)) {
                                memcpy(value, &speed, sizeof(MacCAN_BusSpeed_t));
                                retVal = CMacCAN::NoError;
                            }
                        }
                        break;
                    case KVASERCAN_PROPERTY_STATUS:
                        MacCAN_Status_t status;
                        if ((retVal = GetStatus(status)) == CANERR_NOERROR) {
                            if ((size_t)nbyte == sizeof(uint8_t)) {
                                *(uint8_t*)value = (uint8_t)status.byte;
                                retVal = CMacCAN::NoError;
                            }
                        }
                        break;
                    case KVASERCAN_PROPERTY_BUSLOAD:
                        uint8_t load;
                        if ((retVal = GetBusLoad(load)) == CANERR_NOERROR) {
                            if ((size_t)nbyte == sizeof(uint8_t)) {
                                *(uint8_t*)value = (uint8_t)load;
                                retVal = CMacCAN::NoError;
                            }
                        }
                        break;
                    case KVASERCAN_PROPERTY_TX_COUNTER:
                        if ((size_t)nbyte == sizeof(uint64_t)) {
                            *(uint64_t*)value = (uint64_t)m_Counter.m_u64TxMessages;
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_RX_COUNTER:
                        if ((size_t)nbyte == sizeof(uint64_t)) {
                            *(uint64_t*)value = (uint64_t)m_Counter.m_u64RxMessages;
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    case KVASERCAN_PROPERTY_ERR_COUNTER:
                        if ((size_t)nbyte == sizeof(uint64_t)) {
                            *(uint64_t*)value = (uint64_t)m_Counter.m_u64ErrorFrames;
                            retVal = CMacCAN::NoError;
                        }
                        break;
                    default:
                        retVal = CMacCAN::NotSupported;
                        break;
                }
            } else {
                retVal = CMacCAN::NotInitialized;
            }
            break;
    }
    return retVal;
}

EXPORT
MacCAN_Return_t CKvaserCAN::SetProperty(uint16_t param, const void *value, uint32_t nbyte) {
    // TODO: currently there's nothing to be set!
    (void)param;
    (void)value;
    (void)nbyte;

    return CMacCAN::NotSupported;
}

EXPORT
char *CKvaserCAN::GetHardwareVersion() {
    static char string[CANPROP_MAX_BUFFER_SIZE] = "(unknown)";

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        uint8_t major = (uint8_t)m_pCAN->m_Device.deviceInfo.card.hwRevision;
        uint8_t minor = (uint8_t)0;
#if (0)
        sprintf(string, "%s, hardware revision %u.%u", m_pCAN->m_Device.name, major, minor);
#else
        uint8_t type = (uint8_t)m_pCAN->m_Device.deviceInfo.card.hwType;
        sprintf(string, "%s, hardware revision %u.%u (type %u)", m_pCAN->m_Device.name, major, minor, type);
#endif
        return string;
    }
    return NULL;
}

EXPORT
char *CKvaserCAN::GetFirmwareVersion() {
    static char string[CANPROP_MAX_BUFFER_SIZE] = "(unknown)";

    // (§) must be initialized
    if (m_pCAN->m_Device.configured) {
        uint8_t major = (uint8_t)(m_pCAN->m_Device.deviceInfo.software.firmwareVersion >> 24);
        uint8_t minor = (uint8_t)(m_pCAN->m_Device.deviceInfo.software.firmwareVersion >> 16);
#if (0)
        sprintf(string, "%s, firmware version %u.%u", m_pCAN->m_Device.name, major, minor);
#else
        uint16_t build = (uint16_t)(m_pCAN->m_Device.deviceInfo.software.firmwareVersion >> 0);
        sprintf(string, "%s, firmware version %u.%u (build %u)", m_pCAN->m_Device.name, major, minor, build);
#endif
        return string;
    }
    return NULL;
}

EXPORT
char *CKvaserCAN::GetVersion() {
    return (char *)&version[0];
}

//  methods for bit-rate conversion (CAN API V3 <==> Kvaser CANlib)
//
MacCAN_Return_t CKvaserCAN::MapBitrate2BusParams(MacCAN_Bitrate_t bitrate,
                                                 long &freq,
                                                 unsigned int &tseg1,
                                                 unsigned int &tseg2,
                                                 unsigned int &sjw,
                                                 unsigned int &noSamp,
                                                 unsigned int &syncmode) {
    // sanity check
    if ((bitrate.btr.nominal.brp < CANBTR_NOMINAL_BRP_MIN) || (CANBTR_NOMINAL_BRP_MAX < bitrate.btr.nominal.brp))
        return CMacCAN::InvalidBaudrate;
    if ((bitrate.btr.nominal.tseg1 < CANBTR_NOMINAL_TSEG1_MIN) || (CANBTR_NOMINAL_TSEG1_MAX < bitrate.btr.nominal.tseg1))
        return CMacCAN::InvalidBaudrate;
    if ((bitrate.btr.nominal.tseg2 < CANBTR_NOMINAL_TSEG2_MIN) || (CANBTR_NOMINAL_TSEG2_MAX < bitrate.btr.nominal.tseg2))
        return CMacCAN::InvalidBaudrate;
    if ((bitrate.btr.nominal.sjw < CANBTR_NOMINAL_SJW_MIN) || (CANBTR_NOMINAL_SJW_MAX < bitrate.btr.nominal.sjw))
        return CMacCAN::InvalidBaudrate;
    if (bitrate.btr.nominal.brp == 0)   // devide-by-zero!
        return CMacCAN::InvalidBaudrate;

    // bit-rate = frequency / (brp * (1 + tseg1 + tseg2))
    freq = (long)bitrate.btr.frequency  // Kvaser's freq means bit-rate!
         / ((long)bitrate.btr.nominal.brp * (1l + (long)bitrate.btr.nominal.tseg1 + (long)bitrate.btr.nominal.tseg2));
    tseg1 = (unsigned int)bitrate.btr.nominal.tseg1;
    tseg2 = (unsigned int)bitrate.btr.nominal.tseg2;
    sjw = (unsigned int)bitrate.btr.nominal.sjw;
    noSamp = (unsigned int)((bitrate.btr.nominal.sam != 0) ? 3 : 1);  // SJA1000: single or triple sampling
    syncmode = (unsigned int)0;

    return CMacCAN::NoError;
}

MacCAN_Return_t CKvaserCAN::MapBusParams2Bitrate(long freq,
                                                 unsigned int tseg1,
                                                 unsigned int tseg2,
                                                 unsigned int sjw,
                                                 unsigned int noSamp,
                                                 unsigned int syncmode,
                                                 MacCAN_Bitrate_t &bitrate) {
    // Kvaser canLin32 doesn't offer the used controller frequency and bit-rate prescaler.
    // We suppose it's running with 80MHz and calculate the bit-rate prescaler as follows:
    //
    // (1) brp = 80MHz / (bit-rate * (1 + tseg1 + tseq2))
    //
    if (freq <= 0)   // divide-by-zero!
        return CMacCAN::InvalidBaudrate;

    bitrate.btr.frequency = (int32_t)80000000;
    bitrate.btr.nominal.brp = (uint16_t)(80000000L
                            / (freq * (long)(1u + tseg1 + tseg2)));
    bitrate.btr.nominal.tseg1 = (uint16_t)tseg1;
    bitrate.btr.nominal.tseg2 = (uint16_t)tseg2;
    bitrate.btr.nominal.sjw = (uint16_t)sjw;
    bitrate.btr.nominal.sam = (uint8_t)((noSamp < 3) ? 0 : 1);  // SJA1000: single or triple sampling
#if (OPTION_CAN_2_0_ONLY == 0)
    bitrate.btr.data.brp = bitrate.btr.nominal.brp;
    bitrate.btr.data.tseg1 = bitrate.btr.nominal.tseg1;
    bitrate.btr.data.tseg2 = bitrate.btr.nominal.tseg2;
    bitrate.btr.data.sjw = bitrate.btr.nominal.sjw;
#endif
    (void)syncmode;

    // sanity check
    if ((bitrate.btr.nominal.brp < CANBTR_NOMINAL_BRP_MIN) || (CANBTR_NOMINAL_BRP_MAX < bitrate.btr.nominal.brp))
        return CMacCAN::InvalidBaudrate;
    if ((bitrate.btr.nominal.tseg1 < CANBTR_NOMINAL_TSEG1_MIN) || (CANBTR_NOMINAL_TSEG1_MAX < bitrate.btr.nominal.tseg1))
        return CMacCAN::InvalidBaudrate;
    if ((bitrate.btr.nominal.tseg2 < CANBTR_NOMINAL_TSEG2_MIN) || (CANBTR_NOMINAL_TSEG2_MAX < bitrate.btr.nominal.tseg2))
        return CMacCAN::InvalidBaudrate;
    if ((bitrate.btr.nominal.sjw < CANBTR_NOMINAL_SJW_MIN) || (CANBTR_NOMINAL_SJW_MAX < bitrate.btr.nominal.sjw))
        return CMacCAN::InvalidBaudrate;
    return CMacCAN::NoError;
}
