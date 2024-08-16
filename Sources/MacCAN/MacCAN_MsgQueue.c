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
#include "MacCAN_MsgQueue.h"
#include "MacCAN_Debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

/*#define OPTION_MACCAN_MARK_OVERRUN  1  !* set globally: 1 = mark last message before queue overrun (requires MacCAN_Message.h) */
#if (OPTION_MACCAN_MARK_OVERRUN != 0)
#include "MacCAN_Message.h"
#endif
/*#define OPTION_MACCAN_FILE_DESCRIPTOR  0  !* set globally: 0 = wait condition, 1 = plus file descriptor from pipe for select() */
#if (OPTION_MACCAN_FILE_DESCRIPTOR != 0)
#define PIPO  0
#define PIPI  1
#endif
#define GET_TIME(ts)  do{ clock_gettime(CLOCK_REALTIME, &ts); } while(0)
#define ADD_TIME(ts,to)  do{ ts.tv_sec += (time_t)(to / 1000U); \
                             ts.tv_nsec += (long)(to % 1000U) * (long)1000000; \
                             if (ts.tv_nsec >= (long)1000000000) { \
                                 ts.tv_nsec %= (long)1000000000; \
                                 ts.tv_sec += (time_t)1; \
                             } } while(0)
#define SIGNAL_WAIT_CONDITION(queue,flg)  do{ queue->wait.flag = flg; \
                                              assert(0 == pthread_cond_signal(&queue->wait.cond)); } while(0)
#define WAIT_CONDITION_INFINITE(queue,res)  do{ queue->wait.flag = false; \
                                                res = pthread_cond_wait(&queue->wait.cond, &queue->wait.mutex); } while(0)
#define WAIT_CONDITION_TIMEOUT(queue,abstime,res)  do{ queue->wait.flag = false; \
                                                       res = pthread_cond_timedwait(&queue->wait.cond, &queue->wait.mutex, &abstime); } while(0)

#define ENTER_CRITICAL_SECTION(queue)  assert(0 == pthread_mutex_lock(&queue->wait.mutex))
#define LEAVE_CRITICAL_SECTION(queue)  assert(0 == pthread_mutex_unlock(&queue->wait.mutex))

struct msg_queue_tag {                  /* Message Queue (w/ elements of user-defined size): */
    UInt32 size;                        /* - total number of ring-buffer elements */
    UInt32 used;                        /* - number of used ring-buffer elements */
    UInt32 high;                        /* - maximum fill rate of the ring-buffer */
    UInt32 head;                        /* - read position of the ring-buffer */
    UInt32 tail;                        /* - write position of the ring-buffer */
    UInt8 *queueElem;                   /* - the ring-buffer itself */
    size_t elemSize;                    /* - size of an element */
    Boolean lock;                       /* - disable enqueueing */
    struct cond_wait_t {                /* - blocking operation: */
        Boolean mode;                   /*   - blocking mode */
        pthread_mutex_t mutex;          /*   - a Posix mutex */
        pthread_cond_t cond;            /*   - a Posix condition */
        Boolean flag;                   /*   - and a flag */
#if (OPTION_MACCAN_FILE_DESCRIPTOR != 0)
        int fildes[2];                  /*   - Ceci n'est pas une pipe! */
#endif
    } wait;
    struct overflow_t {                 /* - overflow events: */
        Boolean flag;                   /*   - to indicate an overflow */
        UInt64 counter;                 /*   - overflow counter */
    } ovfl;
};
static CANQUE_Return_t EnqueueWithBlockingRead(CANQUE_MsgQueue_t msgQueue, void const *message);
static CANQUE_Return_t DequeueWithBlockingRead(CANQUE_MsgQueue_t msgQueue, void *message, UInt16 timeout);

static CANQUE_Return_t EnqueueWithBlockingWrite(CANQUE_MsgQueue_t msgQueue, void const *message, UInt16 timeout);
static CANQUE_Return_t DequeueWithBlockingWrite(CANQUE_MsgQueue_t msgQueue, void *message);

static Boolean EnqueueElement(CANQUE_MsgQueue_t queue, const void *element);
static Boolean DequeueElement(CANQUE_MsgQueue_t queue, void *element);

CANQUE_MsgQueue_t CANQUE_Create(size_t numElem, size_t elemSize, UInt8 mode) {
    CANQUE_MsgQueue_t msgQueue = NULL;

    MACCAN_DEBUG_CORE("        - %s queue for %u elements of size %u bytes\n",
        (mode & CANQUE_BLOCKING_WRITE) ? "Transmit" : "Receive", numElem, elemSize);
    if ((msgQueue = (CANQUE_MsgQueue_t)malloc(sizeof(struct msg_queue_tag))) == NULL) {
        MACCAN_DEBUG_ERROR("+++ Unable to create message queue (NULL pointer)\n");
        return NULL;
    }
    bzero(msgQueue, sizeof(struct msg_queue_tag));
    if ((msgQueue->queueElem = (UInt8*)calloc(numElem, elemSize))) {
        /* message queue with Posix wait condition */
        if ((pthread_mutex_init(&msgQueue->wait.mutex, NULL) == 0)
        &&  (pthread_cond_init(&msgQueue->wait.cond, NULL) == 0)
#if (OPTION_MACCAN_FILE_DESCRIPTOR != 0)
            /* plus file descriptors: "Ceci n'est pas une pipe." */
        &&  (pipe(msgQueue->wait.fildes) >= 0)
#endif
        ) {
            msgQueue->elemSize = (size_t)elemSize;
            msgQueue->size = (UInt32)numElem;
            msgQueue->wait.flag = false;
            msgQueue->wait.mode = mode;
            msgQueue->lock = false;
        } else {
            MACCAN_DEBUG_ERROR("+++ Unable to create message queue (wait condition)\n");
            free(msgQueue->queueElem);
            free(msgQueue);
            msgQueue = NULL;
        }
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to create message queue (%u * %u bytes)\n", numElem, elemSize);
        free(msgQueue);
        msgQueue = NULL;
    }
    return msgQueue;
}

CANQUE_Return_t CANQUE_Destroy(CANQUE_MsgQueue_t msgQueue) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (msgQueue) {
#if (OPTION_MACCAN_FILE_DESCRIPTOR != 0)
        /* close pipe file decriptors */
        (void)close(msgQueue->wait.fildes[PIPO]);
        (void)close(msgQueue->wait.fildes[PIPI]);
#endif
        /* destroy the wait condition and the mutex */
        (void)pthread_cond_destroy(&msgQueue->wait.cond);
        (void)pthread_mutex_destroy(&msgQueue->wait.mutex);
        if (msgQueue->queueElem)
            free(msgQueue->queueElem);
        free(msgQueue);
        retVal = CANUSB_SUCCESS;
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to destroy message queue (NULL pointer)\n");
    }
    return retVal;
}

#if (OPTION_MACCAN_FILE_DESCRIPTOR != 0)
int CANQUE_FileDescriptor(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return msgQueue->wait.fildes[PIPO];
    else
        return (-1);
}
#endif

CANQUE_Return_t CANQUE_Signal(CANQUE_MsgQueue_t msgQueue) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
        SIGNAL_WAIT_CONDITION(msgQueue, false);
        LEAVE_CRITICAL_SECTION(msgQueue);
        retVal = CANUSB_SUCCESS;
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to signal message queue (NULL pointer)\n");
    }
    return retVal;
}

CANQUE_Return_t CANQUE_Enqueue(CANQUE_MsgQueue_t msgQueue, void const *message, UInt16 timeout) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    /* enqueue the message */
    if (message && msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
        if (!msgQueue->lock) {
            if ((msgQueue->wait.mode & CANQUE_BLOCKING_WRITE)) {
                /* blocking mode for the transmission queue on the client side (w/ timeout > 0) */
                retVal = EnqueueWithBlockingWrite(msgQueue, message, timeout);
            } else {
                /* non-blocking mode for the reception queue on the driver side (w/o timeout) */
                retVal = EnqueueWithBlockingRead(msgQueue, message);
            }
        } else {
            /* producer cannot enqueue new elements, while client(s) can dequeue elements (if any) */
            retVal = CANUSB_ERROR_DISABLED;
        }
        LEAVE_CRITICAL_SECTION(msgQueue);
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to enqueue message (NULL pointer)\n");
    }
    return retVal;
}

CANQUE_Return_t CANQUE_Dequeue(CANQUE_MsgQueue_t msgQueue, void *message, UInt16 timeout) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (message && msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
        if (!(msgQueue->wait.mode & CANQUE_BLOCKING_WRITE)) {
            /* blocking mode for the reception queue on the client side (w/ timeout > 0), or
               polling (w/ timeout = 0), or by select() (timeout is ignored in this case) */
            retVal = DequeueWithBlockingRead(msgQueue, message, timeout);
        } else {
            /* non-blocking mode for the transmission queue on the driver side (w/o timeout) */
            retVal = DequeueWithBlockingWrite(msgQueue, message);
        }
        LEAVE_CRITICAL_SECTION(msgQueue);
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to dequeue message (NULL pointer)\n");
    }
    return retVal;
}

CANQUE_Return_t CANQUE_Reset(CANQUE_MsgQueue_t msgQueue) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
        msgQueue->used = 0U;
        msgQueue->head = 0U;
        msgQueue->tail = 0U;
        msgQueue->high = 0U;
        msgQueue->wait.flag = false;
        msgQueue->ovfl.flag = false;
        msgQueue->ovfl.counter = 0U;
        LEAVE_CRITICAL_SECTION(msgQueue);
        retVal = CANUSB_SUCCESS;
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to reset message queue (NULL pointer)\n");
    }
    return retVal;
}

CANQUE_Return_t CANQUE_Disable(CANQUE_MsgQueue_t msgQueue) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
        msgQueue->lock = true;
        LEAVE_CRITICAL_SECTION(msgQueue);
        retVal = CANUSB_SUCCESS;
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to disable message queue (NULL pointer)\n");
    }
    return retVal;
}

CANQUE_Return_t CANQUE_Enable(CANQUE_MsgQueue_t msgQueue) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
        msgQueue->lock = false;
        LEAVE_CRITICAL_SECTION(msgQueue);
        retVal = CANUSB_SUCCESS;
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to disable message queue (NULL pointer)\n");
    }
    return retVal;
}

Boolean CANQUE_IsEnabled(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return msgQueue->lock ? false : true;
    else
        return false;
}

Boolean CANQUE_IsEmpty(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return (msgQueue->used == 0U) ? true : false;
    else
        return false;
}

Boolean CANQUE_IsFull(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return (msgQueue->used >= msgQueue->size) ? true : false;
    else
        return true;
}

Boolean CANQUE_OverflowFlag(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return msgQueue->ovfl.flag;
    else
        return false;
}

UInt64 CANQUE_OverflowCounter(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return msgQueue->ovfl.counter;
    else
        return 0U;
}

UInt32 CANQUE_QueueCount(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return msgQueue->used;
    else
        return 0U;
}

UInt32 CANQUE_QueueSize(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return msgQueue->size;
    else
        return 0U;;
}

UInt32 CANQUE_QueueHigh(CANQUE_MsgQueue_t msgQueue) {
    if (msgQueue)
        return msgQueue->high;
    else
        return 0U;;
}

/*  ---  Blocking Read  ---
 */
static CANQUE_Return_t EnqueueWithBlockingRead(CANQUE_MsgQueue_t msgQueue, void const *message) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    assert(message);
    assert(msgQueue);

    /* enqueue the element, if queue not full */
    if (EnqueueElement(msgQueue, message)) {
#if (OPTION_MACCAN_FILE_DESCRIPTOR == 0)
        /* signal the wait condition */
        SIGNAL_WAIT_CONDITION(msgQueue, true);
#else
        /* dummy write into pipe */
        if (!msgQueue->wait.flag) {
            (void)write(msgQueue->wait.fildes[PIPI], (void*)&msgQueue->wait.flag, sizeof(msgQueue->wait.flag));
            msgQueue->wait.flag = true;
        }
#endif
        retVal = CANUSB_SUCCESS;
    } else {
        msgQueue->ovfl.counter += 1U;
        msgQueue->ovfl.flag = true;
        retVal = CANUSB_ERROR_FULL;
    }
    return retVal;
}

static CANQUE_Return_t DequeueWithBlockingRead(CANQUE_MsgQueue_t msgQueue, void *message, UInt16 timeout) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;
    
    assert(message);
    assert(msgQueue);

#if (OPTION_MACCAN_FILE_DESCRIPTOR == 0)
    struct timespec absTime;
    int waitCond = 0;
    GET_TIME(absTime);
    ADD_TIME(absTime, timeout);

    /* dequeue one element (with wait condition) */
dequeue:
    if (DequeueElement(msgQueue, message)) {
        retVal = CANUSB_SUCCESS;
    } else {
        if (timeout == CANUSB_INFINITE) {  /* blocking read */
            WAIT_CONDITION_INFINITE(msgQueue, waitCond);
            if ((waitCond == 0) && msgQueue->wait.flag)
                goto dequeue;
        } else if (timeout != 0U) {  /* timed blocking read */
            WAIT_CONDITION_TIMEOUT(msgQueue, absTime, waitCond);
            if ((waitCond == 0) && msgQueue->wait.flag)
                goto dequeue;
        }
        retVal = CANUSB_ERROR_EMPTY;
    }
#else
    (void)timeout;

    /* dequeue one element (with file descriptor) */
    if (DequeueElement(msgQueue, message)) {
        retVal = CANUSB_SUCCESS;
    } else {
        retVal = CANUSB_ERROR_EMPTY;
    }
    /* dummy read from pipe */
    if (msgQueue->wait.flag) {
        (void)read(msgQueue->wait.fildes[PIPO], (void*)&msgQueue->wait.flag, sizeof(msgQueue->wait.flag));
        msgQueue->wait.flag = false;
    }
#endif
    return retVal;
}

/*  ---  Blocking Write  ---
 */
static CANQUE_Return_t EnqueueWithBlockingWrite(CANQUE_MsgQueue_t msgQueue, void const *message, UInt16 timeout) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    assert(message);
    assert(msgQueue);

    struct timespec absTime;
    int waitCond = 0;
    GET_TIME(absTime);
    ADD_TIME(absTime, timeout);

    /* enqueue the element (with wait condition) */
enqueue:
    if (EnqueueElement(msgQueue, message)) {
        retVal = CANUSB_SUCCESS;
    } else {
        if (timeout == CANUSB_INFINITE) {  /* blocking read */
            WAIT_CONDITION_INFINITE(msgQueue, waitCond);
            if ((waitCond == 0) && msgQueue->wait.flag)
                goto enqueue;
        } else if (timeout != 0U) {  /* timed blocking read */
            WAIT_CONDITION_TIMEOUT(msgQueue, absTime, waitCond);
            if ((waitCond == 0) && msgQueue->wait.flag)
                goto enqueue;
        }
        msgQueue->ovfl.counter += 1U;
        msgQueue->ovfl.flag = true;
        retVal = CANUSB_ERROR_FULL;
    }
    return retVal;
}

static CANQUE_Return_t DequeueWithBlockingWrite(CANQUE_MsgQueue_t msgQueue, void *message) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    assert(message);
    assert(msgQueue);

    /* dequeue one element, if queue not empty */
    if (DequeueElement(msgQueue, message)) {
        /* signal the wait condition */
        SIGNAL_WAIT_CONDITION(msgQueue, true);
        retVal = CANUSB_SUCCESS;
    } else {
        retVal = CANUSB_ERROR_EMPTY;
    }
    return retVal;
}

/*  ---  FIFO  ---
 *
 *  size :  total number of elements
 *  head :  read position of the queue
 *  tail :  write position of the queue
 *  used :  actual number of queued elements
 *  high :  highest number of queued elements
 *
 *  (§1) empty :  used == 0
 *  (§2) full  :  used == size  &&  size > 0
 */
static Boolean EnqueueElement(CANQUE_MsgQueue_t queue, const void *element) {
    assert(queue);
    assert(element);
    assert(queue->size);
    assert(queue->queueElem);

    if (queue->used < queue->size) {
        if (queue->used != 0U)
            queue->tail = (queue->tail + 1U) % queue->size;
        else
            queue->head = queue->tail;  /* to make sure */
        (void)memcpy(&queue->queueElem[(queue->tail * queue->elemSize)], element, queue->elemSize);
        queue->used += 1U;
        if (queue->high < queue->used)
            queue->high = queue->used;
        return true;
    } else {
#if (OPTION_MACCAN_MARK_OVERRUN != 0)
        /* mark the last message before queue overrun (fragile, requires MacCAN_Types.h) */
        ((CANMSG_CanMessage_t*)&queue->queueElem[(queue->tail * queue->elemSize)])->extra |= CANMSG_FLAG_OVERRUN;
#endif
        return false;
    }
}

static Boolean DequeueElement(CANQUE_MsgQueue_t queue, void *element) {
    assert(queue);
    assert(element);
    assert(queue->size);
    assert(queue->queueElem);

    if (queue->used > 0U) {
        (void)memcpy(element, &queue->queueElem[(queue->head * queue->elemSize)], queue->elemSize);
        queue->head = (queue->head + 1U) % queue->size;
        queue->used -= 1U;
        return true;
    } else
        return false;
}

/* * $Id: MacCAN_MsgQueue.c 2024 2024-08-15 15:02:33Z makemake $ *** (c) UV Software, Berlin ***
 */
