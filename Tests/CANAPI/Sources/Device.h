//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (Testing)
//
//  Copyright (c) 2004-2023 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//  All rights reserved.
//
//  This file is part of CAN API V3.
//
//  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License and
//  under the GNU General Public License v3.0 (or any later version).
//  You can choose between one of them if you use this file.
//
//  BSD 2-Clause "Simplified" License:
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this
//     list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  CAN API V3 IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF CAN API V3, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
//  CAN API V3 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  CAN API V3 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with CAN API V3.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef DEVICE_H_INCLUDED
#define DEVICE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include "Driver.h"

class CCanDevice : public CCanDriver {
private:
    int32_t m_nLibraryId;
    int32_t m_nChannelNo;
    CANAPI_OpMode_t m_OpMode;
    CANAPI_Bitrate_t m_Bitrate;
    void *m_pParameter;
public:
    CCanDevice(int32_t library, int32_t channel, CANAPI_OpMode_t opMode, CANAPI_Bitrate_t bitRate, void *param = NULL);
    ~CCanDevice();
    // no hidden overrides
    using CCanDriver::ProbeChannel;
    using CCanDriver::InitializeChannel;
    using CCanDriver::StartController;
    using CCanDriver::GetBitrate;
    // getter/setter
    int32_t GetLibraryId() {
        return m_nLibraryId;
    }
    int32_t GetChannelNo() {
        return m_nChannelNo;
    }
    CANAPI_OpMode_t GetOpMode() {
        return m_OpMode;
    }
    CANAPI_Bitrate_t GetBitrate() {
        return m_Bitrate;
    }
    void *GetParameter() {
        return m_pParameter;
    }
    void SetOpMode(CANAPI_OpMode_t opMode) {
        m_OpMode = opMode;
    }
    void SetBitrate(CANAPI_Bitrate_t bitRate) {
        m_Bitrate = bitRate;
    }
    void SetParameter(void *param) {
        m_pParameter = param;
    }
    // delegation
    CANAPI_Return_t ProbeChannel(EChannelState &state) {
#if (OPTION_CANAPI_LIBRARY != 0)
        return CCanDriver::ProbeChannel(m_nLibraryId, m_nChannelNo, m_OpMode, m_pParameter, state);
#else
        return CCanDriver::ProbeChannel(m_nChannelNo, m_OpMode, m_pParameter, state);
#endif
    }
    CANAPI_Return_t InitializeChannel() {
#if (OPTION_CANAPI_LIBRARY != 0)
        return CCanDriver::InitializeChannel(m_nLibraryId, m_nChannelNo, m_OpMode, m_pParameter);
#else
        return CCanDriver::InitializeChannel(m_nChannelNo, m_OpMode, m_pParameter);
#endif
    }
    CANAPI_Return_t StartController() {
        return CCanDriver::StartController(m_Bitrate);
    }
    // predicates
    bool IsCanFdCapable() {
        CANAPI_OpMode_t opCapa = { CANMODE_FDOE };
        EChannelState state = CCanApi::ChannelNotTestable;
        return (CCanDriver::ProbeChannel(m_nChannelNo, opCapa, state) == CCanApi::NoError) ? true : false;
    }
    // properties
    CANAPI_Return_t GetOpCapabilities(CANAPI_OpMode_t &opCapa) {
        return CCanDriver::GetProperty(CANPROP_GET_OP_CAPABILITY, (void*)&opCapa.byte, sizeof(uint8_t));
    }
    CANAPI_Return_t GetOpMode(CANAPI_OpMode_t &opMode) {
        return CCanDriver::GetProperty(CANPROP_GET_OP_MODE, (void*)&opMode.byte, sizeof(uint8_t));
    }
    CANAPI_Return_t GetBitrate(CANAPI_Bitrate_t &bitRate) {
        return CCanDriver::GetProperty(CANPROP_GET_BITRATE, (void*)&bitRate, sizeof(CANAPI_Bitrate_t));
    }
    CANAPI_Return_t GetBusSpeed(CANAPI_BusSpeed_t &busSpeed) {
        return CCanDriver::GetProperty(CANPROP_GET_SPEED, (void*)&busSpeed, sizeof(CANAPI_BusSpeed_t));
    }
    uint64_t GetTxCounter() {
        uint64_t counter = 0U;
        CCanDriver::GetProperty(CANPROP_GET_TX_COUNTER, (void*)&counter, sizeof(uint64_t));
        return counter;
    }
    uint64_t GetRxCounter() {
        uint64_t counter = 0U;
        CCanDriver::GetProperty(CANPROP_GET_RX_COUNTER, (void*)&counter, sizeof(uint64_t));
        return counter;
    }
    uint64_t GetErrorCounter() {
        uint64_t counter = 0U;
        CCanDriver::GetProperty(CANPROP_GET_ERR_COUNTER, (void*)&counter, sizeof(uint64_t));
        return counter;
    }
    // test information
    void ShowDeviceInformation(const char *prefix = NULL);
    void ShowOperationMode(const char *prefix = NULL);
    void ShowBitrateSettings(const char *prefix = NULL);
    static void ShowLibrayInformation(const char *prefix = NULL);
    // channel information
    void ShowChannelInformation(const char* prefix = NULL);
    void ShowChannelCapabilities(const char* prefix = NULL);
    // transmission/reception
    int32_t SendSomeFrames(CCanDevice &other, int32_t frames);
    int32_t ReceiveSomeFrames(CCanDevice &other, int32_t frames);
    // special stuff
    static bool CompareMessages(CANAPI_Message_t message1, CANAPI_Message_t message2, bool esiFlag = false);
    static bool CompareBitrates(CANAPI_Bitrate_t bitRate1, CANAPI_Bitrate_t bitRate2, bool dataPhase = false);
    static uint64_t TransmissionTime(CANAPI_Bitrate_t bitRate, int32_t frames = 1, uint8_t payload = 8U);
    static void ShowTimeDifference(const char *prefix, struct timespec &start, struct timespec &stop);
private:
    int32_t SendAndReceiveFrames(CCanDevice *sender, CCanDevice *receiver, int32_t frames);
    int32_t CheckMessage(CANAPI_Message_t &message, uint64_t expected, uint32_t id);
    bool GetChannelInfoFromDeviceList(CCanApi::SChannelInfo &info);
};

#endif // DEVICE_H_INCLUDED

// $Id: Device.h 1217 2023-10-10 19:28:31Z haumea $  Copyright (c) UV Software, Berlin.
