#
#	KvaserCAN - macOS User-Space Driver for Kvaser USB CAN Interfaces
#
#	Copyright (c) 2017-2024 Uwe Vogt, UV Software, Berlin (info@mac-can.com)
#	All rights reserved.
#
#	This file is part of MacCAN-KvaserCAN.
#
#	MacCAN-KvaserCAN is dual-licensed under the BSD 2-Clause "Simplified" License
#	and under the GNU General Public License v3.0 (or any later version). You can
#	choose between one of them if you use MacCAN-KvaserCAN in whole or in part.
#
#	BSD 2-Clause "Simplified" License:
#	Redistribution and use in source and binary forms, with or without
#	modification, are permitted provided that the following conditions are met:
#	1. Redistributions of source code must retain the above copyright notice, this
#	   list of conditions and the following disclaimer.
#	2. Redistributions in binary form must reproduce the above copyright notice,
#	   this list of conditions and the following disclaimer in the documentation
#	   and/or other materials provided with the distribution.
#
#	MacCAN-KvaserCAN IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#	OF MacCAN-KvaserCAN, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#	GNU General Public License v3.0 or later:
#	MacCAN-KvaserCAN is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	MacCAN-KvaserCAN is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with MacCAN-KvaserCAN.  If not, see <https://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

MAJOR = 0
MINOR = 4
PATCH = 0

ifeq ($(PATCH),0)
VERSION = $(MAJOR).$(MINOR)
else
VERSION = $(MAJOR).$(MINOR).$(PATCH)
endif

HOME_DIR = ../..
MAIN_DIR = .

SOURCE_DIR = $(HOME_DIR)/Sources
CANAPI_DIR = $(HOME_DIR)/Sources/CANAPI
MACCAN_DIR = $(HOME_DIR)/Sources/MacCAN
DRIVER_DIR = $(HOME_DIR)/Sources/Driver
WRAPPER_DIR = $(HOME_DIR)/Sources/Wrapper

OBJECTS = $(OUTDIR)/KvaserCAN_Driver.o $(OUTDIR)/KvaserCAN_Devices.o \
	$(OUTDIR)/KvaserUSB_LeafDevice.o $(OUTDIR)/KvaserUSB_MhydraDevice.o \
	$(OUTDIR)/KvaserUSB_Device.o \
	$(OUTDIR)/MacCAN_Devices.o $(OUTDIR)/MacCAN_IOUsbKit.o \
	$(OUTDIR)/MacCAN_MsgQueue.o $(OUTDIR)/MacCAN_MsgPipe.o \
	$(OUTDIR)/MacCAN_Internal.o $(OUTDIR)/MacCAN_Debug.o \
	$(OUTDIR)/can_api.o $(OUTDIR)/can_btr.o \
	$(OUTDIR)/KvaserCAN.o

DEFINES = -DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_DRIVER=1 \
	-DOPTION_CANAPI_RETVALS=0 \
	-DOPTION_CANAPI_COMPANIONS=1 \
	-DOPTION_MACCAN_PIPE_INFO=0 \
	-DOPTION_MACCAN_PIPE_TIMEOUT=1 \
	-DOPTION_MACCAN_MULTICHANNEL=0 \
	-DOPTION_MACCAN_MARK_OVERRUN=0 \
	-DOPTION_MACCAN_FILE_DESCRIPTOR=0 \
	-DOPTION_MACCAN_USB_STRINGS=0 \
	-DOPTION_MACCAN_LOGGER=0 \
	-DOPTION_MACCAN_DEBUG_LEVEL=0 \
	-DOPTION_MACCAN_INSTRUMENTATION=0 \
	-DOPTION_CANAPI_DEBUG_LEVEL=0 \
	-DOPTION_CANAPI_INSTRUMENTATION=0

HEADERS = -I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(MACCAN_DIR) \
	-I$(DRIVER_DIR) \
	-I$(WRAPPER_DIR)


ifeq ($(current_OS),Darwin)  # macOS - libUVCANKVL.dylib

PROJECT = KvaserCAN
LIBRARY = lib$(PROJECT)

TARGET  = $(LIBRARY).$(VERSION).dylib
STATIC  = $(LIBRARY).a

INSTALL_NAME = $(TARGET)
CURRENT_VERSION = $(MAJOR).$(MINOR).$(PATCH)
COMPATIBILITY_VERSION = $(MAJOR).$(MINOR).0

INSTALL = /usr/local/lib

DEFINES += -DOPTION_KVASERCAN_DYLIB=1 \
	-DOPTION_CANAPI_KVASERCAN_DYLIB=0

CFLAGS += -O2 -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O2 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS += -nostartfiles -dynamiclib -fvisibility=hidden \
	-Wl,-framework -Wl,IOKit -Wl,-framework -Wl,CoreFoundation \
	-Wl,-install_name,@rpath/$(INSTALL_NAME) \
	-Wl,-current_version,$(CURRENT_VERSION) \
	-Wl,-compatibility_version,$(COMPATIBILITY_VERSION)

LTFLAGS += -static

ifeq ($(BINARY),UNIVERSAL)
CFLAGS += -arch arm64 -arch x86_64
CXXFLAGS += -arch arm64 -arch x86_64
LDFLAGS += -arch arm64 -arch x86_64
endif

LIBRARIES = -lpthread

CXX = clang++
CC = clang
LD = clang++
LT = libtool
endif

RM = rm -f
CP = cp -f
LN = ln -s

OUTDIR = .objects
BINDIR = $(HOME_DIR)/Binaries
INCDIR = $(HOME_DIR)/Includes

.PHONY: info outdir bindir incdir


all: info outdir bindir incdir $(STATIC) $(TARGET)

info:
	@echo $(CXX)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

bindir:
	@mkdir -p $(BINDIR)

incdir:
	@mkdir -p $(INCDIR)
	@cp $(SOURCE_DIR)/KvaserCAN_Defines.h $(INCDIR)
	@cp $(SOURCE_DIR)/KvaserCAN_Defaults.h $(INCDIR)
	@cp $(SOURCE_DIR)/KvaserCAN.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI_Types.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI_Defines.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANBTR_Defaults.h $(INCDIR)

clean:
	$(RM) $(TARGET) $(STATIC) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	$(RM) *.dylib *.a $(OUTDIR)/*.o $(OUTDIR)/*.d
	$(RM) $(BINDIR)/*.dylib $(BINDIR)/*.a
	$(RM) $(INCDIR)/*.h

install:
	@echo "Copying library file..."
	$(RM) $(INSTALL)/$(TARGET); $(CP) $(TARGET) $(INSTALL)
ifeq ($(current_OS),Darwin)
	$(RM) $(INSTALL)/$(LIBRARY).dylib ; $(LN) $(INSTALL)/$(TARGET) $(INSTALL)/$(LIBRARY).dylib
endif


$(OUTDIR)/KvaserCAN_Driver.o: $(DRIVER_DIR)/KvaserCAN_Driver.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/KvaserCAN_Devices.o: $(DRIVER_DIR)/KvaserCAN_Devices.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/KvaserUSB_Device.o: $(DRIVER_DIR)/KvaserUSB_Device.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/KvaserUSB_LeafDevice.o: $(DRIVER_DIR)/KvaserUSB_LeafDevice.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/KvaserUSB_MhydraDevice.o: $(DRIVER_DIR)/KvaserUSB_MhydraDevice.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_Devices.o: $(MACCAN_DIR)/MacCAN_Devices.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_IOUsbKit.o: $(MACCAN_DIR)/MacCAN_IOUsbKit.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_MsgQueue.o: $(MACCAN_DIR)/MacCAN_MsgQueue.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_MsgPipe.o: $(MACCAN_DIR)/MacCAN_MsgPipe.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_Internal.o: $(MACCAN_DIR)/MacCAN_Internal.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_Debug.o: $(MACCAN_DIR)/MacCAN_Debug.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_api.o: $(WRAPPER_DIR)/can_api.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_btr.o: $(CANAPI_DIR)/can_btr.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/KvaserCAN.o: $(SOURCE_DIR)/KvaserCAN.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<


$(STATIC): $(OBJECTS)
	$(LT) $(LTFLAGS) -o $@ $(OBJECTS)
ifeq ($(current_OS),Darwin)
	@lipo -archs $@
endif
	$(CP) $@ $(BINDIR)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
ifeq ($(current_OS),Darwin)
	@lipo -archs $@
endif
	$(CP) $@ $(BINDIR)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
