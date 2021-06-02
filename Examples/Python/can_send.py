#!/usr/bin/env python3
from CANAPI import *
import argparse
import signal
import sys
from time import sleep


# ^C handler to abort the reception loop and exit
#
def sigterm(signo, frame):
    print()
#    print('>>> can.kill()')
#    result = can.kill()
#    if result < 0:
#        print('+++ error: can.kill returned {}'.format(result))
    print('>>> can.exit()')
    result = can.exit()
    if result < 0:
        sys.exit('+++ error: can.exit returned {}'.format(result))
    else:
        sys.exit(0)


# CAN API V3 driver library
lib = 'libUVCANKVL.dylib'
chn = 1
num = 1 + CAN_MAX_STD_ID

# parse the command line
parser = argparse.ArgumentParser()
parser.add_argument('input', type=str, nargs='?', default=lib,
                    help='CAN API V3 driver library, default=\'' + lib + '\'')
parser.add_argument('--channel', type=int, nargs=1, default=[chn],
                    help='CAN interface (channel), default=' + str(chn))
parser.add_argument('--frames', type=int, nargs=1, default=[num],
                    help='number of CAN frames to be sent, default=' + str(num))
args = parser.parse_args()
lib = args.input
chn = args.channel[0]
num = args.frames[0]
opMode = OpMode()
opMode.byte = CANMODE_DEFAULT
bitRate = Bitrate()
bitRate.index = CANBTR_INDEX_250K
message = Message()
message.id = 0x100
message.xtd = 0
message.rtr = 0
message.fdf = 0
message.brs = 0
message.sts = 0
message.dlc = 8

# install ^C handler
signal.signal(signal.SIGTERM, sigterm)
signal.signal(signal.SIGINT, sigterm)

# load the driver library
print(CANAPI.version())
print('>>> can = CANAPI(' + lib + ')')
can = CANAPI(lib)

# initialize the CAN interface
print('>>> can.init({}, 0x{:02X})'.format(chn, opMode.byte))
res = can.init(channel=chn, mode=opMode)
if res < CANERR_NOERROR:
    sys.exit('+++ error: can.init returned {}'.format(res))
res, status = can.status()
if res < CANERR_NOERROR:
    print('+++ error: can.status returned {}'.format(res))
else:
    print('>>> can.status() >>> 0x{:02X}'.format(status.byte))

# start the CAN controller
if int(bitRate.index) > 0:   # FIXME: Expected type 'int', got 'c_int32[int]' instead
    print('>>> can.start([{},[{},{},{},{},{}],[{},{},{},{},])'.format(bitRate.btr.frequency,
                                                                      bitRate.btr.nominal.brp,
                                                                      bitRate.btr.nominal.tseg1,
                                                                      bitRate.btr.nominal.tseg2,
                                                                      bitRate.btr.nominal.sjw,
                                                                      bitRate.btr.nominal.sam,
                                                                      bitRate.btr.data.brp,
                                                                      bitRate.btr.data.tseg1,
                                                                      bitRate.btr.data.tseg2,
                                                                      bitRate.btr.data.sjw))
else:
    print('>>> can.start([{}])'.format(bitRate.index))
res = can.start(bitrate=bitRate)
if res < CANERR_NOERROR:
    can.exit()
    sys.exit('+++ error: can.start returned {}'.format(res))
res, status = can.status()
if res < CANERR_NOERROR:
    print('+++ error: can.status returned {}'.format(res))
else:
    print('>>> can.status() >>> 0x{:02X}'.format(status.byte))

# send a number of CAN frames
print('>>> can.write() * {}...'.format(num))
for i in range(num):
    message.id = i & CAN_MAX_STD_ID
    data = [int(i >> x & 0xFF) for x in (0, 8, 16, 24, 32, 40, 48, 56)]
    for j in range(len(data)):
        message.data[j] = data[j]
    message.dlc = 8
    while True:
        res = can.write(message)
        if res != CANERR_TX_BUSY:
            break
    if res < CANERR_NOERROR:
        print('+++ error: can.write returned {}'.format(res))
        break
sleep(1)  # afterburner
res, status = can.status()
if res < CANERR_NOERROR:
    print('+++ error: can.status returned {}'.format(res))
else:
    print('>>> can.status() >>> 0x{:02X}'.format(status.byte))

# stop the CAN communication
print('>>> can.reset()')
res = can.reset()
if res < CANERR_NOERROR:
    print('+++ error: can.reset returned {}'.format(res))
res, status = can.status()
if res < CANERR_NOERROR:
    print('+++ error: can.status returned {}'.format(res))
else:
    print('>>> can.status() >>> 0x{:02X}'.format(status.byte))

# shutdown the CAN interface
print('>>> can.exit()')
res = can.exit()
if res < CANERR_NOERROR:
    print('+++ error: can.exit returned {}'.format(res))

# have a great time
print('Bye, bye!')
