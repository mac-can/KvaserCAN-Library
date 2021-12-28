//
//  KvaserCAN - macOS User-Space Driver for Kvaser CAN Leaf Interfaces
//
//  Copyright (C) 2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//  All rights reserved.
//
//  This file is part of MacCAN-KvaserCAN.
//
//  MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License and
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
//  MacCAN-KvaserCAN IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF MacCAN-KvaserCAN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
//  MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MacCAN-KvaserCAN is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MacCAN-KvaserCAN.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef TEMPLATE_H_INCLUDED
#define TEMPLATE_H_INCLUDED

//  A Template for CAN API V3 Testing
//
//  (§1) include the header file of the CAN API V3 C++ class of the CAN driver
#include "KvaserCAN.h"

//  (§2) define macro CDriverCAN with the class name of the CAN driver
#define CDriverCAN  CKvaserCAN

//  (§3) define macro CAN_DEVICE1 and CAN_DEVICE2 with a valid CAN channel no.
#define CAN_DEVICE1  KVASER_CAN_CHANNEL0
#define CAN_DEVICE2  KVASER_CAN_CHANNEL1

//  (§4) define macros for CAN Classic bit-rate settings (at least BITRATE_1M, BITRATE_500K, BITRATE_250K, BITRATE_125K)
#define BITRATE_1M(x)       do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 31; x.btr.nominal.tseg2= 8; x.btr.nominal.sjw= 8; } while(0)
#define BITRATE_500K(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 63; x.btr.nominal.tseg2=16; x.btr.nominal.sjw= 8; } while(0)
#define BITRATE_250K(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; } while(0)
#define BITRATE_125K(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=4; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; } while(0)

//  ($5) define macros for unsupported features in CAN Classic operation mode (at least BITRATE_800K_UNSUPPORTED, ..)
#define BITRATE_800K_UNSUPPORTED  0
#define BITRATE_5K_UNSUPPORTED  0
#define TX_ACKNOWLEDGE_UNSUPPORTED  0

//  (§6) define macros for workarounds in CAN Classic operation mode (e.g. TC01_3_WORKARAOUND)
//#define TX0x_y_WORKARAOUND  1

//  (§7) define macro CAN_FD_SUPPORTED when CAN FD operation mode is supported
#define CAN_FD_SUPPORTED 1
#if (CAN_FD_SUPPORTED != 0)

//  (§8) define macros for CAN Classic bit-rate settings (at least BITRATE_1M8M, BITRATE_500K4M, BITRATE_250K2M, BITRATE_125K1M)
#define BITRATE_1M8M(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 31; x.btr.nominal.tseg2= 8; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1= 3; x.btr.data.tseg2=1; x.btr.data.sjw=1; } while(0)
#define BITRATE_500K4M(x)   do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 63; x.btr.nominal.tseg2=16; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1= 7; x.btr.data.tseg2=2; x.btr.data.sjw=2; } while(0)
#define BITRATE_250K2M(x)   do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1=15; x.btr.data.tseg2=4; x.btr.data.sjw=2; } while(0)
#define BITRATE_125K1M(x)   do{ x.btr.frequency=80000000; x.btr.nominal.brp=4; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1=31; x.btr.data.tseg2=8; x.btr.data.sjw=2; } while(0)

//  ($9) define macros for unsupported features in CAN Classic operation mode
//#define BITRATE_SWITCHING_UNSUPPORTED  0

//  (§10) define macros for workarounds in CAN Classic operation mode (e.g. TC01_3_WORKARAOUND)
//#define TX0x_y_WORKARAOUND  1

#endif // CAN_FD_SUPPORTED
#endif // TEMPLATE_H_INCLUDED

// $Id: Template.h 1035 2021-12-21 12:03:27Z makemake $  Copyright (c) UV Software, Berlin //
