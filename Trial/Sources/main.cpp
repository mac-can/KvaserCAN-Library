//
//  main.cpp
//  MacCAN-KvaserCAN
//  Bart Simpson didn´t do it
//
#include "KvaserCAN_Defines.h"
#include "KvaserCAN.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#if !defined(_WIN32) && !defined(_WIN64)
 #include <unistd.h>
#else
 #include <windows.h>
#endif

#include <inttypes.h>
#include "MacCAN_Debug.h"

//#define SECOND_CHANNEL
#define ISSUE_198   (0)

#define BITRATE_DEFAULT(x)  do{ x.btr.frequency=80000000; x.btr.nominal.brp=8; x.btr.nominal.tseg1= 31; x.btr.nominal.tseg2= 8; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1=15; x.btr.data.tseg2=4; x.btr.data.sjw=2; } while(0)
#define BITRATE_125K1M(x)   do{ x.btr.frequency=80000000; x.btr.nominal.brp=4; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1=31; x.btr.data.tseg2=8; x.btr.data.sjw=2; } while(0)
#define BITRATE_250K2M(x)   do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1=15; x.btr.data.tseg2=4; x.btr.data.sjw=2; } while(0)
#define BITRATE_500K4M(x)   do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 63; x.btr.nominal.tseg2=16; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1= 7; x.btr.data.tseg2=2; x.btr.data.sjw=2; } while(0)
#define BITRATE_1M8M(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 31; x.btr.nominal.tseg2= 8; x.btr.nominal.sjw= 8; x.btr.data.brp=2; x.btr.data.tseg1= 3; x.btr.data.tseg2=1; x.btr.data.sjw=1; } while(0)
#define BITRATE_125K(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=4; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; } while(0)
#define BITRATE_250K(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1=127; x.btr.nominal.tseg2=32; x.btr.nominal.sjw= 8; } while(0)
#define BITRATE_500K(x)     do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 63; x.btr.nominal.tseg2=16; x.btr.nominal.sjw= 8; } while(0)
#define BITRATE_1M(x)       do{ x.btr.frequency=80000000; x.btr.nominal.brp=2; x.btr.nominal.tseg1= 31; x.btr.nominal.tseg2= 8; x.btr.nominal.sjw= 8; } while(0)

#define OPTION_NO   (0)
#define OPTION_YES  (1)

#define OPTION_TIME_DRIVER  (0)
#define OPTION_TIME_ZERO    (1)
#define OPTION_TIME_ABS     (2)
#define OPTION_TIME_REL     (3)

#if defined(_WIN32) || defined(_WIN64)
 static void usleep(unsigned int usec);
#endif
static void sigterm(int signo);

static void verbose(const can_mode_t mode, const can_bitrate_t bitrate, const can_speed_t speed);

static volatile int running = 1;

static CKvaserCAN myDriver = CKvaserCAN();
#ifdef SECOND_CHANNEL
 static CKvaserCAN mySecond = CKvaserCAN();
#endif

int main(int argc, const char * argv[]) {
    MacCAN_OpMode_t opMode = {};
    opMode.byte = CANMODE_DEFAULT;
    MacCAN_Status_t status = {};
    status.byte = CANSTAT_RESET;
    MacCAN_Bitrate_t bitrate = {};
    bitrate.index = CANBTR_INDEX_250K;
    MacCAN_Message_t message = {};
    message.id = 0x55AU;
    message.xtd = 0;
    message.rtr = 0;
    message.dlc = CAN_MAX_DLC;
    message.data[0] = 0x11;
    message.data[1] = 0x22;
    message.data[2] = 0x33;
    message.data[3] = 0x44;
    message.data[4] = 0x55;
    message.data[5] = 0x66;
    message.data[6] = 0x77;
    message.data[7] = 0x88;
    message.timestamp.tv_sec = 0;
    message.timestamp.tv_nsec = 0;
    MacCAN_Return_t retVal = 0;
    int32_t channel = (int32_t)KVASER_CAN_CHANNEL0;
    uint16_t timeout = CANREAD_INFINITE;
    useconds_t delay = 0U;
    CMacCAN::EChannelState state;
    char szVal[CANPROP_MAX_BUFFER_SIZE];
    uint16_t u16Val;
    uint32_t u32Val;
    uint8_t u8Val;
    int32_t i32Val;
    int frames = 0;
    int option_info = OPTION_NO;
    int option_stat = OPTION_NO;
    int option_test = OPTION_NO;
    int option_exit = OPTION_NO;
    int option_echo = OPTION_YES;
    int option_stop = OPTION_NO;
    int option_check = ISSUE_198;
    int option_retry = OPTION_NO;
    int option_repeat = OPTION_NO;
    int option_transmit = OPTION_NO;
    uint64_t received = 0ULL;
    uint64_t expected = 0ULL;
    time_t now = time(NULL);

    for (int i = 1, opt = 0; i < argc; i++) {
        /* Kvaser CAN channel */
        if (!strcmp(argv[i], "Kvaser CAN Channel 0") || !strcmp(argv[i], "CH:0")) channel = (int32_t)KVASER_CAN_CHANNEL0;
        if (!strcmp(argv[i], "Kvaser CAN Channel 1") || !strcmp(argv[i], "CH:1")) channel = (int32_t)KVASER_CAN_CHANNEL1;
        if (!strcmp(argv[i], "Kvaser CAN Channel 2") || !strcmp(argv[i], "CH:2")) channel = (int32_t)KVASER_CAN_CHANNEL2;
        if (!strcmp(argv[i], "Kvaser CAN Channel 3") || !strcmp(argv[i], "CH:3")) channel = (int32_t)KVASER_CAN_CHANNEL3;
        if (!strcmp(argv[i], "Kvaser CAN Channel 4") || !strcmp(argv[i], "CH:4")) channel = (int32_t)KVASER_CAN_CHANNEL4;
        if (!strcmp(argv[i], "Kvaser CAN Channel 5") || !strcmp(argv[i], "CH:5")) channel = (int32_t)KVASER_CAN_CHANNEL5;
        if (!strcmp(argv[i], "Kvaser CAN Channel 6") || !strcmp(argv[i], "CH:6")) channel = (int32_t)KVASER_CAN_CHANNEL6;
        if (!strcmp(argv[i], "Kvaser CAN Channel 7") || !strcmp(argv[i], "CH:7")) channel = (int32_t)KVASER_CAN_CHANNEL7;
        /* baud rate (CAN 2.0) */
        if (!strcmp(argv[i], "BD:0") || !strcmp(argv[i], "BD:1000")) bitrate.index = CANBTR_INDEX_1M;
        if (!strcmp(argv[i], "BD:1") || !strcmp(argv[i], "BD:800")) bitrate.index = CANBTR_INDEX_800K;
        if (!strcmp(argv[i], "BD:2") || !strcmp(argv[i], "BD:500")) bitrate.index = CANBTR_INDEX_500K;
        if (!strcmp(argv[i], "BD:3") || !strcmp(argv[i], "BD:250")) bitrate.index = CANBTR_INDEX_250K;
        if (!strcmp(argv[i], "BD:4") || !strcmp(argv[i], "BD:125")) bitrate.index = CANBTR_INDEX_125K;
        if (!strcmp(argv[i], "BD:5") || !strcmp(argv[i], "BD:100")) bitrate.index = CANBTR_INDEX_100K;
        if (!strcmp(argv[i], "BD:6") || !strcmp(argv[i], "BD:50")) bitrate.index = CANBTR_INDEX_50K;
        if (!strcmp(argv[i], "BD:7") || !strcmp(argv[i], "BD:20")) bitrate.index = CANBTR_INDEX_20K;
        if (!strcmp(argv[i], "BD:8") || !strcmp(argv[i], "BD:10")) bitrate.index = CANBTR_INDEX_10K;
        /* CAN FD operation */
        if(!strcmp(argv[i], "CANFD") || !strcmp(argv[i], "FD")) opMode.fdoe = 1;
        if(!strcmp(argv[i], "FDF")) opMode.fdoe = 1;
        if(!strcmp(argv[i], "BRS")) opMode.brse = 1;
        /* bit rate (CAN FD) */
        if(!strcmp(argv[i], "BR:125K1M")) BITRATE_125K1M(bitrate);
        if(!strcmp(argv[i], "BR:250K2M")) BITRATE_250K2M(bitrate);
        if(!strcmp(argv[i], "BR:500K4M")) BITRATE_500K4M(bitrate);
        if(!strcmp(argv[i], "BR:1M8M")) BITRATE_1M8M(bitrate);
        if(!strcmp(argv[i], "BR:125K")) BITRATE_125K(bitrate);
        if(!strcmp(argv[i], "BR:250K")) BITRATE_250K(bitrate);
        if(!strcmp(argv[i], "BR:500K")) BITRATE_500K(bitrate);
        if(!strcmp(argv[i], "BR:1M")) BITRATE_1M(bitrate);
        /* asynchronous IO */
        if (!strcmp(argv[i], "POLLING")) timeout = 0U;
        if (!strcmp(argv[i], "BLOCKING")) timeout = CANREAD_INFINITE;
        /* transmit messages */
        if ((sscanf(argv[i], "%i", &opt) == 1) && (opt > 0)) option_transmit = opt;
        if (!strncmp(argv[i], "C:", 2) && sscanf(argv[i], "C:%i", &opt) == 1) delay = (useconds_t)opt * 1000U;
        if (!strncmp(argv[i], "U:", 2) && sscanf(argv[i], "U:%i", &opt) == 1) delay = (useconds_t)opt;
        /* receive messages */
        if (!strcmp(argv[i], "STOP")) option_stop = OPTION_YES;
#if (ISSUE_198 == 0)
        if (!strcmp(argv[i], "CHECK")) option_check = OPTION_YES;
#else
        if (!strcmp(argv[i], "IGNORE")) option_check = OPTION_NO;
#endif
        if (!strcmp(argv[i], "RETRY")) option_retry = OPTION_YES;
        if (!strcmp(argv[i], "REPEAT")) option_repeat = OPTION_YES;
        if (!strcmp(argv[i], "SILENT")) option_echo = OPTION_NO;
        /* time-stamps */
//        if (!strcmp(argv[i], "ZERO")) option_time = OPTION_TIME_ZERO;
//        if (!strcmp(argv[i], "ABS") || !strcmp(argv[i], "ABSOLUTE")) option_time = OPTION_TIME_ABS;
//        if (!strcmp(argv[i], "REL") || !strcmp(argv[i], "RELATIVE")) option_time = OPTION_TIME_REL;
        /* logging and debugging */
//        if (!strcmp(argv[i], "TRACE")) option_trace = OPTION_YES;
//        if (!strcmp(argv[i], "LOG")) option_log = OPTION_YES;
        /* query some informations: hw, sw, etc. */
        if (!strcmp(argv[i], "INFO")) option_info = OPTION_YES;
        if (!strcmp(argv[i], "STAT")) option_stat = OPTION_YES;
        if (!strcmp(argv[i], "TEST")) option_test = OPTION_YES;
        if (!strcmp(argv[i], "EXIT")) option_exit = OPTION_YES;
        /* additional operation modes (bit field) */
        if (!strcmp(argv[i], "SHARED")) opMode.shrd = 1;
        if (!strcmp(argv[i], "MONITOR")) opMode.mon = 1;
        if (!strcmp(argv[i], "MON:ON")) opMode.mon = 1;
        if (!strcmp(argv[i], "ERR:ON")) opMode.err = 1;
        if (!strcmp(argv[i], "XTD:OFF")) opMode.nxtd = 1;
        if (!strcmp(argv[i], "RTR:OFF")) opMode.nrtr = 1;
    }
    fprintf(stdout, ">>> %s\n", CMacCAN::GetVersion());
    fprintf(stdout, ">>> %s\n", CKvaserCAN::GetVersion());

    if((signal(SIGINT, sigterm) == SIG_ERR) ||
#if !defined(_WIN32) && !defined(_WIN64)
       (signal(SIGHUP, sigterm) == SIG_ERR) ||
#endif
       (signal(SIGTERM, sigterm) == SIG_ERR)) {
        perror("+++ error");
        return errno;
    }
    MACCAN_LOG_OPEN();
    MACCAN_LOG_PRINTF("# MacCAN-KvaserCAN - %s", ctime(&now));
    retVal = CMacCAN::Initializer();
    if (retVal != CMacCAN::NoError) {
        fprintf(stderr, "+++ error: CMacCAN::Initializer returned %i\n", retVal);
        return retVal;
    }
    if (option_info) {
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_CANAPI, (void *)&u16Val, sizeof(uint16_t));
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_CANAPI): value = %u.%u\n", (uint8_t)(u16Val >> 8), (uint8_t)u16Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_CANAPI) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_VERSION, (void *)&u16Val, sizeof(uint16_t));
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_VERSION): value = %u.%u\n", (uint8_t)(u16Val >> 8), (uint8_t)u16Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_VERSION) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_PATCH_NO, (void *)&u8Val, sizeof(uint8_t));
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_PATCH_NO): value = %u\n", (uint8_t)u8Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_PATCH_NO) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_BUILD_NO, (void *)&u32Val, sizeof(uint32_t));
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_BUILD_NO): value = 0x%" PRIx32 "\n", (uint32_t)u32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_BUILD_NO) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_ID, (void *)&i32Val, sizeof(int32_t));
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_ID): value = %d\n", i32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_ID) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_NAME, (void *)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_NAME): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_NAME) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_VENDOR, (void *)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_VENDOR): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_LIBRARY_VENDOR) returned %i\n", retVal);
        if (option_exit && !option_test)
            return 0;
    }
    if (option_test) {
        for (int32_t ch = 0; ch < 8; ch++) {
            retVal = CKvaserCAN::ProbeChannel(ch, opMode, state);
            fprintf(stdout, ">>> myDriver.ProbeChannel(%i): state = %s", ch,
                            (state == CMacCAN::ChannelOccupied) ? "occupied" :
                            (state == CMacCAN::ChannelAvailable) ? "available" :
                            (state == CMacCAN::ChannelNotAvailable) ? "not available" : "not testable");
            fprintf(stdout, "%s", (retVal == CMacCAN::IllegalParameter) ? " (waring: Op.-Mode not supported)\n" : "\n");
        }
        if (option_exit)
            return 0;
    }
    retVal = myDriver.InitializeChannel(channel, opMode);
    if (retVal != CMacCAN::NoError) {
        fprintf(stderr, "+++ error: myDriver.InitializeChannel(%i) returned %i\n", channel, retVal);
        goto end;
    }
    else if (myDriver.GetStatus(status) == CMacCAN::NoError) {
        fprintf(stdout, ">>> myDriver.InitializeChannel(%i): status = 0x%02X\n", channel, status.byte);
    }
    if (option_test) {
        retVal = myDriver.ProbeChannel(channel, opMode, state);
        fprintf(stdout, ">>> myDriver.ProbeChannel(%i): state = %s", channel,
                        (state == CMacCAN::ChannelOccupied) ? "now occupied" :
                        (state == CMacCAN::ChannelAvailable) ? "available" :
                        (state == CMacCAN::ChannelNotAvailable) ? "not available" : "not testable");
        fprintf(stdout, "%s", (retVal == CMacCAN::IllegalParameter) ? " (waring: Op.-Mode not supported)\n" : "\n");
    }
    if (option_info) {
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_TYPE, (void *)&i32Val, sizeof(int32_t));
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_TYPE): value = %d\n", i32Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_TYPE) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_NAME, (void *)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_NAME): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_NAME) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_DRIVER, (void *)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_DRIVER): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_DRIVER) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_VENDOR, (void *)szVal, CANPROP_MAX_BUFFER_SIZE);
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_VENDOR): value = '%s'\n", szVal);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_DEVICE_VENDOR) returned %i\n", retVal);
        // vendor-specific properties
//        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_CLOCK_DOMAIN, (void *)&i32Val, sizeof(int32_t));
//        if (retVal == CMacCAN::NoError)
//            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_CLOCK_DOMAIN): value = %d\n", i32Val);
//        else
//            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_CLOCK_DOMAIN) returned %i\n", retVal);
        retVal = myDriver.GetProperty(KVASERCAN_PROPERTY_OP_CAPABILITY, (void *)&u8Val, sizeof(uint8_t));
        if (retVal == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_OP_CAPABILITY): value = 0x%02X\n", (uint8_t)u8Val);
        else
            fprintf(stderr, "+++ error: myDriver.GetProperty(KVASERCAN_PROPERTY_OP_CAPABILITY) returned %i\n", retVal);
        if (myDriver.GetProperty(KVASERCAN_PROPERTY_OP_MODE, (void *)&opMode.byte, sizeof(uint8_t)) == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_OP_MODE): value = 0x%02X\n", (uint8_t)opMode.byte);
        if (myDriver.GetProperty(KVASERCAN_PROPERTY_STATUS, (void *)&status.byte, sizeof(uint8_t)) == CMacCAN::NoError)
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_STATUS): value = 0x%02X\n", (uint8_t)status.byte);
    }
    retVal = myDriver.StartController(bitrate);
    if (retVal != CMacCAN::NoError) {
        fprintf(stderr, "+++ error: myDriver.StartController returned %i\n", retVal);
        goto teardown;
    }
    else if (myDriver.GetStatus(status) == CMacCAN::NoError) {
        fprintf(stdout, ">>> myDriver.StartController: status = 0x%02X\n", status.byte);
    }
    if (option_info) {
        MacCAN_BusSpeed_t speed;
        if ((myDriver.GetBitrate(bitrate) == CMacCAN::NoError) &&
            (myDriver.GetBusSpeed(speed) == CMacCAN::NoError))
            verbose(opMode, bitrate, speed);
    }
#ifdef SECOND_CHANNEL
    retVal = mySecond.InitializeChannel(channel+1U, opMode);
    if (retVal != CMacCAN::NoError)
        fprintf(stderr, "+++ error: mySecond.InitializeChannel(%i) returned %i\n", channel+1U, retVal);
    retVal = mySecond.StartController(bitrate);
    if (retVal != CMacCAN::NoError)
        fprintf(stderr, "+++ error: mySecond.StartController returned %i\n", retVal);
    retVal = mySecond.WriteMessage(message);
    if (retVal != CMacCAN::NoError)
        fprintf(stderr, "+++ error: mySecond.WriteMessage returned %i\n", retVal);
#endif
    if ((option_transmit > 0) && !option_retry)
        fprintf(stdout, "Attention: The program will be aborted when the transmitter is busy.\n"
                        "           Use progrsm option RETRY to avoid this.\n");
    fprintf(stdout, "Press Ctrl+C to abort...\n");
    while (running && (option_transmit-- > 0)) {
        if (!opMode.fdoe) {
            message.fdf = 0;
            message.brs = 0;
            message.dlc = CAN_MAX_DLC;
        } else {
            message.fdf = opMode.fdoe;
            message.brs = opMode.brse;
            message.dlc = CANFD_MAX_DLC;
        }
retry:
        retVal = myDriver.WriteMessage(message);
        if ((retVal == CMacCAN::TransmitterBusy) && option_retry)
            goto retry;
        else if (retVal != CMacCAN::NoError) {
            fprintf(stderr, "+++ error: myDriver.WriteMessage returned %i\n", retVal);
            goto teardown;
        }
        if (delay)
            usleep(delay);
    }
    while (running) {
        if ((retVal = myDriver.ReadMessage(message, timeout)) == CMacCAN::NoError) {
            if (option_echo) {
                fprintf(stdout, ">>> %i\t", frames++);
                fprintf(stdout, "%7li.%04li\t", (long)message.timestamp.tv_sec, message.timestamp.tv_nsec / 100000);
                if (!opMode.fdoe)
                    fprintf(stdout, "%03x\t%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ', message.dlc);
                else
                    fprintf(stdout, "%03x\t%c%c%c%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ',
                            message.fdf ? 'F' : ' ', message.brs ? 'B' : ' ', message.esi ? 'E' :' ', CMacCAN::Dlc2Len(message.dlc));
                for (uint8_t i = 0; i < CMacCAN::Dlc2Len(message.dlc); i++)
                    fprintf(stdout, " %02x", message.data[i]);
                if (message.sts)
                    fprintf(stdout, " <<< status frame");
                else if (option_repeat) {
                    retVal = myDriver.WriteMessage(message);
                    if (retVal != CMacCAN::NoError) {
                        fprintf(stderr, "+++ error: myDriver.WriteMessage returned %i\n", retVal);
                        goto teardown;
                    }
                }
                fprintf(stdout, "\n");
            } else {
                if(!(frames++ % 2048)) {
                    fprintf(stdout, ".");
                    fflush(stdout);
                }
            }
            if (option_check && !message.sts) {
                received = 0;
                if(message.dlc > 0) received |= (uint64_t)message.data[0] << 0;
                if(message.dlc > 1) received |= (uint64_t)message.data[1] << 8;
                if(message.dlc > 2) received |= (uint64_t)message.data[2] << 16;
                if(message.dlc > 3) received |= (uint64_t)message.data[3] << 24;
                if(message.dlc > 4) received |= (uint64_t)message.data[4] << 32;
                if(message.dlc > 5) received |= (uint64_t)message.data[5] << 40;
                if(message.dlc > 6) received |= (uint64_t)message.data[6] << 48;
                if(message.dlc > 7) received |= (uint64_t)message.data[7] << 56;
                if(received != expected) {
                    fprintf(stderr, "+++ error: received data is not equal to expected data (%" PRIu64 " : %" PRIu64 ")\n", received, expected);
                    if(expected > received)
                        fprintf(stderr, "           issue #198: old messages read again (offset -%" PRIu64 ")\a\n", expected - received);
                    if(option_stop)
                        goto teardown;
                }
                expected = received + 1;
            }
        }
        else if (retVal != CMacCAN::ReceiverEmpty) {
            goto teardown;
        }
#ifdef SECOND_CHANNEL
        if ((retVal = mySecond.ReadMessage(message, 0U)) == CMacCAN::NoError) {
            if (option_echo) {
                fprintf(stdout, ">2> %i\t", frames++);
                fprintf(stdout, "%7li.%04li\t", (long)message.timestamp.tv_sec, message.timestamp.tv_nsec / 100000);
                if (!opMode.fdoe)
                    fprintf(stdout, "%03x\t%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ', message.dlc);
                else
                    fprintf(stdout, "%03x\t%c%c%c%c%c [%i]", message.id, message.xtd ? 'X' : 'S', message.rtr ? 'R' : ' ',
                        message.fdf ? 'F' : ' ', message.brs ? 'B' : ' ', message.esi ? 'E' : ' ', CMacCAN::Dlc2Len(message.dlc));
                for (uint8_t i = 0; i < CMacCAN::Dlc2Len(message.dlc); i++)
                    fprintf(stdout, " %02x", message.data[i]);
                if (message.sts)
                    fprintf(stdout, " <<< status frame");
                else if (option_repeat) {
                    retVal = myDriver.WriteMessage(message);
                    if (retVal != CMacCAN::NoError) {
                        fprintf(stderr, "+++ error: mySecond.WriteMessage returned %i\n", retVal);
                        goto teardown;
                    }
                }
                fprintf(stdout, "\n");
            }
            else {
                if (!(frames++ % 2048)) {
                    fprintf(stdout, ".");
                    fflush(stdout);
                }
            }
        }
        else if (retVal != CMacCAN::ReceiverEmpty) {
            fprintf(stderr, "+++ error: mySecond.ReadMessage returned %i\n", retVal);
            goto teardown;
        }
#endif
    }
    if (myDriver.GetStatus(status) == CMacCAN::NoError) {
        fprintf(stdout, "\n>>> myDriver.ReadMessage: status = 0x%02X\n", status.byte);
    }
    if (option_stat || option_info) {
        uint64_t u64TxCnt, u64RxCnt, u64ErrCnt;
        if ((myDriver.GetProperty(KVASERCAN_PROPERTY_TX_COUNTER, (void *)&u64TxCnt, sizeof(uint64_t)) == CMacCAN::NoError) &&
            (myDriver.GetProperty(KVASERCAN_PROPERTY_RX_COUNTER, (void *)&u64RxCnt, sizeof(uint64_t)) == CMacCAN::NoError) &&
            (myDriver.GetProperty(KVASERCAN_PROPERTY_ERR_COUNTER, (void *)&u64ErrCnt, sizeof(uint64_t)) == CMacCAN::NoError))
            fprintf(stdout, ">>> myDriver.GetProperty(KVASERCAN_PROPERTY_*_COUNTER): TX = %" PRIi64 " RX = %" PRIi64 " ERR = %" PRIi64 "\n", u64TxCnt, u64RxCnt, u64ErrCnt);
    }
    if (option_info) {
        char *hardware = myDriver.GetHardwareVersion();
        if (hardware)
            fprintf(stdout, ">>> myDriver.GetHardwareVersion: '%s'\n", hardware);
        char *firmware = myDriver.GetFirmwareVersion();
        if (firmware)
            fprintf(stdout, ">>> myDriver.GetFirmwareVersion: '%s'\n", firmware);
    }
teardown:
#ifdef SECOND_CHANNEL
    retVal = mySecond.ResetController();
    if (retVal != CMacCAN::NoError)
        fprintf(stderr, "+++ error: mySecond.ResetController returned %i\n", retVal);
    retVal = mySecond.TeardownChannel();
    if (retVal != CMacCAN::NoError)
        fprintf(stderr, "+++ error: mySecond.TeardownChannel returned %i\n", retVal);
#endif
    retVal = myDriver.TeardownChannel();
    if (retVal != CMacCAN::NoError) {
        fprintf(stderr, "+++ error: myDriver.TeardownChannel returned %i\n", retVal);
        goto end;
    }
    else if (myDriver.GetStatus(status) == CMacCAN::NoError) {
        fprintf(stdout, ">>> myDriver.TeardownChannel: status = 0x%02X\n", status.byte);
    }
    else {
        fprintf(stdout, "@@@ Resistance is futile!\n");
    }
end:
    retVal = CMacCAN::Finalizer();
    if (retVal != CMacCAN::NoError) {
        fprintf(stderr, "+++ error: CMacCAN::Finalizer returned %i\n", retVal);
        return retVal;
    }
    now = time(NULL);
    MACCAN_LOG_PRINTF("# MacCAN-KvaserCAN - %s", ctime(&now));
    MACCAN_LOG_CLOSE();
    fprintf(stdout, "Cheers!\n");
    return retVal;
}

static void verbose(const can_mode_t mode, const can_bitrate_t bitrate, const can_speed_t speed)
{
    fprintf(stdout, "Op.-Mode: 0x%02X (fdoe=%u,brse=%u,niso=%u,shrd=%u,nxtd=%u,nrtr=%u,err=%u,mon=%u)\n",
            mode.byte, mode.fdoe, mode.brse, mode.niso, mode.shrd, mode.nxtd, mode.nrtr, mode.err, mode.mon);
    if(bitrate.btr.frequency > 0) {
        fprintf(stdout, "Baudrate: %.0fkbps@%.1f%%",
            speed.nominal.speed / 1000., speed.nominal.samplepoint * 100.);
        if(/*speed.data.brse*/mode.fdoe && mode.brse)
            fprintf(stdout, ":%.0fkbps@%.1f%%",
                speed.data.speed / 1000., speed.data.samplepoint * 100.);
        fprintf(stdout, " (f_clock=%i,nom_brp=%u,nom_tseg1=%u,nom_tseg2=%u,nom_sjw=%u,nom_sam=%u",
            bitrate.btr.frequency,
            bitrate.btr.nominal.brp,
            bitrate.btr.nominal.tseg1,
            bitrate.btr.nominal.tseg2,
            bitrate.btr.nominal.sjw,
            bitrate.btr.nominal.sam);
        if(mode.fdoe && mode.brse)
            fprintf(stdout, ",data_brp=%u,data_tseg1=%u,data_tseg2=%u,data_sjw=%u",
                bitrate.btr.data.brp,
                bitrate.btr.data.tseg1,
                bitrate.btr.data.tseg2,
                bitrate.btr.data.sjw);
        fprintf(stdout, ")\n");
    }
    else {
        fprintf(stdout, "Baudrate: %skbps (CiA index %i)\n",
            bitrate.index == CANBDR_1000 ? "1000" :
            bitrate.index == -CANBDR_800 ? "800" :
            bitrate.index == -CANBDR_500 ? "500" :
            bitrate.index == -CANBDR_250 ? "250" :
            bitrate.index == -CANBDR_125 ? "125" :
            bitrate.index == -CANBDR_100 ? "100" :
            bitrate.index == -CANBDR_50 ? "50" :
            bitrate.index == -CANBDR_20 ? "20" :
            bitrate.index == -CANBDR_10 ? "10" : "?", -bitrate.index);
    }
}

static void sigterm(int signo) {
    //fprintf(stderr, "%s: got signal %d\n", __FILE__, signo);
    (void)myDriver.SignalChannel();
#ifdef SECOND_CHANNEL
    (void)mySecond.SignalChannel();
#endif
    running = 0;
    (void)signo;
}
