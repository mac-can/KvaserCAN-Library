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
#ifdef _MSC_VER
//no Microsoft extensions please!
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif
#include "pch.h"
#include <string.h>
#include <iostream>

#define OPTION_HELP              "--can_help"
#define OPTION_PATH              "--can_path"
#define OPTION_DUT1              "--can_dut1"
#define OPTION_DUT2              "--can_dut2"
#define OPTION_MODE              "--can_mode"
#define OPTION_BAUD              "--can_baudrate"
#define OPTION_BITRATE           "--can_bitrate"
#define OPTION_TEST_FRAMES       "--test_frames"
#define OPTION_SMOKETEST_FRAMES  "--smoketest_frames"
#define OPTION_LIST_DEVICES      "--list_devices"
#define OPTION_CAN_CLASSIC       "--can_classic"
#define OPTION_3RD_DEVICE        "--3rd_device"
#define OPTION_RTR_DEVICE        "--rtr_device"
#define OPTION_RUN_ALL           "--run_all"
#define OPTION_RUN_QUICK         "--run_quick"
#define OPTION_RUN_CALLSEQUENCES "--run_callsequences"

static const bool c_fCanClassic = false;
static const bool c_f3rdDevice = false;
static const bool c_fRtrDevice = false;
static const bool c_fRunQuick = false;
static const bool c_fCallSequences = false;
static const bool c_fBitrateConverter = false;

COptions g_Options = COptions();

COptions::COptions() {
    // device under test #1
    if (!GetChannelInfoFromDeviceList(TEST_LIBRARY, TEST_CHANNEL1, m_Dut[DUT1].m_Info)) {
        m_Dut[DUT1].m_Info.m_nLibraryId = INVALID_HANDLE;
        m_Dut[DUT1].m_Info.m_nChannelNo = INVALID_HANDLE;
    }
    m_Dut[DUT1].m_OpMode = m_DefaultOpMode;
    m_Dut[DUT1].m_Bitrate = m_DefaultBitrate;
    m_Dut[DUT1].m_pParameter = NULL;
    // device under test #2
    if (!GetChannelInfoFromDeviceList(TEST_LIBRARY, TEST_CHANNEL2, m_Dut[DUT2].m_Info)) {
        m_Dut[DUT2].m_Info.m_nLibraryId = INVALID_HANDLE;
        m_Dut[DUT2].m_Info.m_nChannelNo = INVALID_HANDLE;
    }
    m_Dut[DUT2].m_OpMode = m_DefaultOpMode;
    m_Dut[DUT2].m_Bitrate = m_DefaultBitrate;
    m_Dut[DUT2].m_pParameter = NULL;
    // common settings
    // TODO: ...
    // test options
    m_s32TestFrames = (int32_t)TEST_FRAMES;
    m_s32SmokeTestFrames = (int32_t)TEST_TRAFFIC;
    m_fCallSequences = c_fCallSequences;
    m_fBitrateConverter = c_fBitrateConverter;
    m_fCanClassic = c_fCanClassic;
    m_f3rdDevice = c_f3rdDevice;
    m_fRtrDevice = c_fRtrDevice;
    m_fRunQuick = c_fRunQuick;
    m_fShowHelp = false;
}

bool COptions::GetChannelInfoByDeviceName(const char *name, CCanApi::SChannelInfo &info) {
    bool found = CCanDevice::GetFirstChannel(info);
    while (found) {
        if (!strcmp(info.m_szDeviceName, name)) {
            return true;
        }
        found = CCanDevice::GetNextChannel(info);
    }
    return false;
}

bool COptions::GetChannelInfoFromDeviceList(int32_t library, int32_t channel, CCanApi::SChannelInfo &info) {
    bool found = CCanDevice::GetFirstChannel(info);
    while (found) {
        if ((info.m_nLibraryId == library) && (info.m_nChannelNo == channel)) {
            return true;
        }
        found = CCanDevice::GetNextChannel(info);
    }
    return false;
}

int COptions::ScanOptions(int argc, char* argv[], char* err, size_t len) {
#if (OPTION_CANAPI_LIBRARY != 0)
    int opt_path = 0;
#endif
    int opt_dut1 = 0;
    int opt_dut2 = 0;
    int opt_baud = 0;
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    int opt_mode = 0;
    int opt_bitrate = 0;
#endif
    int test_frames = 0;
    int smoketest_frames = 0;
    int can_classic = 0;
    int can_device = 0;
    int rtr_device = 0;
    int run_callsequences = 0;
    int run_quick = 0;
    char* opt = NULL;
    //char *arg = NULL;
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
    bool has_data = false;
    bool has_sam= false;
#endif
    long baudrate = 0;
    long frames = 0;

    for (int i = 1; i < argc; i++) {
        // option: --can_help [to be scanned first]
        if (strcmp(argv[i], OPTION_HELP) == 0) {
            // to show the help screen only!
            m_fShowHelp = true;
            return true;
        }
        // option: --can_path[=<pathname>]
#if (OPTION_CANAPI_LIBRARY != 0)
        else if (strncmp(argv[i], OPTION_PATH, strlen(OPTION_PATH)) == 0)
        {
            if (opt_path++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_PATH);
                return false;
            }
            if (opt_dut1 || opt_dut2) {
                if (err)
                    snprintf(err, len, "option %s must be specified before option %s and %s", OPTION_PATH, OPTION_DUT1, OPTION_DUT2);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_PATH)] == '='))
            {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_PATH);
                    return false;
                }
            }
            else {
                opt = (char*)TEST_SEARCH_PATH;
            }
            CCanDriver *dummy = new CCanDriver();
            if (dummy->SetProperty(CANPROP_SET_SEARCH_PATH, (void*)opt, 0) != CCanApi::NoError) {
                if (err)
                    snprintf(err, len, "search path not set --  %s", opt);
                delete dummy;
                return false;
            }
            std::cout << "JSON path: " << opt << std::endl;
            delete dummy;
        }
#endif
        // option: --can_dut1=<interface>
        else if (strncmp(argv[i], OPTION_DUT1, strlen(OPTION_DUT1)) == 0)
        {
            if (opt_dut1++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_DUT1);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_DUT1)] == '='))
            {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_DUT1);
                    return false;
                }
                if (!GetChannelInfoByDeviceName(opt, m_Dut[DUT1].m_Info)) {
                    if (err)
                        snprintf(err, len, "unknown device \'%s\' with option %s", opt, OPTION_DUT1);
                    return false;
                }
            }
            else {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_DUT1);
                return false;
            }
        }
        // option: --can_dut2=<interface>
        else if (strncmp(argv[i], OPTION_DUT2, strlen(OPTION_DUT2)) == 0)
        {
            if (opt_dut2++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_DUT2);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_DUT2)] == '='))
            {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_DUT2);
                    return false;
                }
                if (!GetChannelInfoByDeviceName(opt, m_Dut[DUT2].m_Info)) {
                    if (err)
                        snprintf(err, len, "unknown device \'%s\' with option %s", opt, OPTION_DUT2);
                    return false;
                }
            }
            else {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_DUT2);
                return false;
            }
        }
        // option: --can_mode=(CAN2.0|CANFD[+BRS])
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
        else if (strncmp(argv[i], OPTION_MODE, strlen(OPTION_MODE)) == 0)
        {
            if (opt_mode++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_MODE);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_MODE)] == '='))
            {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_MODE);
                    return false;
                }
                if (!strcasecmp(opt, "default") ||
                    !strcasecmp(opt, "classic") ||
                    !strcasecmp(opt, "CAN2.0") ||
                    !strcasecmp(opt, "CAN20") ||
                    !strcasecmp(opt, "2.0")) {
                    m_Dut[DUT1].m_OpMode.fdoe = m_Dut[DUT2].m_OpMode.fdoe = 0;
                    m_Dut[DUT1].m_OpMode.brse = m_Dut[DUT2].m_OpMode.brse = 0;
                }
                else if (!strcasecmp(opt, "CANFD") ||
                    !strcasecmp(opt, "FD") ||
                    !strcasecmp(opt, "FDF")) {
                    m_Dut[DUT1].m_OpMode.fdoe = m_Dut[DUT2].m_OpMode.fdoe = 1;
                    m_Dut[DUT1].m_OpMode.brse = m_Dut[DUT2].m_OpMode.brse = 0;
                }
                else if (!strcasecmp(opt, "CANFD+BRS") ||
                    !strcasecmp(opt, "FDF+BRS") ||
                    !strcasecmp(opt, "FD+BRS")) {
                    m_Dut[DUT1].m_OpMode.fdoe = m_Dut[DUT2].m_OpMode.fdoe = 1;
                    m_Dut[DUT1].m_OpMode.brse = m_Dut[DUT2].m_OpMode.brse = 1;
                }
                else {
                    if (err)
                        snprintf(err, len, "illegal argument in option %s", OPTION_MODE);
                    return false;
                }
            }
            else {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_MODE);
                return false;
            }
        }
#endif
        // option: --can_baudrate=(<baudrate>|DEFAULT|FAST|SLOW)
        else if (strncmp(argv[i], OPTION_BAUD, strlen(OPTION_BAUD)) == 0)
        {
            if (opt_baud++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_BAUD);
                return false;
            }
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
            if (opt_bitrate) {
                if (err)
                    snprintf(err, len, "option %s conflicts with option %s", OPTION_BAUD, OPTION_BITRATE);
                return false;
            }
#endif
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_BAUD)] == '='))
            {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_BAUD);
                    return false;
                }
                if (!strcasecmp(opt, "default")) {
                    m_Dut[DUT1].m_Bitrate.index = TEST_BTRINDEX;
                    m_Dut[DUT2].m_Bitrate.index = TEST_BTRINDEX;
                }
                else if (!strcasecmp(opt, "fast")) {
                    m_Dut[DUT1].m_Bitrate.index = FAST_BTRINDEX;
                    m_Dut[DUT2].m_Bitrate.index = FAST_BTRINDEX;
                }
                else if (!strcasecmp(opt, "slow")) {
                    m_Dut[DUT1].m_Bitrate.index = SLOW_BTRINDEX;
                    m_Dut[DUT2].m_Bitrate.index = SLOW_BTRINDEX;
                }
                else if (sscanf(opt, "%li", &baudrate) == 1) {
                    switch (baudrate) {
                    case 1000: case 1000000: baudrate = CANBDR_1000; break;
                    case 800:  case 800000:  baudrate = CANBDR_800; break;
                    case 500:  case 500000:  baudrate = CANBDR_500; break;
                    case 250:  case 250000:  baudrate = CANBDR_250; break;
                    case 125:  case 125000:  baudrate = CANBDR_125; break;
                    case 100:  case 100000:  baudrate = CANBDR_100; break;
                    case 50:   case 50000:   baudrate = CANBDR_50; break;
                    case 20:   case 20000:   baudrate = CANBDR_20; break;
                    case 10:   case 10000:   baudrate = CANBDR_10; break;
                    }
                    if ((baudrate < CANBDR_1000) || (CANBDR_10 < baudrate)) {
                        snprintf(err, len, "illegal argument in option %s", OPTION_BAUD);
                        return false;
                    }
                    m_Dut[DUT1].m_Bitrate.index = (int32_t)baudrate * (-1);
                    m_Dut[DUT2].m_Bitrate.index = (int32_t)baudrate * (-1);
                }
                else {
                    snprintf(err, len, "illegal argument in option %s", OPTION_BAUD);
                    return false;
                }
            }
            else {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_BAUD);
                return false;
            }
        }
        // option: --can_bitrate=(<bitrate>|DEFAULT|FAST|SLOW))
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
        else if (strncmp(argv[i], OPTION_BITRATE, strlen(OPTION_BITRATE)) == 0)
        {
            if (opt_bitrate++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_BITRATE);
                return false;
            }
            if (opt_baud) {
                if (err)
                    snprintf(err, len, "option %s conflicts with option %s", OPTION_BITRATE, OPTION_BAUD);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_BITRATE)] == '='))
            {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_BITRATE);
                    return false;
                }
                if (!strcasecmp(opt, "default")) {
                    TEST_BITRATE_FD(m_Dut[DUT1].m_Bitrate);
                    TEST_BITRATE_FD(m_Dut[DUT2].m_Bitrate);
                }
                else if (!strcasecmp(opt, "fast")) {
                    FAST_BITRATE_FD(m_Dut[DUT1].m_Bitrate);
                    FAST_BITRATE_FD(m_Dut[DUT2].m_Bitrate);
                }
                else if (!strcasecmp(opt, "slow")) {
                    SLOW_BITRATE_FD(m_Dut[DUT1].m_Bitrate);
                    SLOW_BITRATE_FD(m_Dut[DUT2].m_Bitrate);
                }
                else if (CCanDevice::MapString2Bitrate(opt, m_Dut[DUT1].m_Bitrate, has_data, has_sam) == CCanApi::NoError) {
                    m_Dut[DUT2].m_Bitrate = m_Dut[DUT1].m_Bitrate;
                }
                else {
                    snprintf(err, len, "illegal argument in option %s", OPTION_BITRATE);
                    return false;
                }
            }
            else {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_BITRATE);
                return false;
            }
        }
#endif
        // option: --test_frames=<frames>
        else if (strncmp(argv[i], OPTION_TEST_FRAMES, strlen(OPTION_TEST_FRAMES)) == 0)
        {
            if (test_frames++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_TEST_FRAMES);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) == NULL) ||
                (argv[i][strlen(OPTION_TEST_FRAMES)] != '=')) {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_TEST_FRAMES);
                return false;
            }
            if (strlen(++opt) == 0) {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_TEST_FRAMES);
                return false;
            }
            if (sscanf(opt, "%li", &frames) != 1) {
                snprintf(err, len, "illegal argument in option %s", OPTION_TEST_FRAMES);
                return false;
            }
            if (frames >= 0)
                m_s32TestFrames = (int32_t)frames;
            else
                m_s32TestFrames = (int32_t)TEST_FRAMES;
        }
        // option: --smoketest_frames=<frames>
        else if (strncmp(argv[i], OPTION_SMOKETEST_FRAMES, strlen(OPTION_SMOKETEST_FRAMES)) == 0)
        {
            if (smoketest_frames++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_SMOKETEST_FRAMES);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) == NULL) ||
                (argv[i][strlen(OPTION_SMOKETEST_FRAMES)] != '=')) {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_SMOKETEST_FRAMES);
                return false;
            }
            if (strlen(++opt) == 0) {
                if (err)
                    snprintf(err, len, "missing argument for option %s", OPTION_SMOKETEST_FRAMES);
                return false;
            }
            if (sscanf(opt, "%li", &frames) != 1) {
                snprintf(err, len, "illegal argument in option %s", OPTION_SMOKETEST_FRAMES);
                return false;
            }
            if (frames >= 0)
                m_s32SmokeTestFrames = (int32_t)frames;
            else
                m_s32SmokeTestFrames = (int32_t)TEST_TRAFFIC;
        }
        // option: --can_classic[=(YES|NO)]
        else if (strncmp(argv[i], OPTION_CAN_CLASSIC, strlen(OPTION_CAN_CLASSIC)) == 0)
        {
            if (can_classic++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_CAN_CLASSIC);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_CAN_CLASSIC)] == '=')) {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_CAN_CLASSIC);
                    return false;
                }
                if ((strcasecmp(opt, "YES") == 0) || (strcasecmp(opt, "ON") == 0) ||
                    (strcasecmp(opt, "Y") == 0) || (strcasecmp(opt, "1") == 0))
                    m_fCanClassic = true;
                else
                    m_fCanClassic = false;
            }
            else
                m_fCanClassic = true;
        }
        // option: --3rd_device[=(YES|NO)]
        else if (strncmp(argv[i], OPTION_3RD_DEVICE, strlen(OPTION_3RD_DEVICE)) == 0)
        {
            if (can_device++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_3RD_DEVICE);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_3RD_DEVICE)] == '=')) {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_3RD_DEVICE);
                    return false;
                }
                if ((strcasecmp(opt, "YES") == 0) || (strcasecmp(opt, "ON") == 0) ||
                    (strcasecmp(opt, "Y") == 0) || (strcasecmp(opt, "1") == 0))
                    m_f3rdDevice = true;
                else
                    m_f3rdDevice = false;
            }
            else
                m_f3rdDevice = true;
        }
        // option: --rtr_device[=(YES|NO)]
        else if (strncmp(argv[i], OPTION_RTR_DEVICE, strlen(OPTION_RTR_DEVICE)) == 0)
        {
            if (rtr_device++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_RTR_DEVICE);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_RTR_DEVICE)] == '=')) {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_RTR_DEVICE);
                    return false;
                }
                if ((strcasecmp(opt, "YES") == 0) || (strcasecmp(opt, "ON") == 0) ||
                    (strcasecmp(opt, "Y") == 0) || (strcasecmp(opt, "1") == 0))
                    m_fRtrDevice = m_f3rdDevice = m_fCanClassic = true;
                else
                    m_fRtrDevice = false;
            }
            else
                m_fRtrDevice = m_f3rdDevice = m_fCanClassic = true;
        }
        // option: --run_all[=(YES|NO)]
        else if (strncmp(argv[i], OPTION_RUN_ALL, strlen(OPTION_RUN_ALL)) == 0)
        {
            if (run_callsequences++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_RUN_ALL);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_RUN_ALL)] == '=')) {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_RUN_ALL);
                    return false;
                }
                if ((strcasecmp(opt, "YES") == 0) || (strcasecmp(opt, "ON") == 0) ||
                    (strcasecmp(opt, "Y") == 0) || (strcasecmp(opt, "1") == 0))
                    m_fCallSequences = m_fBitrateConverter = true;  // TODO: add further test to run with OPTION_RUN_ALL 
                else
                    m_fCallSequences = m_fBitrateConverter = false;  // TODO: add further test to run with OPTION_RUN_ALL
            }
            else
                m_fCallSequences = m_fBitrateConverter = true;  // TODO: add further test to run with OPTION_RUN_ALL
        }
        // option: --run_callsequences[=(YES|NO)]
        else if (strncmp(argv[i], OPTION_RUN_CALLSEQUENCES, strlen(OPTION_RUN_CALLSEQUENCES)) == 0)
        {
            if (run_callsequences++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_RUN_CALLSEQUENCES);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_RUN_CALLSEQUENCES)] == '=')) {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_RUN_CALLSEQUENCES);
                    return false;
                }
                if ((strcasecmp(opt, "YES") == 0) || (strcasecmp(opt, "ON") == 0) ||
                    (strcasecmp(opt, "Y") == 0) || (strcasecmp(opt, "1") == 0))
                    m_fCallSequences = true;
                else
                    m_fCallSequences = false;
            }
            else
                m_fCallSequences = true;
        }
        // option: --run_quick[=(YES|NO)]
        else if (strncmp(argv[i], OPTION_RUN_QUICK, strlen(OPTION_RUN_QUICK)) == 0)
        {
            if (run_quick++) {
                if (err)
                    snprintf(err, len, "duplicated option %s", OPTION_RUN_QUICK);
                return false;
            }
            if (((opt = strchr(argv[i], '=')) != NULL) &&
                (argv[i][strlen(OPTION_RUN_QUICK)] == '=')) {
                if (strlen(++opt) == 0) {
                    if (err)
                        snprintf(err, len, "missing argument for option %s", OPTION_RUN_QUICK);
                    return false;
                }
                if ((strcasecmp(opt, "YES") == 0) || (strcasecmp(opt, "ON") == 0) ||
                    (strcasecmp(opt, "Y") == 0) || (strcasecmp(opt, "1") == 0))
                    m_fRunQuick = true;
                else
                    m_fRunQuick = false;
            }
            else
                m_fRunQuick = true;
        }
        // unknown option (note: GoogleTest does not eat option '--help')
        else if (strcmp(argv[i], "--help") != 0) {
            if (err)
                snprintf(err, len, "unkown option %s", argv[i]);
            return false;
        }
    }
    return true;
}

int COptions::ShowHelp() {
    if (m_fShowHelp) {
        std::cout << std::endl;
        std::cout << "Test Options:" << std::endl;
        std::cout << "  " << OPTION_DUT1 << "=<interface>" <<  std::endl;
        std::cout << "  " << OPTION_DUT2 << "=<interface>" << std::endl;
        std::cout << "      Name of the CAN device under test." <<  std::endl;
#if (OPTION_CANAPI_LIBRARY != 0)
        std::cout << "  " << OPTION_PATH << "=<pathname>" <<  std::endl;
        std::cout << "      Search path for JSON files (default is current directory)." << std::endl;
        std::cout << "      This option must be given before option " << OPTION_DUT1 << " and " << OPTION_DUT2 << "." << std::endl;
#endif
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
        std::cout << "  " << OPTION_MODE << "=(CAN2.0|CANFD[+BRS])" << std::endl;
        std::cout << "      CAN operation mode: CAN 2.0 or CAN FD format." <<  std::endl;
        std::cout << "              CAN2.0    = CAN classic (default)" << std::endl;
        std::cout << "              CANFD     = CAN FD long frames only" << std::endl;
        std::cout << "              CANFD+BRS = CAN FD long and fast frames" << std::endl;
#endif
        std::cout << "  " << OPTION_BAUD << "=(<baudrate>|DEFAULT|FAST|SLOW)" <<  std::endl;
        std::cout << "      CAN bit-timing as CiA index (default=" << -TEST_BTRINDEX << "):" <<  std::endl;
        std::cout << "              0 = 1000 kBit/s" <<  std::endl;
        std::cout << "              1 = 800 kBit/s" <<  std::endl;
        std::cout << "              2 = 500 kBit/s" <<  std::endl;
        std::cout << "              3 = 250 kBit/s" <<  std::endl;
        std::cout << "              4 = 125 kBit/s" <<  std::endl;
        std::cout << "              5 = 100 kBit/s" <<  std::endl;
        std::cout << "              6 = 50 kBit/s" <<  std::endl;
        std::cout << "              7 = 20 kBit/s" <<  std::endl;
        std::cout << "              8 = 10 kBit/s" <<  std::endl;
#if (CAN_FD_SUPPORTED == FEATURE_SUPPORTED)
        std::cout << "  " << OPTION_BITRATE << "=(<bitrate>|DEFAULT|FAST|SLOW)" <<  std::endl;
        std::cout << "      CAN bit-rate as a comma-separated <key>=<value>-list:" <<  std::endl;
        std::cout << "              f_clock=<value>      Frequency in Hz or" <<  std::endl;
        std::cout << "              f_clock_mhz=<value>  Frequency in MHz" <<  std::endl;
        std::cout << "              nom_brp=<value>      Bit-rate prescaler (nominal)" <<  std::endl;
        std::cout << "              nom_tseg1=<value>    Time segment 1 (nominal)" <<  std::endl;
        std::cout << "              nom_tseg2=<value>    Time segment 2 (nominal)" <<  std::endl;
        std::cout << "              nom_sjw=<value>      Sync. jump width (nominal)" << std::endl;
        std::cout << "              nom_sam=<value>      Number of samples (optional)" << std::endl;
        std::cout << "              data_brp=<value>     Bit-rate prescaler (data phase)" <<  std::endl;
        std::cout << "              data_tseg1=<value>   Time segment 1 (data phase)" <<  std::endl;
        std::cout << "              data_tseg2=<value>   Time segment 2 (data phase)" <<  std::endl;
        std::cout << "              data_sjw=<value>     Sync. jump width (data phase)." <<  std::endl;
        char string[256] = "";
        CANAPI_BusSpeed_t speed;
        CANAPI_Bitrate_t bitrate;
        TEST_BITRATE_FD(bitrate);
        CCanDevice::MapBitrate2Speed(bitrate, speed);
        CCanDevice::MapBitrate2String(bitrate, string, 256, true, false);
        std::cout << "      Default CAN FD bit-rate is nominal " << (speed.nominal.speed / 1000.0f) << "kbps@"
                                                                 << (speed.nominal.samplepoint * 100.0f) << "% " 
                                            << "and data phase " << (speed.data.speed / 1000000.0f) << "Mbps@"
                                                                 << (speed.data.samplepoint * 100.0f) << "%:" << std::endl;
        std::cout << "              " <<  string << std::endl;
#endif
        std::cout << "  " << OPTION_TEST_FRAMES << "=<frames>" << std::endl;
        std::cout << "      Number of frames to be sent during normal tests (default=" << TEST_FRAMES << ")." << std::endl;
        std::cout << "  " << OPTION_SMOKETEST_FRAMES << "=<frames>" << std::endl;
        std::cout << "      Number of frames to be sent during smoke test (default=" << TEST_TRAFFIC << ")." << std::endl;
        std::cout << "  " << OPTION_CAN_CLASSIC << "[=(NO|YES)]" << std::endl;
        std::cout << "      Disables or enables the execution of test cases in CAN FD operation mode (default=" << (c_fCanClassic ? "yes" : "no") << ")." << std::endl;
        std::cout << "  " << OPTION_3RD_DEVICE << "[=(NO|YES)]" << std::endl;
        std::cout << "      Disables or enables the execution of test cases with non-default bit-rate settings (default=" << (c_f3rdDevice ? "yes" : "no") << ")." << std::endl;
        std::cout << "  " << OPTION_RTR_DEVICE << "[=(NO|YES)]" << std::endl;
        std::cout << "      Enables or disables the execution of test cases for which a RTR answering device is required (default=" << (c_fRtrDevice ? "yes" : "no") << ")." << std::endl;
        std::cout << "  " << OPTION_RUN_CALLSEQUENCES << "[=(NO|YES)]" << std::endl;
        std::cout << "      Enables or disables the execution of test cases from suite 'CallSequences' (default=" << (c_fCallSequences ? "yes" : "no") << ")." << std::endl;
        std::cout << "  " << OPTION_RUN_ALL << "[=(NO|YES)]" << std::endl;
        std::cout << "      Enables or disables the execution all optional tests (default=no)." << std::endl;
        std::cout << "  " << OPTION_RUN_QUICK << "[=(NO|YES)]" << std::endl;
        std::cout << "      Disables or enables the execution of long lasting test cases (default=" << (c_fRunQuick ? "yes" : "no") << ")." << std::endl;
        std::cout << std::endl;
        std::cout << "Hazard Note:" << std::endl;
        std::cout << "  If you connect your CAN device to a real CAN network when using this program," << std::endl;
        std::cout << "  you might damage your application." << std::endl;
        std::cout << std::endl;
    }
    return m_fShowHelp;
}

// $Id: Options.cpp 1193 2023-09-06 10:21:35Z haumea $  Copyright (c) UV Software, Berlin //
