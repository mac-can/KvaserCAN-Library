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
#ifndef MACCAN_MSGQUEUE_H_INCLUDED
#define MACCAN_MSGQUEUE_H_INCLUDED

#include "MacCAN_Common.h"

typedef struct msg_queue_t_ {
    UInt32 size;
    UInt32 used;
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

#ifdef __cplusplus
}
#endif
#endif /* MACCAN_MSGQUEUE_H_INCLUDED */

/* * $Id: MacCAN_MsgQueue.h 979 2021-01-04 20:16:55Z eris $ *** (C) UV Software, Berlin ***
 */
