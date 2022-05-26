// swift-tools-version:5.3
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "MacCAN-KvaserCAN",
    platforms: [.macOS(.v11)],
    products: [
        // Products define the executables and libraries a package produces, and make them visible to other packages.
        .library(
            name: "MacCAN-KvaserCAN",
            targets: [
                "KvaserCAN",
                "CKvaserCAN"]
        ),
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        // .package(url: /* package url */, from: "1.0.0"),
    ],
    targets: [
        // Targets are the basic building blocks of a package. A target can define a module or a test suite.
        // Targets can depend on other targets in this package, and on products in packages this package depends on.
        .target(
            name: "KvaserCAN",
            dependencies: ["CKvaserCAN"],
            path: "Sources/Swift"
        ),
        .target(
            name: "CKvaserCAN",
            dependencies: [],
            path: "Sources",
            exclude: [
                "KvaserCAN.cpp",
                "MacCAN/README.md",
                "MacCAN/LICENSE.BSD-2-Clause",
                "MacCAN/LICENSE.GPL-3.0-or-later",
                "CANAPI/README.md",
                "CANAPI/LICENSE.BSD-2-Clause",
                "CANAPI/LICENSE.GPL-3.0-or-later",
                "Swift/CANAPI.swift",
                "Swift/KvaserCAN.swift",
                "Kvaser/*.*",
                "include/README.md"
            ],
            sources: [
                "Driver/KvaserCAN_Driver.c",
                "Driver/KvaserUSB_Device.c",
                "Driver/KvaserUSB_LeafDevice.c",
                "Driver/KvaserUSB_MhydraDevice.c",
                "Driver/KvaserCAN_Devices.c",
                "Wrapper/can_api.c",
                "MacCAN/MacCAN_MsgPipe.c",
                "MacCAN/MacCAN_MsgQueue.c",
                "MacCAN/MacCAN_IOUsbKit.c",
                "MacCAN/MacCAN_Devices.c",
                "MacCAN/MacCAN_Debug.c",
                "CANAPI/can_btr.c",
                "CANAPI/can_msg.c"
            ],
            cSettings: [
                CSetting.headerSearchPath("."),
                CSetting.headerSearchPath("./CANAPI"),
                CSetting.headerSearchPath("./MacCAN"),
                CSetting.headerSearchPath("./Driver"),
                CSetting.headerSearchPath("./Wrapper"),
                CSetting.define("OPTION_CAN_2_0_ONLY=0"),
                CSetting.define("OPTION_KVASERCAN_DYLIB=0"),
                CSetting.define("OPTION_CANAPI_KVASERCAN_DYLIB=0"),
                CSetting.define("OPTION_CANAPI_DRIVER=1"),
                CSetting.define("OPTION_CANAPI_RETVALS=1"),
                CSetting.define("OPTION_CANAPI_COMPANIONS=1"),
                CSetting.define("OPTION_MACCAN_LOGGER=0"),
                CSetting.define("OPTION_MACCAN_MULTICHANNEL=0"),
                CSetting.define("OPTION_MACCAN_PIPE_TIMEOUT=1"),
                CSetting.define("OPTION_MACCAN_DEBUG_LEVEL=0"),
                CSetting.define("OPTION_MACCAN_INSTRUMENTATION=0"),
                CSetting.define("OPTION_CANAPI_DEBUG_LEVEL=0"),
                CSetting.define("OPTION_CANAPI_INSTRUMENTATION=0"),
                CSetting.headerSearchPath("./include"),  // dummy build no. for SPM
            ]
        ),
        .testTarget(
            name: "SwiftTests",
            dependencies: ["KvaserCAN"]
        ),
    ],
    cxxLanguageStandard: .cxx14
)
