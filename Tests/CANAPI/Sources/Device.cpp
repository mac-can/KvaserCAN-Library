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
#include "pch.h"
#include <string.h>
#include <iostream>

#define ERROR_MSG_ID  (-70)
#define ERROR_MSG_DLC  (-71)
#define ERROR_MSG_LOST  (-72)
#define ERROR_MSG_FATAL  (-79)

#define DEVICE_DEBUG 0
#if (DEVICE_DEBUG != 0)
#define MESSAGE(n,i,l,d) do{ printf("n=%x : %03x [%u] %02x %02x %02x %02x %02x %02x %02x %02x\n",n,i,l,d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7]); }while(0)
#else
#define MESSAGE(n,i,l,d) do{ }while(0)
#endif

CCanDevice::CCanDevice(int32_t library, int32_t channel, CANAPI_OpMode_t opMode, CANAPI_Bitrate_t bitRate, void *param) {
    m_nLibraryId = library;
    m_nChannelNo = channel;
    m_OpMode = opMode;
    m_Bitrate = bitRate;
    m_pParameter = param;
}

CCanDevice::~CCanDevice() {
    // to be on the safe side
    CCanDriver::TeardownChannel();
}

int32_t CCanDevice::SendSomeFrames(CCanDevice &other, int32_t frames) {
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    bool initialized = false;
    bool started = false;

    if (frames <= (int32_t)0) {  // just to skip the job
        return (int32_t)0;
    }
    if ((retVal = other.GetStatus(status)) != CCanApi::NoError) {
        initialized = false;
        started = false;
    } else {
        initialized = true;
        started = status.can_stopped ? false : true;
    }
    if (!initialized) {
        if ((retVal = other.InitializeChannel()) != CCanApi::NoError)
            return (int32_t)retVal;
    }
    if (!started) {
        if ((retVal = other.StartController()) != CCanApi::NoError) {
            if (!initialized)
                (void)other.TeardownChannel();
            return (int32_t)retVal;
        }
        PCBUSB_INIT_DELAY();
    }
    int32_t result = SendAndReceiveFrames(this, &other, frames);

    if (!started) {
        if ((retVal = other.ResetController()) != CCanApi::NoError) {
            if (!initialized)
                (void)other.TeardownChannel();
            return (int32_t)retVal;
        }
    }
    if (!initialized) {
        if ((retVal = other.TeardownChannel()) != CCanApi::NoError)
            return (int32_t)retVal;
    }
    return result;
}

int32_t CCanDevice::ReceiveSomeFrames(CCanDevice &other, int32_t frames) {
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    bool initialized = false;
    bool started = false;

    if (frames <= (int32_t)0) {  // just to skip the job
        return (int32_t)0;
    }
    if ((retVal = other.GetStatus(status)) != CCanApi::NoError) {
        initialized = false;
        started = false;
    } else {
        initialized = true;
        started = status.can_stopped ? false : true;
    }
    if (!initialized) {
        if ((retVal = other.InitializeChannel()) != CCanApi::NoError)
            return (int32_t)retVal;
    }
    if (!started) {
        if ((retVal = other.StartController()) != CCanApi::NoError) {
            if (!initialized)
                (void)other.TeardownChannel();
            return (int32_t)retVal;
        }
        PCBUSB_INIT_DELAY();
    }
    int32_t result = SendAndReceiveFrames(&other, this, frames);

    if (!started) {
        if ((retVal = other.ResetController()) != CCanApi::NoError) {
            if (!initialized)
                (void)other.TeardownChannel();
            return (int32_t)retVal;
        }
    }
    if (!initialized) {
        if ((retVal = other.TeardownChannel()) != CCanApi::NoError)
            return (int32_t)retVal;
    }
    return result;
}

int32_t CCanDevice::SendAndReceiveFrames(CCanDevice *sender, CCanDevice *receiver, int32_t frames) {
    CANAPI_Message_t txMessage = {};
    CANAPI_Message_t rxMessage = {};
    CANAPI_Return_t retVal;

    int32_t i, n = 0;
    int rxRetry = 0;

    txMessage.id = (sender == this) ? 0x100U : 0x200U;
#if (OPTION_CAN_2_0_ONLY == 0)
    txMessage.fdf = sender->m_OpMode.fdoe;
    txMessage.brs = sender->m_OpMode.brse;
    txMessage.esi = 0;
#endif
    txMessage.xtd = 0;
    txMessage.rtr = 0;
    txMessage.sts = 0;
    txMessage.dlc = 0U;

#if (DEVICE_DEBUG != 0)
    // time spec
    struct timespec t0 = {};
    struct timespec t1 = {};
    struct timespec t2 = {};
    struct timespec m0 = {};
    struct timespec m1 = {};
    // get current time: start of transmission
    t0 = CTimer::GetTime();
#endif
    // send the messages
    CProgress progress = CProgress(frames);
    for (i = 0; i < frames; i++) {
        // up-counting message content
        txMessage.data[0] = (uint8_t)((uint64_t)i >> 0); if ((uint64_t)i > (uint64_t)0x00) txMessage.dlc = 1U;
        txMessage.data[1] = (uint8_t)((uint64_t)i >> 8); if ((uint64_t)i > (uint64_t)0x00FF) txMessage.dlc = 2U;
        txMessage.data[2] = (uint8_t)((uint64_t)i >> 16); if ((uint64_t)i > (uint64_t)0x0FFFF) txMessage.dlc = 3U;
        txMessage.data[3] = (uint8_t)((uint64_t)i >> 24); if ((uint64_t)i > (uint64_t)0x0FFFFFF) txMessage.dlc = 4U;
        txMessage.data[4] = (uint8_t)((uint64_t)i >> 32); if ((uint64_t)i > (uint64_t)0x0FFFFFFFF) txMessage.dlc = 5U;
        txMessage.data[5] = (uint8_t)((uint64_t)i >> 40); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFF) txMessage.dlc = 6U;
        txMessage.data[6] = (uint8_t)((uint64_t)i >> 48); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFFFF) txMessage.dlc = 7U;
        txMessage.data[7] = (uint8_t)((uint64_t)i >> 56); if ((uint64_t)i > (uint64_t)0x0FFFFFFFFFFFFFF) txMessage.dlc = 8U;
        // send one message (retry if busy)
        do {
            retVal = sender->WriteMessage(txMessage, DEVICE_SEND_TIMEOUT);
            if (retVal == CCanApi::TransmitterBusy)
                PCBUSB_QXMT_DELAY();
        } while (retVal == CCanApi::TransmitterBusy);
        // on error abort
        if (retVal != CCanApi::NoError) {
            progress.Clear();
            return (int32_t)retVal;
        }
        PCBUSB_QXMT_DELAY();  // TODO: why here?
        // update number of transmitted CAN messages
        progress.Update((int)(i + 1), (int)n);
        // read one message from receiver's queue, if any
        retVal = receiver->ReadMessage(rxMessage, 0U);
        if (retVal == CCanApi::NoError) {
            // ignore status messages/error frames
            if (!rxMessage.sts) {
                // update number of received CAN messages
                progress.Update((int)(i + 1), (int)(n + 1));
                // check the message (id, length, up-counting number)
                retVal = CheckMessage(rxMessage, (uint64_t)n, txMessage.id);
                if (retVal != CCanApi::NoError) {
                    progress.Clear();
                    MESSAGE(n, rxMessage.id, rxMessage.dlc, rxMessage.data);
                    return (int32_t)retVal;
                }
#if (DEVICE_DEBUG != 0)
                if (n == 0)  // first message
                    m0 = rxMessage.timestamp;
#endif
                n++;
            }
        } else if (retVal != CCanApi::ReceiverEmpty) {
            if (++rxRetry > DEVICE_READ_RETRIES) {
                progress.Clear();
                return (int32_t)retVal;
            }
        }
    }
#if (DEVICE_DEBUG != 0)
    // get current time: end of transmission
    t1 = CTimer::GetTime();
#endif
    // finish the plate
    int32_t remaining = (int32_t)(frames - n);
#if (0) //defined(_WIN32) || defined(_WIN64)
    uint32_t timeout = ((uint64_t)remaining * DEVICE_LOOP_TIMEOUT * CTimer::MSEC);  // FIXME: does not work for all bit-rates
#else
    uint64_t timeout = (((uint64_t)TransmissionTime(receiver->GetBitrate(), (remaining + DEVICE_LOOP_EXTRA))
                     *   (uint64_t)DEVICE_LOOP_FACTOR) / (uint64_t)DEVICE_LOOP_DIVISOR);  // bit-rate dependent timeout
#endif
    if (n < frames) {
        CTimer timer = CTimer(timeout);
        while ((n < frames) && !timer.Timeout()) {
            // read one message from receiver's queue, if any
            retVal = receiver->ReadMessage(rxMessage, DEVICE_READ_TIMEOUT);
            if (retVal == CCanApi::NoError) {
                // ignore status messages/error frames
                if (!rxMessage.sts) {
                    // update number of received CAN messages
                    progress.Update((int)frames, (int)(n + 1));
                    // check the message (id, length, up-counting number)
                    retVal = CheckMessage(rxMessage, (uint64_t)n, txMessage.id);
                    if (retVal != CCanApi::NoError) {
                        progress.Clear();
                        MESSAGE(n, rxMessage.id, rxMessage.dlc, rxMessage.data);
                        return (int32_t)retVal;
                    }
#if (DEVICE_DEBUG != 0)
                    if (n == 0)  // first message
                        m0 = rxMessage.timestamp;
#endif
                    n++;
#if (DEVICE_DEBUG != 0)
                    if (n == frames)  // last message
                        m1 = rxMessage.timestamp;
#endif
                }
            }
            else if (retVal != CCanApi::ReceiverEmpty) {
                if (++rxRetry > DEVICE_READ_RETRIES) {
                    progress.Clear();
                    return (int32_t)retVal;
                }
            }
        }
        if (timer.Timeout()) {
            progress.Clear();
            return (int32_t)CANERR_TIMEOUT;
        }
    }
#if (DEVICE_DEBUG != 0)
    // get current time: end of reception
    t2 = CTimer::GetTime();
#endif
    // return the number of received messages
    progress.Clear();
#if (DEVICE_DEBUG != 0)
    if (frames > 0) {
        std::cout << "  " << frames << " total sent frames in " << ((float)CTimer::DiffTime(t0, t1) * 1000.f) << "ms";
        if (remaining > 0)
            std::cout << " + " << remaining << " remaining frames in " << ((float)CTimer::DiffTime(t1, t2) * 1000.f) << "ms w/ timeout " << ((float)timeout / 1000.f) << "ms";
        if (remaining > 1)
            std::cout << " : reception of " << n << " frames in " << ((float)CTimer::DiffTime(m0, m1) * 1000.f) << "ms";
        std::cout << std::endl;
    }
#endif
    return (int32_t)n;
}

int32_t CCanDevice::CheckMessage(CANAPI_Message_t &message, uint64_t expected, uint32_t id) {
    uint64_t received = 0U;
    uint8_t len = 0U;

    if (message.dlc > 0) received |= (uint64_t)message.data[0] << 0;
    if (message.dlc > 1) received |= (uint64_t)message.data[1] << 8;
    if (message.dlc > 2) received |= (uint64_t)message.data[2] << 16;
    if (message.dlc > 3) received |= (uint64_t)message.data[3] << 24;
    if (message.dlc > 4) received |= (uint64_t)message.data[4] << 32;
    if (message.dlc > 5) received |= (uint64_t)message.data[5] << 40;
    if (message.dlc > 6) received |= (uint64_t)message.data[6] << 48;
    if (message.dlc > 7) received |= (uint64_t)message.data[7] << 56;

    if (expected > (uint64_t)0x00) len = 1;
    if (expected > (uint64_t)0x00FF) len = 2;
    if (expected > (uint64_t)0x00FFFF) len = 3;
    if (expected > (uint64_t)0x00FFFFFF) len = 4;
    if (expected > (uint64_t)0x00FFFFFFFF) len = 5;
    if (expected > (uint64_t)0x00FFFFFFFFFF) len = 6;
    if (expected > (uint64_t)0x00FFFFFFFFFFFF) len = 7;
    if (expected > (uint64_t)0x00FFFFFFFFFFFFFF) len = 8;

    if (message.id != id)
        return (int32_t)ERROR_MSG_ID;
    if (message.dlc != len)
        return (int32_t)ERROR_MSG_DLC;
    if (expected < received)
        return (int32_t)ERROR_MSG_LOST;
    else if (expected > received)
        return (int32_t)ERROR_MSG_FATAL;

    return (int32_t)CANERR_NOERROR;
}

bool CCanDevice::CompareMessages(CANAPI_Message_t message1, CANAPI_Message_t message2, bool esiFlag) {
    if(message1.id != message2.id) return false;
    if(message1.xtd != message2.xtd) return false;
    if(message1.rtr != message2.rtr) return false;
    if(message1.sts != message2.sts) return false;
#if (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)
    if(message1.fdf != message2.fdf) return false;
    if(message1.brs != message2.brs) return false;
    if(message1.esi != message2.esi && esiFlag) return false;
#endif
    if(message1.dlc != message2.dlc) return false;
    if(memcmp(message1.data, message2.data, (size_t)CCanApi::Dlc2Len(message1.dlc))) return false;
    return true;
}

bool CCanDevice::CompareBitrates(CANAPI_Bitrate_t bitRate1, CANAPI_Bitrate_t bitRate2, bool dataPhase) {
    if ((bitRate1.index <= 0) && (bitRate2.index <= 0) && !dataPhase) {
        /* index to predefine bit-timing (acc. CiA CANopen spec.) 
         */
        return (bitRate1.index == bitRate2.index) ? true : false;
    } else {
        if (bitRate1.index <= 0) {
            if (MapIndex2Bitrate(bitRate1.index, bitRate1) != CCanApi::NoError)
                return false;
        }
        if (bitRate2.index <= 0) {
            if (MapIndex2Bitrate(bitRate2.index, bitRate2) != CCanApi::NoError)
                return false;
        }
        uint64_t speed1 = 0U, speed2 = 0U, sp1 = 0U, sp2 = 0U;
        /* nominal bit-rates (CAN 2.0 and CAN FD):
         *
         * (1) speed = freq / (brp * (1 + tseg1 + tseg2))
         *
         * (2) sp = (1 + tseg1) / (1 + tseg1 + tseg2)
         */
        if (bitRate1.btr.nominal.brp)
            speed1 = ((((uint64_t)bitRate1.btr.frequency * (uint64_t)10)
                   /   ((uint64_t)bitRate1.btr.nominal.brp * ((uint64_t)1 + (uint64_t)bitRate1.btr.nominal.tseg1 + (uint64_t)bitRate1.btr.nominal.tseg2))) + (uint64_t)5) / (uint64_t)10;
        else  // devide-by-zero
            speed1 = 0U;
        sp1 = (((((uint64_t)1 + (uint64_t)bitRate1.btr.nominal.tseg1) * (uint64_t)1000)
            /    ((uint64_t)1 + (uint64_t)bitRate1.btr.nominal.tseg1 + (uint64_t)bitRate1.btr.nominal.tseg2)) + (uint64_t)5) / (uint64_t)10;
        if (bitRate2.btr.nominal.brp)
            speed2 = ((((uint64_t)bitRate2.btr.frequency * (uint64_t)10)
                   /   ((uint64_t)bitRate2.btr.nominal.brp * ((uint64_t)1 + (uint64_t)bitRate2.btr.nominal.tseg1 + (uint64_t)bitRate2.btr.nominal.tseg2))) + (uint64_t)5) / (uint64_t)10;
        else  // devide-by-zero
            speed2 = 0U;
        sp2 = (((((uint64_t)1 + (uint64_t)bitRate2.btr.nominal.tseg1) * (uint64_t)1000)
            /    ((uint64_t)1 + (uint64_t)bitRate2.btr.nominal.tseg1 + (uint64_t)bitRate2.btr.nominal.tseg2)) + (uint64_t)5) / (uint64_t)10;
        // compare nominal bit-rates
        if ((speed1 != speed2) || (sp1 != sp2))
            return false;
#if (OPTION_CAN_2_0_ONLY == OPTION_DISABLED)
        if (dataPhase) {
            /* data phase bit-rate (CAN FD only):
             *
             * (1) speed = freq / (brp * (1 + tseg1 + tseg2))
             *
             * (2) sp = (1 + tseg1) / (1 + tseg1 + tseg2)
             */
            if (bitRate1.btr.data.brp)
                speed1 = ((((uint64_t)bitRate1.btr.frequency * (uint64_t)10)
                       /   ((uint64_t)bitRate1.btr.data.brp * ((uint64_t)1 + (uint64_t)bitRate1.btr.data.tseg1 + (uint64_t)bitRate1.btr.data.tseg2))) + (uint64_t)5) / (uint64_t)10;
            else  // devide-by-zero
                speed1 = 0U;
            sp1 = (((((uint64_t)1 + (uint64_t)bitRate1.btr.data.tseg1) * (uint64_t)1000)
                /    ((uint64_t)1 + (uint64_t)bitRate1.btr.data.tseg1 + (uint64_t)bitRate1.btr.data.tseg2)) + (uint64_t)5) / (uint64_t)10;
            if (bitRate2.btr.data.brp)
                speed2 = ((((uint64_t)bitRate2.btr.frequency * (uint64_t)10)
                       /   ((uint64_t)bitRate2.btr.data.brp * ((uint64_t)1 + (uint64_t)bitRate2.btr.data.tseg1 + (uint64_t)bitRate2.btr.data.tseg2))) + (uint64_t)5) / (uint64_t)10;
            else  // devide-by-zero
                speed2 = 0U;
            sp2 = (((((uint64_t)1 + (uint64_t)bitRate2.btr.data.tseg1) * (uint64_t)1000)
                /   ((uint64_t)1 + (uint64_t)bitRate2.btr.data.tseg1 + (uint64_t)bitRate2.btr.data.tseg2)) + (uint64_t)5) / (uint64_t)10;
            // compare nominal bit-rates
            if ((speed1 != speed2) || (sp1 != sp2))
                return false;
        }
#else
        // to avoid compiler warnings
        (void)dataPhase;
#endif
    }
    return true;
}

uint64_t CCanDevice::TransmissionTime(CANAPI_Bitrate_t bitRate, int32_t frames, uint8_t payload) {
    float time_per_bit = 100.f;  // assume the slowest bit-rate (10kbps)
    float bits_per_msg = 1.f + 11.f + 7.f + ((float)payload * 8.f) + 15.f + 1.f + 2.f + 7.f + 3.f;

    CANAPI_BusSpeed_t speed = {};
    if (CCanDevice::MapBitrate2Speed(bitRate, speed) == CCanApi::NoError)
        time_per_bit = 1000000.f / speed.nominal.speed;

    uint64_t usec = (uint64_t)((float)frames * bits_per_msg * time_per_bit);

    return (usec < 100U) ? 100U : usec;  // FIXME: CTimer::Delay calls Sleep(0) if t < 100us
}

void CCanDevice::ShowTimeDifference(const char *prefix, struct timespec &start, struct timespec &stop) {
    if (prefix)
        std::cout << prefix << ' ';
    std::cout << "dt=" << ((float)CTimer::DiffTime(start, stop) * 1000.f) << "ms" << std::endl;
}

void CCanDevice::ShowLibrayInformation(const char *prefix) {
    char *string = CCanDevice::GetVersion();
    if (prefix)
        std::cout << prefix << ' ';
    if (string)
        std::cout << string;
    else
        std::cout << "(no library information present)";
    std::cout << std::endl;
}

void CCanDevice::ShowDeviceInformation(const char *prefix) {
    CCanApi::SChannelInfo info;
    if (prefix)
        std::cout << prefix << ' ';
    if (GetChannelInfoFromDeviceList(info))
        std::cout << info.m_szDeviceName << " (library=" << info.m_nLibraryId << ", channel=" << info.m_nChannelNo << ", " << info.m_szDeviceDllName << ")";
    else
        std::cout << "(no device information present)";
    std::cout << std::endl;
}

void CCanDevice::ShowOperationMode(const char *prefix) {
    if (prefix)
        std::cout << prefix << ' ';
#if (OPTION_CAN_2_0_ONLY == 0)
    std::cout << "FDOE=" << (m_OpMode.fdoe ? 1 : 0) << ", ";
    std::cout << "BRSE=" << (m_OpMode.brse ? 1 : 0) << ", ";
    std::cout << "NISO=" << (m_OpMode.niso ? 1 : 0) << ", ";
#endif
    std::cout << "SHRD=" << (m_OpMode.shrd ? 1 : 0) << ", ";
    std::cout << "NXTD=" << (m_OpMode.nxtd ? 1 : 0) << ", ";
    std::cout << "NRTR=" << (m_OpMode.nrtr ? 1 : 0) << ", ";
    std::cout << "ERR=" << (m_OpMode.err ? 1 : 0) << ", ";
    std::cout << "MON=" << (m_OpMode.mon ? 1 : 0);
    std::cout << std::endl;
}

void CCanDevice::ShowBitrateSettings(const char *prefix) {
    CANAPI_BusSpeed_t speed = {};
    if (prefix)
        std::cout << prefix << ' ';
    if (MapBitrate2Speed(m_Bitrate, speed) == CCanApi::NoError) {
        std::cout << (speed.nominal.speed / 1000.0f) << "kbps@" << (speed.nominal.samplepoint * 100.0f) << "%";
#if (OPTION_CAN_2_0_ONLY == 0)
        if (m_OpMode.fdoe && m_OpMode.brse)
            std::cout << ":" << (speed.data.speed / 1000000.0f) << "Mbps@" << (speed.data.samplepoint * 100.0f) << "%";
        if (m_OpMode.fdoe)
            std::cout << " (CAN FD " << (m_OpMode.brse ? "with" : "w/o") << " BRSE)";
        else
            std::cout << " (CAN 2.0)";
#endif
    }
    else
        std::cout << "(invalid bit-rate settings)";
    std::cout << std::endl;
}

bool CCanDevice::GetChannelInfoFromDeviceList(CCanApi::SChannelInfo &info) {
    bool found = CCanDevice::GetFirstChannel(info);
    while (found) {
        if ((info.m_nLibraryId == m_nLibraryId) && (info.m_nChannelNo == m_nChannelNo)) {
            return true;
        }
        found = CCanDevice::GetNextChannel(info);
    }
    return false;
}

void CCanDevice::ShowChannelInformation(const char* prefix) {
    char szDeviceName[CANPROP_MAX_BUFFER_SIZE] = "";
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    bool bInitialized = true;
    if (prefix)
        std::cout << prefix << ' ';
    if (GetStatus(status) != CCanApi::NoError) {
        if ((retVal = InitializeChannel(m_nChannelNo, opMode)) != CCanApi::NoError) {
            std::cout << "error(" << retVal << "): channel " << m_nChannelNo << " could not be initialized" << std::endl;
            return;
        }
        bInitialized = false;
    }
    if ((retVal = GetProperty(CANPROP_GET_DEVICE_NAME, (void*)szDeviceName, CANPROP_MAX_BUFFER_SIZE)) != CCanApi::NoError) {
        std::cout << "error(" << retVal << "): device name could not be read" << std::endl;
        return;
    }
    if (!bInitialized) {
        if ((retVal = TeardownChannel()) != CCanApi::NoError) {
            std::cout << "error(" << retVal << "): channel " << m_nChannelNo << " could not be torn down" << std::endl;
            return;
        }
    }
    std::cout << szDeviceName << std::endl;
}

void CCanDevice::ShowChannelCapabilities(const char* prefix) {
    CANAPI_OpMode_t opMode = { CANMODE_DEFAULT };
    CANAPI_OpMode_t opCapa = {};
    CANAPI_Status_t status = {};
    CANAPI_Return_t retVal;
    bool bInitialized = true;
    if (prefix)
        std::cout << prefix << ' ';
    if (GetStatus(status) != CCanApi::NoError) {
        if ((retVal = InitializeChannel(m_nChannelNo, opMode)) != CCanApi::NoError) {
            std::cout << "error(" << retVal << "): channel " << m_nChannelNo << " could not be initialized" << std::endl;
            return;
        }
        bInitialized = false;
    }
    if ((retVal = GetProperty(CANPROP_GET_OP_CAPABILITY, (void*)&opCapa.byte, sizeof(uint8_t))) != CCanApi::NoError) {
        std::cout << "error(" << retVal << "): channel capabilities could not be read" << std::endl;
        return;
    }
    if (!bInitialized) {
        if ((retVal = TeardownChannel()) != CCanApi::NoError) {
            std::cout << "error(" << retVal << "): channel " << m_nChannelNo << " could not be torn down" << std::endl;
            return;
        }
    }
#if (OPTION_CAN_2_0_ONLY == 0)
    std::cout << "FDOE=" << (opCapa.fdoe ? 1 : 0) << ", ";
    std::cout << "BRSE=" << (opCapa.brse ? 1 : 0) << ", ";
    std::cout << "NISO=" << (opCapa.niso ? 1 : 0) << ", ";
#endif
    std::cout << "SHRD=" << (opCapa.shrd ? 1 : 0) << ", ";
    std::cout << "NXTD=" << (opCapa.nxtd ? 1 : 0) << ", ";
    std::cout << "NRTR=" << (opCapa.nrtr ? 1 : 0) << ", ";
    std::cout << "ERR=" << (opCapa.err ? 1 : 0) << ", ";
    std::cout << "MON=" << (opCapa.mon ? 1 : 0);
    std::cout << std::endl;
}

// $Id: Device.cpp 1217 2023-10-10 19:28:31Z haumea $  Copyright (c) UV Software, Berlin //
