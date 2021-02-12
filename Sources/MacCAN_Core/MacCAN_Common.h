/*
 *  MacCAN - macOS User-Space Driver for USB-to-CAN Interfaces
 *
 *  Copyright (C) 2012-2020  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
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
#ifndef MACCAN_COMMAN_H_INCLUDED
#define MACCAN_COMMAN_H_INCLUDED

#include <MacTypes.h>

/* CAN API V3 compatible error codes */
#define CANUSB_ERROR_FATAL    (-99)
#define CANUSB_ERROR_NOTSUPP  (-98)
#define CANUSB_ERROR_LIBRARY  (-97)
#define CANUSB_ERROR_YETINIT  (-96)
#define CANUSB_ERROR_NOTINIT  (-95)
#define CANUSB_ERROR_NULLPTR  (-94)
#define CANUSB_ERROR_ILLPARA  (-93)
#define CANUSB_ERROR_HANDLE   (-92)
#define CANUSB_ERROR_reserved (-91)
#define CANUSB_ERROR_RESOURCE (-90)
#define CANUSB_ERROR_TIMEOUT  (-50)
#define CANUSB_ERROR_EMPTY    (-30)
#define CANUSB_ERROR_FULL     (-20)
#define CANUSB_ERROR_OK         (0)
#define CANUSB_SUCCESS  CANUSB_ERROR_OK

/* CAN API V3 compatible time-out value */
#define CANUSB_INFINITE  (65535U)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif /* MACCAN_COMMAN_H_INCLUDED */

/* * $Id: MacCAN_Common.h 969 2020-12-27 15:56:48Z eris $ *** (C) UV Software, Berlin ***
 */

