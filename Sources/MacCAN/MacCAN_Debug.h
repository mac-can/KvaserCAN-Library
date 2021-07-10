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
#ifndef MACCAN_DEBUG_H_INCLUDED
#define MACCAN_DEBUG_H_INCLUDED

#include <stdio.h>

/* Debug level 1:
 * - output error messages on stderr
 */
#if (OPTION_MACCAN_DEBUG_LEVEL > 0)
    #define MACCAN_DEBUG_ERROR(...)  can_dbg_printf(stderr, __VA_ARGS__)
#else
    #define MACCAN_DEBUG_ERROR(...)  while(0)
#endif

/* Debug level 2:
 * - output general information on stdout and
 * - output error messages on stderr
 */
#if (OPTION_MACCAN_DEBUG_LEVEL > 1)
    #define MACCAN_DEBUG_INFO(...)  can_dbg_printf(stdout, __VA_ARGS__)
#else
    #define MACCAN_DEBUG_INFO(...)  while(0)
#endif

/* Debug level 3:
 * - output general information on stdout and
 * - output driver related information on stdout and
 * - output error messages on stderr
 */
#if (OPTION_MACCAN_DEBUG_LEVEL > 2)
    #define MACCAN_DEBUG_DRIVER(...)  can_dbg_printf(stdout, __VA_ARGS__)
#else
    #define MACCAN_DEBUG_DRIVER(...)  while(0)
#endif

/* Debug level 4:
 * - output general information on stdout and
 * - output driver related information on stdout and
 * - output IOUsbKit related information on stdout and
 * - output error messages on stderr
 */
#if (OPTION_MACCAN_DEBUG_LEVEL > 3)
    #define MACCAN_DEBUG_CORE(...)  can_dbg_printf(stdout, __VA_ARGS__)
#else
    #define MACCAN_DEBUG_CORE(...)  while(0)
#endif

/* Instrumentation level 1:
 * - output function name on stdout
 */
#if (OPTION_MACCAN_INSTRUMENTATION > 0)
    #define MACCAN_DEBUG_FUNC(...)  can_dbg_func_printf(stdout, __FUNCTION__, __VA_ARGS__)
#else
    #define MACCAN_DEBUG_FUNC(...)  while(0)
#endif

/* Instrumentation level 2:
 * - output function name on stdout
 * - output nested code information on stdout
 */
#if (OPTION_MACCAN_INSTRUMENTATION > 1)
    #define MACCAN_DEBUG_CODE(level,...)  can_dbg_code_printf(stdout, __LINE__, level, __VA_ARGS__)
#else
    #define MACCAN_DEBUG_CODE(level,...)  while(0)
#endif

/* Write log message into a file
 */
    #ifndef MACCAN_LOG_FILE
    #define MACCAN_LOG_FILE  "mac-can.log"
    #endif
    #ifndef MACCAN_LOG_MODE
    #define MACCAN_LOG_MODE  "w"  /* "w" or "a" */
    #endif
#if (OPTION_MACCAN_LOGGER > 0)
    #define MACCAN_LOG_OPEN()  can_log_open(NULL)
    #define MACCAN_LOG_CLOSE()  can_log_close()
    #define MACCAN_LOG_PRINTF(...)  can_log_printf(__VA_ARGS__)
    #define MACCAN_LOG_WRITE(buf,len,pre)  can_log_write(buf, len, pre)
#else
    #define MACCAN_LOG_OPEN()  while(0)
    #define MACCAN_LOG_CLOSE()  while(0)
    #define MACCAN_LOG_PRINTF(...)  while(0)
    #define MACCAN_LOG_WRITE(buf,len,pre)  while(0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int can_dbg_printf(FILE *file, const char *format,...);
extern int can_dbg_func_printf(FILE *file, const char *name, const char *format,...);
extern int can_dbg_code_printf(FILE *file, int line, int level, const char *format,...);

extern int can_log_open(const char *filename);
extern int can_log_close(void);
extern int can_log_printf(const char *format,...);
extern int can_log_write(unsigned char *buffer, size_t nbyte, const char *prefix);

#ifdef __cplusplus
}
#endif
#endif /* MACCAN_DEBUG_H_INCLUDED */

/* * $Id: MacCAN_Debug.h 1001 2021-05-25 17:57:49Z eris $ *** (c) UV Software, Berlin ***
 */
