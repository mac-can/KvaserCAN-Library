/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  MacCAN - macOS User-Space Driver for USB-to-CAN Interfaces
 *
 *  Copyright (c) 2012-2024 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *  All rights reserved.
 *
 *  This file is part of MacCAN-Core.
 *
 *  MacCAN-Core is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v3.0 (or any later version).
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
 *  along with MacCAN-Core.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "MacCAN_Internal.h"
#include "MacCAN_Version.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sched.h>

#include <errno.h>
#include <assert.h>

#define POSIX_DEPRECATED 0  /* set to non-zero value to use deprecated 'usleep' */
#if (POSIX_DEPRECATED != 0)
#warning Compilation with deprecated function 'usleep'
#endif

UInt8 CANUSB_GetCoreMajor(void) {
    return (UInt8)MACCAN_CORE_MAJOR;
}

UInt8 CANUSB_GetCoreMinor(void) {
    return (UInt8)MACCAN_CORE_MINOR;
}

UInt8 CANUSB_GetCorePatch(void) {
    return (UInt8)MACCAN_CORE_PATCH;
}

int CANUSB_GetCoreRevNo(void) {
    int rev = 0;
    /* get SVN/RCS revision number from expanded keyword */
    if (sscanf(MACCAN_CORE_REV, "\044Rev: %i\044", &rev) != 1)
        rev = 0;
    return rev;
}

UInt32 CANUSB_GetVersion(void) {
    return ((UInt32)MACCAN_CORE_MAJOR << 24) |
           ((UInt32)MACCAN_CORE_MINOR << 16) |
           ((UInt32)MACCAN_CORE_PATCH << 8);
}

UInt32 CANUSB_GetRevision(void) {
    return (UInt32)CANUSB_GetCoreRevNo();
}

UInt8 CANUSB_Dlc2Len(UInt8 dlc) {
    const static UInt8 dlc_table[16] = {
#if (OPTION_CAN_2_0_ONLY == 0)
        0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U
    };
    return dlc_table[(dlc < 16U) ? dlc : 15U];
#else
    return (dlc < 8U) ? dlc : 8U;
#endif
}

UInt8 CANUSB_Len2Dlc(UInt8 len) {
#if (OPTION_CAN_2_0_ONLY == 0)
    if(len > 48U) return 0x0FU;
    if(len > 32U) return 0x0EU;
    if(len > 24U) return 0x0DU;
    if(len > 20U) return 0x0CU;
    if(len > 16U) return 0x0BU;
    if(len > 12U) return 0x0AU;
    if(len > 8U) return 0x09U;
#else
    if(len > 8U) return 0x08U;
#endif
    return len;
}

Boolean CANUSB_Sleep(UInt32 microseconds) {
#if (POSIX_DEPRECATED != 0)
    if (microseconds)
        return (usleep((useconds_t)microseconds) != 0) ? false : true;
    else
        return (sched_yield() != 0) ? false : true;
#else
    int rc;
    struct timespec delay;
    if (microseconds) {
        delay.tv_sec = (time_t)(microseconds / (UInt32)1000000);
        delay.tv_nsec = (long)((microseconds % (UInt32)1000000) * (UInt32)1000);
        errno = 0;
        while ((rc = nanosleep(&delay, &delay)) != 0) {
            if (errno != EINTR)
                break;
        }
    } else {
        rc = sched_yield();
    }
    return (rc != 0) ? false : true;
#endif
}

/* * $Id: MacCAN_Internal.c 2028 2024-08-16 08:24:21Z makemake $ *** (c) UV Software, Berlin ***
 */
