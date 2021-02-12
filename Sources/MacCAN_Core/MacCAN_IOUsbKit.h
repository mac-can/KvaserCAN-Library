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
#ifndef MACCAN_IOUSBKIT_H_INCLUDED
#define MACCAN_IOUSBKIT_H_INCLUDED

#include "MacCAN_Common.h"

#ifndef CANUSB_MAX_DEVICES
#define CANUSB_MAX_DEVICES  42
#endif
#define CANUSB_INVALID_INDEX  (-1)
#define CANUSB_INVALID_HANDLE  (-1)

#define CANUSB_ANY_VENDOR_ID  0xFFFFU
#define CANUSB_ANY_PRODUCT_ID  0xFFFFU

/* USB endpoint properties */
#define USBPIPE_DIR_OUT   0U
#define USBPIPE_DIR_IN    1U
#define USBPIPE_DIR_NONE  2U
#define USBPIPE_DIR_ANY   3U

#define USBPIPE_TYPE_CTRL  0U
#define USBPIPE_TYPE_ISOC  1U
#define USBPIPE_TYPE_BULK  2U
#define USBPIPE_TYPE_INTR  3U
#define USBPIPE_TYPE_ANY 0xFFU

/* USB device request (setup packet) */
#define USBREQ_HOST_TO_DEVICE  0x00U
#define USBREQ_DEVICE_TO_HOST  0x80U

#define USBREQ_TYPE_STANDARD  0x00U
#define USBREQ_TYPE_CLASS     0x20U
#define USBREQ_TYPE_VENDOR    0x40U

#define USBREQ_RECIPIENT_DEVICE     0x00U
#define USBREQ_RECIPIENT_INTERFACE  0x01U
#define USBREQ_RECIPIENT_ENDPOINT   0x02U
#define USBREQ_RECIPIENT_OTHER      0x03U

typedef int CANUSB_Index_t;
typedef int CANUSB_Handle_t;
typedef int CANUSB_Return_t;

typedef void *CANUSB_Context_t;
typedef void (*CANUSB_Callback_t)(CANUSB_Context_t refCon, UInt8 *buffer, UInt32 nbyte);

typedef struct usb_buffer_t_ {
    UInt8 *data[2];
    UInt8 index;
    UInt32 size;
} CANUSB_Buffer_t;

typedef struct usb_pipe_t_ {
    UInt8 pipeRef;
    CANUSB_Handle_t handle;
    CANUSB_Buffer_t buffer;
    CANUSB_Callback_t callback;
    CANUSB_Context_t context;
    Boolean running;
} *CANUSB_AsyncPipe_t;

typedef struct usb_setup_packet_t_ {
    UInt8  RequestType;
    UInt8  Request;
    UInt16 Value;
    UInt16 Index;
    UInt16 Length;
} CANUSB_SetupPacket_t;

#ifdef __cplusplus
extern "C" {
#endif

extern CANUSB_Return_t CANUSB_Initialize(void);

extern CANUSB_Return_t CANUSB_Teardown(void);

extern CANUSB_Handle_t CANUSB_OpenDevice(CANUSB_Index_t index, UInt16 vendorId, UInt16 productId);

extern CANUSB_Return_t CANUSB_CloseDevice(CANUSB_Handle_t handle);

extern CANUSB_Return_t CANUSB_DeviceRequest(CANUSB_Handle_t handle, CANUSB_SetupPacket_t setupPacket, void *buffer, UInt16 size, UInt32 *transferred);

extern CANUSB_Return_t CANUSB_ReadPipe(CANUSB_Handle_t handle, UInt8 pipeRef, void *buffer, UInt32 *size, UInt16 timeout);

extern CANUSB_Return_t CANUSB_WritePipe(CANUSB_Handle_t handle, UInt8 pipeRef, const void *buffer, UInt32 size, UInt16 timeout);

extern CANUSB_AsyncPipe_t CANUSB_CreatePipeAsync(CANUSB_Handle_t handle, UInt8 pipeRef, size_t bufferSize);

extern CANUSB_Return_t CANUSB_DestroyPipeAsync(CANUSB_AsyncPipe_t asyncPipe);

extern CANUSB_Return_t CANUSB_ReadPipeAsyncStart(CANUSB_AsyncPipe_t asyncPipe, CANUSB_Callback_t callback, CANUSB_Context_t context);

extern CANUSB_Return_t CANUSB_ReadPipeAsyncAbort(CANUSB_AsyncPipe_t asyncPipe);

extern Boolean CANUSB_IsPipeAsyncRunning(CANUSB_AsyncPipe_t asyncPipe);

extern CANUSB_Index_t CANUSB_GetFirstDevice(void);

extern CANUSB_Index_t CANUSB_GetNextDevice(void);

extern Boolean CANUSB_IsDevicePresent(CANUSB_Index_t index);

extern Boolean CANUSB_IsDeviceOpened(CANUSB_Index_t index);

extern CANUSB_Return_t CANUSB_GetDeviceName(CANUSB_Index_t index, char *buffer, size_t n);

extern CANUSB_Return_t CANUSB_GetDeviceVendorId(CANUSB_Index_t index, UInt16 *value);

extern CANUSB_Return_t CANUSB_GetDeviceProductId(CANUSB_Index_t index, UInt16 *value);

extern CANUSB_Return_t CANUSB_GetDeviceReleaseNo(CANUSB_Index_t index, UInt16 *value);

extern CANUSB_Return_t CANUSB_GetDeviceNumCanChannels(CANUSB_Index_t index, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetDeviceCanChannelsOpened(CANUSB_Index_t index, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetDeviceLocation(CANUSB_Index_t index, UInt32 *value);

extern CANUSB_Return_t CANUSB_GetDeviceAddress(CANUSB_Index_t index, UInt16 *value);

extern CANUSB_Return_t CANUSB_GetInterfaceClass(CANUSB_Handle_t handle, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetInterfaceSubClass(CANUSB_Handle_t handle, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetInterfaceProtocol(CANUSB_Handle_t handle, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetInterfaceNumEndpoints(CANUSB_Handle_t handle, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetInterfaceEndpointDirection(CANUSB_Handle_t handle, UInt8 index, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetInterfaceEndpointTransferType(CANUSB_Handle_t handle, UInt8 index, UInt8 *value);

extern CANUSB_Return_t CANUSB_GetInterfaceEndpointMaxPacketSize(CANUSB_Handle_t handle, UInt8 index, UInt16 *value);

extern UInt32 CANUSB_GetVersion(void);

#ifdef __cplusplus
}
#endif
#endif /* MACCAN_IOUSBKIT_H_INCLUDED */

/* * $Id: MacCAN_IOUsbKit.h 980 2021-01-04 20:50:59Z eris $ *** (C) UV Software, Berlin ***
 */
