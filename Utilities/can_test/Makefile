#
#	CAN Tester for Kvaser CAN Interfaces (CAN API V3)
#
#	Copyright (c) 2008-2010,2014-2023  Uwe Vogt, UV Software, Berlin (info@mac-can.com)
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program   If not, see <http://www.gnu.org/licenses/>.
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))

PROJ_DIR = ../..
HOME_DIR = .
MAIN_DIR = ./Sources

DRIVER_DIR = $(PROJ_DIR)/Sources
CANAPI_DIR = $(PROJ_DIR)/Sources/CANAPI

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/Timer.o \
	$(BINDIR)/libKvaserCAN.a


ifeq ($(current_OS),Darwin) # macOS - libKvaserCAN.dylib

VERSION = 0.3.3

TARGET  = can_test

INSTALL = ~/bin

DEFINES = -DOPTION_CANAPI_DRIVER=1 \
	-DOPTION_CANAPI_COMPANIONS=1

HEADERS = -I$(MAIN_DIR) \
	-I$(HOME_DIR) \
	-I$(DRIVER_DIR) \
	-I$(CANAPI_DIR)

CFLAGS += -O2 -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O2 -g -Wall -Wextra -pthread \
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

CXX = clang++
CC = clang
LD = clang++
endif

RM = rm -f
CP = cp -f

OUTDIR = .objects
BINDIR = $(PROJ_DIR)/Binaries
INCDIR = $(PROJ_DIR)/Includes

.PHONY: info outdir bindir incdir


all: info outdir bindir incdir $(TARGET)

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

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d
	$(RM) $(BINDIR)/$(TARGET)

install:
	@echo "Copying binary file..."
	$(CP) $(TARGET) $(INSTALL)


$(OUTDIR)/main.o: $(MAIN_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/Timer.o: $(MAIN_DIR)/Timer.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	$(CP) $(TARGET) $(BINDIR)
	@lipo -archs $@
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
