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
#include "MacCAN_MsgPipe.h"
#include "MacCAN_Debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/select.h>

#define PIPI  0
#define PIPO  1

CANPIP_MsgPipe_t CANPIP_Create(void) {
    CANPIP_MsgPipe_t msgPipe = NULL;
    
    MACCAN_DEBUG_DRIVER("        - Message pipe of size %u bytes\n", PIPE_BUF);
    if ((msgPipe = (CANPIP_MsgPipe_t)malloc(sizeof(struct msg_pipe_t_))) == NULL) {
        MACCAN_DEBUG_ERROR("+++ Unable to create message pipe (NULL pointer)\n");
        return NULL;
    }
    if (pipe(msgPipe->fildes) < 0) {
        MACCAN_DEBUG_ERROR("+++ Unable to open message pipe (errno=%i)\n", errno);
        free(msgPipe);
        return NULL;
    }
    if (fcntl(msgPipe->fildes[PIPI], F_SETFL, O_NONBLOCK) < 0) {
        MACCAN_DEBUG_ERROR("+++ Unable to open message pipe (errno=%i)\n", errno);
        (void)close(msgPipe->fildes[PIPO]);
        (void)close(msgPipe->fildes[PIPI]);
        free(msgPipe);
        return NULL;
    }
    return msgPipe;
}

CANPIP_Return_t CANPIP_Destroy(CANPIP_MsgPipe_t msgPipe) {
    CANPIP_Return_t retVal = CANUSB_ERROR_RESOURCE;
    
    if (msgPipe) {
        (void)close(msgPipe->fildes[PIPO]);
        (void)close(msgPipe->fildes[PIPI]);
        free(msgPipe);
        retVal = CANUSB_SUCCESS;
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to destroy message pipe (NULL pointer)\n");
    }
    return retVal;
}

CANPIP_Return_t CANPIP_Write(CANPIP_MsgPipe_t msgPipe, void const *buffer, size_t nbyte) {
    CANPIP_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (buffer && msgPipe) {
        ssize_t n = write(msgPipe->fildes[PIPO], buffer, nbyte);
        if (n < 0)
            retVal = CANUSB_ERROR_FATAL;
        else if (n < (ssize_t)nbyte)
            retVal = CANUSB_ERROR_FULL;
        else
            retVal = CANUSB_SUCCESS;
    }
    return retVal;
}

CANPIP_Return_t CANPIP_Read(CANPIP_MsgPipe_t msgPipe, void *buffer, size_t maxbyte, UInt16 timeout) {
    CANPIP_Return_t retVal = CANUSB_ERROR_RESOURCE;
    size_t nbyte = 0U;

    if (buffer && msgPipe) {
        fd_set rdfs;
        FD_ZERO(&rdfs);
        FD_SET(msgPipe->fildes[PIPI], &rdfs);
        
        struct timeval timeVal;
        timeVal.tv_sec = timeout / 1000;
        timeVal.tv_usec = (timeout % 1000U) * 1000U;

        for (;;) {
            ssize_t n = read(msgPipe->fildes[PIPI], &buffer[nbyte], (maxbyte - nbyte));
            if (n < 0) {
                if (errno != EAGAIN) {
                    MACCAN_DEBUG_ERROR("+++ Unable to read message pipe (errno=%i)\n", errno);
                    retVal = CANUSB_ERROR_FATAL;
                    break;
                }
            } else
                nbyte += (size_t)n;
            if (nbyte < maxbyte) {
                if (timeout == CANUSB_INFINITE) {  /* blocking read */
                    if (select(msgPipe->fildes[PIPI]+1, &rdfs, NULL, NULL, NULL) < 0) {
                        MACCAN_DEBUG_ERROR("+++ Unable to wait on blocking message pipe (errno=%i)\n", errno);
                        retVal = CANUSB_ERROR_FATAL;
                        break;
                    }
                } else {  /* timed blocking read or polling */
                    if (select(msgPipe->fildes[PIPI]+1, &rdfs, NULL, NULL, &timeVal) < 0) {
                        MACCAN_DEBUG_ERROR("+++ Unable to wait on timed message pipe (errno=%i)\n", errno);
                        retVal = CANUSB_ERROR_FATAL;
                        break;
                    }
                    if (!FD_ISSET(msgPipe->fildes[PIPI], &rdfs)) {
                        retVal = CANUSB_ERROR_TIMEOUT;
                        break;
                    }
                }
            } else {
                retVal = CANUSB_SUCCESS;
                break;
            }
        }
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to read packet (NULL pointer)\n");
    }
    return retVal;
}

/* * $Id: MacCAN_MsgPipe.c 971 2020-12-27 16:02:24Z eris $ *** (C) UV Software, Berlin ***
 */
