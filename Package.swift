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
            path: "Sources/KvaserCAN_Swift"
        ),
        .target(
            name: "CKvaserCAN",
            dependencies: [],
            path: "Sources",
            exclude: [
                "KvaserCAN_Swift/CANAPI.swift",
                "KvaserCAN_Swift/KvaserCAN.swift",
                "MacCAN_Core/README.md",
                "MacCAN_Core/LICENSE.BSD-2-Clause",
                "MacCAN_Core/LICENSE.GPL-3.0-or-later",
                "CANAPI/README.md",
                "CANAPI/LICENSE.BSD-2-Clause",
                "CANAPI/LICENSE.GPL-3.0-or-later",
                "include/README.md"
            ],
            sources: [
                "KvaserCAN.cpp",
                "KvaserCAN_Driver/KvaserCAN_Driver.c",
                "KvaserCAN_Driver/KvaserUSB_Device.c",
                "KvaserCAN_Driver/LeafLight.c",
                "KvaserCAN_Driver/LeafPro.c",
                "KvaserCAN_Wrapper/can_api.cpp",
                "MacCAN_Core/MacCAN.cpp",
                "MacCAN_Core/MacCAN_MsgPipe.c",
                "MacCAN_Core/MacCAN_MsgQueue.c",
                "MacCAN_Core/MacCAN_IOUsbKit.c",
                "MacCAN_Core/MacCAN_Devices.c",
                "MacCAN_Core/MacCAN_Debug.c",
                "CANAPI/can_btr.c",
                "CANAPI/can_msg.c"
            ],
            cSettings: [
                CSetting.headerSearchPath("."),
                CSetting.headerSearchPath("./CANAPI"),
                CSetting.headerSearchPath("./MacCAN_Core"),
                CSetting.headerSearchPath("./KvaserCAN_Driver"),
                CSetting.headerSearchPath("./KvaserCAN_Wrapper"),
                CSetting.define("OPTION_CAN_2_0_ONLY=0"),
                CSetting.define("OPTION_KVASERCAN_DYLIB=1"),
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
