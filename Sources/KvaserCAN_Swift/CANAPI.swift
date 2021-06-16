//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  CAN Interface API, Version 3 (Interface Definition)
//
//  Copyright (C) 2004-2021 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
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
/*!
    CAN API V3 Swift Wrapper for generic CAN Interfaces.

    CAN API V3 is a wrapper specification to have a uniform CAN
    Interface API for various CAN interfaces from different
    vendors running under multiple operating systems.

    @author   $Author: eris $

    @version  $Rev: 1002 $
 */
import Foundation
import CCanApi

private let version = (major: 0, minor: 1, patch: 0)

public class CanApi {
    private lazy var handle: CInt = -1  // CAN interface handle

    public init() {
        // try to relax and enjoy the crisis
    }
    deinit {
        guard self.handle >= 0 else { return }
        let _ = can_exit(self.handle)
    }
    
    // MARK: - types
    
    /*!
        @brief  CAN status register:
                - bit 7: controller stopped
                - bit 6: busoff status
                - bit 5: error warning level
                - bit 4: bus error (LEC)
                - bit 3: transmitter busy
                - bit 2: receiver empty
                - bit 1: message lost
                - bit 0: queue overrun
     */
    public struct Status {
        // status bits as predicates
        public let isCanStopped: Bool
        public let isBusOff: Bool
        public let isWarningLevel: Bool
        public let isBusError: Bool
        public let isTransmitterBusy: Bool
        public let isReceiverEmpty: Bool
        public let isMessageLost: Bool
        public let isQueueOverrun: Bool
        // status register from C interface
        public init(from status: can_status_t) {
            isCanStopped = status.can_stopped != 0
            isBusOff = status.bus_off != 0
            isWarningLevel = status.warning_level != 0
            isBusError = status.bus_error != 0
            isTransmitterBusy = status.transmitter_busy != 0
            isReceiverEmpty = status.receiver_empty != 0
            isMessageLost = status.message_lost != 0
            isQueueOverrun = status.queue_overrun != 0
        }
    }
    /*!
        @brief  CAN operation modes:
                - bit 7: CAN FD operation enable/disable
                - bit 6: bit-rate switch enable/disable
                - bit 5: Non-ISO CAN FD enable/disable
                - bit 4: shared access enable/disable
                - bit 3: extended format disable/enable
                - bit 2: remote frames disable/enable
                - bit 1: error frames enable/disable
                - bit 0: monitor mode enable/disable
     */
    public struct Mode: OptionSet {
        // operation modes as option set
        public static let DefaultOperationMode = Mode([])
        public static let FdOperationEnabled = Mode(rawValue: 0x80)
        public static let BitrateSwitchingEnabled = Mode(rawValue: 0x40)
        public static let NonIsoOperationEnabled = Mode(rawValue: 0x20)
        public static let SharedAccessEnabled = Mode(rawValue: 0x10)
        public static let ExtendedFramesDisabled = Mode(rawValue: 0x08)
        public static let RemoteFramesDisabled = Mode(rawValue: 0x04)
        public static let ErrorFramesEnabled = Mode(rawValue: 0x02)
        public static let MonitorModeEnabled = Mode(rawValue: 0x01)
        // operation mode from C interface
        public init(rawValue: UInt8) {
            self.rawValue = rawValue
        }
        public let rawValue: UInt8
        // operation mode bits as predicates
        public var isFdOperationEnabled: Bool { get { return 0x80 == self.rawValue & 0x80 } }
        public var isBitrateSwitchingEnabled: Bool { get { return 0x40 == self.rawValue & 0x40 } }
        public var isNonIsoOperationEnabled: Bool { get { return 0x20 == self.rawValue & 0x20 } }
        public var isSharedAccessEnabled : Bool { get { return 0x10 == self.rawValue & 0x10 } }
        public var isExtendedFramesDisabled: Bool { get { return 0x08 == self.rawValue & 0x08 } }
        public var isRemoteFramesDisabled: Bool { get { return 0x04 == self.rawValue & 0x04 } }
        public var isErrorFramesEnabled: Bool { get { return 0x02 == self.rawValue & 0x02 } }
        public var isMonitorModeEnabled: Bool { get { return 0x01 == self.rawValue & 0x01 } }
    }
    /*!
        @brief  CAN bit-rate settings (nominal and data phase):
                - clock domain (frequency in [Hz])
                - nominal bus speed:
                  - bit-rate prescaler
                  - timing segment 1
                  - timing segment 2
                  - synchronization jump width
                  - number of samples (SJA1000)
                - data phase bus speed:
                  - bit-rate prescaler
                  - timing segment 1
                  - timing segment 2
                  - synchronization jump width
     */
    public struct Bitrate {
        // struct: nominal bus speed
        public struct Nominal {
            public var brp: UInt16
            public var tseg1: UInt16
            public var tseg2: UInt16
            public var sjw: UInt16
            public var sam: UInt8
        }
        // struct: data phase bus speed
        public struct DataPhase {
            public var brp: UInt16
            public var tseg1: UInt16
            public var tseg2: UInt16
            public var sjw: UInt16
        }
        // member: bit-rats settings
        public var frequency: Int32
        public var nominal: Nominal
        public var data: DataPhase
        // bit-rate settinsg from C interface
        public init(from bitrate: can_bitrate_t) {
            precondition(bitrate.index > 0)
            frequency = bitrate.btr.frequency
            nominal = Nominal(brp: bitrate.btr.nominal.brp,
                              tseg1: bitrate.btr.nominal.tseg1,
                              tseg2: bitrate.btr.nominal.tseg2,
                              sjw: bitrate.btr.nominal.sjw,
                              sam: bitrate.btr.nominal.sam)
            data = DataPhase(brp: bitrate.btr.data.brp,
                             tseg1: bitrate.btr.data.tseg1,
                             tseg2: bitrate.btr.data.tseg2,
                             sjw: bitrate.btr.data.sjw)
        }
        // conversion from Swift to C interface
        public var cbitrate: can_bitrate_t {
            get {
                var bitrate: can_bitrate_t = can_bitrate_t()
                bitrate.btr.frequency = self.frequency
                bitrate.btr.nominal.brp = self.nominal.brp
                bitrate.btr.nominal.tseg1 = self.nominal.tseg1
                bitrate.btr.nominal.tseg2 = self.nominal.tseg2
                bitrate.btr.nominal.sjw = self.nominal.sjw
                bitrate.btr.nominal.sam = self.nominal.sam
                bitrate.btr.data.brp = self.data.brp
                bitrate.btr.data.tseg1 = self.data.tseg1
                bitrate.btr.data.tseg2 = self.data.tseg2
                bitrate.btr.data.sjw = self.data.sjw
                return bitrate
            }
        }
    }
    /*!
        @brief  CAN 2.0 Predefined bit-rates (as CiA index acc. CANopen spec.):
                - index 0: 1000 kbit/s with sample-point at 75.0%
                - index 1:  800 kbit/s with sample-point at 80.0%
                - index 2:  500 kbit/s with sample-point at 87.5%
                - index 3:  250 kbit/s with sample-point at 87.5%
                - index 4:  125 kbit/s with sample-point at 87.5%
                - index 5:  100 kbit/s with sample-point at 87.5%
                - index 6:   50 kbit/s with sample-point at 87.5%
                - index 7:   20 kbit/s with sample-point at 87.5%
                - index 8:   10 kbit/s with sample-point at 87.5%
     */
    public enum CiaIndex: Int {
        // enum: indexes to bit-timing table
        case Index1000kbps = 0
        case Index800kbps = 1
        case Index500kbps = 2
        case Index250kbps = 3
        case Index125kbps = 4
        case Index100kbps = 5
        case Index50kbps = 6
        case Index20kbps = 7
        case Index10kbps = 8
        // conversion from Swift to C interface
        public var cindex: CInt {
            get {
                switch self {
                case .Index1000kbps: return CInt(CANBTR_INDEX_1M)
                case .Index800kbps: return CInt(CANBTR_INDEX_800K)
                case .Index500kbps: return CInt(CANBTR_INDEX_500K)
                case .Index250kbps: return CInt(CANBTR_INDEX_250K)
                case .Index125kbps: return CInt(CANBTR_INDEX_125K)
                case .Index100kbps: return CInt(CANBTR_INDEX_100K)
                case .Index50kbps: return CInt(CANBTR_INDEX_50K)
                case .Index20kbps: return CInt(CANBTR_INDEX_20K)
                case .Index10kbps: return CInt(CANBTR_INDEX_10K)
                }
            }
        }
    }
    /*!
        @brief  CAN transmission rate (nominal and data phase):
                - nominal bus speed:
                  - flag: CAN FD operation enabled
                  - bus speed in [Bit/s]
                  - sample point in [percent]
                - data phase bus speed:
                  - flag: CAN FD bit-rate switching enabled
                  - bus speed in [Bit/s]
                  - sample point in [percent]
     */
    public struct Speed {
        // struct: nominal bus speed
        public struct Nominal {
            public var fdOperationEnabled: Bool
            public var busSpeed: Float
            public var samplePoint: Float
        }
        // struct: data phase bus speed
        public struct DataPhase {
            public var bitrateSwitchingEnabled: Bool
            public var busSpeed: Float
            public var samplePoint: Float
        }
        // member: transmission rate
        public var nominal: Nominal
        public var data: DataPhase
        // transmission rate from C interface
        public init(from speed: CanBusSpeed) {
            nominal = Nominal(fdOperationEnabled: speed.nominal.fdoe,
                              busSpeed: speed.nominal.speed,
                              samplePoint: speed.nominal.samplepoint)
            data = DataPhase(bitrateSwitchingEnabled: speed.data.brse,
                             busSpeed: speed.data.speed,
                             samplePoint: speed.data.samplepoint)
        }
        // conversion from Swift to C interface
        // TODO: not realized yet (will this ever be?)
    }
    /*!
        @brief  CAN message (with time-stamp)
                - CAN identifier (11-bit or 29-bit)
                - CAN message flags:
                  - bit 0: extended format
                  - bit 1: remote frame
                  - bit 2: CAN FD format
                  - bit 3: bit-rate switching
                  - bit 4: error state indicator
                  - bit 7: status message
                - data length code (0 .. 15)
                - payload (CAN FD:  0 .. 64)
                - time-stamp { sec, nsec }
    */
    public struct Message {
        public struct Flags: OptionSet {
            // message flags as option set
            public static let StandardFrame = Flags([])
            public static let ExtendedFrame = Mode(rawValue: 0x01)
            public static let RemoteFrame = Mode(rawValue: 0x02)
            public static let CanFdLongFrame = Mode(rawValue: 0x04)
            public static let CanFdFastFrame = Mode(rawValue: 0x08)
            // message flags from C interface
            public init(rawValue: UInt8) {
                self.rawValue = rawValue
            }
            public let rawValue: UInt8
            // message flags as predicates
            public var isStandardFrame: Bool { get { return 0x00 == self.rawValue & 0xFF } }
            public var isExtendedFrame: Bool { get { return 0x01 == self.rawValue & 0x01 } }
            public var isRemoteFrame: Bool { get { return 0x02 == self.rawValue & 0x02 } }
            public var isCanFdLongFrame: Bool { get { return 0x04 == self.rawValue & 0x04 } }
            public var isCanFdFastFrame: Bool { get { return 0x08 == self.rawValue & 0x08 } }
            public var isErrorStateIndicator: Bool { get { return 0x10 == self.rawValue & 0x10 } }
            public var isStatusMessage: Bool { get { return 0x80 == self.rawValue & 0x80 } }
        }
        // member: CAN message
        public var canId: UInt32
        public var canDlc: UInt8
        public var flags: Flags
        public var data: [UInt8]
        public var time: TimeInterval
        // overloaded initializer
        public init(canId: UInt32, flags: Flags = [.StandardFrame], data: [UInt8]) {
            self.canId = canId
            self.flags = flags
            self.data = data
            self.canDlc = 0
            if data.count >= 1 { self.canDlc = 0x1 }
            if data.count >= 2 { self.canDlc = 0x2 }
            if data.count >= 3 { self.canDlc = 0x3 }
            if data.count >= 4 { self.canDlc = 0x4 }
            if data.count >= 5 { self.canDlc = 0x5 }
            if data.count >= 6 { self.canDlc = 0x6 }
            if data.count >= 7 { self.canDlc = 0x7 }
            if data.count >= 8 { self.canDlc = 0x8 }
            if data.count >= 9 && flags.isCanFdLongFrame { self.canDlc = 0x9 }
            if data.count >= 12 && flags.isCanFdLongFrame { self.canDlc = 0xA }
            if data.count >= 16 && flags.isCanFdLongFrame { self.canDlc = 0xB }
            if data.count >= 20 && flags.isCanFdLongFrame { self.canDlc = 0xC }
            if data.count >= 24 && flags.isCanFdLongFrame { self.canDlc = 0xD }
            if data.count >= 32 && flags.isCanFdLongFrame { self.canDlc = 0xE }
            if data.count >= 48 && flags.isCanFdLongFrame { self.canDlc = 0xF }
            self.time = 0.0
        }
        // CAN message from C interface
        public init(from message: can_message_t) {
            canId = message.id
            canDlc = message.dlc
            flags = Flags(rawValue: message.xtd + message.rtr + message.fdf + message.brs + message.esi + message.sts)
            data = []
            // TODO: there should be something better in Swift
            if message.dlc > 0x0 { data.append(message.data.0) }
            if message.dlc > 0x1 { data.append(message.data.1) }
            if message.dlc > 0x2 { data.append(message.data.2) }
            if message.dlc > 0x3 { data.append(message.data.3) }
            if message.dlc > 0x4 { data.append(message.data.4) }
            if message.dlc > 0x5 { data.append(message.data.5) }
            if message.dlc > 0x6 { data.append(message.data.6) }
            if message.dlc > 0x7 { data.append(message.data.7) }
            if message.dlc > 0x8 { data.append(message.data.8); data.append(message.data.9); data.append(message.data.10); data.append(message.data.11) }
            if message.dlc > 0x9 { data.append(message.data.12); data.append(message.data.13); data.append(message.data.14); data.append(message.data.15) }
            if message.dlc > 0xA { data.append(message.data.16); data.append(message.data.17); data.append(message.data.18); data.append(message.data.19) }
            if message.dlc > 0xB { data.append(message.data.20); data.append(message.data.21); data.append(message.data.22); data.append(message.data.23) }
            if message.dlc > 0xC { data.append(message.data.24); data.append(message.data.25); data.append(message.data.26); data.append(message.data.27); data.append(message.data.28); data.append(message.data.29); data.append(message.data.30); data.append(message.data.31) }
            if message.dlc > 0xD { data.append(message.data.32); data.append(message.data.33); data.append(message.data.34); data.append(message.data.35); data.append(message.data.36); data.append(message.data.37); data.append(message.data.38); data.append(message.data.39);  data.append(message.data.40); data.append(message.data.41); data.append(message.data.42); data.append(message.data.43); data.append(message.data.44); data.append(message.data.45); data.append(message.data.46); data.append(message.data.47) }
            if message.dlc > 0xE { data.append(message.data.48); data.append(message.data.49); data.append(message.data.50); data.append(message.data.51); data.append(message.data.52); data.append(message.data.53); data.append(message.data.54); data.append(message.data.55); data.append(message.data.56); data.append(message.data.57); data.append(message.data.58); data.append(message.data.59); data.append(message.data.60); data.append(message.data.61); data.append(message.data.62); data.append(message.data.63) }
            time = Double(message.timestamp.tv_sec) + Double(message.timestamp.tv_nsec / 1000000000)
        }
        // conversion from Swift to C interface
        public var cmessage: can_message_t {
            get {
                var message: can_message_t = can_message_t()
                message.id = self.canId
                message.dlc = self.canDlc
                // TODO: there should be something better in Swift
                if self.flags.isExtendedFrame { message.xtd = 1 } else  { message.xtd = 0 }
                if self.flags.isRemoteFrame { message.rtr = 1 } else  { message.rtr = 0 }
                if self.flags.isCanFdLongFrame { message.fdf = 1 } else  { message.fdf = 0 }
                if self.flags.isCanFdFastFrame { message.brs = 1 } else  { message.brs = 0 }
                if self.data.count > 0 { message.data.0 = self.data[0] }
                if self.data.count > 1 { message.data.1 = self.data[1] }
                if self.data.count > 2 { message.data.2 = self.data[2] }
                if self.data.count > 3 { message.data.3 = self.data[3] }
                if self.data.count > 4 { message.data.4 = self.data[4] }
                if self.data.count > 5 { message.data.5 = self.data[5] }
                if self.data.count > 6 { message.data.6 = self.data[6] }
                if self.data.count > 7 { message.data.7 = self.data[7] }
                if self.data.count > 8 { message.data.8 = self.data[8] }
                if self.data.count > 9 { message.data.9 = self.data[9] }
                if self.data.count > 10 { message.data.10 = self.data[10] }
                if self.data.count > 11 { message.data.11 = self.data[11] }
                if self.data.count > 12 { message.data.12 = self.data[12] }
                if self.data.count > 13 { message.data.13 = self.data[13] }
                if self.data.count > 14 { message.data.14 = self.data[14] }
                if self.data.count > 15 { message.data.15 = self.data[15] }
                if self.data.count > 16 { message.data.16 = self.data[16] }
                if self.data.count > 17 { message.data.17 = self.data[17] }
                if self.data.count > 18 { message.data.18 = self.data[18] }
                if self.data.count > 19 { message.data.19 = self.data[19] }
                if self.data.count > 20 { message.data.20 = self.data[20] }
                if self.data.count > 21 { message.data.21 = self.data[21] }
                if self.data.count > 22 { message.data.22 = self.data[22] }
                if self.data.count > 23 { message.data.23 = self.data[23] }
                if self.data.count > 24 { message.data.24 = self.data[24] }
                if self.data.count > 25 { message.data.25 = self.data[25] }
                if self.data.count > 26 { message.data.26 = self.data[26] }
                if self.data.count > 27 { message.data.27 = self.data[27] }
                if self.data.count > 28 { message.data.28 = self.data[28] }
                if self.data.count > 29 { message.data.29 = self.data[29] }
                if self.data.count > 30 { message.data.30 = self.data[30] }
                if self.data.count > 31 { message.data.31 = self.data[31] }
                if self.data.count > 32 { message.data.32 = self.data[32] }
                if self.data.count > 33 { message.data.33 = self.data[33] }
                if self.data.count > 34 { message.data.34 = self.data[34] }
                if self.data.count > 35 { message.data.35 = self.data[35] }
                if self.data.count > 36 { message.data.36 = self.data[36] }
                if self.data.count > 37 { message.data.37 = self.data[37] }
                if self.data.count > 38 { message.data.38 = self.data[38] }
                if self.data.count > 39 { message.data.39 = self.data[39] }
                if self.data.count > 40 { message.data.40 = self.data[40] }
                if self.data.count > 41 { message.data.41 = self.data[41] }
                if self.data.count > 42 { message.data.42 = self.data[42] }
                if self.data.count > 43 { message.data.43 = self.data[43] }
                if self.data.count > 44 { message.data.44 = self.data[44] }
                if self.data.count > 45 { message.data.45 = self.data[45] }
                if self.data.count > 46 { message.data.46 = self.data[46] }
                if self.data.count > 47 { message.data.47 = self.data[47] }
                if self.data.count > 48 { message.data.48 = self.data[48] }
                if self.data.count > 49 { message.data.49 = self.data[49] }
                if self.data.count > 50 { message.data.50 = self.data[50] }
                if self.data.count > 51 { message.data.51 = self.data[51] }
                if self.data.count > 52 { message.data.52 = self.data[52] }
                if self.data.count > 53 { message.data.53 = self.data[53] }
                if self.data.count > 54 { message.data.54 = self.data[54] }
                if self.data.count > 55 { message.data.55 = self.data[55] }
                if self.data.count > 56 { message.data.56 = self.data[56] }
                if self.data.count > 57 { message.data.57 = self.data[57] }
                if self.data.count > 58 { message.data.58 = self.data[58] }
                if self.data.count > 59 { message.data.59 = self.data[59] }
                if self.data.count > 60 { message.data.60 = self.data[60] }
                if self.data.count > 61 { message.data.61 = self.data[61] }
                if self.data.count > 62 { message.data.62 = self.data[62] }
                if self.data.count > 63 { message.data.63 = self.data[63] }
                message.timestamp.tv_sec = 0
                message.timestamp.tv_nsec = 0
                return message
            }
        }
    }
    /*!
        @brief  CAN channel state:
                (+1) CAN interface available, but occupied
                 (0) CAN interface available
                (-1) CAN interface not available
                (-2) CAN interface not testable (or legacy API)
     */
    public enum State: Int32 {
        // enum: channel state
        case ChannelOccupied = 1
        case ChannelAvailable = 0
        case ChannelNotAvailable = -1
        case ChannelNotTestable = -2
        // error description (as String)
        public var description: String {
            get {
                switch self {
                case .ChannelOccupied: return "CAN interface available, but occupied"
                case .ChannelAvailable: return "CAN interface available"
                case .ChannelNotAvailable: return "CAN interface not available"
                case .ChannelNotTestable: return "CAN interface not testable"
                }
            }
        }
    }
    /*!
        @brief  CAN error codes (CAN API V1 & V2 compatible)
     */
    public enum Error: Int32, Swift.Error {
        // enum: error code (as Swift error exception)
        case NoError = 0  // no error!
        case BusOFF = -1  // busoff status
        case ErrorWarning = -2  // error warning status
        case BusError = -3  // bus error
        case ControllerOffline = -9  // not started
        case ControllerOnline = -8  // already started
        case MessageLost = -10  // message lost
        case TransmitterBusy = -20  // transmitter busy
        case ReceiverEmpty = -30  // receiver empty
        case ErrorFrame = -40  // error frame
        case Timeout = -50  // timed out
        case ResourceError = -90  // resource allocation
        case InvalidBaudrate = -91  //  illegal baudrate
        case InvalidHandle = -92  //  illegal handle
        case IllegalParameter = -93  // illegal parameter
        case NullPointer = -94  // null-pointer assignment
        case NotInitialized = -95  // not initialized
        case AlreadyInitialized = -96  // already initialized
        case InvalidLibrary = -97 // illegal library
        case NotSupported = -98  // not supported
        case FatalError = -99  // fatal error
        case VendorSpecific = -100  // offset for vendor-specific error code
        // error description (as String)
        public var description: String {
            get {
                switch self {  // TODO: rework the text
                case .NoError: return "no error occurred"
                case .BusOFF: return "busoff status"
                case .ErrorWarning: return "error warning status"
                case .BusError: return "bus error"
                case .ControllerOffline: return "not started"
                case .ControllerOnline: return "already started"
                case .MessageLost: return "message lost"
                case .TransmitterBusy: return "transmitter busy"
                case .ReceiverEmpty: return "receiver empty"
                case .ErrorFrame: return "error frame"
                case .Timeout: return "timed out"
                case .ResourceError: return "resource allocation"
                case .InvalidBaudrate: return "illegal baudrate"
                case .InvalidHandle: return "illegal handle"
                case .IllegalParameter: return "illegal parameter"
                case .NullPointer : return "null-pointer assignment"
                case .NotInitialized: return "not initialized"
                case .AlreadyInitialized: return "already initialized"
                case .InvalidLibrary: return "illegal library"
                case .NotSupported: return "not supported"
                case .FatalError: return "fatal error"
                default: return "vendor-specific or general error"
                }
            }
        }
    }

    // MARK: - constants
    
    /*!
        @brief  constant:  polling (return immediately)
     */
    public static let polling = UInt16(0)
    /*!
        @brief  constant:  blocking read (wait infinitely)
     */
    public static let blocking = UInt16(CANREAD_INFINITE)
    
    // MARK: - properties
    
    /*!
        @brief  property:  CAN status-register (read-only)
     */
    public var status: Status? {
        get {
            var status: can_status_t = can_status_t()
            let result = can_status(self.handle, &status.byte)
            guard result == CANERR_NOERROR else { return nil }
            return Status(from: status)
        }
    }
    /*!
        @brief  property:  CAN bus load (read-only)
     */
    public var busload: Float? {
        get {
            var load: UInt8 = 0
            let result = can_busload(self.handle, &load, nil)
            guard result == CANERR_NOERROR else { return nil }
            return Float(load) / 100.0
        }
    }
    /*!
        @brief  property:  CAN bit-rate settings (read-only)
     */
    public var bitrate: Bitrate? {
        get {
            var bitrate: can_bitrate_t = can_bitrate_t()
            let result: CInt = can_bitrate(self.handle, &bitrate, nil)
            guard result == CANERR_NOERROR else { return nil }
            return Bitrate(from: bitrate)
        }
    }
    /*!
        @brief  property:  CAN transmission rate (read-only)
     */
    public var speed: Speed? {
        get {
            var speed: can_speed_t = can_speed_t()
            let result: CInt = can_bitrate(self.handle, nil, &speed)
            guard result == CANERR_NOERROR else { return nil }
            return Speed(from: speed)
        }
    }
    /*!
        @brief  property:  CAN operation mode (read-only)
     */
    public var mode: Mode? {
        get {
            var mode: can_mode_t = can_mode_t()
            let result: CInt = can_property(self.handle, UInt16(CANPROP_GET_OP_MODE), &mode.byte, UInt32(MemoryLayout<UInt8>.size))
            guard result == CANERR_NOERROR else { return nil }
            return Mode(rawValue: mode.byte)
        }
    }
    /*!
        @brief  property:  CAN operation capability  (read-only)
     */
    public var capability: Mode? {
        get {
            var mode: can_mode_t = can_mode_t()
            let result: CInt = can_property(self.handle, UInt16(CANPROP_GET_OP_CAPABILITY), &mode.byte, UInt32(MemoryLayout<UInt8>.size))
            guard result == CANERR_NOERROR else { return nil }
            return Mode(rawValue: mode.byte)
        }
    }
    /*!
        @brief  property:  CAN statistical counters (read-only)
     */
    public var statistics: (transmitted: UInt64, received: UInt64, errors: UInt64)? {
        get {
            var transmitted: UInt64 = 0
            var received: UInt64 = 0
            var errors: UInt64 = 0
            var result: CInt
            result = can_property(self.handle, UInt16(CANPROP_GET_TX_COUNTER), &transmitted, UInt32(MemoryLayout<UInt64>.size))
            guard result == CANERR_NOERROR else { return nil }
            result = can_property(self.handle, UInt16(CANPROP_GET_RX_COUNTER), &received, UInt32(MemoryLayout<UInt64>.size))
            guard result == CANERR_NOERROR else { return nil }
            result = can_property(self.handle, UInt16(CANPROP_GET_ERR_COUNTER), &errors, UInt32(MemoryLayout<UInt64>.size))
            guard result == CANERR_NOERROR else { return nil }
            return (transmitted: transmitted, received: received, errors: errors)
        }
    }
    /*!
        @brief  property:  CAN device informations (read-only)
     */
    public var deviceInfo: (type: Int32, name: String, vendor: String)? {
        get {
            var type: Int32 = 0
            var name = [UInt8](repeating: 0x00, count: Int(CANPROP_MAX_BUFFER_SIZE))
            var vendor = [UInt8](repeating: 0x00, count: Int(CANPROP_MAX_BUFFER_SIZE))
            var result: CInt
            result = can_property(self.handle, UInt16(CANPROP_GET_DEVICE_TYPE), &type, UInt32(MemoryLayout<Int32>.size))
            guard result == CANERR_NOERROR else { return nil }
            result = can_property(self.handle, UInt16(CANPROP_GET_DEVICE_NAME), &name, UInt32(CANPROP_MAX_BUFFER_SIZE))
            guard result == CANERR_NOERROR else { return nil }
            result = can_property(self.handle, UInt16(CANPROP_GET_DEVICE_VENDOR), &vendor, UInt32(CANPROP_MAX_BUFFER_SIZE))
            guard result == CANERR_NOERROR else { return nil }
            return (type: type, name: String(cString: &name), vendor: String(cString: &vendor))
        }
    }
    /*!
        @brief  property:  CAN library informations (read-only)
     */
    public var libraryInfo: (id: Int32, name: String, vendor: String)? {
        get {
            var id: Int32 = 0
            var name = [UInt8](repeating: 0x00, count: Int(CANPROP_MAX_BUFFER_SIZE))
            var vendor = [UInt8](repeating: 0x00, count: Int(CANPROP_MAX_BUFFER_SIZE))
            var result: CInt
            result = can_property(self.handle, UInt16(CANPROP_GET_LIBRARY_ID), &id, UInt32(MemoryLayout<Int32>.size))  // FIXME: -1 = library property
            guard result == CANERR_NOERROR else { return nil }
            result = can_property(self.handle, UInt16(CANPROP_GET_LIBRARY_DLLNAME), &name, UInt32(CANPROP_MAX_BUFFER_SIZE))  // FIXME: -1 = library property
            guard result == CANERR_NOERROR else { return nil }
            result = can_property(self.handle, UInt16(CANPROP_GET_LIBRARY_VENDOR), &vendor, UInt32(CANPROP_MAX_BUFFER_SIZE))  // FIXME: -1 = library property
            guard result == CANERR_NOERROR else { return nil }
            return (id: id, name: String(cString: &name), vendor: String(cString: &vendor))
        }
    }
    /*!
        @brief  property:  CAN library version  (read-only)
     */
    public var libraryVersion: (major: UInt, minor: UInt, patch: UInt)? {
        get {
            var version: UInt16 = 0
            var patch: UInt8 = 0
            var result: CInt
            result = can_property(self.handle, UInt16(CANPROP_GET_VERSION), &version, UInt32(MemoryLayout<UInt16>.size))  // FIXME: -1 = library property
            guard result == CANERR_NOERROR else { return nil }
            result = can_property(self.handle, UInt16(CANPROP_GET_PATCH_NO), &patch, UInt32(MemoryLayout<UInt8>.size))  // FIXME: -1 = library property
            guard result == CANERR_NOERROR else { return nil }
            return (major: UInt(version >> 8), minor: UInt(version & 0xFF), patch: UInt(patch))
        }
    }
    /*!
        @brief  property:  CAN API version (read-only)
     */
    public var canApiVersion: (major: UInt, minor: UInt, patch: UInt)? {
        get {
            var version: UInt16 = 0
            let patch: UInt8 = 0
            var result: CInt
            result = can_property(self.handle, UInt16(CANPROP_GET_SPEC), &version, UInt32(MemoryLayout<UInt16>.size))  // FIXME: -1 = library property
            guard result == CANERR_NOERROR else { return nil }
            return (major: UInt(version >> 8), minor: UInt(version & 0xFF), patch: UInt(patch))
        }
    }
    /*!
        @brief  property:  Swift warpper version (read-only)
     */
    public var wrapperVersion: (major: UInt, minor: UInt, patch: UInt)? {
        get {
            return (major: UInt(version.major), minor: UInt(version.minor), patch: UInt(version.patch))
        }
    }

    // MARK: - methods
    
    /*!
        @brief       probes if the CAN interface (hardware and driver) given by
                     the argument 'channel' is present, and if the requested
                     operation mode is supported by the CAN controller.

        @note        When a requested operation mode is not supported by the
                     CAN controller, error .IllegalParameter will be thrown.

        @param[in]   channel - channel number of the CAN interface
        @param[in]   mode    - operation mode to be checked
        @param[in]   param   - pointer to channel-specific parameters

        @returns     State   - state of the CAN channel:
                                  < 0 - channel is not present,
                                  = 0 - channel is present,
                                  > 0 - channel is present, but in use
     */
    public static func ProbeChannel(channel: Int32, mode: Mode = .DefaultOperationMode) throws -> State {
        var state: CInt = CInt(CANBRD_INTESTABLE)
        let result: CInt = can_test(channel, mode.rawValue, nil, &state)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return State(rawValue: state) ?? .ChannelNotTestable
    }
    /*!
        @brief       initializes the CAN interface (hardware and driver) given by
                     the argument 'channel'.

                     The operation state of the CAN controller is set to 'stopped';
                     no communication is possible in this state.

        @param[in]   channel - channel number of the CAN interface
        @param[in]   mode    - desired operation mode of the CAN controller
     */
    public func InitializeChannel(channel: Int32, mode: Mode = .DefaultOperationMode) throws {
        let handle = can_init(channel, mode.rawValue, nil)
        guard handle >= 0 else { throw Error(rawValue: handle) ?? Error.FatalError }
        self.handle = handle
    }
    /*!
        @brief       stops any operation of the CAN interface and sets the operation
                     state of the CAN controller to 'stopped'.
     */
    public func TeardownChannel() throws {
        let result =  can_exit(self.handle)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        self.handle = -1
    }
    /*!
        @brief       signals waiting event objects of the CAN interface. This can
                     be used to terminate blocking operations in progress
                     (e.g. by means of a Ctrl-C handler or similar).

        @remarks     Some drivers are using waitable objects to realize blocking
                     operations by a call to WaitForSingleObject (Windows) or
                     pthread_cond_wait (POSIX), but these waitable objects are
                     no cancellation points. This means that they cannot be
                     terminated by Ctrl-C (SIGINT).

        @note        Even though this is not the case with Darwin, we support
                     this feature for compatibility reasons.
     */
    public func SignalChannel() throws {
        let result = can_kill(self.handle)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    /*!
        @brief       initializes the operation mode and the bit-rate settings of the
                     CAN interface and sets the operation state of the CAN controller
                     to 'running'.

        @note        All statistical counters (tx/rx/err) will be reset by this.

        @param[in]   bitrate - bit-rate as bit-timing register
     */
    public func StartController(bitrate: Bitrate) throws {
        var cbitrate: can_bitrate_t = bitrate.cbitrate
        let result = can_start(self.handle, &cbitrate)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    /*!
        @brief       initializes the operation mode and the bit-rate settings of the
                     CAN interface and sets the operation state of the CAN controller
                     to 'running'.

        @note        All statistical counters (tx/rx/err) will be reset by this.

        @param[in]   index - bit-rate as index to CiA bit-timimg table
     */
    public func StartController(index: CiaIndex) throws {
        var bitrate: can_bitrate_t = can_bitrate_t(index: index.cindex)
        let result = can_start(self.handle, &bitrate)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    /*!
        @brief       stops any operation of the CAN interface and sets the operation
                     state of the CAN controller to 'stopped'; no communication is
                     possible in this state.
     */
    public func ResetController() throws {
        let result = can_reset(self.handle)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    /*!
        @brief       transmits one message over the CAN bus. The CAN controller must
                     be in operation state 'running'.
        
        @param[in]   message - the message to be sent
        @param[in]   timeout - time to wait for the transmission of the message:
                                    0 means the function returns immediately,
                                    65535 means blocking write, and any other
                                    value means the time to wait in milliseconds
     */
    public func WriteMessage(message: Message, timeout: UInt16 = 0) throws {
        var cmessage: can_message_t = message.cmessage
        let result = can_write(self.handle, &cmessage, timeout)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    /*!
        @brief       reads one message from the message queue of the CAN interface, if
                     any message was received. The CAN controller must be in operation
                     state 'running'.
        
        @param[in]   timeout - time to wait for the reception of a message:
                                    0 means the function returns immediately,
                                    65535 means blocking read, and any other
                                    value means the time to wait in milliseconds

        @returns     Message - the message read from the message queue, if any
     */
    public func ReadMessage(timeout: UInt16 = blocking) throws -> Message? {
        var message: can_message_t = can_message_t()
        let result = can_read(self.handle, &message, timeout)
        guard result == CANERR_NOERROR || result == CANERR_RX_EMPTY else { throw Error(rawValue: result) ?? Error.FatalError }
        guard result == CANERR_NOERROR else { return nil }
        return Message(from: message)
    }
//    /*!
//        @brief       method tbd.
//     */
//    public func GetProperty<T>(param: UInt16) throws -> T {
//        var value: T = ... // TODO: Get Schwifty
//        let result: CInt = can_property(self.handle, param, &value, UInt32(MemoryLayout<T>.size))
//        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
//        return value
//    }
//    /*!
//        @brief       method tbd.
//     */
//    public func SetProperty<T>(param: UInt16, value: T) throws {
//        // TODO: Morty´s Margarita
//        let result: CInt = can_property(self.handle, param, &value, UInt32(MemoryLayout<T>.size))
//        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
//    }

    // MARK: - deprecated types

    public typealias CanOpMode = can_mode_t
    public typealias CanBitrate = can_bitrate_t
    public typealias CanBusSpeed = can_speed_t
    public typealias CanMessage = can_message_t
    public typealias CanStatus = can_status_t

    // MARK: - deprecated methods

    public static func ProbeChannel(channel: Int32, mode: CanOpMode = CanOpMode(byte: UInt8(CANMODE_DEFAULT))) throws -> CInt {
        var state: CInt = CInt(CANBRD_INTESTABLE)
        let result: CInt = can_test(channel, mode.byte, nil, &state)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return state
    }
    public func InitializeChannel(channel: Int32, mode: CanOpMode = CanOpMode(byte: UInt8(CANMODE_DEFAULT))) throws {
        let handle = can_init(channel, mode.byte, nil)
        guard handle >= 0 else { throw Error(rawValue: handle) ?? Error.FatalError }
        self.handle = handle
    }
    public func StartController(bitrate: CanBitrate) throws {
        var zwerg: can_bitrate_t = bitrate
        let result = can_start(self.handle, &zwerg)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    public func WriteMessage(message: CanMessage, timeout: UInt16 = 0) throws {
        var msg: can_message_t = message
        let result = can_write(self.handle, &msg, timeout)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    public func ReadMessage(timeout: UInt16 = blocking) throws -> CanMessage {
        var msg: can_message_t = can_message_t()
        let result = can_read(self.handle, &msg, timeout)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return msg
    }
    public func GetStatus() throws-> CanStatus {
        var status: can_status_t = can_status_t()
        let result: CInt = can_status(self.handle, &status.byte)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return status
    }
    public func GetBusLoad() throws-> UInt8 {
        var load: UInt8 = 0
        let result: CInt = can_busload(self.handle, &load, nil)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return load
    }
    public func GetBitrate() throws -> CanBitrate {
        var bitrate: can_bitrate_t = can_bitrate_t()
        let result: CInt = can_bitrate(self.handle, &bitrate, nil)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return bitrate
    }
    public func GetBusSpeed() throws -> CanBusSpeed {
        var speed: can_speed_t = can_speed_t()
        let result: CInt = can_bitrate(self.handle, nil, &speed)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return speed
    }
    public func GetProperty(param: UInt16, nbyte: UInt32) throws -> [UInt8] {
        var array = [UInt8](repeating: 0x00, count: Int(CANPROP_MAX_BUFFER_SIZE))
        let result: CInt = can_property(self.handle, param, &array, nbyte)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
        return Array(array.prefix(Int(nbyte)))
    }
    public func SetProperty(param: UInt16, value: [UInt8], nbyte: UInt32) throws {
        var array = value
        let result: CInt = can_property(self.handle, param, &array, nbyte)
        guard result == CANERR_NOERROR else { throw Error(rawValue: result) ?? Error.FatalError }
    }
    public func GetHardwareVersion() throws -> String {
        guard let version = can_hardware(self.handle) else { throw Error.NullPointer }
        return String(cString: version)
    }
    public func GetFirmwareVersion() throws -> String {
        guard let version = can_software(self.handle) else { throw Error.NullPointer }
        return String(cString: version)
    }
    public static func GetVersion() throws -> String {
        guard let version = can_version() else { throw Error.NullPointer }
        return String(cString: version)
    }
}

// $Id: CANAPI.swift 1002 2021-06-16 20:28:39Z eris $  Copyright (c) UV Software, Berlin //
