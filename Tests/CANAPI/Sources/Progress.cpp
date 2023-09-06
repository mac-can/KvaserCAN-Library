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
#include "Progress.h"
#include <iostream>
#include <assert.h>

CProgress::CProgress(int count, int steps) {
    m_nCount = (count > 0) ? count : 1;
    m_nSteps = (steps > 0) ? steps : 0;
    m_Primary.m_nCount = 0;
    m_Primary.m_nIndex = 0;
    m_Secondary.m_nCount = 0;
    m_Secondary.m_nIndex = 0;

    std::cout << '[';
    for (int i = 0; i < m_nSteps; i++)
        std::cout << ' ';
    std::cout << ']';
    std::cout.flush();
}

CProgress::~CProgress() {
    Clear();
}

void CProgress::Clear() {
    for (int i = 0; i < (m_nSteps + 2); i++)
        std::cout << "\b \b";
    std::cout.flush();
}

void CProgress::Update(int primary, int secondary) {
    if (m_Primary.m_nCount < primary)
        m_Primary.m_nCount = primary;
    if (m_Secondary.m_nCount < secondary)
        m_Secondary.m_nCount = secondary;

    int nPrimaryIndex = (((m_Primary.m_nCount * m_nSteps) * 10) + 5) / (m_nCount * 10);
    int nSecondaryIndex = (((m_Secondary.m_nCount * m_nSteps) * 10) + 5) / (m_nCount * 10);

    if ((m_Primary.m_nIndex != nPrimaryIndex) ||
        (m_Secondary.m_nIndex != nSecondaryIndex)) {
        std::cout << "\b \b";
        for (int i = m_nSteps; i > m_Secondary.m_nIndex; i--)
            std::cout << "\b \b";
        for (int i = m_Secondary.m_nIndex; i < m_nSteps; i++)
            if (i < nSecondaryIndex)
                std::cout << '=';
            else if (i < nPrimaryIndex)
                std::cout << '.';
            else if (i == nPrimaryIndex)
                std::cout << ']';
            else
                std::cout << ' ';
        std::cout << ']';
        std::cout.flush();
        m_Primary.m_nIndex = nPrimaryIndex;
        m_Secondary.m_nIndex = nSecondaryIndex;
    }
}

CCounter::CCounter(bool wait) {
    Reset(wait);
}

CCounter::~CCounter() {
    Clear();
}

void CCounter::Clear() {
    for (int i = 0; i < 16; i++)
        std::cout << "\b \b";
    std::cout.flush();
}

void CCounter::Reset(bool wait) {
    Clear();
    if (!wait)
        std::cout << "[ >>TEST<< ] ...";
    else
        std::cout << "[ >>WAIT<< ] ...";
    std::cout.flush();
    m_nCounter = 0;
}

void CCounter::Increment() {
    Clear();
    m_nCounter++;
#if (0)
    std::cout << "[ " << std::setw(6) << m_nCounter << " ] ...";
    std::cout.flush();
#else
    fprintf(stdout, "[ %-8i ] ...", m_nCounter);
    fflush(stdout);
#endif
}

// $Id$  Copyright (c) UV Software, Berlin //
