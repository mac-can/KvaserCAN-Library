//  SPDX-License-Identifier: BSD-2-Clause OR GPL-3.0-or-later
//
//  Software for Industrial Communication, Motion Control and Automation
//
//  Copyright (c) 2002-2022 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
//  All rights reserved.
//
//  This class is dual-licensed under the BSD 2-Clause "Simplified" License and
//  under the GNU General Public License v3.0 (or any later version).
//  You can choose between one of them if you use this class.
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
//  THIS CLASS IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS CLASS, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  GNU General Public License v3.0 or later:
//  This class is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This class is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this class.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef PROGRESS_H_INCLUDED
#define PROGRESS_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

class CProgress {
public:
    CProgress(int count, int steps = 50);
    ~CProgress();
private:
    int m_nSteps;
    int m_nCount;
    struct {
        int m_nCount;
        int m_nIndex;
    }   m_Primary, m_Secondary;
public:
    void Clear();
    void Update(int primary, int secondary = 0);
};

class CCounter {
public:
    CCounter(bool wait = false);
    ~CCounter();
private:
    int m_nCounter;
public:
    void Reset(bool wait = false);
    void Increment();
    void Clear();
};

#endif // PROGRESS_H_INCLUDED

// $Id: Progress.h 798 2023-10-07 19:01:13Z makemake $  Copyright (c) UV Software, Berlin //
