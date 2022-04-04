//
//  MacCAN-KvaserCAN - Swift example for receiving CAN messages
//
//  Please build the MacCAN artifacts first by calling `make` in the repo´s root.
//  You should have the following folders created:
//  - $(REPOROOT)/Binaries
//  - $(REPOROOT)/Includes
//
//  Note: Don´t forget to add `-lc++` to Other Linker Flags.
//
import Foundation

let can = CanApi()
let channel: Int32 = 0
let mode: CanApi.Mode = [.DefaultOperationMode]
let baud: CanApi.CiaIndex = .Index250kbps
var step: String
var n = 0

print("\(try CanApi.GetVersion())")
if let info = try? CanApi.GetFirstChannel() {
    print(">>> channel: \(info.channel), name: \(info.name), vendor: \(info.vendor), library: \(info.library), driver: \(info.driver)")
    while let info = try? CanApi.GetNextChannel() {
        print(">>> channel: \(info.channel), name: \(info.name), vendor: \(info.vendor), library: \(info.library), driver: \(info.driver)")
    }
}
do {
    step = "InitializeChannel"
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

print("Press ^C to abort...")
while running != 0 {
    do {
        step = "ReadMessage"
        if let message = try can.ReadMessage() {
            print(">>> \(n): Message(id: \(message.canId), dlc: \(message.canDlc), data: \(message.data))")
        }
    } catch let e as CanApi.Error {
        print("+++ error: \(step) returned \(e) (\(e.description))")
        break
    } catch {
        print("+++ error: \(step) returned \(error)")
        exit(1)
    }
    n += 1
}
print("")
if let status = can.status {
    print(">>> status: BO=\(status.isBusOff) EW=\(status.isWarningLevel) BE=\(status.isBusError) TP=\(status.isTransmitterBusy) RE=\(status.isReceiverEmpty) ML=\(status.isMessageLost) QO=\(status.isQueueOverrun)")
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
