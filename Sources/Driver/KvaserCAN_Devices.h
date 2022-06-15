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
#ifndef KVASERCAN_DEVICES_H_INCLUDED
#define KVASERCAN_DEVICES_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

/** @name  Supported Kvaser Leaf Interfaces
 *  @brief Options to activate single devices from USB-to-CAN Leaf device family.
 *  @{ */
#define OPTION_USB_LEAF_DEVEL_DEVICE             0 ///< Kvaser Leaf prototype (P010v2 and v3)
#define OPTION_USB_LEAF_LITE_DEVICE              0 ///< Kvaser Leaf Light (P010v3)
#define OPTION_USB_LEAF_PRO_DEVICE               0 ///< Kvaser Leaf Professional HS
#define OPTION_USB_LEAF_SPRO_DEVICE              0 ///< Kvaser Leaf SemiPro HS
#define OPTION_USB_LEAF_PRO_LS_DEVICE            0 ///< Kvaser Leaf Professional LS
#define OPTION_USB_LEAF_PRO_SWC_DEVICE           0 ///< Kvaser Leaf Professional SWC
#define OPTION_USB_LEAF_PRO_LIN_DEVICE           0 ///< Kvaser Leaf Professional LIN
#define OPTION_USB_LEAF_SPRO_LS_DEVICE           0 ///< Kvaser Leaf SemiPro LS
#define OPTION_USB_LEAF_SPRO_SWC_DEVICE          0 ///< Kvaser Leaf SemiPro SWC
#define OPTION_USB_MEMO2_DEVEL_DEVICE            0 ///< Kvaser Memorator II, Prototype
#define OPTION_USB_MEMO2_HSHS_DEVICE             0 ///< Kvaser Memorator II HS/HS
#define OPTION_USB_UPRO_HSHS_DEVICE              0 ///< Kvaser USBcan Professional HS/HS
#define OPTION_USB_LEAF_LITE_GI_DEVICE           0 ///< Kvaser Leaf Light GI
#define OPTION_USB_LEAF_PRO_OBDII_DEVICE         0 ///< Kvaser Leaf Professional HS (OBD-II connector)
#define OPTION_USB_MEMO2_HSLS_DEVICE             0 ///< Kvaser Memorator Professional HS/LS
#define OPTION_USB_LEAF_LITE_CH_DEVICE           0 ///< Kvaser Leaf Light "China"
#define OPTION_USB_BLACKBIRD_SPRO_DEVICE         0 ///< Kvaser BlackBird SemiPro
#define OPTION_USB_MEMO_R_SPRO_DEVICE            0 ///< Kvaser Memorator R SemiPro
#define OPTION_USB_OEM_MERCURY_DEVICE            0 ///< Kvaser OEM Mercury
#define OPTION_USB_OEM_LEAF_DEVICE               0 ///< Kvaser OEM Leaf
#define OPTION_USB_OEM_KEY_DRIVING_DEVICE        0 ///< Key Driving Interface HS
#define OPTION_USB_CAN_R_DEVICE                  0 ///< Kvaser USBcan R
#define OPTION_USB_LEAF_LITE_V2_DEVICE           1 ///< Kvaser Leaf Light v2
#define OPTION_USB_MINI_PCI_EXPRESS_HS_DEVICE    0 ///< Kvaser Mini PCI Express HS
#define OPTION_USB_LEAF_LIGHT_HS_V2_OEM_DEVICE   0 ///< Kvaser Leaf Light HS v2 OEM
#define OPTION_USB_USBCAN_LIGHT_2HS_DEVICE       0 ///< Kvaser USBcan Light 2xHS
#define OPTION_USB_MINI_PCI_EXPRESS_2HS_DEVICE   0 ///< Kvaser Mini PCI Express 2xHS
#define OPTION_USB_USBCAN_R_V2_DEVICE            0 ///< Kvaser USBcan R v2
#define OPTION_USB_LEAF_LITE_R_V2_DEVICE         0 ///< Kvaser Leaf Light R v2
#define OPTION_USB_OEM_ATI_LEAF_LITE_V2_DEVICE   0 ///< Kvaser OEM ATI Leaf Light HS v2
/** @} */

/** @name  Supported Kvaser Mhydra* HW Interfaces
 *  @brief Options to activate single devices from USB-to-CAN Mhydra* device family.
 *  @{ */
#define OPTION_USB_EAGLE_DEVICE                  0 ///< Kvaser Eagle
#define OPTION_USB_BLACKBIRD_V2_DEVICE           0 ///< Kvaser BlackBird v2
#define OPTION_USB_MEMO_PRO_5HS_DEVICE           0 ///< Kvaser Memorator Pro 5xHS
#define OPTION_USB_USBCAN_PRO_5HS_DEVICE         0 ///< Kvaser USBcan Pro 5xHS
#define OPTION_USB_USBCAN_LIGHT_4HS_DEVICE       0 ///< Kvaser USBcan Light 4xHS (00831-1)
#define OPTION_USB_LEAF_PRO_HS_V2_DEVICE         1 ///< Kvaser Leaf Pro HS v2 (00843-4)
#define OPTION_USB_USBCAN_PRO_2HS_V2_DEVICE      0 ///< Kvaser USBcan Pro 2xHS v2 (00752-9)
#define OPTION_USB_MEMO_2HS_DEVICE               0 ///< Kvaser Memorator 2xHS v2 (00821-2)
#define OPTION_USB_MEMO_PRO_2HS_V2_DEVICE        0 ///< Kvaser Memorator Pro 2xHS v2 (00819-9)
#define OPTION_USB_HYBRID_CANLIN_DEVICE          0 ///< Kvaser Hybrid 2xCAN/LIN (00965-3)
#define OPTION_USB_ATI_USBCAN_PRO_2HS_V2_DEVICE  0 ///< ATI USBcan Pro 2xHS v2 (00969-1)
#define OPTION_USB_ATI_MEMO_PRO_2HS_V2_DEVICE    0 ///< ATI Memorator Pro 2xHS v2 (00971-4)
#define OPTION_USB_HYBRID_PRO_CANLIN_DEVICE      1 ///< Kvaser Hybrid Pro 2xCAN/LIN (01042-0)
#define OPTION_USB_BLACKBIRD_PRO_HS_V2_DEVICE    0 ///< Kvaser BlackBird Pro HS v2 (00983-7)
#define OPTION_USB_MEMO_LIGHT_HS_V2_DEVICE       0 ///< Kvaser Memorator Light HS v2 (01058-1)
#define OPTION_USB_U100_DEVICE                   0 ///< Kvaser U100 (01173-1)
#define OPTION_USB_U100P_DEVICE                  1 ///< Kvaser U100P (01174-8)
#define OPTION_USB_U100S_DEVICE                  0 ///< Kvaser U100P (01181-6)
#define OPTION_USB_USBCAN_PRO_4HS_DEVICE         0 ///< Kvaser USBcan Pro 4xHS (01261-5)
#define OPTION_USB_HYBRID_1HS_CANLIN_DEVICE      0 ///< Kvaser Hybrid CAN/LIN (01284-4)
#define OPTION_USB_HYBRID_PRO_1HS_CANLIN_DEVICE  0 ///< Kvaser Hybrid Pro CAN/LIN (01288-2)
/** @} */

/** @name  Kvaser Device Families
 *  @brief Enumeration of device families.
 *  @{ */
typedef enum kavser_device_family_t_ {
    KVASER_USB_LEAF_DEVICE_FAMILY,      /**< USB-to-CAN Leaf devices */
    KVASER_USB_MHYDRA_DEVICE_FAMILY,    /**< USB-to-CAN Mhydra devices */
    KVASER_UNKNOWN_DEVICE_FAMILY        /**< unknown/unsupported (last entry) */
} KvaserCAN_DeviceFamily_t;
/** @} */

/** @name  Kvaser Leaf prototype (P010v2 and v3)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_DEVEL_DEVICE != 0)
    #define USB_LEAF_DEVEL_PRODUCT_ID  10U
    #define USB_LEAF_DEVEL_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Light (P010v3)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_LITE_DEVICE != 0)
    #define USB_LEAF_LITE_PRODUCT_ID  11U
    #define USB_LEAF_LITE_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Professional HS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_PRO_DEVICE != 0)
    #define USB_LEAF_PRO_PRODUCT_ID  12U
    #define USB_LEAF_PRO_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf SemiPro HS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_SPRO_DEVICE != 0)
    #define USB_LEAF_SPRO_PRODUCT_ID  14U
    #define USB_LEAF_SPRO_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Professional LS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_PRO_LS_DEVICE != 0)
    #define USB_LEAF_PRO_LS_PRODUCT_ID  15U
    #define USB_LEAF_PRO_LS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Professional SWC
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_PRO_SWC_DEVICE != 0)
    #define USB_LEAF_PRO_SWC_PRODUCT_ID  16U
    #define USB_LEAF_PRO_SWC_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Professional LIN
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_PRO_LIN_DEVICE != 0)
    #define USB_LEAF_PRO_LIN_PRODUCT_ID  17U
    #define USB_LEAF_PRO_LIN_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf SemiPro LS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_SPRO_LS_DEVICE != 0)
    #define USB_LEAF_SPRO_LS_PRODUCT_ID  18U
    #define USB_LEAF_SPRO_LS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf SemiPro SWC
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_SPRO_SWC_DEVICE != 0)
    #define USB_LEAF_SPRO_SWC_PRODUCT_ID  19U
    #define USB_LEAF_SPRO_SWC_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator II, Prototype
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO2_DEVEL_DEVICE != 0)
    #define USB_MEMO2_DEVEL_PRODUCT_ID  22U
    #define USB_MEMO2_DEVEL_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator II HS/HS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO2_HSHS_DEVICE != 0)
    #define USB_MEMO2_HSHS_PRODUCT_ID  23U
    #define USB_MEMO2_HSHS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser USBcan Professional HS/HS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_UPRO_HSHS_DEVICE != 0)
    #define USB_UPRO_HSHS_PRODUCT_ID  24U
    #define USB_UPRO_HSHS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Light GI
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_LITE_GI_DEVICE != 0)
    #define USB_LEAF_LITE_GI_PRODUCT_ID  25U
    #define USB_LEAF_LITE_GI_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Professional HS (OBD-II connector)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_PRO_OBDII_DEVICE != 0)
    #define USB_LEAF_PRO_OBDII_PRODUCT_ID  26U
    #define USB_LEAF_PRO_OBDII_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator Professional HS/LS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO2_HSLS_DEVICE != 0)
    #define USB_MEMO2_HSLS_PRODUCT_ID  27U
    #define USB_MEMO2_HSLS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Light "China"
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_LITE_CH_DEVICE != 0)
    #define USB_LEAF_LITE_CH_PRODUCT_ID  28U
    #define USB_LEAF_LITE_CH_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser BlackBird SemiPro
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_BLACKBIRD_SPRO_DEVICE != 0)
    #define USB_BLACKBIRD_SPRO_PRODUCT_ID  29U
    #define USB_BLACKBIRD_SPRO_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator R SemiPro
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO_R_SPRO_DEVICE != 0)
    #define USB_MEMO_R_SPRO_PRODUCT_ID  32U
    #define USB_MEMO_R_SPRO_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser OEM Mercury
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_OEM_MERCURY_DEVICE != 0)
    #define USB_OEM_MERCURY_PRODUCT_ID  34U
    #define USB_OEM_MERCURY_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser OEM Leaf
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_OEM_LEAF_DEVICE != 0)
    #define USB_OEM_LEAF_PRODUCT_ID  35U
    #define USB_OEM_LEAF_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Key Driving Interface HS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_OEM_KEY_DRIVING_DEVICE != 0)
    #define USB_OEM_KEY_DRIVING_PRODUCT_ID  38U
    #define USB_OEM_KEY_DRIVING_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser USBcan R
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_CAN_R_DEVICE != 0)
    #define USB_CAN_R_PRODUCT_ID  39U
    #define USB_CAN_R_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Eagle
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_EAGLE_DEVICE != 0)
    #define USB_EAGLE_PRODUCT_ID  256U
    #define USB_EAGLE_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser BlackBird v2
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_BLACKBIRD_V2_DEVICE != 0)
    #define USB_BLACKBIRD_V2_PRODUCT_ID  258U
    #define USB_BLACKBIRD_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator Pro 5xHS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO_PRO_5HS_DEVICE != 0)
    #define USB_MEMO_PRO_5HS_PRODUCT_ID  260U
    #define USB_MEMO_PRO_5HS_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser USBcan Pro 5xHS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_USBCAN_PRO_5HS_DEVICE != 0)
    #define USB_USBCAN_PRO_5HS_PRODUCT_ID  261U
    #define USB_USBCAN_PRO_5HS_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser USBcan Light 4xHS (00831-1)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_USBCAN_LIGHT_4HS_DEVICE != 0)
    #define USB_USBCAN_LIGHT_4HS_PRODUCT_ID  262U
    #define USB_USBCAN_LIGHT_4HS_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Pro HS v2 (00843-4)
 *  @brief Professional single channel USB-to-CAN/CAN FD interface with t-Program.
 *  @{ */
#if (OPTION_USB_LEAF_PRO_HS_V2_DEVICE != 0)
    #define USB_LEAF_PRO_HS_V2_PRODUCT_ID  263U
    #define USB_LEAF_PRO_HS_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #define USB_LEAF_PRO_HS_V2_NUM_CHANNELS  1U
    #define USB_LEAF_PRO_HS_V2_CAN_CLOCK   80U
    #define USB_LEAF_PRO_HS_V2_TIMER_FREQ  80U
    #define USB_LEAF_PRO_HS_V2_CAP_CANFD   true
    #define USB_LEAF_PRO_HS_V2_CAP_NONISO  true
    #define USB_LEAF_PRO_HS_V2_CAP_SILENT_MODE  true
    #define USB_LEAF_PRO_HS_V2_CAP_ERROR_FRAME  true
#endif
/** @} */

/** @name  Kvaser USBcan Pro 2xHS v2 (00752-9)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_USBCAN_PRO_2HS_V2_DEVICE != 0)
    #define USB_USBCAN_PRO_2HS_V2_PRODUCT_ID  264U
    #define USB_USBCAN_PRO_2HS_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator 2xHS v2 (00821-2)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO_2HS_DEVICE != 0)
    #define USB_MEMO_2HS_PRODUCT_ID  265U
    #define USB_MEMO_2HS_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator Pro 2xHS v2 (00819-9)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO_PRO_2HS_V2_DEVICE != 0)
    #define USB_MEMO_PRO_2HS_V2_PRODUCT_ID  266U
    #define USB_MEMO_PRO_2HS_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Hybrid 2xCAN/LIN (00965-3)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_HYBRID_CANLIN_DEVICE != 0)
    #define USB_HYBRID_CANLIN_PRODUCT_ID  267U
    #define USB_HYBRID_CANLIN_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/* ATI USBcan Pro 2xHS v2 (00969-1)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_ATI_USBCAN_PRO_2HS_V2_DEVICE != 0)
    #define USB_ATI_USBCAN_PRO_2HS_V2_PRODUCT_ID  268U
    #define USB_ATI_USBCAN_PRO_2HS_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/* ATI Memorator Pro 2xHS v2 (00971-4)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_ATI_MEMO_PRO_2HS_V2_DEVICE != 0)
    #define USB_ATI_MEMO_PRO_2HS_V2_PRODUCT_ID  269U
    #define USB_ATI_MEMO_PRO_2HS_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Hybrid Pro 2xCAN/LIN (01042-0)
 *  @brief Flexible, dual channel interface for CAN, CAN FD and LIN with t-Program.
 *  @{ */
#if (OPTION_USB_HYBRID_PRO_CANLIN_DEVICE != 0)
    #define USB_HYBRID_PRO_CANLIN_PRODUCT_ID  270U
    #define USB_HYBRID_PRO_CANLIN_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #define USB_HYBRID_PRO_CANLIN_NUM_CHANNELS  1U  // TODO: 2x CAN/LIN
    #define USB_HYBRID_PRO_CANLIN_CAN_CLOCK   80U
    #define USB_HYBRID_PRO_CANLIN_TIMER_FREQ  80U
    #define USB_HYBRID_PRO_CANLIN_CAP_CANFD   true
    #define USB_HYBRID_PRO_CANLIN_CAP_NONISO  true
    #define USB_HYBRID_PRO_CANLIN_CAP_SILENT_MODE  true
    #define USB_HYBRID_PRO_CANLIN_CAP_ERROR_FRAME  true
#endif
/** @} */

/** @name  Kvaser BlackBird Pro HS v2 (00983-7)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_BLACKBIRD_PRO_HS_V2_DEVICE != 0)
    #define USB_BLACKBIRD_PRO_HS_V2_PRODUCT_ID  271U
    #define USB_BLACKBIRD_PRO_HS_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Memorator Light HS v2 (01058-1)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MEMO_LIGHT_HS_V2_DEVICE != 0)
    #define USB_MEMO_LIGHT_HS_V2_PRODUCT_ID  272U
    #define USB_MEMO_LIGHT_HS_V2_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser U100 (01173-1)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_U100_DEVICE != 0)
    #define USB_U100_PRODUCT_ID  273U
    #define USB_U100_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser U100P (01174-8)
 *  @brief Precision version of Kvaser’s U100 range of CAN to USB interfaces.
 *  @{ */
#if (OPTION_USB_U100P_DEVICE != 0)
    #define USB_U100P_PRODUCT_ID  274U
    #define USB_U100P_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #define USB_U100P_NUM_CHANNELS  1U
    #define USB_U100P_CAN_CLOCK   80U
    #define USB_U100P_TIMER_FREQ  24U
    #define USB_U100P_CAP_CANFD   true
    #define USB_U100P_CAP_NONISO  true
    #define USB_U100P_CAP_SILENT_MODE  true
    #define USB_U100P_CAP_ERROR_FRAME  true
#endif
/** @} */

/** @name  Kvaser U100P (01181-6)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_U100S_DEVICE != 0)
    #define USB_U100S_PRODUCT_ID  275U
    #define USB_U100S_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser USBcan Pro 4xHS (01261-5)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_USBCAN_PRO_4HS_DEVICE != 0)
    #define USB_USBCAN_PRO_4HS_PRODUCT_ID  276U
    #define USB_USBCAN_PRO_4HS_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Hybrid CAN/LIN (01284-4)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_HYBRID_1HS_CANLIN_DEVICE != 0)
    #define USB_HYBRID_1HS_CANLIN_PRODUCT_ID  277U
    #define USB_HYBRID_1HS_CANLIN_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Hybrid Pro CAN/LIN (01288-2)
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_HYBRID_PRO_1HS_CANLIN_DEVICE != 0)
    #define USB_HYBRID_PRO_1HS_CANLIN_PRODUCT_ID  278U
    #define USB_HYBRID_PRO_1HS_CANLIN_DRV_FAMILY  KVASER_USB_MHYDRA_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Light v2
 *  @brief Single channel USB-to-CAN PC interface.
 *  @{ */
#if (OPTION_USB_LEAF_LITE_V2_DEVICE != 0)
    #define USB_LEAF_LITE_V2_PRODUCT_ID  288U
    #define USB_LEAF_LITE_V2_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #define USB_LEAF_LITE_V2_NUM_CHANNELS  1U
    #define USB_LEAF_LITE_V2_CAN_CLOCK   24U
    #define USB_LEAF_LITE_V2_TIMER_FREQ  24U
    #define USB_LEAF_LITE_V2_CAP_CANFD   false
    #define USB_LEAF_LITE_V2_CAP_NONISO  false
    #define USB_LEAF_LITE_V2_CAP_SILENT_MODE  false  // note: why?
    #define USB_LEAF_LITE_V2_CAP_ERROR_FRAME  false  // note: why?
#endif
/** @} */

/** @name  Kvaser Mini PCI Express HS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MINI_PCI_EXPRESS_HS_DEVICE != 0)
    #define USB_MINI_PCI_EXPRESS_HS_PRODUCT_ID  289U
    #define USB_MINI_PCI_EXPRESS_HS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Light HS v2 OEM
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_LIGHT_HS_V2_OEM_DEVICE != 0)
    #define USB_LEAF_LIGHT_HS_V2_OEM_PRODUCT_ID  290U
    #define USB_LEAF_LIGHT_HS_V2_OEM_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser USBcan Light 2xHS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_USBCAN_LIGHT_2HS_DEVICE != 0)
    #define USB_USBCAN_LIGHT_2HS_PRODUCT_ID  291U
    #define USB_USBCAN_LIGHT_2HS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Mini PCI Express 2xHS
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_MINI_PCI_EXPRESS_2HS_DEVICE != 0)
    #define USB_MINI_PCI_EXPRESS_2HS_PRODUCT_ID  292U
    #define USB_MINI_PCI_EXPRESS_2HS_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser USBcan R v2
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_USBCAN_R_V2_DEVICE != 0)
    #define USB_USBCAN_R_V2_PRODUCT_ID  294U
    #define USB_USBCAN_R_V2_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser Leaf Light R v2
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_LEAF_LITE_R_V2_DEVICE != 0)
    #define USB_LEAF_LITE_R_V2_PRODUCT_ID  295U
    #define USB_LEAF_LITE_R_V2_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

/** @name  Kvaser OEM ATI Leaf Light HS v2
 *  @brief Tbd.
 *  @{ */
#if (OPTION_USB_OEM_ATI_LEAF_LITE_V2_DEVICE != 0)
    #define USB_OEM_ATI_LEAF_LITE_V2_PRODUCT_ID  296U
    #define USB_OEM_ATI_LEAF_LITE_V2_DRV_FAMILY  KVASER_USB_LEAF_DEVICE_FAMILY
    #error Device properties not defined!
#endif
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

extern KvaserCAN_DeviceFamily_t KvaserDEV_GetDeviceFamily(uint16_t productId);
extern uint8_t KvaserDEV_GetNumberOfCanChannels(uint16_t productId);
extern uint8_t KvaserDEV_GetCanClockInMHz(uint16_t productId);
extern uint8_t KvaserDEV_GetTimerFreqInMHz(uint16_t productId);
extern bool KvaserDEV_IsCanFdSupported(uint16_t productId);
extern bool KvaserDEV_IsNonIsoCanFdSupported(uint16_t productId);
extern bool KvaserDEV_IsSilentModeSupported(uint16_t productId);
extern bool KvaserDEV_IsErrorFrameSupported(uint16_t productId);

#ifdef __cplusplus
}
#endif
#endif /* KVASERCAN_DEVICES_H_INCLUDED */
