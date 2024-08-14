#
#	Trial Program
#	MacCAN-KvaserCAN
#	Bart Simpson didn't do it
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))


TARGET	= kvl_test

INSTALL = ~/bin

PROJ_DIR = ..
HOME_DIR = .
MAIN_DIR = ./Sources

SOURCE_DIR = $(PROJ_DIR)/Sources
CANAPI_DIR = $(PROJ_DIR)/Sources/CANAPI
MACCAN_DIR = $(PROJ_DIR)/Sources/MacCAN
DRIVER_DIR = $(PROJ_DIR)/Sources/Driver
WRAPPER_DIR = $(PROJ_DIR)/Sources/Wrapper

OBJECTS = $(OUTDIR)/KvaserCAN_Driver.o $(OUTDIR)/KvaserCAN_Devices.o \
	$(OUTDIR)/KvaserUSB_LeafDevice.o $(OUTDIR)/KvaserUSB_MhydraDevice.o \
	$(OUTDIR)/KvaserUSB_Device.o \
	$(OUTDIR)/MacCAN_Devices.o $(OUTDIR)/MacCAN_IOUsbKit.o \
	$(OUTDIR)/MacCAN_MsgQueue.o $(OUTDIR)/MacCAN_MsgPipe.o \
	$(OUTDIR)/MacCAN_Internal.o $(OUTDIR)/MacCAN_Debug.o \
	$(OUTDIR)/can_api.o $(OUTDIR)/can_btr.o \
	$(OUTDIR)/KvaserCAN.o $(OUTDIR)/main.o

DEFINES = -DOPTION_CAN_2_0_ONLY=0 \
	-DOPTION_CANAPI_DRIVER=1 \
	-DOPTION_CANAPI_RETVALS=0 \
	-DOPTION_CANAPI_COMPANIONS=1 \
	-DOPTION_MACCAN_PIPE_INFO=0 \
	-DOPTION_MACCAN_PIPE_TIMEOUT=1 \
	-DOPTION_MACCAN_MULTICHANNEL=0 \
	-DOPTION_MACCAN_USB_STRINGS=0 \
	-DOPTION_MACCAN_LOGGER=1 \
	-DOPTION_MACCAN_DEBUG_LEVEL=4 \
	-DOPTION_MACCAN_INSTRUMENTATION=0 \
	-DOPTION_CANAPI_DEBUG_LEVEL=0 \
	-DOPTION_CANAPI_INSTRUMENTATION=0

HEADERS = -I$(SOURCE_DIR) \
	-I$(CANAPI_DIR) \
	-I$(MACCAN_DIR) \
	-I$(DRIVER_DIR) \
	-I$(WRAPPER_DIR) \
	-I$(MAIN_DIR)


ifeq ($(current_OS),Darwin)  # macOS - libKvaserCAN.dylib

CFLAGS += -O0 -g -Wall -Wextra -Wno-parentheses \
	-fmessage-length=0 -fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O0 -g -Wall -Wextra -pthread \
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

CHECKER  = warning,information
IGNORE   = -i can_msg.c -i can_dev.c -i vanilla.c -i main.c
ifeq ($(HUNTER),BUGS)
CHECKER += --bug-hunting
endif

ifeq ($(XCTEST),FULL)
TESTING =
else
TESTING = -only-testing Testing/Testing/testSmokeTest
endif

CXX = clang++
CC = clang
LD = clang++
endif

RM = rm -f
CP = cp -f

OUTDIR = .objects


.PHONY: info outdir


all: info outdir $(TARGET)

info:
	@echo $(CXX)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

check:
	cppcheck --enable=$(CHECKER) --suppressions-list=$(PROJ_DIR)/suppress.txt \
	-D__APPLE__ $(DEFINES) $(HEADERS) $(IGNORE) $(SOURCE_DIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

install:
	$(CP) $(TARGET) $(INSTALL)

test: info outdir $(TARGET)
	./$(TARGET) INFO EXIT

xctest:
	xcodebuild clean build test -project KvaserCAN.xcodeproj -scheme Testing $(TESTING)


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

$(OUTDIR)/main.o: $(MAIN_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
ifeq ($(current_OS),Darwin)
	@lipo -archs $@
endif
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
