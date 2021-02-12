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
#ifndef MACCAN_MSGPIPE_H_INCLUDED
#define MACCAN_MSGPIPE_H_INCLUDED

#include "MacCAN_Common.h"

typedef struct msg_pipe_t_ {
    int fildes[2];
} *CANPIP_MsgPipe_t;

typedef int CANPIP_Return_t;

#ifdef __cplusplus
extern "C" {
#endif

extern CANPIP_MsgPipe_t CANPIP_Create(void);

extern CANPIP_Return_t CANPIP_Destroy(CANPIP_MsgPipe_t msgPipe);

extern CANPIP_Return_t CANPIP_Write(CANPIP_MsgPipe_t msgPipe, void const *buffer, size_t nbyte);

extern CANPIP_Return_t CANPIP_Read(CANPIP_MsgPipe_t msgPipe, void *buffer, size_t maxbyte, UInt16 timeout);

#ifdef __cplusplus
}
#endif
#endif /* MACCAN_MSGPIPE_H_INCLUDED */

/* * $Id: MacCAN_MsgPipe.h 971 2020-12-27 16:02:24Z eris $ *** (C) UV Software, Berlin ***
 */
