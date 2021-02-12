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
        fp = fopen(filename, "w");
    else
        fp = fopen(MACCAN_LOG_FILE, "w");
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
        if (fprintf(fp, "%02X%c", buffer[i], (i+1) < nbyte ? ' ' : '\n') < 3) {
            i = (-1);
            break;
        }
    }
    (void)pthread_mutex_unlock(&mt);
#else
    if (buffer) { i = (-1); } /* to avoid compiler warnings */
    if (nbyte) { i = (-1); } /* to avoid compiler warnings */
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

/* * $Id: MacCAN_Debug.c 969 2020-12-27 15:56:48Z eris $ *** (C) UV Software, Berlin ***
 */
