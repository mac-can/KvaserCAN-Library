/*
 *  CAN Interface API, Version 3 (for Kvaser CAN Interfaces)
 *
 *  Copyright (C) 2017-2021  Uwe Vogt, UV Software, Berlin (info@uv-software.com)
 *
 *  This file is part of CAN API V3.
 *
 *  CAN API V3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CAN API V3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with CAN API V3.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @addtogroup  can_api
 *  @{
 */
#ifndef CANAPI_KVASERCAN_H_INCLUDED
#define CANAPI_KVASERCAN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*  -----------  includes  ------------------------------------------------
 */

#include <stdint.h>                     /* C99 header for sized integer types */
#include <stdbool.h>                    /* C99 header for boolean type */


/*  -----------  options  ------------------------------------------------
 */

#define OPTION_KVASER_CiA_BIT_TIMING    /* CiA bit-timing (from CANopen spec.) */


/*  -----------  defines  ------------------------------------------------
 */

/** @name  CAN API Interfaces
 *  @brief Kvaser CAN channel no.
 *  @{ */
#define KVASER_CAN_CHANNEL0        0    /**< Kvaser CAN Interface, Channel 0 */
#define KVASER_CAN_CHANNEL1        1    /**< Kvaser CAN Interface, Channel 1 */
#define KVASER_CAN_CHANNEL2        2    /**< Kvaser CAN Interface, Channel 2 */
#define KVASER_CAN_CHANNEL3        3    /**< Kvaser CAN Interface, Channel 3 */
#define KVASER_CAN_CHANNEL4        4    /**< Kvaser CAN Interface, Channel 4 */
#define KVASER_CAN_CHANNEL5        5    /**< Kvaser CAN Interface, Channel 5 */
#define KVASER_CAN_CHANNEL6        6    /**< Kvaser CAN Interface, Channel 6 */
#define KVASER_CAN_CHANNEL7        7    /**< Kvaser CAN Interface, Channel 7 */
#define KVASER_BOARDS              8    /**< number of Kvaser Interface boards */
/** @} */

/** @name  CAN API Error Codes
 *  @brief CANlib-specific error code
 *  @{ */
#define KVASER_ERR_PARAM       (-601)   /**< Error in one or more parameters */
#define KVASER_ERR_NOMSG       (-602)   /**< There were no messages to read */
#define KVASER_ERR_NOTFOUND    (-603)   /**< Specified device or channel not found */
#define KVASER_ERR_NOMEM       (-604)   /**< Out of memory */
#define KVASER_ERR_NOCHANNELS  (-605)   /**< No channels available */
#define KVASER_ERR_INTERRUPTED (-606)   /**<  Interrupted by signals */
#define KVASER_ERR_TIMEOUT     (-607)   /**< Timeout occurred */
#define KVASER_ERR_NOTINITIALIZED (-608)/**< The library is not initialized */
#define KVASER_ERR_NOHANDLES   (-609)   /**< Out of handles */
#define KVASER_ERR_INVHANDLE   (-610)   /**< Handle is invalid */
#define KVASER_ERR_INIFILE     (-611)   /**< Error in the ini-file (16-bit only) */
#define KVASER_ERR_DRIVER      (-612)   /**< Driver type not supported */
#define KVASER_ERR_TXBUFOFL    (-613)   /**< Transmit buffer overflow */
#define KVASER_ERR_HARDWARE    (-615)   /**< A hardware error has occurred */
#define KVASER_ERR_DYNALOAD    (-616)   /**< A driver DLL can't be found or loaded */
#define KVASER_ERR_DYNALIB     (-617)   /**< A DLL seems to have wrong version */
#define KVASER_ERR_DYNAINIT    (-618)   /**< Error when initializing a DLL */
#define KVASER_ERR_NOT_SUPPORTED (-619) /**< Operation not supported by hardware or firmware */
#define KVASER_ERR_DRIVERLOAD  (-623)   /**< Can't find or load kernel driver */
#define KVASER_ERR_DRIVERFAILED (-624)  /**< DeviceIOControl failed */
#define KVASER_ERR_NOCONFIGMGR (-625)   /**< Can't find req'd config s/w (e.g. CS/SS) */
#define KVASER_ERR_NOCARD      (-626)   /**< The card was removed or not inserted */
#define KVASER_ERR_REGISTRY    (-628)   /**< Error (missing data) in the Registry */
#define KVASER_ERR_LICENSE     (-629)   /**< The license is not valid. */
#define KVASER_ERR_INTERNAL    (-630)   /**< Internal error in the driver */
#define KVASER_ERR_NO_ACCESS   (-631)   /**< Access denied */
#define KVASER_ERR_NOT_IMPLEMENTED (-632)/**< Not implemented */
#define KVASER_ERR_DEVICE_FILE (-633)   /**< Device File error */
#define KVASER_ERR_HOST_FILE   (-634)   /**< Host File error */
#define KVASER_ERR_DISK        (-635)   /**< Disk error */
#define KVASER_ERR_CRC         (-636)   /**< CRC error*/
#define KVASER_ERR_CONFIG      (-637)   /**< Configuration Error */
#define KVASER_ERR_MEMO_FAIL   (-638)   /**< Memo Error */
#define KVASER_ERR_SCRIPT_FAIL (-639)   /**< Script Fail */
#define KVASER_ERR_SCRIPT_WRONG_VERSION (-640) /**< The t script version dosen't match the version(s) that the device firmware supports */
#define KVASER_ERR_SCRIPT_TXE_CONTAINER_VERSION (-641)/**< The compiled t script container file format is of a version which is not supported by this version of canlib */
#define KVASER_ERR_SCRIPT_TXE_CONTAINER_FORMAT (-642) /**< An error occured while trying to parse the compiled t script file */
#define KVASER_ERR_BUFFER_TOO_SMALL (-643) /**< The buffer provided was not large enough to contain the requested data */
#define KVASER_ERR_IO_WRONG_PIN_TYPE (-644) /**< The I/O pin doesn't exist or the I/O pin type doesn't match the called function, e.g. trying to use input pins as outputs or use digital pins as analog pins*/
#define KVASER_ERR_IO_NOT_CONFIRMED (-645) /**< The I/O pin configuration is not confirmed. Use kvIoConfirmConfig() to confirm the configuration*/
#define KVASER_ERR_IO_CONFIG_CHANGED (-646) /**< The I/O pin configuration has changed after last call to kvIoConfirmConfig. Use kvIoConfirmConfig() to confirm the new configuration*/
#define KVASER_ERR_IO_PENDING  (-647) /**< The previous I/O pin value has not yet changed the output and is still pending */
#define KVASER_ERR_IO_NO_VALID_CONFIG (-648) /**< There is no valid I/O pin configuration */

#define KVASER_ERR_OFFSET      (-600)   /**< offset for CANlib-specific errors */
#define KVASER_ERR_UNKNOWN     (-699)   /**< unknown error */
/** @} */

/** @name  CAN API Property Value
 *  @brief CANlib parameter to be read or written
 *  @{ */
// TODO: define parameters
// ...
#define KVASERCAN_MAX_BUFFER_SIZE 256U  /**< max. buffer size for CAN_GetValue/CAN_SetValue */
/** @} */

/** @name  CAN API Library ID
 *  @brief Library ID and dynamic library names
 *  @{ */
#define KVASER_LIB_ID            600    /**< library ID (CAN/COP API V1 compatible) */
#if defined(_WIN32) || defined (_WIN64)
 #define KVASER_LIB_CANLIB      "canLib32.DLL"
 #define KVASER_LIB_WRAPPER     "u3cankvl.dll"
#elif defined(__APPLE__)
 #define KVASER_LIB_CANLIB      "(n/a)"
 #define KVASER_LIB_WRAPPER     "libUVCANKVL.dylib"
#else
 #error Platform not supported
#endif
/** @} */

/** @name  Miscellaneous
 *  @brief More or less useful stuff
 *  @{ */
#define KVASER_LIB_VENDOR       "Kvaser AB, Sweden"
#define KVASER_LIB_WEBSITE      "www,kvaser.com"
#define KVASER_LIB_HAZARD_NOTE  "Do not connect your CAN device to a real CAN network when using this program.\n" \
                                "This can damage your application."
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CANAPI_KVASERCAN_H_INCLUDED */
/** @}
 */
/*  ----------------------------------------------------------------------
 *  Uwe Vogt,  UV Software,  Chausseestrasse 33 A,  10115 Berlin,  Germany
 *  Tel.: +49-30-46799872,  Fax: +49-30-46799873,  Mobile: +49-170-3801903
 *  E-Mail: uwe.vogt@uv-software.de,  Homepage: http://www.uv-software.de/
 */
