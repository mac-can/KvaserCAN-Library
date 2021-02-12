/*
 *  MacCAN - macOS User-Space Driver for USB-to-CAN Interfaces
 *
 *  Copyright (C) 2012-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *
 *  This file is part of MacCAN-Core.
 *
 *  MacCAN-Core is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MacCAN-Core is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MacCAN-Core.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file      MacCAN.h
 *
 *  @brief     MacCAN - macOS User-Space Driver for CAN to USB Interfaces.
 *
 *             Running CAN and CAN FD on a Mac is the mission of the MacCAN
 *             project. MacCAN-Core is an abstraction or rather a wrapper
 *             of Apple's IOUsbKit to create USB user-space drivers for
 *             CAN interfaces from various vendors under macOS.
 *
 *  @note      This header file provides all data types and defines for the
 *             creation of an USB driver for CAN interfaces under macOS.
 *             The provided data types and defines are compatible with the
 *             CAN API V3 wrapper specification (C part of this header).
 *
 *             It also provides an abstract class with pure virtual methods
 *             as an interface specification for derived MacCAN USB driver
 *             implementation classes (C++ part of this header).
 *
 *             The MacCAN class CMacCAN provides two static methods to
 *             initialize and shutdown the MacCAN IOUsbKit. These methods
 *             must be called at the beginning / at the end of a MacCAN
 *             application or in the initializer / in the finalizer
 *             function of a dynamic library.
 *
 *             Additionally the class CMacCAN provides static methods for
 *             bit-rate conversion using CiA bit-timing indexes as base.
 *
 *  @author    $Author: eris $
 *
 *  @version   $Rev: 979 $
 *
 *  @defgroup  mac_can  macOS User-Space Driver for CAN to USB Interfaces
 *  @{
 */
#ifndef MACCAN_H_INCLUDED
#define MACCAN_H_INCLUDED

#include "CANAPI_Defines.h"
#include "CANAPI_Types.h"

/** @name   Aliases
 *  @brief  CAN API V3 Data-types.
 *  @{ */

/** @brief  CAN Status-register
 */
typedef can_status_t MacCAN_Status_t;

/** @brief  CAN Operation Mode
 */
typedef can_mode_t MacCAN_OpMode_t;

/** @brief  CAN Bit-rate Settings (nominal and data)
 */
typedef can_bitrate_t MacCAN_Bitrate_t;

/** @brief  CAN Transmission Rate (nominal and data)
 */
typedef can_speed_t MacCAN_BusSpeed_t;

/** @brief  CAN Time-stamp
 */
typedef can_timestamp_t MacCAN_Timestamp_t;

/** @brief  CAN Message (with Time-stamp)
 */
typedef can_message_t MacCAN_Message_t;

/** @brief  CAN Device handle (internally)
 */
typedef int MacCAN_Handle_t;

/** @brief  Function results (data type)
 */
typedef int MacCAN_Return_t;
/** @} */

#ifdef __cplusplus
/// \name   MacCAN API
/// \brief  MacCAN API based on CAN Interface API Version 3 (CAN API V3).
/// \note   To implement a MacCAN driver derive a class from abstract class
///         CMacCAN, and override all pure virtual functions and optionally
///         the static function 'ProbeChannel'.
/// \{
class CMacCAN {
public:
    /// \brief  CAN channel states
    enum EChannelState {
        ChannelOccupied = CANBRD_OCCUPIED, ///< channel is available, but occupied
        ChannelAvailable = CANBRD_PRESENT, ///< channel is available and can be used
        ChannelNotAvailable = CANBRD_NOT_PRESENT,  ///< channel is not available
        ChannelNotTestable  = CANBRD_NOT_TESTABLE  ///< channel is not testable
    };
    /// \brief  Common error codes (CAN API V3 compatible)
    enum EErrorCodes {
        NoError = CANERR_NOERROR,  ///< no error!
        BusOFF = CANERR_BOFF,  ///< busoff status
        ErrorWarning = CANERR_EWRN,  ///< error warning status
        BusError = CANERR_BERR,  ///< bus error
        ControllerOffline = CANERR_OFFLINE,  ///< not started
        ControllerOnline = CANERR_ONLINE,  ///< already started
        MessageLost = CANERR_MSG_LST,  ///< message lost
        TransmitterBusy = CANERR_TX_BUSY,  ///< transmitter busy
        ReceiverEmpty = CANERR_RX_EMPTY,  ///< receiver empty
        ErrorFrame = CANERR_ERR_FRAME,  ///< error frame
        Timeout = CANERR_TIMEOUT,  ///< timed out
        ResourceError = CANERR_RESOURCE,  ///< resource allocation
        InvalidBaudrate = CANERR_BAUDRATE,  ///<  illegal baudrate
        IllegalParameter = CANERR_ILLPARA,  ///< illegal parameter
        NullPointer = CANERR_NULLPTR,  ///< null-pointer assignment
        NotInitialized = CANERR_NOTINIT,  ///< not initialized
        AlreadyInitialized = CANERR_YETINIT,  ///< already initialized
        NotSupported = CANERR_NOTSUPP,  ///< not supported
        FatalError = CANERR_FATAL,  ///< fatal error
        VendorSpecific = CANERR_VENDOR  ///< offset for vendor-specific error code
    };
/// \name   MacCAN Driver Interface
/// \brief  Methods to be overridden by a MacCAN driver implementation.
/// \{
public:
    /// \brief       probes if the CAN interface (hardware and driver) given by
    ///              the argument 'channel' is present, and if the requested
    ///              operation mode is supported by the CAN controller.
    //
    /// \note        When a requested operation mode is not supported by the
    ///              CAN controller, error CANERR_ILLPARA will be returned.
    //
    /// \param[in]   channel - channel number of the CAN interface
    /// \param[in]   opMode  - operation mode to be checked
    /// \param[in]   param   - pointer to channel-specific parameters
    /// \param[out]  state   - state of the CAN channel:
    ///                            < 0 - channel is not present,
    ///                            = 0 - channel is present,
    ///                            > 0 - channel is present, but in use
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    static MacCAN_Return_t ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, const void *param, EChannelState &state);
    static MacCAN_Return_t ProbeChannel(int32_t channel, MacCAN_OpMode_t opMode, EChannelState &state);

    /// \brief       initializes the CAN interface (hardware and driver) given by
    ///              the argument 'channel'.
    ///              The operation state of the CAN controller is set to 'stopped';
    ///              no communication is possible in this state.
    //
    /// \param[in]   channel - channel number of the CAN interface
    /// \param[in]   opMode  - desired operation mode of the CAN controller
    /// \param[in]   param   - pointer to channel-specific parameters
    //
    /// \returns     handle of the CAN interface if successful,
    ///              or a negative value on error.
    //
    virtual MacCAN_Return_t InitializeChannel(int32_t channel, MacCAN_OpMode_t opMode, const void *param = NULL) = 0;

    /// \brief       stops any operation of the CAN interface and sets the operation
    ///              state of the CAN controller to 'stopped'.
    //
    /// \returns     0 if successful, or a negative value on error.
    ///
    virtual MacCAN_Return_t TeardownChannel() = 0;

    /// \brief       signals waiting event objects of the CAN interface. This can
    ///              be used to terminate blocking operations in progress
    ///              (e.g. by means of a Ctrl-C handler or similar).
    //
    /// \remarks     Some drivers are using waitable objects to realize blocking
    ///              operations by a call to WaitForSingleObject (Windows) or
    ///              pthread_cond_wait (POSIX), but these waitable objects are
    ///              no cancellation points. This means that they cannot be
    ///              terminated by Ctrl-C (SIGINT).
    //
    /// \note        Even though this is not the case with Darwin, we support
    ///              this feature for compatibility reasons..
    //
    /// \returns     0 if successful, or a negative value on error.
    ///
    virtual MacCAN_Return_t SignalChannel() = 0;

    /// \brief       initializes the operation mode and the bit-rate settings of the
    ///              CAN interface and sets the operation state of the CAN controller
    ///              to 'running'.
    //
    /// \note        All statistical counters (tx/rx/err) will be reset by this.
    //
    /// \param[in]   bitrate - bit-rate as btr register or baud rate index
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t StartController(MacCAN_Bitrate_t bitrate) = 0;

    /// \brief       stops any operation of the CAN interface and sets the operation
    ///              state of the CAN controller to 'stopped'; no communication is
    ///              possible in this state.
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t ResetController() = 0;

    /// \brief       transmits one message over the CAN bus. The CAN controller must
    ///              be in operation state 'running'.
    //
    /// \param[in]   message - the message to be sent
    /// \param[in]   timeout - time to wait for the transmission of the message:
    ///                             0 means the function returns immediately,
    ///                             65535 means blocking write, and any other
    ///                             value means the time to wait im milliseconds
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t WriteMessage(MacCAN_Message_t message, uint16_t timeout = 0U) = 0;

    /// \brief       read one message from the message queue of the CAN interface, if
    ///              any message was received. The CAN controller must be in operation
    ///              state 'running'.
    //
    /// \param[out]  message - the message read from the message queue, if any
    /// \param[in]   timeout - time to wait for the reception of a message:
    ///                             0 means the function returns immediately,
    ///                             65535 means blocking read, and any other
    ///                             value means the time to wait im milliseconds
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t ReadMessage(MacCAN_Message_t &message, uint16_t timeout = CANREAD_INFINITE) = 0;

    /// \brief       retrieves the status register of the CAN interface.
    //
    /// \param[out]  status  - 8-bit status register.
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t GetStatus(MacCAN_Status_t &status) = 0;

    /// \brief       retrieves the bus-load (in percent) of the CAN interface.
    //
    /// \param[out]  load    - bus-load in [%]
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t GetBusLoad(uint8_t &load) = 0;

    /// \brief       retrieves the bit-rate setting of the CAN interface. The
    ///              CAN controller must be in operation state 'running'.
    //
    /// \param[out]  bitrate - bit-rate setting
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t GetBitrate(MacCAN_Bitrate_t &bitrate) = 0;

    /// \brief       retrieves the transmission rate of the CAN interface. The
    ///              CAN controller must be in operation state 'running'.
    //
    /// \param[out]  speed   - transmission rate
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t GetBusSpeed(MacCAN_BusSpeed_t &speed) = 0;

    /// \brief       retrieves a property value of the CAN interface.
    //
    /// \param[in]   param    - property id to be read
    /// \param[out]  value    - pointer to a buffer for the value to be read
    /// \param[in]   nbytes   - size of the given buffer in bytes
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t GetProperty(uint16_t param, void *value, uint32_t nbyte) = 0;

    /// \brief       modifies a property value of the CAN interface.
    //
    /// \param[in]   param    - property id to be written
    /// \param[in]   value    - pointer to a buffer with the value to be written
    /// \param[in]   nbytes   - size of the given buffer in bytes
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    virtual MacCAN_Return_t SetProperty(uint16_t param, const void *value, uint32_t nbyte) = 0;

    /// \brief       retrieves the hardware version of the CAN controller
    ///              board as a zero-terminated string.
    //
    /// \returns     pointer to a zero-terminated string, or NULL on error.
    //
    virtual char *GetHardwareVersion() = 0;

    /// \brief       retrieves the firmware version of the CAN controller
    ///              board as a zero-terminated string.
    //
    /// \returns     pointer to a zero-terminated string, or NULL on error.
    //
    virtual char *GetFirmwareVersion() = 0;
/// \}

/// \name   MacCAN Bit-rate converstion
/// \brief  Methods for bit-rate conversion.
/// \note   To be overridden when required.
/// \{
public:
    static MacCAN_Return_t MapIndex2Bitrate(int32_t index, MacCAN_Bitrate_t &bitrate);
    static MacCAN_Return_t MapString2Bitrate(const char *string, MacCAN_Bitrate_t &bitrate);
    static MacCAN_Return_t MapBitrate2String(MacCAN_Bitrate_t bitrate, char *string, size_t length);
    static MacCAN_Return_t MapBitrate2Speed(MacCAN_Bitrate_t bitrate, MacCAN_BusSpeed_t &speed);
/// \}

/// \name   CAN FD Data Length Code
/// \brief  Methods for DLC conversion.
/// \{
public:
    static uint8_t DLc2Len(uint8_t dlc);
    static uint8_t Len2Dlc(uint8_t len);
/// \}

/// \name   MacCAN Core (C++ Part)
/// \brief  Methods to initialize and release the MacCAN IOUsbKit.
/// \{
public:
    /// \brief       initializes the IOUsbKit to be used by a MacCAN driver.
    //
    /// \note        A CAN device identification list of name \ref CANDEV_Devices
    ///              (an array with tuples of { <vendorId>, <productId>, <numChannels> },
    ///              terminated by a tuple of { (-1), (-1), 0 }) has to be provide.
    //
    /// \remarks     This method must be called at the beginning of a MacCAN application
    ///              or in the initializer function of a dynamic library.
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    static MacCAN_Return_t Initializer();

    /// \brief       releases the MacCAN IOUsbKit.
    //
    /// \returns     0 if successful, or a negative value on error.
    //
    static MacCAN_Return_t Finalizer();

    /// \brief       retrieves version information about the MacCAN Core
    ///              version as a zero-terminated string.
    //
    /// \remarks     A MacCAN driver implementation shall also implement
    ///              a static member function 'GetVersion' to retrieve
    ///              version information of the MacCAN driver.
    //
    /// \returns     pointer to a zero-terminated string, or NULL on error.
    //
    static char *GetVersion();
/// \}
};
/// \}
#else
/** @name   MacCAN Core (C Part)
 *  @brief  Functions to initialize and release the MacCAN IOUsbKit.
 *  @{
 */
extern MacCAN_Return_t MacCAN_Initializer(void);  /**< initializes the IOUsbKit to be used by a MacCAN driver. */
extern MacCAN_Return_t MacCAN_Finalizer(void);  /**< releases the MacCAN IOUsbKit. */
extern char *MacCAN_GetVersion(void);  /**< retrieves version information. */
/** @}
 */
#endif /* __cplusplus */
/** @}
 */
#endif /* MACCAN_H_INCLUDED */

/* * $Id: MacCAN.h 979 2021-01-04 20:16:55Z eris $ *** (C) UV Software, Berlin ***
 */
