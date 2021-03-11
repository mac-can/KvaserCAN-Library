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
#ifndef KVASERCAN_DRIVER_H_INCLUDED
#define KVASERCAN_DRIVER_H_INCLUDED

#include "KvaserUSB_Common.h"
#include "KvaserUSB_Device.h"

typedef int KvaserUSB_Channel_t;

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t KvaserCAN_DriverVersion(void);
extern CANUSB_Return_t KvaserCAN_InitializeDriver(void);
extern CANUSB_Return_t KvaserCAN_TeardownDriver(void);

extern CANUSB_Return_t KvaserCAN_ProbeChannel(KvaserUSB_Channel_t channel, const KvaserUSB_OpMode_t opMode, int *state);
extern CANUSB_Return_t KvaserCAN_InitializeChannel(KvaserUSB_Channel_t channel, const KvaserUSB_OpMode_t opMode, KvaserUSB_Device_t *device);
extern CANUSB_Return_t KvaserCAN_TeardownChannel(KvaserUSB_Device_t *device);
extern CANUSB_Return_t KvaserCAN_SignalChannel(KvaserUSB_Device_t *device);

extern CANUSB_Return_t KvaserCAN_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params);
extern CANUSB_Return_t KvaserCAN_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params);
//extern CANUSB_Return_t KvaserCAN_SetBusParamsFD(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *param);
//extern CANUSB_Return_t KvaserCAN_GetBusParamsFD(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *param);

extern CANUSB_Return_t KvaserCAN_CanBusOn(KvaserUSB_Device_t *device, bool silent);
extern CANUSB_Return_t KvaserCAN_CanBusOff(KvaserUSB_Device_t *device);

extern CANUSB_Return_t KvaserCAN_WriteMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout);
extern CANUSB_Return_t KvaserCAN_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout);

extern CANUSB_Return_t KvaserCAN_GetBusStatus(KvaserUSB_Device_t *device, KvaserUSB_BusStatus_t *status);
extern CANUSB_Return_t KvaserCAN_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load);

extern uint8_t KvaserCAN_Dlc2Len(uint8_t dlc);
extern uint8_t KvaserCAN_Len2Dlc(uint8_t len);

#ifdef __cplusplus
}
#endif
#endif /* KVASERCAN_DRIVER_H_INCLUDED */
