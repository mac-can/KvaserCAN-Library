/*
 *  MacCAN - macOS User-Space Driver for USB-to-CAN Interfaces
 *
 *  Copyright (C) 2012-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
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
#include "MacCAN_MsgQueue.h"
#include "MacCAN_Debug.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#define GET_TIME(ts)  do{ clock_gettime(CLOCK_REALTIME, &ts); } while(0)
#define ADD_TIME(ts,to)  do{ ts.tv_sec += (time_t)(to / 1000U); \
                             ts.tv_nsec += (long)(to % 1000U) * (long)1000000; \
                             if (ts.tv_nsec >= (long)1000000000) { \
                                 ts.tv_nsec %= (long)1000000000; \
                                 ts.tv_sec += (time_t)1; \
                             } } while(0)

#define ENTER_CRITICAL_SECTION(queue)  assert(0 == pthread_mutex_lock(&queue->wait.mutex))
#define LEAVE_CRITICAL_SECTION(queue)  assert(0 == pthread_mutex_unlock(&queue->wait.mutex))

#define SIGNAL_WAIT_CONDITION(queue,flg)  do{ queue->wait.flag = flg; \
                                               assert(0 == pthread_cond_signal(&queue->wait.cond)); } while(0)
#define WAIT_CONDITION_INFINITE(queue,res)  do{ queue->wait.flag = false; \
                                                res = pthread_cond_wait(&queue->wait.cond, &queue->wait.mutex); } while(0)
#define WAIT_CONDITION_TIMEOUT(queue,abstime,res)  do{ queue->wait.flag = false; \
                                                       res = pthread_cond_timedwait(&queue->wait.cond, &queue->wait.mutex, &abstime); } while(0)
 
static Boolean EnqueueElement(CANQUE_MsgQueue_t queue, const void *element);
static Boolean DequeueElement(CANQUE_MsgQueue_t queue, void *element);

CANQUE_MsgQueue_t CANQUE_Create(size_t numElem, size_t elemSize) {
    CANQUE_MsgQueue_t msgQueue = NULL;
    
    MACCAN_DEBUG_DRIVER("        - Message queue for %u elements of size %u bytes\n", numElem, elemSize);
    if ((msgQueue = (CANQUE_MsgQueue_t)malloc(sizeof(struct msg_queue_t_))) == NULL) {
        MACCAN_DEBUG_ERROR("+++ Unable to create message queue (NULL pointer)\n");
        return NULL;
    }
    bzero(msgQueue, sizeof(struct msg_queue_t_));
    if ((msgQueue->queueElem = calloc(numElem, elemSize))) {
        if ((pthread_mutex_init(&msgQueue->wait.mutex, NULL) == 0) &&
            (pthread_cond_init(&msgQueue->wait.cond, NULL)) == 0) {
            msgQueue->elemSize = (size_t)elemSize;
            msgQueue->size = (UInt32)numElem;
            msgQueue->wait.flag = false;
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
        pthread_cond_destroy(&msgQueue->wait.cond);
        pthread_mutex_destroy(&msgQueue->wait.mutex);
        if (msgQueue->queueElem)
            free(msgQueue->queueElem);
        free(msgQueue);
        retVal = CANUSB_SUCCESS;
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to destroy message queue (NULL pointer)\n");
    }
    return retVal;
}

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

CANQUE_Return_t CANQUE_Enqueue(CANQUE_MsgQueue_t msgQueue, void const *message/*, UInt16 timeout*/) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;

    if (message && msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
        if (EnqueueElement(msgQueue, message)) {
            SIGNAL_WAIT_CONDITION(msgQueue, true);
            retVal = CANUSB_SUCCESS;
        } else {
            retVal = CANUSB_ERROR_FULL;
        }
        LEAVE_CRITICAL_SECTION(msgQueue);
    } else {
        MACCAN_DEBUG_ERROR("+++ Unable to enqueue message (NULL pointer)\n");
    }
    return retVal;
}

CANQUE_Return_t CANQUE_Dequeue(CANQUE_MsgQueue_t msgQueue, void *message, UInt16 timeout) {
    CANQUE_Return_t retVal = CANUSB_ERROR_RESOURCE;
    struct timespec absTime;
    int waitCond = 0;

    GET_TIME(absTime);
    ADD_TIME(absTime, timeout);

    if (message && msgQueue) {
        ENTER_CRITICAL_SECTION(msgQueue);
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

/*  ---  FIFO  ---
 *
 *  size :  total number of elements
 *  head :  read position of the queue
 *  tail :  write position of the queue
 *  used :  number of queued elements
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
        return true;
    } else {
        queue->ovfl.counter += 1U;
        queue->ovfl.flag = true;
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

/* * $Id: MacCAN_MsgQueue.c 983 2021-02-08 09:45:23Z eris $ *** (C) UV Software, Berlin ***
 */
