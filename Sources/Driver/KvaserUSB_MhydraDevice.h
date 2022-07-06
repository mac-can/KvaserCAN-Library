/*  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later */
/*
 *  KvaserCAN - macOS User-Space Driver for Kvaser CAN Interfaces
 *
 *  Copyright (c) 2021-2022 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
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
#ifndef KVASERUSB_MHYDRADEVICE_H_INCLUDED
#define KVASERUSB_MHYDRADEVICE_H_INCLUDED

#include "KvaserUSB_Common.h"
#include "KvaserUSB_Device.h"

#define MHYDRA_NUM_CHANNELS  1U
#define MHYDRA_NUM_ENDPOINTS  4U
#define MHYDRA_CPU_FREQUENCY  80U
#define MHYDRA_MAX_OUTSTANDING_TX  200U

#ifdef __cplusplus
extern "C" {
#endif

extern bool Mhydra_ConfigureChannel(KvaserUSB_Device_t *device);

extern CANUSB_Return_t Mhydra_InitializeChannel(KvaserUSB_Device_t *device, const KvaserUSB_OpMode_t opMode);
extern CANUSB_Return_t Mhydra_TeardownChannel(KvaserUSB_Device_t *device);

extern CANUSB_Return_t Mhydra_SetBusParams(KvaserUSB_Device_t *device, const KvaserUSB_BusParams_t *params);
extern CANUSB_Return_t Mhydra_GetBusParams(KvaserUSB_Device_t *device, KvaserUSB_BusParams_t *params);
extern CANUSB_Return_t Mhydra_SetBusParamsFd(KvaserUSB_Device_t *device, const KvaserUSB_BusParamsFd_t *params);
extern CANUSB_Return_t Mhydra_GetBusParamsFd(KvaserUSB_Device_t *device, KvaserUSB_BusParamsFd_t *params);
extern CANUSB_Return_t Mhydra_SetBusParamsTq(KvaserUSB_Device_t *device, const KvaserUSB_BusParamsTq_t *params);
extern CANUSB_Return_t Mhydra_GetBusParamsTq(KvaserUSB_Device_t *device, KvaserUSB_BusParamsTq_t *params);

extern CANUSB_Return_t Mhydra_SetDriverMode(KvaserUSB_Device_t *device, const KvaserUSB_DriverMode_t mode);
extern CANUSB_Return_t Mhydra_GetDriverMode(KvaserUSB_Device_t *device, KvaserUSB_DriverMode_t *mode);

extern CANUSB_Return_t Mhydra_StartChip(KvaserUSB_Device_t *device, uint16_t timeout);
extern CANUSB_Return_t Mhydra_StopChip(KvaserUSB_Device_t *device, uint16_t timeout);
extern CANUSB_Return_t Mhydra_ResetChip(KvaserUSB_Device_t *device, uint16_t delay);
extern CANUSB_Return_t Mhydra_ResetCard(KvaserUSB_Device_t *device, uint16_t delay);
extern CANUSB_Return_t Mhydra_RequestChipState(KvaserUSB_Device_t *device, uint16_t delay);

extern CANUSB_Return_t Mhydra_SendMessage(KvaserUSB_Device_t *device, const KvaserUSB_CanMessage_t *message, uint16_t timeout);
extern CANUSB_Return_t Mhydra_ReadMessage(KvaserUSB_Device_t *device, KvaserUSB_CanMessage_t *message, uint16_t timeout);

extern CANUSB_Return_t Mhydra_FlushQueue(KvaserUSB_Device_t *device/*, uint8_t flags*/);
extern CANUSB_Return_t Mhydra_ResetErrorCounter(KvaserUSB_Device_t *device, uint16_t delay);
extern CANUSB_Return_t Mhydra_ResetStatistics(KvaserUSB_Device_t *device, uint16_t delay);

extern CANUSB_Return_t Mhydra_ReadClock(KvaserUSB_Device_t *device, uint64_t *nsec);
extern CANUSB_Return_t Mhydra_GetBusLoad(KvaserUSB_Device_t *device, KvaserUSB_BusLoad_t *load);

extern CANUSB_Return_t Mhydra_GetCardInfo(KvaserUSB_Device_t *device, KvaserUSB_CardInfo_t *info/*, int8_t dataLevel*/);
extern CANUSB_Return_t Mhydra_GetSoftwareInfo(KvaserUSB_Device_t *device, KvaserUSB_SoftwareInfo_t *info/*, uint8_t hydraExt*/);
extern CANUSB_Return_t Mhydra_GetCapabilities(KvaserUSB_Device_t *device, KvaserUSB_Capabilities_t *capabilities);
extern CANUSB_Return_t Mhydra_GetTransceiverInfo(KvaserUSB_Device_t *device, KvaserUSB_TransceiverInfo_t *info);

#ifdef __cplusplus
}
#endif
#endif /* KVASERUSB_MHYDRADEVICE_H_INCLUDED */
