//  SPDX-License-Identifier: GPL-3.0-or-later
//
//  CAN Monitor for Kvaser USB CAN Interfaces (CAN API V3)
//
//  Copyright (c) 2007,2012-2024 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

#include "KvaserCAN.h"

#if (OPTION_CAN_2_0_ONLY != 0)
#error Compilation with legacy CAN 2.0 frame format!
#else
#define CAN_FD_SUPPORTED  1  // don't touch that dial
#endif
#define MONITOR_INTERFACE "Kvaser USB CAN Interfaces"
#define MONITOR_COPYRIGHT "2007,2012-2024 by Uwe Vogt, UV Software, Berlin"
#define MONITOR_PLATFORM  "Darwin"
#define MONITOR_ALIASNAME "KVL:"

#define BITRATE_1M(x)    KVASER_CAN_BR_1M(x)
#define BITRATE_800K(x)  KVASER_CAN_BR_800K(x)
#define BITRATE_500K(x)  KVASER_CAN_BR_500K(x)
#define BITRATE_250K(x)  KVASER_CAN_BR_250K(x)
#define BITRATE_125K(x)  KVASER_CAN_BR_125K(x)
#define BITRATE_100K(x)  KVASER_CAN_BR_100K(x)
#define BITRATE_50K(x)   KVASER_CAN_BR_50K(x)
#define BITRATE_20K(x)   KVASER_CAN_BR_20K(x)
#define BITRATE_10K(x)   KVASER_CAN_BR_10K(x)
#define BITRATE_5K(x)    KVASER_CAN_BR_5K(x)

#if (CAN_FD_SUPPORTED != 0)
#define BITRATE_FD_1M(x)      KVASER_CAN_FD_BR_1M(x)
#define BITRATE_FD_500K(x)    KVASER_CAN_FD_BR_500K(x)
#define BITRATE_FD_250K(x)    KVASER_CAN_FD_BR_250K(x)
#define BITRATE_FD_125K(x)    KVASER_CAN_FD_BR_125K(x)
#define BITRATE_FD_1M8M(x)    KVASER_CAN_FD_BR_1M8M(x)
#define BITRATE_FD_500K4M(x)  KVASER_CAN_FD_BR_500K4M(x)
#define BITRATE_FD_250K2M(x)  KVASER_CAN_FD_BR_250K2M(x)
#define BITRATE_FD_125K1M(x)  KVASER_CAN_FD_BR_125K1M(x)
#endif

typedef CKvaserCAN  CCanDriver;

#endif // DRIVER_H_INCLUDED
