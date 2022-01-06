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
#ifndef MACCAN_MSGQUEUE_H_INCLUDED
#define MACCAN_MSGQUEUE_H_INCLUDED

#include "MacCAN_Common.h"

/* -- U Can't Touch This! -- */
typedef struct msg_queue_t_ {
    UInt32 size;
    UInt32 used;
    UInt32 high;
    UInt32 head;
    UInt32 tail;
    UInt8 *queueElem;
    size_t elemSize;
    struct cond_wait_t {
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        Boolean flag;
    } wait;
    struct overflow_t {
        Boolean flag;
        UInt64 counter;
    } ovfl;
} *CANQUE_MsgQueue_t;

typedef int CANQUE_Return_t;

#ifdef __cplusplus
extern "C" {
#endif

extern CANQUE_MsgQueue_t CANQUE_Create(size_t numElem, size_t elemSize);

extern CANQUE_Return_t CANQUE_Destroy(CANQUE_MsgQueue_t msgQueue);

extern CANQUE_Return_t CANQUE_Signal(CANQUE_MsgQueue_t msgQueue);

extern CANQUE_Return_t CANQUE_Enqueue(CANQUE_MsgQueue_t msgQueue, void const *message/*, UInt16 timeout*/);

extern CANQUE_Return_t CANQUE_Dequeue(CANQUE_MsgQueue_t msgQueue, void *message, UInt16 timeout);

extern CANQUE_Return_t CANQUE_Reset(CANQUE_MsgQueue_t msgQueue);

extern Boolean CANQUE_OverflowFlag(CANQUE_MsgQueue_t msgQueue);

extern UInt64 CANQUE_OverflowCounter(CANQUE_MsgQueue_t msgQueue);

extern UInt32 CANQUE_QueueSize(CANQUE_MsgQueue_t msgQueue);

extern UInt32 CANQUE_QueueHigh(CANQUE_MsgQueue_t msgQueue);

#ifdef __cplusplus
}
#endif
#endif /* MACCAN_MSGQUEUE_H_INCLUDED */

/* * $Id: MacCAN_MsgQueue.h 1076 2022-01-06 08:02:11Z makemake $ *** (c) UV Software, Berlin ***
 */
