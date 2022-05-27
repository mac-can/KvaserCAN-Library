/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  MacCAN - macOS User-Space Driver for USB-to-CAN Interfaces
 *
 *  Copyright (c) 2012-2022 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
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
#include "MacCAN_Debug.h"

#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <errno.h>

int can_dbg_printf(FILE *file, const char *format,...) {
    int rc = (-1);
#if (OPTION_MACCAN_DEBUG_LEVEL > 0)
    va_list args;
    va_start(args, format);
    rc = vfprintf(file, format, args);
    va_end(args);
    fflush(file);
#else
    if (format) { rc = (-1); } /* to avoid compiler warnings */
    if (file) { rc = (-1); } /* to avoid compiler warnings */
#endif
    return rc;
}

int can_dbg_func_printf(FILE *file, const char *name, const char *format,...) {
    int rc = (-1);
#if (OPTION_MACCAN_INSTRUMENTATION > 0)
    fprintf(file, "%s: ", name);
    va_list args;
    va_start(args, format);
    rc = vfprintf(file, format, args);
    va_end(args);
    fflush(file);
#else
    if (format) { rc = (-1); } /* to avoid compiler warnings */
    if (name) { rc = (-1); } /* to avoid compiler warnings */
    if (file) { rc = (-1); } /* to avoid compiler warnings */
#endif
    return rc;
}

int can_dbg_code_printf(FILE *file, int line, int level, const char *format,...) {
    int rc = (-1);
#if (OPTION_MACCAN_INSTRUMENTATION > 1)
    fprintf(file, "#%i", line);
    for (int i = 0; i <= level; i++)
        fprintf(file, ".");
    va_list args;
    va_start(args, format);
    rc = vfprintf(file, format, args);
    va_end(args);
    fflush(file);
#else
    if (format) { rc = (-1); } /* to avoid compiler warnings */
    if (level) { rc = (-1); } /* to avoid compiler warnings */
    if (line) { rc = (-1); } /* to avoid compiler warnings */
    if (file) { rc = (-1); } /* to avoid compiler warnings */
#endif
    return rc;
}

#if (OPTION_MACCAN_LOGGER > 0)
    static FILE *fp = NULL;
    static pthread_mutex_t mt;
#endif

int can_log_open(const char *filename) {
    int rc = (-1);
#if (OPTION_MACCAN_LOGGER > 0)
    if (fp)
        return rc;
    if ((rc = pthread_mutex_init(&mt, NULL)) < 0)
        return rc;
    if (filename)
        fp = fopen(filename, MACCAN_LOG_MODE);
    else
        fp = fopen(MACCAN_LOG_FILE, MACCAN_LOG_MODE);
    rc = (fp) ? 0 : (-1);
#else
    if (filename) { rc = (-1); } /* to avoid compiler warnings */
#endif
    return rc;
}

int can_log_close(void) {
    int rc = (-1);
#if (OPTION_MACCAN_LOGGER > 0)
    if (fp)
        rc = fclose(fp);
    if (rc == 0)
        fp = NULL;
    (void)pthread_mutex_destroy(&mt);
#endif
    return rc;
}

int can_log_write(unsigned char *buffer, size_t nbyte, const char *prefix) {
    int i = (-1);
#if (OPTION_MACCAN_LOGGER > 0)
    if (!fp)
        return i;  /* shoplifted */
    if ((i = pthread_mutex_lock(&mt)) < 0)
        return i;  /* shoplifted */
    if (prefix)
        fprintf(fp, "%s ", prefix);
    for (i = 0; i < (int)nbyte; i++) {
        if (fprintf(fp, "%02X%c", buffer[i], (i+1) < (int)nbyte ? ' ' : '\n') < 3) {
            i = (-1);
            break;
        }
    }
    (void)pthread_mutex_unlock(&mt);
#else
    if (buffer) { i = (-1); } /* to avoid compiler warnings */
    if (nbyte) { i = (-1); } /* to avoid compiler warnings */
    if (prefix) { i = (-1); } /* to avoid compiler warnings */
#endif
    return i;
}

int can_log_printf(const char *format,...) {
    int rc = (-1);
#if (OPTION_MACCAN_LOGGER > 0)
    if (!fp)
        return rc;
    if ((rc = pthread_mutex_lock(&mt)) < 0)
        return rc;
    va_list args;
    va_start(args, format);
    rc = vfprintf(fp, format, args);
    va_end(args);
    fflush(fp);
    (void)pthread_mutex_unlock(&mt);
#else
    if (format) { rc = (-1); } /* to avoid compiler warnings */
#endif
    return rc;
}

/* * $Id: MacCAN_Debug.c 1191 2022-05-27 09:20:04Z eris $ *** (c) UV Software, Berlin ***
 */
