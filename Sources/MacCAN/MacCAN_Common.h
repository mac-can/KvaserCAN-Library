/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  MacCAN - macOS User-Space Driver for USB-to-CAN Interfaces
 *
 *  Copyright (c) 2012-2021 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *  All rights reserved.
 *
 *  This file is part of MacCAN-Core.
 *
 *  MacCAN-Core is dual-licensed under the BSD 2-Clause "Simplified" License and
 *  under the GNU General Public License v3.0 (or any later version).
 *  You can choose between one of them if you use this file.
 *
 *  BSD 2-Clause "Simplified" License:
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  MacCAN-Core IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF MacCAN-Core, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  GNU General Public License v3.0 or later:
 *  MacCAN-Core is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MacCAN-Core is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MacCAN-Core.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MACCAN_COMMAN_H_INCLUDED
#define MACCAN_COMMAN_H_INCLUDED

#include <MacTypes.h>

/* CAN API V3 compatible error codes */
#define CANUSB_ERROR_FATAL     (-99)
#define CANUSB_ERROR_NOTSUPP   (-98)
#define CANUSB_ERROR_LIBRARY   (-97)
#define CANUSB_ERROR_YETINIT   (-96)
#define CANUSB_ERROR_NOTINIT   (-95)
#define CANUSB_ERROR_NULLPTR   (-94)
#define CANUSB_ERROR_ILLPARA   (-93)
#define CANUSB_ERROR_HANDLE    (-92)
#define CANUSB_ERROR_reserved  (-91)
#define CANUSB_ERROR_RESOURCE  (-90)
#define CANUSB_ERROR_TIMEOUT   (-50)
#define CANUSB_ERROR_EMPTY     (-30)
#define CANUSB_ERROR_FULL      (-20)
#define CANUSB_ERROR_OK          (0)
#define CANUSB_SUCCESS  CANUSB_ERROR_OK

/* CAN API V3 compatible board states */
#define CANUSB_BOARD_NOT_AVAILABLE  (-1)
#define CANUSB_BOARD_AVAILABLE       (0)
#define CANUSB_BOARD_OCCUPIED       (+1)
#define CANUSB_BOARD_NOT_TESTABLE   (-2)

/* CAN API V3 compatible time-out value */
#define CANUSB_INFINITE  (65535U)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif /* MACCAN_COMMAN_H_INCLUDED */

/* * $Id: MacCAN_Common.h 1068 2021-12-30 18:14:57Z makemake $ *** (c) UV Software, Berlin ***
 */
