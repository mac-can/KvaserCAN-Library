/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
 *
 *  Copyright (c) 2022 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *  All rights reserved.
 *
 *  This file is part of MacCAN-KvaserCAN.
 *
 *  MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License
 *  and under the GNU General Public License v3.0 (or any later version). You can
 *  choose between one of them if you use MacCAN-KvaserCAN in whole or in part.
 *
 *  BSD 2-Clause Simplified License:
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  MacCAN-KvaserCAN IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF MacCAN-KvaserCAN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  GNU General Public License v3.0 or later:
 *  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MacCAN-KvaserCAN.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "KvaserCAN_Devices.h"
#include "MacCAN_Devices.h"

#define KVASER_VENDOR_ID  0xBFDU

const CANDEV_Device_t CANDEV_Devices[] = {
#if (OPTION_USB_LEAF_DEVEL_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_SPRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_LS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_SWC_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_LIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_SPRO_LS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_SPRO_SWC_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO2_DEVEL_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO2_HSHS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_UPRO_HSHS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_GI_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_OBDII_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO2_HSLS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_CH_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_BLACKBIRD_SPRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_R_SPRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_MERCURY_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_LEAF_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_KEY_DRIVING_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_CAN_R_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_EAGLE_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_BLACKBIRD_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_PRO_5HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_PRO_5HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_LIGHT_4HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_HS_V2_DEVICE != 0)
    {KVASER_VENDOR_ID, USB_LEAF_PRO_HS_V2_PRODUCT_ID, USB_LEAF_PRO_HS_V2_NUM_CHANNELS},
#endif
#if (OPTION_USB_USBCAN_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_2HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_CANLIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_ATI_USBCAN_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_ATI_MEMO_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_PRO_CANLIN_DEVICE != 0)
    {KVASER_VENDOR_ID, USB_HYBRID_PRO_CANLIN_PRODUCT_ID, USB_HYBRID_PRO_CANLIN_NUM_CHANNELS},
#endif
#if (OPTION_USB_BLACKBIRD_PRO_HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_LIGHT_HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_U100_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_U100P_DEVICE != 0)
    {KVASER_VENDOR_ID, USB_U100P_PRODUCT_ID, USB_U100P_NUM_CHANNELS},
#endif
#if (OPTION_USB_U100S_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_PRO_4HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_1HS_CANLIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_PRO_1HS_CANLIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_V2_DEVICE != 0)
    {KVASER_VENDOR_ID, USB_LEAF_LITE_V2_PRODUCT_ID, USB_LEAF_LITE_V2_NUM_CHANNELS},
#endif
#if (OPTION_USB_MINI_PCI_EXPRESS_HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LIGHT_HS_V2_OEM_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_LIGHT_2HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MINI_PCI_EXPRESS_2HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_R_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_R_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_ATI_LEAF_LITE_V2_DEVICE != 0)
    #error Device not supported!
#endif
    CANDEV_LAST_ENTRY_IN_DEVICE_LIST
};

static const struct kavser_can_device_t_ {
    uint16_t productId;
    KvaserCAN_DeviceFamily_t deviceFamily;
    uint8_t numChannels;
    uint8_t canClock;
    uint8_t tmrFreq;
    bool capCanFd;
    bool capNonIso;
    bool capSilentMode;
    bool capErrorFrame;
} Kvaser_Devices[] = {
#if (OPTION_USB_LEAF_DEVEL_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_SPRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_LS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_SWC_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_LIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_SPRO_LS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_SPRO_SWC_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO2_DEVEL_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO2_HSHS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_UPRO_HSHS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_GI_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_OBDII_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO2_HSLS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_CH_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_BLACKBIRD_SPRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_R_SPRO_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_MERCURY_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_LEAF_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_KEY_DRIVING_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_CAN_R_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_EAGLE_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_BLACKBIRD_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_PRO_5HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_PRO_5HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_LIGHT_4HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_PRO_HS_V2_DEVICE != 0)
    {USB_LEAF_PRO_HS_V2_PRODUCT_ID, USB_LEAF_PRO_HS_V2_DRV_FAMILY, USB_LEAF_PRO_HS_V2_NUM_CHANNELS, USB_LEAF_PRO_HS_V2_CAN_CLOCK, USB_LEAF_PRO_HS_V2_TIMER_FREQ, USB_LEAF_PRO_HS_V2_CAP_CANFD, USB_LEAF_PRO_HS_V2_CAP_NONISO, USB_LEAF_PRO_HS_V2_CAP_SILENT_MODE, USB_LEAF_PRO_HS_V2_CAP_ERROR_FRAME},
#endif
#if (OPTION_USB_USBCAN_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_2HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_CANLIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_ATI_USBCAN_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_ATI_MEMO_PRO_2HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_PRO_CANLIN_DEVICE != 0)
    {USB_HYBRID_PRO_CANLIN_PRODUCT_ID, USB_HYBRID_PRO_CANLIN_DRV_FAMILY, USB_HYBRID_PRO_CANLIN_NUM_CHANNELS, USB_HYBRID_PRO_CANLIN_CAN_CLOCK, USB_HYBRID_PRO_CANLIN_TIMER_FREQ, USB_HYBRID_PRO_CANLIN_CAP_CANFD, USB_HYBRID_PRO_CANLIN_CAP_NONISO, USB_HYBRID_PRO_CANLIN_CAP_SILENT_MODE, USB_HYBRID_PRO_CANLIN_CAP_ERROR_FRAME},
#endif
#if (OPTION_USB_BLACKBIRD_PRO_HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MEMO_LIGHT_HS_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_U100_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_U100P_DEVICE != 0)
    {USB_U100P_PRODUCT_ID, USB_U100P_DRV_FAMILY, USB_U100P_NUM_CHANNELS, USB_U100P_CAN_CLOCK, USB_U100P_TIMER_FREQ, USB_U100P_CAP_CANFD, USB_U100P_CAP_NONISO, USB_U100P_CAP_SILENT_MODE, USB_U100P_CAP_ERROR_FRAME},
#endif
#if (OPTION_USB_U100S_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_PRO_4HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_1HS_CANLIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_HYBRID_PRO_1HS_CANLIN_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_V2_DEVICE != 0)
    {USB_LEAF_LITE_V2_PRODUCT_ID, USB_LEAF_LITE_V2_DRV_FAMILY, USB_LEAF_LITE_V2_NUM_CHANNELS, USB_LEAF_LITE_V2_CAN_CLOCK, USB_LEAF_LITE_V2_TIMER_FREQ, USB_LEAF_LITE_V2_CAP_CANFD, USB_LEAF_LITE_V2_CAP_NONISO, USB_LEAF_LITE_V2_CAP_SILENT_MODE, USB_LEAF_LITE_V2_CAP_ERROR_FRAME},
#endif
#if (OPTION_USB_MINI_PCI_EXPRESS_HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LIGHT_HS_V2_OEM_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_LIGHT_2HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_MINI_PCI_EXPRESS_2HS_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_USBCAN_R_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_LEAF_LITE_R_V2_DEVICE != 0)
    #error Device not supported!
#endif
#if (OPTION_USB_OEM_ATI_LEAF_LITE_V2_DEVICE != 0)
    #error Device not supported!
#endif
    {0xFFFFU, KVASER_UNKNOWN_DEVICE_FAMILY, 0U, 0U, 0U, false, false, false, false}
};

KvaserCAN_DeviceFamily_t KvaserDEV_GetDeviceFamily(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return Kvaser_Devices[i].deviceFamily;
    }
    return KVASER_UNKNOWN_DEVICE_FAMILY;
}

uint8_t KvaserDEV_GetNumberOfCanChannels(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return Kvaser_Devices[i].numChannels;
    }
    return 0U;
}

uint8_t KvaserDEV_GetCanClockInMHz(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return Kvaser_Devices[i].canClock;
    }
    return 0U;
}

uint8_t KvaserDEV_GetTimerFreqInMHz(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return Kvaser_Devices[i].tmrFreq;
    }
    return 0U;
}

bool KvaserDEV_IsCanFdSupported(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return (bool)Kvaser_Devices[i].capCanFd;
    }
    return false;
}

bool KvaserDEV_IsNonIsoCanFdSupported(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return (bool)Kvaser_Devices[i].capNonIso;
    }
    return false;
}

bool KvaserDEV_IsSilentModeSupported(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return (bool)Kvaser_Devices[i].capSilentMode;
    }
    return false;
}

bool KvaserDEV_IsErrorFrameSupported(uint16_t productId) {
    for (int i = 0; Kvaser_Devices[i].productId != 0xFFFFU; i++) {
        if (Kvaser_Devices[i].productId == productId)
            return (bool)Kvaser_Devices[i].capErrorFrame;
    }
    return false;
}
