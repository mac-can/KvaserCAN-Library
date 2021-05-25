/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
 *
 *  Copyright (C) 2020-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
 *
 *  This file is part of MacCAN-KvaserCAN.
 *
 *  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with MacCAN-KvaserCAN.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef LEAF_PRO_H_INCLUDED
#define LEAF_PRO_H_INCLUDED

#include "KvaserUSB_Common.h"
#include "KvaserUSB_Device.h"

#define LEAF_PRO_VENDOR_ID   KVASER_VENDOR_ID
#define LEAF_PRO_PRODUCT_ID  KVASER_LEAF_PRO_ID

#define LEAF_PRO_NUM_CHANNELS  1U
#define LEAF_PRO_NUM_ENDPOINTS  4U
#define LEAF_PRO_CPU_FREQUENCY  80U
#define LEAF_PRO_MAX_OUTSTANDING_TX  200U

#define LEAF_PRO_MODE_FDOE  0 /* CAN FD operation enable/disable */
#define LEAF_PRO_MODE_BRSE  0 /* bit-rate switch enable/disable */
#define LEAF_PRO_MODE_NISO  0 /* Non-ISO CAN FD enable/disable */
#define LEAF_PRO_MODE_SHRD  0 /* shared access enable/disable */
#define LEAF_PRO_MODE_NXTD  1 /* extended format disable/enable */
#define LEAF_PRO_MODE_NRTR  1 /* remote frames disable/enable */
#define LEAF_PRO_MODE_ERR   1 /* error frames enable/disable */
#define LEAF_PRO_MODE_MON   1 /* monitor mode enable/disable */

#ifdef __cplusplus
extern "C" {
#endif

extern bool LeafPro_ConfigureChannel(KvaserUSB_Device_t *device);
extern void LeafPro_GetOperationCapability(KvaserUSB_OpMode_t *opMode);

extern CANUSB_Return_t LeafPro_InitializeChannel(KvaserUSB_Device_t *device, const KvaserUSB_OpMode_t opMode);
extern CANUSB_Return_t LeafPro_TeardownChannel(KvaserUSB_Device_t *device);

extern CANUSB_Return_t LeafPro_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params);
extern CANUSB_Return_t LeafPro_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params);
extern CANUSB_Return_t LeafPro_SetBusParamsFd(KvaserUSB_Device_t *device, const KvaserUSB_BusParamsFd_t *params);
extern CANUSB_Return_t LeafPro_GetBusParamsFd(KvaserUSB_Device_t *device, KvaserUSB_BusParamsFd_t *params);

extern CANUSB_Return_t LeafPro_SetDriverMode(KvaserUSB_Device_t *device, const KvaserUSB_DriverMode_t mode);
extern CANUSB_Return_t LeafPro_GetDriverMode(KvaserUSB_Device_t *device, KvaserUSB_DriverMode_t *mode);

extern CANUSB_Return_t LeafPro_StartChip(KvaserUSB_Device_t *device, uint16_t timeout);
extern CANUSB_Return_t LeafPro_StopChip(KvaserUSB_Device_t *device, uint16_t timeout);
//extern CANUSB_Return_t LeafPro_ResetChip(KvaserUSB_Device_t *device, uint16_t delay);
//extern CANUSB_Return_t LeafPro_ResetCard(KvaserUSB_Device_t *device, uint16_t delay);
extern CANUSB_Return_t LeafPro_RequestChipState(KvaserUSB_Device_t *device, uint16_t delay);

extern CANUSB_Return_t LeafPro_SendMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout);
extern CANUSB_Return_t LeafPro_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout);

extern CANUSB_Return_t LeafPro_FlushQueue(KvaserUSB_Device_t *device/*, uint8_t flags*/);
//extern CANUSB_Return_t LeafPro_ResetErrorCounter(KvaserUSB_Device_t *device, uint16_t delay);
//extern CANUSB_Return_t LeafPro_ResetStatistics(KvaserUSB_Device_t *device, uint16_t delay);

extern CANUSB_Return_t LeafPro_ReadClock(KvaserUSB_Device_t *device, uint64_t *nsec);
extern CANUSB_Return_t LeafPro_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load);

extern CANUSB_Return_t LeafPro_GetCardInfo(KvaserUSB_Device_t *device, KvaserUSB_CardInfo_t *info/*, int8_t dataLevel*/);
extern CANUSB_Return_t LeafPro_GetSoftwareInfo(KvaserUSB_Device_t *device, KvaserUSB_SoftwareInfo_t *info/*, uint8_t hydraExt*/);
//extern CANUSB_Return_t LeafPro_GetInterfaceInfo(KvaserUSB_Device_t *device, KvaserUSB_InterfaceInfo_t *info);


#ifdef __cplusplus
}
#endif
#endif /* LEAF_PRO_H_INCLUDED */
