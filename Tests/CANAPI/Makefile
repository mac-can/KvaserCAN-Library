#
#  CAN Interface API, Version 3 (Testing)
#
#  Copyright (c) 2004-2023 Uwe Vogt, UV Software, Berlin (info@uv-software.com)
#  All rights reserved.
#
#  This file is part of CAN API V3.
#
#  CAN API V3 is dual-licensed under the BSD 2-Clause "Simplified" License and
#  under the GNU General Public License v3.0 (or any later version).
#  You can choose between one of them if you use this file.
#
#  BSD 2-Clause "Simplified" License:
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#  CAN API V3 IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF CAN API V3, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  GNU General Public License v3.0 or later:
#  CAN API V3 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  CAN API V3 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with CAN API V3.  If not, see <http://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

PROJ_DIR = ../..
HOME_DIR = .
MAIN_DIR = ./Sources
TEST_DIR = ./Testcases

GTEST_INC = $(HOME_DIR)/GoogleTest/include
GTEST_LIB = $(HOME_DIR)/GoogleTest/macOS/lib

CANAPI_INC = $(PROJ_DIR)/Includes
CANAPI_LIB = $(PROJ_DIR)/Binaries

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/Device.o $(OUTDIR)/Options.o \
	$(OUTDIR)/TC00_SmokeTest.o $(OUTDIR)/TC01_ProbeChannel.o \
	$(OUTDIR)/TC02_InitializeChannel.o $(OUTDIR)/TC03_StartController.o \
	$(OUTDIR)/TC04_ReadMessage.o $(OUTDIR)/TC05_WriteMessage.o \
	$(OUTDIR)/TC06_ResetController.o $(OUTDIR)/TC08_TeardownChannel.o \
	$(OUTDIR)/TC09_GetStatus.o \
	$(OUTDIR)/TC11_GetBitrate.o $(OUTDIR)/Bitrates.o \
	$(OUTDIR)/TC12_GetProperty.o $(OUTDIR)/Properties.o \
	$(OUTDIR)/TCx1_CallSequences.o $(OUTDIR)/TCx2_BitrateConverter.o \
	$(OUTDIR)/Timer.o $(OUTDIR)/Progress.o \
	$(GTEST_LIB)/libgtest.a $(CANAPI_LIB)/libKvaserCAN.a

ifeq ($(current_OS),Darwin)  # macOS - libKvaserCAN.dylib  - - - - - - - - -

TARGET	= kvl_testing

INSTALL = ~/bin

DEFINES = -DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_RETVALS=1

HEADERS = -I$(HOME_DIR) \
	-I$(MAIN_DIR) \
	-I$(GTEST_INC) \
	-I$(CANAPI_INC)

CFLAGS += -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -std=c++11 -O2 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LIBRARIES =

LDFLAGS  += -lpthread \
	-Wl,-framework -Wl,IOKit -Wl,-framework -Wl,CoreFoundation

ifeq ($(BINARY),UNIVERSAL)
CFLAGS += -arch arm64 -arch x86_64
CXXFLAGS += -arch arm64 -arch x86_64
LDFLAGS += -arch arm64 -arch x86_64
endif

endif  # - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CXX = clang++
CC = clang
LD = clang++
RM = rm -f
CP = cp -f

OUTDIR = .objects


.PHONY: info outdir


all: info outdir $(TARGET)

info:
	@echo $(CXX)" on "$(current_OS)
	@echo "target: "$(TARGET) $(WARNING)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

install:
	$(CP) $(TARGET) $(INSTALL)


$(OUTDIR)/TC00_SmokeTest.o: $(TEST_DIR)/TC00_SmokeTest.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC01_ProbeChannel.o: $(TEST_DIR)/TC01_ProbeChannel.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC02_InitializeChannel.o: $(TEST_DIR)/TC02_InitializeChannel.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC03_StartController.o: $(TEST_DIR)/TC03_StartController.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC04_ReadMessage.o: $(TEST_DIR)/TC04_ReadMessage.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC05_WriteMessage.o: $(TEST_DIR)/TC05_WriteMessage.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC06_ResetController.o: $(TEST_DIR)/TC06_ResetController.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC08_TeardownChannel.o: $(TEST_DIR)/TC08_TeardownChannel.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC09_GetStatus.o: $(TEST_DIR)/TC09_GetStatus.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC11_GetBitrate.o: $(TEST_DIR)/TC11_GetBitrate.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TC12_GetProperty.o: $(TEST_DIR)/TC12_GetProperty.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TCx1_CallSequences.o: $(TEST_DIR)/TCx1_CallSequences.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/TCx2_BitrateConverter.o: $(TEST_DIR)/TCx2_BitrateConverter.cc
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/main.o: $(MAIN_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Device.o: $(MAIN_DIR)/Device.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Options.o: $(MAIN_DIR)/Options.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Timer.o: $(MAIN_DIR)/Timer.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Progress.o: $(MAIN_DIR)/Progress.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Bitrates.o: $(MAIN_DIR)/Bitrates.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Properties.o: $(MAIN_DIR)/Properties.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
