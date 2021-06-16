//
//  Please build the MacCAN artifacts first by calling `make` in the repo´s root.
//  You should have the following folders created:
//  - $(REPOROOT)/Binaries
//  - $(REPOROOT)/Includes
//
//  Note: Don´t forget to add `-lc++` to Other Linker Flags.
//
import Foundation

let can = KvaserCAN()
let channel: Int32 = 1
let mode: CanApi.Mode = [.DefaultOperationMode]
let baud: CanApi.CiaIndex = .Index250kbps
var step: String

var message = CanApi.Message(canId: 0x000, data: [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
let timeout: UInt16 = 1000
let frames = 2048

print("\(try CanApi.GetVersion())")
//for x in KvaserCanChannel.allCases {
//    let state = try KvaserCAN.ProbeChannel(channel: x.rawValue, mode: mode)
//    print("Channel \(x.rawValue): \(x.ChannelName()) -> (\(state))")
//}
do {
    step = "InitializeChannel)"
    print(">>> \(step)(channel: \(channel), mode: \(mode))")
    try can.InitializeChannel(channel: channel, mode: mode)
    step = "StartController"
    print(">>> \(step)(index: \(baud))")
    try can.StartController(index: baud)
} catch let e as CanApi.Error {
    print("+++ error: \(step) returned \(e) (\(e.description))")
    exit(1)
} catch {
    print("+++ error: \(step) returned \(error)")
    exit(1)
}
var running: sig_atomic_t = 1
signal(SIGINT) { signal in running = 0 }

print("!!! Be patient...")
for i in 0..<frames {
    do {
        message.canId = UInt32(i & 0x7FF)
        if i > 0x00 { message.data[0] = UInt8(i & 0xFF) }
        if i > 0xFF { message.data[1] = UInt8((i >> 8) & 0xFF) }
        if i > 0xFFFF { message.data[2] = UInt8((i >> 16) & 0xFF) }
        if i > 0xFFFFFF { message.data[3] = UInt8((i >> 24) & 0xFF) }
        step = "WriteMessage"
        try can.WriteMessage(message: message, timeout: timeout)
    } catch let e as CanApi.Error {
        print("+++ error: \(step) returned \(e) (\(e.description))")
        break
    } catch {
        print("+++ error: \(step) returned \(error)")
        exit(1)
    }
    if running == 0 {
        break;
    }
}
DispatchQueue.main.asyncAfter(deadline: .now() + 1) {
    // Put your code which should be executed with a delay here
}
if let statistics = can.statistics {
    print(">>> \(statistics.transmitted) frame(s) sent")
}
do {
    step = "ResetController"
    print(">>> \(step)()")
    try can.ResetController()
    step = "TeardownChannel"
    print(">>> \(step)()")
    try can.TeardownChannel()
} catch let e as CanApi.Error {
    print("+++ error: \(step) returned \(e) (\(e.description))")
    exit(1)
} catch {
    print("+++ error: \(step) returned \(error)")
    exit(1)
}
print("Bye!")
exit(0)
