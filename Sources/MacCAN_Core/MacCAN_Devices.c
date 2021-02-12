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
#include "MacCAN_Devices.h"

#include <stdio.h>

#define FINAL_VENDOR_ID  (UInt16)-1
#define FINAL_PRODUCT_ID  (UInt16)-1

extern const CANDEV_Device_t CANDEV_Devices[];

static int nIndex = 0;

const CANDEV_Device_t *CANDEV_GetFirstDevice(void)
{
    const CANDEV_Device_t *ptrDevice = NULL;
    
    nIndex = 0;
    if (CANDEV_Devices[nIndex].vendorId != FINAL_VENDOR_ID)
        ptrDevice = &CANDEV_Devices[nIndex];
    return ptrDevice;
}

const CANDEV_Device_t *CANDEV_GetNextDevice(void)
{
    const CANDEV_Device_t *ptrDevice = NULL;
    
    if (CANDEV_Devices[nIndex].vendorId != FINAL_VENDOR_ID)
        nIndex += 1;
    if (CANDEV_Devices[nIndex].vendorId != FINAL_VENDOR_ID)
        ptrDevice = &CANDEV_Devices[nIndex];
    return ptrDevice;
}

const CANDEV_Device_t *CANDEV_GetDeviceById(UInt16 vendorId, UInt16 productId)
{
    const CANDEV_Device_t *ptrDevice = NULL;
    
    for (int i = 0; CANDEV_Devices[i].vendorId != FINAL_VENDOR_ID; i++) {
        if ((CANDEV_Devices[i].vendorId == vendorId) &&
            (CANDEV_Devices[i].productId == productId)) {
            ptrDevice = &CANDEV_Devices[i];
            break;
        }
    }
    return ptrDevice;
}

/* * $Id: MacCAN_Devices.c 969 2020-12-27 15:56:48Z eris $ *** (C) UV Software, Berlin ***
 */
