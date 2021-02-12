#
#	CAN Interface API, Version 3 (for Kvaser CAN Leaf Interfaces)
#
#	Copyright (C) 2017-2021  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
#
#	This file is part of MacCAN-KvaserCAN.
#
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
#	along with MacCAN-KvaserCAN.  If not, see <http://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

HOME_DIR = ../..

SOURCE_DIR = $(HOME_DIR)/Sources
CANAPI_DIR = $(HOME_DIR)/Sources/CANAPI
MACCAN_DIR = $(HOME_DIR)/Sources/MacCAN_Core
KVASER_DIR = $(HOME_DIR)/Sources/KvaserCAN_Driver

OBJECTS = $(OUTDIR)/KvaserCAN.o $(OUTDIR)/KvaserCAN_Driver.o \
	$(OUTDIR)/KvaserUSB_Device.o $(OUTDIR)/LeafLight.o \
	$(OUTDIR)/MacCAN.o $(OUTDIR)/MacCAN_Debug.o $(OUTDIR)/MacCAN_Devices.o \
	$(OUTDIR)/MacCAN_IOUsbKit.o $(OUTDIR)/MacCAN_MsgQueue.o $(OUTDIR)/MacCAN_MsgPipe.o \
	$(OUTDIR)/can_btr.o


ifeq ($(current_OS),Darwin) # macOS - libKvaserCAN.dylib

LIBRARY = libKvaserCAN

MAJOR = 0
MINOR = 1
PATCH = 0

VERSION = $(MAJOR).$(MINOR)
TARGET  = $(LIBRARY).$(VERSION).dylib
STATIC  = $(LIBRARY).a

INSTALL_NAME = $(TARGET)
CURRENT_VERSION = $(MAJOR).$(MINOR).0
COMPATIBILITY_VERSION = $(MAJOR).1.0

INSTALL = /usr/local/lib

INCLUDE = /usr/local/include

DEFINES = -DOPTION_KVASERCAN_DYLIB=1 \
	-DOPTION_CANAPI_KVASERCAN_DYLIB=0 \
	-DOPTION_CANAPI_DRIVER=1 \
	-DOPTION_CANAPI_RETVALS=0 \
	-DOPTION_CANAPI_COMPANIONS=1 \
	-DOPTION_MACCAN_LOGGER=0 \
	-DOPTION_MACCAN_MULTICHANNEL=0 \
	-DOPTION_MACCAN_PIPE_TIMEOUT=0 \
	-DOPTION_MACCAN_DEBUG_LEVEL=0 \
	-DOPTION_MACCAN_INSTRUMENTATION=0 \
	-DOPTION_CANAPI_DEBUG_LEVEL=0 \
	-DOPTION_CANAPI_INSTRUMENTATION=0

HEADERS = -I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(MACCAN_DIR) \
	-I$(KVASER_DIR)

CFLAGS += -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LIBRARIES =

LDFLAGS  += -lpthread \
	-nostartfiles -dynamiclib -fvisibility=hidden \
	-Wl,-framework -Wl,IOKit -Wl,-framework -Wl,CoreFoundation \
	-install_name $(INSTALL_NAME) \
	-current_version $(CURRENT_VERSION) \
	-compatibility_version $(COMPATIBILITY_VERSION)

ARFLAGS = r

endif


CXX = g++
CC = gcc
LD = g++
AR = ar
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
	@cp $(SOURCE_DIR)/KvaserCAN.h $(INCDIR)
	@cp $(MACCAN_DIR)/MacCAN.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI_Types.h $(INCDIR)
	@cp $(CANAPI_DIR)/CANAPI_Defines.h $(INCDIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d
	$(RM) $(BINDIR)/$(TARGET) $(BINDIR)/$(STATIC) $(BINDIR)/$(LIBRARY).dylib

install:
	@echo "Copying library file..."
	$(CP) $(TARGET) $(INSTALL)
ifeq ($(current_OS),Darwin)
	$(RM) $(INSTALL)/$(LIBRARY).dylib ; $(LN) $(INSTALL)/$(TARGET) $(INSTALL)/$(LIBRARY).dylib
endif


$(OUTDIR)/KvaserCAN.o: $(SOURCE_DIR)/KvaserCAN.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/KvaserCAN_Driver.o: $(KVASER_DIR)/KvaserCAN_Driver.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/KvaserUSB_Device.o: $(KVASER_DIR)/KvaserUSB_Device.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/LeafLight.o: $(KVASER_DIR)/LeafLight.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN.o: $(MACCAN_DIR)/MacCAN.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_Debug.o: $(MACCAN_DIR)/MacCAN_Debug.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_Devices.o: $(MACCAN_DIR)/MacCAN_Devices.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_IOUsbKit.o: $(MACCAN_DIR)/MacCAN_IOUsbKit.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_MsgQueue.o: $(MACCAN_DIR)/MacCAN_MsgQueue.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/MacCAN_MsgPipe.o: $(MACCAN_DIR)/MacCAN_MsgPipe.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/can_btr.o: $(CANAPI_DIR)/can_btr.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<


$(STATIC): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS) $(LIBRARIES)
	$(CP) $(STATIC) $(BINDIR)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	$(CP) $(TARGET) $(BINDIR)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
