//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  KvaserCAN - macOS User-Space Driver for Kvaser CAN Interfaces
//
//  Copyright (c) 2020-2022 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
//  All rights reserved.
//
//  This file is part of MacCAN-KvaserCAN.
//
//  MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License
//  and under the GNU General Public License v3.0 (or any later version). You can
//  choose between one of them if you use MacCAN-KvaserCAN in whole or in part.
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
import Foundation

public enum KvaserCanChannel: Int32, CaseIterable {
    case Channel0 = 0
    case Channel1, Channel2, Channel3,
         Channel4, Channel5, Channel6,
         Channel7
    public var description: String {
        return "\"Kvaser CAN Channel \(self.rawValue)\""
    }
}

public class KvaserCAN: CanApi {
    // properties and methods from class CanApi
}
